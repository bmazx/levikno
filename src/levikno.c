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

LvnResult lvn_memBlockCreate(LvnMemoryBlock** memBlock, const LvnMemoryBlockCreateInfo* createInfo)
{
    LVN_ASSERT(memBlock && createInfo, "memBlock and createInfo cannot be null");
    LVN_ASSERT(createInfo->align != 0 && (createInfo->align & (createInfo->align - 1)) == 0, "align cannot be zero or a non power of two");
    LVN_ASSERT(createInfo->align >= LVN_ALIGNOF(void*), "align must be >= default alignment");
    LVN_ASSERT(createInfo->size <= SIZE_MAX - createInfo->align - sizeof(LvnMemoryBlock), "size overflow, size + align + sizeof(LvnMemoryBlock) must be <= SIZE_MAX");

    LvnResult errResult = Lvn_Result_Failure;
    LvnMemoryBlock* memBlockPtr = NULL;

    *memBlock = (LvnMemoryBlock*) lvn_calloc(sizeof(LvnMemoryBlock) + createInfo->size + createInfo->align);
    if (!*memBlock)
    {
        errResult = Lvn_Result_OutOfMemory;
        goto fail_cleanup;
    }

    memset(*memBlock, 0, sizeof(LvnMemoryBlock) + createInfo->size + createInfo->align);
    memBlockPtr = *memBlock;

    memBlockPtr->allocation = (uint8_t*) memBlockPtr + sizeof(LvnMemoryBlock);
    memBlockPtr->allocation = (uint8_t*) LVN_ALIGN_UP((uintptr_t)memBlockPtr->allocation, createInfo->align);

    memBlockPtr->size = createInfo->size;
    memBlockPtr->currIndex = memBlockPtr->allocation;
    memBlockPtr->next = createInfo->next;

#ifdef LVN_CONFIG_DEBUG
    memset(memBlockPtr->allocation, LVN_DEBUG_FREE_VALUE, createInfo->size + createInfo->align);
#endif

    return Lvn_Result_Success;

fail_cleanup:
    if (*memBlock)
    {
        lvn_free(*memBlock);
        *memBlock = NULL;
    }
    return errResult;
}

void lvn_memBlockDestroy(LvnMemoryBlock* memBlock)
{
    if (!memBlock) { return; }
    lvn_free(memBlock);
}

void lvn_memBlockDestroyChain(LvnMemoryBlock* memBlock)
{
    while (memBlock)
    {
        LvnMemoryBlock* temp = memBlock;
        memBlock = memBlock->next;
        lvn_free(temp);
    }
}

size_t lvn_memBlockGetSize(LvnMemoryBlock* memBlock)
{
    LVN_ASSERT(memBlock, "memBlock cannot be null");
    return memBlock->size;
}

size_t lvn_memBlockGetOffset(LvnMemoryBlock* memBlock)
{
    LVN_ASSERT(memBlock, "memBlock cannot be null");
    return (uintptr_t)(memBlock->currIndex - memBlock->allocation);
}

LvnResult lvn_memPoolCreate(LvnMemoryPool* memPool, const LvnMemoryPoolCreateInfo* createInfo)
{
    LVN_ASSERT(memPool && createInfo, "memPool and createInfo cannot be null");
    LVN_ASSERT(createInfo->stride == 0 || createInfo->count <= SIZE_MAX / createInfo->stride, "overflow on creating memory size (count * stride)");
    LVN_ASSERT(createInfo->align != 0 && (createInfo->align & (createInfo->align - 1)) == 0, "align cannot be zero or a non power of two");
    LVN_ASSERT(createInfo->align >= LVN_ALIGNOF(void*), "align must be >= pointer alignment");

    LvnResult errResult = Lvn_Result_Failure;
    LvnMemoryBlock* memBlock = NULL;

    // set min stride
    const size_t minStride = sizeof(void*);
    size_t stride = createInfo->stride < minStride ? minStride : createInfo->stride;

    // align stride
    size_t strideAligned = LVN_ALIGN_UP(stride, createInfo->align);

    LVN_ASSERT(strideAligned % createInfo->align == 0, "stride must be multiple of align");
    LVN_ASSERT(createInfo->count <= SIZE_MAX / strideAligned, "memory size overflow, count * strideAligned must be <= SIZE_MAX");

    // create memory block
    LvnMemoryBlockCreateInfo memBlockCreateInfo = {
        .size = createInfo->count * strideAligned,
        .align = createInfo->align,
        .next = NULL,
    };

    LvnResult result = lvn_memBlockCreate(&memBlock, &memBlockCreateInfo);
    if (result != Lvn_Result_Success)
    {
        errResult = result;
        goto fail_cleanup;
    }

    // fill memory pool info
    *memPool = (LvnMemoryPool){
        .blocks = memBlock,
        .freeList = NULL,
        .stride = createInfo->stride,
        .strideAligned = strideAligned,
        .align = createInfo->align,
        .allocCount = 0,
    };

    return Lvn_Result_Success;

fail_cleanup:
    if (memBlock) { lvn_memBlockDestroy(memBlock); }
    return errResult;
}

void lvn_memPoolDestroy(LvnMemoryPool* memPool)
{
    if (!memPool) { return; }

    lvn_memBlockDestroyChain(memPool->blocks);

    memPool->blocks = NULL;
    memPool->freeList = NULL;
    memPool->stride = 0;
    memPool->strideAligned = 0;
    memPool->align = 0;
    memPool->allocCount = 0;
}

LvnResult lvn_memPoolPushBlock(LvnMemoryPool* memPool, size_t count)
{
    LVN_ASSERT(memPool, "memPool cannot be null");
    LVN_ASSERT(count <= SIZE_MAX / memPool->strideAligned, "memory size overflow, count * strideAligned must be <= SIZE_MAX");

    if (!count)
        return Lvn_Result_Success;

    LvnResult errResult = Lvn_Result_Failure;
    LvnMemoryBlock* memBlock = NULL;

    LvnMemoryBlockCreateInfo memBlockCreateInfo = {
        .size = count * memPool->strideAligned,
        .align = memPool->align,
        .next = memPool->blocks,
    };

    LvnResult result = lvn_memBlockCreate(&memBlock, &memBlockCreateInfo);
    if (result != Lvn_Result_Success)
    {
        errResult = result;
        goto fail_cleanup;
    }

    memPool->blocks = memBlock;

    return Lvn_Result_Success;

fail_cleanup:
    if (memBlock) { lvn_memBlockDestroy(memBlock); }
    return errResult;
}

void* lvn_memPoolAlloc(LvnMemoryPool* memPool)
{
    LVN_ASSERT(memPool, "memPool cannot be null");

    void* ptr = NULL;

    // check free list
    if (memPool->freeList)
    {
        ptr = memPool->freeList;
        memPool->freeList = memPool->freeList->next;
        goto alloc_success;
    }

    // get next memory block index in pool if available
    for (LvnMemoryBlock* currBlock = memPool->blocks; currBlock; currBlock = currBlock->next)
    {
        if ((currBlock->currIndex + memPool->strideAligned) <= (currBlock->allocation + currBlock->size))
        {
            ptr = currBlock->currIndex;
            currBlock->currIndex += memPool->strideAligned;
            goto alloc_success;
        }
    }

    // create new memory block if no space left
    if (lvn_memPoolPushBlock(memPool, lvn_memPoolGetTotalCapacity(memPool)) != Lvn_Result_Success)
        return NULL;

    if ((memPool->blocks->currIndex + memPool->strideAligned) <= (memPool->blocks->allocation + memPool->blocks->size))
    {
        ptr = memPool->blocks->currIndex;
        memPool->blocks->currIndex += memPool->strideAligned;
        goto alloc_success;
    }

    // unable to find next block index
    return NULL;

alloc_success:
#ifdef LVN_CONFIG_DEBUG
    memset(ptr, LVN_DEBUG_ALLOC_VALUE, memPool->strideAligned);
#endif
    memPool->allocCount++;
    return ptr;
}

void lvn_memPoolFree(LvnMemoryPool* memPool, void* ptr)
{
    LVN_ASSERT(memPool, "memPool cannot be null");

    if (!ptr) { return; }

#ifdef LVN_CONFIG_DEBUG
    // find block the ptr was allocated from
    LvnMemoryBlock* currBlock = NULL;
    for (currBlock = memPool->blocks; currBlock; currBlock = currBlock->next)
    {
        if (lvn_ptrInBlock(currBlock->allocation, currBlock->size, ptr))
            break;
    }
    LVN_ASSERT(currBlock, "ptr not found within memory pool blocks");

    // check alignment of pointer
    size_t offset = (uint8_t*)ptr - (uint8_t*)currBlock->allocation;
    LVN_ASSERT(offset % memPool->strideAligned == 0, "invalid pool pointer, pointer not aligned to pool stride align");

    // checks if ptr was already freed (double free)
    for (LvnFreeNode* node = memPool->freeList; node; node = node->next)
    {
        LVN_ASSERT(node != ptr, "double free in memory pool");
    }
    memset(ptr, LVN_DEBUG_FREE_VALUE, memPool->strideAligned);
#endif

    // check if ptr is trying to be freed after pool reset/create
    LVN_ASSERT(memPool->allocCount > 0, "cannot free ptr to pool, pool was probably reset or just created");
    memPool->allocCount--;

    // free ptr, add to free list
    LvnFreeNode* node = (LvnFreeNode*) ptr;
    node->next = memPool->freeList;
    memPool->freeList = node;
}

void lvn_memPoolReset(LvnMemoryPool* memPool)
{
    LVN_ASSERT(memPool, "memPool cannot be null");

    for (LvnMemoryBlock* currBlock = memPool->blocks; currBlock; currBlock = currBlock->next)
    {
        currBlock->currIndex = currBlock->allocation;
#ifdef LVN_CONFIG_DEBUG
        memset(currBlock->allocation, LVN_DEBUG_FREE_VALUE, currBlock->size);
#endif
    }

    memPool->freeList = NULL;
    memPool->allocCount = 0;
}

LvnResult lvn_memPoolResetMergeBlocks(LvnMemoryPool* memPool)
{
    LVN_ASSERT(memPool, "memPool cannot be null");

    LvnResult errResult = Lvn_Result_Failure;
    LvnMemoryBlock* memBlock = NULL;

    size_t totalSize = 0;
    for (LvnMemoryBlock* currBlock = memPool->blocks; currBlock; currBlock = currBlock->next)
        totalSize += currBlock->size;

    LvnMemoryBlockCreateInfo memBlockCreateInfo = {
        .size = totalSize,
        .align = memPool->align,
        .next = NULL,
    };

    LvnResult result = lvn_memBlockCreate(&memBlock, &memBlockCreateInfo);
    if (result != Lvn_Result_Success)
    {
        errResult = result;
        goto fail_cleanup;
    }

    lvn_memBlockDestroyChain(memPool->blocks);

    memPool->blocks = memBlock;
    memPool->freeList = NULL;
    memPool->allocCount = 0;

    return Lvn_Result_Success;

fail_cleanup:
    if (memBlock) { lvn_memBlockDestroy(memBlock); }
    return errResult;
}

size_t lvn_memPoolGetTotalCapacity(LvnMemoryPool* memPool)
{
    LVN_ASSERT(memPool, "memPool cannot be null");

    size_t count = 0;

    for (LvnMemoryBlock* currBlock = memPool->blocks; currBlock; currBlock = currBlock->next)
        count += currBlock->size / memPool->strideAligned;

    return count;
}

size_t lvn_memPoolGetAllocCount(LvnMemoryPool* memPool)
{
    LVN_ASSERT(memPool, "memPool cannot be null");
    return memPool->allocCount;
}

LvnResult lvn_memArenaCreate(LvnMemoryArena* memArena, const LvnMemoryArenaCreateInfo* createInfo)
{
    LVN_ASSERT(memArena && createInfo, "memArena and createInfo cannot be null");
    LVN_ASSERT(createInfo->size <= SIZE_MAX, "arena size overflow");
    LVN_ASSERT(createInfo->align != 0 && (createInfo->align & (createInfo->align - 1)) == 0, "align cannot be zero or a non power of two");
    LVN_ASSERT(createInfo->align >= LVN_ALIGNOF(lvn_max_align_t), "align must be >= max align");

    LvnResult errResult = Lvn_Result_Failure;
    LvnMemoryBlock* memBlock = NULL;

    // create memory block
    LvnMemoryBlockCreateInfo memBlockCreateInfo = {
        .size = createInfo->size,
        .align = createInfo->align,
        .next = NULL,
    };

    LvnResult result = lvn_memBlockCreate(&memBlock, &memBlockCreateInfo);
    if (result != Lvn_Result_Success)
    {
        errResult = result;
        goto fail_cleanup;
    }

    // fill memory arena info
    *memArena = (LvnMemoryArena){
        .blocks = memBlock,
        .align = createInfo->align,
        .generation = 0,
    };

    return Lvn_Result_Success;

fail_cleanup:
    if (memBlock) { lvn_memBlockDestroy(memBlock); }
    return errResult;
}

void lvn_memArenaDestroy(LvnMemoryArena* memArena)
{
    if (!memArena) { return; }

    lvn_memBlockDestroyChain(memArena->blocks);

    memArena->blocks = NULL;
    memArena->align = 0;
}

LvnResult lvn_memArenaPushBlock(LvnMemoryArena* memArena, size_t size)
{
    LVN_ASSERT(memArena, "memArena cannot be null");

    if (!size)
        return Lvn_Result_Success;

    LvnResult errResult = Lvn_Result_Failure;
    LvnMemoryBlock* memBlock = NULL;

    LvnMemoryBlockCreateInfo memBlockCreateInfo = {
        .size = size,
        .align = memArena->align,
        .next = memArena->blocks,
    };

    LvnResult result = lvn_memBlockCreate(&memBlock, &memBlockCreateInfo);
    if (result != Lvn_Result_Success)
    {
        errResult = result;
        goto fail_cleanup;
    }

    memArena->blocks = memBlock;

    return Lvn_Result_Success;

fail_cleanup:
    if (memBlock) { lvn_memBlockDestroy(memBlock); }
    return errResult;
}

void* lvn_memArenaAlloc(LvnMemoryArena* memArena, size_t size)
{
    LVN_ASSERT(memArena, "memArena cannot be null");
    return lvn_memArenaAllocAligned(memArena, size, memArena->align);
}

void* lvn_memArenaAllocAligned(LvnMemoryArena* memArena, size_t size, size_t align)
{
    LVN_ASSERT(memArena, "memArena cannot be null");
    LVN_ASSERT(align != 0 && (align & (align - 1)) == 0, "align cannot be zero or a non power of two");

    if (!size)
        return NULL;

    void* ptr = NULL;
    size_t newSize = 0;

    // get alloc from first block in arena if available
    uint8_t* alignedIndex = (uint8_t*) LVN_ALIGN_UP((uintptr_t)memArena->blocks->currIndex, align);
    if ((alignedIndex + size) <= (memArena->blocks->allocation + memArena->blocks->size))
    {
        ptr = alignedIndex;
        memArena->blocks->currIndex = alignedIndex + size;
        goto alloc_success;
    }

    // create new memory block if no space left
    LVN_ASSERT(memArena->blocks->size <= SIZE_MAX / 2, "arena size growth overflow");
    newSize = memArena->blocks->size * 2;
    newSize = (newSize < size) ? size : newSize;

    // check if align is greater than arena align, add to newSize if larger
    LVN_ASSERT(newSize <= SIZE_MAX - (align > memArena->align ? align : 0), "new size growth overflow on align");
    newSize += (align > memArena->align) ? align : 0;

    if (lvn_memArenaPushBlock(memArena, newSize) != Lvn_Result_Success)
        return NULL;

    alignedIndex = (uint8_t*) LVN_ALIGN_UP((uintptr_t)memArena->blocks->currIndex, align);
    if ((alignedIndex + size) <= (memArena->blocks->allocation + memArena->blocks->size))
    {
        ptr = alignedIndex;
        memArena->blocks->currIndex = alignedIndex + size;
        goto alloc_success;
    }

    // unable to find next block index
    return NULL;

alloc_success:
#ifdef LVN_CONFIG_DEBUG
    memset(ptr, LVN_DEBUG_ALLOC_VALUE, size);
#endif
    return ptr;
}

LvnArenaMark lvn_memArenaMark(LvnMemoryArena* memArena)
{
    LVN_ASSERT(memArena, "memArena cannot be null");
    return (LvnArenaMark){
        .block = memArena->blocks,
        .next = NULL,
        .offset = (uintptr_t)(memArena->blocks->currIndex - memArena->blocks->allocation),
        .generation = memArena->generation,
    };
}

void lvn_memArenaMarkRevert(LvnMemoryArena* memArena, const LvnArenaMark* mark)
{
    LVN_ASSERT(memArena && mark, "memArena and mark cannot be null");
    LVN_ASSERT(mark->block, "mark block cannot be null");
    LVN_ASSERT(mark->offset <= mark->block->size, "mark offset must be <= block size");
    LVN_ASSERT(mark->offset <= (uintptr_t)(mark->block->currIndex - mark->block->allocation), "mark offset must be <= block current index");
    LVN_ASSERT(mark->generation == memArena->generation, "mark generation must be the same to memArena generation");

    LvnMemoryBlock* currBlock = NULL;

#ifdef LVN_CONFIG_DEBUG
    for (currBlock = memArena->blocks; currBlock; currBlock = currBlock->next)
    {
        if (currBlock == mark->block)
            break;
    }
    LVN_ASSERT(currBlock, "mark not found within memory arena");
#endif

    currBlock = memArena->blocks;
    while (currBlock != mark->block)
    {
        LvnMemoryBlock* temp = currBlock;
        currBlock = currBlock->next;
        lvn_memBlockDestroy(temp);
    }

    memArena->blocks = mark->block;
    memArena->blocks->currIndex = memArena->blocks->allocation + mark->offset;

#ifdef LVN_CONFIG_DEBUG
    memset(memArena->blocks->allocation + mark->offset, LVN_DEBUG_FREE_VALUE, memArena->blocks->size - mark->offset);
#endif
}

LvnResult lvn_memArenaResetMergeBlocks(LvnMemoryArena* memArena)
{
    LVN_ASSERT(memArena, "memArena cannot be null");

    LvnResult errResult = Lvn_Result_Failure;
    LvnMemoryBlock* memBlock = NULL;

    size_t totalSize = 0;
    for (LvnMemoryBlock* currBlock = memArena->blocks; currBlock; currBlock = currBlock->next)
        totalSize += currBlock->size;

    LvnMemoryBlockCreateInfo memBlockCreateInfo = {
        .size = totalSize,
        .align = memArena->align,
        .next = NULL,
    };

    LvnResult result = lvn_memBlockCreate(&memBlock, &memBlockCreateInfo);
    if (result != Lvn_Result_Success)
    {
        errResult = result;
        goto fail_cleanup;
    }

    lvn_memBlockDestroyChain(memArena->blocks);

    memArena->blocks = memBlock;
    memArena->generation++;

    return Lvn_Result_Success;

fail_cleanup:
    if (memBlock) { lvn_memBlockDestroy(memBlock); }
    return errResult;
}

size_t lvn_memArenaGetTotalSize(LvnMemoryArena* memArena)
{
    LVN_ASSERT(memArena, "memArena cannot be null");

    size_t size = 0;

    for (LvnMemoryBlock* currBlock = memArena->blocks; currBlock; currBlock = currBlock->next)
        size += currBlock->size;

    return size;
}

void lvn_getWindowPlatform(LvnWindowPlatformSupport* windowPlatformSupport)
{
    LVN_ASSERT(windowPlatformSupport, "windowPlatformSupport cannot be null");

#if defined(LVN_INCLUDE_WIN32)
    windowPlatformSupport->win32Support = true;
#else
    windowPlatformSupport->win32Support = false;
#endif
#if defined(LVN_INCLUDE_WAYLAND) || defined(LVN_INCLUDE_X11)
    const char* session = getenv("XDG_SESSION_TYPE");
    if (session && (strcmp(session, "wayland") == 0 || strcmp(session, "x11") == 0))
    {
        if (strcmp(session, "wayland") == 0)
            windowPlatformSupport->waylandSupport = true;
        else if (strcmp(session, "x11") == 0)
            windowPlatformSupport->x11Support = true;
    }
    else
    {
        const char* waylandenv = getenv("WAYLAND_DISPLAY");
        const char* x11env = getenv("DISPLAY");
        if (waylandenv)
            windowPlatformSupport->waylandSupport = true;
        else if (x11env)
            windowPlatformSupport->x11Support = true;
    }
#else
    windowPlatformSupport->waylandSupport = false;
    windowPlatformSupport->x11Support = false;
#endif
}
