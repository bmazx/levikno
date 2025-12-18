#include "levikno_internal.h"

#if defined(LVN_PLATFORM_UNIX)

#include <dlfcn.h>

void* lvn_platformLoadModule(const char* path)
{
    return dlopen(path, RTLD_LAZY | RTLD_LOCAL);
}

void lvn_platformFreeModule(void* handle)
{
    dlclose(handle);
}

LvnProc lvn_platformGetModuleSymbol(void* handle, const char* name)
{
    LvnProc proc = NULL;
    *(void**) &proc = dlsym(handle, name); // cast to (void**) to avoid ISO C warning in gcc
    return proc;
}

#endif
