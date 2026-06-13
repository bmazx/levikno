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
    #define LVN_ASSERT(x, str) assert(x)
#endif

// boolean type
#if (defined(__STDC__) && __STDC_VERSION__ >= 199901L)
    #include <stdbool.h>
#elif !defined(__cplusplus) && !defined(bool)
    typedef enum bool { false = 0, true = !false } bool;
#endif

// memory
#ifdef LVN_CONFIG_DEBUG
    #ifndef LVN_DEBUG_ALLOC_VALUE
        #define LVN_DEBUG_ALLOC_VALUE (0xCD)
    #endif
    #ifndef LVN_DEBUG_FREE_VALUE
        #define LVN_DEBUG_FREE_VALUE (0xDD)
    #endif
#endif

// alignment
#if defined(__cplusplus) || (defined(__STDC__) && (__STDC_VERSION__ >= 202311L))
    #define LVN_ALIGNOF(T) alignof(T)
    typedef max_align_t lvn_max_align_t;
#elif defined(__STDC__) && (__STDC_VERSION__ >= 201112L)
    #define LVN_ALIGNOF(T) _Alignof(T)
    typedef max_align_t lvn_max_align_t;
#else
    #define LVN_ALIGNOF(T) ((size_t)offsetof(struct { char c; T x; }, x))
    typedef union {
        long long ll;
        long double ld;
        void* p;
    } lvn_max_align_t;
#endif

#define LVN_DEFAULT_ALIGN (LVN_ALIGNOF(lvn_max_align_t))
#define LVN_ALIGN_UP(x, a) (((x) + ((a) - 1)) & ~((a) - 1))
#define LVN_ALIGN_DOWN(x, a) ((x) & ~((a) - 1))
#define LVN_ALIGNED(x, a) (((x) & ((a) - 1)) == 0)

// logging
#ifndef LVN_DISABLE_LOGGING
    #define LVN_ENABLE_LOGGING
#endif

// misc
#define LVN_ARRAY_LEN(x) (sizeof(x) / sizeof(x[0]))

#include <stdint.h>
#include <stddef.h>

typedef uint32_t LvnFlags;

typedef enum LvnResult
{
    Lvn_Result_Success =  0,
    Lvn_Result_Failure = -1,
    Lvn_Result_OutOfMemory = -2,
    Lvn_Result_OutOfDate = 1,
    Lvn_Result_TimeOut = 2,
} LvnResult;



#endif // !HG_LVN_CONFIG_H
