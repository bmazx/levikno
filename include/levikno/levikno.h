#ifndef HG_LEVIKNO_H
#define HG_LEVIKNO_H


#include "lvn_config.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>


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
    char* (*func)(LvnLogMessage*);
} LvnLogPattern;

typedef struct LvnLoggerCreateContext
{
    const char* name;
    const char* format;
    LvnLogLevel level;
    const LvnSink* pSinks;
    uint32_t sinkCount;
} LvnLoggerCreateContext;

typedef struct LvnContextCreateInfo
{
    const char* appName;

    struct
    {
        bool enableLogging;
        bool enableCoreLogging;
        const char* coreLogFormat;
        LvnLogLevel coreLogLevel;
        const LvnSink* pCoreSinks;
        uint32_t coreSinkCount;
    } logging;
} LvnContextCreateInfo;



typedef void* (*LvnMemAllocFn)(size_t, void*);
typedef void  (*LvnMemFreeFn)(void*, void*);
typedef void* (*LvnMemReallocFn)(void*, size_t, void*);


#ifdef __cplusplus
extern "C" {
#endif

LVN_API LvnResult               lvnCreateContext(LvnContext** ctx, LvnContextCreateInfo* createInfo);                                               // create the core context
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
LVN_API long long               lvnDateGetSecondsSinceEpoch(void);                         // get the time in seconds since 00::00:00 UTC 1 January 1970

LVN_API const char*             lvnDateGetMonthName(void);                                 // get the current month name (eg. January, April)
LVN_API const char*             lvnDateGetMonthNameShort(void);                            // get the current month shortened name (eg. Jan, Apr)
LVN_API const char*             lvnDateGetWeekDayName(void);                               // get the current day name in the week (eg. Monday, Friday)
LVN_API const char*             lvnDateGetWeekDayNameShort(void);                          // get the current day shortened name in the week (eg. Mon, Fri)
LVN_API const char*             lvnDateGetTimeMeridiem(void);                              // get the time meridiem of the current day (eg. AM, PM)
LVN_API const char*             lvnDateGetTimeMeridiemLower(void);                         // get the time meridiem of the current day in lower case (eg. am, pm)
LVN_API char*                   lvnLogCreateOneShotStrMsg(const char* str);

#ifdef __cplusplus
}
#endif


#ifdef __cplusplus

namespace lvn
{

}

#endif /* !__cplusplus */


#endif /* !HG_LEVIKNO_H */
