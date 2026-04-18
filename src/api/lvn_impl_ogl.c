#include "lvn_impl_ogl.h"

#if defined(LVN_INCLUDE_X11)
    #include <GL/glx.h>
#endif

#if defined(LVN_PLATFORM_LINUX)
    static const char* s_LvnOglLibName = "libGL.so.1";
#elif defined(LVN_PLATFORM_WINDOWS)
    static const char* s_LvnOglLibName = "opengl32.dll";
#elif defined(LVN_PLATFORM_MACOS)
    static const char* s_LvnOglLibName = "/System/Library/Frameworks/OpenGL.framework/OpenGL";
#endif

LvnResult lvnImplOglInit(LvnGraphicsContext* graphicsctx, const LvnGraphicsContextCreateInfo* createInfo)
{
    LVN_ASSERT(graphicsctx && createInfo, "graphicsctx and createInfo cannot be null");

    LvnOpenglBackends* oglBackends = (LvnOpenglBackends*) lvn_calloc(sizeof(LvnOpenglBackends));
    graphicsctx->implData = oglBackends;

    oglBackends->graphicsctx = graphicsctx;

    // load opengl library
    oglBackends->handle = lvn_platformLoadModule(s_LvnOglLibName);

    if (!oglBackends->handle)
    {
        LVN_LOG_ERROR(graphicsctx->coreLogger,
                      "[opengl] unable to load opengl shared library: %s",
                      s_LvnOglLibName);
        goto fail_cleanup;
    }

    return Lvn_Result_Success;

fail_cleanup:
    lvnImplOglTerminate(graphicsctx);
    return Lvn_Result_Failure;
}

void lvnImplOglTerminate(LvnGraphicsContext* graphicsctx)
{
    LVN_ASSERT(graphicsctx, "graphicsctx cannot be null");

    LvnOpenglBackends* oglBackends = (LvnOpenglBackends*) graphicsctx->implData;
    if (!oglBackends)
        return;

    if (oglBackends->handle)
        lvn_platformFreeModule(oglBackends->handle);

    lvn_free(oglBackends);
    graphicsctx->implData = NULL;
}
