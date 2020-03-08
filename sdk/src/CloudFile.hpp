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

namespace elastos {
namespace sdk {

class CloudDisk;

class CloudFile {
public:
    /*** type define ***/
    enum class Mode : int {
        UserAll = 0,
        OtherReady = 1,
        OtherAll = 2,
    }; // enum Mode

    /*** static function and variable ***/

    /*** class function and variable ***/
    explicit CloudFile() = default;
    virtual ~CloudFile() = default;

    virtual int open(std::shared_ptr<CloudDisk> disk,
                     const std::string& path,
                     Mode mode = Mode::UserAll);
    virtual int close();

    virtual int mkdirs();

private:
    /*** type define ***/

    /*** static function and variable ***/

    /*** class function and variable ***/
    std::shared_ptr<CloudDisk> mDisk;
    std::string mPath;
    Mode mMode;
}; // class CloudFileInterface

} // namespace sdk
} // namespace elastos

#endif /* _ELASTOS_SDK_CLOUD_STORAGE_STRATEGY_HPP_ */
