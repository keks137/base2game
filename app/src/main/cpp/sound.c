#ifdef SOUNDS_ON

#include "sound.h"
#include "raylib.h"
#include <stddef.h>

Sound Sounds[256] = {};

const char *SoundFileNames[] = {"sfx/moveSound.wav", "sfx/merge.wav"};
void LoadAllSounds() {
    size_t num_sfx = sizeof(SoundFileNames) / sizeof(const char *);

    for (int i = 0; i < num_sfx; i++) {
        Sounds[i] = LoadSound(SoundFileNames[i]);
    }
}
void UnloadAllSounds() {
    size_t num_sfx = sizeof(SoundFileNames) / sizeof(const char *);

    for (int i = 0; i < num_sfx; i++) {
        UnloadSound(Sounds[i]);
    }
}

void PlaySfx(SoundID sfxID) { PlaySound(Sounds[sfxID]); }
void PlaySfxPb(SoundID sfxID, float offsetRange) {
    Sound sfx = Sounds[sfxID];

    float offset = 1.0 + ((float)GetRandomValue(-offsetRange * 100, offsetRange * 100))/100;

    SetSoundPitch(sfx, offset);
    PlaySound(sfx);
}
#endif //SOUNDS_ON
