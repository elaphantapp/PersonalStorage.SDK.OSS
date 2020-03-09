//
//  Elastos.SDK.CloudPartition.cpp
//
//  Created by mengxk on 20/03/06.
//  Copyright © 2016 mengxk. All rights reserved.
//


#include <CloudFile.hpp>
#include <CloudFileSystem.hpp>
#include <CloudPartition.hpp>
#include <ErrCode.hpp>
#include <Log.hpp>
#include <sstream>
#include <fstream>

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
    auto ret = mPartition->getFileSystem()->open(mPartition->getLabel(),
                                                  path, mode, mFile);
    CHECK_ERRCODE(ret);

    return 0;
}

int CloudFile::close()
{
    auto ret = mPartition->getFileSystem()->close(mFile);
    CHECK_ERRCODE(ret);

    mPartition = nullptr;
    mFile = nullptr;
    return 0;
}

int CloudFile::write(uint8_t buf[], int size)
{
    // auto istream = std::make_shared<std::iostream>();
    std::shared_ptr<std::iostream> istream = std::make_shared<std::stringstream>();
    istream->write(reinterpret_cast<char*>(buf), size);
    auto ret = mPartition->getFileSystem()->write(mFile, istream);
    CHECK_ERRCODE(ret);

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

