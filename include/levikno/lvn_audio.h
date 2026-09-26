#ifndef HG_LVN_AUDIO_H
#define HG_LVN_AUDIO_H

#include "lvn_config.h"


typedef struct LvnAudioContext LvnAudioContext;
typedef struct LvnAudioContextFunctions LvnAudioContextFunctions;
typedef struct LvnAudioContextCreateInfo LvnAudioContextCreateInfo;

struct LvnContext;


struct LvnAudioContextFunctions
{

};

struct LvnAudioContextCreateInfo
{
    LvnAudioContextFunctions*    actxFuncs;
};


#ifdef __cplusplus
extern "C" {
#endif

LVN_API LvnResult lvnCreateAudioContext(struct LvnContext* ctx, LvnAudioContext** audioctx, const LvnAudioContextCreateInfo* createInfo); // createa audio context
LVN_API void      lvnDestroyAudioContext(LvnAudioContext* audioctx);                                                                      // destroy audio context

#ifdef __cplusplus
}
#endif


#endif // !HG_LVN_AUDIO_H
