//
//  AliCloudFileSystemImpl.cpp
//
//  Created by mengxk on 20/03/06.
//  Copyright © 2016 mengxk. All rights reserved.
//

#include <AliCloudFileSystemImpl.hpp>
#include <ErrCode.hpp>
#include <Log.hpp>

#include <alibabacloud/oss/OssClient.h>

namespace AliOss = AlibabaCloud::OSS;

#define CHECK_ALIOSS_ECODE(aliOssRet) {                                     \
    int ret = transAliOssErrCode(aliOssRet.isSuccess(), aliOssRet.error()); \
    CHECK_ERRCODE(ret);                                                     \
}

namespace elastos {
namespace sdk {

struct AliOssFile: public CloudFileSystem::File {
    std::shared_ptr<std::iostream> mPartUploadCache;
    int mPartUploadCacheSize = 0;
    std::string mPartUploadToken;
    int mPartUploadNumber = 0;
    AliOss::PartList mPartUploadETagList;


    void clear() override {
        CloudFileSystem::File::clear();

        mPartUploadCache = nullptr;
        mPartUploadCacheSize = 0;
        mPartUploadToken.clear();
        mPartUploadNumber = 0;
        mPartUploadETagList.clear();
    }

    static constexpr const int UploadPartMinSize = 100 * 1024;
};

/***********************************************/
/***** static variables initialize *************/
/***********************************************/
std::atomic<int> AliCloudFileSystemImpl::sCounter;


/***********************************************/
/***** static function implement ***************/
/***********************************************/

/***********************************************/
/***** class public function implement  ********/
/***********************************************/
AliCloudFileSystemImpl::AliCloudFileSystemImpl()
    : mAliOssClient()
{
    Log::D(Log::TAG, __PRETTY_FUNCTION__);
}

AliCloudFileSystemImpl::~AliCloudFileSystemImpl()
{
    Log::D(Log::TAG, __PRETTY_FUNCTION__);

    if(--sCounter == 0) {
        AliOss::ShutdownSdk();
        Log::D(Log::TAG, "%s Deinitialized aos sdk", __PRETTY_FUNCTION__);
    }
}

int AliCloudFileSystemImpl::login(const std::string& site,
                                  const std::string& user,
                                  const std::string& password,
                                  const std::string& token)
{
    Log::D(Log::TAG, __PRETTY_FUNCTION__);
    if(sCounter <= 0) {
        Log::D(Log::TAG, "%s Initialize aos sdk", __PRETTY_FUNCTION__);
        AliOss::InitializeSdk();
        sCounter = 1;
    }

    AliOss::Credentials credentials(user, password, token);
    AliOss::ClientConfiguration conf;
    mAliOssClient = std::make_shared<AliOss::OssClient>(site, credentials, conf);

    return 0;
}

int AliCloudFileSystemImpl::mount(const std::string& label, CloudMode mode)
{
    Log::D(Log::TAG, __PRETTY_FUNCTION__);

    AliOss::CreateBucketRequest request{label,
                                        AliOss::StorageClass::Standard,
                                        static_cast<AliOss::CannedAccessControlList>(mode)};
    auto aliOssRet = mAliOssClient->CreateBucket(request);
    CHECK_ALIOSS_ECODE(aliOssRet);

    return 0;
}

int AliCloudFileSystemImpl::open(const std::string& label,
                                 const std::string& filepath,
                                 CloudMode mode,
                                 std::shared_ptr<File>& file)
{
    file = std::make_shared<AliOssFile>();
    file->mLabel = label;
    file->mPath = filepath;


    return 0;
}

int AliCloudFileSystemImpl::close(const std::shared_ptr<File> file)
{
    auto filePtr = std::static_pointer_cast<AliOssFile>(file);
    if(filePtr == nullptr) {
        CHECK_ERRCODE(ErrCode::InvalidArgument);
    }

    int ret = partUpload(file, true);
    CHECK_ERRCODE(ret);

    return 0;
}

int AliCloudFileSystemImpl::write(const std::shared_ptr<File> file,
                                  const uint8_t buf[], int size)
{
    auto filePtr = std::static_pointer_cast<AliOssFile>(file);
    if(filePtr == nullptr) {
        CHECK_ERRCODE(ErrCode::InvalidArgument);
    }

    if(filePtr->mPartUploadCache == nullptr) {
        filePtr->mPartUploadCache = std::make_shared<std::stringstream>();
        filePtr->mPartUploadCacheSize = 0;
    }
    filePtr->mPartUploadCache->write(reinterpret_cast<const char*>(buf), size);
    filePtr->mPartUploadCacheSize += size;
    filePtr->mWritePostion += size;
    if(filePtr->mPartUploadCacheSize < AliOssFile::UploadPartMinSize) {
        return size;
    }

    int ret = partUpload(file, false);
    CHECK_ERRCODE(ret);

    return size;
}

int AliCloudFileSystemImpl::read(const std::shared_ptr<File> file,
                                 uint8_t buf[], int size)
{
    auto filePtr = std::static_pointer_cast<AliOssFile>(file);
    if(filePtr == nullptr) {
        CHECK_ERRCODE(ErrCode::InvalidArgument);
    }

    if(filePtr->mSize <= 0) {
        auto aliOssRet = mAliOssClient->HeadObject(filePtr->mLabel, filePtr->mPath);
        CHECK_ALIOSS_ECODE(aliOssRet);
        filePtr->mSize = aliOssRet.result().ContentLength();
    }

    if(filePtr->mReadPostion >= filePtr->mSize) {
        return 0;
    }
    if(filePtr->mReadPostion + size >= filePtr->mSize) {
        size = filePtr->mSize - filePtr->mReadPostion;
    }

    AliOss::GetObjectRequest request {filePtr->mLabel, filePtr->mPath};
    request.setRange(filePtr->mReadPostion , filePtr->mReadPostion + size - 1);
    auto aliOssRet = mAliOssClient->GetObject(request);
    CHECK_ALIOSS_ECODE(aliOssRet);

    int recvSize = static_cast<int>(aliOssRet.result().Metadata().ContentLength());
    // Log::W(Log::TAG, "%s range=(%d~%d), return=%d", __PRETTY_FUNCTION__, filePtr->mReadPostion , filePtr->mReadPostion + size - 1, recvSize);
    aliOssRet.result().Content()->read(reinterpret_cast<char*>(buf), recvSize);

    filePtr->mReadPostion += recvSize;

    return recvSize;
}

int AliCloudFileSystemImpl::write(const std::shared_ptr<File> file,
                                  const std::shared_ptr<std::iostream> stream)
{
    auto filePtr = std::static_pointer_cast<AliOssFile>(file);
    if(filePtr == nullptr) {
        CHECK_ERRCODE(ErrCode::InvalidArgument);
    }

    stream->seekg(0, stream->end);
    int size = stream->tellg();
    stream->seekg(0, stream->beg);

    AliOss::PutObjectRequest request {filePtr->mLabel, filePtr->mPath, stream};
    auto aliOssRet = mAliOssClient->PutObject(request);
    CHECK_ALIOSS_ECODE(aliOssRet);
    
    return size;
}

int AliCloudFileSystemImpl::read(const std::shared_ptr<File> file,
                                 std::shared_ptr<std::iostream> stream)
{
    auto filePtr = std::static_pointer_cast<AliOssFile>(file);
    if(filePtr == nullptr) {
        CHECK_ERRCODE(ErrCode::InvalidArgument);
    }

    stream->clear();

    AliOss::GetObjectRequest request {filePtr->mLabel, filePtr->mPath};
    request.setRange(filePtr->mReadPostion, -1);
    request.setResponseStreamFactory([=]() {return stream;});
    auto aliOssRet = mAliOssClient->GetObject(request);
    CHECK_ALIOSS_ECODE(aliOssRet);

    int size = static_cast<int>(aliOssRet.result().Metadata().ContentLength());

    // filePtr->mReadPostion += size;

    return size;
}

/***********************************************/
/***** class protected function implement  *****/
/***********************************************/


/***********************************************/
/***** class private function implement  *******/
/***********************************************/
int AliCloudFileSystemImpl::partUpload(const std::shared_ptr<File> file, bool lastTime)
{
    auto filePtr = std::static_pointer_cast<AliOssFile>(file);
    if(filePtr == nullptr) {
        CHECK_ERRCODE(ErrCode::InvalidArgument);
    }

    if(filePtr->mPartUploadCache == nullptr
    || filePtr->mPartUploadCacheSize <= 0) {
        return 0;
    }

    if(filePtr->mPartUploadToken.empty() == true) {
        AliOss::InitiateMultipartUploadRequest request(filePtr->mLabel, filePtr->mPath);
        auto aliOssRet = mAliOssClient->InitiateMultipartUpload(request);
        CHECK_ALIOSS_ECODE(aliOssRet);
        filePtr->mPartUploadToken = aliOssRet.result().UploadId();
    }

    AliOss::UploadPartRequest request {filePtr->mLabel, filePtr->mPath, filePtr->mPartUploadCache};
    request.setContentLength(filePtr->mPartUploadCacheSize);
    request.setUploadId(filePtr->mPartUploadToken);
    request.setPartNumber(filePtr->mPartUploadNumber + 1);
    auto aliOssRet = mAliOssClient->UploadPart(request);
    CHECK_ALIOSS_ECODE(aliOssRet);

    filePtr->mPartUploadNumber++;
    AliOss::Part part(filePtr->mPartUploadNumber, aliOssRet.result().ETag());
    filePtr->mPartUploadETagList.push_back(part);

    filePtr->mPartUploadCache = nullptr;
    filePtr->mPartUploadCacheSize = 0;

    // Log::D(Log::TAG, "%s num=%d,tag=%s", __PRETTY_FUNCTION__,
    //                  filePtr->mPartUploadNumber, aliOssRet.result().ETag().c_str());

    if(lastTime == false) {
        return 0;
    }

    if(filePtr->mPartUploadNumber > 0) {
        AliOss::CompleteMultipartUploadRequest request {filePtr->mLabel, filePtr->mPath};
        request.setUploadId(filePtr->mPartUploadToken);
        request.setPartList(filePtr->mPartUploadETagList);
        auto aliOssRet = mAliOssClient->CompleteMultipartUpload(request);
        CHECK_ALIOSS_ECODE(aliOssRet);
    }

    return 0;
}

int AliCloudFileSystemImpl::transAliOssErrCode(bool isSuccess, AlibabaCloud::OSS::OssError& aliOssError)
{
    if (isSuccess == true) {
        return 0;
    }

    const std::map<std::string, int> errCodeMap {
        {"AccessDenied", ErrCode::AliOssAccessDenied},
        {"BucketAlreadyExists", ErrCode::AliOssBucketAlreadyExists},
        {"BucketNotEmpty", ErrCode::AliOssBucketNotEmpty},
        {"EntityTooLarge", ErrCode::AliOssEntityTooLarge},
        {"EntityTooSmall", ErrCode::AliOssEntityTooSmall},
        {"FileGroupTooLarge", ErrCode::AliOssFileGroupTooLarge},
        {"InvalidLinkName", ErrCode::AliOssInvalidLinkName},
        {"LinkPartNotExist", ErrCode::AliOssLinkPartNotExist},
        {"ObjectLinkTooLarge", ErrCode::AliOssObjectLinkTooLarge},
        {"FieldItemTooLong", ErrCode::AliOssFieldItemTooLong},
        {"FilePartInterity", ErrCode::AliOssFilePartInterity},
        {"FilePartNotExist", ErrCode::AliOssFilePartNotExist},
        {"FilePartStale", ErrCode::AliOssFilePartStale},
        {"IncorrectNumberOfFilesInPOSTRequest", ErrCode::AliOssIncorrectNumberOfFilesInPOSTRequest},
        {"InvalidArgument", ErrCode::AliOssInvalidArgument},
        {"InvalidAccessKeyId", ErrCode::AliOssInvalidAccessKeyId},
        {"InvalidBucketName", ErrCode::AliOssInvalidBucketName},
        {"InvalidDigest", ErrCode::AliOssInvalidDigest},
        {"InvalidEncryptionAlgorithmError", ErrCode::AliOssInvalidEncryptionAlgorithmError},
        {"InvalidObjectName", ErrCode::AliOssInvalidObjectName},
        {"InvalidPart", ErrCode::AliOssInvalidPart},
        {"InvalidPartOrder", ErrCode::AliOssInvalidPartOrder},
        {"InvalidPolicyDocument", ErrCode::AliOssInvalidPolicyDocument},
        {"InvalidTargetBucketForLogging", ErrCode::AliOssInvalidTargetBucketForLogging},
        {"InternalError", ErrCode::AliOssInternalError},
        {"MalformedXML", ErrCode::AliOssMalformedXML},
        {"MalformedPOSTRequest", ErrCode::AliOssMalformedPOSTRequest},
        {"MaxPOSTPreDataLengthExceededError", ErrCode::AliOssMaxPOSTPreDataLengthExceededError},
        {"MethodNotAllowed", ErrCode::AliOssMethodNotAllowed},
        {"MissingArgument", ErrCode::AliOssMissingArgument},
        {"MissingContentLength", ErrCode::AliOssMissingContentLength},
        {"NoSuchBucket", ErrCode::AliOssNoSuchBucket},
        {"NoSuchKey", ErrCode::AliOssNoSuchKey},
        {"NoSuchUpload", ErrCode::AliOssNoSuchUpload},
        {"NotImplemented", ErrCode::AliOssNotImplemented},
        {"PreconditionFailed", ErrCode::AliOssPreconditionFailed},
        {"RequestTimeTooSkewed", ErrCode::AliOssRequestTimeTooSkewed},
        {"RequestTimeout", ErrCode::AliOssRequestTimeout},
        {"RequestIsNotMultiPartContent", ErrCode::AliOssRequestIsNotMultiPartContent},
        {"SignatureDoesNotMatch", ErrCode::AliOssSignatureDoesNotMatch},
        {"TooManyBuckets", ErrCode::AliOssTooManyBuckets},
        {"ValidateError", ErrCode::AliOssValidateError},
        {"ClientError:200023", ErrCode::AliOssClientError200023},
    };

    int ret = ErrCode::AliOssUnknownError;
    auto item = errCodeMap.find(aliOssError.Code());
    if(item != errCodeMap.end()) {
        ret = item->second;
    }
    Log::E(Log::TAG, "Found oss error. ret=%d ecode=%s, emsg=%s, reqid=%s",
                     ret, aliOssError.Code().c_str(), aliOssError.Message().c_str(), aliOssError.RequestId().c_str());

    return ret;
}


} // namespace sdk
} // namespace elastos

