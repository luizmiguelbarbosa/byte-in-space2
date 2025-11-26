#ifndef AUDIO_H
#define AUDIO_H

#include "raylib.h"

#define ATTACK_WEAK 1
#define ATTACK_MEDIUM 2
#define ATTACK_STRONG 3

typedef enum {
    MUSIC_SHOP,
    MUSIC_GAMEPLAY,
    MUSIC_CUTSCENE // Tipo de música para a Cutscene
} MusicType;

typedef struct AudioManager {
    Music musicShop;
    Music musicGameplay;
    Music musicCutscene; // Áudio da cutscene
    Music* currentMusic;

    Sound sfxWeak;
    Sound sfxMedium;
    Sound sfxStrong;
    Sound sfxCharge;
} AudioManager;

void InitAudioManager(AudioManager *manager);
void UpdateAudioManager(AudioManager *manager);
void PlayMusicTrack(AudioManager *manager, MusicType type);
void PlayAttackSfx(AudioManager *manager, int attackType);
void UnloadAudioManager(AudioManager *manager);

#endif // AUDIO_H