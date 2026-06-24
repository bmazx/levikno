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
static GLenum    lvn_getOglTopologyEnum(LvnTopologyType topology);
static GLenum    lvn_getOglCullModeFlagEnum(LvnCullFaceMode cullFaceMode);
static GLenum    lvn_getOglCullFrontFaceEnum(LvnCullFrontFace frontFace);
static GLenum    lvn_getOglBlendFactorEnum(LvnColorBlendFactor blendFactor);
static GLenum    lvn_getOglCompareOpEnum(LvnCompareOperation compareOp);
static GLenum    lvn_getOglStencilOpEnum(LvnStencilOperation stencilOp);
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

static GLenum lvn_getOglTopologyEnum(LvnTopologyType topology)
{
    switch (topology)
    {
        case Lvn_TopologyType_Point: { return GL_POINTS; }
        case Lvn_TopologyType_Line: { return GL_LINES; }
        case Lvn_TopologyType_LineStrip: { return GL_LINE_STRIP; }
        case Lvn_TopologyType_Triangle: { return GL_TRIANGLES; }
        case Lvn_TopologyType_TriangleStrip: { return GL_TRIANGLE_STRIP; }
    }

    LVN_ASSERT(false, "invalid topology enum");
    return GL_POINTS;
}

static GLenum lvn_getOglCullModeFlagEnum(LvnCullFaceMode cullFaceMode)
{
    switch (cullFaceMode)
    {
        case Lvn_CullFaceMode_Disable: { return GL_NONE; }
        case Lvn_CullFaceMode_Front: { return GL_FRONT; }
        case Lvn_CullFaceMode_Back: { return GL_BACK; }
        case Lvn_CullFaceMode_Both: { return GL_FRONT_AND_BACK; }
    }

    LVN_ASSERT(false, "invalid cull face mode enum");
    return GL_BACK;
}

static GLenum lvn_getOglCullFrontFaceEnum(LvnCullFrontFace frontFace)
{
    switch (frontFace)
    {
        case Lvn_CullFrontFace_Clockwise: { return GL_CW; }
        case Lvn_CullFrontFace_CounterClockwise: { return GL_CCW; }
    }

    LVN_ASSERT(false, "invalid cull front face enum");
    return GL_CCW;
}

static GLenum lvn_getOglBlendFactorEnum(LvnColorBlendFactor blendFactor)
{
    switch (blendFactor)
    {
        case Lvn_ColorBlendFactor_Zero: { return GL_ZERO; }
        case Lvn_ColorBlendFactor_One: { return GL_ONE; }
        case Lvn_ColorBlendFactor_SrcColor: { return GL_SRC_COLOR; }
        case Lvn_ColorBlendFactor_OneMinusSrcColor: { return GL_ONE_MINUS_SRC_COLOR; }
        case Lvn_ColorBlendFactor_DstColor: { return GL_DST_COLOR; }
        case Lvn_ColorBlendFactor_OneMinusDstColor: { return GL_ONE_MINUS_DST_COLOR; }
        case Lvn_ColorBlendFactor_SrcAlpha: { return GL_SRC_ALPHA; }
        case Lvn_ColorBlendFactor_OneMinusSrcAlpha: { return GL_ONE_MINUS_SRC_ALPHA; }
        case Lvn_ColorBlendFactor_DstAlpha: { return GL_DST_ALPHA; }
        case Lvn_ColorBlendFactor_OneMinusDstAlpha: { return GL_ONE_MINUS_DST_ALPHA; }
        case Lvn_ColorBlendFactor_ConstantColor: { return GL_CONSTANT_COLOR; }
        case Lvn_ColorBlendFactor_OneMinusConstantColor: { return GL_ONE_MINUS_CONSTANT_COLOR; }
        case Lvn_ColorBlendFactor_ConstantAlpha: { return GL_CONSTANT_ALPHA; }
        case Lvn_ColorBlendFactor_OneMinusConstantAlpha: { return GL_ONE_MINUS_CONSTANT_ALPHA; }
        case Lvn_ColorBlendFactor_SrcAlphaSaturate: { return GL_SRC_ALPHA_SATURATE; }
        case Lvn_ColorBlendFactor_Src1Color: { return GL_SRC1_COLOR; }
        case Lvn_ColorBlendFactor_OneMinusSrc1Color: { return GL_ONE_MINUS_SRC1_COLOR; }
        case Lvn_ColorBlendFactor_Src1_Alpha: { return GL_SRC1_ALPHA; }
        case Lvn_ColorBlendFactor_OneMinusSrc1Alpha: { return GL_ONE_MINUS_SRC1_ALPHA; }
    }

    LVN_ASSERT(false, "invalid blend factor enum");
    return GL_ZERO;
}

static GLenum lvn_getOglBlendOperationEnum(LvnColorBlendOperation blendOp)
{
    switch (blendOp)
    {
        case Lvn_ColorBlendOp_Add: { return GL_FUNC_ADD; }
        case Lvn_ColorBlendOp_Subtract: { return GL_FUNC_SUBTRACT; }
        case Lvn_ColorBlendOp_ReverseSubtract: { return GL_FUNC_REVERSE_SUBTRACT; }
        case Lvn_ColorBlendOp_Min: { return GL_MIN; }
        case Lvn_ColorBlendOp_Max: { return GL_MAX; }
    }

    LVN_ASSERT(false, "invalid blend operation enum");
    return GL_FUNC_ADD;
}

static GLenum lvn_getOglCompareOpEnum(LvnCompareOperation compareOp)
{
    switch (compareOp)
    {
        case Lvn_CompareOp_Never: { return GL_NEVER; }
        case Lvn_CompareOp_Less: { return GL_LESS; }
        case Lvn_CompareOp_Equal: { return GL_EQUAL; }
        case Lvn_CompareOp_LessOrEqual: { return GL_LEQUAL; }
        case Lvn_CompareOp_Greater: { return GL_GREATER; }
        case Lvn_CompareOp_NotEqual: { return GL_NOTEQUAL; }
        case Lvn_CompareOp_GreaterOrEqual: { return GL_GEQUAL; }
        case Lvn_CompareOp_Always: { return GL_ALWAYS; }
    }

    LVN_ASSERT(false, "invalid compare operation enum");
    return GL_EQUAL;
}

static GLenum lvn_getOglStencilOpEnum(LvnStencilOperation stencilOp)
{
    switch (stencilOp)
    {
        case Lvn_StencilOp_Keep: { return GL_KEEP; }
        case Lvn_StencilOp_Zero: { return GL_ZERO; }
        case Lvn_StencilOp_Replace: { return GL_REPLACE; }
        case Lvn_StencilOp_IncrementAndClamp: { return GL_INCR; }
        case Lvn_StencilOp_DecrementAndClamp: { return GL_DECR; }
        case Lvn_StencilOp_Invert: { return GL_INVERT; }
        case Lvn_StencilOp_IncrementAndWrap: { return GL_INCR_WRAP; }
        case Lvn_StencilOp_DecrementAndWrap: { return GL_DECR_WRAP; }
    }

    LVN_ASSERT(false, "invalid stencil operation enum");
    return GL_KEEP;
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
        !oglBackends->glEnable ||
        !oglBackends->glEnablei ||
        !oglBackends->glDisable ||
        !oglBackends->glDisablei ||
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
        !oglBackends->glFenceSync ||
        !oglBackends->glDeleteSync ||
        !oglBackends->glClientWaitSync ||
        !oglBackends->glWaitSync ||
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
        !oglBackends->glGetProgramiv ||
        !oglBackends->glGetProgramInfoLog ||
        !oglBackends->glGetShaderInfoLog ||
        !oglBackends->glBlendFuncSeparatei ||
        !oglBackends->glBlendEquationSeparatei ||
        !oglBackends->glColorMaski ||
        !oglBackends->glPolygonMode ||
        !oglBackends->glNamedBufferStorage ||
        !oglBackends->glNamedBufferData ||
        !oglBackends->glMapNamedBufferRange ||
        !oglBackends->glUnmapNamedBuffer ||
        !oglBackends->glUseProgram ||
        !oglBackends->glBindBuffer ||
        !oglBackends->glBindVertexArray ||
        !oglBackends->glClear ||
        !oglBackends->glClearColor ||
        !oglBackends->glClearNamedFramebufferiv ||
        !oglBackends->glClearNamedFramebufferuiv ||
        !oglBackends->glClearNamedFramebufferfv ||
        !oglBackends->glClearNamedFramebufferfi ||
        !oglBackends->glDrawArraysInstanced ||
        !oglBackends->glDrawElementsInstanced)
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
    graphicsctx->implCreateBuffer = lvnImplOglCreateBuffer;
    graphicsctx->implDestroyBuffer = lvnImplOglDestroyBuffer;
    graphicsctx->implCreateSampler = lvnImplOglCreateSampler;
    graphicsctx->implDestroySampler = lvnImplOglDestroySampler;
    graphicsctx->implCreateTexture = lvnImplOglCreateTexture;
    graphicsctx->implDestroyTexture = lvnImplOglDestroyTexture;
    graphicsctx->implCreateCommandBuffer = lvnImplOglCreateCommandBuffer;
    graphicsctx->implDestroyCommandBuffer = lvnImplOglDestroyCommandBuffer;
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

        swapchain->pSwapchainImages[i].textureData = lvn_calloc(sizeof(LvnOglTextureData));
        LvnOglTextureData* textureData = (LvnOglTextureData*) swapchain->pSwapchainImages[i].textureData;
        textureData->textureId = swapchainData->images[i];
    }

    swapchain->swapchainData = swapchainData;
    swapchain->swapchainImageCount = createInfo->minImageCount;
    swapchain->extent = (LvnExtent2D){ .width = createInfo->width, .height = createInfo->height };

    return Lvn_Result_Success;

fail_cleanup:
    if (swapchainData)
    {
        if (swapchainData->images)
        {
            oglBackends->glDeleteTextures(createInfo->minImageCount, swapchainData->images);
            lvn_free(swapchainData->images);
        }

        lvn_free(swapchainData);
    }
    if (swapchain->pSwapchainImages)
    {
        for (uint32_t i = 0; i < createInfo->minImageCount; i++)
            lvn_free(swapchain->pSwapchainImages[i].textureData);
        lvn_free(swapchain->pSwapchainImages);
    }
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

    renderpass->renderpassData = renderpassData;

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

    LvnOglRenderpassData* renderpassData = (LvnOglRenderpassData*) renderpass->renderpassData;

    if (renderpassData->colorAttachments)
        lvn_free(renderpassData->colorAttachments);
    if (renderpassData->resolveAttachments)
        lvn_free(renderpassData->resolveAttachments);
    if (renderpassData->hasResolves)
        lvn_free(renderpassData->hasResolves);

    lvn_free(renderpassData);
    renderpass->renderpassData = NULL;
}

LvnResult lvnImplOglCreateFramebuffer(const LvnGraphicsContext* graphicsctx, LvnFramebuffer* framebuffer, const LvnFramebufferCreateInfo* createInfo)
{
    LVN_ASSERT(graphicsctx && framebuffer && createInfo, "graphicsctx, framebuffer, and createInfo cannot be null");

    const LvnOpenglBackends* oglBackends = (const LvnOpenglBackends*) graphicsctx->implData;
    const LvnOglRenderpassData* renderpassData = (const LvnOglRenderpassData*) createInfo->renderPass->renderpassData;

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
    {
        LvnOglTextureData* textureData = (LvnOglTextureData*) createInfo->pColorAttachments[i]->textureData;
        oglBackends->glNamedFramebufferTexture(framebufferData->fboId, GL_COLOR_ATTACHMENT0 + i, textureData->textureId, 0);
    }

    if (createInfo->depthStencilAttachment)
    {
        LvnOglTextureData* textureData = (LvnOglTextureData*) createInfo->depthStencilAttachment->textureData;
        GLenum attachment = lvn_getOglDepthStencilAttachmentTypeEnum(renderpassData->depthStencilAttachment.format);
        oglBackends->glNamedFramebufferTexture(framebufferData->fboId, attachment, textureData->textureId, 0);
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
        {
            LvnOglTextureData* textureData = (LvnOglTextureData*) createInfo->pResolveAttachments[i]->textureData;
            oglBackends->glNamedFramebufferTexture(framebufferData->resolveId, GL_COLOR_ATTACHMENT0 + i, textureData->textureId, 0);
        }
    }

    framebuffer->framebufferData = framebufferData;

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
    LvnOglFramebufferData* framebufferData = (LvnOglFramebufferData*) framebuffer->framebufferData;

    oglBackends->glDeleteFramebuffers(1, &framebufferData->fboId);
    oglBackends->glDeleteFramebuffers(1, &framebufferData->resolveId);

    lvn_free(framebufferData);
    framebuffer->framebufferData = NULL;
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
                      "[opengl] shader compile error in (%s) shader %p | info log: %s",
                      lvn_getShaderStageEnumName(createInfo->stage),
                      shader,
                      infoLog);

        goto fail_cleanup;
    }

    shaderData->stage = createInfo->stage;

    shader->shaderData = shaderData;

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

    LvnOglShaderData* shaderData = (LvnOglShaderData*) shader->shaderData;

    oglBackends->glDeleteShader(shaderData->shaderId);
    lvn_free(shaderData);

    shader->shaderData = NULL;
}

LvnResult lvnImplOglCreatePipeline(const LvnGraphicsContext* graphicsctx, LvnPipeline* pipeline, const LvnPipelineCreateInfo* createInfo)
{
    LVN_ASSERT(graphicsctx && pipeline && createInfo, "graphicsctx, pipeline, and createInfo cannot be null");

    const LvnOpenglBackends* oglBackends = (const LvnOpenglBackends*) graphicsctx->implData;

    LvnResult errResult = Lvn_Result_Failure;
    LvnOglPipelineData* pipelineData = NULL;

    pipelineData = (LvnOglPipelineData*) lvn_calloc(sizeof(LvnOglPipelineData));
    if (!pipelineData)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[opengl] failed to allocate memory for pipeline data in pipeline %p",
                      pipeline);
        errResult = Lvn_Result_OutOfMemory;
        goto fail_cleanup;
    }

    // create vao
    oglBackends->glCreateVertexArrays(1, &pipelineData->vaoId);

    // create pipeline/program
    pipelineData->pipelineId = oglBackends->glCreateProgram();

    // link shaders
    for (uint32_t i = 0; i < createInfo->stageCount; i++)
    {
        LvnOglShaderData* shaderData = (LvnOglShaderData*) createInfo->pShaderStages[i]->shaderData;
        oglBackends->glAttachShader(pipelineData->pipelineId, shaderData->shaderId);
    }

    oglBackends->glLinkProgram(pipelineData->pipelineId);

    GLint success;
    oglBackends->glGetProgramiv(pipelineData->pipelineId, GL_LINK_STATUS, &success);

    if (!success)
    {
        char infoLog[1024];
        oglBackends->glGetProgramInfoLog(pipelineData->pipelineId, 1024, NULL, infoLog);

        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[opengl] program compile error in pipeline %p | info log: %s",
                      pipeline,
                      infoLog);

        goto fail_cleanup;
    }

    // pipeline fixed functions
    const LvnPipelineFixedFunctions* pipelineFixedFunctions = createInfo->pipelineFixedFunctions;

    // input assembly
    pipelineData->fixedFuncEnums.inputAssembly = pipelineFixedFunctions->inputAssembly;
    pipelineData->fixedFuncEnums.primitiveMode = lvn_getOglTopologyEnum(pipelineFixedFunctions->inputAssembly.topology);

    // rasterizer
    pipelineData->fixedFuncEnums.rasterizer = pipelineFixedFunctions->rasterizer;

    if (pipelineFixedFunctions->rasterizer.cullMode != Lvn_CullFaceMode_Disable)
    {
        pipelineData->fixedFuncEnums.cullMode = lvn_getOglCullModeFlagEnum(pipelineFixedFunctions->rasterizer.cullMode);
        pipelineData->fixedFuncEnums.frontFace = lvn_getOglCullFrontFaceEnum(pipelineFixedFunctions->rasterizer.frontFace);
    }

    // multisampling
    pipelineData->fixedFuncEnums.multisampling = pipelineFixedFunctions->multisampling;

    // color blend
    pipelineData->fixedFuncEnums.colorBlend = pipelineFixedFunctions->colorBlend;

    pipelineData->fixedFuncEnums.colorBlendAttachmentCount =
        (pipelineFixedFunctions->colorBlend.colorBlendAttachmentCount == 0)
        ? 1
        : pipelineFixedFunctions->colorBlend.colorBlendAttachmentCount;

    pipelineData->fixedFuncEnums.pColorBlendAttachments = (LvnOglColorBlendAttachment*)
        lvn_calloc(pipelineData->fixedFuncEnums.colorBlendAttachmentCount * sizeof(LvnOglColorBlendAttachment));
    if (!pipelineData->fixedFuncEnums.pColorBlendAttachments)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[opengl] failed to allocate memory for color blend attachments in pipeline %p",
                      pipeline);
        errResult = Lvn_Result_OutOfMemory;
        goto fail_cleanup;
    }

    if (pipelineFixedFunctions->colorBlend.colorBlendAttachmentCount == 0)
    {
        pipelineData->fixedFuncEnums.pColorBlendAttachments[0].srcRGB = GL_ONE;
        pipelineData->fixedFuncEnums.pColorBlendAttachments[0].dstRGB = GL_ZERO;
        pipelineData->fixedFuncEnums.pColorBlendAttachments[0].srcAlpha = GL_ONE;
        pipelineData->fixedFuncEnums.pColorBlendAttachments[0].dstAlpha = GL_ZERO;
        pipelineData->fixedFuncEnums.pColorBlendAttachments[0].modeRGB = GL_FUNC_ADD;
        pipelineData->fixedFuncEnums.pColorBlendAttachments[0].modeAlpha = GL_FUNC_ADD;
        pipelineData->fixedFuncEnums.pColorBlendAttachments[0].writeMaskR = true;
        pipelineData->fixedFuncEnums.pColorBlendAttachments[0].writeMaskG = true;
        pipelineData->fixedFuncEnums.pColorBlendAttachments[0].writeMaskB = true;
        pipelineData->fixedFuncEnums.pColorBlendAttachments[0].writeMaskA = true;
        pipelineData->fixedFuncEnums.pColorBlendAttachments[0].blendEnable = false;
    }
    else
    {
        for (uint32_t i = 0; i < pipelineData->fixedFuncEnums.colorBlendAttachmentCount; i++)
        {
            pipelineData->fixedFuncEnums.pColorBlendAttachments[i].srcRGB =
                lvn_getOglBlendFactorEnum(pipelineFixedFunctions->colorBlend.pColorBlendAttachments[i].srcColorBlendFactor);
            pipelineData->fixedFuncEnums.pColorBlendAttachments[i].dstRGB =
                lvn_getOglBlendFactorEnum(pipelineFixedFunctions->colorBlend.pColorBlendAttachments[i].dstColorBlendFactor);
            pipelineData->fixedFuncEnums.pColorBlendAttachments[i].srcAlpha =
                lvn_getOglBlendFactorEnum(pipelineFixedFunctions->colorBlend.pColorBlendAttachments[i].srcAlphaBlendFactor);
            pipelineData->fixedFuncEnums.pColorBlendAttachments[i].dstAlpha =
                lvn_getOglBlendFactorEnum(pipelineFixedFunctions->colorBlend.pColorBlendAttachments[i].dstAlphaBlendFactor);
            pipelineData->fixedFuncEnums.pColorBlendAttachments[i].modeRGB =
                lvn_getOglBlendOperationEnum(pipelineFixedFunctions->colorBlend.pColorBlendAttachments[i].colorBlendOp);
            pipelineData->fixedFuncEnums.pColorBlendAttachments[i].modeAlpha =
                lvn_getOglBlendOperationEnum(pipelineFixedFunctions->colorBlend.pColorBlendAttachments[i].alphaBlendOp);
            pipelineData->fixedFuncEnums.pColorBlendAttachments[i].writeMaskR =
                (pipelineFixedFunctions->colorBlend.pColorBlendAttachments[i].colorWriteMask & Lvn_ColorComponentFlag_R) ? true : false;
            pipelineData->fixedFuncEnums.pColorBlendAttachments[i].writeMaskG =
                (pipelineFixedFunctions->colorBlend.pColorBlendAttachments[i].colorWriteMask & Lvn_ColorComponentFlag_G) ? true : false;
            pipelineData->fixedFuncEnums.pColorBlendAttachments[i].writeMaskB =
                (pipelineFixedFunctions->colorBlend.pColorBlendAttachments[i].colorWriteMask & Lvn_ColorComponentFlag_B) ? true : false;
            pipelineData->fixedFuncEnums.pColorBlendAttachments[i].writeMaskA =
                (pipelineFixedFunctions->colorBlend.pColorBlendAttachments[i].colorWriteMask & Lvn_ColorComponentFlag_A) ? true : false;
            pipelineData->fixedFuncEnums.pColorBlendAttachments[i].blendEnable =
                pipelineFixedFunctions->colorBlend.pColorBlendAttachments[i].blendEnable;
        }
    }

    // depth stencil
    pipelineData->fixedFuncEnums.depthStencil = pipelineFixedFunctions->depthstencil;

    pipelineData->fixedFuncEnums.depthCompareOp = lvn_getOglCompareOpEnum(pipelineFixedFunctions->depthstencil.depthOpCompare);
    pipelineData->fixedFuncEnums.stencilCompareOp = lvn_getOglCompareOpEnum(pipelineFixedFunctions->depthstencil.stencil.compareOp);
    pipelineData->fixedFuncEnums.stencilFailOp = lvn_getOglStencilOpEnum(pipelineFixedFunctions->depthstencil.stencil.failOp);
    pipelineData->fixedFuncEnums.stencilPassOp = lvn_getOglStencilOpEnum(pipelineFixedFunctions->depthstencil.stencil.passOp);
    pipelineData->fixedFuncEnums.stencilDepthFailOp = lvn_getOglStencilOpEnum(pipelineFixedFunctions->depthstencil.stencil.depthFailOp);

    pipeline->pipelineData = pipelineData;

    return Lvn_Result_Success;

fail_cleanup:
    if (pipelineData)
    {
        oglBackends->glDeleteVertexArrays(1, &pipelineData->vaoId);
        oglBackends->glDeleteProgram(pipelineData->pipelineId);
        lvn_free(pipelineData->fixedFuncEnums.pColorBlendAttachments);
        lvn_free(pipelineData);
    }
    return errResult;
}

void lvnImplOglDestroyPipeline(LvnPipeline* pipeline)
{
    LVN_ASSERT(pipeline, "pipeline cannot be null");

    const LvnOpenglBackends* oglBackends = (const LvnOpenglBackends*) pipeline->graphicsctx->implData;

    LvnOglPipelineData* pipelineData = (LvnOglPipelineData*) pipeline->pipelineData;

    oglBackends->glDeleteVertexArrays(1, &pipelineData->vaoId);
    oglBackends->glDeleteProgram(pipelineData->pipelineId);

    lvn_free(pipelineData->fixedFuncEnums.pColorBlendAttachments);
    lvn_free(pipelineData);

    pipeline->pipelineData = NULL;
}

LvnResult lvnImplOglCreateFence(const LvnGraphicsContext* graphicsctx, LvnFence* fence, bool signaled)
{
    LVN_ASSERT(graphicsctx && fence, "graphicsctx and fence cannot be null");

    const LvnOpenglBackends* oglBackends = (const LvnOpenglBackends*) graphicsctx->implData;

    LvnResult errResult = Lvn_Result_Failure;
    LvnOglFenceData* fenceData = NULL;

    fenceData = (LvnOglFenceData*) lvn_calloc(sizeof(LvnOglFenceData));
    if (!fenceData)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[opengl] failed to allocate memory for fence data in fence %p",
                      fence);
        errResult = Lvn_Result_OutOfMemory;
        goto fail_cleanup;
    }

    fence->fenceData = fenceData;

    return Lvn_Result_Success;

fail_cleanup:
    if (fenceData)
    {
        if (fenceData->fenceId)
            oglBackends->glDeleteSync(fenceData->fenceId);
        lvn_free(fenceData);
    }
    return errResult;
}

void lvnImplOglDestroyFence(LvnFence* fence)
{
    LVN_ASSERT(fence, "fence cannot be null");

    const LvnOpenglBackends* oglBackends = (const LvnOpenglBackends*) fence->graphicsctx->implData;

    LvnOglFenceData* fenceData = (LvnOglFenceData*) fence->fenceData;

    oglBackends->glDeleteSync(fenceData->fenceId);
    lvn_free(fenceData);

    fence->fenceData = NULL;
}

LvnResult lvnImplOglCreateSemaphore(const LvnGraphicsContext* graphicsctx, LvnSemaphore* semaphore)
{
    LVN_ASSERT(graphicsctx && semaphore, "graphicsctx and semaphore cannot be null");

    return Lvn_Result_Success;
}

void lvnImplOglDestroySemaphore(LvnSemaphore* semaphore)
{
    LVN_ASSERT(semaphore, "semaphore cannot be null");
}

LvnResult lvnImplOglCreateBuffer(const LvnGraphicsContext* graphicsctx, LvnBuffer* buffer, const LvnBufferCreateInfo* createInfo)
{
    LVN_ASSERT(graphicsctx && buffer && createInfo, "graphicsctx, buffer, and createInfo cannot be null");

    const LvnOpenglBackends* oglBackends = (const LvnOpenglBackends*) graphicsctx->implData;

    LvnResult errResult = Lvn_Result_Failure;
    LvnOglBufferData* bufferData = NULL;

    bufferData = (LvnOglBufferData*) lvn_calloc(sizeof(LvnOglBufferData));
    if (!bufferData)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[opengl] failed to allocate memory for buffer data in buffer %p",
                      buffer);
        errResult = Lvn_Result_OutOfMemory;
        goto fail_cleanup;
    }

    oglBackends->glCreateBuffers(1, &bufferData->bufferId);

    if (createInfo->usage == Lvn_BufferMemoryUsage_GpuOnly)
    {
        oglBackends->glNamedBufferStorage(bufferData->bufferId, createInfo->size, createInfo->data, GL_DYNAMIC_STORAGE_BIT);
    }
    else
    {
        GLbitfield flags = GL_MAP_READ_BIT | GL_MAP_PERSISTENT_BIT;
        if (createInfo->usage == Lvn_BufferMemoryUsage_CpuToGpu)
            flags |= GL_MAP_WRITE_BIT;
        else if (createInfo->usage == Lvn_BufferMemoryUsage_GpuToCpu)
            flags |= GL_MAP_READ_BIT;

        oglBackends->glNamedBufferStorage(bufferData->bufferId, createInfo->size, createInfo->data, flags);
        bufferData->bufferMap = oglBackends->glMapNamedBufferRange(bufferData->bufferId, 0, createInfo->size, flags);
    }

    buffer->bufferData = bufferData;

    return Lvn_Result_Success;

fail_cleanup:
    if (bufferData)
    {
        if (bufferData->bufferMap)
            oglBackends->glUnmapNamedBuffer(bufferData->bufferId);

        oglBackends->glDeleteBuffers(1, &bufferData->bufferId);
        lvn_free(bufferData);
    }
    return errResult;
}

void lvnImplOglDestroyBuffer(LvnBuffer* buffer)
{
    LVN_ASSERT(buffer, "buffer cannot be null");

    const LvnOpenglBackends* oglBackends = (const LvnOpenglBackends*) buffer->graphicsctx->implData;

    LvnOglBufferData* bufferData = (LvnOglBufferData*) buffer->bufferData;

    if (bufferData->bufferMap)
        oglBackends->glUnmapNamedBuffer(bufferData->bufferId);

    oglBackends->glDeleteBuffers(1, &bufferData->bufferId);
    lvn_free(bufferData);

    buffer->bufferData = NULL;
}

LvnResult lvnImplOglCreateSampler(const LvnGraphicsContext* graphicsctx, LvnSampler* sampler, const LvnSamplerCreateInfo* createInfo)
{
    LVN_ASSERT(graphicsctx && sampler && createInfo, "graphicsctx, sampler, and createInfo cannot be null");

    const LvnOpenglBackends* oglBackends = (const LvnOpenglBackends*) graphicsctx->implData;

    LvnResult errResult = Lvn_Result_Failure;
    LvnOglSamplerData* samplerData = NULL;

    samplerData = (LvnOglSamplerData*) lvn_calloc(sizeof(LvnOglSamplerData));
    if (!samplerData)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[opengl] failed to allocate memory for sampler data in sampler %p",
                      sampler);
        errResult = Lvn_Result_OutOfMemory;
        goto fail_cleanup;
    }

    oglBackends->glCreateSamplers(1, &samplerData->samplerId);

    oglBackends->glSamplerParameteri(samplerData->samplerId, GL_TEXTURE_WRAP_S, lvn_getOglTextureModeEnum(createInfo->wrapS));
    oglBackends->glSamplerParameteri(samplerData->samplerId, GL_TEXTURE_WRAP_T, lvn_getOglTextureModeEnum(createInfo->wrapT));
    oglBackends->glSamplerParameteri(samplerData->samplerId, GL_TEXTURE_WRAP_R, lvn_getOglTextureModeEnum(createInfo->wrapR));
    oglBackends->glSamplerParameteri(samplerData->samplerId, GL_TEXTURE_MIN_FILTER, lvn_getOglTextureFilterEnum(createInfo->minFilter, createInfo->mipmapMode));
    oglBackends->glSamplerParameteri(samplerData->samplerId, GL_TEXTURE_MAG_FILTER, lvn_getOglTextureFilterEnum(createInfo->minFilter, Lvn_MipmapMode_Disabled));

    sampler->samplerData = samplerData;

    return Lvn_Result_Success;

fail_cleanup:
    if (samplerData)
    {
        oglBackends->glDeleteSamplers(1, &samplerData->samplerId);
        lvn_free(samplerData);
    }
    return errResult;
}

void lvnImplOglDestroySampler(LvnSampler* sampler)
{
    LVN_ASSERT(sampler, "sampler cannot be null");
    const LvnOpenglBackends* oglBackends = (const LvnOpenglBackends*) sampler->graphicsctx->implData;
    LvnOglSamplerData* samplerData = (LvnOglSamplerData*) sampler->samplerData;
    oglBackends->glDeleteSamplers(1, &samplerData->samplerId);
    lvn_free(samplerData);
    sampler->samplerData = NULL;
}

LvnResult lvnImplOglCreateTexture(const LvnGraphicsContext* graphicsctx, LvnTexture* texture, const LvnTextureCreateInfo* createInfo)
{
    LVN_ASSERT(graphicsctx && texture && createInfo, "graphicsctx, texture, and createInfo cannot be null");

    const LvnOpenglBackends* oglBackends = (const LvnOpenglBackends*) graphicsctx->implData;
    const LvnImage* image = createInfo->image;

    LvnResult errResult = Lvn_Result_Failure;
    LvnOglTextureData* textureData = NULL;

    textureData = (LvnOglTextureData*) lvn_calloc(sizeof(LvnOglTextureData*));
    if (!textureData)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[opengl] failed to allocate memory for texture data in texture %p",
                      texture);
        errResult = Lvn_Result_OutOfMemory;
        goto fail_cleanup;
    }

    oglBackends->glCreateTextures(GL_TEXTURE_2D, 1, &textureData->textureId);

    GLenum internalFormat = lvn_getOglInternalFormatEnum(createInfo->format);
    GLenum dataFormat = lvn_getOglDataFormatEnum(createInfo->format);
    GLenum formatType = lvn_getOglFormatTypeEnum(createInfo->format);

    if (createInfo->samples == Lvn_SampleCountFlag_1_Bit)
        oglBackends->glTextureStorage2D(textureData->textureId, 1, internalFormat, createInfo->width, createInfo->height);
    else
    {
        oglBackends->glTextureStorage2DMultisample(textureData->textureId,
                                                   lvn_getSampleCount(createInfo->samples),
                                                   internalFormat,
                                                   createInfo->width,
                                                   createInfo->height,
                                                   GL_TRUE);
    }

    oglBackends->glTextureSubImage2D(textureData->textureId, 0, 0, 0, createInfo->width, createInfo->height, dataFormat, formatType, image->data);

    texture->textureData = textureData;
    texture->width = createInfo->width;
    texture->height = createInfo->height;

    return Lvn_Result_Success;

fail_cleanup:
    if (textureData)
    {
        oglBackends->glDeleteTextures(1, &textureData->textureId);
        lvn_free(textureData);
    }
    return errResult;
}

void lvnImplOglDestroyTexture(LvnTexture* texture)
{
    LVN_ASSERT(texture, "texture cannot be null");
    const LvnOpenglBackends* oglBackends = (const LvnOpenglBackends*) texture->graphicsctx->implData;
    LvnOglTextureData* textureData = (LvnOglTextureData*) texture->textureData;
    oglBackends->glDeleteTextures(1, &textureData->textureId);
    lvn_free(textureData);
    texture->textureData = NULL;
}

LvnResult lvnImplOglCreateCommandBuffer(const LvnGraphicsContext* graphicsctx, LvnCommandBuffer* commandBuffer)
{
    LVN_ASSERT(graphicsctx && commandBuffer, "graphicsctx and commandBuffer cannot be null");

    return Lvn_Result_Success;
}

void lvnImplOglDestroyCommandBuffer(LvnCommandBuffer* commandBuffer)
{
    LVN_ASSERT(commandBuffer, "commandBuffer cannot be null");
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
    LVN_ASSERT(fence, "fence cannot be null");

    const LvnOpenglBackends* oglBackends = (const LvnOpenglBackends*) fence->graphicsctx->implData;

    LvnOglFenceData* fenceData = (LvnOglFenceData*) fence->fenceData;

    if (fenceData->fenceId)
    {
        GLenum result = oglBackends->glClientWaitSync(fenceData->fenceId, GL_SYNC_FLUSH_COMMANDS_BIT, timeout);

        if (result == GL_TIMEOUT_EXPIRED)
            return Lvn_Result_TimeOut;

        oglBackends->glDeleteSync(fenceData->fenceId);
        fenceData->fenceId = NULL;

        if (result == GL_ALREADY_SIGNALED || result == GL_CONDITION_SATISFIED)
            return Lvn_Result_Success;
        else if (result == GL_WAIT_FAILED)
            return Lvn_Result_Failure;
        else
            return Lvn_Result_Failure;
    }

    return Lvn_Result_Success;
}

LvnResult lvnImplOglFenceReset(LvnFence* fence)
{
    LVN_ASSERT(fence, "fence cannot be null");

    const LvnOpenglBackends* oglBackends = (const LvnOpenglBackends*) fence->graphicsctx->implData;

    LvnOglFenceData* fenceData = (LvnOglFenceData*) fence->fenceData;

    if (fenceData->fenceId)
    {
        oglBackends->glDeleteSync(fenceData->fenceId);
        fenceData->fenceId = NULL;
        fenceData->pending = true;
        return Lvn_Result_Success;
    }

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
    LVN_ASSERT(graphicsctx, "graphicsctx cannot be null");

    const LvnOpenglBackends* oglBackends = (const LvnOpenglBackends*) graphicsctx->implData;

    LvnOglFenceData* fenceData = (LvnOglFenceData*) fence->fenceData;

    if (fenceData->pending)
    {
        fenceData->pending = false;
        fenceData->fenceId = oglBackends->glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    }

    return Lvn_Result_Success;
}

LvnResult lvnImplOglRenderPresent(const LvnGraphicsContext* graphicsctx, const LvnPresentInfo* presentInfo)
{
    return Lvn_Result_Success;
}

void lvnCmdBuffImplOglCmdBeginRenderPass(void* data)
{
    LVN_ASSERT(data, "data cannot be null");

    const LvnOglCmdBuffBeginRenderPassData* cmdData = (const LvnOglCmdBuffBeginRenderPassData*) data;
    const LvnOpenglBackends* oglBackends = (const LvnOpenglBackends*) cmdData->commandBuffer->graphicsctx->implData;
    const LvnRenderPassBeginInfo* beginInfo = (const LvnRenderPassBeginInfo*) cmdData->beginInfo;
    const LvnOglRenderpassData* renderpassData = (const LvnOglRenderpassData*) beginInfo->renderPass->renderpassData;
    const LvnOglFramebufferData* framebufferData = (const LvnOglFramebufferData*) beginInfo->framebuffer->framebufferData;

    for (uint32_t i = 0; i < beginInfo->clearColorValueCount; i++)
    {
        oglBackends->glClearNamedFramebufferfv(framebufferData->fboId, GL_COLOR, i, beginInfo->pClearColorValues[i].float32);
    }

    if (renderpassData->hasDepth)
    {
        if (renderpassData->depthStencilAttachment.format == Lvn_Format_D32_FLOAT)
            oglBackends->glClearNamedFramebufferfv(framebufferData->fboId, GL_DEPTH, 0, &beginInfo->clearDepthStencilValue.depth);
        else if (renderpassData->depthStencilAttachment.format == Lvn_Format_D24_UNORM_S8_UINT)
            oglBackends->glClearNamedFramebufferfi(framebufferData->fboId, GL_DEPTH_STENCIL, 0, beginInfo->clearDepthStencilValue.depth, beginInfo->clearDepthStencilValue.stencil);
    }
}

void lvnCmdBuffImplOglCmdEndRenderPass(void* data)
{
    LVN_ASSERT(data, "data cannot be null");
}

void lvnCmdBuffImplOglCmdBindPipeline(void* data)
{
    LVN_ASSERT(data, "data cannot be null");

    const LvnOglCmdBuffBindPipelineData* cmdData = (const LvnOglCmdBuffBindPipelineData*) data;
    const LvnOpenglBackends* oglBackends = (const LvnOpenglBackends*) cmdData->commandBuffer->graphicsctx->implData;
    const LvnOglPipelineData* pipelineData = (const LvnOglPipelineData*) cmdData->pipeline->pipelineData;

    oglBackends->glUseProgram(pipelineData->pipelineId);
}

void lvnCmdBuffImplOglCmdBindVertexBuffer(void* data)
{
    LVN_ASSERT(data, "data cannot be null");

}

void lvnCmdBuffImplOglCmdBindIndexBuffer(void* data)
{
    LVN_ASSERT(data, "data cannot be null");

}

void lvnCmdBuffImplOglCmdSetViewport(void* data)
{
    LVN_ASSERT(data, "data cannot be null");

}

void lvnCmdBuffImplOglCmdSetScissor(void* data)
{
    LVN_ASSERT(data, "data cannot be null");

}

void lvnCmdBuffImplOglCmdDraw(void* data)
{
    LVN_ASSERT(data, "data cannot be null");

}

void lvnCmdBuffImplOglCmdDrawIndexed(void* data)
{
    LVN_ASSERT(data, "data cannot be null");

}
