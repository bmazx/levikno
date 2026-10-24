#include <levikno/levikno.h>
#include <levikno/lvn_audio.h>

int main(int argc, char** argv)
{
    LvnContextCreateInfo ctxCreateInfo{};
    ctxCreateInfo.logging.enableLogging = true;
    ctxCreateInfo.appName = "simpleAudio";

    LvnContext* ctx;
    lvnCreateContext(&ctx, &ctxCreateInfo);

    LvnAudioContextCreateInfo actxCreateInfo{};

    LvnAudioContext* audioctx;
    lvnCreateAudioContext(ctx, &audioctx, &actxCreateInfo);

    LvnSoundBufferCreateInfo soundBufferCreateInfo{};
    soundBufferCreateInfo.filepath = "/home/bma/Documents/dev/levikno/examples/res/audio/bird_chirps.mp3";

    LvnSoundBuffer* soundBuffer;
    lvnCreateSoundBuffer(audioctx, &soundBuffer, &soundBufferCreateInfo);

    LvnSoundCreateInfo soundCreateInfo{};
    soundCreateInfo.soundBuffer = soundBuffer;
    soundCreateInfo.flags = 0;

    LvnSound* sound;
    lvnCreateSound(audioctx, &sound, &soundCreateInfo);



    lvnDestroySound(sound);
    lvnDestroySoundBuffer(soundBuffer);
    lvnDestroyAudioContext(audioctx);
    lvnDestroyContext(ctx);
}
