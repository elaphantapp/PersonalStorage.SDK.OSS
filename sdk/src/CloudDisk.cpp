//
//  Elastos.SDK.CloudDisk.cpp
//
//  Created by mengxk on 20/03/06.
//  Copyright © 2016 mengxk. All rights reserved.
//


#include <CloudDisk.hpp>
#include <AliCloudDiskImpl.hpp>

namespace elastos {
namespace sdk {

/***********************************************/
/***** static variables initialize *************/
/***********************************************/

/***********************************************/
/***** static function implement ***************/
/***********************************************/
std::shared_ptr<CloudDisk> CloudDisk::Factory::Create(Domain domain)
{
    std::shared_ptr<CloudDisk> storage;
    switch (domain) {
    case Domain::AliOss:
        storage = std::make_shared<AliCloudDiskImpl>();
        break;
    }

    return storage;
}

/***********************************************/
/***** class public function implement  ********/
/***********************************************/


/***********************************************/
/***** class protected function implement  *****/
/***********************************************/


/***********************************************/
/***** class private function implement  *******/
/***********************************************/


} // namespace sdk
} // namespace elastos

