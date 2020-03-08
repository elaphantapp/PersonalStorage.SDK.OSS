//
//  Elastos.SDK.CloudDisk.cpp
//
//  Created by mengxk on 20/03/06.
//  Copyright © 2016 mengxk. All rights reserved.
//

#include <AliCloudDiskImpl.hpp>
#include <Log.hpp>
#include <ErrCode.hpp>

#include <oss_api.h>
#include <oss_auth.h>
#include <oss_util.h>

namespace elastos {
namespace sdk {

#define CHECK_AOSERR(code)               \
    if(code != AOSE_OK) {                \
        int ret = code;                  \
        if(ret == AOSE_UNKNOWN_ERROR) {  \
            ret -= 800;                  \
        }                                \
        CHECK_ERRCODE(ret);              \
    }                                     

struct AliOssAccess {
    std::string mEndpoint;
    std::string mAccessKeyId;
    std::string mAccessKeySecret;
};

struct DiskOptions {
    explicit DiskOptions(std::shared_ptr<AliOssAccess> access)
        : aliOssAccess(access)
        , aliOssOptions(nullptr)
        , aliOssPool(nullptr) {
        aos_pool_create(&aliOssPool, nullptr);
        aliOssOptions = oss_request_options_create(aliOssPool);
        aliOssOptions->config = oss_config_create(aliOssOptions->pool);
        aos_str_set(&aliOssOptions->config->endpoint, aliOssAccess->mEndpoint.c_str());
        aos_str_set(&aliOssOptions->config->access_key_id, aliOssAccess->mAccessKeyId.c_str());
        aos_str_set(&aliOssOptions->config->access_key_secret, aliOssAccess->mAccessKeySecret.c_str());
        aliOssOptions->config->is_cname = 0;
        aliOssOptions->ctl = aos_http_controller_create(aliOssOptions->pool, 0);
    };
    virtual ~DiskOptions() {
        aos_pool_destroy(aliOssPool);
        aliOssPool = nullptr;
        aliOssPool = nullptr;
    };
    std::shared_ptr<AliOssAccess> aliOssAccess;
    oss_request_options_t* aliOssOptions;
    aos_pool_t* aliOssPool;
};

/***********************************************/
/***** static variables initialize *************/
/***********************************************/
std::atomic<int> AliCloudDiskImpl::sCounter;


/***********************************************/
/***** static function implement ***************/
/***********************************************/

/***********************************************/
/***** class public function implement  ********/
/***********************************************/
AliCloudDiskImpl::AliCloudDiskImpl()
    : mAliOssAccess()
{
    Log::D(Log::TAG, __PRETTY_FUNCTION__);
}

AliCloudDiskImpl::~AliCloudDiskImpl()
{
    Log::D(Log::TAG, __PRETTY_FUNCTION__);

    if(--sCounter == 0) {
        aos_http_io_deinitialize();
        Log::D(Log::TAG, "%s Deinitialized aos http io", __PRETTY_FUNCTION__);
    }
}

int AliCloudDiskImpl::mount(const std::string& mountTo,
                               const std::string& user,
                               const std::string& password)
{
    Log::D(Log::TAG, __PRETTY_FUNCTION__);
    if(sCounter <= 0) {
        Log::D(Log::TAG, "%s Initialize aos http io", __PRETTY_FUNCTION__);
        int ret = aos_http_io_initialize(nullptr, 0);
        CHECK_AOSERR(ret);

        sCounter = 1;
    }

    mAliOssAccess = std::make_shared<AliOssAccess>();
    mAliOssAccess->mEndpoint = mountTo;
    mAliOssAccess->mAccessKeyId = user;
    mAliOssAccess->mAccessKeySecret = password;

    return 0;
}

int AliCloudDiskImpl::mkdirs(const std::string& path, CloudFile::Mode mode)
{
    Log::D(Log::TAG, __PRETTY_FUNCTION__);
    DiskOptions options {mAliOssAccess};

    aos_string_t bucket;
    std::string realPath = "elastos-cloudstorage2" + path;
    aos_str_set(&bucket, realPath.c_str());
    oss_acl_e aliOssAcl = static_cast<oss_acl_e>(mode);

    aos_status_t* aliOssStatus = oss_create_bucket(options.aliOssOptions, &bucket, aliOssAcl, nullptr);
    int ret = transAliOssErrCode(aliOssStatus);
    CHECK_ERRCODE(ret);

    return 0;
}


/***********************************************/
/***** class protected function implement  *****/
/***********************************************/


/***********************************************/
/***** class private function implement  *******/
/***********************************************/
int AliCloudDiskImpl::transAliOssErrCode(void* aliOssStatus)
{
    aos_status_t* status = reinterpret_cast<aos_status_t*>(aliOssStatus);
    if (aos_status_is_ok(status) == true) {
        return 0;
    }

    Log::E(Log::TAG, "Found error status. code=%d, ecode=%s, emsg=%s, reqid=%s",
                     status->code, status->error_code, status->error_msg, status->req_id);
    if(status->code < 0) {
        return status->code;
    }

    int ret = ErrCode::NetworkIOException;            
    std::string errCode = status->error_code;
    if (errCode == "SignatureDoesNotMatch") {
        ret = ErrCode::SignatureDoesNotMatch;
    } else if (errCode == "BucketAlreadyExists") {
        ret = ErrCode::FileExists;
    } else if (status->code == 403) {
        ret = ErrCode::PermissionDenied;
    }

    return ret;
}


} // namespace sdk
} // namespace elastos

