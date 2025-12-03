#include "levikno_internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define LVN_DEFAULT_LOG_PATTERN "[%Y-%m-%d] [%T] [%#%l%^] %n: %v%$"


static void*   mallocWrapper(size_t size, void* userData)               { (void)userData; return malloc(size); }
static void    freeWrapper(void* ptr, void* userData)                   { (void)userData; free(ptr); }
static void*   reallocWrapper(void* ptr, size_t size, void* userData)   { (void)userData; return realloc(ptr, size); }
static LvnMemAllocFn s_LvnMemAllocFnCallback = mallocWrapper;
static LvnMemFreeFn s_LvnMemFreeFnCallback = freeWrapper;
static LvnMemReallocFn s_LvnMemReallocFnCallback = reallocWrapper;
static void* s_LvnMemUserData = NULL;

static const char*    lvn_getLogLevelName(LvnLogLevel level);
static const char*    lvn_getLogLevelColor(LvnLogLevel level);
static char*          lvn_logPatternStrNewLine(LvnLogMessage* msg);
static char*          lvn_logPatternStrLoggerName(LvnLogMessage* msg);
static char*          lvn_logPatternStrLogLevelName(LvnLogMessage* msg);
static char*          lvn_logPatternStrLogLevelColor(LvnLogMessage* msg);
static char*          lvn_logPatternStrLogLevelReset(LvnLogMessage* msg);
static char*          lvn_logPatternStrMsg(LvnLogMessage* msg);
static char*          lvn_logPatternStrPercent(LvnLogMessage* msg);
static char*          lvn_logPatternStrDateMonthName(LvnLogMessage* msg);
static char*          lvn_logPatternStrDateMonthNameShort(LvnLogMessage* msg);
static char*          lvn_logPatternStrDateDayName(LvnLogMessage* msg);
static char*          lvn_logPatternStrDateDayNameShort(LvnLogMessage* msg);
static char*          lvn_logPatternStrDateTimeMeridiem(LvnLogMessage* msg);
static char*          lvn_logPatternStrDateTimeMeridiemLower(LvnLogMessage* msg);
static char*          lvn_logPatternStrDateTimeHHMMSS(LvnLogMessage* msg);
static char*          lvn_logPatternStrDateTimeHHMMSS12(LvnLogMessage* msg);
static char*          lvn_logPatternStrDateYear(LvnLogMessage* msg);
static char*          lvn_logPatternStrDateYear02d(LvnLogMessage* msg);
static char*          lvn_logPatternStrDateMonth(LvnLogMessage* msg);
static char*          lvn_logPatternStrDateDay(LvnLogMessage* msg);
static char*          lvn_logPatternStrDateHour(LvnLogMessage* msg);
static char*          lvn_logPatternStrDateHour12(LvnLogMessage* msg);
static char*          lvn_logPatternStrDateMinute(LvnLogMessage* msg);
static char*          lvn_logPatternStrDateSecond(LvnLogMessage* msg);
static LvnLogPattern* lvn_logParseFormat(const LvnContext* ctx, const char* fmt, uint32_t* logPatternCount);

static const char* lvn_getLogLevelName(LvnLogLevel level)
{
    switch (level)
    {
        case Lvn_LogLevel_None:     { return "none"; }
        case Lvn_LogLevel_Trace:    { return "trace"; }
        case Lvn_LogLevel_Debug:    { return "debug"; }
        case Lvn_LogLevel_Info:     { return "info"; }
        case Lvn_LogLevel_Warn:     { return "warn"; }
        case Lvn_LogLevel_Error:    { return "error"; }
        case Lvn_LogLevel_Fatal:    { return "fatal"; }
    }

    return NULL;
}

static const char* lvn_getLogLevelColor(LvnLogLevel level)
{
    switch (level)
    {
        case Lvn_LogLevel_None:     { return LVN_LOG_COLOR_RESET; }
        case Lvn_LogLevel_Trace:    { return LVN_LOG_COLOR_TRACE; }
        case Lvn_LogLevel_Debug:    { return LVN_LOG_COLOR_DEBUG; }
        case Lvn_LogLevel_Info:     { return LVN_LOG_COLOR_INFO; }
        case Lvn_LogLevel_Warn:     { return LVN_LOG_COLOR_WARN; }
        case Lvn_LogLevel_Error:    { return LVN_LOG_COLOR_ERROR; }
        case Lvn_LogLevel_Fatal:    { return LVN_LOG_COLOR_FATAL; }
    }

    return NULL;
}

static char* lvn_logPatternStrNewLine(LvnLogMessage* msg) { return lvn_strdup("\n"); }
static char* lvn_logPatternStrLoggerName(LvnLogMessage* msg) { return lvn_strdup(msg->loggerName); }
static char* lvn_logPatternStrLogLevelName(LvnLogMessage* msg) { return lvn_strdup(lvn_getLogLevelName(msg->level)); }
static char* lvn_logPatternStrLogLevelColor(LvnLogMessage* msg) { return lvn_strdup(lvn_getLogLevelColor(msg->level)); }
static char* lvn_logPatternStrLogLevelReset(LvnLogMessage* msg) { return lvn_strdup(LVN_LOG_COLOR_RESET); }
static char* lvn_logPatternStrMsg(LvnLogMessage* msg) { return lvn_strdup(msg->msg); }
static char* lvn_logPatternStrPercent(LvnLogMessage* msg) { return lvn_strdup("%"); }
static char* lvn_logPatternStrDateMonthName(LvnLogMessage* msg) { return lvn_strdup(lvnDateGetMonthName()); }
static char* lvn_logPatternStrDateMonthNameShort(LvnLogMessage* msg) { return lvn_strdup(lvnDateGetMonthNameShort()); }
static char* lvn_logPatternStrDateDayName(LvnLogMessage* msg) { return lvn_strdup(lvnDateGetDayName()); }
static char* lvn_logPatternStrDateDayNameShort(LvnLogMessage* msg) { return lvn_strdup(lvnDateGetDayNameShort()); }
static char* lvn_logPatternStrDateTimeMeridiem(LvnLogMessage* msg) { return lvn_strdup(lvnDateGetTimeMeridiem()); }
static char* lvn_logPatternStrDateTimeMeridiemLower(LvnLogMessage* msg) { return lvn_strdup(lvnDateGetTimeMeridiemLower()); }

static char* lvn_logPatternStrDateTimeHHMMSS(LvnLogMessage* msg)
{
    char buff[9];
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    snprintf(buff, 9, "%02d:%02d:%02d", tm.tm_hour, tm.tm_min, tm.tm_sec);
    return lvn_strdup(buff);
}

static char* lvn_logPatternStrDateTimeHHMMSS12(LvnLogMessage* msg)
{
    // make buff size larger to supress gcc truncate warning
    char buff[16];
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    snprintf(buff, 16, "%02d:%02d:%02d", ((tm.tm_hour + 11) % 12) + 1, tm.tm_min, tm.tm_sec);
    return lvn_strdup(buff);
}

static char* lvn_logPatternStrDateYear(LvnLogMessage* msg)
{
    char buff[5];
    snprintf(buff, 5, "%d", lvnDateGetYear());
    return lvn_strdup(buff);
}

static char* lvn_logPatternStrDateYear02d(LvnLogMessage* msg)
{
    char buff[3];
    snprintf(buff, 3, "%d", lvnDateGetYear02d());
    return lvn_strdup(buff);
}

static char* lvn_logPatternStrDateMonth(LvnLogMessage* msg)
{
    char buff[3];
    snprintf(buff, 3, "%02d", lvnDateGetMonth());
    return lvn_strdup(buff);
}

static char* lvn_logPatternStrDateDay(LvnLogMessage* msg)
{
    char buff[3];
    snprintf(buff, 3, "%02d", lvnDateGetDay());
    return lvn_strdup(buff);
}

static char* lvn_logPatternStrDateHour(LvnLogMessage* msg)
{
    char buff[3];
    snprintf(buff, 3, "%02d", lvnDateGetHour());
    return lvn_strdup(buff);
}

static char* lvn_logPatternStrDateHour12(LvnLogMessage* msg)
{
    char buff[3];
    snprintf(buff, 3, "%02d", lvnDateGetHour12());
    return lvn_strdup(buff);
}

static char* lvn_logPatternStrDateMinute(LvnLogMessage* msg)
{
    char buff[3];
    snprintf(buff, 3, "%02d", lvnDateGetMinute());
    return lvn_strdup(buff);
}

static char* lvn_logPatternStrDateSecond(LvnLogMessage* msg)
{
    char buff[3];
    snprintf(buff, 3, "%02d", lvnDateGetSecond());
    return lvn_strdup(buff);
}

static LvnLogPattern s_LvnLogPatterns[] =
{
    { .symbol = '$', .func = lvn_logPatternStrNewLine },
    { .symbol = 'n', .func = lvn_logPatternStrLoggerName },
    { .symbol = 'l', .func = lvn_logPatternStrLogLevelName },
    { .symbol = '#', .func = lvn_logPatternStrLogLevelColor },
    { .symbol = '^', .func = lvn_logPatternStrLogLevelReset },
    { .symbol = 'v', .func = lvn_logPatternStrMsg },
    { .symbol = '%', .func = lvn_logPatternStrPercent },
    { .symbol = 'T', .func = lvn_logPatternStrDateTimeHHMMSS },
    { .symbol = 't', .func = lvn_logPatternStrDateTimeHHMMSS12 },
    { .symbol = 'Y', .func = lvn_logPatternStrDateYear },
    { .symbol = 'y', .func = lvn_logPatternStrDateYear02d },
    { .symbol = 'm', .func = lvn_logPatternStrDateMonth },
    { .symbol = 'B', .func = lvn_logPatternStrDateMonthName },
    { .symbol = 'b', .func = lvn_logPatternStrDateMonthNameShort },
    { .symbol = 'd', .func = lvn_logPatternStrDateDay },
    { .symbol = 'A', .func = lvn_logPatternStrDateDayName },
    { .symbol = 'a', .func = lvn_logPatternStrDateDayNameShort },
    { .symbol = 'H', .func = lvn_logPatternStrDateHour },
    { .symbol = 'h', .func = lvn_logPatternStrDateHour12 },
    { .symbol = 'M', .func = lvn_logPatternStrDateMinute },
    { .symbol = 'S', .func = lvn_logPatternStrDateSecond },
    { .symbol = 'P', .func = lvn_logPatternStrDateTimeMeridiem },
    { .symbol = 'p', .func = lvn_logPatternStrDateTimeMeridiemLower },
};

static LvnLogPattern* lvn_logParseFormat(const LvnContext* ctx, const char* fmt, uint32_t* logPatternCount)
{
    LVN_ASSERT(ctx && fmt && logPatternCount && fmt[0] != '\0', "ctx and fmt and logPatternCount cannot not be null or empty");

    LvnLogPattern* patterns = NULL;
    uint32_t patternCount = 0;

    for (uint32_t i = 0; i < strlen(fmt) - 1; i++)
    {
        if (fmt[i] != '%') // other characters in format
        {
            LvnLogPattern pattern = { .symbol = fmt[i], .func = NULL };
            patterns = lvn_realloc(patterns, ++patternCount * sizeof(LvnLogPattern));
            memcpy(&patterns[patternCount - 1], &pattern, sizeof(LvnLogPattern));
            continue;
        }

        // find pattern with matching symbol
        bool skip = false;
        for (uint32_t j = 0; j < LVN_ARRAY_LEN(s_LvnLogPatterns); j++)
        {
            if (fmt[i + 1] == s_LvnLogPatterns[j].symbol)
            {
                patterns = lvn_realloc(patterns, ++patternCount * sizeof(LvnLogPattern));
                memcpy(&patterns[patternCount - 1], &s_LvnLogPatterns[j], sizeof(LvnLogPattern));
                skip = true;
                break;
            }
        }

        if (skip)
        {
            i++;
            continue;
        }

        // find and add user defined patterns
        for (uint32_t j = 0; j < ctx->userLogPatternCount; j++)
        {
            if (fmt[i + 1] != ctx->pUserLogPatterns[j].symbol)
                continue;

            patterns = lvn_realloc(patterns, ++patternCount * sizeof(LvnLogPattern));
            memcpy(&patterns[patternCount - 1], &ctx->pUserLogPatterns[j], sizeof(LvnLogPattern));
        }

        i++; // incramant past symbol on next character in format
    }

    *logPatternCount = patternCount;

    return patterns;
}

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
        ctxPtr->coreLogger.loggerName = lvn_strdup(createInfo->appName);
    else
        ctxPtr->coreLogger.loggerName = lvn_strdup("CORE");

    if (createInfo && createInfo->logging.coreLogFormat)
        ctxPtr->coreLogger.logPatternFormat = lvn_strdup(createInfo->logging.coreLogFormat);
    else
        ctxPtr->coreLogger.logPatternFormat = lvn_strdup(LVN_DEFAULT_LOG_PATTERN);

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

    ctxPtr->coreLogger.pLogPatterns = lvn_logParseFormat(ctxPtr, LVN_DEFAULT_LOG_PATTERN, &ctxPtr->coreLogger.logPatternCount);

    return Lvn_Result_Success;
}

void lvnDestroyContext(LvnContext* ctx)
{
    if (!ctx)
        return;

    lvn_free(ctx->coreLogger.loggerName);
    lvn_free(ctx->coreLogger.logPatternFormat);
    lvn_free(ctx->coreLogger.pSinks);
    lvn_free(ctx->coreLogger.pLogPatterns);

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

const char* lvnDateGetDayName(void)
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    return s_LvnWeekDayName[tm.tm_wday];
}

const char* lvnDateGetDayNameShort(void)
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
    return lvn_strdup(str);
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

char* lvn_strdup(const char* str)
{
    const size_t length = strlen(str) + 1;
    char* result = (char*) lvn_calloc(length);
    memcpy(result, str, length);
    return result;
}
