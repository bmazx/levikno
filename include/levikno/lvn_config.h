#ifndef HG_LVN_CONFIG_H
#define HG_LVN_CONFIG_H


#if defined(_WIN32) || defined(WIN32)
    #ifndef LVN_PLATFORM_WINDOWS
        #define LVN_PLATFORM_WINDOWS
    #endif
#elif defined(__linux__)
    #ifndef LVN_PLATFORM_LINUX
        #define LVN_PLATFORM_LINUX
    #endif
    #include <assert.h> /* assert */
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


#ifdef LVN_PLATFORM_LINUX
    #define LVN_ASSERT(x, ...) assert(x && __VA_ARGS__)
#endif


typedef enum LvnResult
{
    Lvn_Result_Success =  0,
    Lvn_Result_Failure = -1,
} LvnResult;



#endif /* !HG_LVN_CONFIG_H */
