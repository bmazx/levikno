#ifndef HG_LVN_IMPL_EGL_LOADER_H
#define HG_LVN_IMPL_EGL_LOADER_H

#include "lvn_impl_ogl.h"

#include <EGL/egl.h>


typedef struct LvnEglLoader
{
    void* handle;

    PFNEGLGETPLATFORMDISPLAYPROC eglGetPlatformDisplay;
    PFNEGLINITIALIZEPROC eglInitialize;
    PFNEGLCHOOSECONFIGPROC eglChooseConfig;
    PFNEGLCREATEWINDOWSURFACEPROC eglCreateWindowSurface;
    PFNEGLCREATECONTEXTPROC eglCreateContext;
    PFNEGLMAKECURRENTPROC eglMakeCurrent;
    PFNEGLSWAPBUFFERSPROC eglSwapBuffers;
    PFNEGLBINDAPIPROC eglBindAPI;
    PFNEGLGETPROCADDRESSPROC eglGetProcAddress;
} LvnEglLoader;


LvnResult lvnEglLoaderInit(LvnOpenglBackends* oglBackends, void* display, void* window, uint32_t width, uint32_t height);
void      lvnEglLoaderTerminate(LvnOpenglBackends* oglBackends);


#endif
