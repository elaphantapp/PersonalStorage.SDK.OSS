#include <iostream>
#include <vector>
#include <Elastos.SDK.CloudStorage.hpp>

#define CHECK_RETVAL(ret)                                           \
    if(ret < 0) {                                                   \
        std::cout << "Failed to test disk. line:" << __LINE__;   \
        std::cout <<  ", ret=" << ret << std::endl;                 \
        return ret;                                                 \
    }

static int testBuffer(std::shared_ptr<elastos::sdk::CloudPartition> partition);
static int testFile(std::shared_ptr<elastos::sdk::CloudPartition> partition);

int main( int argc, char **argv )
{
    auto disk = elastos::sdk::CloudDisk::Find(elastos::sdk::CloudDisk::Domain::AliOss);
    int ret = disk->login("oss-cn-beijing.aliyuncs.com",
                          "LTAI4Fp5AVnjm6whAs9Whu4E",
                          "3N6YyUPedbtAMyn5VtkwaMesFu2VaU"); 
    CHECK_RETVAL(ret);

    std::shared_ptr<elastos::sdk::CloudPartition> partition;
    ret = disk->getPartition("elastos-partition", partition);
    // ret = disk->getPartition("ela-sh-storage", partition);
    CHECK_RETVAL(ret);

    ret = partition->mount();
    CHECK_RETVAL(ret);

    ret = testBuffer(partition);
    CHECK_RETVAL(ret);

    // ret = testFile(partition);
    // CHECK_RETVAL(ret);

    return 0;
}

int testBuffer(std::shared_ptr<elastos::sdk::CloudPartition> partition)
{
    std::cout << "test buffer..." << std::endl;
    const char* inputBufferName = "/tmp/elastos.sdk.CloudStorate.Test.inputBuffer";
    const char* outputBufferName = "/tmp/elastos.sdk.CloudStorate.Test.outputBuffer";

    std::vector<uint8_t> writeBuf;
    for(auto idx = 0; idx < 1024; idx++) {
        writeBuf.insert(writeBuf.end(), {0, 1, 2, 3, 0, 4, 5, 6});
    }
    auto inputFile = std::make_shared<std::fstream>();
    inputFile->open(inputBufferName, std::ios::out | std::ios::binary);
    inputFile->write(reinterpret_cast<char*>(writeBuf.data()), writeBuf.size());
    inputFile->close();

    const char* cloudFilePath = "test1/test2/test3/test4/buffer";

    auto file = std::make_shared<elastos::sdk::CloudFile>();
    int ret = file->open(partition, cloudFilePath, elastos::sdk::CloudMode::UserAll);
    CHECK_RETVAL(ret);

    inputFile = std::make_shared<std::fstream>(inputBufferName, std::ios::in);
    for(auto idx = 0; idx < 8; idx++) {
        uint8_t buf[1024];
        inputFile->read((char*)buf, sizeof(buf));
        ret = file->write(buf, sizeof(buf));
        CHECK_RETVAL(ret);
        std::cout << "write buff idx=" << idx << std::endl;
    }

    ret = file->close();
    CHECK_RETVAL(ret);

    ret = file->open(partition, cloudFilePath, elastos::sdk::CloudMode::UserAll);
    CHECK_RETVAL(ret);
    auto outputFile = std::make_shared<std::fstream>(outputBufferName, std::ios::out);
    for(auto idx = 0; idx < 80; idx++) {
        uint8_t buf[1020];
        ret = file->read(buf, sizeof(buf));
        if(ret == 0) { // finished.
            break;
        }
        CHECK_RETVAL(ret);
        outputFile->write((char*)buf, ret);
        std::cout << "read buff idx=" << idx << std::endl;
    }
    outputFile->close();

    ret = file->close();
    CHECK_RETVAL(ret);

    return 0;
}

int testFile(std::shared_ptr<elastos::sdk::CloudPartition> partition)
{
    std::cout << "test file..." << std::endl;
    const char* inputFileName = "/tmp/elastos.sdk.CloudStorate.Test.inputFile";
    const char* outputFileName = "/tmp/elastos.sdk.CloudStorate.Test.outputFile";

    std::vector<uint8_t> writeBuf;
    for(auto idx = 0; idx < 1024 * 1024; idx++) {
        writeBuf.insert(writeBuf.end(), {0, 1, 2, 3, 0, 4, 5, 6});
    }
    auto inputFile = std::make_shared<std::ofstream>();
    inputFile->open(inputFileName, std::ios::out | std::ios::binary);
    inputFile->write(reinterpret_cast<char*>(writeBuf.data()), writeBuf.size());
    inputFile->close();

    auto file = std::make_shared<elastos::sdk::CloudFile>();
    int ret = file->open(partition, "test-file", elastos::sdk::CloudMode::UserAll);
    CHECK_RETVAL(ret);

    ret = file->write(std::make_shared<std::fstream>(inputFileName, std::ios::in));
    CHECK_RETVAL(ret);
    std::cout << "write file: " << inputFileName << std::endl;

    ret = file->read(std::make_shared<std::fstream>(outputFileName, std::ios::out));
    CHECK_RETVAL(ret);
    std::cout << "read file: " << outputFileName << std::endl;

    ret = file->close();
    CHECK_RETVAL(ret);

    return 0;
}
