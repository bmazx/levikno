#include "levikno_internal.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>

#define LVN_DEFAULT_LOG_PATTERN "[%Y-%m-%d] [%T] [%#%l%^] %n: %v%$"
#define LVN_ABORT(rc) exit(rc)


static void*   mallocWrapper(size_t size, void* userData)               { (void)userData; return malloc(size); }
static void    freeWrapper(void* ptr, void* userData)                   { (void)userData; free(ptr); }
static void*   reallocWrapper(void* ptr, size_t size, void* userData)   { (void)userData; return realloc(ptr, size); }
static LvnMemAllocFn s_LvnMemAllocFnCallback = mallocWrapper;
static LvnMemFreeFn s_LvnMemFreeFnCallback = freeWrapper;
static LvnMemReallocFn s_LvnMemReallocFnCallback = reallocWrapper;
static void* s_LvnMemUserData = NULL;


LvnResult lvnCreateContext(LvnContext** ctx, LvnContextCreateInfo* createInfo)
{
    if (!ctx)
        return Lvn_Result_Failure;

    *ctx = (LvnContext*) lvn_calloc(sizeof(LvnContext));

    if (!*ctx)
        return Lvn_Result_Failure;

    memset(*ctx, 0, sizeof(LvnContext));
    LvnContext* ctxPtr = *ctx;

    if (createInfo)
    {
        ctxPtr->enableLogging = createInfo->logging.enableLogging;
        ctxPtr->enableCoreLogging = createInfo->logging.enableCoreLogging;
    }
    else
    {
        ctxPtr->enableLogging = true;
        ctxPtr->enableCoreLogging = true;
    }

    if (createInfo && createInfo->appName)
        ctxPtr->coreLogger.loggerName = lvn_strcon(createInfo->appName);
    else
        ctxPtr->coreLogger.loggerName = lvn_strcon("CORE");

    if (createInfo && createInfo->logging.coreLogFormat)
        ctxPtr->coreLogger.logPatternFormat = lvn_strcon(createInfo->logging.coreLogFormat);
    else
        ctxPtr->coreLogger.logPatternFormat = lvn_strcon(LVN_DEFAULT_LOG_PATTERN);

    if (createInfo && createInfo->logging.coreLogLevel)
        ctxPtr->coreLogger.logLevel = createInfo->logging.coreLogLevel;
    else
        ctxPtr->coreLogger.logLevel = Lvn_LogLevel_None;

    if (createInfo && createInfo->logging.pCoreSinks)
    {
        ctxPtr->coreLogger.pSinks = (LvnSink*) lvn_calloc(sizeof(LvnSink) * createInfo->logging.coreSinkCount);
        memcpy(ctxPtr->coreLogger.pSinks, createInfo->logging.pCoreSinks, sizeof(LvnSink) * createInfo->logging.coreSinkCount);
        ctxPtr->coreLogger.sinkCount = createInfo->logging.coreSinkCount;
    }
    else
    {
        ctxPtr->coreLogger.pSinks = (LvnSink*) lvn_calloc(sizeof(LvnSink));
        ctxPtr->coreLogger.pSinks->logFunc = NULL;
        ctxPtr->coreLogger.sinkCount = 1;
    }

    return Lvn_Result_Success;
}

void lvnDestroyContext(LvnContext* ctx)
{
    if (!ctx)
        return;

    lvn_free(ctx->coreLogger.loggerName);
    lvn_free(ctx->coreLogger.logPatternFormat);
    lvn_free(ctx->coreLogger.pLogPatterns);
    lvn_free(ctx->coreLogger.pSinks);

    lvn_free(ctx);
}

LvnResult lvnSetMemAllocCallbacks(LvnMemAllocFn allocFn, LvnMemFreeFn freeFn, LvnMemReallocFn reallocFn, void* userData)
{
    if (!allocFn || ! !freeFn || !reallocFn)
        return Lvn_Result_Failure;

    s_LvnMemAllocFnCallback = allocFn;
    s_LvnMemFreeFnCallback = freeFn;
    s_LvnMemReallocFnCallback = reallocFn;
    s_LvnMemUserData = userData;

    return Lvn_Result_Success;
}

int lvnDateGetYear(void)
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    return tm.tm_year + 1900;
}

int lvnDateGetYear02d(void)
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    return (tm.tm_year + 1900) % 100;
}

int lvnDateGetMonth(void)
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    return tm.tm_mon + 1;
}

int lvnDateGetDay(void)
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    return tm.tm_mday;
}

int lvnDateGetHour(void)
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    return tm.tm_hour;
}

int lvnDateGetHour12(void)
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    return ((tm.tm_hour + 11) % 12) + 1;
}

int lvnDateGetMinute(void)
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    return tm.tm_min;
}

int lvnDateGetSecond(void)
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    return tm.tm_sec;
}

long long lvnDateGetSecondsSinceEpoch(void)
{
    return time(NULL);
}

static const char* const s_LvnMonthName[12] = { "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" };
static const char* const s_LvnMonthNameShort[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
static const char* const s_LvnWeekDayName[7] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
static const char* const s_LvnWeekDayNameShort[7] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

const char* lvnDateGetMonthName(void)
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    return s_LvnMonthName[tm.tm_mon];
}

const char* lvnDateGetMonthNameShort(void)
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    return s_LvnMonthNameShort[tm.tm_mon];
}

const char* lvnDateGetWeekDayName(void)
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    return s_LvnWeekDayName[tm.tm_wday];
}

const char* lvnDateGetWeekDayNameShort(void)
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    return s_LvnWeekDayNameShort[tm.tm_wday];
}

const char* lvnDateGetTimeMeridiem(void)
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    if (tm.tm_hour < 12)
        return "AM";
    else
        return "PM";
}

const char* lvnDateGetTimeMeridiemLower(void)
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    if (tm.tm_hour < 12)
        return "am";
    else
        return "pm";
}

char* lvnLogCreateOneShotStrMsg(const char* str)
{
    return lvn_strcon(str);
}

void* lvn_calloc(size_t size)
{
    void* result = s_LvnMemAllocFnCallback(size, s_LvnMemUserData);
    if (!result) { return NULL; }
    memset(result, 0, size);
    return result;
}

void lvn_free(void* ptr)
{
    s_LvnMemFreeFnCallback(ptr, s_LvnMemUserData);
}

void* lvn_realloc(void* ptr, size_t size)
{
    return s_LvnMemReallocFnCallback(ptr, size, s_LvnMemUserData);
}

char* lvn_strcon(const char* str)
{
    const size_t length = strlen(str) + 1;
    char* result = (char*) lvn_calloc(length);
    memcpy(result, str, length);
    return result;
}
