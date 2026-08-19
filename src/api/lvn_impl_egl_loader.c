#include "lvn_impl_egl_loader.h"
#include "lvn_impl_ogl.h"

#include <string.h>

#include <EGL/egl.h>
#include <EGL/eglext.h>

#ifdef LVN_INCLUDE_WAYLAND
    #include <wayland-egl.h>
#endif

static const char* s_EglLibNames[] = { "libEGL.so.1", "libEGL.so" };
static const char* s_WaylandEglNames[] = { "libwayland-egl.so.1", "libwayland-egl.so" };

static LvnResult lvn_eglCreateSurface(const LvnGraphicsContext* graphicsctx, LvnEglLoader* eglLoader, LvnEglSurfaceData* surfaceData, EGLDisplay eglDisplay, EGLConfig config, void* nwh, uint32_t widht, uint32_t height);
static void lvn_eglDestroySurface(LvnEglLoader* eglLoader, LvnEglSurfaceData* surfaceData);

static LvnResult lvn_eglCreateSurface(
    const LvnGraphicsContext* graphicsctx,
    LvnEglLoader* eglLoader,
    LvnEglSurfaceData* surfaceData,
    EGLDisplay eglDisplay,
    EGLConfig config,
    void* nwh,
    uint32_t widht,
    uint32_t height)
{
    LVN_ASSERT(eglLoader && surfaceData && eglDisplay, "eglLoader, surface, and eglDisplay cannot be null");

    LvnResult result = Lvn_Result_Failure;
    LvnWindowPlatformSupport wps = lvn_getWindowPlatform();

    EGLint surfaceAttribs[] =
    {
        EGL_GL_COLORSPACE_KHR, EGL_GL_COLORSPACE_SRGB_KHR,
        EGL_NONE,
    };

#ifdef LVN_INCLUDE_WAYLAND
    if (wps.wayland)
    {
        surfaceData->wlWindow = eglLoader->wl.wl_egl_window_create(nwh, widht, height);
        if (!surfaceData->wlWindow)
        {
            LVN_LOG_ERROR(graphicsctx->coreLogger,
                          "[egl] failed to create wayland egl window for surface data %p",
                          surfaceData);
            goto fail_cleanup;
        }

        surfaceData->surface =
            eglLoader->eglCreateWindowSurface(eglDisplay, config, (EGLNativeWindowType)surfaceData->wlWindow, eglLoader->ext.colorspace ? surfaceAttribs : NULL);
        if (!surfaceData->surface)
        {
            LVN_LOG_ERROR(graphicsctx->coreLogger,
                          "[egl] failed to create egl surface for surface data %p",
                          surfaceData);
            goto fail_cleanup;
        }

        goto create_success;
    }
#endif
#ifdef LVN_INCLUDE_X11
    if (wps.x11)
    {
        surfaceData->surface =
            eglLoader->eglCreateWindowSurface(eglDisplay, config, (EGLNativeWindowType)nwh, eglLoader->ext.colorspace ? surfaceAttribs : NULL);
        if (!surfaceData->surface)
        {
            LVN_LOG_ERROR(graphicsctx->coreLogger,
                          "[egl] failed to create egl surface for surface data %p",
                          surfaceData);
            goto fail_cleanup;
        }

        goto create_success;
    }
#endif

create_success:
    return Lvn_Result_Success;
fail_cleanup:
    lvn_eglDestroySurface(eglLoader, surfaceData);
    return result;
}

static void lvn_eglDestroySurface(LvnEglLoader* eglLoader, LvnEglSurfaceData* surfaceData)
{
    LVN_ASSERT(eglLoader && surfaceData, "eglLoader and surfaceData cannot be null");

#ifdef LVN_INCLUDE_WAYLAND
    if (surfaceData->wlWindow)
    {
        eglLoader->wl.wl_egl_window_destroy(surfaceData->wlWindow);
        surfaceData->wlWindow = NULL;
    }
#endif

    if (surfaceData->surface)
    {
        eglLoader->eglDestroySurface(eglLoader->display, surfaceData->surface);
        surfaceData->surface = NULL;
    }
}

LvnResult lvnEglLoaderInit(LvnOpenglBackends* oglBackends, const LvnPlatformData* platformData)
{
    LVN_ASSERT(oglBackends && platformData, "oglBackends and platformData cannot be null");

    LvnEglLoader* eglLoader = (LvnEglLoader*) lvn_calloc(sizeof(LvnEglLoader));

    if (!eglLoader)
    {
        LVN_LOG_ERROR(oglBackends->graphicsctx->coreLogger,
                      "[egl] failed to allocate memory for LvnEglLoader");
        goto fail_cleanup;
    }

    oglBackends->loaderHandle = eglLoader;

    // load shared library module
    for (uint32_t i = 0; i < LVN_ARRAY_LEN(s_EglLibNames); i++)
    {
        eglLoader->handle = lvn_platformLoadModule(s_EglLibNames[i]);
        if (eglLoader->handle)
            break;
    }

    if (!eglLoader->handle)
    {
        LVN_LOG_ERROR(oglBackends->graphicsctx->coreLogger, "[egl] failed to load egl shared library");
        LVN_LOG_ERROR(oglBackends->graphicsctx->coreLogger, "[egl] shared library names tried:");
        for (uint32_t i = 0; i < LVN_ARRAY_LEN(s_EglLibNames); i++) {
            LVN_LOG_ERROR(oglBackends->graphicsctx->coreLogger, "[egl] %s", s_EglLibNames[i]);
        }
        goto fail_cleanup;
    }

    // load egl function symbols
    eglLoader->eglGetPlatformDisplay = (PFNEGLGETPLATFORMDISPLAYPROC)
        lvn_platformGetModuleSymbol(eglLoader->handle, "eglGetPlatformDisplay");
    eglLoader->eglInitialize = (PFNEGLINITIALIZEPROC)
        lvn_platformGetModuleSymbol(eglLoader->handle, "eglInitialize");
    eglLoader->eglQueryString = (PFNEGLQUERYSTRINGPROC)
        lvn_platformGetModuleSymbol(eglLoader->handle, "eglQueryString");
    eglLoader->eglChooseConfig = (PFNEGLCHOOSECONFIGPROC)
        lvn_platformGetModuleSymbol(eglLoader->handle, "eglChooseConfig");
    eglLoader->eglCreateWindowSurface = (PFNEGLCREATEWINDOWSURFACEPROC)
        lvn_platformGetModuleSymbol(eglLoader->handle, "eglCreateWindowSurface");
    eglLoader->eglCreateContext = (PFNEGLCREATECONTEXTPROC)
        lvn_platformGetModuleSymbol(eglLoader->handle, "eglCreateContext");
    eglLoader->eglMakeCurrent = (PFNEGLMAKECURRENTPROC)
        lvn_platformGetModuleSymbol(eglLoader->handle, "eglMakeCurrent");
    eglLoader->eglSwapBuffers = (PFNEGLSWAPBUFFERSPROC)
        lvn_platformGetModuleSymbol(eglLoader->handle, "eglSwapBuffers");
    eglLoader->eglSwapInterval = (PFNEGLSWAPINTERVALPROC)
        lvn_platformGetModuleSymbol(eglLoader->handle, "eglSwapInterval");
    eglLoader->eglBindAPI = (PFNEGLBINDAPIPROC)
        lvn_platformGetModuleSymbol(eglLoader->handle, "eglBindAPI");
    eglLoader->eglDestroySurface = (PFNEGLDESTROYSURFACEPROC)
        lvn_platformGetModuleSymbol(eglLoader->handle, "eglDestroySurface");
    eglLoader->eglDestroyContext = (PFNEGLDESTROYCONTEXTPROC)
        lvn_platformGetModuleSymbol(eglLoader->handle, "eglDestroyContext");
    eglLoader->eglTerminate = (PFNEGLTERMINATEPROC)
        lvn_platformGetModuleSymbol(eglLoader->handle, "eglTerminate");
    eglLoader->eglGetProcAddress = (PFNEGLGETPROCADDRESSPROC)
        lvn_platformGetModuleSymbol(eglLoader->handle, "eglGetProcAddress");

    if (!eglLoader->eglGetPlatformDisplay ||
        !eglLoader->eglInitialize ||
        !eglLoader->eglQueryString ||
        !eglLoader->eglChooseConfig ||
        !eglLoader->eglCreateWindowSurface ||
        !eglLoader->eglCreateContext ||
        !eglLoader->eglMakeCurrent ||
        !eglLoader->eglSwapBuffers ||
        !eglLoader->eglSwapInterval ||
        !eglLoader->eglBindAPI ||
        !eglLoader->eglDestroySurface ||
        !eglLoader->eglDestroyContext ||
        !eglLoader->eglTerminate ||
        !eglLoader->eglGetProcAddress)
    {
        LVN_LOG_ERROR(oglBackends->graphicsctx->coreLogger,
                      "[egl] failed to load egl function symbols");
        goto fail_cleanup;
    }

    // get wayland library symbols if using wayland
    LvnWindowPlatformSupport wps = lvn_getWindowPlatform();
    if (wps.wayland)
    {
        // find wayland-egl
        for (uint32_t i = 0; i < LVN_ARRAY_LEN(s_WaylandEglNames); i++)
        {
            eglLoader->wl.waylandEglHandle = lvn_platformLoadModule(s_WaylandEglNames[i]);
            if (eglLoader->wl.waylandEglHandle)
                break;
        }
        if (!eglLoader->wl.waylandEglHandle)
        {
            LVN_LOG_ERROR(oglBackends->graphicsctx->coreLogger, "[egl] failed to load wayland-egl shared library");
            LVN_LOG_ERROR(oglBackends->graphicsctx->coreLogger, "[egl] shared library names tried:");
            for (uint32_t i = 0; i < LVN_ARRAY_LEN(s_WaylandEglNames); i++) {
                LVN_LOG_ERROR(oglBackends->graphicsctx->coreLogger, "[egl] %s", s_WaylandEglNames[i]);
            }
            goto fail_cleanup;
        }

        // load wayland-egl symbols
        eglLoader->wl.wl_egl_window_create = (PFN_wl_egl_window_create)
            lvn_platformGetModuleSymbol(eglLoader->wl.waylandEglHandle, "wl_egl_window_create");
        eglLoader->wl.wl_egl_window_destroy = (PFN_wl_egl_window_destroy)
            lvn_platformGetModuleSymbol(eglLoader->wl.waylandEglHandle, "wl_egl_window_destroy");
        eglLoader->wl.wl_egl_window_resize = (PFN_wl_egl_window_resize)
            lvn_platformGetModuleSymbol(eglLoader->wl.waylandEglHandle, "wl_egl_window_resize");

        if (!eglLoader->wl.wl_egl_window_create ||
            !eglLoader->wl.wl_egl_window_destroy ||
            !eglLoader->wl.wl_egl_window_resize)
        {
            LVN_LOG_ERROR(oglBackends->graphicsctx->coreLogger,
                          "[egl] failed to load wayland-egl function symbols");
            goto fail_cleanup;
        }
    }

    // get platform display
    EGLenum platformEnum = 0;
    if (wps.wayland)
        platformEnum = EGL_PLATFORM_WAYLAND_KHR;
    else if (wps.x11)
        platformEnum = EGL_PLATFORM_X11_KHR;
    else
    {
        LVN_LOG_ERROR(oglBackends->graphicsctx->coreLogger,
                      "[egl] failed to determine window platform enum");
        goto fail_cleanup;
    }

    eglLoader->display = eglLoader->eglGetPlatformDisplay(platformEnum, platformData->ndh, NULL);
    if (eglLoader->display == EGL_NO_DISPLAY)
    {
        LVN_LOG_ERROR(oglBackends->graphicsctx->coreLogger,
                      "[egl] failed to create egl display");
        goto fail_cleanup;
    }

    // egl initialize
    if (!eglLoader->eglInitialize(eglLoader->display, &eglLoader->versionMajor, &eglLoader->versionMinor))
    {
        LVN_LOG_ERROR(oglBackends->graphicsctx->coreLogger,
                      "[egl] failed to initialize egl display");
        goto fail_cleanup;
    }
    if (eglLoader->versionMajor < 1 || (eglLoader->versionMajor == 1 && eglLoader->versionMinor < 5))
    {
        LVN_LOG_ERROR(oglBackends->graphicsctx->coreLogger,
                      "[egl] failed to initialize egl, EGL version 1.5 or higher required, current version: (%d.%d)",
                      eglLoader->versionMajor, eglLoader->versionMinor);
        goto fail_cleanup;
    }

    // extensions
    const char* eglExt = eglLoader->eglQueryString(eglLoader->display, EGL_EXTENSIONS);
    if (eglExt && strstr(eglExt, "EGL_KHR_gl_colorspace"))
    {
        eglLoader->ext.colorspace = true;
    }

    // choose framebuffer config attributes
    EGLint configAttribs[] =
    {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 24,
        EGL_NONE
    };

    EGLint numConfigs;
    eglLoader->eglChooseConfig(eglLoader->display, configAttribs, &eglLoader->config, 1, &numConfigs);

    // create surface
    if (lvn_eglCreateSurface(oglBackends->graphicsctx,
                             eglLoader,
                             &eglLoader->surfaceData,
                             eglLoader->display,
                             eglLoader->config,
                             platformData->nwh,
                             1, 1) != Lvn_Result_Success)
    {
        LVN_LOG_ERROR(oglBackends->graphicsctx->coreLogger,
                      "[egl] failed to create egl surface");
        goto fail_cleanup;
    }

    oglBackends->defaultSurface = (LvnSurface){
        .graphicsctx = oglBackends->graphicsctx,
        .surfaceData = &eglLoader->surfaceData,
    };

    // bind egl api
    if (!eglLoader->eglBindAPI(EGL_OPENGL_API))
    {
        LVN_LOG_ERROR(oglBackends->graphicsctx->coreLogger,
                      "[egl] failed to bind egl api EGL_OPENGL_API");
        goto fail_cleanup;
    }

    // create context and attributes
    EGLint ctxAttribs[] =
    {
        EGL_CONTEXT_MAJOR_VERSION, LVN_OGL_CONTEXT_MAJOR,
        EGL_CONTEXT_MINOR_VERSION, LVN_OGL_CONTEXT_MINOR,
        EGL_CONTEXT_OPENGL_PROFILE_MASK, EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT,
#ifdef LVN_CONFIG_DEBUG
        EGL_CONTEXT_OPENGL_DEBUG, EGL_TRUE,
#endif
        EGL_NONE,
    };

    eglLoader->context = eglLoader->eglCreateContext(eglLoader->display, eglLoader->config, EGL_NO_CONTEXT, ctxAttribs);
    if (!eglLoader->context)
    {
        LVN_LOG_ERROR(oglBackends->graphicsctx->coreLogger,
                      "[egl] failed to create egl context");
        goto fail_cleanup;
    }

    // get opengl function symbols
    oglBackends->glGetString = (PFNGLGETSTRINGPROC)
        eglLoader->eglGetProcAddress("glGetString");
    oglBackends->glGetError = (PFNGLGETERRORPROC)
        eglLoader->eglGetProcAddress("glGetError");
    oglBackends->glDebugMessageCallback = (PFNGLDEBUGMESSAGECALLBACKPROC)
        eglLoader->eglGetProcAddress("glDebugMessageCallback");
    oglBackends->glGetIntegerv = (PFNGLGETINTEGERVPROC)
        eglLoader->eglGetProcAddress("glGetIntegerv");
    oglBackends->glEnable = (PFNGLENABLEPROC)
        eglLoader->eglGetProcAddress("glEnable");
    oglBackends->glEnablei = (PFNGLENABLEIPROC)
        eglLoader->eglGetProcAddress("glEnablei");
    oglBackends->glDisable = (PFNGLDISABLEPROC)
        eglLoader->eglGetProcAddress("glDisable");
    oglBackends->glDisablei = (PFNGLDISABLEIPROC)
        eglLoader->eglGetProcAddress("glDisablei");
    oglBackends->glCreateBuffers = (PFNGLCREATEBUFFERSPROC)
        eglLoader->eglGetProcAddress("glCreateBuffers");
    oglBackends->glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)
        eglLoader->eglGetProcAddress("glDeleteBuffers");
    oglBackends->glCreateSamplers = (PFNGLCREATESAMPLERSPROC)
        eglLoader->eglGetProcAddress("glCreateSamplers");
    oglBackends->glDeleteSamplers = (PFNGLDELETESAMPLERSPROC)
        eglLoader->eglGetProcAddress("glDeleteSamplers");
    oglBackends->glCreateTextures = (PFNGLCREATETEXTURESPROC)
        eglLoader->eglGetProcAddress("glCreateTextures");
    oglBackends->glDeleteTextures = (PFNGLDELETETEXTURESPROC)
        eglLoader->eglGetProcAddress("glDeleteTextures");
    oglBackends->glCreateFramebuffers = (PFNGLCREATEFRAMEBUFFERSPROC)
        eglLoader->eglGetProcAddress("glCreateFramebuffers");
    oglBackends->glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)
        eglLoader->eglGetProcAddress("glDeleteFramebuffers");
    oglBackends->glCreateVertexArrays = (PFNGLCREATEVERTEXARRAYSPROC)
        eglLoader->eglGetProcAddress("glCreateVertexArrays");
    oglBackends->glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)
        eglLoader->eglGetProcAddress("glDeleteVertexArrays");
    oglBackends->glCreateShader = (PFNGLCREATESHADERPROC)
        eglLoader->eglGetProcAddress("glCreateShader");
    oglBackends->glDeleteShader = (PFNGLDELETESHADERPROC)
        eglLoader->eglGetProcAddress("glDeleteShader");
    oglBackends->glCreateProgram = (PFNGLCREATEPROGRAMPROC)
        eglLoader->eglGetProcAddress("glCreateProgram");
    oglBackends->glDeleteProgram = (PFNGLDELETEPROGRAMPROC)
        eglLoader->eglGetProcAddress("glDeleteProgram");
    oglBackends->glFenceSync = (PFNGLFENCESYNCPROC)
        eglLoader->eglGetProcAddress("glFenceSync");
    oglBackends->glDeleteSync = (PFNGLDELETESYNCPROC)
        eglLoader->eglGetProcAddress("glDeleteSync");
    oglBackends->glClientWaitSync = (PFNGLCLIENTWAITSYNCPROC)
        eglLoader->eglGetProcAddress("glClientWaitSync");
    oglBackends->glWaitSync = (PFNGLWAITSYNCPROC)
        eglLoader->eglGetProcAddress("glWaitSync");
    oglBackends->glFlush = (PFNGLFLUSHPROC)
        eglLoader->eglGetProcAddress("glFlush");
    oglBackends->glCheckNamedFramebufferStatus = (PFNGLCHECKNAMEDFRAMEBUFFERSTATUSPROC)
        eglLoader->eglGetProcAddress("glCheckNamedFramebufferStatus");
    oglBackends->glNamedFramebufferTexture = (PFNGLNAMEDFRAMEBUFFERTEXTUREPROC)
        eglLoader->eglGetProcAddress("glNamedFramebufferTexture");
    oglBackends->glNamedFramebufferDrawBuffer = (PFNGLNAMEDFRAMEBUFFERDRAWBUFFERPROC)
        eglLoader->eglGetProcAddress("glNamedFramebufferDrawBuffer");
    oglBackends->glNamedFramebufferDrawBuffers = (PFNGLNAMEDFRAMEBUFFERDRAWBUFFERSPROC)
        eglLoader->eglGetProcAddress("glNamedFramebufferDrawBuffers");
    oglBackends->glNamedBufferSubData = (PFNGLNAMEDBUFFERSUBDATAPROC)
        eglLoader->eglGetProcAddress("glNamedBufferSubData");
    oglBackends->glSamplerParameteri = (PFNGLSAMPLERPARAMETERIPROC)
        eglLoader->eglGetProcAddress("glSamplerParameteri");
    oglBackends->glTextureParameteri = (PFNGLTEXTUREPARAMETERIPROC)
        eglLoader->eglGetProcAddress("glTextureParameteri");
    oglBackends->glTextureStorage2D = (PFNGLTEXTURESTORAGE2DPROC)
        eglLoader->eglGetProcAddress("glTextureStorage2D");
    oglBackends->glTextureStorage2DMultisample = (PFNGLTEXTURESTORAGE2DMULTISAMPLEPROC)
        eglLoader->eglGetProcAddress("glTextureStorage2DMultisample");
    oglBackends->glTextureSubImage2D = (PFNGLTEXTURESUBIMAGE2DPROC)
        eglLoader->eglGetProcAddress("glTextureSubImage2D");
    oglBackends->glShaderSource = (PFNGLSHADERSOURCEPROC)
        eglLoader->eglGetProcAddress("glShaderSource");
    oglBackends->glCompileShader = (PFNGLCOMPILESHADERPROC)
        eglLoader->eglGetProcAddress("glCompileShader");
    oglBackends->glGetShaderiv = (PFNGLGETSHADERIVPROC)
        eglLoader->eglGetProcAddress("glGetShaderiv");
    oglBackends->glAttachShader = (PFNGLATTACHSHADERPROC)
        eglLoader->eglGetProcAddress("glAttachShader");
    oglBackends->glLinkProgram = (PFNGLLINKPROGRAMPROC)
        eglLoader->eglGetProcAddress("glLinkProgram");
    oglBackends->glGetProgramiv = (PFNGLGETPROGRAMIVPROC)
        eglLoader->eglGetProcAddress("glGetProgramiv");
    oglBackends->glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)
        eglLoader->eglGetProcAddress("glGetProgramInfoLog");
    oglBackends->glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)
        eglLoader->eglGetProcAddress("glGetShaderInfoLog");
    oglBackends->glBlendFuncSeparatei = (PFNGLBLENDFUNCSEPARATEIPROC)
        eglLoader->eglGetProcAddress("glBlendFuncSeparatei");
    oglBackends->glBlendEquationSeparatei = (PFNGLBLENDEQUATIONSEPARATEIPROC)
        eglLoader->eglGetProcAddress("glBlendEquationSeparatei");
    oglBackends->glColorMaski = (PFNGLCOLORMASKIPROC)
        eglLoader->eglGetProcAddress("glColorMaski");
    oglBackends->glPolygonMode = (PFNGLPOLYGONMODEPROC)
        eglLoader->eglGetProcAddress("glPolygonMode");
    oglBackends->glNamedBufferStorage = (PFNGLNAMEDBUFFERSTORAGEPROC)
        eglLoader->eglGetProcAddress("glNamedBufferStorage");
    oglBackends->glNamedBufferData = (PFNGLNAMEDBUFFERDATAPROC)
        eglLoader->eglGetProcAddress("glNamedBufferData");
    oglBackends->glMapNamedBufferRange = (PFNGLMAPNAMEDBUFFERRANGEPROC)
        eglLoader->eglGetProcAddress("glMapNamedBufferRange");
    oglBackends->glUnmapNamedBuffer = (PFNGLUNMAPNAMEDBUFFERPROC)
        eglLoader->eglGetProcAddress("glUnmapNamedBuffer");
    oglBackends->glEnableVertexArrayAttrib = (PFNGLENABLEVERTEXARRAYATTRIBPROC)
        eglLoader->eglGetProcAddress("glEnableVertexArrayAttrib");
    oglBackends->glVertexArrayAttribBinding = (PFNGLVERTEXARRAYATTRIBBINDINGPROC)
        eglLoader->eglGetProcAddress("glVertexArrayAttribBinding");
    oglBackends->glVertexArrayAttribFormat = (PFNGLVERTEXARRAYATTRIBFORMATPROC)
        eglLoader->eglGetProcAddress("glVertexArrayAttribFormat");
    oglBackends->glVertexArrayAttribIFormat = (PFNGLVERTEXARRAYATTRIBIFORMATPROC)
        eglLoader->eglGetProcAddress("glVertexArrayAttribIFormat");
    oglBackends->glVertexArrayAttribLFormat = (PFNGLVERTEXARRAYATTRIBLFORMATPROC)
        eglLoader->eglGetProcAddress("glVertexArrayAttribLFormat");
    oglBackends->glVertexArrayVertexBuffer = (PFNGLVERTEXARRAYVERTEXBUFFERPROC)
        eglLoader->eglGetProcAddress("glVertexArrayVertexBuffer");
    oglBackends->glVertexArrayVertexBuffers = (PFNGLVERTEXARRAYVERTEXBUFFERSPROC)
        eglLoader->eglGetProcAddress("glVertexArrayVertexBuffers");
    oglBackends->glUseProgram = (PFNGLUSEPROGRAMPROC)
        eglLoader->eglGetProcAddress("glUseProgram");
    oglBackends->glBindBuffer = (PFNGLBINDBUFFERPROC)
        eglLoader->eglGetProcAddress("glBindBuffer");
    oglBackends->glBindVertexBuffers = (PFNGLBINDVERTEXBUFFERSPROC)
        eglLoader->eglGetProcAddress("glBindVertexBuffers");
    oglBackends->glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)
        eglLoader->eglGetProcAddress("glBindVertexArray");
    oglBackends->glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)
        eglLoader->eglGetProcAddress("glBindFramebuffer");
    oglBackends->glBindBufferRange = (PFNGLBINDBUFFERRANGEPROC)
        eglLoader->eglGetProcAddress("glBindBufferRange");
    oglBackends->glBindTextureUnit = (PFNGLBINDTEXTUREUNITPROC)
        eglLoader->eglGetProcAddress("glBindTextureUnit");
    oglBackends->glBlitFramebuffer = (PFNGLBLITFRAMEBUFFERPROC)
        eglLoader->eglGetProcAddress("glBlitFramebuffer");
    oglBackends->glClear = (PFNGLCLEARPROC)
        eglLoader->eglGetProcAddress("glClear");
    oglBackends->glClearColor = (PFNGLCLEARCOLORPROC)
        eglLoader->eglGetProcAddress("glClearColor");
    oglBackends->glClearNamedFramebufferiv = (PFNGLCLEARNAMEDFRAMEBUFFERIVPROC)
        eglLoader->eglGetProcAddress("glClearNamedFramebufferiv");
    oglBackends->glClearNamedFramebufferuiv = (PFNGLCLEARNAMEDFRAMEBUFFERUIVPROC)
        eglLoader->eglGetProcAddress("glClearNamedFramebufferuiv");
    oglBackends->glClearNamedFramebufferfv = (PFNGLCLEARNAMEDFRAMEBUFFERFVPROC)
        eglLoader->eglGetProcAddress("glClearNamedFramebufferfv");
    oglBackends->glClearNamedFramebufferfi = (PFNGLCLEARNAMEDFRAMEBUFFERFIPROC)
        eglLoader->eglGetProcAddress("glClearNamedFramebufferfi");
    oglBackends->glDrawArraysInstancedBaseInstance = (PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC)
        eglLoader->eglGetProcAddress("glDrawArraysInstancedBaseInstance");
    oglBackends->glDrawElementsInstancedBaseVertexBaseInstance = (PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEPROC)
        eglLoader->eglGetProcAddress("glDrawElementsInstancedBaseVertexBaseInstance");
    oglBackends->glDepthRange = (PFNGLDEPTHRANGEPROC)
        eglLoader->eglGetProcAddress("glDepthRange");
    oglBackends->glViewport = (PFNGLVIEWPORTPROC)
        eglLoader->eglGetProcAddress("glViewport");
    oglBackends->glScissor = (PFNGLSCISSORPROC)
        eglLoader->eglGetProcAddress("glScissor");
    oglBackends->glDepthMask = (PFNGLDEPTHMASKPROC)
        eglLoader->eglGetProcAddress("glDepthMask");
    oglBackends->glDepthFunc = (PFNGLDEPTHFUNCPROC)
        eglLoader->eglGetProcAddress("glDepthFunc");

    // bind function pointers
    oglBackends->ogllCreateSurface = lvnEglCreateSurface;
    oglBackends->ogllDestroySurface = lvnEglDestroySurface;
    oglBackends->ogllSurfaceResize = lvnEglSurfaceResize;
    oglBackends->ogllMakeCurrent = lvnEglMakeCurrent;
    oglBackends->ogllSwapBuffers = lvnEglSwapBuffers;
    oglBackends->ogllSwapInterval = lvnEglSwapInterval;

    eglLoader->eglMakeCurrent(eglLoader->display, eglLoader->surfaceData.surface, eglLoader->surfaceData.surface, eglLoader->context);

    return Lvn_Result_Success;

fail_cleanup:
    lvnEglLoaderTerminate(oglBackends);
    return Lvn_Result_Failure;
}

void lvnEglLoaderTerminate(LvnOpenglBackends* oglBackends)
{
    LVN_ASSERT(oglBackends, "oglBackends cannot be null");

    LvnEglLoader* eglLoader = (LvnEglLoader*) oglBackends->loaderHandle;
    if (!eglLoader)
        return;

    if (eglLoader->context)
        eglLoader->eglDestroyContext(eglLoader->display, eglLoader->context);

    lvn_eglDestroySurface(eglLoader, &eglLoader->surfaceData);

    eglLoader->eglTerminate(eglLoader->display);

    if (eglLoader->wl.waylandEglHandle) lvn_platformFreeModule(eglLoader->wl.waylandEglHandle);
    if (eglLoader->handle) lvn_platformFreeModule(eglLoader->handle);

    lvn_free(eglLoader);
    oglBackends->loaderHandle = NULL;
}

LvnResult lvnEglCreateSurface(const LvnOpenglBackends* oglBackends, LvnSurface* surface, const LvnSurfaceCreateInfo* createInfo)
{
    LVN_ASSERT(oglBackends && surface && createInfo, "oglBackends, surface, and createInfo cannot be null");

    LvnEglLoader* eglLoader = (LvnEglLoader*) oglBackends->loaderHandle;

    LvnResult result = Lvn_Result_Failure;
    LvnEglSurfaceData* surfaceData = NULL;

    surfaceData = (LvnEglSurfaceData*) lvn_calloc(sizeof(LvnEglSurfaceData));
    if (!surfaceData)
    {
        LVN_LOG_ERROR(oglBackends->graphicsctx->coreLogger,
                      "[egl] failed to allocate memory for egl surface data for surface object at %p",
                      surface);
        result = Lvn_Result_OutOfMemory;
        goto fail_cleanup;
    }

    if (lvn_eglCreateSurface(oglBackends->graphicsctx,
                             eglLoader,
                             surfaceData,
                             eglLoader->display,
                             eglLoader->config,
                             createInfo->nwh,
                             1, 1) != Lvn_Result_Success)
    {
        LVN_LOG_ERROR(oglBackends->graphicsctx->coreLogger,
                      "[egl] failed to create egl surface for surface object at %p",
                      surface);
        goto fail_cleanup;
    }

    surface->surfaceData = surfaceData;

    return Lvn_Result_Success;

fail_cleanup:
    if (surfaceData)
        lvn_eglDestroySurface(eglLoader, surfaceData);
    lvn_free(surfaceData);
    return result;
}

void lvnEglDestroySurface(const LvnOpenglBackends* oglBackends, LvnSurface* surface)
{
    LVN_ASSERT(oglBackends && surface, "oglBackends and surface cannot be null");

    LvnEglLoader* eglLoader = (LvnEglLoader*) oglBackends->loaderHandle;
    LvnEglSurfaceData* surfaceData = (LvnEglSurfaceData*) surface->surfaceData;

    lvn_eglDestroySurface(eglLoader, surfaceData);
    lvn_free(surfaceData);

    surface->surfaceData = NULL;
}

void lvnEglSurfaceResize(const LvnOpenglBackends* oglBackends, const LvnSurface* surface, int width, int height)
{
    LVN_ASSERT(oglBackends && surface, "oglBackends and surface cannot be null");
    LvnEglLoader* eglLoader = (LvnEglLoader*) oglBackends->loaderHandle;
    LvnEglSurfaceData* surfaceData = (LvnEglSurfaceData*) surface->surfaceData;

#ifdef LVN_INCLUDE_WAYLAND
    if (surfaceData->wlWindow)
        eglLoader->wl.wl_egl_window_resize(surfaceData->wlWindow, width, height, 0, 0);
#endif
}

void lvnEglMakeCurrent(const LvnOpenglBackends* oglBackends, const LvnSurface* surface)
{
    LVN_ASSERT(oglBackends, "oglBackends cannot be null");
    LvnEglLoader* eglLoader = (LvnEglLoader*) oglBackends->loaderHandle;
    LvnEglSurfaceData* surfaceData = (LvnEglSurfaceData*) surface->surfaceData;

    eglLoader->eglMakeCurrent(eglLoader->display, surfaceData->surface, surfaceData->surface, eglLoader->context);
}

void lvnEglSwapBuffers(const LvnOpenglBackends* oglBackends, const LvnSurface* surface)
{
    LVN_ASSERT(oglBackends, "oglBackends cannot be null");
    LvnEglLoader* eglLoader = (LvnEglLoader*) oglBackends->loaderHandle;
    LvnEglSurfaceData* surfaceData = (LvnEglSurfaceData*) surface->surfaceData;

    eglLoader->eglSwapBuffers(eglLoader->display, surfaceData->surface);
}

void lvnEglSwapInterval(const LvnOpenglBackends* oglBackends, int interval)
{
    LVN_ASSERT(oglBackends, "oglBackends cannot be null");
    LvnEglLoader* eglLoader = (LvnEglLoader*) oglBackends->loaderHandle;

    eglLoader->eglSwapInterval(eglLoader->display, interval);
}
