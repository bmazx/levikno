#ifndef HG_LVN_AUDIO_H
#define HG_LVN_AUDIO_H

#include "lvn_config.h"


typedef struct LvnAudioContext LvnAudioContext;

struct LvnContext;


typedef struct LvnAudioContextCreateInfo
{

} LvnAudioContextCreateInfo;


#ifdef __cplusplus
extern "C" {
#endif

LVN_API LvnResult lvnCreateAudioContext(struct LvnContext* ctx, LvnAudioContext** audioctx, const LvnAudioContextCreateInfo* createInfo);
LVN_API void      lvnDestroyGraphicsContext(LvnAudioContext* audioctx);

#ifdef __cplusplus
}
#endif


#endif // !HG_LVN_AUDIO_H
