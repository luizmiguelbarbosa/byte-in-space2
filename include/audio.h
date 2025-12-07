// audio.h
#ifndef AUDIO_H
#define AUDIO_H

#include "raylib.h"

#define ATTACK_WEAK 1
#define ATTACK_MEDIUM 2
#define ATTACK_STRONG 3

typedef enum {
    MUSIC_SHOP,
    MUSIC_GAMEPLAY,
    MUSIC_CUTSCENE, // Música da Cutscene de Abertura
    MUSIC_ENDING    // <--- NOVO: Música do Final (Comics)
} MusicType;

typedef struct AudioManager {
    Music musicShop;
    Music musicGameplay;
    Music musicCutscene;
    Music musicEnding;  // <--- NOVO: Música do Final
    Music* currentMusic;

    Sound sfxWeak;
    Sound sfxMedium;
    Sound sfxStrong;
    Sound sfxCharge;
    Sound sfxExplosionEnemy;
} AudioManager;

void InitAudioManager(AudioManager *manager);
void UpdateAudioManager(AudioManager *manager);
void PlayMusicTrack(AudioManager *manager, MusicType type);
void PlayAttackSfx(AudioManager *manager, int attackType);
void PlayEnemyExplosionSfx(AudioManager *manager);
void UnloadAudioManager(AudioManager *manager);

#endif // AUDIO_H