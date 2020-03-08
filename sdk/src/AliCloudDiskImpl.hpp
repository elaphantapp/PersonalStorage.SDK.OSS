/**
 * @file	CloudDisk.hpp
 * @brief	CloudDisk
 * @details	
 *
 * @author	xxx
 * @author	<xxx@xxx.com>
 * @copyright	(c) 2012 xxx All rights reserved.
 **/

#ifndef _ELASTOS_SDK_ALI_CLOUD_DISK_IMPL_HPP_
#define _ELASTOS_SDK_ALI_CLOUD_DISK_IMPL_HPP_

#include <CloudDisk.hpp>

namespace elastos {
namespace sdk {

class AliOssAccess;

class AliCloudDiskImpl : public CloudDisk {
public:
    /*** type define ***/

    /*** static function and variable ***/

    /*** class function and variable ***/
    explicit AliCloudDiskImpl();
    virtual ~AliCloudDiskImpl();

    // implement CloudDisk
    virtual int mount(const std::string& mountTo,
                      const std::string& user,
                      const std::string& password) override;

protected:
    virtual int mkdirs(const std::string& path, CloudFile::Mode mode) override;


    // implement CloudFile

private:
    /*** type define ***/

    /*** static function and variable ***/
    static std::atomic<int> sCounter;

    /*** class function and variable ***/
    static int transAliOssErrCode(void* aliOssStatus);

    std::shared_ptr<AliOssAccess> mAliOssAccess;
}; // class CloudDisk

} // namespace sdk
} // namespace elastos

#endif /* _ELASTOS_SDK_ALI_CLOUD_DISK_IMPL_HPP_ */
