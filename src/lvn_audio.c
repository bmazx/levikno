#include "lvn_audio_internal.h"

#include <math.h>

#ifdef LVN_INCLUDE_MINIAUDIO

#define MINIAUDIO_IMPLEMENTATION
#define MA_MALLOC(sz)      lvn_calloc((sz))
#define MA_REALLOC(p, sz)  lvn_realloc((p), (sz))
#define MA_FREE(p)         lvn_free((p))
#include <miniaudio.h>


typedef struct LvnMaSoundBuffer
{
    LvnFile    audioData;
    char*      filepath;
} LvnMaSoundBuffer;

typedef struct LvnMaSound
{
    const LvnSoundBuffer* soundBuffer;
    ma_sound sound;
} LvnMaSound;

static ma_uint32                   lvn_getMaSoundFlags(LvnSoundFlagBits flags);

static ma_uint32 lvn_getMaSoundFlags(LvnSoundFlagBits flags)
{
    ma_uint32 ret = 0;
    if (flags & Lvn_SoundFlag_Stream)   { ret |= MA_SOUND_FLAG_STREAM; }
    if (flags & Lvn_SoundFlag_Decode)   { ret |= MA_SOUND_FLAG_DECODE; }
    if (flags & Lvn_SoundFlag_Async)    { ret |= MA_SOUND_FLAG_ASYNC; }
    if (flags & Lvn_SoundFlag_WaitInit) { ret |= MA_SOUND_FLAG_WAIT_INIT; }
    return ret;
}

static LvnResult                   lvn_ImplMaInitEngine(LvnAudioContext* audioctx);
static void                        lvn_ImplMaTerminateEngine(LvnAudioContext* audioctx);
static void                        lvn_ImplMaAudioSetGlobalTimeMilliSeconds(LvnAudioContext* audioctx, uint64_t ms);
static void                        lvn_ImplMaAudioSetGlobalTimePcmFrames(LvnAudioContext* audioctx, uint64_t pcm);
static void                        lvn_ImplMaAudioSetMasterVolume(LvnAudioContext* audioctx, float volume);
static uint32_t                    lvn_ImplMaAudioGetSampleRate(LvnAudioContext* audioctx);
static uint64_t                    lvn_ImplMaAudioGetGlobalTimeMilliseconds(LvnAudioContext* audioctx);
static uint64_t                    lvn_ImplMaAudioGetGlobalTimePcmFrames(LvnAudioContext* audioctx);
static void                        lvn_ImplMaListenerSetPosition(LvnAudioContext* audioctx, float x, float y, float z);
static void                        lvn_ImplMaListenerSetDirection(LvnAudioContext* audioctx, float x, float y, float z);
static void                        lvn_ImplMaListenerSetVelocity(LvnAudioContext* audioctx, float x, float y, float z);
static void                        lvn_ImplMaListenerSetWorldUp(LvnAudioContext* audioctx, float x, float y, float z);
static void                        lvn_ImplMaListenerSetCone(LvnAudioContext* audioctx, float innerAngleRad, float outerAngleRad, float outerGain);
static void                        lvn_ImplMaListenerGetPosition(LvnAudioContext* audioctx, float* x, float* y, float* z);
static void                        lvn_ImplMaListenerGetDirection(LvnAudioContext* audioctx, float* x, float* y, float* z);
static void                        lvn_ImplMaListenerGetWorldUp(LvnAudioContext* audioctx, float* x, float* y, float* z);
static void                        lvn_ImplMaListenerGetCone(LvnAudioContext* audioctx, float* innerAngleRad, float* outerAngleRad, float* outerGain);
static LvnResult                   lvn_ImplMaCreateSoundBuffer(const LvnAudioContext* audioctx, LvnSoundBuffer* soundBuffer, const LvnSoundBufferCreateInfo* createInfo);
static void                        lvn_ImplMaDestroySoundBuffer(LvnSoundBuffer* soundBuffer);
static LvnResult                   lvn_ImplMaCreateSound(const LvnAudioContext* audioctx, LvnSound* sound, const LvnSoundCreateInfo* createInfo);
static void                        lvn_ImplMaDestroySound(LvnSound* sound);
static void                        lvn_ImplMaSoundSetVolume(LvnSound* sound, float volume);
static void                        lvn_ImplMaSoundSetPan(LvnSound* sound, float pan);
static void                        lvn_ImplMaSoundSetPitch(LvnSound* sound, float pitch);
static void                        lvn_ImplMaSoundSetPositioning(LvnSound* sound, LvnSoundPositioningFlags positioning);
static void                        lvn_ImplMaSoundSetPosition(LvnSound* sound, float x, float y, float z);
static void                        lvn_ImplMaSoundSetDirection(LvnSound* sound, float x, float y, float z);
static void                        lvn_ImplMaSoundSetVelocity(LvnSound* sound, float x, float y, float z);
static void                        lvn_ImplMaSoundSetCone(LvnSound* sound, float innerAngleRad, float outerAngleRad, float outerGain);
static void                        lvn_ImplMaSoundSetAttenuation(LvnSound* sound, LvnSoundAttenuationFlags attenuation);
static void                        lvn_ImplMaSoundSetRolloff(LvnSound* sound, float rolloff);
static void                        lvn_ImplMaSoundSetMinGain(LvnSound* sound, float minGain);
static void                        lvn_ImplMaSoundSetMaxGain(LvnSound* sound, float maxGain);
static void                        lvn_ImplMaSoundSetMinDistance(LvnSound* sound, float minDist);
static void                        lvn_ImplMaSoundSetMaxDistance(LvnSound* sound, float maxDist);
static void                        lvn_ImplMaSoundSetDopplerFactor(LvnSound* sound, float dopplerFactor);
static void                        lvn_ImplMaSoundSetLooping(LvnSound* sound, bool looping);
static void                        lvn_ImplMaSoundStart(LvnSound* sound);
static void                        lvn_ImplMaSoundStop(LvnSound* sound);
static void                        lvn_ImplMaSoundScheduleStartTimePcmFrames(LvnSound* sound, uint64_t pcm);
static void                        lvn_ImplMaSoundScheduleStartTimeMilliseconds(LvnSound* sound, uint64_t ms);
static void                        lvn_ImplMaSoundScheduleStopTimePcmFrames(LvnSound* sound, uint64_t pcm);
static void                        lvn_ImplMaSoundScheduleStopTimeMilliseconds(LvnSound* sound, uint64_t ms);
static void                        lvn_ImplMaSoundSetFadeMilliseconds(LvnSound* sound, float volBegin, float volEnd, uint64_t ms);
static void                        lvn_ImplMaSoundSetFadePcmFrames(LvnSound* sound, float volBegin, float volEnd, uint64_t pcm);
static void                        lvn_ImplMaSoundSeekToPcmFrame(LvnSound* sound, uint64_t pcm);
static float                       lvn_ImplMaSoundGetVolume(const LvnSound* sound);
static float                       lvn_ImplMaSoundGetPan(const LvnSound* sound);
static float                       lvn_ImplMaSoundGetPitch(const LvnSound* sound);
static LvnSoundPositioningFlags    lvn_ImplMaSoundGetPositioning(const LvnSound* sound);
static void                        lvn_ImplMaSoundGetPosition(const LvnSound* sound, float* x, float* y, float* z);
static void                        lvn_ImplMaSoundGetDirection(const LvnSound* sound, float* x, float* y, float* z);
static void                        lvn_ImplMaSoundGetVelocity(const LvnSound* sound, float* x, float* y, float* z);
static void                        lvn_ImplMaSoundGetCone(const LvnSound* sound, float* innerAngleRad, float* outerAngleRad, float* outerGain);
static LvnSoundAttenuationFlags    lvn_ImplMaSoundGetAttenuation(const LvnSound* sound);
static float                       lvn_ImplMaSoundGetRolloff(const LvnSound* sound);
static float                       lvn_ImplMaSoundGetMinGain(const LvnSound* sound);
static float                       lvn_ImplMaSoundGetMaxGain(const LvnSound* sound);
static float                       lvn_ImplMaSoundGetMinDistance(const LvnSound* sound);
static float                       lvn_ImplMaSoundGetMaxDistance(const LvnSound* sound);
static float                       lvn_ImplMaSoundGetDopplerFactor(const LvnSound* sound);
static bool                        lvn_ImplMaSoundIsLooping(const LvnSound* sound);
static bool                        lvn_ImplMaSoundIsPlaying(const LvnSound* sound);
static bool                        lvn_ImplMaSoundAtEnd(const LvnSound* sound);
static uint64_t                    lvn_ImplMaSoundGetTimeMilliseconds(const LvnSound* sound);
static uint64_t                    lvn_ImplMaSoundGetTimePcmFrames(const LvnSound* sound);
static float                       lvn_ImplMaSoundGetLengthSeconds(LvnSound* sound);


static LvnResult lvn_ImplMaInitEngine(LvnAudioContext* audioctx)
{
    LVN_ASSERT(audioctx, "audioctx cannot be null");

    ma_engine* pEngine = (ma_engine*) lvn_calloc(sizeof(ma_engine));

    if (ma_engine_init(NULL, pEngine) != MA_SUCCESS)
    {
        LVN_LOG_ERROR(audioctx->coreLogger, "[audio] failed to initialize miniaudio engine");
        return Lvn_Result_Failure;
    }

    // set audio function symbols
    audioctx->implData = pEngine;
    audioctx->implAudioSetGlobalTimeMilliSeconds = lvn_ImplMaAudioSetGlobalTimeMilliSeconds;
    audioctx->implAudioSetGlobalTimePcmFrames = lvn_ImplMaAudioSetGlobalTimePcmFrames;
    audioctx->implAudioSetMasterVolume = lvn_ImplMaAudioSetMasterVolume;
    audioctx->implAudioGetSampleRate = lvn_ImplMaAudioGetSampleRate;
    audioctx->implAudioGetGlobalTimeMilliseconds = lvn_ImplMaAudioGetGlobalTimeMilliseconds;
    audioctx->implAudioGetGlobalTimePcmFrames = lvn_ImplMaAudioGetGlobalTimePcmFrames;
    audioctx->implListenerSetPosition = lvn_ImplMaListenerSetPosition;
    audioctx->implListenerSetDirection = lvn_ImplMaListenerSetDirection;
    audioctx->implListenerSetVelocity = lvn_ImplMaListenerSetVelocity;
    audioctx->implListenerSetWorldUp = lvn_ImplMaListenerSetWorldUp;
    audioctx->implListenerSetCone = lvn_ImplMaListenerSetCone;
    audioctx->implListenerGetPosition = lvn_ImplMaListenerGetPosition;
    audioctx->implListenerGetDirection = lvn_ImplMaListenerGetDirection;
    audioctx->implListenerGetWorldUp = lvn_ImplMaListenerGetWorldUp;
    audioctx->implListenerGetCone = lvn_ImplMaListenerGetCone;
    audioctx->implCreateSoundBuffer = lvn_ImplMaCreateSoundBuffer;
    audioctx->implDestroySoundBuffer = lvn_ImplMaDestroySoundBuffer;
    audioctx->implCreateSound = lvn_ImplMaCreateSound;
    audioctx->implDestroySound = lvn_ImplMaDestroySound;
    audioctx->implSoundSetVolume = lvn_ImplMaSoundSetVolume;
    audioctx->implSoundSetPan = lvn_ImplMaSoundSetPan;
    audioctx->implSoundSetPitch = lvn_ImplMaSoundSetPitch;
    audioctx->implSoundSetPositioning = lvn_ImplMaSoundSetPositioning;
    audioctx->implSoundSetPosition = lvn_ImplMaSoundSetPosition;
    audioctx->implSoundSetDirection = lvn_ImplMaSoundSetDirection;
    audioctx->implSoundSetVelocity = lvn_ImplMaSoundSetVelocity;
    audioctx->implSoundSetCone = lvn_ImplMaSoundSetCone;
    audioctx->implSoundSetAttenuation = lvn_ImplMaSoundSetAttenuation;
    audioctx->implSoundSetRolloff = lvn_ImplMaSoundSetRolloff;
    audioctx->implSoundSetMinGain = lvn_ImplMaSoundSetMinGain;
    audioctx->implSoundSetMaxGain = lvn_ImplMaSoundSetMaxGain;
    audioctx->implSoundSetMinDistance = lvn_ImplMaSoundSetMinDistance;
    audioctx->implSoundSetMaxDistance = lvn_ImplMaSoundSetMaxDistance;
    audioctx->implSoundSetDopplerFactor = lvn_ImplMaSoundSetDopplerFactor;
    audioctx->implSoundSetLooping = lvn_ImplMaSoundSetLooping;
    audioctx->implSoundStart = lvn_ImplMaSoundStart;
    audioctx->implSoundStop = lvn_ImplMaSoundStop;
    audioctx->implSoundScheduleStartTimePcmFrames = lvn_ImplMaSoundScheduleStartTimePcmFrames;
    audioctx->implSoundScheduleStartTimeMilliseconds = lvn_ImplMaSoundScheduleStartTimeMilliseconds;
    audioctx->implSoundScheduleStopTimePcmFrames = lvn_ImplMaSoundScheduleStopTimePcmFrames;
    audioctx->implSoundScheduleStopTimeMilliseconds = lvn_ImplMaSoundScheduleStopTimeMilliseconds;
    audioctx->implSoundSetFadeMilliseconds = lvn_ImplMaSoundSetFadeMilliseconds;
    audioctx->implSoundSetFadePcmFrames = lvn_ImplMaSoundSetFadePcmFrames;
    audioctx->implSoundSeekToPcmFrame = lvn_ImplMaSoundSeekToPcmFrame;
    audioctx->implSoundGetVolume = lvn_ImplMaSoundGetVolume;
    audioctx->implSoundGetPan = lvn_ImplMaSoundGetPan;
    audioctx->implSoundGetPitch = lvn_ImplMaSoundGetPitch;
    audioctx->implSoundGetPositioning = lvn_ImplMaSoundGetPositioning;
    audioctx->implSoundGetPosition = lvn_ImplMaSoundGetPosition;
    audioctx->implSoundGetDirection = lvn_ImplMaSoundGetDirection;
    audioctx->implSoundGetVelocity = lvn_ImplMaSoundGetVelocity;
    audioctx->implSoundGetCone = lvn_ImplMaSoundGetCone;
    audioctx->implSoundGetAttenuation = lvn_ImplMaSoundGetAttenuation;
    audioctx->implSoundGetRolloff = lvn_ImplMaSoundGetRolloff;
    audioctx->implSoundGetMinGain = lvn_ImplMaSoundGetMinGain;
    audioctx->implSoundGetMaxGain = lvn_ImplMaSoundGetMaxGain;
    audioctx->implSoundGetMinDistance = lvn_ImplMaSoundGetMinDistance;
    audioctx->implSoundGetMaxDistance = lvn_ImplMaSoundGetMaxDistance;
    audioctx->implSoundGetDopplerFactor = lvn_ImplMaSoundGetDopplerFactor;
    audioctx->implSoundIsLooping = lvn_ImplMaSoundIsLooping;
    audioctx->implSoundIsPlaying = lvn_ImplMaSoundIsPlaying;
    audioctx->implSoundAtEnd = lvn_ImplMaSoundAtEnd;
    audioctx->implSoundGetTimeMilliseconds = lvn_ImplMaSoundGetTimeMilliseconds;
    audioctx->implSoundGetTimePcmFrames = lvn_ImplMaSoundGetTimePcmFrames;
    audioctx->implSoundGetLengthSeconds = lvn_ImplMaSoundGetLengthSeconds;

    return Lvn_Result_Success;
}

static void lvn_ImplMaTerminateEngine(LvnAudioContext* audioctx)
{
    LVN_ASSERT(audioctx, "audioctx cannot be null");

    if (audioctx->implData)
    {
        ma_engine_uninit((ma_engine*)audioctx->implData);
        lvn_free(audioctx->implData);
        audioctx->implData = NULL;
    }
}

static void lvn_ImplMaAudioSetGlobalTimeMilliSeconds(LvnAudioContext* audioctx, uint64_t ms)
{
    LVN_ASSERT(audioctx, "audioctx cannot be null");
    ma_engine* engine = (ma_engine*) audioctx->implData;
    ma_engine_set_time_in_milliseconds(engine, ms);
}

static void lvn_ImplMaAudioSetGlobalTimePcmFrames(LvnAudioContext* audioctx, uint64_t pcm)
{
    LVN_ASSERT(audioctx, "audioctx cannot be null");
    ma_engine* engine = (ma_engine*) audioctx->implData;
    ma_engine_set_time_in_pcm_frames(engine, pcm);
}

static void lvn_ImplMaAudioSetMasterVolume(LvnAudioContext* audioctx, float volume)
{
    LVN_ASSERT(audioctx, "audioctx cannot be null");
    ma_engine* engine = (ma_engine*) audioctx->implData;
    ma_engine_set_volume(engine, volume);
}

static uint32_t lvn_ImplMaAudioGetSampleRate(LvnAudioContext* audioctx)
{
    LVN_ASSERT(audioctx, "audioctx cannot be null");
    ma_engine* engine = (ma_engine*) audioctx->implData;
    return ma_engine_get_sample_rate(engine);
}

static uint64_t lvn_ImplMaAudioGetGlobalTimeMilliseconds(LvnAudioContext* audioctx)
{
    LVN_ASSERT(audioctx, "audioctx cannot be null");
    ma_engine* engine = (ma_engine*) audioctx->implData;
    return ma_engine_get_time_in_milliseconds(engine);
}

static uint64_t lvn_ImplMaAudioGetGlobalTimePcmFrames(LvnAudioContext* audioctx)
{
    LVN_ASSERT(audioctx, "audioctx cannot be null");
    ma_engine* engine = (ma_engine*) audioctx->implData;
    return ma_engine_get_time_in_pcm_frames(engine);
}

static void lvn_ImplMaListenerSetPosition(LvnAudioContext* audioctx, float x, float y, float z)
{
    LVN_ASSERT(audioctx, "audioctx cannot be null");
    ma_engine* engine = (ma_engine*) audioctx->implData;
    ma_engine_listener_set_position(engine, 0, x, y, z);
}

static void lvn_ImplMaListenerSetDirection(LvnAudioContext* audioctx, float x, float y, float z)
{
    LVN_ASSERT(audioctx, "audioctx cannot be null");
    ma_engine* engine = (ma_engine*) audioctx->implData;
    ma_engine_listener_set_direction(engine, 0, x, y, z);
}

static void lvn_ImplMaListenerSetVelocity(LvnAudioContext* audioctx, float x, float y, float z)
{
    LVN_ASSERT(audioctx, "audioctx cannot be null");
    ma_engine* engine = (ma_engine*) audioctx->implData;
    ma_engine_listener_set_velocity(engine, 0, x, y, z);
}

static void lvn_ImplMaListenerSetWorldUp(LvnAudioContext* audioctx, float x, float y, float z)
{
    LVN_ASSERT(audioctx, "audioctx cannot be null");
    ma_engine* engine = (ma_engine*) audioctx->implData;
    ma_engine_listener_set_world_up(engine, 0, x, y, z);
}

static void lvn_ImplMaListenerSetCone(LvnAudioContext* audioctx, float innerAngleRad, float outerAngleRad, float outerGain)
{
    LVN_ASSERT(audioctx, "audioctx cannot be null");
    ma_engine* engine = (ma_engine*) audioctx->implData;
    ma_engine_listener_set_cone(engine, 0, innerAngleRad, outerAngleRad, outerGain);
}

static void lvn_ImplMaListenerGetPosition(LvnAudioContext* audioctx, float* x, float* y, float* z)
{
    LVN_ASSERT(audioctx && x && y && z, "audioctx, x, y, and z cannot be null");
    ma_engine* engine = (ma_engine*) audioctx->implData;
    ma_vec3f vec = ma_engine_listener_get_position(engine, 0);
    *x = vec.x;
    *y = vec.y;
    *z = vec.z;
}

static void lvn_ImplMaListenerGetDirection(LvnAudioContext* audioctx, float* x, float* y, float* z)
{
    LVN_ASSERT(audioctx, "audioctx cannot be null");
    ma_engine* engine = (ma_engine*) audioctx->implData;
    ma_vec3f vec = ma_engine_listener_get_direction(engine, 0);
    *x = vec.x;
    *y = vec.y;
    *z = vec.z;
}

static void lvn_ImplMaListenerGetWorldUp(LvnAudioContext* audioctx, float* x, float* y, float* z)
{
    LVN_ASSERT(audioctx, "audioctx cannot be null");
    ma_engine* engine = (ma_engine*) audioctx->implData;
    ma_vec3f vec = ma_engine_listener_get_world_up(engine, 0);
    *x = vec.x;
    *y = vec.y;
    *z = vec.z;
}

static void lvn_ImplMaListenerGetCone(LvnAudioContext* audioctx, float* innerAngleRad, float* outerAngleRad, float* outerGain)
{
    LVN_ASSERT(audioctx, "audioctx cannot be null");
    ma_engine* engine = (ma_engine*) audioctx->implData;
    ma_engine_listener_get_cone(engine, 0, innerAngleRad, outerAngleRad, outerGain);
}

static LvnResult lvn_ImplMaCreateSoundBuffer(const LvnAudioContext* audioctx, LvnSoundBuffer* soundBuffer, const LvnSoundBufferCreateInfo* createInfo)
{
    LVN_ASSERT(audioctx && soundBuffer && createInfo, "audioctx, soundBuffer, and, createInfo cannot be null");

    ma_engine* engine = (ma_engine*) audioctx->implData;
    LvnResult errResult = Lvn_Result_Failure;
    LvnMaSoundBuffer* maSoundBuffer = NULL;
    ma_resource_manager* resourceManager = NULL;

    maSoundBuffer = (LvnMaSoundBuffer*) lvn_calloc(sizeof(LvnMaSoundBuffer));
    if (!maSoundBuffer)
    {
        LVN_LOG_ERROR(audioctx->coreLogger,
                      "[miniaudio] failed to allocate memory for miniaudio sound buffer struct in sound buffer at %p",
                      soundBuffer);
        errResult = Lvn_Result_OutOfMemory;
        goto fail_cleanup;
    }

    maSoundBuffer->audioData = lvnLoadFileBin(createInfo->filepath);
    if (!maSoundBuffer->audioData.data)
    {
        LVN_LOG_ERROR(audioctx->coreLogger,
                      "[miniaudio] failed to load encoded audio source data for sound buffer at %p from filepath: %s",
                      soundBuffer, createInfo->filepath);
        goto fail_cleanup;
    }

    maSoundBuffer->filepath = lvn_strdup(createInfo->filepath);
    if (!maSoundBuffer->filepath)
    {
        LVN_LOG_ERROR(audioctx->coreLogger,
                      "[miniaudio] failed to allocate memory for filepath string at %p, filepath: %s",
                      soundBuffer, createInfo->filepath);
        errResult = Lvn_Result_OutOfMemory;
        goto fail_cleanup;
    }

    // encode audio data
    resourceManager = ma_engine_get_resource_manager(engine);
    if (!resourceManager)
    {
        LVN_LOG_ERROR(audioctx->coreLogger,
                      "[miniaudio] failed to retrieve resource manager for soundBuffer at %p",
                      soundBuffer);
        goto fail_cleanup;
    }

    if (ma_resource_manager_register_encoded_data(resourceManager, maSoundBuffer->filepath, maSoundBuffer->audioData.data, maSoundBuffer->audioData.size) != MA_SUCCESS)
    {
        LVN_LOG_ERROR(audioctx->coreLogger,
                      "[miniaudio] failed to register encoded data for soundBuffer at %p",
                      soundBuffer);
        goto fail_cleanup;
    }

    soundBuffer->soundBufferData = maSoundBuffer;

    return Lvn_Result_Success;

fail_cleanup:
    ma_resource_manager_unregister_data(resourceManager, maSoundBuffer->filepath);
    lvn_free(maSoundBuffer->filepath);
    lvnUnloadFile(&maSoundBuffer->audioData);
    lvn_free(maSoundBuffer);
    return errResult;
}

static void lvn_ImplMaDestroySoundBuffer(LvnSoundBuffer* soundBuffer)
{
    LVN_ASSERT(soundBuffer, "soundBuffer cannot be null");
    ma_engine* engine = (ma_engine*) soundBuffer->audioctx->implData;
    LvnMaSoundBuffer* maSoundBuffer = (LvnMaSoundBuffer*) soundBuffer->soundBufferData;
    ma_resource_manager* resourceManager = ma_engine_get_resource_manager(engine);

    ma_resource_manager_unregister_data(resourceManager, maSoundBuffer->filepath);
    lvn_free(maSoundBuffer->filepath);
    lvnUnloadFile(&maSoundBuffer->audioData);
    lvn_free(maSoundBuffer);
    soundBuffer->soundBufferData = NULL;
}

static LvnResult lvn_ImplMaCreateSound(const LvnAudioContext* audioctx, LvnSound* sound, const LvnSoundCreateInfo* createInfo)
{
    LVN_ASSERT(audioctx, "audioctx cannot be null");
    const LvnMaSoundBuffer* maSoundBuffer = (const LvnMaSoundBuffer*) createInfo->soundBuffer->soundBufferData;
    ma_engine* engine = (ma_engine*) audioctx->implData;

    LvnResult errResult = Lvn_Result_Failure;
    LvnMaSound* maSound = NULL;

    maSound = (LvnMaSound*) lvn_calloc(sizeof(LvnMaSound));
    if (!maSound)
    {
        LVN_LOG_ERROR(audioctx->coreLogger,
                      "[miniaudio] failed to allocate memory for miniaudio sound struct at %p",
                      sound);
        errResult = Lvn_Result_OutOfMemory;
        goto fail_cleanup;
    }

    maSound->soundBuffer = createInfo->soundBuffer;

    if (ma_sound_init_from_file(engine, maSoundBuffer->filepath, lvn_getMaSoundFlags(createInfo->flags), NULL, NULL, &maSound->sound) != MA_SUCCESS)
    {
        LVN_LOG_ERROR(audioctx->coreLogger,
                      "[miniaudio] failed to create minisudio sound for sound object at %p",
                      sound);
        goto fail_cleanup;
    }


    sound->soundData = maSound;

    return Lvn_Result_Success;

fail_cleanup:
    ma_sound_uninit(&maSound->sound);
    lvn_free(maSound);
    return errResult;
}

static void lvn_ImplMaDestroySound(LvnSound* sound)
{
    if (!sound) { return; }
    LvnMaSound* maSound = (LvnMaSound*) sound->soundData;
    ma_sound_uninit(&maSound->sound);
    lvn_free(maSound);
    sound->soundData = NULL;
}

static void lvn_ImplMaSoundSetVolume(LvnSound* sound, float volume)
{

}

static void lvn_ImplMaSoundSetPan(LvnSound* sound, float pan)
{

}

static void lvn_ImplMaSoundSetPitch(LvnSound* sound, float pitch)
{

}

static void lvn_ImplMaSoundSetPositioning(LvnSound* sound, LvnSoundPositioningFlags positioning)
{

}

static void lvn_ImplMaSoundSetPosition(LvnSound* sound, float x, float y, float z)
{

}

static void lvn_ImplMaSoundSetDirection(LvnSound* sound, float x, float y, float z)
{

}

static void lvn_ImplMaSoundSetVelocity(LvnSound* sound, float x, float y, float z)
{

}

static void lvn_ImplMaSoundSetCone(LvnSound* sound, float innerAngleRad, float outerAngleRad, float outerGain)
{

}

static void lvn_ImplMaSoundSetAttenuation(LvnSound* sound, LvnSoundAttenuationFlags attenuation)
{

}

static void lvn_ImplMaSoundSetRolloff(LvnSound* sound, float rolloff)
{

}

static void lvn_ImplMaSoundSetMinGain(LvnSound* sound, float minGain)
{

}

static void lvn_ImplMaSoundSetMaxGain(LvnSound* sound, float maxGain)
{

}

static void lvn_ImplMaSoundSetMinDistance(LvnSound* sound, float minDist)
{

}

static void lvn_ImplMaSoundSetMaxDistance(LvnSound* sound, float maxDist)
{

}

static void lvn_ImplMaSoundSetDopplerFactor(LvnSound* sound, float dopplerFactor)
{

}

static void lvn_ImplMaSoundSetLooping(LvnSound* sound, bool looping)
{

}

static void lvn_ImplMaSoundStart(LvnSound* sound)
{

}

static void lvn_ImplMaSoundStop(LvnSound* sound)
{

}

static void lvn_ImplMaSoundScheduleStartTimePcmFrames(LvnSound* sound, uint64_t pcm)
{

}

static void lvn_ImplMaSoundScheduleStartTimeMilliseconds(LvnSound* sound, uint64_t ms)
{

}

static void lvn_ImplMaSoundScheduleStopTimePcmFrames(LvnSound* sound, uint64_t pcm)
{

}

static void lvn_ImplMaSoundScheduleStopTimeMilliseconds(LvnSound* sound, uint64_t ms)
{

}

static void lvn_ImplMaSoundSetFadeMilliseconds(LvnSound* sound, float volBegin, float volEnd, uint64_t ms)
{

}

static void lvn_ImplMaSoundSetFadePcmFrames(LvnSound* sound, float volBegin, float volEnd, uint64_t pcm)
{

}

static void lvn_ImplMaSoundSeekToPcmFrame(LvnSound* sound, uint64_t pcm)
{

}

static float lvn_ImplMaSoundGetVolume(const LvnSound* sound)
{

}

static float lvn_ImplMaSoundGetPan(const LvnSound* sound)
{

}

static float lvn_ImplMaSoundGetPitch(const LvnSound* sound)
{

}

static LvnSoundPositioningFlags lvn_ImplMaSoundGetPositioning(const LvnSound* sound)
{

}

static void lvn_ImplMaSoundGetPosition(const LvnSound* sound, float* x, float* y, float* z)
{

}

static void lvn_ImplMaSoundGetDirection(const LvnSound* sound, float* x, float* y, float* z)
{

}

static void lvn_ImplMaSoundGetVelocity(const LvnSound* sound, float* x, float* y, float* z)
{

}

static void lvn_ImplMaSoundGetCone(const LvnSound* sound, float* innerAngleRad, float* outerAngleRad, float* outerGain)
{

}

static LvnSoundAttenuationFlags lvn_ImplMaSoundGetAttenuation(const LvnSound* sound)
{

}

static float lvn_ImplMaSoundGetRolloff(const LvnSound* sound)
{

}

static float lvn_ImplMaSoundGetMinGain(const LvnSound* sound)
{

}

static float lvn_ImplMaSoundGetMaxGain(const LvnSound* sound)
{

}

static float lvn_ImplMaSoundGetMinDistance(const LvnSound* sound)
{

}

static float lvn_ImplMaSoundGetMaxDistance(const LvnSound* sound)
{

}

static float lvn_ImplMaSoundGetDopplerFactor(const LvnSound* sound)
{

}

static bool lvn_ImplMaSoundIsLooping(const LvnSound* sound)
{

}

static bool lvn_ImplMaSoundIsPlaying(const LvnSound* sound)
{

}

static bool lvn_ImplMaSoundAtEnd(const LvnSound* sound)
{

}

static uint64_t lvn_ImplMaSoundGetTimeMilliseconds(const LvnSound* sound)
{

}

static uint64_t lvn_ImplMaSoundGetTimePcmFrames(const LvnSound* sound)
{

}

static float lvn_ImplMaSoundGetLengthSeconds(LvnSound* sound)
{

}

#endif

static LvnResult lvn_setCustomAudioContextPfn(LvnAudioContext* audioctx, const LvnAudioContextCreateInfo* createInfo);

static LvnResult lvn_setCustomAudioContextPfn(LvnAudioContext* audioctx, const LvnAudioContextCreateInfo* createInfo)
{
    LVN_ASSERT(audioctx, "audioctx cannot be null");

    if (!createInfo->actxFuncs)
        return Lvn_Result_Failure;

    audioctx->implAudioSetGlobalTimeMilliSeconds = createInfo->actxFuncs->audioSetGlobalTimeMilliSeconds;
    audioctx->implAudioSetGlobalTimePcmFrames = createInfo->actxFuncs->audioSetGlobalTimePcmFrames;
    audioctx->implAudioSetMasterVolume = createInfo->actxFuncs->audioSetMasterVolume;
    audioctx->implAudioGetSampleRate = createInfo->actxFuncs->audioGetSampleRate;
    audioctx->implAudioGetGlobalTimeMilliseconds = createInfo->actxFuncs->audioGetGlobalTimeMilliseconds;
    audioctx->implAudioGetGlobalTimePcmFrames = createInfo->actxFuncs->audioGetGlobalTimePcmFrames;
    audioctx->implListenerSetPosition = createInfo->actxFuncs->listenerSetPosition;
    audioctx->implListenerSetDirection = createInfo->actxFuncs->listenerSetDirection;
    audioctx->implListenerSetVelocity = createInfo->actxFuncs->listenerSetVelocity;
    audioctx->implListenerSetWorldUp = createInfo->actxFuncs->listenerSetWorldUp;
    audioctx->implListenerSetCone = createInfo->actxFuncs->listenerSetCone;
    audioctx->implListenerGetPosition = createInfo->actxFuncs->listenerGetPosition;
    audioctx->implListenerGetDirection = createInfo->actxFuncs->listenerGetDirection;
    audioctx->implListenerGetWorldUp = createInfo->actxFuncs->listenerGetWorldUp;
    audioctx->implListenerGetCone = createInfo->actxFuncs->listenerGetCone;
    audioctx->implCreateSoundBuffer = createInfo->actxFuncs->createSoundBuffer;
    audioctx->implDestroySoundBuffer = createInfo->actxFuncs->destroySoundBuffer;
    audioctx->implCreateSound = createInfo->actxFuncs->createSound;
    audioctx->implDestroySound = createInfo->actxFuncs->destroySound;
    audioctx->implSoundSetVolume = createInfo->actxFuncs->soundSetVolume;
    audioctx->implSoundSetPan = createInfo->actxFuncs->soundSetPan;
    audioctx->implSoundSetPitch = createInfo->actxFuncs->soundSetPitch;
    audioctx->implSoundSetPositioning = createInfo->actxFuncs->soundSetPositioning;
    audioctx->implSoundSetPosition = createInfo->actxFuncs->soundSetPosition;
    audioctx->implSoundSetDirection = createInfo->actxFuncs->soundSetDirection;
    audioctx->implSoundSetVelocity = createInfo->actxFuncs->soundSetVelocity;
    audioctx->implSoundSetCone = createInfo->actxFuncs->soundSetCone;
    audioctx->implSoundSetAttenuation = createInfo->actxFuncs->soundSetAttenuation;
    audioctx->implSoundSetRolloff = createInfo->actxFuncs->soundSetRolloff;
    audioctx->implSoundSetMinGain = createInfo->actxFuncs->soundSetMinGain;
    audioctx->implSoundSetMaxGain = createInfo->actxFuncs->soundSetMaxGain;
    audioctx->implSoundSetMinDistance = createInfo->actxFuncs->soundSetMinDistance;
    audioctx->implSoundSetMaxDistance = createInfo->actxFuncs->soundSetMaxDistance;
    audioctx->implSoundSetDopplerFactor = createInfo->actxFuncs->soundSetDopplerFactor;
    audioctx->implSoundSetLooping = createInfo->actxFuncs->soundSetLooping;
    audioctx->implSoundStart = createInfo->actxFuncs->soundStart;
    audioctx->implSoundStop = createInfo->actxFuncs->soundStop;
    audioctx->implSoundScheduleStartTimePcmFrames = createInfo->actxFuncs->soundScheduleStartTimePcmFrames;
    audioctx->implSoundScheduleStartTimeMilliseconds = createInfo->actxFuncs->soundScheduleStartTimeMilliseconds;
    audioctx->implSoundScheduleStopTimePcmFrames = createInfo->actxFuncs->soundScheduleStopTimePcmFrames;
    audioctx->implSoundScheduleStopTimeMilliseconds = createInfo->actxFuncs->soundScheduleStopTimeMilliseconds;
    audioctx->implSoundSetFadeMilliseconds = createInfo->actxFuncs->soundSetFadeMilliseconds;
    audioctx->implSoundSetFadePcmFrames = createInfo->actxFuncs->soundSetFadePcmFrames;
    audioctx->implSoundSeekToPcmFrame = createInfo->actxFuncs->soundSeekToPcmFrame;
    audioctx->implSoundGetVolume = createInfo->actxFuncs->soundGetVolume;
    audioctx->implSoundGetPan = createInfo->actxFuncs->soundGetPan;
    audioctx->implSoundGetPitch = createInfo->actxFuncs->soundGetPitch;
    audioctx->implSoundGetPositioning = createInfo->actxFuncs->soundGetPositioning;
    audioctx->implSoundGetPosition = createInfo->actxFuncs->soundGetPosition;
    audioctx->implSoundGetDirection = createInfo->actxFuncs->soundGetDirection;
    audioctx->implSoundGetVelocity = createInfo->actxFuncs->soundGetVelocity;
    audioctx->implSoundGetCone = createInfo->actxFuncs->soundGetCone;
    audioctx->implSoundGetAttenuation = createInfo->actxFuncs->soundGetAttenuation;
    audioctx->implSoundGetRolloff = createInfo->actxFuncs->soundGetRolloff;
    audioctx->implSoundGetMinGain = createInfo->actxFuncs->soundGetMinGain;
    audioctx->implSoundGetMaxGain = createInfo->actxFuncs->soundGetMaxGain;
    audioctx->implSoundGetMinDistance = createInfo->actxFuncs->soundGetMinDistance;
    audioctx->implSoundGetMaxDistance = createInfo->actxFuncs->soundGetMaxDistance;
    audioctx->implSoundGetDopplerFactor = createInfo->actxFuncs->soundGetDopplerFactor;
    audioctx->implSoundIsLooping = createInfo->actxFuncs->soundIsLooping;
    audioctx->implSoundIsPlaying = createInfo->actxFuncs->soundIsPlaying;
    audioctx->implSoundAtEnd = createInfo->actxFuncs->soundAtEnd;
    audioctx->implSoundGetTimeMilliseconds = createInfo->actxFuncs->soundGetTimeMilliseconds;
    audioctx->implSoundGetTimePcmFrames = createInfo->actxFuncs->soundGetTimePcmFrames;
    audioctx->implSoundGetLengthSeconds = createInfo->actxFuncs->soundGetLengthSeconds;

    return Lvn_Result_Success;
}

LvnResult lvnCreateAudioContext(struct LvnContext* ctx, LvnAudioContext** audioctx, const LvnAudioContextCreateInfo* createInfo)
{
    LVN_ASSERT(ctx && audioctx && createInfo, "ctx, audioctx, and createInfo cannot be null");

    LvnResult errResult = Lvn_Result_Failure;

    // create and init audio context
    *audioctx = (LvnAudioContext*) lvn_calloc(sizeof(LvnAudioContext));

    if (!*audioctx)
    {
        LVN_LOG_ERROR(&ctx->coreLogger, "[audio] failed to allocate memory for graphics context");
        errResult = Lvn_Result_OutOfMemory;
        goto fail_cleanup;
    }

    LvnAudioContext* actxPtr = *audioctx;
    actxPtr->ctx = ctx;
    actxPtr->coreLogger = &ctx->coreLogger;

    LvnResult result = lvn_ImplMaInitEngine(actxPtr);
    if (result != Lvn_Result_Success)
    {
        errResult = result;
        goto fail_cleanup;
    }

    LVN_LOG_TRACE(&ctx->coreLogger, "audio context initialized");

    return Lvn_Result_Success;

fail_cleanup:
    if (*audioctx)
    {
        lvnDestroyAudioContext(*audioctx);
        *audioctx = NULL;
    }
    return errResult;
}

void lvnDestroyAudioContext(LvnAudioContext* audioctx)
{
    LVN_ASSERT(audioctx, "audioctx cannot be null");

    const LvnContext* ctx = audioctx->ctx;

    lvn_ImplMaTerminateEngine(audioctx);
    lvn_free(audioctx);

    LVN_LOG_TRACE(&ctx->coreLogger, "audio context terminated: (%p)", audioctx);
}

float lvnVolumeDbToLinear(float db)
{
    return powf(10, db / 20.0f);
}

float lvnVolumeLinearToDb(float volume)
{
    return 20 * log10f(volume);
}

void lvnAudioSetGlobalTimeMilliSeconds(LvnAudioContext* audioctx, uint64_t ms)
{
    LVN_ASSERT(audioctx, "audioctx cannot be null");
    audioctx->implAudioSetGlobalTimeMilliSeconds(audioctx, ms);
}

void lvnAudioSetGlobalTimePcmFrames(LvnAudioContext* audioctx, uint64_t pcm)
{
    LVN_ASSERT(audioctx, "audioctx cannot be null");
    audioctx->implAudioSetGlobalTimePcmFrames(audioctx, pcm);
}

void lvnAudioSetMasterVolume(LvnAudioContext* audioctx, float volume)
{
    LVN_ASSERT(audioctx, "audioctx cannot be null");
    audioctx->implAudioSetMasterVolume(audioctx, volume);
}

uint32_t lvnAudioGetSampleRate(LvnAudioContext* audioctx)
{
    LVN_ASSERT(audioctx, "audioctx cannot be null");
    return audioctx->implAudioGetSampleRate(audioctx);
}

uint64_t lvnAudioGetGlobalTimeMilliseconds(LvnAudioContext* audioctx)
{
    LVN_ASSERT(audioctx, "audioctx cannot be null");
    return audioctx->implAudioGetGlobalTimeMilliseconds(audioctx);
}

uint64_t lvnAudioGetGlobalTimePcmFrames(LvnAudioContext* audioctx)
{
    LVN_ASSERT(audioctx, "audioctx cannot be null");
    return audioctx->implAudioGetGlobalTimePcmFrames(audioctx);
}

void lvnListenerSetPosition(LvnAudioContext* audioctx, float x, float y, float z)
{
    LVN_ASSERT(audioctx, "audioctx cannot be null");
    audioctx->implListenerSetPosition(audioctx, x, y, z);
}

void lvnListenerSetDirection(LvnAudioContext* audioctx, float x, float y, float z)
{
    LVN_ASSERT(audioctx, "audioctx cannot be null");
    audioctx->implListenerSetDirection(audioctx, x, y, z);
}

void lvnListenerSetVelocity(LvnAudioContext* audioctx, float x, float y, float z)
{
    LVN_ASSERT(audioctx, "audioctx cannot be null");
    audioctx->implListenerSetVelocity(audioctx, x, y, z);
}

void lvnListenerSetWorldUp(LvnAudioContext* audioctx, float x, float y, float z)
{
    LVN_ASSERT(audioctx, "audioctx cannot be null");
    audioctx->implListenerSetWorldUp(audioctx, x, y, z);
}

void lvnListenerSetCone(LvnAudioContext* audioctx, float innerAngleRad, float outerAngleRad, float outerGain)
{
    LVN_ASSERT(audioctx, "audioctx cannot be null");
    audioctx->implListenerSetCone(audioctx, innerAngleRad, outerAngleRad, outerGain);
}

void lvnListenerGetPosition(LvnAudioContext* audioctx, float* x, float* y, float* z)
{
    LVN_ASSERT(audioctx && x && y && z, "audioctx, x, y, and z cannot be null");
    audioctx->implListenerGetPosition(audioctx, x, y, z);
}

void lvnListenerGetDirection(LvnAudioContext* audioctx, float* x, float* y, float* z)
{
    LVN_ASSERT(audioctx && x && y && z, "audioctx, x, y, and z cannot be null");
    audioctx->implListenerGetDirection(audioctx, x, y, z);
}

void lvnListenerGetWorldUp(LvnAudioContext* audioctx, float* x, float* y, float* z)
{
    LVN_ASSERT(audioctx && x && y && z, "audioctx, x, y, and z cannot be null");
    audioctx->implListenerGetWorldUp(audioctx, x, y, z);
}

void lvnListenerGetCone(LvnAudioContext* audioctx, float* innerAngleRad, float* outerAngleRad, float* outerGain)
{
    LVN_ASSERT(audioctx && innerAngleRad && outerAngleRad && outerGain, "audioctx, innerAngleRad, outerAngleRad, and outerGain cannot be null");
    audioctx->implListenerGetCone(audioctx, innerAngleRad, outerAngleRad, outerGain);
}

LvnResult lvnCreateSoundBuffer(const LvnAudioContext* audioctx, LvnSoundBuffer** soundBuffer, const LvnSoundBufferCreateInfo* createInfo)
{
    LVN_ASSERT(audioctx && soundBuffer && createInfo, "audioctx, soundBuffer, and createInfo cannot be null");

    LvnResult errResult = Lvn_Result_Failure;
    *soundBuffer = NULL;

    *soundBuffer = (LvnSoundBuffer*) lvn_calloc(sizeof(LvnSoundBuffer));
    if (!*soundBuffer)
    {
        LVN_LOG_ERROR(audioctx->coreLogger,
                      "failed to allocate memory for sound buffer at %p",
                      soundBuffer);
        errResult = Lvn_Result_OutOfMemory;
        goto fail_cleanup;
    }

    LvnSoundBuffer* soundBufferPtr = *soundBuffer;
    soundBufferPtr->audioctx = audioctx;

    LvnResult result = audioctx->implCreateSoundBuffer(audioctx, *soundBuffer, createInfo);
    if (result != Lvn_Result_Success)
    {
        LVN_LOG_ERROR(audioctx->coreLogger,
                      "failed to create sound buffer at %p",
                      soundBuffer);
        errResult = result;
        goto fail_cleanup;
    }

    return Lvn_Result_Success;

fail_cleanup:
    if (*soundBuffer)
    {
        lvn_free(*soundBuffer);
        *soundBuffer = NULL;
    }
    return errResult;
}

void lvnDestroySoundBuffer(LvnSoundBuffer* soundBuffer)
{
    if (!soundBuffer) { return; }
    const LvnAudioContext* audioctx = soundBuffer->audioctx;
    audioctx->implDestroySoundBuffer(soundBuffer);
    lvn_free(soundBuffer);
}

LvnResult lvnCreateSound(const LvnAudioContext* audioctx, LvnSound** sound, const LvnSoundCreateInfo* createInfo)
{
    LVN_ASSERT(audioctx && sound && createInfo, "audioctx, sound, and createInfo cannot be null");

    LvnResult errResult = Lvn_Result_Failure;
    *sound = NULL;

    *sound = (LvnSound*) lvn_calloc(sizeof(LvnSound));
    if (!*sound)
    {
        LVN_LOG_ERROR(audioctx->coreLogger,
                      "failed to allocate memory for sound  at %p",
                      sound);
        errResult = Lvn_Result_OutOfMemory;
        goto fail_cleanup;
    }

    LvnSound* soundPtr = *sound;
    soundPtr->audioctx = audioctx;
    soundPtr->flags = createInfo->flags;

    LvnResult result = audioctx->implCreateSound(audioctx, *sound, createInfo);
    if (result != Lvn_Result_Success)
    {
        LVN_LOG_ERROR(audioctx->coreLogger,
                      "failed to create sound at %p",
                      sound);
        errResult = result;
        goto fail_cleanup;
    }

    return Lvn_Result_Success;

fail_cleanup:
    if (*sound)
    {
        lvn_free(*sound);
        *sound = NULL;
    }
    return errResult;
}

void lvnDestroySound(LvnSound* sound)
{
    if (!sound) { return; }
    const LvnAudioContext* audioctx = sound->audioctx;
    audioctx->implDestroySound(sound);
    lvn_free(sound);
}

void lvnSoundSetVolume(LvnSound* sound, float volume)
{
    LVN_ASSERT(sound, "sound cannot be nulll");
    const LvnAudioContext* audioctx = (const LvnAudioContext*) sound->audioctx;
    audioctx->implSoundSetVolume(sound, volume);
}

void lvnSoundSetPan(LvnSound* sound, float pan)
{
    LVN_ASSERT(sound, "sound cannot be nulll");
    const LvnAudioContext* audioctx = (const LvnAudioContext*) sound->audioctx;
    audioctx->implSoundSetVolume(sound, pan);
}

void lvnSoundSetPitch(LvnSound* sound, float pitch)
{
    LVN_ASSERT(sound, "sound cannot be nulll");
    const LvnAudioContext* audioctx = (const LvnAudioContext*) sound->audioctx;
    audioctx->implSoundSetPitch(sound, pitch);
}

void lvnSoundSetPositioning(LvnSound* sound, LvnSoundPositioningFlags positioning)
{
    LVN_ASSERT(sound, "sound cannot be nulll");
    const LvnAudioContext* audioctx = (const LvnAudioContext*) sound->audioctx;
    audioctx->implSoundSetPositioning(sound, positioning);
}

void lvnSoundSetPosition(LvnSound* sound, float x, float y, float z)
{
    LVN_ASSERT(sound, "sound cannot be nulll");
    const LvnAudioContext* audioctx = (const LvnAudioContext*) sound->audioctx;
    audioctx->implSoundSetPosition(sound, x, y, z);
}

void lvnSoundSetDirection(LvnSound* sound, float x, float y, float z)
{
    LVN_ASSERT(sound, "sound cannot be nulll");
    const LvnAudioContext* audioctx = (const LvnAudioContext*) sound->audioctx;
    audioctx->implSoundSetDirection(sound, x, y, z);
}

void lvnSoundSetVelocity(LvnSound* sound, float x, float y, float z)
{
    LVN_ASSERT(sound, "sound cannot be nulll");
    const LvnAudioContext* audioctx = (const LvnAudioContext*) sound->audioctx;
    audioctx->implSoundSetVelocity(sound, x, y, z);
}

void lvnSoundSetCone(LvnSound* sound, float innerAngleRad, float outerAngleRad, float outerGain)
{
    LVN_ASSERT(sound, "sound cannot be nulll");
    const LvnAudioContext* audioctx = (const LvnAudioContext*) sound->audioctx;
    audioctx->implSoundSetCone(sound, innerAngleRad, outerAngleRad, outerGain);
}

void lvnSoundSetAttenuation(LvnSound* sound, LvnSoundAttenuationFlags attenuation)
{
    LVN_ASSERT(sound, "sound cannot be nulll");
    const LvnAudioContext* audioctx = (const LvnAudioContext*) sound->audioctx;
    audioctx->implSoundSetAttenuation(sound, attenuation);
}

void lvnSoundSetRolloff(LvnSound* sound, float rolloff)
{
    LVN_ASSERT(sound, "sound cannot be nulll");
    const LvnAudioContext* audioctx = (const LvnAudioContext*) sound->audioctx;
    audioctx->implSoundSetRolloff(sound, rolloff);
}

void lvnSoundSetMinGain(LvnSound* sound, float minGain)
{
    LVN_ASSERT(sound, "sound cannot be nulll");
    const LvnAudioContext* audioctx = (const LvnAudioContext*) sound->audioctx;
    audioctx->implSoundSetMinGain(sound, minGain);
}

void lvnSoundSetMaxGain(LvnSound* sound, float maxGain)
{
    LVN_ASSERT(sound, "sound cannot be nulll");
    const LvnAudioContext* audioctx = (const LvnAudioContext*) sound->audioctx;
    audioctx->implSoundSetMaxGain(sound, maxGain);
}

void lvnSoundSetMinDistance(LvnSound* sound, float minDist)
{
    LVN_ASSERT(sound, "sound cannot be nulll");
    const LvnAudioContext* audioctx = (const LvnAudioContext*) sound->audioctx;
    audioctx->implSoundSetMinDistance(sound, minDist);
}

void lvnSoundSetMaxDistance(LvnSound* sound, float maxDist)
{
    LVN_ASSERT(sound, "sound cannot be nulll");
    const LvnAudioContext* audioctx = (const LvnAudioContext*) sound->audioctx;
    audioctx->implSoundSetMaxDistance(sound, maxDist);
}

void lvnSoundSetDopplerFactor(LvnSound* sound, float dopplerFactor)
{
    LVN_ASSERT(sound, "sound cannot be nulll");
    const LvnAudioContext* audioctx = (const LvnAudioContext*) sound->audioctx;
    audioctx->implSoundSetDopplerFactor(sound, dopplerFactor);
}

void lvnSoundSetLooping(LvnSound* sound, bool looping)
{
    LVN_ASSERT(sound, "sound cannot be nulll");
    const LvnAudioContext* audioctx = (const LvnAudioContext*) sound->audioctx;
    audioctx->implSoundSetLooping(sound, looping);
}

void lvnSoundPlayStart(LvnSound* sound)
{
    LVN_ASSERT(sound, "sound cannot be nulll");
    const LvnAudioContext* audioctx = (const LvnAudioContext*) sound->audioctx;
    audioctx->implSoundStart(sound);
}

void lvnSoundPlayStop(LvnSound* sound)
{
    LVN_ASSERT(sound, "sound cannot be nulll");
    const LvnAudioContext* audioctx = (const LvnAudioContext*) sound->audioctx;
    audioctx->implSoundStop(sound);
}

void lvnSoundScheduleStartTimePcmFrames(LvnSound* sound, uint64_t pcm)
{
    LVN_ASSERT(sound, "sound cannot be nulll");
    const LvnAudioContext* audioctx = (const LvnAudioContext*) sound->audioctx;
    audioctx->implSoundScheduleStartTimePcmFrames(sound, pcm);
}

void lvnSoundScheduleStartTimeMilliseconds(LvnSound* sound, uint64_t ms)
{
    LVN_ASSERT(sound, "sound cannot be nulll");
    const LvnAudioContext* audioctx = (const LvnAudioContext*) sound->audioctx;
    audioctx->implSoundScheduleStartTimeMilliseconds(sound, ms);
}

void lvnSoundScheduleStopTimePcmFrames(LvnSound* sound, uint64_t pcm)
{
    LVN_ASSERT(sound, "sound cannot be nulll");
    const LvnAudioContext* audioctx = (const LvnAudioContext*) sound->audioctx;
    audioctx->implSoundScheduleStopTimePcmFrames(sound, pcm);
}

void lvnSoundScheduleStopTimeMilliseconds(LvnSound* sound, uint64_t ms)
{
    LVN_ASSERT(sound, "sound cannot be nulll");
    const LvnAudioContext* audioctx = (const LvnAudioContext*) sound->audioctx;
    audioctx->implSoundScheduleStopTimeMilliseconds(sound, ms);
}

void lvnSoundSetFadeMilliseconds(LvnSound* sound, float volBegin, float volEnd, uint64_t ms)
{
    LVN_ASSERT(sound, "sound cannot be nulll");
    const LvnAudioContext* audioctx = (const LvnAudioContext*) sound->audioctx;
    audioctx->implSoundSetFadeMilliseconds(sound, volBegin, volEnd, ms);
}

void lvnSoundSetFadePcmFrames(LvnSound* sound, float volBegin, float volEnd, uint64_t pcm)
{
    LVN_ASSERT(sound, "sound cannot be nulll");
    const LvnAudioContext* audioctx = (const LvnAudioContext*) sound->audioctx;
    audioctx->implSoundSetFadePcmFrames(sound, volBegin, volEnd, pcm);
}

void lvnSoundSeekToPcmFrame(LvnSound* sound, uint64_t pcm)
{
    LVN_ASSERT(sound, "sound cannot be nulll");
    const LvnAudioContext* audioctx = (const LvnAudioContext*) sound->audioctx;
    audioctx->implSoundSeekToPcmFrame(sound, pcm);
}

float lvnSoundGetVolume(const LvnSound* sound)
{
    LVN_ASSERT(sound, "sound cannot be nulll");
    const LvnAudioContext* audioctx = (const LvnAudioContext*) sound->audioctx;
    return audioctx->implSoundGetVolume(sound);
}

float lvnSoundGetPan(const LvnSound* sound)
{
    LVN_ASSERT(sound, "sound cannot be nulll");
    const LvnAudioContext* audioctx = (const LvnAudioContext*) sound->audioctx;
    return audioctx->implSoundGetPan(sound);
}

float lvnSoundGetPitch(const LvnSound* sound)
{
    LVN_ASSERT(sound, "sound cannot be nulll");
    const LvnAudioContext* audioctx = (const LvnAudioContext*) sound->audioctx;
    return audioctx->implSoundGetPitch(sound);
}

LvnSoundPositioningFlags lvnSoundGetPositioning(const LvnSound* sound)
{
    LVN_ASSERT(sound, "sound cannot be nulll");
    const LvnAudioContext* audioctx = (const LvnAudioContext*) sound->audioctx;
    return audioctx->implSoundGetPositioning(sound);
}

void lvnSoundGetPosition(const LvnSound* sound, float* x, float* y, float* z)
{
    LVN_ASSERT(sound, "sound cannot be nulll");
    const LvnAudioContext* audioctx = (const LvnAudioContext*) sound->audioctx;
    audioctx->implSoundGetPosition(sound, x, y, z);
}

void lvnSoundGetDirection(const LvnSound* sound, float* x, float* y, float* z)
{
    LVN_ASSERT(sound, "sound cannot be nulll");
    const LvnAudioContext* audioctx = (const LvnAudioContext*) sound->audioctx;
    audioctx->implSoundGetDirection(sound, x, y, z);
}

void lvnSoundGetVelocity(const LvnSound* sound, float* x, float* y, float* z)
{
    LVN_ASSERT(sound, "sound cannot be nulll");
    const LvnAudioContext* audioctx = (const LvnAudioContext*) sound->audioctx;
    audioctx->implSoundGetVelocity(sound, x, y, z);
}

void lvnSoundGetCone(const LvnSound* sound, float* innerAngleRad, float* outerAngleRad, float* outerGain)
{
    LVN_ASSERT(sound, "sound cannot be nulll");
    const LvnAudioContext* audioctx = (const LvnAudioContext*) sound->audioctx;
    audioctx->implSoundGetCone(sound, innerAngleRad, outerAngleRad, outerGain);
}

LvnSoundAttenuationFlags lvnSoundGetAttenuation(const LvnSound* sound)
{
    LVN_ASSERT(sound, "sound cannot be nulll");
    const LvnAudioContext* audioctx = (const LvnAudioContext*) sound->audioctx;
    return audioctx->implSoundGetAttenuation(sound);
}

float lvnSoundGetRolloff(const LvnSound* sound)
{
    LVN_ASSERT(sound, "sound cannot be nulll");
    const LvnAudioContext* audioctx = (const LvnAudioContext*) sound->audioctx;
    return audioctx->implSoundGetRolloff(sound);
}

float lvnSoundGetMinGain(const LvnSound* sound)
{
    LVN_ASSERT(sound, "sound cannot be nulll");
    const LvnAudioContext* audioctx = (const LvnAudioContext*) sound->audioctx;
    return audioctx->implSoundGetMinGain(sound);
}

float lvnSoundGetMaxGain(const LvnSound* sound)
{
    LVN_ASSERT(sound, "sound cannot be nulll");
    const LvnAudioContext* audioctx = (const LvnAudioContext*) sound->audioctx;
    return audioctx->implSoundGetMaxGain(sound);
}

float lvnSoundGetMinDistance(const LvnSound* sound)
{
    LVN_ASSERT(sound, "sound cannot be nulll");
    const LvnAudioContext* audioctx = (const LvnAudioContext*) sound->audioctx;
    return audioctx->implSoundGetMinDistance(sound);
}

float lvnSoundGetMaxDistance(const LvnSound* sound)
{
    LVN_ASSERT(sound, "sound cannot be nulll");
    const LvnAudioContext* audioctx = (const LvnAudioContext*) sound->audioctx;
    return audioctx->implSoundGetMaxDistance(sound);
}

float lvnSoundGetDopplerFactor(const LvnSound* sound)
{
    LVN_ASSERT(sound, "sound cannot be nulll");
    const LvnAudioContext* audioctx = (const LvnAudioContext*) sound->audioctx;
    return audioctx->implSoundGetDopplerFactor(sound);
}

bool lvnSoundIsLooping(const LvnSound* sound)
{
    LVN_ASSERT(sound, "sound cannot be nulll");
    const LvnAudioContext* audioctx = (const LvnAudioContext*) sound->audioctx;
    return audioctx->implSoundIsLooping(sound);
}

bool lvnSoundIsPlaying(const LvnSound* sound)
{
    LVN_ASSERT(sound, "sound cannot be nulll");
    const LvnAudioContext* audioctx = (const LvnAudioContext*) sound->audioctx;
    return audioctx->implSoundIsPlaying(sound);
}

bool lvnSoundAtEnd(const LvnSound* sound)
{
    LVN_ASSERT(sound, "sound cannot be nulll");
    const LvnAudioContext* audioctx = (const LvnAudioContext*) sound->audioctx;
    return audioctx->implSoundAtEnd(sound);
}

uint64_t lvnSoundGetTimeMilliseconds(const LvnSound* sound)
{
    LVN_ASSERT(sound, "sound cannot be nulll");
    const LvnAudioContext* audioctx = (const LvnAudioContext*) sound->audioctx;
    return audioctx->implSoundGetTimeMilliseconds(sound);
}

uint64_t lvnSoundGetTimePcmFrames(const LvnSound* sound)
{
    LVN_ASSERT(sound, "sound cannot be nulll");
    const LvnAudioContext* audioctx = (const LvnAudioContext*) sound->audioctx;
    return audioctx->implSoundGetTimePcmFrames(sound);
}

float lvnSoundGetLengthSeconds(LvnSound* sound)
{
    LVN_ASSERT(sound, "sound cannot be nulll");
    const LvnAudioContext* audioctx = (const LvnAudioContext*) sound->audioctx;
    return audioctx->implSoundGetLengthSeconds(sound);
}
