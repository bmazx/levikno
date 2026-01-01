#ifndef HG_LVN_IMPL_VK_H
#define HG_LVN_IMPL_VK_H

#include "lvn_graphics_internal.h"
#include "levikno_internal.h"

LvnResult lvnImplVkInit(LvnGraphicsContext* graphicsctx, const LvnGraphicsContextCreateInfo* createInfo);
void      lvnImplVkTerminate(LvnGraphicsContext* graphicsctx);

LvnResult lvnImplVkCreateSurface(const LvnGraphicsContext* graphicsctx, LvnSurface* surface, const LvnSurfaceCreateInfo* createInfo);
void      lvnImplVkDestroySurface(LvnSurface* surface);
LvnResult lvnImplVkCreateShader(const LvnGraphicsContext* graphicsctx, LvnShader* shader, const LvnShaderCreateInfo* createInfo);
void      lvnImplVkDestroyShader(LvnShader* shader);
LvnResult lvnImplVkCreatePipeline(const LvnGraphicsContext* graphicsctx, LvnPipeline* pipeline, const LvnPipelineCreateInfo* createInfo);
void      lvnImplVkDestroyPipeline(LvnPipeline* pipeline);
LvnResult lvnImplVkCreateCommandBuffer(const LvnGraphicsContext* graphicsctx, LvnCommandBuffer* commandBuffer, const LvnCommandBufferCreateInfo* createInfo);
void      lvnImplVkDestroyCommandBuffer(LvnCommandBuffer* commandBuffer);
LvnResult lvnImplVkCreateFence(const LvnGraphicsContext* graphicsctx, LvnFence* fence);
void      lvnImplVkDestroyFence(LvnFence* fence);
LvnResult lvnImplVkCreateSemaphore(const LvnGraphicsContext* graphicsctx, LvnSemaphore* semaphore);
void      lvnImplVkDestroySemaphore(LvnSemaphore* semaphore);

void      lvnImplVkBeginCommandBuffer(LvnCommandBuffer* commandBuffer);
void      lvnImplVkEndCommandBuffer(LvnCommandBuffer* commandBuffer);
void      lvnImplVkCmdBeginRendering(LvnCommandBuffer* commandBuffer, const LvnRenderingInfo* renderInfo);
void      lvnImplVkCmdEndRendering(LvnCommandBuffer* commandBuffer);
LvnResult lvnImplVkSurfaceAcquireNextImage(LvnSurface* surface, LvnSemaphore* semaphore, LvnFence* fence, uint32_t* imageIndex);

#endif // !HG_LVN_IMPL_VK_H
