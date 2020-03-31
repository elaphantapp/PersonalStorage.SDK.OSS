//
//  Elastos.SDK.CloudDisk.cpp
//
//  Created by mengxk on 20/03/06.
//  Copyright © 2016 mengxk. All rights reserved.
//


#include <CloudDisk.hpp>

#include <AliCloudFileSystemImpl.hpp>
#include <CloudPartition.hpp>
#include <CloudErrCode.hpp>
#include <Log.hpp>

namespace elastos {
namespace sdk {

/***********************************************/
/***** static variables initialize *************/
/***********************************************/

/***********************************************/
/***** static function implement ***************/
/***********************************************/
std::shared_ptr<CloudDisk> CloudDisk::Find(Domain domain)
{
    struct Impl : CloudDisk {
    };
    auto disk = std::make_shared<Impl>();

    switch (domain) {
    case Domain::AliOss:
        disk->mCloudFileSystem = std::make_shared<AliCloudFileSystemImpl>();
        break;
    }

    return disk;
}

/***********************************************/
/***** class public function implement  ********/
/***********************************************/
int CloudDisk::login(const std::string& site,
                     const std::string& user,
                     const std::string& password,
                     const std::string& token)
{
    int ret = mCloudFileSystem->login(site, user, password, token);
    CHECK_CLOUD_ECODE(ret);

    mLogined = true;
    return 0;
}

int CloudDisk::getPartition(const std::string& label,
                            std::shared_ptr<CloudPartition>& parition)
{
    if(mLogined == false) {
        CHECK_CLOUD_ECODE(CloudErrCode::ForbiddenBeforeLogin);
    }

    parition = std::make_shared<CloudPartition>(label, mCloudFileSystem);
    return 0;
}


/***********************************************/
/***** class protected function implement  *****/
/***********************************************/

/***********************************************/
/***** class private function implement  *******/
/***********************************************/


} // namespace sdk
} // namespace elastos

