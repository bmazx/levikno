#include <levikno/levikno.h>
#include <levikno/lvn_graphics.h>
#include <stdio.h>
#include <stdlib.h>

void myPrint(const char* msg)
{
    printf("%s", msg);
}

char* myLogPattern(const LvnLogMessage* logmsg)
{
    return lvnLogCreateOneShotStrMsg(">>>");
}

int main(int argc, char** argv)
{
    LvnContextCreateInfo ctxCreateInfo =
    {
        .logging.enableLogging = true,
    };

    LvnContext* ctx;
    lvnCreateContext(&ctx, &ctxCreateInfo);

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
        .format = "[%Y-%m-%d] [%T] [%#%l%^] %> %n: %v%$",
        .pSinks = &sink,
        .sinkCount = 1,
    };

    LvnLogger* mylog;
    lvnCreateLogger(ctx, &mylog, &logCreateInfo);

    LvnLogPattern logPattern =
    {
        .symbol = '>',
        .func = myLogPattern,
    };

    lvnCtxAddLogPatterns(ctx, &logPattern, 1);

    lvnLogParseLogPatternFormat(mylog, "[%Y-%m-%d] [%#%l%^] %> %n: %v%$");

    lvnLogMessageDebug(mylog, "hello there %f", 3.1415);
    lvnLogMessageError(mylog, "hello there %f", 3.1415);

    lvnDestroyLogger(mylog);


    LvnGraphicsContextCreateInfo graphicsCreateInfo = {0};
    graphicsCreateInfo.graphicsapi = Lvn_GraphicsApi_Vulkan;
    graphicsCreateInfo.enableGraphicsApiDebugLogging = true;
    graphicsCreateInfo.presentationModeFlags = Lvn_PresentationModeFlag_Headless | Lvn_PresentationModeFlag_Surface;

    LvnGraphicsContext* graphicsctx;
    lvnCreateGraphicsContext(ctx, &graphicsctx, &graphicsCreateInfo);

    lvnDestroyGraphicsContext(graphicsctx);

    lvnDestroyContext(ctx);
}
