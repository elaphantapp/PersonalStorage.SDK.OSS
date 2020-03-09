#ifndef _ELASTOS_ERRCODE_HPP_
#define _ELASTOS_ERRCODE_HPP_

namespace elastos {

class ErrCode {
public:
    /*** type define ***/
#define CHECK_ERRCODE(errCode)                                           \
    if(errCode < 0) {                                                    \
        int errRet = errCode;                                            \
        if(errRet > elastos::ErrCode::SourceLineSection) {               \
            errRet *= (-elastos::ErrCode::SourceLineSection);            \
            errRet -= __LINE__;                                          \
        }                                                                \
        Log::E(Log::TAG, "Failed to call %s in line %d, errcode=%d.",    \
                         __PRETTY_FUNCTION__, __LINE__, errRet);         \
        return errRet;                                                   \
    }

// #define CHECK_RETVAL(ret) \
// 	if(ret < 0) { \
// 		Log::E(Log::TAG, "Failed to call %s in line %d, return %d.", __PRETTY_FUNCTION__, __LINE__, ret); \
//         return; \
// 	}

// #define CHECK_AND_RETDEF(ret, def) \
// 	if(ret < 0) { \
// 		Log::E(Log::TAG, "Failed to call %s in line %d, return %d.", __PRETTY_FUNCTION__, __LINE__, ret); \
// 		return def; \
// 	}

    /*** static function and variable ***/
    constexpr static const int UnknownError = -1;
    constexpr static const int UnimplementedError = -2;
    constexpr static const int NetworkIOException = -3;
    constexpr static const int SignatureDoesNotMatch = -4;
    constexpr static const int PermissionDenied = -5;
    constexpr static const int FileExists = -6;
    constexpr static const int ForbiddenBeforeLogin = -7;
    
    
    // constexpr static const int NotFoundError = -3;
    // constexpr static const int NotReadyError = -4;
    // constexpr static const int InvalidArgument = -5;
    // constexpr static const int IOSystemException = -6;
    // constexpr static const int NetworkException = -7;
    // constexpr static const int PointerReleasedError = -8;
    // constexpr static const int DevUUIDError = -9;
    // constexpr static const int FileNotExistsError = -10;
    // constexpr static const int JsonParseException = -11;
    // constexpr static const int ConflictWithExpectedError = -12;
    // constexpr static const int MergeInfoFailed = -13;
    // constexpr static const int IgnoreMergeOldInfo = -14;
    // constexpr static const int EmptyInfoError = -15;
    // constexpr static const int InvalidFriendCode = -16;
    // constexpr static const int RepeatOperationError = -17;
	// constexpr static const int CreateDirectoryError = -18;
    // constexpr static const int ExpectedBeforeStartedError = -19;
    // constexpr static const int SizeOverflowError = -20;


    constexpr static const int SourceLineSection = -10000;

    /*** class function and variable ***/

private:
    /*** type define ***/

    /*** static function and variable ***/

    /*** class function and variable ***/
    explicit ErrCode() = delete;
    virtual ~ErrCode() = delete;

}; // class ErrCode

} // namespace elastos

#endif /* _ELASTOS_ERRCODE_HPP_ */
