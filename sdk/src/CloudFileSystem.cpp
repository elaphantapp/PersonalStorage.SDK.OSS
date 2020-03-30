//
//  AliCloudFileSystemImpl.cpp
//
//  Created by mengxk on 20/03/06.
//  Copyright © 2016 mengxk. All rights reserved.
//

#include <CloudFileSystem.hpp>

namespace elastos {
namespace sdk {

/***********************************************/
/***** static variables initialize *************/
/***********************************************/


/***********************************************/
/***** static function implement ***************/
/***********************************************/
std::string CloudFileSystem::FormatPath(const std::string& path)
{
    std::string formattedPath = path;
    auto pos = std::string::npos;
    while ((pos = formattedPath.find("/")) == 0) {
        formattedPath.erase(pos, 1);
    }
    while ((pos = formattedPath.find("//")) != std::string::npos) {
        formattedPath.erase(pos, 1);
    }

    return formattedPath;
}

/***********************************************/
/***** class public function implement  ********/
/***********************************************/


/***********************************************/
/***** class protected function implement  *****/
/***********************************************/


/***********************************************/
/***** class private function implement  *******/
/***********************************************/

} // namespace sdk
} // namespace elastos

