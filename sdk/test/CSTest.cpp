#include <iostream>
#include <Elastos.SDK.CloudStorage.hpp>

#define CHECK_RETVAL(ret)                                           \
    if(ret < 0) {                                                   \
        std::cout << "Failed to test disk. line:" << __LINE__;   \
        std::cout <<  ", ret=" << ret << std::endl;                 \
        return ret;                                                 \
    }

int main( int argc, char **argv )
{
    auto disk = elastos::sdk::CloudDisk::Find(elastos::sdk::CloudDisk::Domain::AliOss);
    int ret = disk->login("oss-cn-beijing.aliyuncs.com",
                          "LTAI4Fp5AVnjm6whAs9Whu4E",
                          "3N6YyUPedbtAMyn5VtkwaMesFu2VaU"); 
    CHECK_RETVAL(ret);

    std::shared_ptr<elastos::sdk::CloudPartition> partition;
    ret = disk->getPartition("elastos-partition", partition);
    CHECK_RETVAL(ret);

    ret = partition->mount();
    CHECK_RETVAL(ret);

    auto file = std::make_shared<elastos::sdk::CloudFile>();
    ret = file->open(partition, "test2", elastos::sdk::CloudMode::OtherAll);
    CHECK_RETVAL(ret);

    uint8_t buf[] = {0, 1, 2, 3, 0, 4, 5, 6};
    ret = file->write(buf, sizeof(buf));
    CHECK_RETVAL(ret);

    ret = file->write(buf, sizeof(buf));
    CHECK_RETVAL(ret);

    ret = file->close();
    CHECK_RETVAL(ret);

    std::cout << "================ ret=" << ret << std::endl;

    return 0;
}
