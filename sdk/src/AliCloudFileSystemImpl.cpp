//
//  AliCloudFileSystemImpl.cpp
//
//  Created by mengxk on 20/03/06.
//  Copyright © 2016 mengxk. All rights reserved.
//

#include <AliCloudFileSystemImpl.hpp>

#include <alibabacloud/oss/OssClient.h>
#include <CloudErrCode.hpp>
#include <Log.hpp>

namespace AliOss = AlibabaCloud::OSS;

#define CHECK_ALIOSS_ECODE(aliOssRet) {                                     \
    int ret = transAliOssErrCode(aliOssRet.isSuccess(), aliOssRet.error()); \
    CHECK_CLOUD_ECODE(ret);                                                     \
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
    Log::D(Log::TAG, FORMAT_METHOD);
}

AliCloudFileSystemImpl::~AliCloudFileSystemImpl()
{
    Log::D(Log::TAG, FORMAT_METHOD);

    if(--sCounter == 0) {
        AliOss::ShutdownSdk();
        Log::D(Log::TAG, "%s Deinitialized aos sdk", FORMAT_METHOD);
    }
}

int AliCloudFileSystemImpl::login(const std::string& site,
                                  const std::string& user,
                                  const std::string& password,
                                  const std::string& token)
{
    Log::D(Log::TAG, FORMAT_METHOD);
    if(sCounter <= 0) {
        Log::D(Log::TAG, "%s Initialize aos sdk", FORMAT_METHOD);
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
    Log::D(Log::TAG, FORMAT_METHOD);

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
    file->label = label;
    file->path = FormatPath(filepath);


    return 0;
}

int AliCloudFileSystemImpl::close(const std::shared_ptr<File> file)
{
    auto filePtr = std::static_pointer_cast<AliOssFile>(file);
    if(filePtr == nullptr) {
        CHECK_CLOUD_ECODE(CloudErrCode::InvalidArgument);
    }

    int ret = partUpload(file, true);
    CHECK_CLOUD_ECODE(ret);

    return 0;
}

int AliCloudFileSystemImpl::stat(const std::shared_ptr<File> file,
                                 Stat& stat)
{
    auto filePtr = std::static_pointer_cast<AliOssFile>(file);
    if(filePtr == nullptr) {
        CHECK_CLOUD_ECODE(CloudErrCode::InvalidArgument);
    }

    auto aliOssRet = mAliOssClient->GetObjectMeta(filePtr->label, filePtr->path);
    CHECK_ALIOSS_ECODE(aliOssRet);

    auto metaData = aliOssRet.result();
    stat.size = metaData.ContentLength();

    std::tm tm = {};
    strptime(metaData.LastModified().c_str(), "%a, %d %b %Y %H:%M:%S %Z", &tm); // Tue, 24 Mar 2020 06:42:43 GMT
    auto timePoint = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    auto now = std::chrono::duration_cast<std::chrono::milliseconds>(timePoint.time_since_epoch());
    stat.modifiedTime = now.count();

    return 0;
}

int AliCloudFileSystemImpl::list(const std::shared_ptr<File> file,
                                 std::vector<std::string>& subFiles)
{
    auto filePtr = std::static_pointer_cast<AliOssFile>(file);
    if(filePtr == nullptr) {
        CHECK_CLOUD_ECODE(CloudErrCode::InvalidArgument);
    }

    AliOss::ListObjectsRequest request {filePtr->label};
    request.setPrefix(filePtr->path);
    request.setMaxKeys(1000);
    auto aliOssRet = mAliOssClient->ListObjects(request);
    CHECK_ALIOSS_ECODE(aliOssRet);

    subFiles.clear();
    for (const auto& object : aliOssRet.result().ObjectSummarys()) {
        subFiles.push_back(object.Key());
    }

    return subFiles.size();
}

int AliCloudFileSystemImpl::remove(const std::shared_ptr<File> file)
{
    auto filePtr = std::static_pointer_cast<AliOssFile>(file);
    if(filePtr == nullptr) {
        CHECK_CLOUD_ECODE(CloudErrCode::InvalidArgument);
    }

    std::vector<std::string> subFiles;
    int ret = list(filePtr, subFiles);
    CHECK_CLOUD_ECODE(ret);

    AliOss::DeleteObjectsRequest request {filePtr->label};
    for(const auto& it: subFiles) {
        request.addKey(it);
    }
    request.addKey(filePtr->path);
    auto aliOssRet = mAliOssClient->DeleteObjects(request);
    CHECK_ALIOSS_ECODE(aliOssRet);

    return 0;
}

int AliCloudFileSystemImpl::write(const std::shared_ptr<File> file,
                                  const uint8_t buf[], int size)
{
    if(buf == nullptr) {
        CHECK_CLOUD_ECODE(CloudErrCode::InvalidArgument);
    }

    auto filePtr = std::static_pointer_cast<AliOssFile>(file);
    if(filePtr == nullptr) {
        CHECK_CLOUD_ECODE(CloudErrCode::InvalidArgument);
    }

    if(filePtr->mPartUploadCache == nullptr) {
        filePtr->mPartUploadCache = std::make_shared<std::stringstream>();
        filePtr->mPartUploadCacheSize = 0;
    }
    filePtr->mPartUploadCache->write(reinterpret_cast<const char*>(buf), size);
    filePtr->mPartUploadCacheSize += size;
    filePtr->writePosition += size;
    if(filePtr->mPartUploadCacheSize < AliOssFile::UploadPartMinSize) {
        return size;
    }

    int ret = partUpload(file, false);
    CHECK_CLOUD_ECODE(ret);

    return size;
}

int AliCloudFileSystemImpl::read(const std::shared_ptr<File> file,
                                 uint8_t buf[], int size)
{
    if(buf == nullptr) {
        CHECK_CLOUD_ECODE(CloudErrCode::InvalidArgument);
    }

    auto filePtr = std::static_pointer_cast<AliOssFile>(file);
    if(filePtr == nullptr) {
        CHECK_CLOUD_ECODE(CloudErrCode::InvalidArgument);
    }

    if(filePtr->size <= 0) {
        auto aliOssRet = mAliOssClient->GetObjectMeta(filePtr->label, filePtr->path);
        CHECK_ALIOSS_ECODE(aliOssRet);
        filePtr->size = aliOssRet.result().ContentLength();
    }

    if(filePtr->readPosition >= filePtr->size) {
        return 0;
    }
    if(filePtr->readPosition + size >= filePtr->size) {
        size = filePtr->size - filePtr->readPosition;
    }

    AliOss::GetObjectRequest request {filePtr->label, filePtr->path};
    request.setRange(filePtr->readPosition , filePtr->readPosition + size - 1);
    auto aliOssRet = mAliOssClient->GetObject(request);
    CHECK_ALIOSS_ECODE(aliOssRet);

    int recvSize = static_cast<int>(aliOssRet.result().Metadata().ContentLength());
    // Log::W(Log::TAG, "%s range=(%d~%d), return=%d", FORMAT_METHOD, filePtr->readPosition , filePtr->readPosition + size - 1, recvSize);
    aliOssRet.result().Content()->read(reinterpret_cast<char*>(buf), recvSize);

    filePtr->readPosition += recvSize;

    return recvSize;
}

int AliCloudFileSystemImpl::write(const std::shared_ptr<File> file,
                                  const std::shared_ptr<std::iostream> stream)
{
    auto filePtr = std::static_pointer_cast<AliOssFile>(file);
    if(filePtr == nullptr) {
        CHECK_CLOUD_ECODE(CloudErrCode::InvalidArgument);
    }

    stream->seekg(0, stream->end);
    int size = stream->tellg();
    stream->seekg(0, stream->beg);

    AliOss::PutObjectRequest request {filePtr->label, filePtr->path, stream};
    auto aliOssRet = mAliOssClient->PutObject(request);
    CHECK_ALIOSS_ECODE(aliOssRet);
    
    return size;
}

int AliCloudFileSystemImpl::read(const std::shared_ptr<File> file,
                                 std::shared_ptr<std::iostream> stream)
{
    auto filePtr = std::static_pointer_cast<AliOssFile>(file);
    if(filePtr == nullptr) {
        CHECK_CLOUD_ECODE(CloudErrCode::InvalidArgument);
    }

    stream->clear();

    AliOss::GetObjectRequest request {filePtr->label, filePtr->path};
    request.setRange(filePtr->readPosition, -1);
    request.setResponseStreamFactory([=]() {return stream;});
    auto aliOssRet = mAliOssClient->GetObject(request);
    CHECK_ALIOSS_ECODE(aliOssRet);

    int size = static_cast<int>(aliOssRet.result().Metadata().ContentLength());

    // filePtr->readPosition += size;

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
        CHECK_CLOUD_ECODE(CloudErrCode::InvalidArgument);
    }

    if(filePtr->mPartUploadCache == nullptr
    || filePtr->mPartUploadCacheSize <= 0) {
        return 0;
    }

    if(filePtr->mPartUploadToken.empty() == true) {
        AliOss::InitiateMultipartUploadRequest request(filePtr->label, filePtr->path);
        auto aliOssRet = mAliOssClient->InitiateMultipartUpload(request);
        CHECK_ALIOSS_ECODE(aliOssRet);
        filePtr->mPartUploadToken = aliOssRet.result().UploadId();
    }

    AliOss::UploadPartRequest request {filePtr->label, filePtr->path, filePtr->mPartUploadCache};
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

    // Log::D(Log::TAG, "%s num=%d,tag=%s", FORMAT_METHOD,
    //                  filePtr->mPartUploadNumber, aliOssRet.result().ETag().c_str());

    if(lastTime == false) {
        return 0;
    }

    if(filePtr->mPartUploadNumber > 0) {
        AliOss::CompleteMultipartUploadRequest request {filePtr->label, filePtr->path};
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
        {"AccessDenied", CloudErrCode::AliOssAccessDenied},
        {"BucketAlreadyExists", CloudErrCode::AliOssBucketAlreadyExists},
        {"BucketNotEmpty", CloudErrCode::AliOssBucketNotEmpty},
        {"EntityTooLarge", CloudErrCode::AliOssEntityTooLarge},
        {"EntityTooSmall", CloudErrCode::AliOssEntityTooSmall},
        {"FileGroupTooLarge", CloudErrCode::AliOssFileGroupTooLarge},
        {"InvalidLinkName", CloudErrCode::AliOssInvalidLinkName},
        {"LinkPartNotExist", CloudErrCode::AliOssLinkPartNotExist},
        {"ObjectLinkTooLarge", CloudErrCode::AliOssObjectLinkTooLarge},
        {"FieldItemTooLong", CloudErrCode::AliOssFieldItemTooLong},
        {"FilePartInterity", CloudErrCode::AliOssFilePartInterity},
        {"FilePartNotExist", CloudErrCode::AliOssFilePartNotExist},
        {"FilePartStale", CloudErrCode::AliOssFilePartStale},
        {"IncorrectNumberOfFilesInPOSTRequest", CloudErrCode::AliOssIncorrectNumberOfFilesInPOSTRequest},
        {"InvalidArgument", CloudErrCode::AliOssInvalidArgument},
        {"InvalidAccessKeyId", CloudErrCode::AliOssInvalidAccessKeyId},
        {"InvalidBucketName", CloudErrCode::AliOssInvalidBucketName},
        {"InvalidDigest", CloudErrCode::AliOssInvalidDigest},
        {"InvalidEncryptionAlgorithmError", CloudErrCode::AliOssInvalidEncryptionAlgorithmError},
        {"InvalidObjectName", CloudErrCode::AliOssInvalidObjectName},
        {"InvalidPart", CloudErrCode::AliOssInvalidPart},
        {"InvalidPartOrder", CloudErrCode::AliOssInvalidPartOrder},
        {"InvalidPolicyDocument", CloudErrCode::AliOssInvalidPolicyDocument},
        {"InvalidTargetBucketForLogging", CloudErrCode::AliOssInvalidTargetBucketForLogging},
        {"InternalError", CloudErrCode::AliOssInternalError},
        {"MalformedXML", CloudErrCode::AliOssMalformedXML},
        {"MalformedPOSTRequest", CloudErrCode::AliOssMalformedPOSTRequest},
        {"MaxPOSTPreDataLengthExceededError", CloudErrCode::AliOssMaxPOSTPreDataLengthExceededError},
        {"MethodNotAllowed", CloudErrCode::AliOssMethodNotAllowed},
        {"MissingArgument", CloudErrCode::AliOssMissingArgument},
        {"MissingContentLength", CloudErrCode::AliOssMissingContentLength},
        {"NoSuchBucket", CloudErrCode::AliOssNoSuchBucket},
        {"NoSuchKey", CloudErrCode::AliOssNoSuchKey},
        {"NoSuchUpload", CloudErrCode::AliOssNoSuchUpload},
        {"NotImplemented", CloudErrCode::AliOssNotImplemented},
        {"PreconditionFailed", CloudErrCode::AliOssPreconditionFailed},
        {"RequestTimeTooSkewed", CloudErrCode::AliOssRequestTimeTooSkewed},
        {"RequestTimeout", CloudErrCode::AliOssRequestTimeout},
        {"RequestIsNotMultiPartContent", CloudErrCode::AliOssRequestIsNotMultiPartContent},
        {"SignatureDoesNotMatch", CloudErrCode::AliOssSignatureDoesNotMatch},
        {"TooManyBuckets", CloudErrCode::AliOssTooManyBuckets},
        {"ValidateError", CloudErrCode::AliOssValidateError},
        {"ClientError:200023", CloudErrCode::AliOssClientError200023},
        {"ServerError:404", CloudErrCode::AliOssServerError404},
        
    };

    int ret = CloudErrCode::AliOssUnknownError;
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

