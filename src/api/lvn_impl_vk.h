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
LvnResult lvnImplVkCreatePipeline(const LvnGraphicsContext* graphicsctx, LvnPipeline** pipeline, const LvnPipelineCreateInfo* createInfo);
void      lvnImplVkDestroyPipeline(LvnPipeline* pipeline);

#endif // !HG_LVN_IMPL_VK_H
