#ifndef HG_LVN_IMPL_OGL_H
#define HG_LVN_IMPL_OGL_H

#include "lvn_graphics_internal.h"
#include "levikno_internal.h"

#include <KHR/khrplatform.h>

#define GLAPI KHRONOS_APICALL
#define GLAPIENTRY KHRONOS_APIENTRY

#define GL_FALSE 0
#define GL_TRUE 1
#define GL_POINTS 0x0000
#define GL_LINES 0x0001
#define GL_LINE_LOOP 0x0002
#define GL_LINE_STRIP 0x0003
#define GL_TRIANGLES 0x0004
#define GL_TRIANGLE_STRIP 0x0005
#define GL_TRIANGLE_FAN 0x0006
#define GL_NEVER 0x0200
#define GL_LESS 0x0201
#define GL_EQUAL 0x0202
#define GL_LEQUAL 0x0203
#define GL_GREATER 0x0204
#define GL_NOTEQUAL 0x0205
#define GL_GEQUAL 0x0206
#define GL_ALWAYS 0x0207
#define GL_ZERO 0
#define GL_ONE 1
#define GL_KEEP 0x1E00
#define GL_REPLACE 0x1E01
#define GL_INCR 0x1E02
#define GL_DECR 0x1E03
#define GL_INVERT 0x150A
#define GL_INCR_WRAP 0x8507
#define GL_DECR_WRAP 0x8508
#define GL_SRC_COLOR 0x0300
#define GL_ONE_MINUS_SRC_COLOR 0x0301
#define GL_SRC_ALPHA 0x0302
#define GL_ONE_MINUS_SRC_ALPHA 0x0303
#define GL_DST_ALPHA 0x0304
#define GL_ONE_MINUS_DST_ALPHA 0x0305
#define GL_DST_COLOR 0x0306
#define GL_ONE_MINUS_DST_COLOR 0x0307
#define GL_SRC_ALPHA_SATURATE 0x0308
#define GL_CONSTANT_COLOR 0x8001
#define GL_ONE_MINUS_CONSTANT_COLOR 0x8002
#define GL_CONSTANT_ALPHA 0x8003
#define GL_ONE_MINUS_CONSTANT_ALPHA 0x8004
#define GL_SRC1_COLOR 0x88F9
#define GL_SRC1_ALPHA 0x8589
#define GL_ONE_MINUS_SRC1_COLOR 0x88FA
#define GL_ONE_MINUS_SRC1_ALPHA 0x88FB
#define GL_NONE 0
#define GL_FUNC_ADD 0x8006
#define GL_FUNC_REVERSE_SUBTRACT 0x800B
#define GL_FUNC_SUBTRACT 0x800A
#define GL_MIN 0x8007
#define GL_MAX 0x8008
#define GL_FRONT_LEFT 0x0400
#define GL_FRONT_RIGHT 0x0401
#define GL_BACK_LEFT 0x0402
#define GL_BACK_RIGHT 0x0403
#define GL_FRONT 0x0404
#define GL_BACK 0x0405
#define GL_LEFT 0x0406
#define GL_RIGHT 0x0407
#define GL_FRONT_AND_BACK 0x0408
#define GL_NO_ERROR 0
#define GL_INVALID_ENUM 0x0500
#define GL_INVALID_VALUE 0x0501
#define GL_INVALID_OPERATION 0x0502
#define GL_OUT_OF_MEMORY 0x0505
#define GL_CW 0x0900
#define GL_CCW 0x0901
#define GL_CONTEXT_FLAGS 0x821E
#define GL_CONTEXT_FLAG_DEBUG_BIT 0x00000002
#define GL_DEBUG_SOURCE_API 0x8246
#define GL_DEBUG_SOURCE_WINDOW_SYSTEM 0x8247
#define GL_DEBUG_SOURCE_SHADER_COMPILER 0x8248
#define GL_DEBUG_SOURCE_THIRD_PARTY 0x8249
#define GL_DEBUG_SOURCE_APPLICATION 0x824A
#define GL_DEBUG_SOURCE_OTHER 0x824B
#define GL_DEBUG_TYPE_ERROR 0x824C
#define GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR 0x824D
#define GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR 0x824E
#define GL_DEBUG_TYPE_PORTABILITY 0x824F
#define GL_DEBUG_TYPE_PERFORMANCE 0x8250
#define GL_DEBUG_TYPE_OTHER 0x8251
#define GL_DEBUG_TYPE_MARKER 0x8268
#define GL_DEBUG_SEVERITY_HIGH 0x9146
#define GL_DEBUG_SEVERITY_MEDIUM 0x9147
#define GL_DEBUG_SEVERITY_LOW 0x9148
#define GL_DEBUG_SEVERITY_NOTIFICATION 0x826B
#define GL_DEBUG_OUTPUT_SYNCHRONOUS 0x8242
#define GL_DEBUG_OUTPUT 0x92E0
#define GL_MAJOR_VERSION 0x821B
#define GL_MINOR_VERSION 0x821C
#define GL_FRAMEBUFFER_COMPLETE 0x8CD5
#define GL_MAX_COLOR_ATTACHMENTS 0x8CDF
#define GL_MAX_DRAW_BUFFERS 0x8824
#define GL_COLOR_ATTACHMENT0 0x8CE0
#define GL_COLOR_ATTACHMENT1 0x8CE1
#define GL_COLOR_ATTACHMENT2 0x8CE2
#define GL_COLOR_ATTACHMENT3 0x8CE3
#define GL_COLOR_ATTACHMENT4 0x8CE4
#define GL_COLOR_ATTACHMENT5 0x8CE5
#define GL_COLOR_ATTACHMENT6 0x8CE6
#define GL_COLOR_ATTACHMENT7 0x8CE7
#define GL_COLOR_ATTACHMENT8 0x8CE8
#define GL_COLOR_ATTACHMENT9 0x8CE9
#define GL_COLOR_ATTACHMENT10 0x8CEA
#define GL_COLOR_ATTACHMENT11 0x8CEB
#define GL_COLOR_ATTACHMENT12 0x8CEC
#define GL_COLOR_ATTACHMENT13 0x8CED
#define GL_COLOR_ATTACHMENT14 0x8CEE
#define GL_COLOR_ATTACHMENT15 0x8CEF
#define GL_COLOR_ATTACHMENT16 0x8CF0
#define GL_COLOR_ATTACHMENT17 0x8CF1
#define GL_COLOR_ATTACHMENT18 0x8CF2
#define GL_COLOR_ATTACHMENT19 0x8CF3
#define GL_COLOR_ATTACHMENT20 0x8CF4
#define GL_COLOR_ATTACHMENT21 0x8CF5
#define GL_COLOR_ATTACHMENT22 0x8CF6
#define GL_COLOR_ATTACHMENT23 0x8CF7
#define GL_COLOR_ATTACHMENT24 0x8CF8
#define GL_COLOR_ATTACHMENT25 0x8CF9
#define GL_COLOR_ATTACHMENT26 0x8CFA
#define GL_COLOR_ATTACHMENT27 0x8CFB
#define GL_COLOR_ATTACHMENT28 0x8CFC
#define GL_COLOR_ATTACHMENT29 0x8CFD
#define GL_COLOR_ATTACHMENT30 0x8CFE
#define GL_COLOR_ATTACHMENT31 0x8CFF
#define GL_DEPTH_ATTACHMENT 0x8D00
#define GL_STENCIL_ATTACHMENT 0x8D20
#define GL_DEPTH_STENCIL_ATTACHMENT 0x821A
#define GL_TEXTURE_2D 0x0DE1
#define GL_NEAREST 0x2600
#define GL_LINEAR 0x2601
#define GL_NEAREST_MIPMAP_NEAREST 0x2700
#define GL_LINEAR_MIPMAP_NEAREST 0x2701
#define GL_NEAREST_MIPMAP_LINEAR 0x2702
#define GL_LINEAR_MIPMAP_LINEAR 0x2703
#define GL_TEXTURE_MAG_FILTER 0x2800
#define GL_TEXTURE_MIN_FILTER 0x2801
#define GL_TEXTURE_WRAP_S 0x2802
#define GL_TEXTURE_WRAP_T 0x2803
#define GL_TEXTURE_WRAP_R 0x8072
#define GL_REPEAT 0x2901
#define GL_MIRRORED_REPEAT 0x8370
#define GL_CLAMP_TO_BORDER 0x812D
#define GL_CLAMP_TO_EDGE 0x812F
#define GL_BYTE 0x1400
#define GL_UNSIGNED_BYTE 0x1401
#define GL_SHORT 0x1402
#define GL_UNSIGNED_SHORT 0x1403
#define GL_INT 0x1404
#define GL_UNSIGNED_INT 0x1405
#define GL_FLOAT 0x1406
#define GL_HALF_FLOAT 0x140B
#define GL_UNSIGNED_INT_24_8 0x84FA
#define GL_NONE 0
#define GL_RED 0x1903
#define GL_RG 0x8227
#define GL_RGB 0x1907
#define GL_RGBA 0x1908
#define GL_BGR 0x80E0
#define GL_BGRA 0x80E1
#define GL_DEPTH_COMPONENT 0x1902
#define GL_DEPTH_STENCIL 0x84F9
#define GL_R8 0x8229
#define GL_R16 0x822A
#define GL_RG8 0x822B
#define GL_RG16 0x822C
#define GL_R16F 0x822D
#define GL_R32F 0x822E
#define GL_RG16F 0x822F
#define GL_RG32F 0x8230
#define GL_RGB8 0x8051
#define GL_RGBA8 0x8058
#define GL_RGBA32F 0x8814
#define GL_RGB32F 0x8815
#define GL_RGBA16F 0x881A
#define GL_RGB16F 0x881B
#define GL_SRGB 0x8C40
#define GL_SRGB8 0x8C41
#define GL_SRGB_ALPHA 0x8C42
#define GL_SRGB8_ALPHA8 0x8C43
#define GL_DEPTH_COMPONENT32F 0x8CAC
#define GL_DEPTH24_STENCIL8 0x88F0
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_VERTEX_SHADER 0x8B31
#define GL_COMPILE_STATUS 0x8B81
#define GL_LINK_STATUS 0x8B82
#define GL_MAP_READ_BIT 0x0001
#define GL_MAP_WRITE_BIT 0x0002
#define GL_MAP_PERSISTENT_BIT 0x0040
#define GL_MAP_COHERENT_BIT 0x0080
#define GL_DYNAMIC_STORAGE_BIT 0x0100
#define GL_SYNC_GPU_COMMANDS_COMPLETE 0x9117
#define GL_UNSIGNALED 0x9118
#define GL_SIGNALED 0x9119
#define GL_ALREADY_SIGNALED 0x911A
#define GL_TIMEOUT_EXPIRED 0x911B
#define GL_CONDITION_SATISFIED 0x911C
#define GL_WAIT_FAILED 0x911D
#define GL_TIMEOUT_IGNORED 0xFFFFFFFFFFFFFFFF
#define GL_SYNC_FLUSH_COMMANDS_BIT 0x00000001

typedef khronos_int8_t GLbyte;
typedef khronos_uint8_t GLubyte;
typedef khronos_int16_t GLshort;
typedef khronos_uint16_t GLushort;
typedef khronos_float_t GLfloat;
typedef unsigned int GLenum;
typedef unsigned char GLboolean;
typedef unsigned int GLbitfield;
typedef void GLvoid;
typedef int GLint;
typedef unsigned int GLuint;
typedef int GLsizei;
typedef double GLdouble;
typedef char GLchar;
typedef khronos_uint16_t GLhalf;
typedef khronos_int32_t GLfixed;
typedef khronos_intptr_t GLintptr;
typedef khronos_ssize_t GLsizeiptr;
typedef khronos_int64_t GLint64;
typedef khronos_uint64_t GLuint64;
typedef struct __GLsync *GLsync;

typedef void (GLAPIENTRY *GLDEBUGPROC)(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam);
typedef const GLubyte* (GLAPIENTRY *PFNGLGETSTRINGPROC)(GLenum name);
typedef void (GLAPIENTRY *PFNGLDEBUGMESSAGECALLBACKPROC)(GLDEBUGPROC callback, const void *userParam);
typedef void (GLAPIENTRY *PFNGLGETINTEGERVPROC)(GLenum pname, GLint* data);
typedef GLenum (GLAPIENTRY *PFNGLGETERRORPROC)(void);
typedef void (GLAPIENTRY *PFNGLENABLEPROC)(GLenum cap);
typedef void (GLAPIENTRY *PFNGLENABLEIPROC)(GLenum target, GLuint index);
typedef void (GLAPIENTRY *PFNGLDISABLEPROC)(GLenum cap);
typedef void (GLAPIENTRY *PFNGLDISABLEIPROC)(GLenum target, GLuint index);
typedef void (GLAPIENTRY *PFNGLCREATEBUFFERSPROC)(GLsizei n, GLuint* buffers);
typedef void (GLAPIENTRY *PFNGLDELETEBUFFERSPROC)(GLsizei n, const GLuint* buffers);
typedef void (GLAPIENTRY *PFNGLCREATESAMPLERSPROC)(GLsizei n, GLuint *samplers);
typedef void (GLAPIENTRY *PFNGLDELETESAMPLERSPROC)(GLsizei count, const GLuint *samplers);
typedef void (GLAPIENTRY *PFNGLCREATETEXTURESPROC)(GLenum target, GLsizei n, GLuint* textures);
typedef void (GLAPIENTRY *PFNGLDELETETEXTURESPROC)(GLsizei n, const GLuint* textures);
typedef void (GLAPIENTRY *PFNGLCREATEFRAMEBUFFERSPROC)(GLsizei n, GLuint* framebuffers);
typedef void (GLAPIENTRY *PFNGLDELETEFRAMEBUFFERSPROC)(GLsizei n, const GLuint* framebuffers);
typedef void (GLAPIENTRY *PFNGLCREATEVERTEXARRAYSPROC)(GLsizei n, GLuint *arrays);
typedef void (GLAPIENTRY *PFNGLDELETEVERTEXARRAYSPROC)(GLsizei n, const GLuint *arrays);
typedef GLuint (GLAPIENTRY *PFNGLCREATESHADERPROC)(GLenum type);
typedef void (GLAPIENTRY *PFNGLDELETESHADERPROC)(GLuint shader);
typedef GLuint (GLAPIENTRY *PFNGLCREATEPROGRAMPROC)(void);
typedef void (GLAPIENTRY *PFNGLDELETEPROGRAMPROC)(GLuint program);
typedef GLsync (GLAPIENTRY *PFNGLFENCESYNCPROC)(GLenum condition, GLbitfield flags);
typedef void (GLAPIENTRY *PFNGLDELETESYNCPROC)(GLsync sync);
typedef GLenum (GLAPIENTRY *PFNGLCLIENTWAITSYNCPROC)(GLsync sync, GLbitfield flags, GLuint64 timeout);
typedef void (GLAPIENTRY *PFNGLWAITSYNCPROC)(GLsync sync, GLbitfield flags, GLuint64 timeout);
typedef GLenum (GLAPIENTRY *PFNGLCHECKNAMEDFRAMEBUFFERSTATUSPROC)(GLuint framebuffer, GLenum target);
typedef void (GLAPIENTRY *PFNGLNAMEDFRAMEBUFFERTEXTUREPROC)(GLuint framebuffer, GLenum attachment, GLuint texture, GLint level);
typedef void (GLAPIENTRY *PFNGLNAMEDFRAMEBUFFERDRAWBUFFERPROC)(GLuint framebuffer, GLenum buf);
typedef void (GLAPIENTRY *PFNGLNAMEDFRAMEBUFFERDRAWBUFFERSPROC)(GLuint framebuffer, GLsizei n, const GLenum *bufs);
typedef void (GLAPIENTRY *PFNGLSAMPLERPARAMETERIPROC)(GLuint sampler, GLenum pname, GLint param);
typedef void (GLAPIENTRY *PFNGLTEXTUREPARAMETERIPROC)(GLuint texture, GLenum pname, GLint param);
typedef void (GLAPIENTRY *PFNGLTEXTURESTORAGE2DPROC)(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);
typedef void (GLAPIENTRY *PFNGLTEXTURESTORAGE2DMULTISAMPLEPROC)(GLuint texture, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
typedef void (GLAPIENTRY *PFNGLTEXTURESUBIMAGE2DPROC)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels);
typedef void (GLAPIENTRY *PFNGLSHADERSOURCEPROC)(GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length);
typedef void (GLAPIENTRY *PFNGLCOMPILESHADERPROC)(GLuint shader);
typedef void (GLAPIENTRY *PFNGLGETSHADERIVPROC)(GLuint shader, GLenum pname, GLint *params);
typedef void (GLAPIENTRY *PFNGLATTACHSHADERPROC)(GLuint program, GLuint shader);
typedef void (GLAPIENTRY *PFNGLLINKPROGRAMPROC)(GLuint program);
typedef void (GLAPIENTRY *PFNGLGETPROGRAMIVPROC)(GLuint program, GLenum pname, GLint *params);
typedef void (GLAPIENTRY *PFNGLGETPROGRAMINFOLOGPROC)(GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef void (GLAPIENTRY *PFNGLGETSHADERINFOLOGPROC)(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef void (GLAPIENTRY *PFNGLBLENDFUNCSEPARATEIPROC)(GLuint buf, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);
typedef void (GLAPIENTRY *PFNGLBLENDEQUATIONSEPARATEIPROC)(GLuint buf, GLenum modeRGB, GLenum modeAlpha);
typedef void (GLAPIENTRY *PFNGLCOLORMASKIPROC)(GLuint index, GLboolean r, GLboolean g, GLboolean b, GLboolean a);
typedef void (GLAPIENTRY *PFNGLPOLYGONMODEPROC)(GLenum face, GLenum mode);
typedef void (GLAPIENTRY *PFNGLNAMEDBUFFERSTORAGEPROC)(GLuint buffer, GLsizeiptr size, const void *data, GLbitfield flags);
typedef void (GLAPIENTRY *PFNGLNAMEDBUFFERDATAPROC)(GLuint buffer, GLsizeiptr size, const void *data, GLenum usage);
typedef void* (GLAPIENTRY *PFNGLMAPNAMEDBUFFERRANGEPROC)(GLuint buffer, GLintptr offset, GLsizeiptr length, GLbitfield access);
typedef GLboolean (GLAPIENTRY *PFNGLUNMAPNAMEDBUFFERPROC)(GLuint buffer);
typedef void (GLAPIENTRY *PFNGLUSEPROGRAMPROC)(GLuint program);
typedef void (GLAPIENTRY *PFNGLBINDBUFFERPROC)(GLenum target, GLuint buffer);
typedef void (GLAPIENTRY *PFNGLBINDVERTEXARRAYPROC)(GLuint array);
typedef void (GLAPIENTRY *PFNGLDRAWARRAYSINSTANCEDPROC)(GLenum mode, GLint first, GLsizei count, GLsizei instancecount);
typedef void (GLAPIENTRY *PFNGLDRAWELEMENTSINSTANCEDPROC)(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount);


typedef struct LvnOglSwapchainData
{
    GLuint*           images;
    uint32_t          imageCount;
    uint32_t          currImage;
    uint32_t          width;
    uint32_t          height;
    LvnFormat         format;
    LvnPresentMode    presentMode;
} LvnOglSwapchainData;

typedef struct LvnOglRenderpassData
{
    LvnColorAttachment*          colorAttachments;
    LvnResolveAttachment*        resolveAttachments;
    uint32_t                     colorAttachmentCount;
    LvnDepthStencilAttachment    depthStencilAttachment;
    uint32_t*                    hasResolves;
    bool                         hasDepth;
} LvnOglRenderpassData;

typedef struct LvnOglFramebufferData
{
    LvnOglRenderpassData    renderpassData;
    uint32_t                fboId;
    uint32_t                resolveId;
    uint32_t                width;
    uint32_t                height;
    bool                    multisample;
} LvnOglFramebufferData;

typedef struct LvnOglColorBlendAttachment
{
    GLenum    srcRGB;
    GLenum    dstRGB;
    GLenum    srcAlpha;
    GLenum    dstAlpha;
    GLenum    modeRGB;
    GLenum    modeAlpha;
    bool      writeMaskR;
    bool      writeMaskG;
    bool      writeMaskB;
    bool      writeMaskA;
    bool      blendEnable;
} LvnOglColorBlendAttachment;

typedef struct LvnOglPipelineData
{
    uint32_t pipelineId;

    struct
    {
        LvnPipelineInputAssembly       inputAssembly;
        LvnPipelineRasterizer          rasterizer;
        LvnPipelineMultiSampling       multisampling;
        LvnPipelineColorBlend          colorBlend;
        LvnPipelineDepthStencil        depthStencil;
        LvnOglColorBlendAttachment*    pColorBlendAttachments;
        uint32_t                       colorBlendAttachmentCount;

        GLenum                         primitiveMode;
        GLenum                         cullMode;
        GLenum                         frontFace;
        GLenum                         srcBlendFactor;
        GLenum                         dstBlendFactor;
        GLenum                         depthCompareOp;
        GLenum                         stencilCompareOp;
        GLenum                         stencilFailOp;
        GLenum                         stencilPassOp;
        GLenum                         stencilDepthFailOp;
    } fixedFuncEnums;
} LvnOglPipelineData;

typedef struct LvnOglShaderData
{
    uint32_t          shaderId;
    LvnShaderStage    stage;
} LvnOglShaderData;

typedef struct LvnOglBufferData
{
    uint32_t    bufferId;
    void*       bufferMap;
} LvnOglBufferData;

typedef struct LvnOglFenceData
{
    GLsync    fenceId;
    bool      pending;
} LvnOglFenceData;

typedef struct LvnOpenglBackends
{
    const LvnGraphicsContext*               graphicsctx;
    GLint                                   versionMajor, versionMinor;
    GLint                                   maxColorAttachments;
    GLint                                   maxDrawBuffers;

    void*                                   loaderHandle;
    void*                                   handle;

    LvnResult                               (*ogllCreateSurface)(const struct LvnOpenglBackends*, LvnSurface*, const LvnSurfaceCreateInfo*);
    void                                    (*ogllDestroySurface)(const struct LvnOpenglBackends*, LvnSurface*);
    void                                    (*ogllMakeCurrent)(const struct LvnOpenglBackends*, LvnSurface*);
    void                                    (*ogllSwapBuffers)(const struct LvnOpenglBackends*, LvnSurface*);

    PFNGLGETSTRINGPROC                      glGetString;
    PFNGLGETERRORPROC                       glGetError;
    PFNGLDEBUGMESSAGECALLBACKPROC           glDebugMessageCallback;
    PFNGLGETINTEGERVPROC                    glGetIntegerv;
    PFNGLENABLEPROC                         glEnable;
    PFNGLENABLEIPROC                        glEnablei;
    PFNGLDISABLEPROC                        glDisable;
    PFNGLDISABLEIPROC                       glDisablei;
    PFNGLCREATEBUFFERSPROC                  glCreateBuffers;
    PFNGLDELETEBUFFERSPROC                  glDeleteBuffers;
    PFNGLCREATESAMPLERSPROC                 glCreateSamplers;
    PFNGLDELETESAMPLERSPROC                 glDeleteSamplers;
    PFNGLCREATETEXTURESPROC                 glCreateTextures;
    PFNGLDELETETEXTURESPROC                 glDeleteTextures;
    PFNGLCREATEFRAMEBUFFERSPROC             glCreateFramebuffers;
    PFNGLDELETEFRAMEBUFFERSPROC             glDeleteFramebuffers;
    PFNGLCREATEVERTEXARRAYSPROC             glCreateVertexArrays;
    PFNGLDELETEVERTEXARRAYSPROC             glDeleteVertexArrays;
    PFNGLCREATESHADERPROC                   glCreateShader;
    PFNGLDELETESHADERPROC                   glDeleteShader;
    PFNGLCREATEPROGRAMPROC                  glCreateProgram;
    PFNGLDELETEPROGRAMPROC                  glDeleteProgram;
    PFNGLFENCESYNCPROC                      glFenceSync;
    PFNGLDELETESYNCPROC                     glDeleteSync;
    PFNGLCLIENTWAITSYNCPROC                 glClientWaitSync;
    PFNGLWAITSYNCPROC                       glWaitSync;
    PFNGLCHECKNAMEDFRAMEBUFFERSTATUSPROC    glCheckNamedFramebufferStatus;
    PFNGLNAMEDFRAMEBUFFERTEXTUREPROC        glNamedFramebufferTexture;
    PFNGLNAMEDFRAMEBUFFERDRAWBUFFERPROC     glNamedFramebufferDrawBuffer;
    PFNGLNAMEDFRAMEBUFFERDRAWBUFFERSPROC    glNamedFramebufferDrawBuffers;
    PFNGLSAMPLERPARAMETERIPROC              glSamplerParameteri;
    PFNGLTEXTUREPARAMETERIPROC              glTextureParameteri;
    PFNGLTEXTURESTORAGE2DPROC               glTextureStorage2D;
    PFNGLTEXTURESTORAGE2DMULTISAMPLEPROC    glTextureStorage2DMultisample;
    PFNGLTEXTURESUBIMAGE2DPROC              glTextureSubImage2D;
    PFNGLSHADERSOURCEPROC                   glShaderSource;
    PFNGLCOMPILESHADERPROC                  glCompileShader;
    PFNGLGETSHADERIVPROC                    glGetShaderiv;
    PFNGLATTACHSHADERPROC                   glAttachShader;
    PFNGLLINKPROGRAMPROC                    glLinkProgram;
    PFNGLGETPROGRAMIVPROC                   glGetProgramiv;
    PFNGLGETPROGRAMINFOLOGPROC              glGetProgramInfoLog;
    PFNGLGETSHADERINFOLOGPROC               glGetShaderInfoLog;
    PFNGLBLENDFUNCSEPARATEIPROC             glBlendFuncSeparatei;
    PFNGLBLENDEQUATIONSEPARATEIPROC         glBlendEquationSeparatei;
    PFNGLCOLORMASKIPROC                     glColorMaski;
    PFNGLPOLYGONMODEPROC                    glPolygonMode;
    PFNGLNAMEDBUFFERSTORAGEPROC             glNamedBufferStorage;
    PFNGLNAMEDBUFFERDATAPROC                glNamedBufferData;
    PFNGLMAPNAMEDBUFFERRANGEPROC            glMapNamedBufferRange;
    PFNGLUNMAPNAMEDBUFFERPROC               glUnmapNamedBuffer;
    PFNGLUSEPROGRAMPROC                     glUseProgram;
    PFNGLBINDBUFFERPROC                     glBindBuffer;
    PFNGLBINDVERTEXARRAYPROC                glBindVertexArray;
    PFNGLDRAWARRAYSINSTANCEDPROC            glDrawArraysInstanced;
    PFNGLDRAWELEMENTSINSTANCEDPROC          glDrawElementsInstanced;
} LvnOpenglBackends;

LvnResult lvnImplOglInit(LvnGraphicsContext* graphicsctx, const LvnGraphicsContextCreateInfo* createInfo);
void      lvnImplOglTerminate(LvnGraphicsContext* graphicsctx);

LvnResult lvnImplOglCreateSurface(const LvnGraphicsContext* graphicsctx, LvnSurface* surface, const LvnSurfaceCreateInfo* createInfo);
void      lvnImplOglDestroySurface(LvnSurface* surface);
LvnResult lvnImplOglCreateSwapchain(const LvnGraphicsContext* graphicsctx, LvnSwapchain* swapchain, const LvnSwapchainCreateInfo* createInfo);
void      lvnImplOglDestroySwapchain(LvnSwapchain* swapchain);
LvnResult lvnImplOglCreateRenderPass(const LvnGraphicsContext* graphicsctx, LvnRenderPass* renderpass, const LvnRenderPassCreateInfo* createInfo);
void      lvnImplOglDestroyRenderPass(LvnRenderPass* renderpass);
LvnResult lvnImplOglCreateFramebuffer(const LvnGraphicsContext* graphicsctx, LvnFramebuffer* framebuffer, const LvnFramebufferCreateInfo* createInfo);
void      lvnImplOglDestroyFramebuffer(LvnFramebuffer* framebuffer);
LvnResult lvnImplOglCreateShader(const LvnGraphicsContext* graphicsctx, LvnShader* shader, const LvnShaderCreateInfo* createInfo);
void      lvnImplOglDestroyShader(LvnShader* shader);
LvnResult lvnImplOglCreatePipeline(const LvnGraphicsContext* graphicsctx, LvnPipeline* pipeline, const LvnPipelineCreateInfo* createInfo);
void      lvnImplOglDestroyPipeline(LvnPipeline* pipeline);
LvnResult lvnImplOglCreateFence(const LvnGraphicsContext* graphicsctx, LvnFence* fence, bool signaled);
void      lvnImplOglDestroyFence(LvnFence* fence);
LvnResult lvnImplOglCreateSemaphore(const LvnGraphicsContext* graphicsctx, LvnSemaphore* semaphore);
void      lvnImplOglDestroySemaphore(LvnSemaphore* semaphore);
LvnResult lvnImplOglsCreateBuffer(const LvnGraphicsContext* graphicsctx, LvnBuffer* buffer, const LvnBufferCreateInfo* createInfo);
void      lvnImplOglsDestroyBuffer(LvnBuffer* buffer);
LvnResult lvnImplOglsCreateSampler(const LvnGraphicsContext* graphicsctx, LvnSampler* sampler, const LvnSamplerCreateInfo* createInfo);
void      lvnImplOglsDestroySampler(LvnSampler* sampler);
LvnResult lvnImplOglsCreateTexture(const LvnGraphicsContext* graphicsctx, LvnTexture* texture, const LvnTextureCreateInfo* createInfo);
void      lvnImplOglsDestroyTexture(LvnTexture* texture);
LvnResult lvnImplOglAllocateCommandBuffers(const LvnGraphicsContext* graphicsctx, const LvnCommandBufferAllocInfo* allocInfo, LvnCommandBuffer** pCommandBuffers);

void      lvnImplOglSurfaceGetSupportedFormats(const LvnSurface* surface, uint32_t* formatCount, LvnFormat* pSurfaceFormats);
void      lvnImplOglSurfaceGetSupportedPresentModes(const LvnSurface* surface, uint32_t* presentModeCount, LvnPresentMode* pPresentModes);

LvnResult lvnImplOglSwapchainResize(LvnSwapchain* swapchain, uint32_t width, uint32_t height);
LvnResult lvnImplOglSwapchainAcquireNextImage(LvnSwapchain* swapchain, LvnSemaphore* semaphore, LvnFence* fence, uint32_t* imageIndex);

LvnResult lvnImplOglFenceWait(LvnFence* fence, uint64_t timeout);
LvnResult lvnImplOglFenceReset(LvnFence* fence);

void      lvnImplOglBufferUpdateData(LvnBuffer* buffer, void* data, uint64_t size, uint64_t offset);
void      lvnImplOglBufferResize(LvnBuffer* buffer, uint64_t size);

void      lvnImplOglBeginCommandBuffer(LvnCommandBuffer* commandBuffer);
void      lvnImplOglEndCommandBuffer(LvnCommandBuffer* commandBuffer);
void      lvnImplOglCmdBeginRenderPass(LvnCommandBuffer* commandBuffer, LvnRenderPassBeginInfo* beginInfo);
void      lvnImplOglCmdEndRenderPass(LvnCommandBuffer* commandBuffer);
void      lvnImplOglCmdBindPipeline(LvnCommandBuffer* commandBuffer, LvnPipeline* pipeline);
void      lvnImplOglCmdBindVertexBuffer(LvnCommandBuffer* commandBuffer, uint32_t firstBinding, uint32_t bindingCount, LvnBuffer** pBuffers, uint64_t* pOffsets);
void      lvnImplOglCmdBindIndexBuffer(LvnCommandBuffer* commandBuffer, LvnBuffer* buffer, uint64_t offset);
void      lvnImplOglCmdSetViewport(LvnCommandBuffer* commandBuffer, const LvnViewport* viewport);
void      lvnImplOglCmdSetScissor(LvnCommandBuffer* commandBuffer, const LvnRenderArea* scissor);
void      lvnImplOglCmdDraw(LvnCommandBuffer* commandBuffer, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance);
void      lvnImplOglCmdDrawIndexed(LvnCommandBuffer* commandBuffer, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance);
LvnResult lvnImplOglRenderSubmit(const LvnGraphicsContext* graphicsctx, const LvnSubmitInfo* pSubmits, uint32_t submitCount, LvnFence* fence);
LvnResult lvnImplOglRenderPresent(const LvnGraphicsContext* graphicsctx, const LvnPresentInfo* presentInfo);

#endif // !HG_LVN_IMPL_OGL_H
