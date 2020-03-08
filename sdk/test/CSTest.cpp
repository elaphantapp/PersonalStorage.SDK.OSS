#include <iostream>
#include <Elastos.SDK.CloudStorage.hpp>

#define CHECK_RETVAL(ret)                                           \
    if(ret < 0) {                                                   \
        std::cout << "Failed to test storage. line:" << __LINE__;   \
        std::cout <<  ", ret=" << ret << std::endl;                 \
        return ret;                                                 \
    }

int main( int argc, char **argv )
{
    auto storage = elastos::sdk::CloudDisk::Factory::Create(elastos::sdk::CloudDisk::Domain::AliOss);
    int ret = storage->mount("oss-cn-beijing.aliyuncs.com",
                             "LTAI4Fp5AVnjm6whAs9Whu4E", "3N6YyUPedbtAMyn5VtkwaMesFu2VaU"); 
                              
    CHECK_RETVAL(ret);

    auto file = std::make_shared<elastos::sdk::CloudFile>();

    ret = file->open(storage, "test", elastos::sdk::CloudFile::Mode::OtherAll);
    CHECK_RETVAL(ret);

    ret = file->mkdirs();
    CHECK_RETVAL(ret);

    std::cout << "================ ret=" << ret << std::endl;

    return 0;
}
