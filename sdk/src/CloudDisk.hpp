/**
 * @file	CloudDisk.hpp
 * @brief	CloudDisk
 * @details	
 *
 * @author	xxx
 * @author	<xxx@xxx.com>
 * @copyright	(c) 2012 xxx All rights reserved.
 **/

#ifndef _ELASTOS_SDK_CLOUD_DISK_HPP_
#define _ELASTOS_SDK_CLOUD_DISK_HPP_

#include <string>
#include <CloudFile.hpp>

namespace elastos {
namespace sdk {

class CloudDisk {
public:
    /*** type define ***/
    enum Domain {
        AliOss,
    }; // enum Domain

    class Factory final {
    public:
        static std::shared_ptr<CloudDisk> Create(Domain domain);
    private:
        explicit Factory() = delete;
        virtual ~Factory() = delete;
    }; // class Factory

    /*** static function and variable ***/

    /*** class function and variable ***/
    virtual int mount(const std::string& mountTo,
                      const std::string& user,
                      const std::string& password) = 0;

protected:
    explicit CloudDisk() = default;
    virtual ~CloudDisk() = default;

    virtual int mkdirs(const std::string& path, CloudFile::Mode mode) = 0;

private:
    /*** type define ***/

    /*** static function and variable ***/

    /*** class function and variable ***/

    friend class CloudFile;
}; // class CloudDisk

} // namespace sdk
} // namespace elastos

#endif /* _ELASTOS_SDK_CLOUD_DISK_HPP_ */
