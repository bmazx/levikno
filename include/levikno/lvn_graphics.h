#ifndef HG_LVN_GRAPHICS_H
#define HG_LVN_GRAPHICS_H

#include "lvn_config.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>


typedef enum LvnGraphicsApi
{
    Lvn_GraphicsApi_None = 0,
    Lvn_GraphicsApi_Opengl,
    Lvn_GraphicsApi_Vulkan,
} LvnGraphicsApi;

typedef struct LvnGraphicsContext LvnGraphicsContext;

struct LvnContext;


typedef struct LvnGraphicsContextCreateInfo
{
    LvnGraphicsApi graphicsapi;
} LvnGraphicsContextCreateInfo;


#ifdef __cplusplus
extern "C" {
#endif

LVN_API LvnResult               lvnCreateGraphicsContext(struct LvnContext* ctx, LvnGraphicsContext** graphicsctx, const LvnGraphicsContextCreateInfo* createInfo); // create the graphics context
LVN_API void                    lvnDestroyGraphicsContext(LvnGraphicsContext* graphicsctx); // destroy the graphics context



#ifdef __cplusplus
}
#endif


#endif // !HG_LVN_GRAPHICS_H
