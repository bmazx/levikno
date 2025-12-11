#ifndef HG_LEVIKNO_H
#define HG_LEVIKNO_H


#include "lvn_config.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>


#ifdef LVN_ENABLE_LOGGING
    #define LVN_LOG_TRACE(logger, ...) lvnLogMessageTrace(logger, __VA_ARGS__)
    #define LVN_LOG_DEBUG(logger, ...) lvnLogMessageDebug(logger, __VA_ARGS__)
    #define LVN_LOG_INFO(logger, ...) lvnLogMessageInfo(logger, __VA_ARGS__)
    #define LVN_LOG_WARN(logger, ...) lvnLogMessageWarn(logger, __VA_ARGS__)
    #define LVN_LOG_ERROR(logger, ...) lvnLogMessageError(logger, __VA_ARGS__)
    #define LVN_LOG_FATAL(logger, ...) lvnLogMessageFatal(logger, __VA_ARGS__)
#else
    #define LVN_LOG_TRACE(logger, ...)
    #define LVN_LOG_DEBUG(logger, ...)
    #define LVN_LOG_INFO(logger, ...)
    #define LVN_LOG_WARN(logger, ...)
    #define LVN_LOG_ERROR(logger, ...)
    #define LVN_LOG_FATAL(logger, ...)
#endif

#define LVN_LOG_COLOR_TRACE                     "\x1b[0;37m"
#define LVN_LOG_COLOR_DEBUG                     "\x1b[0;34m"
#define LVN_LOG_COLOR_INFO                      "\x1b[0;32m"
#define LVN_LOG_COLOR_WARN                      "\x1b[1;33m"
#define LVN_LOG_COLOR_ERROR                     "\x1b[1;31m"
#define LVN_LOG_COLOR_FATAL                     "\x1b[1;37;41m"
#define LVN_LOG_COLOR_RESET                     "\x1b[0m"


typedef enum LvnLogLevel
{
    Lvn_LogLevel_None = 0,
    Lvn_LogLevel_Trace,
    Lvn_LogLevel_Debug,
    Lvn_LogLevel_Info,
    Lvn_LogLevel_Warn,
    Lvn_LogLevel_Error,
    Lvn_LogLevel_Fatal,
} LvnLogLevel;


typedef struct LvnContext LvnContext;
typedef struct LvnLogger LvnLogger;


typedef struct LvnSink
{
    void (*logFunc)(const char*);
} LvnSink;

typedef struct LvnLogMessage
{
    const char* msg;
    const char* loggerName;
    LvnLogLevel level;
    size_t timeEpoch;
} LvnLogMessage;

typedef struct LvnLogPattern
{
    char symbol;
    char* (*func)(const LvnLogMessage*);
} LvnLogPattern;

typedef struct LvnLoggerCreateInfo
{
    const char* name;
    const char* format;
    LvnLogLevel level;
    const LvnSink* pSinks;
    uint32_t sinkCount;
} LvnLoggerCreateInfo;

typedef struct LvnContextCreateInfo
{
    struct
    {
        bool enableLogging;                // enable logging for the core logger
        const char* coreLogFormat;         // the log format for the core logger
        LvnLogLevel coreLogLevel;          // the log level for the core logger
        const LvnSink* pCoreSinks;         // array of output sinks for the core logger
        uint32_t coreSinkCount;            // number of output sinks in pCoreSinks
    } logging;
} LvnContextCreateInfo;



typedef void* (*LvnMemAllocFn)(size_t, void*);
typedef void  (*LvnMemFreeFn)(void*, void*);
typedef void* (*LvnMemReallocFn)(void*, size_t, void*);


#ifdef __cplusplus
extern "C" {
#endif

LVN_API LvnResult               lvnCreateContext(LvnContext** ctx, const LvnContextCreateInfo* createInfo);                                         // create the core context
LVN_API void                    lvnDestroyContext(LvnContext* ctx);                                                                                 // destroy the core context

LVN_API LvnResult               lvnSetMemAllocCallbacks(LvnMemAllocFn allocFn, LvnMemFreeFn freeFn, LvnMemReallocFn reallocFn, void* userData);     // set memory allocation callback functions; all callback functions must be set, userData can be null

LVN_API int                     lvnDateGetYear(void);                                      // get the year number (eg. 2025)
LVN_API int                     lvnDateGetYear02d(void);                                   // get the last two digits of the year number (eg. 25)
LVN_API int                     lvnDateGetMonth(void);                                     // get the month number (1...12)
LVN_API int                     lvnDateGetDay(void);                                       // get the date number (1...31)
LVN_API int                     lvnDateGetHour(void);                                      // get the hour of the current day in 24 hour format (0...24)
LVN_API int                     lvnDateGetHour12(void);                                    // get the hour of the current day in 12 hour format (0...12)
LVN_API int                     lvnDateGetMinute(void);                                    // get the minute of the current day (0...60)
LVN_API int                     lvnDateGetSecond(void);                                    // get the second of the current dat (0...60)
LVN_API size_t                  lvnDateGetSecondsSinceEpoch(void);                         // get the time in seconds since 00:00:00 UTC 1 January 1970

LVN_API const char*             lvnDateGetMonthName(void);                                 // get the current month name (eg. January, April)
LVN_API const char*             lvnDateGetMonthNameShort(void);                            // get the current month shortened name (eg. Jan, Apr)
LVN_API const char*             lvnDateGetDayName(void);                                   // get the current day name in the week (eg. Monday, Friday)
LVN_API const char*             lvnDateGetDayNameShort(void);                              // get the current day shortened name in the week (eg. Mon, Fri)
LVN_API const char*             lvnDateGetTimeMeridiem(void);                              // get the time meridiem of the current day (eg. AM, PM)
LVN_API const char*             lvnDateGetTimeMeridiemLower(void);                         // get the time meridiem of the current day in lower case (eg. am, pm)

LVN_API LvnLogger*              lvnCtxGetCoreLogger(LvnContext* ctx);                                         // get the core logger from the context
LVN_API void                    lvnCtxEnableLogging(LvnContext* ctx, bool enable);                            // enable or disable logging for all loggers created from the context
LVN_API void                    lvnCtxAddLogPatterns(LvnContext* ctx, const LvnLogPattern* pLogPatterns, uint32_t logPatternCount); // add log patterns to the context
LVN_API void                    lvnLogEnableLogging(LvnLogger* logger, bool enable);                          // enable or disable logging for the logger
LVN_API const char*             lvnLogGetANSIcodeColor(LvnLogLevel level);                                    // get the ANSI color code string of the log level
LVN_API void                    lvnLogOutputMessage(const LvnLogger* logger, LvnLogMessage* msg);             // prints the log message
LVN_API uint32_t                lvnLogFormatMessage(const LvnLogger* logger, char* dst, uint32_t length, LvnLogLevel level, const char* msg); // formats the log message into the log pattern set by the logger, returns the length of the formatted log message
LVN_API uint32_t                lvnLogFormatMessageArgs(const LvnLogger* logger, char* dst, uint32_t length, LvnLogLevel level, const char* fmt, ...); // formats the log message with args into the log pattern set by the logger, returns the length of the formatted log message
LVN_API void                    lvnLogParseLogPatternFormat(LvnLogger* logger, const char* fmt);              // update the logger's log pattern format with the new format string
LVN_API void                    lvnLogMessage(const LvnLogger* logger, LvnLogLevel level, const char* msg);   // log message with given log level
LVN_API bool                    lvnLogCheckLevel(const LvnLogger* logger, LvnLogLevel level);                 // check level witht the logger, returns true if larger or equal to the level of the logger, returns false otherwise
LVN_API void                    lvnLogSetLevel(LvnLogger* logger, LvnLogLevel level);                         // sets the log level of logger, will only print messages with set log level and higher
LVN_API void                    lvnLogMessageTrace(const LvnLogger* logger, const char* fmt, ...);            // log message with level trace; ANSI code "\x1b[0;37m"
LVN_API void                    lvnLogMessageDebug(const LvnLogger* logger, const char* fmt, ...);            // log message with level debug; ANSI code "\x1b[0;34m"
LVN_API void                    lvnLogMessageInfo(const LvnLogger* logger, const char* fmt, ...);             // log message with level info;  ANSI code "\x1b[0;32m"
LVN_API void                    lvnLogMessageWarn(const LvnLogger* logger, const char* fmt, ...);             // log message with level warn;  ANSI code "\x1b[1;33m"
LVN_API void                    lvnLogMessageError(const LvnLogger* logger, const char* fmt, ...);            // log message with level error; ANSI code "\x1b[1;31m"
LVN_API void                    lvnLogMessageFatal(const LvnLogger* logger, const char* fmt, ...);            // log message with level fatal; ANSI code "\x1b[1;37;41m"
LVN_API char*                   lvnLogCreateOneShotStrMsg(const char* str);

LVN_API LvnResult               lvnCreateLogger(const LvnContext* ctx, LvnLogger** logger, const LvnLoggerCreateInfo* createInfo);   // create logger object
LVN_API void                    lvnDestroyLogger(LvnLogger* logger);                                                                 // destroy logger object


#ifdef __cplusplus
}
#endif


#endif // !HG_LEVIKNO_H
