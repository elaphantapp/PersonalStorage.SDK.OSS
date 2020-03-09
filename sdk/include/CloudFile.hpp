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

#include <memory>
#include <string>
#include <CloudMode.hpp>

namespace elastos {
namespace sdk {

class CloudPartition;

class CloudFile {
public:
    /*** type define ***/

    /*** static function and variable ***/

    /*** class function and variable ***/
    explicit CloudFile() = default;
    virtual ~CloudFile() = default;

    virtual int open(std::shared_ptr<CloudPartition> partition,
                     const std::string& path,
                     CloudMode mode = CloudMode::UserAll);
    virtual int close();

private:
    /*** type define ***/

    /*** static function and variable ***/

    /*** class function and variable ***/
    std::shared_ptr<CloudPartition> mPartition;
    std::string mPath;
    CloudMode mMode;
}; // class CloudFileInterface

} // namespace sdk
} // namespace elastos

#endif /* _ELASTOS_SDK_CLOUD_STORAGE_STRATEGY_HPP_ */
