#include <levikno/levikno.h>
#include <levikno/lvn_graphics.h>

#define LVN_GMATH_WHITELIST_INCLUDES
#define LVN_GMATH_INCLUDE_GRAPHICS_ESSENTIAL
#define LVN_GMATH_IMPL
#include <levikno/lvn_gmath.h>

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

typedef struct UniformData
{
    LvnMat4 matrix;
} UniformData;

static float s_Vertices[] = {
    /*    pos (x,y,z)    |      UV   */
    -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,    1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,    1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,    1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,    0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,    1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,    1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,    1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,    0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,    1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,    1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,    1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,    1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,    1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,    0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,    0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,    0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,    1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,    1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,    1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,    1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,    0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,    1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,    1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,    1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,    0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,    0.0f, 1.0f
};

static uint32_t s_Indices[] = {
    0, 1, 2,
    0, 2, 3,
};

void myPrint(const char* msg)
{
    printf("%s", msg);
}

char* myloggerPattern(const LvnLogMessage* logmsg)
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
            .colorAttachmentCount = 1,
            .pColorAttachments = &swapchainImage,
            .depthStencilAttachment = lvnSwapchainGetDepthImage(swapchain),
            .width = width,
            .height = height,
        };

        lvnCreateFramebuffer(graphicsctx, &(*swapchainFramebuffers)[i], &framebufferCreateInfo);
    }
}

void resizeSemaphores(const LvnGraphicsContext* graphicsctx, LvnSwapchain* swapchain, LvnSemaphore*** pSemaphores, uint32_t oldImageCount)
{
    uint32_t newImageCount = lvnSwapchainGetImageCount(swapchain);
    for (uint32_t i = 0; i < oldImageCount; i++)
        lvnDestroySemaphore((*pSemaphores)[i]);
    *pSemaphores = realloc(*pSemaphores, newImageCount * sizeof(LvnSemaphore*));
    for (uint32_t i = 0; i < newImageCount; i++)
        lvnCreateSemaphore(graphicsctx, &(*pSemaphores)[i]);
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
        .name = "mylogger",
        .level = Lvn_LogLevel_None,
        .format = "[%Y-%m-%d] [%T] [%#%l%^] %n: %v%$",
        .pSinks = &sink,
        .sinkCount = 1,
    };

    LvnLogger* mylogger;
    lvnCreateLogger(ctx, &mylogger, &logCreateInfo);

    lvnLogMessageTrace(mylogger, "this is a log message");
    lvnLogMessageInfo(mylogger, "the value of pi is %f", 3.1415);

    LvnLogPattern logPattern =
    {
        .symbol = '>',
        .func = myloggerPattern,
    };

    lvnCtxAddLogPatterns(ctx, &logPattern, 1);

    lvnLogParseLogPatternFormat(mylogger, "[%Y-%m-%d] [%#%l%^] %> %n: %v%$");

    lvnLogMessageTrace(mylogger, "this is a log message");
    lvnLogMessageInfo(mylogger, "the value of pi is %f", 3.1415);

    lvnDestroyLogger(mylogger);



    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwSetErrorCallback(GLFWerrorCallback);

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(800, 600, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwSetWindowSizeLimits(window, 400, 300, GLFW_DONT_CARE, GLFW_DONT_CARE);

    WindowData winData = {0};
    glfwSetWindowUserPointer(window, &winData);

    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);

    // struct wl_display* nativeDisplay = glfwGetWaylandDisplay();
    // struct wl_surface* nativeWindow = glfwGetWaylandWindow(window);

    Display* nativeDisplay = glfwGetX11Display();
    Window nativeWindow = glfwGetX11Window(window);

    LvnPlatformData pd = {0};
    pd.ndh = nativeDisplay;
    pd.nwh = (void*)nativeWindow;

    LvnGraphicsContextCreateInfo graphicsCreateInfo = {0};
    graphicsCreateInfo.graphicsapi = Lvn_GraphicsApi_Vulkan;
    graphicsCreateInfo.presentationModeFlags = Lvn_PresentationModeFlag_Headless | Lvn_PresentationModeFlag_Surface;
    graphicsCreateInfo.platformData = &pd;
    graphicsCreateInfo.enableGraphicsApiDebugLogging = true;

    LvnGraphicsContext* graphicsctx;
    lvnCreateGraphicsContext(ctx, &graphicsctx, &graphicsCreateInfo);

    const LvnSurface* surface = lvnGetSurface(graphicsctx);

    uint32_t formatCount;
    lvnSurfaceGetSupportedFormats(surface, &formatCount, NULL);

    LvnFormat* formats = (LvnFormat*) malloc(formatCount * sizeof(LvnFormat));
    lvnSurfaceGetSupportedFormats(surface, &formatCount, formats);

    LvnFormat selFormat = formats[0];
    for (uint32_t i = 0; i < formatCount; i++)
    {
        if (formats[i] == Lvn_Format_B8G8R8A8_SRGB)
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
        if (presentModes[i] == Lvn_PresentMode_FIFO)
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
    swapchainCreateInfo.depthFormat = Lvn_Format_D32_FLOAT;

    LvnSwapchain* swapchain;
    lvnCreateSwapchain(graphicsctx, &swapchain, &swapchainCreateInfo);

    LvnColorAttachment colorAttachment = {
        .usage = Lvn_AttachmentUsage_PresentSrc,
        .format = Lvn_Format_B8G8R8A8_SRGB,
        .samples = Lvn_SampleCountFlag_1_Bit,
        .loadOp = Lvn_AttachmentLoadOp_Clear,
        .storeOp = Lvn_AttachmentStoreOp_Store,
    };

    LvnDepthStencilAttachment depthAttachment = {
        .samples = Lvn_SampleCountFlag_1_Bit,
        .format = Lvn_Format_D32_FLOAT,
        .loadOp = Lvn_AttachmentLoadOp_Clear,
        .storeOp = Lvn_AttachmentStoreOp_Store,
        .stencilLoadOp = Lvn_AttachmentLoadOp_Clear,
        .stencilStoreOp = Lvn_AttachmentStoreOp_Store,
    };

    LvnRenderPassCreateInfo renderPassCreateInfo = {
        .pColorAttachments = &colorAttachment,
        .colorAttachmentCount = 1,
        .depthStencilAttachment = &depthAttachment,
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
            .colorAttachmentCount = 1,
            .pColorAttachments = &swapchainImage,
            .depthStencilAttachment = lvnSwapchainGetDepthImage(swapchain),
            .width = extent.width,
            .height = extent.height,
        };

        lvnCreateFramebuffer(graphicsctx, &swapchainFramebuffers[i], &framebufferCreateInfo);
    }

    LvnFile vertfile = lvnLoadFileSrc("/home/bma/Documents/dev/levikno/examples/res/shaders/vert.spv");
    LvnFile fragfile = lvnLoadFileSrc("/home/bma/Documents/dev/levikno/examples/res/shaders/frag.spv");

    LvnShaderCreateInfo vertShCreateInfo = {0};
    vertShCreateInfo.pCode = vertfile.data;
    vertShCreateInfo.codeSize = vertfile.size;
    vertShCreateInfo.stage = Lvn_ShaderStageFlag_Vertex;
    vertShCreateInfo.entryPoint = "main";

    LvnShader* vertShader;
    lvnCreateShader(graphicsctx, &vertShader, &vertShCreateInfo);

    LvnShaderCreateInfo fragShCreateInfo = {0};
    fragShCreateInfo.pCode = fragfile.data;
    fragShCreateInfo.codeSize = fragfile.size;
    fragShCreateInfo.stage = Lvn_ShaderStageFlag_Fragment;
    fragShCreateInfo.entryPoint = "main";

    LvnShader* fragShader;
    lvnCreateShader(graphicsctx, &fragShader, &fragShCreateInfo);

    // descriptor layout
    LvnDescriptorBinding descriptorBindings[] = {
        { 0, Lvn_DescriptorType_UniformBuffer, Lvn_ShaderStageFlag_Vertex },
        { 1, Lvn_DescriptorType_CombinedImageSampler, Lvn_ShaderStageFlag_Fragment },
    };

    LvnDescriptorLayoutCreateInfo descriptorLayoutCreateInfo = {
        .descriptorBindingCount = LVN_ARRAY_LEN(descriptorBindings),
        .pDescriptorBindings = descriptorBindings,
    };

    LvnDescriptorLayout* descriptorLayout;
    lvnCreateDescriptorLayout(graphicsctx, &descriptorLayout, &descriptorLayoutCreateInfo);

    // descriptor pool
    LvnDescriptorPoolSize descriptorPoolSizes[] = {
        { Lvn_DescriptorType_UniformBuffer, 1, },
        { Lvn_DescriptorType_CombinedImageSampler, 1, },
    };

    LvnDescriptorPoolCreateInfo descriptorPoolCreateInfo = {
        .poolSizeCount = LVN_ARRAY_LEN(descriptorPoolSizes),
        .pPoolSizes = descriptorPoolSizes,
        .maxSets = 1,
    };

    LvnDescriptorPool* descriptorPool;
    lvnCreateDescriptorPool(graphicsctx, &descriptorPool, &descriptorPoolCreateInfo);

    // descriptor set
    LvnDescriptorSetAllocateInfo descriptorSetAllocInfo = {
        .descriptorPool = descriptorPool,
        .descriptorSetCount = 1,
        .pDescriptorLayouts = descriptorLayout,
    };

    LvnDescriptorSet* descriptorSet;
    lvnAllocateDescriptorSets(graphicsctx, &descriptorSet, &descriptorSetAllocInfo);

    // pipeline
    LvnPipelineFixedFunctions pipelineFixedFuncs = lvnConfigPipelineFixedFunctionsInit();
    pipelineFixedFuncs.depthstencil.depthTestEnable = true;
    pipelineFixedFuncs.depthstencil.depthWriteEnable = true;
    pipelineFixedFuncs.depthstencil.depthOpCompare = Lvn_CompareOp_LessOrEqual;

    LvnVertexAttribute attributes[] =
    {
        { 0, 0, Lvn_Format_R32G32B32_FLOAT, 0 },
        { 0, 2, Lvn_Format_R32G32_FLOAT, (3 * sizeof(float)) },
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
    pipelineCreateInfo.pDescriptorLayouts = &descriptorLayout;
    pipelineCreateInfo.descriptorLayoutCount = 1;
    pipelineCreateInfo.pShaderStages = shaderStages;
    pipelineCreateInfo.stageCount = LVN_ARRAY_LEN(shaderStages);
    pipelineCreateInfo.renderPass = renderPass;

    LvnPipeline* pipeline;
    lvnCreatePipeline(graphicsctx, &pipeline, &pipelineCreateInfo);

    lvnDestroyShader(vertShader);
    lvnDestroyShader(fragShader);
    lvnUnloadFile(&vertfile);
    lvnUnloadFile(&fragfile);

    LvnCommandBuffer* cmdBuff;
    lvnCreateCommandBuffer(graphicsctx, &cmdBuff);

    LvnFence* fence;
    lvnCreateFence(graphicsctx, &fence, true);

    LvnSemaphore* imageWaitSemaphore;
    lvnCreateSemaphore(graphicsctx, &imageWaitSemaphore);

    LvnSemaphore** renderFinishedSemaphores = malloc(imageCount * sizeof(LvnSemaphore*));
    for (uint32_t i = 0; i < imageCount; i++)
        lvnCreateSemaphore(graphicsctx, &renderFinishedSemaphores[i]);


    // vertex buffer
    LvnBufferCreateInfo bufferCreateInfo = {
        .type = Lvn_BufferTypeFlag_Vertex,
        .usage = Lvn_BufferMemoryUsage_CpuToGpu,
        .data = s_Vertices,
        .size = sizeof(s_Vertices),
    };

    LvnBuffer* vertexBuffer;
    lvnCreateBuffer(graphicsctx, &vertexBuffer, &bufferCreateInfo);

    // index buffer
    bufferCreateInfo.type = Lvn_BufferTypeFlag_Index;
    bufferCreateInfo.usage = Lvn_BufferMemoryUsage_CpuToGpu;
    bufferCreateInfo.data = s_Indices;
    bufferCreateInfo.size = sizeof(s_Indices);

    LvnBuffer* indexBuffer;
    lvnCreateBuffer(graphicsctx, &indexBuffer, &bufferCreateInfo);

    // uniform buffer
    bufferCreateInfo.type = Lvn_BufferTypeFlag_Uniform;
    bufferCreateInfo.usage = Lvn_BufferMemoryUsage_CpuToGpu;
    bufferCreateInfo.size = sizeof(UniformData);
    bufferCreateInfo.data = NULL;

    LvnBuffer* uniformBuffer;
    lvnCreateBuffer(graphicsctx, &uniformBuffer, &bufferCreateInfo);

    LvnSamplerCreateInfo samplerCreateInfo = {
        .magFilter = Lvn_TextureFilter_Nearest,
        .minFilter = Lvn_TextureFilter_Nearest,
        .wrapR = Lvn_TextureMode_Repeat,
        .wrapS = Lvn_TextureMode_Repeat,
        .wrapT = Lvn_TextureMode_Repeat,
    };

    LvnSampler* sampler;
    lvnCreateSampler(graphicsctx, &sampler, &samplerCreateInfo);

    LvnLoadImageInfo loadImageInfo = {
        .filepath = "res/images/debug.png",
        .forceChannels = 4,
        .flipVertically = true,
    };

    LvnImage image = lvnLoadImage(&loadImageInfo);

    LvnTextureCreateInfo textureCreateInfo = {
        .format = Lvn_Format_R8G8B8A8_SRGB,
        .samples = Lvn_SampleCountFlag_1_Bit,
        .image = image.data,
        .width = image.width,
        .height = image.height,
    };

    LvnTexture* texture;
    lvnCreateTexture(graphicsctx, &texture, &textureCreateInfo);

    LvnResult result;
    uint32_t imageIndex = 0;
    int fps = 0;
    double prevTime = 0.0;

    LvnDescriptorBufferInfo descriptorBufferInfo = {
        .buffer = uniformBuffer,
        .range = sizeof(UniformData),
        .offset = 0,
    };

    LvnDescriptorImageInfo descriptorImageInfo = {
        .texture = texture,
        .sampler = sampler,
    };

    LvnDescriptorSetWriteInfo descriptorWriteInfos[] = {
        {
            .descriptorSet = descriptorSet,
            .descriptorType = Lvn_DescriptorType_UniformBuffer,
            .binding = 0,
            .bufferInfo = &descriptorBufferInfo,
        },
        {
            .descriptorSet = descriptorSet,
            .descriptorType = Lvn_DescriptorType_CombinedImageSampler,
            .binding = 1,
            .imageInfo = &descriptorImageInfo,
        },
    };

    lvnUpdateDescriptorSets(graphicsctx, LVN_ARRAY_LEN(descriptorWriteInfos), descriptorWriteInfos, 0, NULL);

    UniformData uboData = {0};
    lvn_mat4_identity(uboData.matrix);

    while (!glfwWindowShouldClose(window))
    {
        double currTime = glfwGetTime();
        double delta = currTime - prevTime;
        fps++;
        if (delta >= 1.0)
        {
            LVN_LOG_DEBUG(logger, "fps: %d", fps);
            prevTime = currTime;
            fps = 0;
        }

        int width, height;
        glfwGetWindowSize(window, &width, &height);

        float aspect = (float)width / height;

        // uniform buffer
        LvnMat4 proj;
        lvn_perspectiveRHZO(proj, lvn_rad(60.0f), aspect, 0.01f, 1000.0f);
        proj[1][1] *= -1;
        LvnMat4 view;
        lvn_lookAtRH(view, (LvnVec3){0.0f, 2.0f, 2.0f}, (LvnVec3){0.0f, 0.0f, 0.0f}, (LvnVec3){0.0f, 1.0f, 0.0f});
        LvnMat4 model;
        lvn_mat4_identity(model);
        // lvn_translate(model, (LvnVec3){1.0f * sin(currTime), 0.0f, 0.0f});
        lvn_rotate(model, lvn_rad(currTime * 20), (LvnVec3){0.0f, 1.0f, 0.0f});
        // lvn_scale(model, (LvnVec3){1.0f * sin(currTime * 2.0f), 1.0f, 1.0f});

        LvnMat4 camera, temp;
        lvn_mat4_mul(view, model, temp);
        lvn_mat4_mul(proj, temp, camera);

        lvn_mat4_copy(camera, uboData.matrix);
        lvnBufferUpdate(uniformBuffer, &uboData, sizeof(UniformData), 0);

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
            resizeSemaphores(graphicsctx, swapchain, &renderFinishedSemaphores, imageCount);
            imageCount = lvnSwapchainGetImageCount(swapchain);
            continue;
        }
        else if (result != Lvn_Result_Success)
        {
            LVN_LOG_ERROR(logger, "failed to get image");
            continue;
        }

        lvnBeginCommandBuffer(cmdBuff);

        LvnClearColorValue clearValues[] = {
            {{0.0f, 0.1f, 0.2f, 1.0f }},
        };

        LvnClearDepthStencilValue depthValue = { 1.0f, 0 };

        LvnRenderPassBeginInfo beginInfo = {
            .renderPass = renderPass,
            .framebuffer = swapchainFramebuffers[imageIndex],
            .renderArea = {{extent.width, extent.height}, {0, 0}},
            .clearColorValueCount = 1,
            .pClearColorValues = clearValues,
            .clearDepthStencilValue = depthValue,
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

        lvnCmdBindDescriptorSets(cmdBuff, pipeline, 0, 1, &descriptorSet, 0, NULL);

        lvnCmdDraw(cmdBuff, LVN_ARRAY_LEN(s_Vertices), 1, 0, 0);

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
            resizeSemaphores(graphicsctx, swapchain, &renderFinishedSemaphores, imageCount);
            imageCount = lvnSwapchainGetImageCount(swapchain);
            winData.framebufferResized = false;
        }
        glfwPollEvents();
    }

    lvnUnloadImage(&image);

    lvnDestroyDescriptorPool(descriptorPool);
    lvnDestroyDescriptorLayout(descriptorLayout);
    lvnDestroyCommandBuffer(cmdBuff);
    lvnDestroyTexture(texture);
    lvnDestroySampler(sampler);
    lvnDestroyBuffer(vertexBuffer);
    lvnDestroyBuffer(indexBuffer);
    lvnDestroyBuffer(uniformBuffer);
    lvnDestroyFence(fence);
    lvnDestroySemaphore(imageWaitSemaphore);
    for (uint32_t i = 0; i < imageCount; i++)
        lvnDestroySemaphore(renderFinishedSemaphores[i]);
    free(renderFinishedSemaphores);
    lvnDestroyPipeline(pipeline);

    for (uint32_t i = 0; i < imageCount; i++)
        lvnDestroyFramebuffer(swapchainFramebuffers[i]);
    lvnDestroyRenderPass(renderPass);
    lvnDestroySwapchain(swapchain);
    lvnDestroyGraphicsContext(graphicsctx);

    lvnDestroyContext(ctx);

    glfwDestroyWindow(window);
    glfwTerminate();

    printf("s_MemAllocCount: %zu\n", s_MemAllocCount);
}
