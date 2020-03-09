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
                      const std::shared_ptr<std::iostream> istream) = 0;
    // virtual int read(const std::string& volume,
    //                  const std::string& filepath,
    //                  std::shared_ptr<std::iostream>& ostream) = 0;

    virtual int flush(const std::shared_ptr<File> file) = 0;

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
