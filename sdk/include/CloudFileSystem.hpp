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
#include "CloudMode.hpp"

namespace elastos {
namespace sdk {

class CloudFileSystem : public std::enable_shared_from_this<CloudFileSystem> {
public:
    /*** type define ***/
    struct File {
        std::string label;
        std::string path;
        int readPosition = 0;
        int writePosition = 0;
        int64_t size = 0;

        virtual void clear() {
            readPosition = 0;
            writePosition = 0;
            size = 0;
        }
    };

    struct Stat {
        int64_t size;
        int64_t modifiedTime;
    };

    /*** static function and variable ***/
    static std::string FormatPath(const std::string& path);

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

    virtual int stat(const std::shared_ptr<File> file,
                     Stat& stat) = 0;
    virtual int list(const std::shared_ptr<File> file,
                     std::vector<std::string>& subFiles) = 0;
    virtual int remove(const std::shared_ptr<File> file) = 0;

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
