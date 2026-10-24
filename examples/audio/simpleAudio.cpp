#include <levikno/levikno.h>
#include <levikno/lvn_audio.h>

int main(int argc, char** argv)
{
    LvnContextCreateInfo ctxCreateInfo{};
    ctxCreateInfo.logging.enableLogging = true;

    LvnContext* ctx;
    lvnCreateContext(&ctx, &ctxCreateInfo);

    LvnAudioContextCreateInfo actxCreateInfo{};

    LvnAudioContext* audioctx;
    lvnCreateAudioContext(ctx, &audioctx, &actxCreateInfo);

    LvnSoundBufferCreateInfo soundBufferCreateInfo{};
    soundBufferCreateInfo.filepath = "res/audio/bird_chirps.mp3";

    LvnSoundBuffer* soundBuffer;
    lvnCreateSoundBuffer(audioctx, &soundBuffer, &soundBufferCreateInfo);

    LvnSoundCreateInfo soundCreateInfo{};
    soundCreateInfo.soundBuffer = soundBuffer;
    soundCreateInfo.flags = 0;

    LvnSound* sound;
    lvnCreateSound(audioctx, &sound, &soundCreateInfo);

    lvnSoundStart(sound);

    while (lvnSoundIsPlaying(sound))
    {
        // wait for sound to finish
    }

    lvnDestroySound(sound);
    lvnDestroySoundBuffer(soundBuffer);
    lvnDestroyAudioContext(audioctx);
    lvnDestroyContext(ctx);
}
