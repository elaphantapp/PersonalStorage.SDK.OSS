#include <iostream>
#include <vector>
#include <PersonalStorage.SDK.OSS.hpp>

#define CHECK_RETVAL(ret)                                           \
    if(ret < 0) {                                                   \
        std::cout << "Failed to test disk. line:" << __LINE__;   \
        std::cout <<  ", ret=" << ret << std::endl;                 \
        return ret;                                                 \
    }

static int testBuffer(std::shared_ptr<elastos::sdk::CloudPartition> partition);
static int testFile(std::shared_ptr<elastos::sdk::CloudPartition> partition);

// struct OssInfo {
//     static constexpr const char* user = "LTAI4Fp5AVnjm6whAs9Whu4E";
//     static constexpr const char* password = "3N6YyUPedbtAMyn5VtkwaMesFu2VaU";
//     static constexpr const char* disk = "oss-cn-beijing.aliyuncs.com";
//     static constexpr const char* partition = "elastos-partition";
//     static constexpr const char* path = "test-file";
// };

struct OssInfo {
    static constexpr const char* user = "STS.NTSJ8pVG8SWt7KuNQFBWbDeYY";
    static constexpr const char* password = "7RQmRG8dxYGJWhsTQNWZ2gwssHLdML5xaHueTJacESPM";
    static constexpr const char* token = "CAIS2AJ1q6Ft5B2yfSjIr5fmAYLEu5gZ5JWfNW3EqlETTthOq6Dyuzz2IH1OfnRrAuEWsvQynWxR5v0flqZ0UIQAQEXCZMR99NFe6QL5xiUFEyrzv9I+k5SANTW5x3eZtZaglYybIfrZfvCyEQ6m8gZ43br9cxi7QlWhKufnoJV7b9MRLH/aCD1dH4V5KxdFos0XPmezUPG2KUzSn3b3BkhlsRYGvAEZ06mkxdCG4RfzlUDzzvRvx778OZ+5dcJhTr4dBN6+x75xbbGTknwSuRVO/eJtjOlf5TDdt96QRFhbwxOsCdHE38dAFTdIZfE7PfBv9tzxhOcp4MbilprMkgQKf8MtCnyDFdr+nZWYQ7z3aYxjKYyWYSqdjoq9UbDuqB4hbH4hMwdHRsEsMHcYC2Z3F2uAd/T8qQCWMlr6F/HVi/wstp1u1BDt9NyDKFWVBryB0DbI2RTb1OKuXhqAAaO3hyiCOCJqkf1JRqYXV5Jkek1ULcV6x3pKzm3LvZpAmTp0ypjpHNCWYVnQOaAuUyVpOhdbYWO2g4qORtjHRnEcxK1JFICPc8gy7wz1W098Zangg0oBn4yP3R5xRWosM/TyUzb0q8e/CYdHeIqFRSCBkWPnW0gdotlkeJ7sGZUz";
    static constexpr const char* disk = "oss-cn-shanghai.aliyuncs.com";
    static constexpr const char* partition = "ela-sh-storage";
    static constexpr const char* path = "ibHnBRSxErGQPYk1iN6C2Leyr92BYivS1z/test-file";
};

int main( int argc, char **argv )
{
    auto disk = elastos::sdk::CloudDisk::Find(elastos::sdk::CloudDisk::Domain::AliOss);
    int ret = disk->login(OssInfo::disk, OssInfo::user, OssInfo::password, OssInfo::token);
    CHECK_RETVAL(ret);

    std::shared_ptr<elastos::sdk::CloudPartition> partition;
    ret = disk->getPartition(OssInfo::partition, partition);
    CHECK_RETVAL(ret);

    // ret = partition->mount();
    // CHECK_RETVAL(ret);

    // ret = testBuffer(partition);
    // CHECK_RETVAL(ret);

    ret = testFile(partition);
    CHECK_RETVAL(ret);

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

    auto file = std::make_shared<elastos::sdk::CloudFile>();
    int ret = file->open(partition, OssInfo::path, elastos::sdk::CloudMode::UserAll);
    CHECK_RETVAL(ret);

    inputFile = std::make_shared<std::fstream>(inputBufferName, std::ios::in);
    for(auto idx = 0; idx < 8; idx++) {
        uint8_t buf[1024];
        inputFile->read((char*)buf, sizeof(buf));
        ret = file->write(buf, sizeof(buf));
        CHECK_RETVAL(ret);
        std::cout << "write buff idx=" << idx << std::endl;
    }
    ret = file->flush();
    CHECK_RETVAL(ret);
    std::cout << "flush buff" << std::endl;

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
    int ret = file->open(partition, OssInfo::path, elastos::sdk::CloudMode::UserAll);
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
