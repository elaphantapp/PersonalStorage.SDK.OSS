/**
 * @file	CloudPartition.hpp
 * @brief	CloudPartition
 * @details	
 *
 * @author	xxx
 * @author	<xxx@xxx.com>
 * @copyright	(c) 2012 xxx All rights reserved.
 **/

#ifndef _ELASTOS_SDK_CLOUD_PARITION_HPP_
#define _ELASTOS_SDK_CLOUD_PARITION_HPP_

#include <string>
#include "CloudMode.hpp"

namespace elastos {
namespace sdk {

class CloudFileSystem;

class CloudPartition {
public:
    /*** type define ***/

    /*** static function and variable ***/

    /*** class function and variable ***/
    explicit CloudPartition(const std::string& label,
                            std::shared_ptr<CloudFileSystem> cloudFileSystem);
    virtual ~CloudPartition();

    virtual int mount(CloudMode mode = CloudMode::UserAll);

    std::string getLabel();
    std::shared_ptr<CloudFileSystem> getFileSystem();

private:
    /*** type define ***/

    /*** static function and variable ***/

    /*** class function and variable ***/
    std::string mLabel;
    std::shared_ptr<CloudFileSystem> mCloudFileSystem = nullptr;

}; // class CloudPartition

} // namespace sdk
} // namespace elastos

#endif /* _ELASTOS_SDK_CLOUD_PARITION_HPP_ */
