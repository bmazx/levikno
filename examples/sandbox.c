#include <levikno/levikno.h>
#include <levikno/lvn_graphics.h>
#include <stdio.h>
#include <stdlib.h>

#include <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_WAYLAND
#define GLFW_EXPOSE_NATIVE_X11
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

    // struct wl_display* wldisplay = glfwGetWaylandDisplay();
    // struct wl_surface* wlsurface = glfwGetWaylandWindow(window);

    Display* x11display = glfwGetX11Display();
    Window x11win = glfwGetX11Window(window);

    LvnPlatformData pd = {0};
    pd.nativeDisplayHandle = x11display;
    pd.nativeWindowHandle = &x11win;

    LvnGraphicsContextCreateInfo graphicsCreateInfo = {0};
    graphicsCreateInfo.graphicsapi = Lvn_GraphicsApi_Vulkan;
    graphicsCreateInfo.presentationModeFlags = Lvn_PresentationModeFlag_Headless | Lvn_PresentationModeFlag_Surface;
    graphicsCreateInfo.platformData = &pd;
    graphicsCreateInfo.enableGraphicsApiDebugLogging = true;

    LvnGraphicsContext* graphicsctx;
    lvnCreateGraphicsContext(ctx, &graphicsctx, &graphicsCreateInfo);

    LvnSurfaceCreateInfo sci = {0};
    sci.nativeDisplayHandle = x11display;
    sci.nativeWindowHandle = &x11win;
    sci.width = 600;
    sci.height = 800;
    sci.surfaceFormat = Lvn_Format_B8G8R8A8_SRGB;
    sci.presentMode = Lvn_PresentMode_Mailbox;
    sci.minImageCount = 3;

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

    LvnPipelineFixedFunctions pipelineFixedFuncs = lvnConfigPipelineFixedFunctionsInit();
    pipelineFixedFuncs.viewport.width = 800;
    pipelineFixedFuncs.viewport.height = 600;
    pipelineFixedFuncs.scissor.extent.width = 800;
    pipelineFixedFuncs.scissor.extent.height = 600;

    LvnFormat colorFormat = lvnSurfaceGetSwapchainFormat(surface);

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
    pipelineCreateInfo.pColorAttachmentFormats = &colorFormat;
    pipelineCreateInfo.colorAttachmentCount = 1;

    LvnPipeline* pipeline;
    lvnCreatePipeline(graphicsctx, &pipeline, &pipelineCreateInfo);

    lvnDestroyShader(vertShader);
    lvnDestroyShader(fragShader);
    lvnUnloadFile(&vertfile);
    lvnUnloadFile(&fragfile);

    LvnCommandBufferCreateInfo cmdBuffCreateInfo = {0};

    LvnCommandBuffer* cmdBuff;
    lvnCreateCommandBuffer(graphicsctx, &cmdBuff, &cmdBuffCreateInfo);

    LvnFence* fence;
    lvnCreateFence(graphicsctx, &fence);

    LvnSemaphore* imageWaitSemaphore;
    lvnCreateSemaphore(graphicsctx, &imageWaitSemaphore);

    // NOTE: hard coding to be set number of images temporarily for now
    LvnSemaphore* renderFinishedSemaphores[12];
    for (uint32_t i = 0; i < 12; i++)
        lvnCreateSemaphore(graphicsctx, &renderFinishedSemaphores[i]);

    uint32_t imageIndex = 0;
    int width = 0, height = 0, oldWidth = 0, oldHeight = 0;
    LvnResult result;

    while (!glfwWindowShouldClose(window))
    {
        glfwGetWindowSize(window, &width, &height);

        lvnFenceWait(fence, UINT64_MAX);
        lvnFenceReset(fence);

        result = lvnSurfaceAcquireNextImage(surface, imageWaitSemaphore, NULL, &imageIndex);

        if (result == Lvn_Result_OutOfDate)
        {
            lvnSurfaceResize(surface, width, height);
            oldWidth = width;
            oldHeight = height;
            continue;
        }
        else if (result != Lvn_Result_Success)
        {
            LVN_LOG_ERROR(logger, "failed to get image");
            continue;
        }

        LvnExtent2D extent = lvnSurfaceGetSwapchainExtent(surface);

        lvnBeginCommandBuffer(cmdBuff);

        LvnRenderingAttachmentInfo colorAttachment =
        {
            .loadOp = Lvn_AttachmentLoadOp_Clear,
            .storeOp = Lvn_AttachmentStoreOp_Store,
            .clearValue.color = {{ 0.0f, 0.0f, 0.0f, 0.0f }},
            .imageView = lvnSurfaceGetSwapchainImageView(surface, imageIndex),
        };

        LvnRenderingInfo renderInfo = {0};
        renderInfo.renderArea.extent.width = extent.width;
        renderInfo.renderArea.extent.height = extent.height;
        renderInfo.renderArea.offset.x = 0;
        renderInfo.renderArea.offset.y = 0;
        renderInfo.pColorAttachments = &colorAttachment;
        renderInfo.colorAttachmentCount = 1;

        lvnCmdBeginRendering(cmdBuff, &renderInfo);

        lvnCmdBindPipeline(cmdBuff, pipeline);

        LvnViewport viewport = {
            .x = 0, .y = 0,
            .width = extent.width, .height = extent.height,
            .minDepth = 0.0f, .maxDepth = 1.0f,
        };

        lvnCmdSetViewport(cmdBuff, &viewport);

        LvnRenderArea renderArea = {
            .extent = { extent.width, extent.height },
            .offset = { 0.0f, 0.0f },
        };

        lvnCmdSetScissor(cmdBuff, &renderArea);

        lvnCmdDraw(cmdBuff, 3, 1, 0, 0);

        lvnCmdEndRendering(cmdBuff);

        lvnEndCommandBuffer(cmdBuff);

        LvnSubmitInfo submitInfo = {
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &imageWaitSemaphore,
            .signalSemaphoreCount = 1,
            .pSignalSemaphores = &renderFinishedSemaphores[imageIndex],
            .commandBufferCount = 1,
            .pCommandBuffers = &cmdBuff,
        };
        lvnRenderSubmit(graphicsctx, &submitInfo, 1, fence);

        LvnPresentInfo presentInfo = {
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &renderFinishedSemaphores[imageIndex],
            .surfaceCount = 1,
            .pSurfaces = &surface,
            .pImageIndices = &imageIndex,
        };
        result = lvnRenderPresent(graphicsctx, &presentInfo);

        if (result == Lvn_Result_OutOfDate || width != oldWidth || height != oldHeight)
        {
            glfwGetWindowSize(window, &width, &height);
            lvnSurfaceResize(surface, width, height);
        }

        oldWidth = width;
        oldHeight = height;

        glfwPollEvents();
    }

    lvnDestroyFence(fence);
    lvnDestroySemaphore(imageWaitSemaphore);
    for (uint32_t i = 0; i < 12; i++)
        lvnDestroySemaphore(renderFinishedSemaphores[i]);
    lvnDestroyCommandBuffer(cmdBuff);
    lvnDestroyPipeline(pipeline);
    lvnDestroySurface(surface);
    lvnDestroyGraphicsContext(graphicsctx);

    lvnDestroyContext(ctx);

    glfwDestroyWindow(window);
    glfwTerminate();
}
