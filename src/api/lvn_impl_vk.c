#include "lvn_impl_vk.h"

LvnResult lvnImplVkInit(LvnGraphicsContext* graphicsctx, const LvnGraphicsContextCreateInfo* createInfo)
{
    LVN_ASSERT(graphicsctx && createInfo, "graphicsctx and createInfo cannot be nullptr");

    return Lvn_Result_Success;
}

void lvnImplVkTerminate(LvnGraphicsContext* graphicsctx)
{

}
