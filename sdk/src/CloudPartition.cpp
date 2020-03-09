//
//  Elastos.SDK.CloudPartition.cpp
//
//  Created by mengxk on 20/03/06.
//  Copyright © 2016 mengxk. All rights reserved.
//


#include <CloudPartition.hpp>
#include <CloudFileSystem.hpp>
#include <ErrCode.hpp>
#include <Log.hpp>

namespace elastos {
namespace sdk {

/***********************************************/
/***** static variables initialize *************/
/***********************************************/

/***********************************************/
/***** static function implement ***************/
/***********************************************/

/***********************************************/
/***** class public function implement  ********/
/***********************************************/
CloudPartition::CloudPartition(const std::string& label,
                               std::shared_ptr<CloudFileSystem> cloudFileSystem)
    : mLabel(label)
    , mCloudFileSystem(cloudFileSystem) 
{
}
CloudPartition::~CloudPartition()
{
}

int CloudPartition::mount(CloudMode mode)
{
    int ret = mCloudFileSystem->mount(mLabel, mode);
    CHECK_ERRCODE(ret);

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

