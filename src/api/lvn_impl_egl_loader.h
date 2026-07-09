#ifndef HG_LVN_IMPL_EGL_LOADER_H
#define HG_LVN_IMPL_EGL_LOADER_H

#include "lvn_impl_ogl.h"

#include <EGL/egl.h>


typedef struct LvnEglLoader
{
    void* handle;

    PFNEGLGETPLATFORMDISPLAYPROC      eglGetPlatformDisplay;
    PFNEGLINITIALIZEPROC              eglInitialize;
    PFNEGLQUERYSTRINGPROC             eglQueryString;
    PFNEGLCHOOSECONFIGPROC            eglChooseConfig;
    PFNEGLCREATEPBUFFERSURFACEPROC    eglCreatePbufferSurface;
    PFNEGLCREATEWINDOWSURFACEPROC     eglCreateWindowSurface;
    PFNEGLCREATECONTEXTPROC           eglCreateContext;
    PFNEGLMAKECURRENTPROC             eglMakeCurrent;
    PFNEGLSWAPBUFFERSPROC             eglSwapBuffers;
    PFNEGLSWAPINTERVALPROC            eglSwapInterval;
    PFNEGLBINDAPIPROC                 eglBindAPI;
    PFNEGLGETPROCADDRESSPROC          eglGetProcAddress;
    PFNEGLDESTROYSURFACEPROC          eglDestroySurface;
    PFNEGLDESTROYCONTEXTPROC          eglDestroyContext;
    PFNEGLTERMINATEPROC               eglTerminate;

    EGLint                            versionMinor, versionMajor;
    EGLDisplay                        display;
    EGLConfig                         config;
    EGLSurface                        surface;
    EGLContext                        context;

    struct
    {
        bool colorspace;
    } ext;
} LvnEglLoader;


LvnResult lvnEglLoaderInit(LvnOpenglBackends* oglBackends, void* display);
void      lvnEglLoaderTerminate(LvnOpenglBackends* oglBackends);
LvnResult lvnEglCreateSurface(const LvnOpenglBackends* oglBackends, LvnSurface* surface, const LvnSurfaceCreateInfo* createInfo);
void      lvnEglDestroySurface(const LvnOpenglBackends* oglBackends, LvnSurface* surface);
void      lvnEglMakeCurrent(const LvnOpenglBackends* oglBackends, LvnSurface* surface);
void      lvnEglSwapBuffers(const LvnOpenglBackends* oglBackends, LvnSurface* surface);
void      lvnEglSwapInterval(const LvnOpenglBackends* oglBackends, int interval);


#endif
