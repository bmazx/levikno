
#include <levikno/levikno.h>
#include <levikno/lvn_graphics.h>
#include <stdio.h>
#include <stdlib.h>

#include <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_WAYLAND
#define GLFW_EXPOSE_NATIVE_X11
#include <GLFW/glfw3native.h>

typedef struct WindowData
{
    bool framebufferResized;
    int width, height;
} WindowData;

static float s_Vertices[] = {
    0.0f,-0.5f, 1.0f, 0.0f, 0.0f,
   -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
    0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
};

static uint32_t s_Indices[] = {
    0, 1, 2,
};

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

static size_t s_MemAllocCount = 0;

void* customMalloc(size_t size, void* userData)
{
    if (!size) { return NULL; }
    void* ptr = malloc(size);
    if (!ptr) { printf("alloc fail\n"); exit(-1); }
    s_MemAllocCount++;
    return ptr;
}

void customFree(void* ptr, void* userData)
{
    if (!ptr) { return; }
    s_MemAllocCount--;
    free(ptr);
}

void* customRealloc(void* ptr, size_t size, void* userData)
{
    if (!ptr) { return customMalloc(size, userData); }
    void* newptr = realloc(ptr, size);
    if (!newptr) { printf("realloc fail\n"); exit(-1); }
    return newptr;
}

void resizeFramebuffers(const LvnGraphicsContext* graphicsctx, LvnSwapchain* swapchain, LvnRenderPass* renderPass, LvnFramebuffer*** swapchainFramebuffers, uint32_t frameBufferCount, uint32_t width, uint32_t height)
{
    for (uint32_t i = 0; i < frameBufferCount; i++)
    {
        lvnDestroyFramebuffer((*swapchainFramebuffers)[i]);
    }
    free(*swapchainFramebuffers);

    uint32_t imageCount = lvnSwapchainGetImageCount(swapchain);

    *swapchainFramebuffers = (LvnFramebuffer**) malloc(imageCount * sizeof(LvnFramebuffer*));

    for (uint32_t i = 0; i < imageCount; i++)
    {
        LvnTexture* swapchainImage = lvnSwapchainGetImage(swapchain, i);
        LvnFramebufferCreateInfo framebufferCreateInfo = {
            .renderPass = renderPass,
            .pColorAttachments = &swapchainImage,
            .colorAttachmentCount = 1,
            .width = width,
            .height = height,
        };

        lvnCreateFramebuffer(graphicsctx, &(*swapchainFramebuffers)[i], &framebufferCreateInfo);
    }
}

static void framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    WindowData* winData = (WindowData*) glfwGetWindowUserPointer(window);
    winData->framebufferResized = true;
    winData->width = width;
    winData->height = height;
}

int main(int argc, char** argv)
{
    lvnSetMemAllocCallbacks(customMalloc, customFree, customRealloc, NULL);

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

    WindowData winData = {0};
    glfwSetWindowUserPointer(window, &winData);

    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);

    // struct wl_display* nativeDisplay = glfwGetWaylandDisplay();
    // struct wl_surface* nativeWindow = glfwGetWaylandWindow(window);

    Display* nativeDisplay = glfwGetX11Display();
    Window nativeWindow = glfwGetX11Window(window);

    LvnPlatformData pd = {0};
    pd.ndh = nativeDisplay;
    pd.nwh = &nativeWindow;

    LvnGraphicsContextCreateInfo graphicsCreateInfo = {0};
    graphicsCreateInfo.graphicsapi = Lvn_GraphicsApi_Vulkan;
    graphicsCreateInfo.presentationModeFlags = Lvn_PresentationModeFlag_Headless | Lvn_PresentationModeFlag_Surface;
    graphicsCreateInfo.platformData = &pd;
    graphicsCreateInfo.enableGraphicsApiDebugLogging = true;

    LvnGraphicsContext* graphicsctx;
    lvnCreateGraphicsContext(ctx, &graphicsctx, &graphicsCreateInfo);

    LvnSurfaceCreateInfo sci = {0};
    sci.nativeDisplayHandle = nativeDisplay;
    sci.nativeWindowHandle = &nativeWindow;

    LvnSurface* surface;
    lvnCreateSurface(graphicsctx, &surface, &sci);

    uint32_t formatCount;
    lvnSurfaceGetSupportedFormats(surface, &formatCount, NULL);

    LvnFormat* formats = (LvnFormat*) malloc(formatCount * sizeof(LvnFormat));
    lvnSurfaceGetSupportedFormats(surface, &formatCount, formats);

    LvnFormat selFormat = formats[0];
    for (uint32_t i = 0; i < formatCount; i++)
    {
        if (formats[i] == Lvn_Format_BGRA8_SRGB)
        {
            selFormat = formats[i];
            printf("found surface format\n");
            break;
        }
    }

    free(formats);

    uint32_t presentModeCount;
    lvnSurfaceGetSupportedPresentModes(surface, &presentModeCount, NULL);

    LvnPresentMode* presentModes = (LvnPresentMode*) malloc(presentModeCount * sizeof(LvnPresentMode));
    lvnSurfaceGetSupportedPresentModes(surface, &presentModeCount, presentModes);

    LvnPresentMode selPresentMode = Lvn_PresentMode_FIFO;
    for (uint32_t i = 0; i < presentModeCount; i++)
    {
        if (presentModes[i] == Lvn_PresentMode_Mailbox)
        {
            selPresentMode = presentModes[i];
            printf("found present mode\n");
        }
    }

    free(presentModes);

    LvnSwapchainCreateInfo swapchainCreateInfo = {0};
    swapchainCreateInfo.surface = surface;
    swapchainCreateInfo.width = 800;
    swapchainCreateInfo.height = 600;
    swapchainCreateInfo.surfaceFormat = selFormat;
    swapchainCreateInfo.presentMode = selPresentMode;
    swapchainCreateInfo.minImageCount = 3;

    LvnSwapchain* swapchain;
    lvnCreateSwapchain(graphicsctx, &swapchain, &swapchainCreateInfo);

    LvnColorAttachment colorAttachment = {
        .usage = Lvn_AttachmentUsage_PresentSrc,
        .format = Lvn_Format_BGRA8_SRGB,
        .samples = Lvn_SampleCountFlag_1_Bit,
        .loadOp = Lvn_AttachmentLoadOp_Clear,
        .storeOp = Lvn_AttachmentStoreOp_Store,
    };

    LvnRenderPassCreateInfo renderPassCreateInfo = {
        .pColorAttachments = &colorAttachment,
        .colorAttachmentCount = 1,
        .depthStencilAttachment = NULL,
    };

    LvnRenderPass* renderPass;
    lvnCreateRenderPass(graphicsctx, &renderPass, &renderPassCreateInfo);

    uint32_t imageCount = lvnSwapchainGetImageCount(swapchain);
    LvnExtent2D extent = lvnSwapchainGetExtent(swapchain);

    LvnFramebuffer** swapchainFramebuffers = (LvnFramebuffer**) malloc(imageCount * sizeof(LvnFramebuffer*));

    for (uint32_t i = 0; i < imageCount; i++)
    {
        LvnTexture* swapchainImage = lvnSwapchainGetImage(swapchain, i);
        LvnFramebufferCreateInfo framebufferCreateInfo = {
            .renderPass = renderPass,
            .pColorAttachments = &swapchainImage,
            .colorAttachmentCount = 1,
            .width = extent.width,
            .height = extent.height,
        };

        lvnCreateFramebuffer(graphicsctx, &swapchainFramebuffers[i], &framebufferCreateInfo);
    }

    LvnFile vertfile = lvnLoadFileBin("/home/bma/Documents/dev/levikno/examples/res/shaders/vert.spv");
    LvnFile fragfile = lvnLoadFileBin("/home/bma/Documents/dev/levikno/examples/res/shaders/frag.spv");

    LvnShaderCreateInfo vertShCreateInfo = {0};
    vertShCreateInfo.pCode = vertfile.data;
    vertShCreateInfo.codeSize = vertfile.size;
    vertShCreateInfo.stage = Lvn_ShaderStage_Vertex;
    vertShCreateInfo.entryPoint = "main";

    LvnShader* vertShader;
    lvnCreateShader(graphicsctx, &vertShader, &vertShCreateInfo);

    LvnShaderCreateInfo fragShCreateInfo = {0};
    fragShCreateInfo.pCode = fragfile.data;
    fragShCreateInfo.codeSize = fragfile.size;
    fragShCreateInfo.stage = Lvn_ShaderStage_Fragment;
    fragShCreateInfo.entryPoint = "main";

    LvnShader* fragShader;
    lvnCreateShader(graphicsctx, &fragShader, &fragShCreateInfo);

    LvnPipelineFixedFunctions pipelineFixedFuncs = lvnConfigPipelineFixedFunctionsInit();

    LvnVertexAttribute attributes[2] =
    {
        { 0, 0, Lvn_AttributeFormat_Vec2_f32, 0 },
        { 0, 1, Lvn_AttributeFormat_Vec3_f32, (2 * sizeof(float)) },
    };

    LvnVertexBindingDescription vertexBindingDescription = {
        .binding = 0,
        .stride = 5 * sizeof(float),
    };

    LvnShader* shaderStages[] =
    {
        vertShader, fragShader,
    };

    LvnPipelineCreateInfo pipelineCreateInfo = {0};
    pipelineCreateInfo.pipelineFixedFunctions = &pipelineFixedFuncs;
    pipelineCreateInfo.pVertexAttributes = attributes;
    pipelineCreateInfo.vertexAttributeCount = LVN_ARRAY_LEN(attributes);
    pipelineCreateInfo.pVertexBindingDescriptions = &vertexBindingDescription;
    pipelineCreateInfo.vertexBindingDescriptionCount = 1;
    pipelineCreateInfo.pDescriptorLayouts = NULL;
    pipelineCreateInfo.descriptorLayoutCount = 0;
    pipelineCreateInfo.pShaderStages = shaderStages;
    pipelineCreateInfo.stageCount = LVN_ARRAY_LEN(shaderStages);
    pipelineCreateInfo.renderPass = renderPass;

    LvnPipeline* pipeline;
    lvnCreatePipeline(graphicsctx, &pipeline, &pipelineCreateInfo);

    lvnDestroyShader(vertShader);
    lvnDestroyShader(fragShader);
    lvnUnloadFile(&vertfile);
    lvnUnloadFile(&fragfile);

    LvnCommandBufferAllocInfo cmdBuffAllocInfo = {
        .level = Lvn_CommandBufferLevel_Primary,
        .count = 1,
    };

    LvnCommandBuffer* cmdBuff;
    lvnAllocateCommandBuffers(graphicsctx, &cmdBuffAllocInfo, &cmdBuff);

    LvnFence* fence;
    lvnCreateFence(graphicsctx, &fence);

    LvnSemaphore* imageWaitSemaphore;
    lvnCreateSemaphore(graphicsctx, &imageWaitSemaphore);

    // NOTE: hard coding to be set number of images temporarily for now
    LvnSemaphore* renderFinishedSemaphores[12];
    for (uint32_t i = 0; i < 12; i++)
        lvnCreateSemaphore(graphicsctx, &renderFinishedSemaphores[i]);


    // vertex buffer create info struct
    LvnBufferCreateInfo bufferCreateInfo = {
        .type = Lvn_BufferTypeFlag_Vertex,
        .usage = Lvn_BufferMemoryUsage_CpuToGpu,
        .data = s_Vertices,
        .size = sizeof(s_Vertices),
    };

    // create buffer
    LvnBuffer* vertexBuffer;
    lvnCreateBuffer(graphicsctx, &vertexBuffer, &bufferCreateInfo);

    // index buffer create info struct
    bufferCreateInfo.type = Lvn_BufferTypeFlag_Index;
    bufferCreateInfo.usage = Lvn_BufferMemoryUsage_CpuToGpu;
    bufferCreateInfo.data = s_Indices;
    bufferCreateInfo.size = sizeof(s_Indices);

    // create buffer
    LvnBuffer* indexBuffer;
    lvnCreateBuffer(graphicsctx, &indexBuffer, &bufferCreateInfo);

    LvnSamplerCreateInfo samplerCreateInfo = {
        .magFilter = Lvn_TextureFilter_Nearest,
        .minFilter = Lvn_TextureFilter_Nearest,
        .wrapR = Lvn_TextureMode_Repeat,
        .wrapS = Lvn_TextureMode_Repeat,
        .wrapT = Lvn_TextureMode_Repeat,
    };

    LvnSampler* sampler;
    lvnCreateSampler(graphicsctx, &sampler, &samplerCreateInfo);

    LvnImage image = lvnLoadImageEx("/home/bma/Documents/textures/woodBox.jpg", 4, false);

    LvnTextureCreateInfo textureCreateInfo = {
        .format = Lvn_Format_RGBA8_SRGB,
        .image = &image,
        .sampler = sampler,
        .samples = Lvn_SampleCountFlag_1_Bit,
        .width = image.width,
        .height = image.height,
    };

    LvnTexture* texture;
    lvnCreateTexture(graphicsctx, &texture, &textureCreateInfo);

    LvnResult result;
    uint32_t imageIndex = 0;

    while (!glfwWindowShouldClose(window))
    {
        lvnFenceWait(fence, UINT64_MAX);
        lvnFenceReset(fence);

        result = lvnSwapchainAcquireNextImage(swapchain, imageWaitSemaphore, NULL, &imageIndex);

        if (result == Lvn_Result_OutOfDate)
        {
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);
            lvnSwapchainResize(swapchain, width, height);
            extent = lvnSwapchainGetExtent(swapchain);
            resizeFramebuffers(graphicsctx, swapchain, renderPass, &swapchainFramebuffers, imageCount, extent.width, extent.height);
            imageCount = lvnSwapchainGetImageCount(swapchain);
            continue;
        }
        else if (result != Lvn_Result_Success)
        {
            LVN_LOG_ERROR(logger, "failed to get image");
            continue;
        }


        lvnBeginCommandBuffer(cmdBuff);

        LvnClearValue clearValues[] = {
            {{{0.0f, 0.0f, 0.0f}}},
        };

        LvnRenderPassBeginInfo beginInfo = {
            .renderPass = renderPass,
            .framebuffer = swapchainFramebuffers[imageIndex],
            .renderArea = {{extent.width, extent.height}, {0, 0}},
            .pClearValues = clearValues,
            .clearValueCount = 1,
        };

        lvnCmdBeginRenderPass(cmdBuff, &beginInfo);

        lvnCmdBindPipeline(cmdBuff, pipeline);

        LvnViewport viewport = {
            .x = 0, .y = 0,
            .width = extent.width, .height = extent.height,
            .minDepth = 0.0f, .maxDepth = 1.0f,
        };

        lvnCmdSetViewport(cmdBuff, &viewport);

        LvnRenderArea renderArea = {
            .extent = extent,
            .offset = { 0.0f, 0.0f },
        };

        lvnCmdSetScissor(cmdBuff, &renderArea);

        uint64_t offsets[] = {0};
        lvnCmdBindVertexBuffer(cmdBuff, 0, 1, &vertexBuffer, offsets);
        lvnCmdBindIndexBuffer(cmdBuff, indexBuffer, 0);

        lvnCmdDrawIndexed(cmdBuff, LVN_ARRAY_LEN(s_Indices), 1, 0, 0, 0);

        lvnCmdEndRenderPass(cmdBuff);
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
            .swapchainCount = 1,
            .pSwapchains = &swapchain,
            .pImageIndices = &imageIndex,
        };
        result = lvnRenderPresent(graphicsctx, &presentInfo);

        if (result == Lvn_Result_OutOfDate || winData.framebufferResized)
        {
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);
            lvnSwapchainResize(swapchain, width, height);
            extent = lvnSwapchainGetExtent(swapchain);
            resizeFramebuffers(graphicsctx, swapchain, renderPass, &swapchainFramebuffers, imageCount, extent.width, extent.height);
            imageCount = lvnSwapchainGetImageCount(swapchain);
            winData.framebufferResized = false;
        }

        glfwPollEvents();
    }

    lvnUnloadImage(&image);

    lvnDestroyTexture(texture);
    lvnDestroySampler(sampler);
    lvnDestroyBuffer(vertexBuffer);
    lvnDestroyBuffer(indexBuffer);
    lvnDestroyFence(fence);
    lvnDestroySemaphore(imageWaitSemaphore);
    for (uint32_t i = 0; i < 12; i++)
        lvnDestroySemaphore(renderFinishedSemaphores[i]);
    lvnDestroyPipeline(pipeline);

    for (uint32_t i = 0; i < imageCount; i++)
        lvnDestroyFramebuffer(swapchainFramebuffers[i]);
    lvnDestroyRenderPass(renderPass);
    lvnDestroySwapchain(swapchain);
    lvnDestroySurface(surface);
    lvnDestroyGraphicsContext(graphicsctx);

    lvnDestroyContext(ctx);

    glfwDestroyWindow(window);
    glfwTerminate();

    printf("s_MemAllocCount: %zu\n", s_MemAllocCount);
}
