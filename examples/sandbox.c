#include <levikno/levikno.h>
#include <stdio.h>
#include <stdlib.h>

void myPrint(const char* msg)
{
    printf("%s", msg);
}

int main(int argc, char** argv)
{
    LvnContext* ctx;
    lvnCreateContext(&ctx, NULL);

    LvnLogger* logger = lvnCtxGetCoreLogger(ctx);

    float b = 3.1415f;
    const char* c = "world";

    lvnLogMessageTrace(logger, "hello %d | %s | %.5f | %p", 12, c, b, &b);
    lvnLogMessageDebug(logger, "hello %d | %s | %.5f | %p", 12, c, b, &b);
    lvnLogMessageInfo(logger, "hello %d | %s | %.5f | %p", 12, c, b, &b);
    lvnLogMessageWarn(logger, "hello %d | %s | %.5f | %p", 12, c, b, &b);
    lvnLogMessageError(logger, "hello %d | %s | %.5f | %p", 12, c, b, &b);
    lvnLogMessageFatal(logger, "hello %d | %s | %.5f | %p", 12, c, b, &b);

    uint32_t len = lvnLogFormatMessageArgs(logger, NULL, 0, Lvn_LogLevel_Warn, "hello world %s , %d", c, 42);
    printf("%u\n", len);
    char* str = (char*) malloc(len * sizeof(char));
    lvnLogFormatMessageArgs(logger, str, len, Lvn_LogLevel_Warn, "hello world %s , %d", c, 42);

    printf("%.*s", len, str);
    free(str);

    LvnSink sink =
    {
        .logFunc = myPrint,
    };

    LvnLoggerCreateInfo logCreateInfo =
    {
        .name = "myLog",
        .level = Lvn_LogLevel_None,
        .format = "[%Y-%m-%d] [%T] [%#%l%^] >>> %n: %v%$",
        .pSinks = &sink,
        .sinkCount = 1,
    };

    LvnLogger* mylog;
    lvnCreateLogger(ctx, &mylog, &logCreateInfo);

    lvnLogMessageDebug(mylog, "hello there %f", 3.1415);
    lvnLogMessageError(mylog, "hello there %f", 3.1415);

    lvnDestroyLogger(mylog);

    lvnDestroyContext(ctx);
}
