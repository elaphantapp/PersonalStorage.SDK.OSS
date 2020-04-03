/**
 * @file	CloudFile.hpp
 * @brief	CloudFile
 * @details	
 *
 * @author	xxx
 * @author	<xxx@xxx.com>
 * @copyright	(c) 2012 xxx All rights reserved.
 **/

#ifndef _ELASTOS_SDK_CLOUD_STORAGE_STRATEGY_HPP_
#define _ELASTOS_SDK_CLOUD_STORAGE_STRATEGY_HPP_

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "CloudFileSystem.hpp"
#include "CloudMode.hpp"

namespace elastos {
namespace sdk {

class CloudPartition;

class CloudFile {
public:
    /*** type define ***/
    using Stat = CloudFileSystem::Stat;

    /*** static function and variable ***/

    /*** class function and variable ***/
    explicit CloudFile() = default;
    virtual ~CloudFile() = default;

    virtual int open(std::shared_ptr<CloudPartition> partition,
                     const std::string& path,
                     CloudMode mode = CloudMode::UserAll);
    virtual int close();
    virtual int flush();

    virtual int stat(Stat& stat);
    virtual int list(std::vector<std::string>& subFiles);
    virtual int remove();

    virtual int write(const uint8_t from[], int size);
    virtual int read(uint8_t to[], int size);

    virtual int write(const std::shared_ptr<std::iostream> from);
    virtual int read(std::shared_ptr<std::iostream> to);

private:
    /*** type define ***/

    /*** static function and variable ***/

    /*** class function and variable ***/
    std::shared_ptr<CloudPartition> mPartition;
    std::shared_ptr<CloudFileSystem::File> mFile;
}; // class CloudFileInterface

} // namespace sdk
} // namespace elastos

#endif /* _ELASTOS_SDK_CLOUD_STORAGE_STRATEGY_HPP_ */
