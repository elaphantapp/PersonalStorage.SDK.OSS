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
#include <Log.hpp>

namespace AlibabaCloud {
namespace OSS {
class OssError;
class OssClient;
} // namespace OSS
} // namespace AlibabaCloud

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
                      const std::string& password,
                      const std::string& token) override;

    virtual int mount(const std::string& label, CloudMode mode) override;

    virtual int open(const std::string& label,
                     const std::string& filepath,
                     CloudMode mode,
                     std::shared_ptr<File>& file) override;
    virtual int close(const std::shared_ptr<File> file) override;

    virtual int list(const std::shared_ptr<File> file,
                     std::vector<std::string>& subFiles) override;

    virtual int write(const std::shared_ptr<File> file,
                      const uint8_t buf[], int size) override;
    virtual int read(const std::shared_ptr<File> file,
                     uint8_t buf[], int size) override;

    virtual int write(const std::shared_ptr<File> file,
                      const std::shared_ptr<std::iostream> stream) override;
    virtual int read(const std::shared_ptr<File> file,
                     std::shared_ptr<std::iostream> stream) override;

private:
    /*** type define ***/

    /*** static function and variable ***/
    static std::atomic<int> sCounter;

    /*** class function and variable ***/
    int partUpload(const std::shared_ptr<File> file, bool lastTime);
    int transAliOssErrCode(bool isSuccess, AlibabaCloud::OSS::OssError& aliOssError);

    std::shared_ptr<AlibabaCloud::OSS::OssClient> mAliOssClient;
}; // class CloudFileSystem

} // namespace sdk
} // namespace elastos

#endif /* _ELASTOS_SDK_ALI_CLOUD_FILESYSTEM_IMPL_HPP_ */
