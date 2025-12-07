// audio.c
#include "audio.h"
#include <stdio.h>
#include "raylib.h"

#define BACKGROUND_VOLUME 0.3f
#define SFX_VOLUME 0.3f
#define CHARGE_VOLUME 0.25f
#define EXPLOSION_VOLUME 0.6f
#define CUTSCENE_VOLUME 0.6f
#define ENDING_VOLUME 0.6f // <--- NOVO: Volume para a música final

void InitAudioManager(AudioManager *manager) {
    if (!IsAudioDeviceReady()) {
        InitAudioDevice();
    }

    // --- MÚSICAS ---
    manager->musicShop = LoadMusicStream("assets/ost/shop.mp3");
    manager->musicGameplay = LoadMusicStream("assets/ost/murder_byte.mp3");
    manager->musicCutscene = LoadMusicStream("assets/ost/cutscene_byte1.mp3");

    // <--- NOVO: Carrega a Música Final ---
    manager->musicEnding = LoadMusicStream("assets/ost/musicafinal.mp3");
    if (manager->musicEnding.frameCount == 0) {
        printf("[AUDIO ERROR] Nao foi possivel carregar musicafinal.mp3. Verifique o caminho 'assets/ost/musicafinal.mp3'.\n");
    }
    // ------------------------------------

    if (manager->musicCutscene.frameCount == 0) {
        printf("[AUDIO ERROR] Nao foi possivel carregar cutscene_byte1.mp3. Verifique o caminho 'assets/ost/cutscene_byte1.mp3' relativo ao executavel.\n");
    }

    manager->currentMusic = NULL;

    // --- SFX DE TIROS E CHARGE ---
    manager->sfxWeak = LoadSound("assets/ost/biscoito_laser_1.mp3");
    manager->sfxMedium = LoadSound("assets/ost/biscoito_laser_2.mp3");
    manager->sfxStrong = LoadSound("assets/ost/biscoito_laser_3.mp3");
    manager->sfxCharge = LoadSound("assets/ost/charge_byte.mp3");

    // --- NOVO: SFX DE EXPLOSÃO ---
    manager->sfxExplosionEnemy = LoadSound("assets/ost/explosion_enemy.mp3");
    if (manager->sfxExplosionEnemy.frameCount == 0) {
        printf("[AUDIO ERROR] Nao foi possivel carregar explosion_enemy.mp3. Verifique o caminho 'assets/ost/explosion_enemy.mp3'.\n");
    }

    // --- CONFIGURAÇÃO DE VOLUME ---
    SetSoundVolume(manager->sfxWeak, SFX_VOLUME);
    SetSoundVolume(manager->sfxMedium, SFX_VOLUME);
    SetSoundVolume(manager->sfxStrong, SFX_VOLUME);
    SetSoundVolume(manager->sfxCharge, CHARGE_VOLUME);
    SetSoundVolume(manager->sfxExplosionEnemy, EXPLOSION_VOLUME);
}

void PlayMusicTrack(AudioManager *manager, MusicType type) {
    // Adicionado um retorno de segurança
    if (type == MUSIC_CUTSCENE && manager->musicCutscene.frameCount == 0) return;
    if (type == MUSIC_SHOP && manager->musicShop.frameCount == 0) return;
    if (type == MUSIC_GAMEPLAY && manager->musicGameplay.frameCount == 0) return;
    if (type == MUSIC_ENDING && manager->musicEnding.frameCount == 0) return; // <--- NOVO

    if (manager->currentMusic != NULL && IsMusicStreamPlaying(*manager->currentMusic)) {
        StopMusicStream(*manager->currentMusic);
    }

    switch (type) {
        case MUSIC_SHOP:
            manager->currentMusic = &manager->musicShop;
            SetMusicVolume(*manager->currentMusic, BACKGROUND_VOLUME);
            break;
        case MUSIC_GAMEPLAY:
            manager->currentMusic = &manager->musicGameplay;
            SetMusicVolume(*manager->currentMusic, BACKGROUND_VOLUME);
            break;
        case MUSIC_CUTSCENE:
            manager->currentMusic = &manager->musicCutscene;
            SetMusicVolume(*manager->currentMusic, CUTSCENE_VOLUME);
            break;
        case MUSIC_ENDING: // <--- NOVO: Música do Final
            manager->currentMusic = &manager->musicEnding;
            SetMusicVolume(*manager->currentMusic, ENDING_VOLUME);
            break;
        default: manager->currentMusic = NULL; return;
    }

    if (manager->currentMusic != NULL) {
        PlayMusicStream(*manager->currentMusic);
    }
}

void UpdateAudioManager(AudioManager *manager) {
    if (manager->currentMusic != NULL) {
        UpdateMusicStream(*manager->currentMusic);
    }
}

void PlayAttackSfx(AudioManager *manager, int attackType) {
    if (!IsAudioDeviceReady()) return;

    switch (attackType) {
        case ATTACK_WEAK: PlaySound(manager->sfxWeak); break;
        case ATTACK_MEDIUM: PlaySound(manager->sfxMedium); break;
        case ATTACK_STRONG: PlaySound(manager->sfxStrong); break;
    }
}

void PlayEnemyExplosionSfx(AudioManager *manager) {
    if (!IsAudioDeviceReady()) return;
    PlaySound(manager->sfxExplosionEnemy);
}

void UnloadAudioManager(AudioManager *manager) {
    UnloadMusicStream(manager->musicShop);
    UnloadMusicStream(manager->musicGameplay);
    UnloadMusicStream(manager->musicCutscene);
    UnloadMusicStream(manager->musicEnding); // <--- NOVO: Descarrega música final
    UnloadSound(manager->sfxWeak);
    UnloadSound(manager->sfxMedium);
    UnloadSound(manager->sfxStrong);
    UnloadSound(manager->sfxCharge);
    UnloadSound(manager->sfxExplosionEnemy);
}