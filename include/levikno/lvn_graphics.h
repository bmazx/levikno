#ifndef HG_LVN_GRAPHICS_H
#define HG_LVN_GRAPHICS_H

#include "lvn_config.h"


typedef enum LvnGraphicsApi
{
    Lvn_GraphicsApi_None = 0,
    Lvn_GraphicsApi_Opengl,
    Lvn_GraphicsApi_Vulkan,
} LvnGraphicsApi;

typedef enum LvnPresentationModeFlagBits
{
    Lvn_PresentationModeFlag_Headless = 0x00000001,
    Lvn_PresentationModeFlag_Surface  = 0x00000002,
} LvnPresentationModeFlagBits;
typedef LvnFlags LvnPresentationModeFlags;


typedef struct LvnGraphicsContext LvnGraphicsContext;
typedef struct LvnSurface LvnSurface;

struct LvnContext;

typedef struct LvnSurfaceCreateInfo
{
    void* nativeDisplayHandle;
    void* nativeWindowHandle;
} LvnSurfaceCreateInfo;

typedef struct LvnGraphicsContextCreateInfo
{
    LvnGraphicsApi graphicsapi;                          // graphics api backend
    LvnPresentationModeFlags presentationModeFlags;      // type of output the graphics api will render to
    bool enableGraphicsApiDebugLogging;                  // enable logging for graphics api layer debug logs
} LvnGraphicsContextCreateInfo;


#ifdef __cplusplus
extern "C" {
#endif

LVN_API LvnResult               lvnCreateGraphicsContext(struct LvnContext* ctx, LvnGraphicsContext** graphicsctx, const LvnGraphicsContextCreateInfo* createInfo); // create the graphics context
LVN_API void                    lvnDestroyGraphicsContext(LvnGraphicsContext* graphicsctx); // destroy the graphics context

LVN_API LvnResult               lvnCreateSurface(const LvnGraphicsContext* graphicsctx, LvnSurface** surface, const LvnSurfaceCreateInfo* createInfo);
LVN_API void                    lvnDestroySurface(LvnSurface* surface);


#ifdef __cplusplus
}
#endif


#endif // !HG_LVN_GRAPHICS_H
