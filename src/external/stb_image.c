#include "levikno_internal.h"

#define STBI_MALLOC(sz)           lvn_calloc(sz)
#define STBI_REALLOC(p,newsz)     lvn_realloc(p,newsz)
#define STBI_FREE(p)              lvn_free(p)

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
