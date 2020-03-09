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
    std::shared_ptr<std::iostream> mCache = std::make_shared<std::stringstream>();
    int mCacheSize = 0;
    std::string mToken;
    int mPartNumber = 1;
    AliOss::PartList mPartETagList;
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
    AliOss::InitiateMultipartUploadRequest request(label, filepath);
    auto aliOssRet = mAliOssClient->InitiateMultipartUpload(request);
    CHECK_ALIOSS_ECODE(aliOssRet);


    auto filePtr = std::make_shared<AliOssFile>();
    filePtr->mLabel = aliOssRet.result().Bucket();
    filePtr->mPath = aliOssRet.result().Key();
    filePtr->mToken = aliOssRet.result().UploadId();
    file = filePtr;

    return 0;
}

int AliCloudFileSystemImpl::close(const std::shared_ptr<File> file)
{
    auto filePtr = std::static_pointer_cast<AliOssFile>(file);
    if(filePtr == nullptr) {
        CHECK_ERRCODE(ErrCode::InvalidArgument);
    }

    flush(file);

    AliOss::CompleteMultipartUploadRequest request {filePtr->mLabel, filePtr->mPath};
    request.setUploadId(filePtr->mToken);
    request.setPartList(filePtr->mPartETagList);
    auto aliOssRet = mAliOssClient->CompleteMultipartUpload(request);
    CHECK_ALIOSS_ECODE(aliOssRet);

    return 0;
}

int AliCloudFileSystemImpl::write(const std::shared_ptr<File> file,
                                  const std::shared_ptr<std::iostream> istream)
{
    auto filePtr = std::static_pointer_cast<AliOssFile>(file);
    if(filePtr == nullptr) {
        CHECK_ERRCODE(ErrCode::InvalidArgument);
    }

    istream->seekg(0, istream->end);
    int size = istream->tellg();
    istream->seekg(0, istream->beg);

    (*filePtr->mCache) << istream;
    filePtr->mCacheSize += size;
    if(filePtr->mCacheSize < 819200) {
        return 0;
    }

    int ret = flush(file);
    CHECK_ERRCODE(ret);

    return 0;
}

int AliCloudFileSystemImpl::flush(const std::shared_ptr<File> file)
{
    auto filePtr = std::static_pointer_cast<AliOssFile>(file);
    if(filePtr == nullptr) {
        CHECK_ERRCODE(ErrCode::InvalidArgument);
    }

    if(filePtr->mCacheSize <= 0) {
        return 0;
    }

    AliOss::UploadPartRequest request {filePtr->mLabel, filePtr->mPath, filePtr->mCache};
    request.setContentLength(filePtr->mCacheSize);
    request.setUploadId(filePtr->mToken);
    request.setPartNumber(filePtr->mPartNumber);
    auto aliOssRet = mAliOssClient->UploadPart(request);
    CHECK_ALIOSS_ECODE(aliOssRet);

    Log::D(Log::TAG, "%s num=%d,tag=%s", __PRETTY_FUNCTION__,
                     filePtr->mPartNumber, aliOssRet.result().ETag().c_str());
    AliOss::Part part(filePtr->mPartNumber, aliOssRet.result().ETag());
    filePtr->mPartETagList.push_back(part);
    filePtr->mPartNumber++;

    filePtr->mCache->clear();
    filePtr->mCacheSize = 0;

    return 0;
}

/***********************************************/
/***** class protected function implement  *****/
/***********************************************/


/***********************************************/
/***** class private function implement  *******/
/***********************************************/
int AliCloudFileSystemImpl::transAliOssErrCode(bool isSuccess, AlibabaCloud::OSS::OssError& aliOssError)
{
    if (isSuccess == true) {
        return 0;
    }

    Log::E(Log::TAG, "Found oss error.  ecode=%s, emsg=%s, reqid=%s",
                     aliOssError.Code().c_str(), aliOssError.Message().c_str(), aliOssError.RequestId().c_str());
    int ret = ErrCode::NetworkIOException;            
    std::string errCode = aliOssError.Code();
    if (errCode == "SignatureDoesNotMatch") {
        ret = ErrCode::SignatureDoesNotMatch;
    } else if (errCode == "BucketAlreadyExists") {
        ret = ErrCode::FileExists;
    // } else if (status->code == 403) {
    //     ret = ErrCode::PermissionDenied;
    }

    return ret;
}


} // namespace sdk
} // namespace elastos

