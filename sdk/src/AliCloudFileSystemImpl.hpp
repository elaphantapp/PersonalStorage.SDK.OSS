/**
 * @file	AliCloudFileSystemImpl.hpp
 * @brief	AliCloudFileSystemImpl
 * @details	
 *
 * @author	xxx
 * @author	<xxx@xxx.com>
 * @copyright	(c) 2012 xxx All rights reserved.
 **/

#ifndef _ELASTOS_SDK_ALI_CLOUD_FILESYSTEM_IMPL_HPP_
#define _ELASTOS_SDK_ALI_CLOUD_FILESYSTEM_IMPL_HPP_

#include <CloudFileSystem.hpp>

namespace elastos {
namespace sdk {

class AliOssAuth;

class AliCloudFileSystemImpl : public CloudFileSystem {
public:
    /*** type define ***/

    /*** static function and variable ***/

    /*** class function and variable ***/
    explicit AliCloudFileSystemImpl();
    virtual ~AliCloudFileSystemImpl();

    // implement CloudFileSystem
    virtual int login(const std::string& site,
                      const std::string& user,
                      const std::string& password) override;

    virtual int mount(const std::string& name, CloudMode mode) override;


    // implement CloudFile

private:
    /*** type define ***/

    /*** static function and variable ***/
    static std::atomic<int> sCounter;

    /*** class function and variable ***/
    static int transAliOssErrCode(void* aliOssStatus);

    std::shared_ptr<AliOssAuth> mAliOssAuth;
}; // class CloudFileSystem

} // namespace sdk
} // namespace elastos

#endif /* _ELASTOS_SDK_ALI_CLOUD_FILESYSTEM_IMPL_HPP_ */
