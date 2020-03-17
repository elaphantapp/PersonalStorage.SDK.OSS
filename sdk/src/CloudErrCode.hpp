#ifndef _ELASTOS_SDK_CLOUD_ERRCODE_HPP_
#define _ELASTOS_SDK_CLOUD_ERRCODE_HPP_

namespace elastos {

class ErrCode {
public:
    /*** type define ***/
#define CHECK_ERRCODE(errCode)                                           \
    if(errCode < 0) {                                                    \
        int errRet = errCode;                                            \
        if(errRet < elastos::ErrCode::SourceLineSection) {               \
            errRet += (__LINE__ * elastos::ErrCode::SourceLineSection);  \
        }                                                                \
        Log::E(Log::TAG, "errcode=%d: (%d) %s.",                         \
                         errRet, __LINE__, FORMAT_METHOD);         \
        return errRet;                                                   \
    }

// #define CHECK_RETVAL(ret) \
// 	if(ret < 0) { \
// 		Log::E(Log::TAG, "Failed to call %s in line %d, return %d.", FORMAT_METHOD, __LINE__, ret); \
//         return; \
// 	}

// #define CHECK_AND_RETDEF(ret, def) \
// 	if(ret < 0) { \
// 		Log::E(Log::TAG, "Failed to call %s in line %d, return %d.", FORMAT_METHOD, __LINE__, ret); \
// 		return def; \
// 	}

    /*** static function and variable ***/
    constexpr static const int ReservedError = -1;
    constexpr static const int UnknownError = -2;
    constexpr static const int UnimplementedError = -3;
    constexpr static const int InvalidArgument = -4;
    constexpr static const int NetworkIOException = -5;
    constexpr static const int SignatureDoesNotMatch = -6;
    constexpr static const int PermissionDenied = -7;
    constexpr static const int FileExists = -8;
    constexpr static const int ForbiddenBeforeLogin = -9;
    constexpr static const int FileNotOpened = -10;
    
    // AliOss ErrCode
    constexpr static const int AliOssUnknownError = -100;
    constexpr static const int AliOssAccessDenied = -101;
    constexpr static const int AliOssBucketAlreadyExists = -102;
    constexpr static const int AliOssBucketNotEmpty = -103;
    constexpr static const int AliOssEntityTooLarge = -104;
    constexpr static const int AliOssEntityTooSmall = -105;
    constexpr static const int AliOssFileGroupTooLarge = -106;
    constexpr static const int AliOssInvalidLinkName = -107;
    constexpr static const int AliOssLinkPartNotExist = -108;
    constexpr static const int AliOssObjectLinkTooLarge = -109;
    constexpr static const int AliOssFieldItemTooLong = -110;
    constexpr static const int AliOssFilePartInterity = -111;
    constexpr static const int AliOssFilePartNotExist = -112;
    constexpr static const int AliOssFilePartStale = -113;
    constexpr static const int AliOssIncorrectNumberOfFilesInPOSTRequest = -114;
    constexpr static const int AliOssInvalidArgument = -115;
    constexpr static const int AliOssInvalidAccessKeyId = -116;
    constexpr static const int AliOssInvalidBucketName = -117;
    constexpr static const int AliOssInvalidDigest = -118;
    constexpr static const int AliOssInvalidEncryptionAlgorithmError = -119;
    constexpr static const int AliOssInvalidObjectName = -120;
    constexpr static const int AliOssInvalidPart = -121;
    constexpr static const int AliOssInvalidPartOrder = -122;
    constexpr static const int AliOssInvalidPolicyDocument = -123;
    constexpr static const int AliOssInvalidTargetBucketForLogging = -124;
    constexpr static const int AliOssInternalError = -125;
    constexpr static const int AliOssMalformedXML = -126;
    constexpr static const int AliOssMalformedPOSTRequest = -127;
    constexpr static const int AliOssMaxPOSTPreDataLengthExceededError = -128;
    constexpr static const int AliOssMethodNotAllowed = -129;
    constexpr static const int AliOssMissingArgument = -130;
    constexpr static const int AliOssMissingContentLength = -131;
    constexpr static const int AliOssNoSuchBucket = -132;
    constexpr static const int AliOssNoSuchKey = -133;
    constexpr static const int AliOssNoSuchUpload = -134;
    constexpr static const int AliOssNotImplemented = -135;
    constexpr static const int AliOssPreconditionFailed = -136;
    constexpr static const int AliOssRequestTimeTooSkewed = -137;
    constexpr static const int AliOssRequestTimeout = -138;
    constexpr static const int AliOssRequestIsNotMultiPartContent = -139;
    constexpr static const int AliOssSignatureDoesNotMatch = -140;
    constexpr static const int AliOssTooManyBuckets = -141;
    constexpr static const int AliOssValidateError = -142;
    constexpr static const int AliOssClientError200023 = -143;
    constexpr static const int AliOssServerError404 = -144;
    
    


    constexpr static const int SourceLineSection = -100000;

    /*** class function and variable ***/

private:
    /*** type define ***/

    /*** static function and variable ***/

    /*** class function and variable ***/
    explicit ErrCode() = delete;
    virtual ~ErrCode() = delete;

}; // class ErrCode

} // namespace elastos

#endif /* _ELASTOS_SDK_CLOUD_ERRCODE_HPP_ */
