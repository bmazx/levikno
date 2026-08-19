#ifndef HG_LVN_IMPL_EGL_LOADER_H
#define HG_LVN_IMPL_EGL_LOADER_H

#include "lvn_impl_ogl.h"

#include <EGL/egl.h>

struct wl_egl_window;
struct wl_surface;

typedef struct wl_egl_window* (*PFN_wl_egl_window_create)(struct wl_surface *surface, int width, int height);
typedef void (*PFN_wl_egl_window_destroy)(struct wl_egl_window *egl_window);
typedef void (*PFN_wl_egl_window_resize)(struct wl_egl_window *egl_window, int width, int height, int dx, int dy);


typedef struct LvnEglSurfaceData
{
    EGLSurface               surface;
    struct wl_egl_window*    wlWindow;
} LvnEglSurfaceData;

typedef struct LvnEglLoader
{
    void*                             handle;

    struct
    {
        void*                         waylandEglHandle;
        PFN_wl_egl_window_create      wl_egl_window_create;
        PFN_wl_egl_window_destroy     wl_egl_window_destroy;
        PFN_wl_egl_window_resize      wl_egl_window_resize;
    } wl;

    PFNEGLGETPLATFORMDISPLAYPROC      eglGetPlatformDisplay;
    PFNEGLINITIALIZEPROC              eglInitialize;
    PFNEGLQUERYSTRINGPROC             eglQueryString;
    PFNEGLCHOOSECONFIGPROC            eglChooseConfig;
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
    LvnEglSurfaceData                 surfaceData;
    EGLContext                        context;

    struct
    {
        bool                          colorspace;
    } ext;
} LvnEglLoader;


LvnResult lvnEglLoaderInit(LvnOpenglBackends* oglBackends, const LvnPlatformData* platformData);
void      lvnEglLoaderTerminate(LvnOpenglBackends* oglBackends);
LvnResult lvnEglCreateSurface(const LvnOpenglBackends* oglBackends, LvnSurface* surface, const LvnSurfaceCreateInfo* createInfo);
void      lvnEglDestroySurface(const LvnOpenglBackends* oglBackends, LvnSurface* surface);
void      lvnEglSurfaceResize(const LvnOpenglBackends* oglBackends, const LvnSurface* surface, int width, int height);
void      lvnEglMakeCurrent(const LvnOpenglBackends* oglBackends, const LvnSurface* surface);
void      lvnEglSwapBuffers(const LvnOpenglBackends* oglBackends, const LvnSurface* surface);
void      lvnEglSwapInterval(const LvnOpenglBackends* oglBackends, int interval);


#endif
