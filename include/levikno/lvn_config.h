#ifndef HG_LVN_CONFIG_H
#define HG_LVN_CONFIG_H


// platform
#if defined(_WIN32) || defined(WIN32)
    #ifndef LVN_PLATFORM_WINDOWS
        #define LVN_PLATFORM_WINDOWS
    #endif
#elif defined(__linux__)
    #ifndef LVN_PLATFORM_LINUX
        #define LVN_PLATFORM_LINUX
    #endif
#endif

#if defined(__unix__)
    #ifndef LVN_PLATFORM_UNIX
        #define LVN_PLATFORM_UNIX
    #endif
#endif

// dll
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

// debug
#ifndef LVN_CONFIG_DEBUG
    #ifndef NDEBUG
        #define LVN_CONFIG_DEBUG
    #endif
#endif

// asserts
#ifdef LVN_CONFIG_DEBUG
    #define LVN_ENABLE_ASSERTS
#endif

#if defined(LVN_DISABLE_ASSERTS)
    #define LVN_ASSERT(x, ...)
#elif defined(LVN_ENABLE_ASSERTS)
    #include <assert.h>
    #define LVN_ASSERT(x, ...) assert(x && __VA_ARGS__)
#endif

// logging
#ifndef LVN_DISABLE_LOGGING
    #define LVN_ENABLE_LOGGING
#endif

// misc
#define LVN_ARRAY_LEN(x) (sizeof(x) / sizeof(x[0]))


#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef uint32_t LvnFlags;

typedef enum LvnResult
{
    Lvn_Result_Success =  0,
    Lvn_Result_Failure = -1,
} LvnResult;



#endif // !HG_LVN_CONFIG_H
