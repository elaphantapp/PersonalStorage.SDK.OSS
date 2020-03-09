//
//  Elastos.SDK.CloudPartition.cpp
//
//  Created by mengxk on 20/03/06.
//  Copyright © 2016 mengxk. All rights reserved.
//


#include <CloudFile.hpp>
#include <CloudPartition.hpp>
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
int CloudFile::open(std::shared_ptr<CloudPartition> partition,
                    const std::string& path,
                    CloudMode mode)
{
    mPartition = partition;
    mPath = path;
    mMode = mode;

    return 0;
}

int CloudFile::close()
{
    mPartition = nullptr;
    mPath = "";
    mMode = CloudMode::UserAll;

    return 0;
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

