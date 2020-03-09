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

namespace elastos {
namespace sdk {

class CloudFileSystem;
class CloudPartition;

class CloudDisk {
public:
    /*** type define ***/
    enum Domain {
        AliOss,
    }; // enum Domain

    /*** static function and variable ***/
    static std::shared_ptr<CloudDisk> Find(Domain domain);

    /*** class function and variable ***/
    virtual int login(const std::string& site,
                      const std::string& user,
                      const std::string& password,
                      const std::string& token = "");

    virtual int getPartition(const std::string& label,
                             std::shared_ptr<CloudPartition>& parition);

private:
    /*** type define ***/

    /*** static function and variable ***/

    /*** class function and variable ***/
    explicit CloudDisk() = default;
    virtual ~CloudDisk() = default;

    std::shared_ptr<CloudFileSystem> mCloudFileSystem = nullptr;
    bool mLogined = false;

}; // class CloudDisk

} // namespace sdk
} // namespace elastos

#endif /* _ELASTOS_SDK_CLOUD_DISK_HPP_ */
