#ifndef HG_LVN_AUDIO_H
#define HG_LVN_AUDIO_H

#include "lvn_config.h"


typedef enum LvnSoundFlags
{
    Lvn_SoundFlag_None                  = 0x00000000,
    Lvn_SoundFlag_Stream                = 0x00000001,
    Lvn_SoundFlag_Decode                = 0x00000002,
    Lvn_SoundFlag_Async                 = 0x00000004,
    Lvn_SoundFlag_WaitInit              = 0x00000008,
    LVN_SOUNDFLAG_ENUM_MAX              = 0x7fffffff,
} LvnSoundBufferFlags;
typedef LvnFlags LvnSoundFlagBits;

typedef enum LvnSoundPositioning
{
    Lvn_SoundPositioning_Absolute,
    Lvn_SoundPositioning_Relative,
    LVN_SOUNDPOSITIONING_ENUM_MAX = 0x7fffffff,
} LvnSoundPositioning;

typedef enum LvnSoundAttenuation
{
    Lvn_Attenuation_None,
    Lvn_Attenuation_Inverse,
    Lvn_Attenuation_Linear,
    Lvn_Attenuation_Exponential,
    LVN_ATTENUATION_ENUM_MAX = 0x7fffffff,
} LvnSoundAttenuation;


typedef struct LvnAudioContext LvnAudioContext;
typedef struct LvnAudioContextFunctions LvnAudioContextFunctions;
typedef struct LvnAudioContextCreateInfo LvnAudioContextCreateInfo;
typedef struct LvnSoundBuffer LvnSoundBuffer;
typedef struct LvnSound LvnSound;
typedef struct LvnSoundBufferCreateInfo LvnSoundBufferCreateInfo;
typedef struct LvnSoundCreateInfo LvnSoundCreateInfo;

struct LvnContext;


typedef void                     (*PFN_lvnAudioSetGlobalTimeMilliSeconds)(LvnAudioContext* audioctx, uint64_t ms);
typedef void                     (*PFN_lvnAudioSetGlobalTimePcmFrames)(LvnAudioContext* audioctx, uint64_t pcm);
typedef void                     (*PFN_lvnAudioSetMasterVolume)(LvnAudioContext* audioctx, float volume);
typedef uint32_t                 (*PFN_lvnAudioGetSampleRate)(LvnAudioContext* audioctx);
typedef uint64_t                 (*PFN_lvnAudioGetGlobalTimeMilliseconds)(LvnAudioContext* audioctx);
typedef uint64_t                 (*PFN_lvnAudioGetGlobalTimePcmFrames)(LvnAudioContext* audioctx);
typedef void                     (*PFN_lvnListenerSetPosition)(LvnAudioContext* audioctx, float x, float y, float z);
typedef void                     (*PFN_lvnListenerSetDirection)(LvnAudioContext* audioctx, float x, float y, float z);
typedef void                     (*PFN_lvnListenerSetVelocity)(LvnAudioContext* audioctx, float x, float y, float z);
typedef void                     (*PFN_lvnListenerSetWorldUp)(LvnAudioContext* audioctx, float x, float y, float z);
typedef void                     (*PFN_lvnListenerSetCone)(LvnAudioContext* audioctx, float innerAngleRad, float outerAngleRad, float outerGain);
typedef void                     (*PFN_lvnListenerGetPosition)(LvnAudioContext* audioctx, float* x, float* y, float* z);
typedef void                     (*PFN_lvnListenerGetDirection)(LvnAudioContext* audioctx, float* x, float* y, float* z);
typedef void                     (*PFN_lvnListenerGetWorldUp)(LvnAudioContext* audioctx, float* x, float* y, float* z);
typedef void                     (*PFN_lvnListenerGetCone)(LvnAudioContext* audioctx, float* innerAngleRad, float* outerAngleRad, float* outerGain);
typedef LvnResult                (*PFN_lvnCreateSoundBuffer)(const LvnAudioContext* audioctx, LvnSoundBuffer* soundBuffer, const LvnSoundBufferCreateInfo* createInfo);
typedef void                     (*PFN_lvnDestroySoundBuffer)(LvnSoundBuffer* soundBuffer);
typedef LvnResult                (*PFN_lvnCreateSound)(const LvnAudioContext* audioctx, LvnSound* sound, const LvnSoundCreateInfo* createInfo);
typedef void                     (*PFN_lvnDestroySound)(LvnSound* sound);
typedef void                     (*PFN_lvnSoundSetVolume)(LvnSound* sound, float volume);
typedef void                     (*PFN_lvnSoundSetPan)(LvnSound* sound, float pan);
typedef void                     (*PFN_lvnSoundSetPitch)(LvnSound* sound, float pitch);
typedef void                     (*PFN_lvnSoundSetPositioning)(LvnSound* sound, LvnSoundPositioning positioning);
typedef void                     (*PFN_lvnSoundSetPosition)(LvnSound* sound, float x, float y, float z);
typedef void                     (*PFN_lvnSoundSetDirection)(LvnSound* sound, float x, float y, float z);
typedef void                     (*PFN_lvnSoundSetVelocity)(LvnSound* sound, float x, float y, float z);
typedef void                     (*PFN_lvnSoundSetCone)(LvnSound* sound, float innerAngleRad, float outerAngleRad, float outerGain);
typedef void                     (*PFN_lvnSoundSetAttenuation)(LvnSound* sound, LvnSoundAttenuation attenuation);
typedef void                     (*PFN_lvnSoundSetRolloff)(LvnSound* sound, float rolloff);
typedef void                     (*PFN_lvnSoundSetMinGain)(LvnSound* sound, float minGain);
typedef void                     (*PFN_lvnSoundSetMaxGain)(LvnSound* sound, float maxGain);
typedef void                     (*PFN_lvnSoundSetMinDistance)(LvnSound* sound, float minDist);
typedef void                     (*PFN_lvnSoundSetMaxDistance)(LvnSound* sound, float maxDist);
typedef void                     (*PFN_lvnSoundSetDopplerFactor)(LvnSound* sound, float dopplerFactor);
typedef void                     (*PFN_lvnSoundSetLooping)(LvnSound* sound, bool looping);
typedef void                     (*PFN_lvnSoundStart)(LvnSound* sound);
typedef void                     (*PFN_lvnSoundStop)(LvnSound* sound);
typedef void                     (*PFN_lvnSoundScheduleStartTimePcmFrames)(LvnSound* sound, uint64_t pcm);
typedef void                     (*PFN_lvnSoundScheduleStartTimeMilliseconds)(LvnSound* sound, uint64_t ms);
typedef void                     (*PFN_lvnSoundScheduleStopTimePcmFrames)(LvnSound* sound, uint64_t pcm);
typedef void                     (*PFN_lvnSoundScheduleStopTimeMilliseconds)(LvnSound* sound, uint64_t ms);
typedef void                     (*PFN_lvnSoundSetFadeMilliseconds)(LvnSound* sound, float volBegin, float volEnd, uint64_t ms);
typedef void                     (*PFN_lvnSoundSetFadePcmFrames)(LvnSound* sound, float volBegin, float volEnd, uint64_t pcm);
typedef void                     (*PFN_lvnSoundSeekToPcmFrame)(LvnSound* sound, uint64_t pcm);
typedef float                    (*PFN_lvnSoundGetVolume)(const LvnSound* sound);
typedef float                    (*PFN_lvnSoundGetPan)(const LvnSound* sound);
typedef float                    (*PFN_lvnSoundGetPitch)(const LvnSound* sound);
typedef LvnSoundPositioning      (*PFN_lvnSoundGetPositioning)(const LvnSound* sound);
typedef void                     (*PFN_lvnSoundGetPosition)(const LvnSound* sound, float* x, float* y, float* z);
typedef void                     (*PFN_lvnSoundGetDirection)(const LvnSound* sound, float* x, float* y, float* z);
typedef void                     (*PFN_lvnSoundGetVelocity)(const LvnSound* sound, float* x, float* y, float* z);
typedef void                     (*PFN_lvnSoundGetCone)(const LvnSound* sound, float* innerAngleRad, float* outerAngleRad, float* outerGain);
typedef LvnSoundAttenuation      (*PFN_lvnSoundGetAttenuation)(const LvnSound* sound);
typedef float                    (*PFN_lvnSoundGetRolloff)(const LvnSound* sound);
typedef float                    (*PFN_lvnSoundGetMinGain)(const LvnSound* sound);
typedef float                    (*PFN_lvnSoundGetMaxGain)(const LvnSound* sound);
typedef float                    (*PFN_lvnSoundGetMinDistance)(const LvnSound* sound);
typedef float                    (*PFN_lvnSoundGetMaxDistance)(const LvnSound* sound);
typedef float                    (*PFN_lvnSoundGetDopplerFactor)(const LvnSound* sound);
typedef bool                     (*PFN_lvnSoundIsLooping)(const LvnSound* sound);
typedef bool                     (*PFN_lvnSoundIsPlaying)(const LvnSound* sound);
typedef bool                     (*PFN_lvnSoundAtEnd)(const LvnSound* sound);
typedef uint64_t                 (*PFN_lvnSoundGetTimeMilliseconds)(const LvnSound* sound);
typedef uint64_t                 (*PFN_lvnSoundGetTimePcmFrames)(const LvnSound* sound);
typedef float                    (*PFN_lvnSoundGetLengthSeconds)(LvnSound* sound);

struct LvnSoundBufferCreateInfo
{
    const char*               filepath;      // the filepath to the sound file (.wav .mp3)
};

struct LvnSoundCreateInfo
{
    const LvnSoundBuffer*    soundBuffer;    // sound buffer object
    LvnSoundFlagBits         flags;          // audio data flags
};

struct LvnAudioContextFunctions
{
    PFN_lvnAudioSetGlobalTimeMilliSeconds        audioSetGlobalTimeMilliSeconds;
    PFN_lvnAudioSetGlobalTimePcmFrames           audioSetGlobalTimePcmFrames;
    PFN_lvnAudioSetMasterVolume                  audioSetMasterVolume;
    PFN_lvnAudioGetSampleRate                    audioGetSampleRate;
    PFN_lvnAudioGetGlobalTimeMilliseconds        audioGetGlobalTimeMilliseconds;
    PFN_lvnAudioGetGlobalTimePcmFrames           audioGetGlobalTimePcmFrames;
    PFN_lvnListenerSetPosition                   listenerSetPosition;
    PFN_lvnListenerSetDirection                  listenerSetDirection;
    PFN_lvnListenerSetVelocity                   listenerSetVelocity;
    PFN_lvnListenerSetWorldUp                    listenerSetWorldUp;
    PFN_lvnListenerSetCone                       listenerSetCone;
    PFN_lvnListenerGetPosition                   listenerGetPosition;
    PFN_lvnListenerGetDirection                  listenerGetDirection;
    PFN_lvnListenerGetWorldUp                    listenerGetWorldUp;
    PFN_lvnListenerGetCone                       listenerGetCone;
    PFN_lvnCreateSoundBuffer                     createSoundBuffer;
    PFN_lvnDestroySoundBuffer                    destroySoundBuffer;
    PFN_lvnCreateSound                           createSound;
    PFN_lvnDestroySound                          destroySound;
    PFN_lvnSoundSetVolume                        soundSetVolume;
    PFN_lvnSoundSetPan                           soundSetPan;
    PFN_lvnSoundSetPitch                         soundSetPitch;
    PFN_lvnSoundSetPositioning                   soundSetPositioning;
    PFN_lvnSoundSetPosition                      soundSetPosition;
    PFN_lvnSoundSetDirection                     soundSetDirection;
    PFN_lvnSoundSetVelocity                      soundSetVelocity;
    PFN_lvnSoundSetCone                          soundSetCone;
    PFN_lvnSoundSetAttenuation                   soundSetAttenuation;
    PFN_lvnSoundSetRolloff                       soundSetRolloff;
    PFN_lvnSoundSetMinGain                       soundSetMinGain;
    PFN_lvnSoundSetMaxGain                       soundSetMaxGain;
    PFN_lvnSoundSetMinDistance                   soundSetMinDistance;
    PFN_lvnSoundSetMaxDistance                   soundSetMaxDistance;
    PFN_lvnSoundSetDopplerFactor                 soundSetDopplerFactor;
    PFN_lvnSoundSetLooping                       soundSetLooping;
    PFN_lvnSoundStart                            soundStart;
    PFN_lvnSoundStop                             soundStop;
    PFN_lvnSoundScheduleStartTimePcmFrames       soundScheduleStartTimePcmFrames;
    PFN_lvnSoundScheduleStartTimeMilliseconds    soundScheduleStartTimeMilliseconds;
    PFN_lvnSoundScheduleStopTimePcmFrames        soundScheduleStopTimePcmFrames;
    PFN_lvnSoundScheduleStopTimeMilliseconds     soundScheduleStopTimeMilliseconds;
    PFN_lvnSoundSetFadeMilliseconds              soundSetFadeMilliseconds;
    PFN_lvnSoundSetFadePcmFrames                 soundSetFadePcmFrames;
    PFN_lvnSoundSeekToPcmFrame                   soundSeekToPcmFrame;
    PFN_lvnSoundGetVolume                        soundGetVolume;
    PFN_lvnSoundGetPan                           soundGetPan;
    PFN_lvnSoundGetPitch                         soundGetPitch;
    PFN_lvnSoundGetPositioning                   soundGetPositioning;
    PFN_lvnSoundGetPosition                      soundGetPosition;
    PFN_lvnSoundGetDirection                     soundGetDirection;
    PFN_lvnSoundGetVelocity                      soundGetVelocity;
    PFN_lvnSoundGetCone                          soundGetCone;
    PFN_lvnSoundGetAttenuation                   soundGetAttenuation;
    PFN_lvnSoundGetRolloff                       soundGetRolloff;
    PFN_lvnSoundGetMinGain                       soundGetMinGain;
    PFN_lvnSoundGetMaxGain                       soundGetMaxGain;
    PFN_lvnSoundGetMinDistance                   soundGetMinDistance;
    PFN_lvnSoundGetMaxDistance                   soundGetMaxDistance;
    PFN_lvnSoundGetDopplerFactor                 soundGetDopplerFactor;
    PFN_lvnSoundIsLooping                        soundIsLooping;
    PFN_lvnSoundIsPlaying                        soundIsPlaying;
    PFN_lvnSoundAtEnd                            soundAtEnd;
    PFN_lvnSoundGetTimeMilliseconds              soundGetTimeMilliseconds;
    PFN_lvnSoundGetTimePcmFrames                 soundGetTimePcmFrames;
    PFN_lvnSoundGetLengthSeconds                 soundGetLengthSeconds;
};

struct LvnAudioContextCreateInfo
{
    LvnAudioContextFunctions*    actxFuncs;
};


#ifdef __cplusplus
extern "C" {
#endif

LVN_API LvnResult                   lvnCreateAudioContext(struct LvnContext* ctx, LvnAudioContext** audioctx, const LvnAudioContextCreateInfo* createInfo); // createa audio context
LVN_API void                        lvnDestroyAudioContext(LvnAudioContext* audioctx);                                                                      // destroy audio context

LVN_API float                       lvnVolumeDbToLinear(float db);
LVN_API float                       lvnVolumeLinearToDb(float volume);

LVN_API void                        lvnAudioSetGlobalTimeMilliSeconds(LvnAudioContext* audioctx, uint64_t ms);
LVN_API void                        lvnAudioSetGlobalTimePcmFrames(LvnAudioContext* audioctx, uint64_t pcm);
LVN_API void                        lvnAudioSetMasterVolume(LvnAudioContext* audioctx, float volume);

LVN_API uint32_t                    lvnAudioGetSampleRate(LvnAudioContext* audioctx);
LVN_API uint64_t                    lvnAudioGetGlobalTimeMilliseconds(LvnAudioContext* audioctx);
LVN_API uint64_t                    lvnAudioGetGlobalTimePcmFrames(LvnAudioContext* audioctx);

LVN_API void                        lvnListenerSetPosition(LvnAudioContext* audioctx, float x, float y, float z);
LVN_API void                        lvnListenerSetDirection(LvnAudioContext* audioctx, float x, float y, float z);
LVN_API void                        lvnListenerSetVelocity(LvnAudioContext* audioctx, float x, float y, float z);
LVN_API void                        lvnListenerSetWorldUp(LvnAudioContext* audioctx, float x, float y, float z);
LVN_API void                        lvnListenerSetCone(LvnAudioContext* audioctx, float innerAngleRad, float outerAngleRad, float outerGain);

LVN_API void                        lvnListenerGetPosition(LvnAudioContext* audioctx, float* x, float* y, float* z);
LVN_API void                        lvnListenerGetDirection(LvnAudioContext* audioctx, float* x, float* y, float* z);
LVN_API void                        lvnListenerGetWorldUp(LvnAudioContext* audioctx, float* x, float* y, float* z);
LVN_API void                        lvnListenerGetCone(LvnAudioContext* audioctx, float* innerAngleRad, float* outerAngleRad, float* outerGain);

LVN_API LvnResult                   lvnCreateSoundBuffer(const LvnAudioContext* audioctx, LvnSoundBuffer** soundBuffer, const LvnSoundBufferCreateInfo* createInfo);
LVN_API void                        lvnDestroySoundBuffer(LvnSoundBuffer* soundBuffer);

LVN_API LvnResult                   lvnCreateSound(const LvnAudioContext* audioctx, LvnSound** sound, const LvnSoundCreateInfo* createInfo);
LVN_API void                        lvnDestroySound(LvnSound* sound);

LVN_API void                        lvnSoundSetVolume(LvnSound* sound, float volume);
LVN_API void                        lvnSoundSetPan(LvnSound* sound, float pan);
LVN_API void                        lvnSoundSetPitch(LvnSound* sound, float pitch);
LVN_API void                        lvnSoundSetPositioning(LvnSound* sound, LvnSoundPositioning positioning);
LVN_API void                        lvnSoundSetPosition(LvnSound* sound, float x, float y, float z);
LVN_API void                        lvnSoundSetDirection(LvnSound* sound, float x, float y, float z);
LVN_API void                        lvnSoundSetVelocity(LvnSound* sound, float x, float y, float z);
LVN_API void                        lvnSoundSetCone(LvnSound* sound, float innerAngleRad, float outerAngleRad, float outerGain);
LVN_API void                        lvnSoundSetAttenuation(LvnSound* sound, LvnSoundAttenuation attenuation);
LVN_API void                        lvnSoundSetRolloff(LvnSound* sound, float rolloff);
LVN_API void                        lvnSoundSetMinGain(LvnSound* sound, float minGain);
LVN_API void                        lvnSoundSetMaxGain(LvnSound* sound, float maxGain);
LVN_API void                        lvnSoundSetMinDistance(LvnSound* sound, float minDist);
LVN_API void                        lvnSoundSetMaxDistance(LvnSound* sound, float maxDist);
LVN_API void                        lvnSoundSetDopplerFactor(LvnSound* sound, float dopplerFactor);
LVN_API void                        lvnSoundSetLooping(LvnSound* sound, bool looping);
LVN_API void                        lvnSoundStart(LvnSound* sound);
LVN_API void                        lvnSoundStop(LvnSound* sound);
LVN_API void                        lvnSoundScheduleStartTimePcmFrames(LvnSound* sound, uint64_t pcm);
LVN_API void                        lvnSoundScheduleStartTimeMilliseconds(LvnSound* sound, uint64_t ms);
LVN_API void                        lvnSoundScheduleStopTimePcmFrames(LvnSound* sound, uint64_t pcm);
LVN_API void                        lvnSoundScheduleStopTimeMilliseconds(LvnSound* sound, uint64_t ms);
LVN_API void                        lvnSoundSetFadeMilliseconds(LvnSound* sound, float volBegin, float volEnd, uint64_t ms);
LVN_API void                        lvnSoundSetFadePcmFrames(LvnSound* sound, float volBegin, float volEnd, uint64_t pcm);
LVN_API void                        lvnSoundSeekToPcmFrame(LvnSound* sound, uint64_t pcm);

LVN_API float                       lvnSoundGetVolume(const LvnSound* sound);
LVN_API float                       lvnSoundGetPan(const LvnSound* sound);
LVN_API float                       lvnSoundGetPitch(const LvnSound* sound);
LVN_API LvnSoundPositioning         lvnSoundGetPositioning(const LvnSound* sound);
LVN_API void                        lvnSoundGetPosition(const LvnSound* sound, float* x, float* y, float* z);
LVN_API void                        lvnSoundGetDirection(const LvnSound* sound, float* x, float* y, float* z);
LVN_API void                        lvnSoundGetVelocity(const LvnSound* sound, float* x, float* y, float* z);
LVN_API void                        lvnSoundGetCone(const LvnSound* sound, float* innerAngleRad, float* outerAngleRad, float* outerGain);
LVN_API LvnSoundAttenuation         lvnSoundGetAttenuation(const LvnSound* sound);
LVN_API float                       lvnSoundGetRolloff(const LvnSound* sound);
LVN_API float                       lvnSoundGetMinGain(const LvnSound* sound);
LVN_API float                       lvnSoundGetMaxGain(const LvnSound* sound);
LVN_API float                       lvnSoundGetMinDistance(const LvnSound* sound);
LVN_API float                       lvnSoundGetMaxDistance(const LvnSound* sound);
LVN_API float                       lvnSoundGetDopplerFactor(const LvnSound* sound);
LVN_API bool                        lvnSoundIsLooping(const LvnSound* sound);
LVN_API bool                        lvnSoundIsPlaying(const LvnSound* sound);
LVN_API bool                        lvnSoundAtEnd(const LvnSound* sound);
LVN_API uint64_t                    lvnSoundGetTimeMilliseconds(const LvnSound* sound);
LVN_API uint64_t                    lvnSoundGetTimePcmFrames(const LvnSound* sound);
LVN_API float                       lvnSoundGetLengthSeconds(LvnSound* sound);

#ifdef __cplusplus
}
#endif


#endif // !HG_LVN_AUDIO_H
