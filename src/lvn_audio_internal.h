#include "lvn_audio.h"

#include "levikno_internal.h"


struct LvnAudioContext
{
    const LvnContext*                         ctx;
    const LvnLogger*                          coreLogger;

    // audio implementation
    void*                                     implData;
};
