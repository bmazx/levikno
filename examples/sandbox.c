#include <levikno/levikno.h>
#include <levikno/lvn_graphics.h>
#include <stdio.h>
#include <stdlib.h>

#include <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_WAYLAND
#include <GLFW/glfw3native.h>

void myPrint(const char* msg)
{
    printf("%s", msg);
}

char* myLogPattern(const LvnLogMessage* logmsg)
{
    return lvnLogCreateOneShotStrMsg(">>>");
}

static void GLFWerrorCallback(int error, const char* descripion)
{
    printf("[glfw]: (%d): %s\n", error, descripion);
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



    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwSetErrorCallback(GLFWerrorCallback);

    /* Create a windowed mode window and its OpenGL context */
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(800, 600, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    struct wl_display* wldisplay = glfwGetWaylandDisplay();
    struct wl_surface* wlsurface = glfwGetWaylandWindow(window);

    LvnPlatformData pd = {0};
    pd.nativeDisplayHandle = wldisplay;
    pd.nativeWindowHandle = wlsurface;

    LvnGraphicsContextCreateInfo graphicsCreateInfo = {0};
    graphicsCreateInfo.graphicsapi = Lvn_GraphicsApi_Vulkan;
    graphicsCreateInfo.presentationModeFlags = Lvn_PresentationModeFlag_Headless | Lvn_PresentationModeFlag_Surface;
    graphicsCreateInfo.platformData = &pd;
    graphicsCreateInfo.enableGraphicsApiDebugLogging = true;

    LvnGraphicsContext* graphicsctx;
    lvnCreateGraphicsContext(ctx, &graphicsctx, &graphicsCreateInfo);

    LvnSurfaceCreateInfo sci = {0};
    sci.nativeDisplayHandle = wldisplay;
    sci.nativeWindowHandle = wlsurface;
    sci.width = 600;
    sci.height = 800;

    LvnSurface* surface;
    lvnCreateSurface(graphicsctx, &surface, &sci);

    LvnFile vertfile = lvnLoadFileBin("res/shaders/vert.spv");
    LvnFile fragfile = lvnLoadFileBin("res/shaders/frag.spv");

    LvnShaderCreateInfo vertShCreateInfo = {0};
    vertShCreateInfo.pCode = vertfile.data;
    vertShCreateInfo.codeSize = vertfile.size;

    LvnShader* vertShader;
    lvnCreateShader(graphicsctx, &vertShader, &vertShCreateInfo);

    LvnShaderCreateInfo fragShCreateInfo = {0};
    fragShCreateInfo.pCode = fragfile.data;
    fragShCreateInfo.codeSize = fragfile.size;

    LvnShader* fragShader;
    lvnCreateShader(graphicsctx, &fragShader, &fragShCreateInfo);

    LvnPipelineShaderStageCreateInfo stages[] =
    {
        { Lvn_ShaderStage_Vertex, vertShader, "main" },
        { Lvn_ShaderStage_Fragment, fragShader, "main" },
    };

    LvnRenderPass* renderPass = lvnSurfaceGetRenderPass(surface);
    LvnPipelineFixedFunctions pipelineFixedFuncs = lvnConfigPipelineFixedFunctions();
    pipelineFixedFuncs.viewport.width = 800;
    pipelineFixedFuncs.viewport.height = 600;
    pipelineFixedFuncs.scissor.extent.width = 800;
    pipelineFixedFuncs.scissor.extent.height = 600;

    LvnPipelineCreateInfo pipelineCreateInfo = {0};
    pipelineCreateInfo.pipelineFixedFunctions = &pipelineFixedFuncs;
    pipelineCreateInfo.pVertexAttributes = NULL;
    pipelineCreateInfo.vertexAttributeCount = 0;
    pipelineCreateInfo.pVertexBindingDescriptions = NULL;
    pipelineCreateInfo.vertexBindingDescriptionCount = 0;
    pipelineCreateInfo.pDescriptorLayouts = NULL;
    pipelineCreateInfo.descriptorLayoutCount = 0;
    pipelineCreateInfo.pStages = stages;
    pipelineCreateInfo.stageCount = LVN_ARRAY_LEN(stages);
    pipelineCreateInfo.renderPass = renderPass;

    LvnPipeline* pipeline;
    lvnCreatePipeline(graphicsctx, &pipeline, &pipelineCreateInfo);

    lvnDestroyShader(vertShader);
    lvnDestroyShader(fragShader);
    lvnUnloadFile(&vertfile);
    lvnUnloadFile(&fragfile);

    // while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }

    lvnDestroyPipeline(pipeline);
    lvnDestroySurface(surface);
    lvnDestroyGraphicsContext(graphicsctx);

    lvnDestroyContext(ctx);

    glfwDestroyWindow(window);
    glfwTerminate();
}
