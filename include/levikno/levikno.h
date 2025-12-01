#ifndef HG_LEVIKNO_H
#define HG_LEVIKNO_H


#if defined(_WIN32) || defined(WIN32)
    #ifndef LVN_PLATFORM_WINDOWS
        #define LVN_PLATFORM_WINDOWS
    #endif
#elif defined(__linux__)
    #ifndef LVN_PLATFORM_LINUX
        #define LVN_PLATFORM_LINUX
    #endif
#endif


#ifndef LVN_API
    #ifdef LVN_PLATFORM_WINDOWS
        #ifdef LVN_SHARED_LIBRARY_EXPORT
            #define LVN_API __declspec(dllexport)
        #elif LVN_SHARED_LIBRARY_IMPORT
            #define LVN_API __declspec(dllimport)
        #else
            #define LVN_API
        #endif
    #else
        #define LVN_API
    #endif
#endif


#include <stdint.h>
#include <stdbool.h>

typedef enum LvnResult
{
    Lvn_Result_Success =  0,
    Lvn_Result_Failure = -1,
} LvnResult;


typedef struct LvnContext LvnContext;


typedef struct LvnContextCreateInfo
{
    bool enableLogging;
} LvnContextCreateInfo;


#ifdef __cplusplus
extern "C" {
#endif

LVN_API LvnResult lvnCreateContext(LvnContext** ctx, LvnContextCreateInfo* createInfo);
LVN_API void      lvnDestroyContext(LvnContext* ctx);


#ifdef __cplusplus
}
#endif


#endif /* !HG_LEVIKNO_H */
