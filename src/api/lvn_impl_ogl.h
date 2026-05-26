#ifndef HG_LVN_IMPL_OGL_H
#define HG_LVN_IMPL_OGL_H

#include "lvn_graphics_internal.h"
#include "levikno_internal.h"

#include <KHR/khrplatform.h>

#define GLAPI KHRONOS_APICALL
#define GLAPIENTRY KHRONOS_APIENTRY

#define GL_FALSE 0
#define GL_TRUE 1
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
typedef void (GLAPIENTRY *GLDEBUGPROC)(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam);

typedef const GLubyte* (GLAPIENTRY *PFNGLGETSTRINGPROC)(GLenum name);
typedef void (GLAPIENTRY *PFNGLDEBUGMESSAGECALLBACKPROC)(GLDEBUGPROC callback, const void *userParam);
typedef void (GLAPIENTRY *PFNGLGETINTEGERVPROC)(GLenum pname, GLint* data);
typedef GLenum (GLAPIENTRY *PFNGLGETERRORPROC)(void);
typedef void (GLAPIENTRY *PFNGLDISABLEPROC)(GLenum cap);
typedef void (GLAPIENTRY *PFNGLENABLEPROC)(GLenum cap);
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
typedef GLenum (GLAPIENTRY *PFNGLCHECKNAMEDFRAMEBUFFERSTATUSPROC)(GLuint framebuffer, GLenum target);
typedef void (GLAPIENTRY *PFNGLNAMEDFRAMEBUFFERTEXTUREPROC)(GLuint framebuffer, GLenum attachment, GLuint texture, GLint level);
typedef void (GLAPIENTRY *PFNGLNAMEDFRAMEBUFFERDRAWBUFFERPROC)(GLuint framebuffer, GLenum buf);
typedef void (GLAPIENTRY *PFNGLNAMEDFRAMEBUFFERDRAWBUFFERSPROC)(GLuint framebuffer, GLsizei n, const GLenum *bufs);
typedef void (GLAPIENTRY *PFNGLSAMPLERPARAMETERIPROC)(GLuint sampler, GLenum pname, GLint param);
typedef void (GLAPIENTRY *PFNGLTEXTUREPARAMETERIPROC)(GLuint texture, GLenum pname, GLint param);
typedef void (GLAPIENTRY *PFNGLTEXTURESTORAGE2DPROC)(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);
typedef void (GLAPIENTRY *PFNGLTEXTURESTORAGE2DMULTISAMPLEPROC)(GLuint texture, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
typedef void (GLAPIENTRY *PFNGLTEXTURESUBIMAGE2DPROC)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels);

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
    PFNGLDISABLEPROC                        glDisable;
    PFNGLENABLEPROC                         glEnable;
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
    PFNGLCHECKNAMEDFRAMEBUFFERSTATUSPROC    glCheckNamedFramebufferStatus;
    PFNGLNAMEDFRAMEBUFFERTEXTUREPROC        glNamedFramebufferTexture;
    PFNGLNAMEDFRAMEBUFFERDRAWBUFFERPROC     glNamedFramebufferDrawBuffer;
    PFNGLNAMEDFRAMEBUFFERDRAWBUFFERSPROC    glNamedFramebufferDrawBuffers;
    PFNGLSAMPLERPARAMETERIPROC              glSamplerParameteri;
    PFNGLTEXTUREPARAMETERIPROC              glTextureParameteri;
    PFNGLTEXTURESTORAGE2DPROC               glTextureStorage2D;
    PFNGLTEXTURESTORAGE2DMULTISAMPLEPROC    glTextureStorage2DMultisample;
    PFNGLTEXTURESUBIMAGE2DPROC              glTextureSubImage2D;
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
LvnResult lvnImplOglCreateFence(const LvnGraphicsContext* graphicsctx, LvnFence* fence);
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
