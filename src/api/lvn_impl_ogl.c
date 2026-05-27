#include "lvn_impl_ogl.h"

#if defined(LVN_INCLUDE_EGL)
    #include "lvn_impl_egl_loader.h"
#endif

#if defined(LVN_PLATFORM_LINUX)
    static const char* s_LvnOglLibName = "libGL.so.1";
#elif defined(LVN_PLATFORM_WINDOWS)
    static const char* s_LvnOglLibName = "opengl32.dll";
#elif defined(LVN_PLATFORM_MACOS)
    static const char* s_LvnOglLibName = "/System/Library/Frameworks/OpenGL.framework/OpenGL";
#endif

static LvnResult lvn_loadOglLoader(LvnOpenglBackends* oglBackends, const LvnGraphicsContextCreateInfo* createInfo);
static void      lvn_unloadOglLoader(LvnOpenglBackends* oglBackends);
static GLenum    lvn_getOglTextureFilterEnum(LvnTextureFilter filter, LvnMipmapMode mipmapMode);
static GLenum    lvn_getOglTextureModeEnum(LvnTextureMode mode);
static GLenum    lvn_getOglInternalFormatEnum(LvnFormat format);
static GLenum    lvn_getOglDataFormatEnum(LvnFormat format);
static GLenum    lvn_getOglFormatTypeEnum(LvnFormat format);
static GLenum    lvn_getOglDepthStencilAttachmentTypeEnum(LvnFormat format);
static GLenum    lvn_getOglShaderStageEnum(LvnShaderStage stage);
static uint32_t  lvn_getSampleCount(LvnSampleCountFlags samples);

static void GLAPIENTRY lvn_openglDebugCallback(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam)
{
    const LvnGraphicsContext* graphicsctx = (const LvnGraphicsContext*) userParam;

    const char* srcstr = "";
    switch (source)
    {
        case GL_DEBUG_SOURCE_API: { srcstr = "API"; break; }
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM: { srcstr = "WINDOW SYSTEM"; break; }
        case GL_DEBUG_SOURCE_SHADER_COMPILER: { srcstr = "SHADER COMPILER"; break; }
        case GL_DEBUG_SOURCE_THIRD_PARTY: { srcstr = "THIRD PARTY"; break; }
        case GL_DEBUG_SOURCE_APPLICATION: { srcstr = "APPLICATION"; break; }
        case GL_DEBUG_SOURCE_OTHER: { srcstr = "OTHER"; break; }
        default: { srcstr = ""; break; }
    }

    const char* typestr = "";
    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR: { typestr = "ERROR"; break; }
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: { typestr = "DEPRECATED_BEHAVIOR"; break; }
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: { typestr = "UNDEFINED_BEHAVIOR"; break; }
        case GL_DEBUG_TYPE_PORTABILITY: { typestr = "PORTABILITY"; break; }
        case GL_DEBUG_TYPE_PERFORMANCE: { typestr = "PERFORMANCE"; break; }
        case GL_DEBUG_TYPE_MARKER: { typestr = "MARKER"; break; }
        case GL_DEBUG_TYPE_OTHER: { typestr = "OTHER"; break; }
        default: { typestr = ""; break; }
    }

    const char* severitystr = "";
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_NOTIFICATION: { severitystr = "NOTIFICATION"; break; }
        case GL_DEBUG_SEVERITY_LOW: { severitystr = "LOW"; break; }
        case GL_DEBUG_SEVERITY_MEDIUM: { severitystr = "MEDIUM"; break; }
        case GL_DEBUG_SEVERITY_HIGH: { severitystr = "HIGH"; break; }
        default: { severitystr = ""; break; }
    }

    LVN_LOG_ERROR(graphicsctx->coreLogger, "[opengl] [%s] | type: %s | severity: %s | message: %s", srcstr, typestr, severitystr, message);
}

static LvnResult lvn_loadOglLoader(LvnOpenglBackends* oglBackends, const LvnGraphicsContextCreateInfo* createInfo)
{
    LVN_ASSERT(oglBackends && createInfo, "oglBackends and createInfo cannot be null");

    LvnResult result = Lvn_Result_Failure;

#if defined(LVN_INCLUDE_EGL)
    result = lvnEglLoaderInit(oglBackends, createInfo->platformData->ndh, createInfo->platformData->nwh, 1, 1);
#endif

    return result;
}

static void lvn_unloadOglLoader(LvnOpenglBackends* oglBackends)
{
    LVN_ASSERT(oglBackends, "oglBackends cannot be null");

#if defined(LVN_INCLUDE_EGL)
    lvnEglLoaderTerminate(oglBackends);
#endif
}

static GLenum lvn_getOglTextureFilterEnum(LvnTextureFilter filter, LvnMipmapMode mipmapMode)
{
    if (filter == Lvn_TextureFilter_Nearest)
    {
        if (mipmapMode == Lvn_MipmapMode_Disabled)
            return GL_NEAREST;
        else if (mipmapMode == Lvn_MipmapMode_Nearest)
            return GL_NEAREST_MIPMAP_NEAREST;
        else if (mipmapMode == Lvn_MipmapMode_Linear)
            return GL_NEAREST_MIPMAP_LINEAR;
    }
    else if (filter == Lvn_TextureFilter_Linear)
    {
        if (mipmapMode == Lvn_MipmapMode_Disabled)
            return GL_LINEAR;
        else if (mipmapMode == Lvn_MipmapMode_Nearest)
            return GL_LINEAR_MIPMAP_NEAREST;
        else if (mipmapMode == Lvn_MipmapMode_Linear)
            return GL_LINEAR_MIPMAP_LINEAR;
    }

    LVN_ASSERT(false, "invalid texture filter enum");
    return GL_NEAREST;
}

static GLenum lvn_getOglTextureModeEnum(LvnTextureMode mode)
{
    switch (mode)
    {
        case Lvn_TextureMode_Repeat: { return GL_REPEAT; }
        case Lvn_TextureMode_MirrorRepeat: { return GL_MIRRORED_REPEAT; }
        case Lvn_TextureMode_ClampToEdge: { return GL_CLAMP_TO_EDGE; }
        case Lvn_TextureMode_ClampToBorder: { return GL_CLAMP_TO_BORDER; }
    }

    LVN_ASSERT(false, "invalid wrap mode enum");
    return GL_MIRRORED_REPEAT;
}

static GLenum lvn_getOglInternalFormatEnum(LvnFormat format)
{
    switch (format)
    {
        case Lvn_Format_Undefined: { return GL_NONE; }
        case Lvn_Format_R8_UNORM: { return GL_R8; }
        case Lvn_Format_R16_FLOAT: { return GL_R16F; }
        case Lvn_Format_R32_FLOAT: { return GL_R32F; }
        case Lvn_Format_RG8_UNORM: { return GL_RG8; }
        case Lvn_Format_RG16_FLOAT: { return GL_RG16F; }
        case Lvn_Format_RG32_FLOAT: { return GL_RG32F; }
        case Lvn_Format_RGBA8_UNORM: { return GL_RGBA8; }
        case Lvn_Format_RGBA8_SRGB: { return GL_SRGB8_ALPHA8; }
        case Lvn_Format_RGBA16_FLOAT: { return GL_RGBA16F; }
        case Lvn_Format_RGBA32_FLOAT: { return GL_RGBA32F; }
        case Lvn_Format_BGRA8_UNORM: { return GL_RGBA8; }
        case Lvn_Format_BGRA8_SRGB: { return GL_SRGB8_ALPHA8; }
        case Lvn_Format_D24_UNORM_S8_UINT: { return GL_DEPTH24_STENCIL8; }
        case Lvn_Format_D32_FLOAT: { return GL_DEPTH_COMPONENT32F; }
    }

    LVN_ASSERT(false, "invalid format enum");
    return GL_NONE;
}

static GLenum lvn_getOglDataFormatEnum(LvnFormat format)
{
    switch (format)
    {
        case Lvn_Format_Undefined: { return GL_NONE; }
        case Lvn_Format_R8_UNORM: { return GL_RED; }
        case Lvn_Format_R16_FLOAT: { return GL_RED; }
        case Lvn_Format_R32_FLOAT: { return GL_RED; }
        case Lvn_Format_RG8_UNORM: { return GL_RG; }
        case Lvn_Format_RG16_FLOAT: { return GL_RG; }
        case Lvn_Format_RG32_FLOAT: { return GL_RG; }
        case Lvn_Format_RGBA8_UNORM: { return GL_RGBA; }
        case Lvn_Format_RGBA8_SRGB: { return GL_RGBA; }
        case Lvn_Format_RGBA16_FLOAT: { return GL_RGBA; }
        case Lvn_Format_RGBA32_FLOAT: { return GL_RGBA; }
        case Lvn_Format_BGRA8_UNORM: { return GL_BGRA; }
        case Lvn_Format_BGRA8_SRGB: { return GL_BGRA; }
        case Lvn_Format_D24_UNORM_S8_UINT: { return GL_DEPTH_STENCIL; }
        case Lvn_Format_D32_FLOAT: { return GL_DEPTH_COMPONENT; }
    }

    LVN_ASSERT(false, "invalid format enum");
    return GL_NONE;
}

static GLenum lvn_getOglFormatTypeEnum(LvnFormat format)
{
    switch (format)
    {
        case Lvn_Format_Undefined: { return GL_NONE; }
        case Lvn_Format_R8_UNORM: { return GL_UNSIGNED_BYTE; }
        case Lvn_Format_R16_FLOAT: { return GL_HALF_FLOAT; }
        case Lvn_Format_R32_FLOAT: { return GL_FLOAT; }
        case Lvn_Format_RG8_UNORM: { return GL_UNSIGNED_BYTE; }
        case Lvn_Format_RG16_FLOAT: { return GL_HALF_FLOAT; }
        case Lvn_Format_RG32_FLOAT: { return GL_FLOAT; }
        case Lvn_Format_RGBA8_UNORM: { return GL_UNSIGNED_BYTE; }
        case Lvn_Format_RGBA8_SRGB: { return GL_UNSIGNED_BYTE; }
        case Lvn_Format_RGBA16_FLOAT: { return GL_HALF_FLOAT; }
        case Lvn_Format_RGBA32_FLOAT: { return GL_FLOAT; }
        case Lvn_Format_BGRA8_UNORM: { return GL_UNSIGNED_BYTE; }
        case Lvn_Format_BGRA8_SRGB: { return GL_UNSIGNED_BYTE; }
        case Lvn_Format_D24_UNORM_S8_UINT: { return GL_UNSIGNED_INT_24_8; }
        case Lvn_Format_D32_FLOAT: { return GL_FLOAT; }
    }

    LVN_ASSERT(false, "invalid format enum");
    return GL_NONE;
}

static GLenum lvn_getOglDepthStencilAttachmentTypeEnum(LvnFormat format)
{
    switch (format)
    {
        case Lvn_Format_D24_UNORM_S8_UINT: { return GL_DEPTH_STENCIL_ATTACHMENT; }
        case Lvn_Format_D32_FLOAT: { return GL_DEPTH_ATTACHMENT; }
        default: { break; }
    }

    LVN_ASSERT(false, "invalid depth stencil format enum");
    return GL_NONE;
}

static GLenum lvn_getOglShaderStageEnum(LvnShaderStage stage)
{
    switch (stage)
    {
        case Lvn_ShaderStage_Vertex: { return GL_VERTEX_SHADER; }
        case Lvn_ShaderStage_Fragment: { return GL_FRAGMENT_SHADER; }
    }

    LVN_ASSERT(false, "invalid shader stage enum");
    return GL_NONE;
}

static uint32_t lvn_getSampleCount(LvnSampleCountFlags samples)
{
    switch (samples)
    {
        case Lvn_SampleCountFlag_1_Bit: { return 1; }
        case Lvn_SampleCountFlag_2_Bit: { return 2; }
        case Lvn_SampleCountFlag_4_Bit: { return 4; }
        case Lvn_SampleCountFlag_8_Bit: { return 8; }
        case Lvn_SampleCountFlag_16_Bit: { return 16; }
        case Lvn_SampleCountFlag_32_Bit: { return 32; }
        case Lvn_SampleCountFlag_64_Bit: { return 64; }
    }

    LVN_ASSERT(false, "invalid sample count enum");
    return 1;
}

LvnResult lvnImplOglInit(LvnGraphicsContext* graphicsctx, const LvnGraphicsContextCreateInfo* createInfo)
{
    LVN_ASSERT(graphicsctx && createInfo, "graphicsctx and createInfo cannot be null");

    LvnOpenglBackends* oglBackends = (LvnOpenglBackends*) lvn_calloc(sizeof(LvnOpenglBackends));
    graphicsctx->implData = oglBackends;

    oglBackends->graphicsctx = graphicsctx;

    // load opengl library
    oglBackends->handle = lvn_platformLoadModule(s_LvnOglLibName);

    if (!oglBackends->handle)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[opengl] unable to load opengl shared library: %s",
                      s_LvnOglLibName);
        goto fail_cleanup;
    }

    // load opengl loader
    if (lvn_loadOglLoader(oglBackends, createInfo) != Lvn_Result_Success)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[opengl] unable to load opengl loader");
        goto fail_cleanup;
    }

    // opengl loader function symbols
    if (!oglBackends->ogllCreateSurface ||
        !oglBackends->ogllDestroySurface ||
        !oglBackends->ogllMakeCurrent ||
        !oglBackends->ogllSwapBuffers)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[opengl] failed to load opengl loader function symbols");
        goto fail_cleanup;
    }

    // NOTE: opengl function symbols should be loaded in the opengl loader
    if (!oglBackends->glGetString ||
        !oglBackends->glGetError ||
        !oglBackends->glDebugMessageCallback ||
        !oglBackends->glGetIntegerv ||
        !oglBackends->glDisable ||
        !oglBackends->glEnable ||
        !oglBackends->glCreateBuffers ||
        !oglBackends->glDeleteBuffers ||
        !oglBackends->glCreateSamplers ||
        !oglBackends->glDeleteSamplers ||
        !oglBackends->glCreateTextures ||
        !oglBackends->glDeleteTextures ||
        !oglBackends->glCreateFramebuffers ||
        !oglBackends->glDeleteFramebuffers ||
        !oglBackends->glCreateVertexArrays ||
        !oglBackends->glDeleteVertexArrays ||
        !oglBackends->glCreateShader ||
        !oglBackends->glDeleteShader ||
        !oglBackends->glCreateProgram ||
        !oglBackends->glDeleteProgram ||
        !oglBackends->glCheckNamedFramebufferStatus ||
        !oglBackends->glNamedFramebufferTexture ||
        !oglBackends->glNamedFramebufferDrawBuffer ||
        !oglBackends->glNamedFramebufferDrawBuffers ||
        !oglBackends->glSamplerParameteri ||
        !oglBackends->glTextureParameteri ||
        !oglBackends->glTextureStorage2D ||
        !oglBackends->glTextureStorage2DMultisample ||
        !oglBackends->glTextureSubImage2D ||
        !oglBackends->glShaderSource ||
        !oglBackends->glCompileShader ||
        !oglBackends->glGetShaderiv ||
        !oglBackends->glAttachShader ||
        !oglBackends->glLinkProgram ||
        !oglBackends->glGetShaderInfoLog)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[opengl] failed to load opengl function symbols");
        goto fail_cleanup;
    }

    // set debug message callback
    GLint flags;
    oglBackends->glGetIntegerv(GL_CONTEXT_FLAGS, &flags);

    if ((createInfo->enableGraphicsApiDebugLogging) && (flags & GL_CONTEXT_FLAG_DEBUG_BIT))
    {
        oglBackends->glEnable(GL_DEBUG_OUTPUT);
        oglBackends->glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

        oglBackends->glDebugMessageCallback(lvn_openglDebugCallback, graphicsctx);
    }

    // get opengl version
    oglBackends->glGetIntegerv(GL_MAJOR_VERSION, &oglBackends->versionMajor);
    oglBackends->glGetIntegerv(GL_MINOR_VERSION, &oglBackends->versionMinor);

    // get opengl capabilities info
    oglBackends->glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &oglBackends->maxColorAttachments);
    oglBackends->glGetIntegerv(GL_MAX_DRAW_BUFFERS, &oglBackends->maxDrawBuffers);

    // set opengl implementation function pointers
    graphicsctx->implCreateSurface = lvnImplOglCreateSurface;
    graphicsctx->implDestroySurface = lvnImplOglDestroySurface;
    graphicsctx->implCreateSwapchain = lvnImplOglCreateSwapchain;
    graphicsctx->implDestroySwapchain = lvnImplOglDestroySwapchain;
    graphicsctx->implCreateRenderPass = lvnImplOglCreateRenderPass;
    graphicsctx->implDestroyRenderPass = lvnImplOglDestroyRenderPass;
    graphicsctx->implCreateFramebuffer = lvnImplOglCreateFramebuffer;
    graphicsctx->implDestroyFramebuffer = lvnImplOglDestroyFramebuffer;
    graphicsctx->implCreateShader = lvnImplOglCreateShader;
    graphicsctx->implDestroyShader = lvnImplOglDestroyShader;
    graphicsctx->implCreatePipeline = lvnImplOglCreatePipeline;
    graphicsctx->implDestroyPipeline = lvnImplOglDestroyPipeline;
    graphicsctx->implCreateFence = lvnImplOglCreateFence;
    graphicsctx->implDestroyFence = lvnImplOglDestroyFence;
    graphicsctx->implCreateSemaphore = lvnImplOglCreateSemaphore;
    graphicsctx->implDestroySemaphore = lvnImplOglDestroySemaphore;
    graphicsctx->implCreateBuffer = lvnImplOglsCreateBuffer;
    graphicsctx->implDestroyBuffer = lvnImplOglsDestroyBuffer;
    graphicsctx->implCreateSampler = lvnImplOglsCreateSampler;
    graphicsctx->implDestroySampler = lvnImplOglsDestroySampler;
    graphicsctx->implCreateTexture = lvnImplOglsCreateTexture;
    graphicsctx->implDestroyTexture = lvnImplOglsDestroyTexture;
    graphicsctx->implAllocateCommandBuffers = lvnImplOglAllocateCommandBuffers;
    graphicsctx->implSurfaceGetSupportedFormats = lvnImplOglSurfaceGetSupportedFormats;
    graphicsctx->implSurfaceGetSupportedPresentModes = lvnImplOglSurfaceGetSupportedPresentModes;
    graphicsctx->implSwapchainResize = lvnImplOglSwapchainResize;
    graphicsctx->implSwapchainAcquireNextImage = lvnImplOglSwapchainAcquireNextImage;
    graphicsctx->implFenceWait = lvnImplOglFenceWait;
    graphicsctx->implFenceReset = lvnImplOglFenceReset;
    graphicsctx->implBufferUpdateData = lvnImplOglBufferUpdateData;
    graphicsctx->implBufferResize = lvnImplOglBufferResize;
    graphicsctx->implBeginCommandBuffer = lvnImplOglBeginCommandBuffer;
    graphicsctx->implEndCommandBuffer = lvnImplOglEndCommandBuffer;
    graphicsctx->implCmdBeginRenderPass = lvnImplOglCmdBeginRenderPass;
    graphicsctx->implCmdEndRenderPass = lvnImplOglCmdEndRenderPass;
    graphicsctx->implCmdBindPipeline = lvnImplOglCmdBindPipeline;
    graphicsctx->implCmdBindVertexBuffer = lvnImplOglCmdBindVertexBuffer;
    graphicsctx->implCmdBindIndexBuffer = lvnImplOglCmdBindIndexBuffer;
    graphicsctx->implCmdSetViewport = lvnImplOglCmdSetViewport;
    graphicsctx->implCmdSetScissor = lvnImplOglCmdSetScissor;
    graphicsctx->implCmdDraw = lvnImplOglCmdDraw;
    graphicsctx->implCmdDrawIndexed = lvnImplOglCmdDrawIndexed;
    graphicsctx->implRenderSubmit = lvnImplOglRenderSubmit;
    graphicsctx->implRenderPresent = lvnImplOglRenderPresent;

    return Lvn_Result_Success;

fail_cleanup:
    lvnImplOglTerminate(graphicsctx);
    return Lvn_Result_Failure;
}

void lvnImplOglTerminate(LvnGraphicsContext* graphicsctx)
{
    LVN_ASSERT(graphicsctx, "graphicsctx cannot be null");

    LvnOpenglBackends* oglBackends = (LvnOpenglBackends*) graphicsctx->implData;
    if (!oglBackends)
        return;

    lvn_unloadOglLoader(oglBackends);

    if (oglBackends->handle)
        lvn_platformFreeModule(oglBackends->handle);

    lvn_free(oglBackends);
    graphicsctx->implData = NULL;
}

LvnResult lvnImplOglCreateSurface(const LvnGraphicsContext* graphicsctx, LvnSurface* surface, const LvnSurfaceCreateInfo* createInfo)
{
    LVN_ASSERT(graphicsctx && surface && createInfo, "graphicsctx, surface, and createInfo cannot be null");

    const LvnOpenglBackends* oglBackends = (const LvnOpenglBackends*) graphicsctx->implData;

    if (oglBackends->ogllCreateSurface(oglBackends, surface, createInfo) != Lvn_Result_Success)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[opengl] unable to create api level surface at %p", surface);
        return Lvn_Result_Failure;
    }

    return Lvn_Result_Success;
}

void lvnImplOglDestroySurface(LvnSurface* surface)
{
    LVN_ASSERT(surface, "surface cannot be null");

    const LvnOpenglBackends* oglBackends = (const LvnOpenglBackends*) surface->graphicsctx->implData;

    oglBackends->ogllDestroySurface(oglBackends, surface);
}

LvnResult lvnImplOglCreateSwapchain(const LvnGraphicsContext* graphicsctx, LvnSwapchain* swapchain, const LvnSwapchainCreateInfo* createInfo)
{
    LVN_ASSERT(graphicsctx && swapchain && createInfo, "graphicsctx, surface, and createInfo cannot be null");

    const LvnOpenglBackends* oglBackends = (const LvnOpenglBackends*) graphicsctx->implData;

    LvnResult errResult = Lvn_Result_Failure;
    LvnOglSwapchainData* swapchainData = NULL;

    swapchainData = (LvnOglSwapchainData*) lvn_calloc(sizeof(LvnOglSwapchainData));
    if (!swapchainData)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[opengl] failed to allocate memory for swapchain data in swapchain %p",
                      swapchain);
        errResult = Lvn_Result_OutOfMemory;
        goto fail_cleanup;
    }

    swapchainData->images = (GLuint*) lvn_calloc(createInfo->minImageCount * sizeof(GLuint));
    if (!swapchainData->images)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[opengl] failed to allocate memory for swapchain images <GLuint> in swapchain %p",
                      swapchain);
        errResult = Lvn_Result_OutOfMemory;
        goto fail_cleanup;
    }

    oglBackends->glCreateTextures(GL_TEXTURE_2D, createInfo->minImageCount, swapchainData->images);

    swapchainData->imageCount = createInfo->minImageCount;
    swapchainData->width = createInfo->width;
    swapchainData->height = createInfo->height;
    swapchainData->format = createInfo->surfaceFormat;
    swapchainData->presentMode = createInfo->presentMode;

    swapchain->pSwapchainImages = (LvnTexture*) lvn_calloc(createInfo->minImageCount * sizeof(LvnTexture));
    if (!swapchain->pSwapchainImages)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[opengl] failed to allocate memory for swapchain images <LvnTexture> in swapchain %p",
                      swapchain);
        errResult = Lvn_Result_OutOfMemory;
        goto fail_cleanup;
    }

    for (uint32_t i = 0; i < createInfo->minImageCount; i++)
    {
        oglBackends->glTextureStorage2D(swapchainData->images[i],
                                        1,
                                        lvn_getOglInternalFormatEnum(createInfo->surfaceFormat),
                                        createInfo->width,
                                        createInfo->height);

        swapchain->pSwapchainImages[i].texId = swapchainData->images[i];
        swapchain->pSwapchainImages[i].width = createInfo->width;
        swapchain->pSwapchainImages[i].width = createInfo->height;
    }

    swapchain->swapchainData = swapchainData;
    swapchain->swapchainImageCount = createInfo->minImageCount;
    swapchain->extent = (LvnExtent2D){ .width = createInfo->width, .height = createInfo->height };

    return Lvn_Result_Success;

fail_cleanup:
    if (swapchainData)
    {
        if (swapchainData->images)
            lvn_free(swapchainData->images);

        lvn_free(swapchainData);
    }
    if (swapchain->pSwapchainImages)
        lvn_free(swapchain->pSwapchainImages);
    return errResult;
}

void lvnImplOglDestroySwapchain(LvnSwapchain* swapchain)
{
    LVN_ASSERT(swapchain, "swapchain cannot be null");

    const LvnOpenglBackends* oglBackends = (const LvnOpenglBackends*) swapchain->graphicsctx->implData;

    LvnOglSwapchainData* swapchainData = (LvnOglSwapchainData*) swapchain->swapchainData;

    oglBackends->glDeleteTextures(swapchainData->imageCount, swapchainData->images);

    lvn_free(swapchainData->images);
    lvn_free(swapchainData);
    lvn_free(swapchain->pSwapchainImages);

    swapchain->swapchainData = NULL;
    swapchain->pSwapchainImages = NULL;
    swapchain->swapchainImageCount = 0;
}

LvnResult lvnImplOglCreateRenderPass(const LvnGraphicsContext* graphicsctx, LvnRenderPass* renderpass, const LvnRenderPassCreateInfo* createInfo)
{
    LVN_ASSERT(graphicsctx && renderpass && createInfo, "graphicsctx, renderpass, and createInfo cannot be null");

    LvnResult errResult = Lvn_Result_Failure;
    LvnOglRenderpassData* renderpassData = NULL;

    renderpassData = (LvnOglRenderpassData*) lvn_calloc(sizeof(LvnOglRenderpassData));
    if (!renderpassData)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[opengl] failed to allocate memory for renderpass data in renderpass %p",
                      renderpass);
        errResult = Lvn_Result_OutOfMemory;
        goto fail_cleanup;
    }

    // color attachments
    renderpassData->colorAttachmentCount = createInfo->colorAttachmentCount;

    renderpassData->colorAttachments = lvn_calloc(createInfo->colorAttachmentCount * sizeof(LvnColorAttachment));
    if (!renderpassData->colorAttachments)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[opengl] failed to allocate memory for color attachments info in renderpass %p",
                      renderpass);
        errResult = Lvn_Result_OutOfMemory;
        goto fail_cleanup;
    }

    for (uint32_t i = 0; i < createInfo->colorAttachmentCount; i++)
        renderpassData->colorAttachments[i] = createInfo->pColorAttachments[i];

    // resolve attachments
    renderpassData->resolveAttachments = lvn_calloc(createInfo->colorAttachmentCount * sizeof(LvnResolveAttachment));
    if (!renderpassData->resolveAttachments)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[opengl] failed to allocate memory for resolve attachments info in renderpass %p",
                      renderpass);
        errResult = Lvn_Result_OutOfMemory;
        goto fail_cleanup;
    }

    renderpassData->hasResolves = lvn_calloc(createInfo->colorAttachmentCount * sizeof(uint32_t));
    if (!renderpassData->hasResolves)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[opengl] failed to allocate memory for hasResolves array in renderpass %p",
                      renderpass);
        errResult = Lvn_Result_OutOfMemory;
        goto fail_cleanup;
    }

    for (uint32_t i = 0; i < createInfo->colorAttachmentCount; i++)
    {
        if (createInfo->pColorAttachments[i].resolveAttachment)
        {
            renderpassData->resolveAttachments[i] = *createInfo->pColorAttachments[i].resolveAttachment;
            renderpassData->hasResolves[i] = 1;
        }
    }

    // depth stencil attachment
    if (createInfo->depthStencilAttachment)
    {
        renderpassData->depthStencilAttachment = *createInfo->depthStencilAttachment;
        renderpassData->hasDepth = true;
    }

    renderpass->renderpass = renderpassData;

    return Lvn_Result_Success;

fail_cleanup:
    if (renderpassData)
    {
        if (renderpassData->colorAttachments)
            lvn_free(renderpassData->colorAttachments);
        if (renderpassData->resolveAttachments)
            lvn_free(renderpassData->resolveAttachments);
        if (renderpassData->hasResolves)
            lvn_free(renderpassData->hasResolves);

        lvn_free(renderpassData);
    }
    return errResult;
}

void lvnImplOglDestroyRenderPass(LvnRenderPass* renderpass)
{
    LVN_ASSERT(renderpass, "renderpass cannot be null");

    LvnOglRenderpassData* renderpassData = (LvnOglRenderpassData*) renderpass->renderpass;

    if (renderpassData->colorAttachments)
        lvn_free(renderpassData->colorAttachments);
    if (renderpassData->resolveAttachments)
        lvn_free(renderpassData->resolveAttachments);
    if (renderpassData->hasResolves)
        lvn_free(renderpassData->hasResolves);

    lvn_free(renderpassData);
    renderpass->renderpass = NULL;
}

LvnResult lvnImplOglCreateFramebuffer(const LvnGraphicsContext* graphicsctx, LvnFramebuffer* framebuffer, const LvnFramebufferCreateInfo* createInfo)
{
    LVN_ASSERT(graphicsctx && framebuffer && createInfo, "graphicsctx, framebuffer, and createInfo cannot be null");

    const LvnOpenglBackends* oglBackends = (const LvnOpenglBackends*) graphicsctx->implData;
    const LvnOglRenderpassData* renderpassData = (const LvnOglRenderpassData*) createInfo->renderPass->renderpass;

    LvnResult errResult = Lvn_Result_Failure;
    LvnOglFramebufferData* framebufferData = NULL;

    framebufferData = (LvnOglFramebufferData*) lvn_calloc(sizeof(LvnOglFramebufferData));
    if (!framebufferData)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[opengl] failed to allocate memory for framebuffer data in framebuffer %p",
                      framebuffer);
        errResult = Lvn_Result_OutOfMemory;
        goto fail_cleanup;
    }

    oglBackends->glCreateFramebuffers(1, &framebufferData->fboId);

    for (uint32_t i = 0; i < createInfo->colorAttachmentCount; i++)
        oglBackends->glNamedFramebufferTexture(framebufferData->fboId, GL_COLOR_ATTACHMENT0 + i, createInfo->pColorAttachments[i]->texId, 0);

    if (createInfo->depthStencilAttachment)
    {
        GLenum attachment = lvn_getOglDepthStencilAttachmentTypeEnum(renderpassData->depthStencilAttachment.format);
        oglBackends->glNamedFramebufferTexture(framebufferData->fboId, attachment, createInfo->depthStencilAttachment->texId, 0);
    }

    // get min between maxColorAttachments and maxDrawBuffers
    uint32_t maxColorAttachments = (oglBackends->maxColorAttachments > oglBackends->maxDrawBuffers)
        ? oglBackends->maxDrawBuffers
        : oglBackends->maxColorAttachments;

    if (createInfo->colorAttachmentCount > maxColorAttachments)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[opengl] failed to create framebuffer %p | createInfo->colorAttachmentCount cannot be greater than the max supported color attachment count (%zu)",
                      framebuffer,
                      maxColorAttachments);
        goto fail_cleanup;
    }

    // set opengl attachment draw buffers
    if (createInfo->colorAttachmentCount > 1)
    {
        GLenum colorAttachmentBufs[32] = { GL_COLOR_ATTACHMENT0 };
        for (uint32_t i = 1; i < createInfo->colorAttachmentCount; i++)
            colorAttachmentBufs[i] = colorAttachmentBufs[0] + i;

        oglBackends->glNamedFramebufferDrawBuffers(framebufferData->fboId, createInfo->colorAttachmentCount, colorAttachmentBufs);
    }
    else if (createInfo->colorAttachmentCount == 0)
    {
        oglBackends->glNamedFramebufferDrawBuffer(framebufferData->fboId, GL_NONE);
    }

    // multisampling
    if (createInfo->pResolveAttachments)
    {
        framebufferData->multisample = true;
        oglBackends->glCreateFramebuffers(1, &framebufferData->resolveId);

        for (uint32_t i = 0; i < createInfo->colorAttachmentCount; i++)
            oglBackends->glNamedFramebufferTexture(framebufferData->resolveId, GL_COLOR_ATTACHMENT0 + i, createInfo->pResolveAttachments[i]->texId, 0);
    }

    framebuffer->framebufferHandle = framebufferData;

    return Lvn_Result_Success;

fail_cleanup:
    oglBackends->glDeleteFramebuffers(1, &framebufferData->fboId);
    oglBackends->glDeleteFramebuffers(1, &framebufferData->resolveId);
    lvn_free(framebufferData);
    return errResult;
}

void lvnImplOglDestroyFramebuffer(LvnFramebuffer* framebuffer)
{
    LVN_ASSERT(framebuffer, "framebuffer cannot be null");

    const LvnOpenglBackends* oglBackends = (const LvnOpenglBackends*) framebuffer->graphicsctx->implData;
    LvnOglFramebufferData* framebufferData = (LvnOglFramebufferData*) framebuffer->framebufferHandle;

    oglBackends->glDeleteFramebuffers(1, &framebufferData->fboId);
    oglBackends->glDeleteFramebuffers(1, &framebufferData->resolveId);

    lvn_free(framebufferData);
    framebuffer->framebufferHandle = NULL;
}

LvnResult lvnImplOglCreateShader(const LvnGraphicsContext* graphicsctx, LvnShader* shader, const LvnShaderCreateInfo* createInfo)
{
    LVN_ASSERT(graphicsctx && shader && createInfo, "graphicsctx, shader, and createInfo cannot be null");

    const LvnOpenglBackends* oglBackends = (const LvnOpenglBackends*) graphicsctx->implData;

    LvnResult errResult = Lvn_Result_Failure;
    LvnOglShaderData* shaderData = NULL;

    shaderData = (LvnOglShaderData*) lvn_calloc(sizeof(LvnOglShaderData));
    if (!shaderData)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[opengl] failed to allocate memory for shader data in shader %p",
                      shader);
        errResult = Lvn_Result_OutOfMemory;
        goto fail_cleanup;
    }

    shaderData->shaderId = oglBackends->glCreateShader(lvn_getOglShaderStageEnum(createInfo->stage));

    oglBackends->glShaderSource(shaderData->shaderId, 1, (const GLchar* const*)(&createInfo->pCode), NULL);
    oglBackends->glCompileShader(shaderData->shaderId);

    GLint success;
    oglBackends->glGetShaderiv(shaderData->shaderId, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        char infoLog[1024];
        oglBackends->glGetShaderInfoLog(shaderData->shaderId, 1024, NULL, infoLog);

        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[opengl] shader compile error in shader %p | info log: %s",
                      shader,
                      infoLog);

        goto fail_cleanup;
    }

    shaderData->stage = createInfo->stage;

    shader->shader = shaderData;

    return Lvn_Result_Success;

fail_cleanup:
    if (shaderData)
    {
        oglBackends->glDeleteShader(shaderData->shaderId);
        lvn_free(shaderData);
    }
    return errResult;
}

void lvnImplOglDestroyShader(LvnShader* shader)
{
    LVN_ASSERT(shader, "shader cannot be null");

    const LvnOpenglBackends* oglBackends = (const LvnOpenglBackends*) shader->graphicsctx->implData;

    LvnOglShaderData* shaderData = (LvnOglShaderData*) shader->shader;

    oglBackends->glDeleteShader(shaderData->shaderId);
    lvn_free(shaderData);

    shader->shader = NULL;
}

LvnResult lvnImplOglCreatePipeline(const LvnGraphicsContext* graphicsctx, LvnPipeline* pipeline, const LvnPipelineCreateInfo* createInfo)
{
    return Lvn_Result_Success;
}

void lvnImplOglDestroyPipeline(LvnPipeline* pipeline)
{

}

LvnResult lvnImplOglCreateFence(const LvnGraphicsContext* graphicsctx, LvnFence* fence)
{
    return Lvn_Result_Success;
}

void lvnImplOglDestroyFence(LvnFence* fence)
{

}

LvnResult lvnImplOglCreateSemaphore(const LvnGraphicsContext* graphicsctx, LvnSemaphore* semaphore)
{
    return Lvn_Result_Success;
}

void lvnImplOglDestroySemaphore(LvnSemaphore* semaphore)
{

}

LvnResult lvnImplOglsCreateBuffer(const LvnGraphicsContext* graphicsctx, LvnBuffer* buffer, const LvnBufferCreateInfo* createInfo)
{
    return Lvn_Result_Success;
}

void lvnImplOglsDestroyBuffer(LvnBuffer* buffer)
{

}

LvnResult lvnImplOglsCreateSampler(const LvnGraphicsContext* graphicsctx, LvnSampler* sampler, const LvnSamplerCreateInfo* createInfo)
{
    LVN_ASSERT(graphicsctx && sampler && createInfo, "graphicsctx, sampler, and createInfo cannot be null");

    const LvnOpenglBackends* oglBackends = (const LvnOpenglBackends*) graphicsctx->implData;

    oglBackends->glCreateSamplers(1, &sampler->samplerId);

    oglBackends->glSamplerParameteri(sampler->samplerId, GL_TEXTURE_WRAP_S, lvn_getOglTextureModeEnum(createInfo->wrapS));
    oglBackends->glSamplerParameteri(sampler->samplerId, GL_TEXTURE_WRAP_T, lvn_getOglTextureModeEnum(createInfo->wrapT));
    oglBackends->glSamplerParameteri(sampler->samplerId, GL_TEXTURE_WRAP_R, lvn_getOglTextureModeEnum(createInfo->wrapR));
    oglBackends->glSamplerParameteri(sampler->samplerId, GL_TEXTURE_MIN_FILTER, lvn_getOglTextureFilterEnum(createInfo->minFilter, createInfo->mipmapMode));
    oglBackends->glSamplerParameteri(sampler->samplerId, GL_TEXTURE_MAG_FILTER, lvn_getOglTextureFilterEnum(createInfo->minFilter, Lvn_MipmapMode_Disabled));

    return Lvn_Result_Success;
}

void lvnImplOglsDestroySampler(LvnSampler* sampler)
{
    LVN_ASSERT(sampler, "sampler cannot be null");
    const LvnOpenglBackends* oglBackends = (const LvnOpenglBackends*) sampler->graphicsctx->implData;
    oglBackends->glDeleteSamplers(1, &sampler->samplerId);
    sampler->samplerId = 0;
}

LvnResult lvnImplOglsCreateTexture(const LvnGraphicsContext* graphicsctx, LvnTexture* texture, const LvnTextureCreateInfo* createInfo)
{
    LVN_ASSERT(graphicsctx && texture && createInfo, "graphicsctx, texture, and createInfo cannot be null");

    const LvnOpenglBackends* oglBackends = (const LvnOpenglBackends*) graphicsctx->implData;
    const LvnImage* image = createInfo->image;

    oglBackends->glCreateTextures(GL_TEXTURE_2D, 1, &texture->texId);

    GLenum internalFormat = lvn_getOglInternalFormatEnum(createInfo->format);
    GLenum dataFormat = lvn_getOglDataFormatEnum(createInfo->format);
    GLenum formatType = lvn_getOglFormatTypeEnum(createInfo->format);

    if (createInfo->samples == Lvn_SampleCountFlag_1_Bit)
        oglBackends->glTextureStorage2D(texture->texId, 1, internalFormat, createInfo->width, createInfo->height);
    else
    {
        oglBackends->glTextureStorage2DMultisample(texture->texId,
                                                   lvn_getSampleCount(createInfo->samples),
                                                   internalFormat,
                                                   createInfo->width,
                                                   createInfo->height,
                                                   GL_TRUE);
    }

    oglBackends->glTextureSubImage2D(texture->texId, 0, 0, 0, createInfo->width, createInfo->height, dataFormat, formatType, image->data);

    return Lvn_Result_Success;
}

void lvnImplOglsDestroyTexture(LvnTexture* texture)
{
    LVN_ASSERT(texture, "texture cannot be null");
    const LvnOpenglBackends* oglBackends = (const LvnOpenglBackends*) texture->graphicsctx->implData;
    oglBackends->glDeleteTextures(1, &texture->texId);
    texture->texId = 0;
}

LvnResult lvnImplOglAllocateCommandBuffers(const LvnGraphicsContext* graphicsctx, const LvnCommandBufferAllocInfo* allocInfo, LvnCommandBuffer** pCommandBuffers)
{
    return Lvn_Result_Success;
}

void lvnImplOglSurfaceGetSupportedFormats(const LvnSurface* surface, uint32_t* formatCount, LvnFormat* pSurfaceFormats)
{
    LVN_ASSERT(surface && formatCount, "surface and formatCount cannot be null");

    *formatCount = 4;

    if (!pSurfaceFormats)
        return;

    pSurfaceFormats[0] = Lvn_Format_RGBA8_UNORM;
    pSurfaceFormats[1] = Lvn_Format_RGBA8_SRGB;
    pSurfaceFormats[2] = Lvn_Format_BGRA8_UNORM;
    pSurfaceFormats[3] = Lvn_Format_BGRA8_SRGB;
}

void lvnImplOglSurfaceGetSupportedPresentModes(const LvnSurface* surface, uint32_t* presentModeCount, LvnPresentMode* pPresentModes)
{
    LVN_ASSERT(surface && presentModeCount, "surface and presentModeCount cannot be null");

    *presentModeCount = 3;

    if (!pPresentModes)
        return;

    pPresentModes[0] = Lvn_PresentMode_FIFO;
    pPresentModes[1] = Lvn_PresentMode_Immediate;
    pPresentModes[2] = Lvn_PresentMode_Mailbox;
}

LvnResult lvnImplOglSwapchainResize(LvnSwapchain* swapchain, uint32_t width, uint32_t height)
{
    return Lvn_Result_Success;
}

LvnResult lvnImplOglSwapchainAcquireNextImage(LvnSwapchain* swapchain, LvnSemaphore* semaphore, LvnFence* fence, uint32_t* imageIndex)
{
    return Lvn_Result_Success;
}

LvnResult lvnImplOglFenceWait(LvnFence* fence, uint64_t timeout)
{
    return Lvn_Result_Success;
}

LvnResult lvnImplOglFenceReset(LvnFence* fence)
{
    return Lvn_Result_Success;
}

void lvnImplOglBufferUpdateData(LvnBuffer* buffer, void* data, uint64_t size, uint64_t offset)
{

}

void lvnImplOglBufferResize(LvnBuffer* buffer, uint64_t size)
{

}

void lvnImplOglBeginCommandBuffer(LvnCommandBuffer* commandBuffer)
{

}

void lvnImplOglEndCommandBuffer(LvnCommandBuffer* commandBuffer)
{

}

void lvnImplOglCmdBeginRenderPass(LvnCommandBuffer* commandBuffer, LvnRenderPassBeginInfo* beginInfo)
{

}

void lvnImplOglCmdEndRenderPass(LvnCommandBuffer* commandBuffer)
{

}

void lvnImplOglCmdBindPipeline(LvnCommandBuffer* commandBuffer, LvnPipeline* pipeline)
{

}

void lvnImplOglCmdBindVertexBuffer(LvnCommandBuffer* commandBuffer, uint32_t firstBinding, uint32_t bindingCount, LvnBuffer** pBuffers, uint64_t* pOffsets)
{

}

void lvnImplOglCmdBindIndexBuffer(LvnCommandBuffer* commandBuffer, LvnBuffer* buffer, uint64_t offset)
{

}

void lvnImplOglCmdSetViewport(LvnCommandBuffer* commandBuffer, const LvnViewport* viewport)
{

}

void lvnImplOglCmdSetScissor(LvnCommandBuffer* commandBuffer, const LvnRenderArea* scissor)
{

}

void lvnImplOglCmdDraw(LvnCommandBuffer* commandBuffer, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
{

}

void lvnImplOglCmdDrawIndexed(LvnCommandBuffer* commandBuffer, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance)
{

}

LvnResult lvnImplOglRenderSubmit(const LvnGraphicsContext* graphicsctx, const LvnSubmitInfo* pSubmits, uint32_t submitCount, LvnFence* fence)
{
    return Lvn_Result_Success;
}

LvnResult lvnImplOglRenderPresent(const LvnGraphicsContext* graphicsctx, const LvnPresentInfo* presentInfo)
{
    return Lvn_Result_Success;
}
