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

#define LVN_ARRAY_LEN(x) (sizeof(x) / sizeof(x[0]))


// log defines
// ------------------------------------------------------------

/*
*   color          | fg | bg
* -----------------+----+----
*   black          | 30 | 40
*   red            | 31 | 41
*   green          | 32 | 42
*   yellow         | 33 | 43
*   blue           | 34 | 44
*   magenta        | 35 | 45
*   cyan           | 36 | 46
*   white          | 37 | 47
*   bright black   | 90 | 100
*   bright red     | 91 | 101
*   bright green   | 92 | 102
*   bright yellow  | 93 | 103
*   bright blue    | 94 | 104
*   bright magenta | 95 | 105
*   bright cyan    | 96 | 106
*   bright white   | 97 | 107
*
*
*   reset             0
*   bold/bright       1
*   underline         4
*   inverse           7
*   bold/bright off  21
*   underline off    24
*   inverse off      27
*
*
*   Log Colors:
*   TRACE           \x1b[0;37m
*   DEBUG           \x1b[0;34m
*   INFO            \x1b[0;32m
*   WARN            \x1b[1;33m
*   ERROR           \x1b[1;31m
*   FATAL           \x1b[1;37;41m
*
*/

#define LVN_LOG_COLOR_TRACE                     "\x1b[0;37m"
#define LVN_LOG_COLOR_DEBUG                     "\x1b[0;34m"
#define LVN_LOG_COLOR_INFO                      "\x1b[0;32m"
#define LVN_LOG_COLOR_WARN                      "\x1b[1;33m"
#define LVN_LOG_COLOR_ERROR                     "\x1b[1;31m"
#define LVN_LOG_COLOR_FATAL                     "\x1b[1;37;41m"
#define LVN_LOG_COLOR_RESET                     "\x1b[0m"

typedef enum LvnResult
{
    Lvn_Result_Success =  0,
    Lvn_Result_Failure = -1,
} LvnResult;



#endif /* !HG_LVN_CONFIG_H */
