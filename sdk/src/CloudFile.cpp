//
//  Elastos.SDK.CloudPartition.cpp
//
//  Created by mengxk on 20/03/06.
//  Copyright © 2016 mengxk. All rights reserved.
//


#include <CloudFile.hpp>
#include <CloudFileSystem.hpp>
#include <CloudPartition.hpp>
#include <CloudErrCode.hpp>
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

int CloudFile::flush()
{
    auto ret = mPartition->getFileSystem()->close(mFile);
    CHECK_ERRCODE(ret);

    mFile->clear();

    return 0;
}

int CloudFile::write(const uint8_t from[], int size)
{
    if(from == nullptr) {
        CHECK_ERRCODE(ErrCode::InvalidArgument);
    }
    if(mFile == nullptr) {
        CHECK_ERRCODE(ErrCode::FileNotOpened);
    }

    auto ret = mPartition->getFileSystem()->write(mFile, from, size);
    CHECK_ERRCODE(ret);

    return ret;
}

int CloudFile::read(uint8_t to[], int size)
{
    if(to == nullptr) {
        CHECK_ERRCODE(ErrCode::InvalidArgument);
    }
    if(mFile == nullptr) {
        CHECK_ERRCODE(ErrCode::FileNotOpened);
    }

    auto ret = mPartition->getFileSystem()->read(mFile, to, size);
    CHECK_ERRCODE(ret);

    return ret;
}

int CloudFile::write(const std::shared_ptr<std::iostream> from)
{
    if(mFile == nullptr) {
        CHECK_ERRCODE(ErrCode::FileNotOpened);
    }

    auto ret = mPartition->getFileSystem()->write(mFile, from);
    CHECK_ERRCODE(ret);

    return ret;
}

int CloudFile::read(std::shared_ptr<std::iostream> to)
{
    if(mFile == nullptr) {
        CHECK_ERRCODE(ErrCode::FileNotOpened);
    }

    auto ret = mPartition->getFileSystem()->read(mFile, to);
    CHECK_ERRCODE(ret);

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

