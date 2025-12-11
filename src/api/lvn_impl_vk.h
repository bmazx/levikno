#ifndef HG_LVN_IMPL_VK_H
#define HG_LVN_IMPL_VK_H

#include "lvn_graphics_internal.h"
#include "levikno_internal.h"

LvnResult lvnImplVkInit(LvnGraphicsContext* graphicsctx, const LvnGraphicsContextCreateInfo* createInfo);
void      lvnImplVkTerminate(LvnGraphicsContext* graphicsctx);

#endif // !HG_LVN_IMPL_VK_H
