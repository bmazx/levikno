#include <levikno/levikno.h>

#include <stdio.h>
#include <stdlib.h>


void myPrint(const LvnLogMessage* outmsg)
{
    if (outmsg->level == Lvn_LogLevel_Error)
        fprintf(stderr, "%s", outmsg->msg);
    else
        fprintf(stdout, "%s", outmsg->msg);
}

char* myloggerPattern(const LvnLogMessage* logmsg)
{
    return lvnLogCreateOneShotStrMsg(">>>");
}


int main(int argc, char** argv)
{
    // create the context, remeber to set enableLogging to true
    LvnContextCreateInfo ctxCreateInfo = {
        .logging.enableLogging = true,
    };

    LvnContext* ctx;
    lvnCreateContext(&ctx, &ctxCreateInfo);

    // get the core logger
    LvnLogger* logger = lvnCtxGetCoreLogger(ctx);

    // log some messages
    float b = 3.1415f;
    const char* c = "world";

    lvnLogMessageTrace(logger, "hello %d | %s | %.5f | %p", 12, c, b, &b);
    lvnLogMessageDebug(logger, "hello %d | %s | %.5f | %p", 12, c, b, &b);
    lvnLogMessageInfo(logger, "hello %d | %s | %.5f | %p", 12, c, b, &b);
    lvnLogMessageWarn(logger, "hello %d | %s | %.5f | %p", 12, c, b, &b);
    lvnLogMessageError(logger, "hello %d | %s | %.5f | %p", 12, c, b, &b);
    lvnLogMessageFatal(logger, "hello %d | %s | %.5f | %p", 12, c, b, &b);

    // format a message using the log pattern format of a logger
    uint32_t len = lvnLogFormatMessage(logger, NULL, 0, Lvn_LogLevel_Warn, "hello world %s , %d", c, 42);
    char* str = (char*) malloc(len * sizeof(char));
    lvnLogFormatMessage(logger, str, len, Lvn_LogLevel_Warn, "hello world %s , %d", c, 42);

    printf("%.*s", len, str);
    free(str);

    // create a new logger
    LvnSink sink =
    {
        .logFunc = myPrint,
    };

    LvnLoggerCreateInfo logCreateInfo =
    {
        .name = "mylogger",
        .level = Lvn_LogLevel_None,
        .format = "[%Y-%m-%d] [%T] [%#%l%^] %n: %v%$",
        .pSinks = &sink,
        .sinkCount = 1,
    };

    LvnLogger* mylogger;
    lvnCreateLogger(ctx, &mylogger, &logCreateInfo);

    // log messages using our custom logger
    lvnLogMessageTrace(mylogger, "this is a log message");
    lvnLogMessageInfo(mylogger, "the value of pi is %f", 3.1415);

    // add a logger pattern to our logger
    LvnLogPattern logPattern =
    {
        .symbol = '>',
        .func = myloggerPattern,
    };

    lvnCtxAddLogPatterns(ctx, &logPattern, 1);

    lvnLogParseLogPatternFormat(mylogger, "[%Y-%m-%d] [%#%l%^] %> %n: %v%$");

    lvnLogMessageTrace(mylogger, "this is a log message");
    lvnLogMessageInfo(mylogger, "the value of pi is %f", 3.1415);

    // destroy the logger when we no longer need to use it
    lvnDestroyLogger(mylogger);
}
