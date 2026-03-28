#include "levikno_internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>

// ansi color terminal logging support on windows
#ifdef LVN_PLATFORM_WINDOWS
#include <windows.h>
#endif

#define LVN_DEFAULT_LOG_PATTERN "[%Y-%m-%d] [%T] [%#%l%^] %n: %v%$"
#define LVN_DEFAULT_APP_NAME "levikno"

// memory
static void*   mallocWrapper(size_t size, void* userData)               { (void)userData; return malloc(size); }
static void    freeWrapper(void* ptr, void* userData)                   { (void)userData; free(ptr); }
static void*   reallocWrapper(void* ptr, size_t size, void* userData)   { (void)userData; return realloc(ptr, size); }
static LvnMemAllocFn s_LvnMemAllocFnCallback = mallocWrapper;
static LvnMemFreeFn s_LvnMemFreeFnCallback = freeWrapper;
static LvnMemReallocFn s_LvnMemReallocFnCallback = reallocWrapper;
static void* s_LvnMemUserData = NULL;

// logging
static void    printWrapper(const char* msg) { printf("%s", msg); }

// utils
static const char*    lvn_getLogLevelName(LvnLogLevel level);
static const char*    lvn_getLogLevelColor(LvnLogLevel level);
static char*          lvn_logPatternStrNewLine(const LvnLogMessage* msg);
static char*          lvn_logPatternStrLoggerName(const LvnLogMessage* msg);
static char*          lvn_logPatternStrLogLevelName(const LvnLogMessage* msg);
static char*          lvn_logPatternStrLogLevelColor(const LvnLogMessage* msg);
static char*          lvn_logPatternStrLogLevelReset(const LvnLogMessage* msg);
static char*          lvn_logPatternStrMsg(const LvnLogMessage* msg);
static char*          lvn_logPatternStrPercent(const LvnLogMessage* msg);
static char*          lvn_logPatternStrDateMonthName(const LvnLogMessage* msg);
static char*          lvn_logPatternStrDateMonthNameShort(const LvnLogMessage* msg);
static char*          lvn_logPatternStrDateDayName(const LvnLogMessage* msg);
static char*          lvn_logPatternStrDateDayNameShort(const LvnLogMessage* msg);
static char*          lvn_logPatternStrDateTimeMeridiem(const LvnLogMessage* msg);
static char*          lvn_logPatternStrDateTimeMeridiemLower(const LvnLogMessage* msg);
static char*          lvn_logPatternStrDateTimeHHMMSS(const LvnLogMessage* msg);
static char*          lvn_logPatternStrDateTimeHHMMSS12(const LvnLogMessage* msg);
static char*          lvn_logPatternStrDateYear(const LvnLogMessage* msg);
static char*          lvn_logPatternStrDateYear02d(const LvnLogMessage* msg);
static char*          lvn_logPatternStrDateMonth(const LvnLogMessage* msg);
static char*          lvn_logPatternStrDateDay(const LvnLogMessage* msg);
static char*          lvn_logPatternStrDateHour(const LvnLogMessage* msg);
static char*          lvn_logPatternStrDateHour12(const LvnLogMessage* msg);
static char*          lvn_logPatternStrDateMinute(const LvnLogMessage* msg);
static char*          lvn_logPatternStrDateSecond(const LvnLogMessage* msg);
static LvnLogPattern* lvn_logParseFormat(const LvnContext* ctx, const char* fmt, uint32_t* logPatternCount);


#ifdef LVN_PLATFORM_WINDOWS
static void lvn_enableLogANSIcodeColors()
{
    DWORD consoleMode;
    HANDLE outputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (GetConsoleMode(outputHandle, &consoleMode))
    {
        SetConsoleMode(outputHandle, consoleMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
}
#endif

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

static char* lvn_logPatternStrNewLine(const LvnLogMessage* msg) { return lvn_strdup("\n"); }
static char* lvn_logPatternStrLoggerName(const LvnLogMessage* msg) { return lvn_strdup(msg->loggerName); }
static char* lvn_logPatternStrLogLevelName(const LvnLogMessage* msg) { return lvn_strdup(lvn_getLogLevelName(msg->level)); }
static char* lvn_logPatternStrLogLevelColor(const LvnLogMessage* msg) { return lvn_strdup(lvn_getLogLevelColor(msg->level)); }
static char* lvn_logPatternStrLogLevelReset(const LvnLogMessage* msg) { return lvn_strdup(LVN_LOG_COLOR_RESET); }
static char* lvn_logPatternStrMsg(const LvnLogMessage* msg) { return lvn_strdup(msg->msg); }
static char* lvn_logPatternStrPercent(const LvnLogMessage* msg) { return lvn_strdup("%"); }
static char* lvn_logPatternStrDateMonthName(const LvnLogMessage* msg) { return lvn_strdup(lvnDateGetMonthName()); }
static char* lvn_logPatternStrDateMonthNameShort(const LvnLogMessage* msg) { return lvn_strdup(lvnDateGetMonthNameShort()); }
static char* lvn_logPatternStrDateDayName(const LvnLogMessage* msg) { return lvn_strdup(lvnDateGetDayName()); }
static char* lvn_logPatternStrDateDayNameShort(const LvnLogMessage* msg) { return lvn_strdup(lvnDateGetDayNameShort()); }
static char* lvn_logPatternStrDateTimeMeridiem(const LvnLogMessage* msg) { return lvn_strdup(lvnDateGetTimeMeridiem()); }
static char* lvn_logPatternStrDateTimeMeridiemLower(const LvnLogMessage* msg) { return lvn_strdup(lvnDateGetTimeMeridiemLower()); }

static char* lvn_logPatternStrDateTimeHHMMSS(const LvnLogMessage* msg)
{
    char buff[9];
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    snprintf(buff, 9, "%02d:%02d:%02d", tm.tm_hour, tm.tm_min, tm.tm_sec);
    return lvn_strdup(buff);
}

static char* lvn_logPatternStrDateTimeHHMMSS12(const LvnLogMessage* msg)
{
    // make buff size larger to supress gcc truncate warning
    char buff[16];
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    snprintf(buff, 16, "%02d:%02d:%02d", ((tm.tm_hour + 11) % 12) + 1, tm.tm_min, tm.tm_sec);
    return lvn_strdup(buff);
}

static char* lvn_logPatternStrDateYear(const LvnLogMessage* msg)
{
    char buff[5];
    snprintf(buff, 5, "%d", lvnDateGetYear());
    return lvn_strdup(buff);
}

static char* lvn_logPatternStrDateYear02d(const LvnLogMessage* msg)
{
    char buff[3];
    snprintf(buff, 3, "%d", lvnDateGetYear02d());
    return lvn_strdup(buff);
}

static char* lvn_logPatternStrDateMonth(const LvnLogMessage* msg)
{
    char buff[3];
    snprintf(buff, 3, "%02d", lvnDateGetMonth());
    return lvn_strdup(buff);
}

static char* lvn_logPatternStrDateDay(const LvnLogMessage* msg)
{
    char buff[3];
    snprintf(buff, 3, "%02d", lvnDateGetDay());
    return lvn_strdup(buff);
}

static char* lvn_logPatternStrDateHour(const LvnLogMessage* msg)
{
    char buff[3];
    snprintf(buff, 3, "%02d", lvnDateGetHour());
    return lvn_strdup(buff);
}

static char* lvn_logPatternStrDateHour12(const LvnLogMessage* msg)
{
    char buff[3];
    snprintf(buff, 3, "%02d", lvnDateGetHour12());
    return lvn_strdup(buff);
}

static char* lvn_logPatternStrDateMinute(const LvnLogMessage* msg)
{
    char buff[3];
    snprintf(buff, 3, "%02d", lvnDateGetMinute());
    return lvn_strdup(buff);
}

static char* lvn_logPatternStrDateSecond(const LvnLogMessage* msg)
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
            if (fmt[i + 1] == ctx->pUserLogPatterns[j].symbol)
            {
                patterns = lvn_realloc(patterns, ++patternCount * sizeof(LvnLogPattern));
                memcpy(&patterns[patternCount - 1], &ctx->pUserLogPatterns[j], sizeof(LvnLogPattern));
                break;
            }
        }

        i++; // incramant past symbol on next character in format
    }

    *logPatternCount = patternCount;

    return patterns;
}

LvnFile lvnLoadFileSrc(const char* filepath)
{
    return lvnLoadFile(filepath, Lvn_FileType_Src);
}

LvnFile lvnLoadFileBin(const char* filepath)
{
    return lvnLoadFile(filepath, Lvn_FileType_Bin);
}

LvnFile lvnLoadFile(const char* filepath, LvnFileType type)
{
    LVN_ASSERT(filepath, "filepath cannot be null");

    LvnFile file = {0};

    const char* mode = "r";
    if (type == Lvn_FileType_Src)
        mode = "r";
    else if (type == Lvn_FileType_Bin)
        mode = "rb";

    FILE* fileptr = fopen(filepath, mode);

    if (!fileptr)
        return file;

    fseek(fileptr, 0, SEEK_END);
    size_t filesize = ftell(fileptr);
    fseek(fileptr, 0, SEEK_SET);

    file.data = lvn_calloc(filesize * sizeof(uint8_t));
    if (!file.data) { goto fail_cleanup; }
    file.size = filesize;

    fread(file.data, sizeof(uint8_t), filesize, fileptr);
    fclose(fileptr);
    return file;

fail_cleanup:
    file.data = NULL;
    file.size = 0;
    fclose(fileptr);
    return file;
}

void lvnUnloadFile(LvnFile* file)
{
    if (!file) return;
    lvn_free(file->data);
    file->data = NULL;
    file->size = 0;
}

LvnResult lvnCreateContext(LvnContext** ctx, const LvnContextCreateInfo* createInfo)
{
    if (!ctx)
        return Lvn_Result_Failure;

    *ctx = (LvnContext*) lvn_calloc(sizeof(LvnContext));

    if (!*ctx) { goto fail_cleanup; }

    memset(*ctx, 0, sizeof(LvnContext));
    LvnContext* ctxPtr = *ctx;

#ifdef LVN_PLATFORM_WINDOWS
    lvn_enableLogANSIcodeColors();
#endif

    if (createInfo)
    {
        ctxPtr->enableLogging = createInfo->logging.enableLogging;
        ctxPtr->coreLogger.logLevel = createInfo->logging.coreLogLevel;
    }
    else
    {
        ctxPtr->enableLogging = false;
        ctxPtr->coreLogger.logLevel = Lvn_LogLevel_None;
    }

    // app
    if (createInfo && createInfo->appName)
        ctxPtr->appName = lvn_strdup(createInfo->appName);
    else
        ctxPtr->appName = lvn_strdup(LVN_DEFAULT_APP_NAME);

    if (!ctxPtr->appName) { goto fail_cleanup; }

    // logging
    if (createInfo && createInfo->logging.coreLogFormat)
        ctxPtr->coreLogger.logPatternFormat = lvn_strdup(createInfo->logging.coreLogFormat);
    else
        ctxPtr->coreLogger.logPatternFormat = lvn_strdup(LVN_DEFAULT_LOG_PATTERN);

    if (!ctxPtr->coreLogger.logPatternFormat) { goto fail_cleanup; }

    if (createInfo && createInfo->logging.pCoreSinks)
    {
        ctxPtr->coreLogger.pSinks = (LvnSink*) lvn_calloc(sizeof(LvnSink) * createInfo->logging.coreSinkCount);
        if (!ctxPtr->coreLogger.pSinks) { goto fail_cleanup; }
        memcpy(ctxPtr->coreLogger.pSinks, createInfo->logging.pCoreSinks, sizeof(LvnSink) * createInfo->logging.coreSinkCount);
        ctxPtr->coreLogger.sinkCount = createInfo->logging.coreSinkCount;
    }
    else
    {
        ctxPtr->coreLogger.pSinks = (LvnSink*) lvn_calloc(sizeof(LvnSink));
        if (!ctxPtr->coreLogger.pSinks) { goto fail_cleanup; }
        ctxPtr->coreLogger.pSinks->logFunc = printWrapper;
        ctxPtr->coreLogger.sinkCount = 1;
    }

    ctxPtr->coreLogger.ctx = ctxPtr;
    ctxPtr->coreLogger.loggerName = lvn_strdup("CORE");
    if (!ctxPtr->coreLogger.loggerName) { goto fail_cleanup; }
    ctxPtr->coreLogger.pLogPatterns = lvn_logParseFormat(ctxPtr, LVN_DEFAULT_LOG_PATTERN, &ctxPtr->coreLogger.logPatternCount);
    ctxPtr->coreLogger.logging = true;

    LVN_LOG_TRACE(&ctxPtr->coreLogger, "levikno context created: (%p)", *ctx);
    return Lvn_Result_Success;

fail_cleanup:
    lvnDestroyContext(*ctx);
    *ctx = NULL;
    return Lvn_Result_Failure;
}

void lvnDestroyContext(LvnContext* ctx)
{
    if (!ctx) return;

    LVN_LOG_TRACE(&ctx->coreLogger, "terminating levikno context: (%p)", ctx);

    if (ctx->appName)
        lvn_free(ctx->appName);
    if (ctx->coreLogger.loggerName)
        lvn_free(ctx->coreLogger.loggerName);
    if (ctx->coreLogger.logPatternFormat)
        lvn_free(ctx->coreLogger.logPatternFormat);
    if (ctx->coreLogger.pSinks)
        lvn_free(ctx->coreLogger.pSinks);
    if (ctx->coreLogger.pLogPatterns)
        lvn_free(ctx->coreLogger.pLogPatterns);
    if (ctx->pUserLogPatterns)
        lvn_free(ctx->pUserLogPatterns);

    lvn_free(ctx);
}

LvnResult lvnSetMemAllocCallbacks(LvnMemAllocFn allocFn, LvnMemFreeFn freeFn, LvnMemReallocFn reallocFn, void* userData)
{
    if (!allocFn || !freeFn || !reallocFn)
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

size_t lvnDateGetSecondsSinceEpoch(void)
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

LvnLogger* lvnCtxGetCoreLogger(LvnContext* ctx)
{
    LVN_ASSERT(ctx, "ctx cannot be null");
    return &ctx->coreLogger;
}

void lvnCtxEnableLogging(LvnContext* ctx, bool enable)
{
    LVN_ASSERT(ctx, "ctx cannot be null");
    ctx->enableLogging = enable;
}

void lvnCtxAddLogPatterns(LvnContext* ctx, const LvnLogPattern* pLogPatterns, uint32_t logPatternCount)
{
    LVN_ASSERT(ctx, "ctx cannot be null");

    if (!logPatternCount || !pLogPatterns)
        return;

    ctx->pUserLogPatterns = lvn_realloc(ctx->pUserLogPatterns, (ctx->userLogPatternCount + logPatternCount) * sizeof(LvnLogPattern));
    memcpy(ctx->pUserLogPatterns + ctx->userLogPatternCount, pLogPatterns, logPatternCount * sizeof(LvnLogPattern));
    ctx->userLogPatternCount += logPatternCount;
}

void lvnLogEnableLogging(LvnLogger* logger, bool enable)
{
    LVN_ASSERT(logger, "logger cannot be null");
    logger->logging = enable;
}

const char* lvnLogGetANSIcodeColor(LvnLogLevel level)
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

void lvnLogOutputMessage(const LvnLogger* logger, LvnLogMessage* msg)
{
    LVN_ASSERT(logger && msg, "logger and msg cannot be null");

    if (!logger->logging) { return; }

    char* msgstr = NULL;
    size_t msglen = 0;

    for (uint32_t i = 0; i < logger->logPatternCount; i++)
    {
        if (!logger->pLogPatterns[i].func) // no special format character '%' found
        {
            msgstr = lvn_realloc(msgstr, ++msglen * sizeof(char) + 1);
            memcpy(&msgstr[msglen - 1], &logger->pLogPatterns[i].symbol, sizeof(char));
        }
        else // call func of special format
        {
            char* logmsg = logger->pLogPatterns[i].func(msg);
            size_t loglen = strlen(logmsg);
            msglen += loglen;
            msgstr = lvn_realloc(msgstr, msglen * sizeof(char) + 1);
            memcpy(&msgstr[msglen - loglen], logmsg, loglen * sizeof(char));
            lvn_free(logmsg);
        }
    }

    if (!msgstr)
        return;

    msgstr[msglen] = '\0';
    for (uint32_t i = 0; i < logger->sinkCount; i++)
        logger->pSinks[i].logFunc(msgstr);

    lvn_free(msgstr);
}

uint32_t lvnLogFormatMessage(const LvnLogger* logger, char* dst, uint32_t length, LvnLogLevel level, const char* msg)
{
    LVN_ASSERT(logger && msg, "logger and msg cannot be null");

    LvnLogMessage logMsg =
    {
        .msg = msg,
        .loggerName = logger->loggerName,
        .level = level,
        .timeEpoch = lvnDateGetSecondsSinceEpoch(),
    };

    char* msgstr = NULL;
    int msglen = 0;

    for (uint32_t i = 0; i < logger->logPatternCount; i++)
    {
        if (!logger->pLogPatterns[i].func) // no special format character '%' found
        {
            msgstr = lvn_realloc(msgstr, ++msglen * sizeof(char));
            memcpy(&msgstr[msglen - 1], &logger->pLogPatterns[i].symbol, sizeof(char));
        }
        else // call func of special format
        {
            char* logmsg = logger->pLogPatterns[i].func(&logMsg);
            int loglen = strlen(logmsg);
            msglen += loglen;
            msgstr = lvn_realloc(msgstr, msglen * sizeof(char));
            memcpy(&msgstr[msglen - loglen], logmsg, loglen * sizeof(char));
            lvn_free(logmsg);
        }
    }

    if (dst)
        memcpy(dst, msgstr, length <= msglen ? length * sizeof(char) : msglen * sizeof(char));

    lvn_free(msgstr);
    return msglen;
}

uint32_t lvnLogFormatMessageArgs(const LvnLogger* logger, char* dst, uint32_t length, LvnLogLevel level, const char* fmt, ...)
{
    LVN_ASSERT(logger && fmt, "logger and fmt cannot be null");

    char* buff;

    va_list argptr, argcopy;
    va_start(argptr, fmt);
    va_copy(argcopy, argptr);

    int len = vsnprintf(NULL, 0, fmt, argptr);
    buff = lvn_calloc((len + 1) * sizeof(char));
    if (!buff) { return 0; }
    vsnprintf(buff, len + 1, fmt, argcopy);
    uint32_t msglen = lvnLogFormatMessage(logger, dst, length, level, buff);

    va_end(argcopy);
    va_end(argptr);

    lvn_free(buff);

    return msglen;
}

void lvnLogParseLogPatternFormat(LvnLogger* logger, const char* fmt)
{
    LVN_ASSERT(logger && fmt, "logger and fmt cannot be null");

    const LvnContext* ctx = logger->ctx;

    lvn_free(logger->pLogPatterns);
    logger->pLogPatterns = lvn_logParseFormat(ctx, fmt, &logger->logPatternCount);
}

void lvnLogMessage(const LvnLogger* logger, LvnLogLevel level, const char* msg)
{
    LVN_ASSERT(logger && msg, "logger and msg cannot be null");

    if (!logger->logging) { return; }

    LvnLogMessage logMsg =
    {
        .msg = msg,
        .loggerName = logger->loggerName,
        .level = level,
        .timeEpoch = lvnDateGetSecondsSinceEpoch(),
    };

    lvnLogOutputMessage(logger, &logMsg);
}

bool lvnLogCheckLevel(const LvnLogger* logger, LvnLogLevel level)
{
    LVN_ASSERT(logger, "logger cannot be null");
    return (level >= logger->logLevel);
}

void lvnLogSetLevel(LvnLogger* logger, LvnLogLevel level)
{
    LVN_ASSERT(logger, "logger cannot be null");
    logger->logLevel = level;
}

void lvnLogMessageTrace(const LvnLogger* logger, const char* fmt, ...)
{
    LVN_ASSERT(logger && fmt, "logger and fmt cannot be null");

    if (!logger->logging || !logger->ctx->enableLogging) { return; }
    if (!lvnLogCheckLevel(logger, Lvn_LogLevel_Trace)) { return; }

    char* buff;

    va_list argptr, argcopy;
    va_start(argptr, fmt);
    va_copy(argcopy, argptr);

    int len = vsnprintf(NULL, 0, fmt, argptr);
    buff = lvn_calloc((len + 1) * sizeof(char));
    if (!buff) { return; }
    vsnprintf(buff, len + 1, fmt, argcopy);
    lvnLogMessage(logger, Lvn_LogLevel_Trace, buff);

    va_end(argcopy);
    va_end(argptr);

    lvn_free(buff);
}

void lvnLogMessageDebug(const LvnLogger* logger, const char* fmt, ...)
{
    LVN_ASSERT(logger && fmt, "logger and fmt cannot be null");

    if (!logger->logging || !logger->ctx->enableLogging) { return; }
    if (!lvnLogCheckLevel(logger, Lvn_LogLevel_Debug)) { return; }

    char* buff;

    va_list argptr, argcopy;
    va_start(argptr, fmt);
    va_copy(argcopy, argptr);

    int len = vsnprintf(NULL, 0, fmt, argptr);
    buff = lvn_calloc((len + 1) * sizeof(char));
    if (!buff) { return; }
    vsnprintf(buff, len + 1, fmt, argcopy);
    lvnLogMessage(logger, Lvn_LogLevel_Debug, buff);

    va_end(argcopy);
    va_end(argptr);

    lvn_free(buff);
}

void lvnLogMessageInfo(const LvnLogger* logger, const char* fmt, ...)
{

    LVN_ASSERT(logger && fmt, "logger and fmt cannot be null");

    if (!logger->logging || !logger->ctx->enableLogging) { return; }
    if (!lvnLogCheckLevel(logger, Lvn_LogLevel_Info)) { return; }

    char* buff;

    va_list argptr, argcopy;
    va_start(argptr, fmt);
    va_copy(argcopy, argptr);

    int len = vsnprintf(NULL, 0, fmt, argptr);
    buff = lvn_calloc((len + 1) * sizeof(char));
    if (!buff) { return; }
    vsnprintf(buff, len + 1, fmt, argcopy);
    lvnLogMessage(logger, Lvn_LogLevel_Info, buff);

    va_end(argcopy);
    va_end(argptr);

    lvn_free(buff);
}

void lvnLogMessageWarn(const LvnLogger* logger, const char* fmt, ...)
{

    LVN_ASSERT(logger && fmt, "logger and fmt cannot be null");

    if (!logger->logging || !logger->ctx->enableLogging) { return; }
    if (!lvnLogCheckLevel(logger, Lvn_LogLevel_Warn)) { return; }

    char* buff;

    va_list argptr, argcopy;
    va_start(argptr, fmt);
    va_copy(argcopy, argptr);

    int len = vsnprintf(NULL, 0, fmt, argptr);
    buff = lvn_calloc((len + 1) * sizeof(char));
    if (!buff) { return; }
    vsnprintf(buff, len + 1, fmt, argcopy);
    lvnLogMessage(logger, Lvn_LogLevel_Warn, buff);

    va_end(argcopy);
    va_end(argptr);

    lvn_free(buff);
}

void lvnLogMessageError(const LvnLogger* logger, const char* fmt, ...)
{
    LVN_ASSERT(logger && fmt, "logger and fmt cannot be null");

    if (!logger->logging || !logger->ctx->enableLogging) { return; }
    if (!lvnLogCheckLevel(logger, Lvn_LogLevel_Error)) { return; }

    char* buff;

    va_list argptr, argcopy;
    va_start(argptr, fmt);
    va_copy(argcopy, argptr);

    int len = vsnprintf(NULL, 0, fmt, argptr);
    buff = lvn_calloc((len + 1) * sizeof(char));
    if (!buff) { return; }
    vsnprintf(buff, len + 1, fmt, argcopy);
    lvnLogMessage(logger, Lvn_LogLevel_Error, buff);

    va_end(argcopy);
    va_end(argptr);

    lvn_free(buff);
}

void lvnLogMessageFatal(const LvnLogger* logger, const char* fmt, ...)
{
    LVN_ASSERT(logger && fmt, "logger and fmt cannot be null");

    if (!logger->logging || !logger->ctx->enableLogging) { return; }
    if (!lvnLogCheckLevel(logger, Lvn_LogLevel_Fatal)) { return; }

    char* buff;

    va_list argptr, argcopy;
    va_start(argptr, fmt);
    va_copy(argcopy, argptr);

    int len = vsnprintf(NULL, 0, fmt, argptr);
    buff = lvn_calloc((len + 1) * sizeof(char));
    if (!buff) { return; }
    vsnprintf(buff, len + 1, fmt, argcopy);
    lvnLogMessage(logger, Lvn_LogLevel_Fatal, buff);

    va_end(argcopy);
    va_end(argptr);

    lvn_free(buff);
}

char* lvnLogCreateOneShotStrMsg(const char* str)
{
    return lvn_strdup(str);
}

LvnResult lvnCreateLogger(const LvnContext* ctx, LvnLogger** logger, const LvnLoggerCreateInfo* createInfo)
{
    LVN_ASSERT(logger && createInfo, "logger and createInfo cannot be null");
    LVN_ASSERT(createInfo->name, "createInfo->name cannot be null");
    LVN_ASSERT(createInfo->format, "createInfo->format cannot be null");

    *logger = (LvnLogger*) lvn_calloc(sizeof(LvnLogger));

    if (!*logger)
    {
        LVN_LOG_ERROR(&ctx->coreLogger, "failed to allocate memory for logger at %p", logger);
        goto fail_cleanup;
    }

    LvnLogger* loggerPtr = *logger;
    loggerPtr->ctx = ctx;
    loggerPtr->logging = true;
    loggerPtr->logLevel = createInfo->level;
    loggerPtr->sinkCount = createInfo->sinkCount;
    loggerPtr->logging = true;

    loggerPtr->loggerName = lvn_strdup(createInfo->name);
    if (!loggerPtr->loggerName) { goto fail_cleanup; }

    loggerPtr->logPatternFormat = lvn_strdup(createInfo->format);
    if (!loggerPtr->logPatternFormat) { goto fail_cleanup; }

    loggerPtr->pLogPatterns = lvn_logParseFormat(ctx, createInfo->format, &loggerPtr->logPatternCount);
    if (!loggerPtr->pLogPatterns) { goto fail_cleanup;}

    loggerPtr->pSinks = lvn_calloc(createInfo->sinkCount * sizeof(LvnSink));
    if (!loggerPtr->pSinks) { goto fail_cleanup; }
    memcpy(loggerPtr->pSinks, createInfo->pSinks, createInfo->sinkCount * sizeof(LvnSink));

    return Lvn_Result_Success;

fail_cleanup:
    lvnDestroyLogger(*logger);
    *logger = NULL;
    return Lvn_Result_OutOfMemory;
}

void lvnDestroyLogger(LvnLogger* logger)
{
    if (!logger) return;

    if (logger->loggerName)
        lvn_free(logger->loggerName);
    if (logger->logPatternFormat)
        lvn_free(logger->logPatternFormat);
    if (logger->pLogPatterns)
        lvn_free(logger->pLogPatterns);
    if (logger->pSinks)
        lvn_free(logger->pSinks);

    lvn_free(logger);
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
    LVN_ASSERT(str, "str cannot be null");
    const size_t length = strlen(str) + 1;
    char* result = (char*) lvn_calloc(length);
    if (!result) { return NULL; }
    memcpy(result, str, length);
    return result;
}

bool lvn_ptrInBlock(uint8_t* block, size_t size, void* ptr)
{
    uint8_t* start = block;
    uint8_t* end = start + size;
    return (uint8_t*)ptr >= start && (uint8_t*)ptr < end;
}

LvnMemoryPool* lvn_memPoolCreate(size_t count, size_t stride, size_t align)
{
    LVN_ASSERT(stride == 0 || count <= SIZE_MAX / stride, "overflow on creating memory size (count * stride)");
    LVN_ASSERT(align != 0 && (align & (align - 1)) == 0, "align cannot be zero or a non power of two");
    LVN_ASSERT(align >= LVN_ALIGNOF(void*), "align must be >= pointer alignment");

    LvnMemoryPool* memPool = (LvnMemoryPool*) lvn_calloc(sizeof(LvnMemoryPool));
    if (!memPool) { return NULL; }

    // set min stride
    const size_t minStride = sizeof(void*);
    stride = stride < minStride ? minStride : stride;

    // align stride
    size_t strideAligned = LVN_ALIGN_UP(stride, align);

    LVN_ASSERT(strideAligned % align == 0, "stride must be multiple of align");

    size_t memSize = count * strideAligned;
    memPool->memBlock.block = lvn_calloc(memSize + align);
    if (!memPool->memBlock.block)
    {
        lvn_free(memPool);
        return NULL;
    }

#ifdef LVN_CONFIG_DEBUG
    memset(memPool->memBlock.block, LVN_DEBUG_ALLOC_VALUE, memSize + align);
    memPool->d_AllocCount = 0;
#endif

    memPool->memBlock.blockAligned = (uint8_t*)LVN_ALIGN_UP((uintptr_t)memPool->memBlock.block, align);
    memPool->memBlock.size = memSize + align;

    memPool->currIndex = 0;
    memPool->capacity = count;
    memPool->stride = stride;
    memPool->align = align;
    memPool->strideAligned = strideAligned;
    memPool->freeList = NULL;
    memPool->next = NULL;

    return memPool;
}

LvnMemoryPool* lvn_memPoolPush(LvnMemoryPool* headPool, size_t count)
{
    LVN_ASSERT(headPool, "headPool cannot be null");

    LvnMemoryPool* memPool = lvn_memPoolCreate(count, headPool->stride, headPool->align);
    if (!memPool) { return NULL; }

    while (headPool->next)
        headPool = headPool->next;

    headPool->next = memPool;
    return memPool;
}

void lvn_memPoolDestroy(LvnMemoryPool* headPool)
{
    while (headPool)
    {
        LvnMemoryPool* temp = headPool;
        headPool = headPool->next;
        lvn_free(temp->memBlock.block);
        lvn_free(temp);
    }
}

void* lvn_memPoolAlloc(LvnMemoryPool* memPool)
{
    void* ptr = NULL;

    while (memPool)
    {
        // check free list
        if (memPool->freeList)
        {
            ptr = memPool->freeList;
            memPool->freeList = memPool->freeList->next;
            goto alloc_success;
        }

        // get pool next memory block index
        if (memPool->currIndex < memPool->capacity)
        {
            ptr = &memPool->memBlock.blockAligned[memPool->currIndex++ * memPool->strideAligned];
            goto alloc_success;
        }

        // create next pool if possible
        if (!memPool->next)
        {
            LVN_ASSERT(LVN_MEMPOOL_NEXT_MALLOC_MULTIPLIER != 0, "memory pool multiplier cannot be zero");
            memPool->next = lvn_memPoolCreate(memPool->capacity * LVN_MEMPOOL_NEXT_MALLOC_MULTIPLIER,
                                              memPool->stride,
                                              memPool->align);
            if (!memPool->next) { return NULL; }
        }

        // pool is full, alloc in next pool
        memPool = memPool->next;
    }

    // unable to find next block index
    return NULL;

alloc_success:
#ifdef LVN_CONFIG_DEBUG
    memPool->d_AllocCount++;
#endif
    return ptr;
}

void lvn_memPoolFree(LvnMemoryPool* memPool, void* ptr)
{
    LVN_ASSERT(memPool && ptr, "memPool and ptr cannot be null");

    // find which pool ptr was allocated from
    // NOTE: only checks if ptr is within capacity * strideAligned of the block rather than the full block size,
    //       this is to ensure the ptr is a valid ptr that came from pool alloc
    while (memPool && !lvn_ptrInBlock(memPool->memBlock.blockAligned, memPool->capacity * memPool->strideAligned, ptr))
        memPool = memPool->next;
    LVN_ASSERT(memPool, "ptr not found within pool chain memory boundaries");

    // check alignment of pointer
    size_t offset = (uint8_t*)ptr - (uint8_t*)memPool->memBlock.blockAligned;
    LVN_ASSERT(offset % memPool->strideAligned == 0, "invalid pool pointer, pointer not aligned to pool stride align");

#ifdef LVN_CONFIG_DEBUG
    // checks if ptr was already freed (double free)
    for (LvnFreeNode* node = memPool->freeList; node; node = node->next)
    {
        LVN_ASSERT(node != ptr, "double free in memory pool");
    }
    // check if ptr is trying to be freed after pool reset/create
    LVN_ASSERT(memPool->d_AllocCount > 0, "cannot free ptr to pool, pool was probably reset or just created");
    memPool->d_AllocCount--;
#endif

    // free ptr, add to free list
    LvnFreeNode* node = (LvnFreeNode*) ptr;
    node->next = memPool->freeList;
    memPool->freeList = node;
}

void lvn_memPoolReset(LvnMemoryPool* headPool)
{
    LVN_ASSERT(headPool, "headPool cannot be null");

    for (LvnMemoryPool* memPool = headPool; memPool; memPool = memPool->next)
    {
        memPool->currIndex = 0;
        memPool->freeList = NULL;
#ifdef LVN_CONFIG_DEBUG
        memset(memPool->memBlock.blockAligned, LVN_DEBUG_FREE_VALUE, memPool->capacity * memPool->strideAligned);
        memPool->d_AllocCount = 0;
#endif
    }
}

LvnMemoryPool* lvn_memPoolRebuild(LvnMemoryPool* headPool)
{
    LVN_ASSERT(headPool, "headPool cannot be null");

    // clear values only if there is only one pool in the chain (head pool)
    if (!headPool->next)
    {
        lvn_memPoolReset(headPool);
        return headPool;
    }

    size_t totalSize = 0;
    LvnMemoryPool* head = headPool;
    while (head)
    {
        totalSize += head->capacity;
        head = head->next;
    }

    LvnMemoryPool* memPool = lvn_memPoolCreate(totalSize, headPool->stride, headPool->align);
    if (!memPool) { return NULL; }

    head = headPool;
    while (head)
    {
        LvnMemoryPool* temp = head;
        head = head->next;
        lvn_free(temp->memBlock.block);
        lvn_free(temp);
    }

    return memPool;
}

LvnMemoryArena* lvn_memArenaCreate(size_t size, size_t align)
{
    LVN_ASSERT(size <= SIZE_MAX, "arena size overflow");
    LVN_ASSERT(align != 0 && (align & (align - 1)) == 0, "align cannot be zero or a non power of two");
    LVN_ASSERT(align >= LVN_ALIGNOF(void*), "align must be >= pointer alignment");

    LvnMemoryArena* memArena = (LvnMemoryArena*) lvn_calloc(sizeof(LvnMemoryArena));
    if (!memArena) { return NULL; }

    memArena->memBlock.block = lvn_calloc(size + align);
    if (!memArena->memBlock.block)
    {
        lvn_free(memArena);
        return NULL;
    }

#ifdef LVN_CONFIG_DEBUG
    memset(memArena->memBlock.block, LVN_DEBUG_ALLOC_VALUE, size);
    memArena->d_AllocCount = 0;
#endif

    memArena->memBlock.blockAligned = (uint8_t*)LVN_ALIGN_UP((uintptr_t)memArena->memBlock.block, align);
    memArena->memBlock.size = size + align;

    memArena->currIndex = 0;
    memArena->capacity = size;
    memArena->align = align;
    memArena->next = NULL;

    return memArena;
}

LvnMemoryArena* lvn_memArenaPush(LvnMemoryArena* headArena, size_t size)
{
    LVN_ASSERT(headArena, "headArena cannot be null");

    LvnMemoryArena* memArena = lvn_memArenaCreate(size, headArena->align);
    if (!memArena) { return NULL; }

    while (headArena->next)
        headArena = headArena->next;

    headArena->next = memArena;
    return memArena;
}

void lvn_memArenaDestroy(LvnMemoryArena* headArena)
{
    while (headArena)
    {
        LvnMemoryArena* temp = headArena;
        headArena = headArena->next;
        lvn_free(temp->memBlock.block);
        lvn_free(temp);
    }
}

void* lvn_memArenaAlloc(LvnMemoryArena* memArena, size_t size)
{
    LVN_ASSERT(memArena, "memArena cannot be null");

    while (memArena)
    {
        // get arena next memory block index
        uintptr_t base = (uintptr_t)memArena->memBlock.blockAligned;
        uintptr_t aligned = LVN_ALIGN_UP(base + memArena->currIndex, memArena->align);
        size_t newIndex = aligned - base;
        if (newIndex + size <= memArena->capacity)
        {
            void* ptr = &(memArena->memBlock.blockAligned)[memArena->currIndex];
            memArena->currIndex = newIndex + size;
#ifdef LVN_CONFIG_DEBUG
            memArena->d_AllocCount++;
#endif
            return ptr;
        }

        // create next arena if possible
        if (!memArena->next)
        {
            LVN_ASSERT(LVN_MEMARENA_NEXT_MALLOC_MULTIPLIER != 0, "memory arena multiplier cannot be zero");
            size_t nextSize = memArena->capacity * LVN_MEMARENA_NEXT_MALLOC_MULTIPLIER;
            nextSize = (size > nextSize) ? size : nextSize;
            memArena->next = lvn_memArenaCreate(nextSize, memArena->align);
            if (!memArena->next) { return NULL; }
        }

        // arena is full, get alloc in next arena
        memArena = memArena->next;
    }

    // unable to find next block index
    return NULL;
}

void* lvn_memArenaAllocAligned(LvnMemoryArena* memArena, size_t size, size_t align)
{
    LVN_ASSERT(memArena, "memArena cannot be null");
    LVN_ASSERT(align != 0 && (align & (align - 1)) == 0, "align cannot be zero or a non power of two");

    while (memArena)
    {
        // get arena next memory block index
        uintptr_t base = (uintptr_t)memArena->memBlock.blockAligned;
        uintptr_t aligned = LVN_ALIGN_UP(base + memArena->currIndex, align);
        size_t newIndex = aligned - base;
        if (newIndex + size <= memArena->capacity)
        {
            void* ptr = &(memArena->memBlock.blockAligned)[newIndex];
            memArena->currIndex = newIndex + size;
#ifdef LVN_CONFIG_DEBUG
            memArena->d_AllocCount++;
#endif
            return ptr;
        }

        // create next arena if possible
        if (!memArena->next)
        {
            LVN_ASSERT(LVN_MEMARENA_NEXT_MALLOC_MULTIPLIER != 0, "memory arena multiplier cannot be zero");
            size_t nextSize = memArena->capacity * LVN_MEMARENA_NEXT_MALLOC_MULTIPLIER;
            nextSize = (size > nextSize) ? size : nextSize;
            memArena->next = lvn_memArenaCreate(nextSize, memArena->align);
            if (!memArena->next) { return NULL; }
        }

        // arena is full, get alloc in next arena
        memArena = memArena->next;
    }

    // unable to find next block index
    return NULL;
}

void lvn_memArenaReset(LvnMemoryArena* headArena)
{
    LVN_ASSERT(headArena, "memArena cannot be null");

    for (LvnMemoryArena* memArena = headArena; memArena; memArena = memArena->next)
    {
        memArena->currIndex = 0;
#ifdef LVN_CONFIG_DEBUG
        memset(memArena->memBlock.blockAligned, LVN_DEBUG_FREE_VALUE, memArena->capacity);
        memArena->d_AllocCount = 0;
#endif
    }
}

LvnMemoryArena* lvn_memArenaRebuild(LvnMemoryArena* headArena)
{
    LVN_ASSERT(headArena, "memArena cannot be null");

    // clear values only if there is only one arena in the chain (head arena)
    if (!headArena->next)
    {
        lvn_memArenaReset(headArena);
        return headArena;
    }

    size_t totalSize = 0;
    LvnMemoryArena* head = headArena;
    while (head)
    {
        totalSize += head->capacity;
        head = head->next;
    }

    LvnMemoryArena* memArena = lvn_memArenaCreate(totalSize, headArena->align);
    if (!memArena) { return NULL; }

    head = headArena;
    while (head)
    {
        LvnMemoryArena* temp = head;
        head = head->next;
        lvn_free(temp->memBlock.block);
        lvn_free(temp);
    }

    return memArena;
}
