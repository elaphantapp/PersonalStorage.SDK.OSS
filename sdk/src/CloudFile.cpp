//
//  Elastos.SDK.CloudDisk.cpp
//
//  Created by mengxk on 20/03/06.
//  Copyright © 2016 mengxk. All rights reserved.
//


#include <CloudFile.hpp>
#include <CloudDisk.hpp>
#include <Log.hpp>
#include <ErrCode.hpp>

namespace elastos {
namespace sdk {

/***********************************************/
/***** static variables initialize *************/
/***********************************************/

/***********************************************/
/***** static function implement ***************/
/***********************************************/
int CloudFile::open(std::shared_ptr<CloudDisk> disk,
                    const std::string& path,
                    Mode mode)
{
    mDisk = disk;
    mPath = path;
    mMode = mode;

    return 0;
}

int CloudFile::close()
{
    mDisk = nullptr;
    mPath = "";
    mMode = Mode::UserAll;

    return 0;
}

int CloudFile::mkdirs()
{
    int ret = mDisk->mkdirs(mPath, mMode);
    return ret;
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

