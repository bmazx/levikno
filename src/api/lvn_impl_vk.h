#ifndef HG_LVN_IMPL_VK_H
#define HG_LVN_IMPL_VK_H

#include "lvn_graphics_internal.h"
#include "levikno_internal.h"

LvnResult lvnImplVkInit(LvnGraphicsContext* graphicsctx, const LvnGraphicsContextCreateInfo* createInfo);
void      lvnImplVkTerminate(LvnGraphicsContext* graphicsctx);

LvnResult lvnImplVkCreateSurface(const LvnGraphicsContext* graphicsctx, LvnSurface* surface, const LvnSurfaceCreateInfo* createInfo);
void      lvnImplVkDestroySurface(LvnSurface* surface);
LvnResult lvnImplVkCreateSwapchain(const LvnGraphicsContext* graphicsctx, LvnSwapchain* swapchain, const LvnSwapchainCreateInfo* createInfo);
void      lvnImplVkDestroySwapchain(LvnSwapchain* swapchain);
LvnResult lvnImplVkCreateRenderPass(const LvnGraphicsContext* graphicsctx, LvnRenderPass* renderpass, const LvnRenderPassCreateInfo* createInfo);
void      lvnImplVkDestroyRenderPass(LvnRenderPass* renderpass);
LvnResult lvnImplVkCreateFramebuffer(const LvnGraphicsContext* graphicsctx, LvnFramebuffer* framebuffer, const LvnFramebufferCreateInfo* createInfo);
void      lvnImplVkDestroyFramebuffer(LvnFramebuffer* framebuffer);
LvnResult lvnImplVkCreateShader(const LvnGraphicsContext* graphicsctx, LvnShader* shader, const LvnShaderCreateInfo* createInfo);
void      lvnImplVkDestroyShader(LvnShader* shader);
LvnResult lvnImplVkCreatePipeline(const LvnGraphicsContext* graphicsctx, LvnPipeline* pipeline, const LvnPipelineCreateInfo* createInfo);
void      lvnImplVkDestroyPipeline(LvnPipeline* pipeline);
LvnResult lvnImplVkCreateFence(const LvnGraphicsContext* graphicsctx, LvnFence* fence);
void      lvnImplVkDestroyFence(LvnFence* fence);
LvnResult lvnImplVkCreateSemaphore(const LvnGraphicsContext* graphicsctx, LvnSemaphore* semaphore);
void      lvnImplVkDestroySemaphore(LvnSemaphore* semaphore);
LvnResult lvnImplVksCreateBuffer(const LvnGraphicsContext* graphicsctx, LvnBuffer* buffer, const LvnBufferCreateInfo* createInfo);
void      lvnImplVksDestroyBuffer(LvnBuffer* buffer);
LvnResult lvnImplVksCreateSampler(const LvnGraphicsContext* graphicsctx, LvnSampler* sampler, const LvnSamplerCreateInfo* createInfo);
void      lvnImplVksDestroySampler(LvnSampler* sampler);
LvnResult lvnImplVksCreateTexture(const LvnGraphicsContext* graphicsctx, LvnTexture* texture, const LvnTextureCreateInfo* createInfo);
void      lvnImplVksDestroyTexture(LvnTexture* texture);
LvnResult lvnImplVkAllocateCommandBuffers(const LvnGraphicsContext* graphicsctx, const LvnCommandBufferAllocInfo* allocInfo, LvnCommandBuffer** pCommandBuffers);

LvnResult lvnImplVkSwapchainResize(LvnSwapchain* swapchain, uint32_t width, uint32_t height);
LvnResult lvnImplVkSwapchainAcquireNextImage(LvnSwapchain* swapchain, LvnSemaphore* semaphore, LvnFence* fence, uint32_t* imageIndex);

LvnResult lvnImplVkFenceWait(LvnFence* fence, uint64_t timeout);
LvnResult lvnImplVkFenceReset(LvnFence* fence);

void      lvnImplVkBufferUpdateData(LvnBuffer* buffer, void* data, uint64_t size, uint64_t offset);
void      lvnImplVkBufferResize(LvnBuffer* buffer, uint64_t size);

void      lvnImplVkBeginCommandBuffer(LvnCommandBuffer* commandBuffer);
void      lvnImplVkEndCommandBuffer(LvnCommandBuffer* commandBuffer);
void      lvnImplVkCmdBeginRenderPass(LvnCommandBuffer* commandBuffer, LvnRenderPassBeginInfo* beginInfo);
void      lvnImplVkCmdEndRenderPass(LvnCommandBuffer* commandBuffer);
void      lvnImplVkCmdBindPipeline(LvnCommandBuffer* commandBuffer, LvnPipeline* pipeline);
void      lvnImplVkCmdBindVertexBuffer(LvnCommandBuffer* commandBuffer, uint32_t firstBinding, uint32_t bindingCount, LvnBuffer** pBuffers, uint64_t* pOffsets);
void      lvnImplVkCmdBindIndexBuffer(LvnCommandBuffer* commandBuffer, LvnBuffer* buffer, uint64_t offset);
void      lvnImplVkCmdSetViewport(LvnCommandBuffer* commandBuffer, const LvnViewport* viewport);
void      lvnImplVkCmdSetScissor(LvnCommandBuffer* commandBuffer, const LvnRenderArea* scissor);
void      lvnImplVkCmdDraw(LvnCommandBuffer* commandBuffer, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance);
void      lvnImplVkCmdDrawIndexed(LvnCommandBuffer* commandBuffer, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance);
LvnResult lvnImplVkRenderSubmit(const LvnGraphicsContext* graphicsctx, const LvnSubmitInfo* pSubmits, uint32_t submitCount, LvnFence* fence);
LvnResult lvnImplVkRenderPresent(const LvnGraphicsContext* graphicsctx, const LvnPresentInfo* presentInfo);

#endif // !HG_LVN_IMPL_VK_H
