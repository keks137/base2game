#ifndef INCLUDE_CPP_SOUND_H_
#define INCLUDE_CPP_SOUND_H_

typedef enum {
    SFX_MOVE = 0,
    SFX_MERGE,
} SoundID;

extern const char *SoundFileNames[];

void LoadAllSounds();
void PlaySfx(SoundID sfxID);
void PlaySfxPb(SoundID sfxID, float offsetRange);
void UnloadAllSounds();

#endif // INCLUDE_CPP_SOUND_H_
