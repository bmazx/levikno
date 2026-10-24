#include "lvn_audio.h"

#include "levikno_internal.h"


struct LvnSoundBuffer
{
    const LvnAudioContext*    audioctx;
    void*                     soundBufferData;
};

struct LvnSound
{
    const LvnAudioContext*    audioctx;
    void*                     soundData;
    LvnSoundFlagBits          flags;
};

struct LvnAudioContext
{
    const LvnContext*                            ctx;
    const LvnLogger*                             coreLogger;

    // audio implementation
    void*                                        implData;
    PFN_lvnAudioSetGlobalTimeMilliSeconds        implAudioSetGlobalTimeMilliSeconds;
    PFN_lvnAudioSetGlobalTimePcmFrames           implAudioSetGlobalTimePcmFrames;
    PFN_lvnAudioSetMasterVolume                  implAudioSetMasterVolume;
    PFN_lvnAudioGetSampleRate                    implAudioGetSampleRate;
    PFN_lvnAudioGetGlobalTimeMilliseconds        implAudioGetGlobalTimeMilliseconds;
    PFN_lvnAudioGetGlobalTimePcmFrames           implAudioGetGlobalTimePcmFrames;
    PFN_lvnListenerSetPosition                   implListenerSetPosition;
    PFN_lvnListenerSetDirection                  implListenerSetDirection;
    PFN_lvnListenerSetVelocity                   implListenerSetVelocity;
    PFN_lvnListenerSetWorldUp                    implListenerSetWorldUp;
    PFN_lvnListenerSetCone                       implListenerSetCone;
    PFN_lvnListenerGetPosition                   implListenerGetPosition;
    PFN_lvnListenerGetDirection                  implListenerGetDirection;
    PFN_lvnListenerGetWorldUp                    implListenerGetWorldUp;
    PFN_lvnListenerGetCone                       implListenerGetCone;
    PFN_lvnCreateSoundBuffer                     implCreateSoundBuffer;
    PFN_lvnDestroySoundBuffer                    implDestroySoundBuffer;
    PFN_lvnCreateSound                           implCreateSound;
    PFN_lvnDestroySound                          implDestroySound;
    PFN_lvnSoundSetVolume                        implSoundSetVolume;
    PFN_lvnSoundSetPan                           implSoundSetPan;
    PFN_lvnSoundSetPitch                         implSoundSetPitch;
    PFN_lvnSoundSetPositioning                   implSoundSetPositioning;
    PFN_lvnSoundSetPosition                      implSoundSetPosition;
    PFN_lvnSoundSetDirection                     implSoundSetDirection;
    PFN_lvnSoundSetVelocity                      implSoundSetVelocity;
    PFN_lvnSoundSetCone                          implSoundSetCone;
    PFN_lvnSoundSetAttenuation                   implSoundSetAttenuation;
    PFN_lvnSoundSetRolloff                       implSoundSetRolloff;
    PFN_lvnSoundSetMinGain                       implSoundSetMinGain;
    PFN_lvnSoundSetMaxGain                       implSoundSetMaxGain;
    PFN_lvnSoundSetMinDistance                   implSoundSetMinDistance;
    PFN_lvnSoundSetMaxDistance                   implSoundSetMaxDistance;
    PFN_lvnSoundSetDopplerFactor                 implSoundSetDopplerFactor;
    PFN_lvnSoundSetLooping                       implSoundSetLooping;
    PFN_lvnSoundStart                            implSoundStart;
    PFN_lvnSoundStop                             implSoundStop;
    PFN_lvnSoundScheduleStartTimePcmFrames       implSoundScheduleStartTimePcmFrames;
    PFN_lvnSoundScheduleStartTimeMilliseconds    implSoundScheduleStartTimeMilliseconds;
    PFN_lvnSoundScheduleStopTimePcmFrames        implSoundScheduleStopTimePcmFrames;
    PFN_lvnSoundScheduleStopTimeMilliseconds     implSoundScheduleStopTimeMilliseconds;
    PFN_lvnSoundSetFadeMilliseconds              implSoundSetFadeMilliseconds;
    PFN_lvnSoundSetFadePcmFrames                 implSoundSetFadePcmFrames;
    PFN_lvnSoundSeekToPcmFrame                   implSoundSeekToPcmFrame;
    PFN_lvnSoundGetVolume                        implSoundGetVolume;
    PFN_lvnSoundGetPan                           implSoundGetPan;
    PFN_lvnSoundGetPitch                         implSoundGetPitch;
    PFN_lvnSoundGetPositioning                   implSoundGetPositioning;
    PFN_lvnSoundGetPosition                      implSoundGetPosition;
    PFN_lvnSoundGetDirection                     implSoundGetDirection;
    PFN_lvnSoundGetVelocity                      implSoundGetVelocity;
    PFN_lvnSoundGetCone                          implSoundGetCone;
    PFN_lvnSoundGetAttenuation                   implSoundGetAttenuation;
    PFN_lvnSoundGetRolloff                       implSoundGetRolloff;
    PFN_lvnSoundGetMinGain                       implSoundGetMinGain;
    PFN_lvnSoundGetMaxGain                       implSoundGetMaxGain;
    PFN_lvnSoundGetMinDistance                   implSoundGetMinDistance;
    PFN_lvnSoundGetMaxDistance                   implSoundGetMaxDistance;
    PFN_lvnSoundGetDopplerFactor                 implSoundGetDopplerFactor;
    PFN_lvnSoundIsLooping                        implSoundIsLooping;
    PFN_lvnSoundIsPlaying                        implSoundIsPlaying;
    PFN_lvnSoundAtEnd                            implSoundAtEnd;
    PFN_lvnSoundGetTimeMilliseconds              implSoundGetTimeMilliseconds;
    PFN_lvnSoundGetTimePcmFrames                 implSoundGetTimePcmFrames;
    PFN_lvnSoundGetLengthSeconds                 implSoundGetLengthSeconds;
};
