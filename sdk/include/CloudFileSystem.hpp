/**
 * @file	CloudFileSystem.hpp
 * @brief	CloudFileSystem
 * @details	
 *
 * @author	xxx
 * @author	<xxx@xxx.com>
 * @copyright	(c) 2012 xxx All rights reserved.
 **/

#ifndef _ELASTOS_SDK_CLOUD_FILESYSTEM_HPP_
#define _ELASTOS_SDK_CLOUD_FILESYSTEM_HPP_

#include <string>
#include <CloudMode.hpp>

namespace elastos {
namespace sdk {

class CloudFileSystem : public std::enable_shared_from_this<CloudFileSystem> {
public:
    /*** type define ***/
    struct File {
        std::string mLabel;
        std::string mPath;
        int mReadPostion = 0;
        int mWritePostion = 0;
        int64_t mSize = 0;

        virtual void clear() {
            mReadPostion = 0;
            mWritePostion = 0;
            mSize = 0;
        }
    };

    /*** static function and variable ***/

    /*** class function and variable ***/
    virtual int login(const std::string& site,
                      const std::string& user,
                      const std::string& password,
                      const std::string& token) = 0;

    virtual int mount(const std::string& label, CloudMode mode) = 0; 

    virtual int open(const std::string& label,
                     const std::string& filepath,
                     CloudMode mode,
                     std::shared_ptr<File>& file) = 0;
    virtual int close(const std::shared_ptr<File> file) = 0;

    virtual int write(const std::shared_ptr<File> file,
                      const uint8_t buf[], int size) = 0;
    virtual int read(const std::shared_ptr<File> file,
                     uint8_t buf[], int size) = 0;

    virtual int write(const std::shared_ptr<File> file,
                      const std::shared_ptr<std::iostream> stream) = 0;
    virtual int read(const std::shared_ptr<File> file,
                     std::shared_ptr<std::iostream> stream) = 0;

protected:
    explicit CloudFileSystem() = default;
    virtual ~CloudFileSystem() = default;

private:
    /*** type define ***/

    /*** static function and variable ***/

    /*** class function and variable ***/

    friend class CloudFile;
}; // class CloudFileSystem

} // namespace sdk
} // namespace elastos

#endif /* _ELASTOS_SDK_CLOUD_FILESYSTEM_HPP_ */
