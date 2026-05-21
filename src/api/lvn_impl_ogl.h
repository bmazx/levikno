#ifndef HG_LVN_IMPL_OGL_H
#define HG_LVN_IMPL_OGL_H

#include "lvn_graphics_internal.h"
#include "levikno_internal.h"

#include <KHR/khrplatform.h>

#define GLAPI KHRONOS_APICALL
#define GLAPIENTRY KHRONOS_APIENTRY

#define GL_MAJOR_VERSION 0x821B
#define GL_MINOR_VERSION 0x821C
#define GL_NO_ERROR 0
#define GL_INVALID_ENUM 0x0500
#define GL_INVALID_VALUE 0x0501
#define GL_INVALID_OPERATION 0x0502
#define GL_STACK_OVERFLOW 0x0503
#define GL_STACK_UNDERFLOW 0x0504
#define GL_OUT_OF_MEMORY 0x0505
#define GL_INVALID_FRAMEBUFFER_OPERATION 0x0506

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

typedef const GLubyte* (GLAPIENTRY *PFNGLGETSTRINGPROC)(GLenum);
typedef void (GLAPIENTRY *PFNGLGETINTEGERVPROC)(GLenum, GLint*);
typedef GLenum (GLAPIENTRY *PFNGLGETERRORPROC)(void);
typedef void (GLAPIENTRY *PFNGLCREATEBUFFERSPROC)(GLsizei, GLuint*);
typedef void (GLAPIENTRY *PFNGLDELETEBUFFERSPROC)(GLsizei, const GLuint*);
typedef void (GLAPIENTRY *PFNGLCREATETEXTURESPROC)(GLenum, GLsizei, GLuint*);
typedef void (GLAPIENTRY *PFNGLDELETETEXTURESPROC)(GLsizei, const GLuint*);
typedef void (GLAPIENTRY *PFNGLCREATEFRAMEBUFFERSPROC)(GLsizei, GLuint*);
typedef void (GLAPIENTRY *PFNGLDELETEFRAMEBUFFERSPROC)(GLsizei, const GLuint*);

typedef struct LvnOglSwapchainData
{
    GLuint* images;
    uint32_t imageCount;
    uint32_t currImage;
    uint32_t width;
    uint32_t height;
    LvnFormat format;
    LvnPresentMode presentMode;
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

typedef struct LvnOpenglBackends
{
    const LvnGraphicsContext*      graphicsctx;
    GLint                          versionMajor, versionMinor;

    void*                          loaderHandle;
    void*                          handle;

    LvnResult                      (*ogllCreateSurface)(const struct LvnOpenglBackends*, LvnSurface*, const LvnSurfaceCreateInfo*);
    void                           (*ogllDestroySurface)(const struct LvnOpenglBackends*, LvnSurface*);
    void                           (*ogllMakeCurrent)(const struct LvnOpenglBackends*, LvnSurface*);
    void                           (*ogllSwapBuffers)(const struct LvnOpenglBackends*, LvnSurface*);

    PFNGLGETSTRINGPROC             glGetString;
    PFNGLGETERRORPROC              glGetError;
    PFNGLGETINTEGERVPROC           glGetIntegerv;
    PFNGLCREATEBUFFERSPROC         glCreateBuffers;
    PFNGLDELETEBUFFERSPROC         glDeleteBuffers;
    PFNGLCREATETEXTURESPROC        glCreateTextures;
    PFNGLDELETETEXTURESPROC        glDeleteTextures;
    PFNGLCREATEFRAMEBUFFERSPROC    glCreateFramebuffers;
    PFNGLDELETEFRAMEBUFFERSPROC    glDeleteFramebuffers;

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
