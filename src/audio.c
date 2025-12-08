// audio.c
#include "audio.h"
#include <stdio.h>
#include "raylib.h"

// --- CONSTANTES DE VOLUME ---
#define BACKGROUND_VOLUME 0.3f  // Volume padrão para músicas de fundo (Shop, Gameplay)
#define SFX_VOLUME 0.3f         // Volume padrão para SFX de tiros
#define CHARGE_VOLUME 0.25f     // Volume para o som de carregamento
#define EXPLOSION_VOLUME 0.6f   // Volume para o novo SFX de explosão
#define CUTSCENE_VOLUME 0.6f    // Volume para a música da cutscene
#define ENDING_VOLUME 0.6f      // Volume para a música final

// --- FUNÇÃO DE INICIALIZAÇÃO ---
void InitAudioManager(AudioManager *manager) {
    // Inicializa o dispositivo de áudio se ainda não estiver pronto
    if (!IsAudioDeviceReady()) {
        InitAudioDevice();
    }

    // --- MÚSICAS ---
    manager->musicShop = LoadMusicStream("assets/ost/shop.mp3");
    manager->musicGameplay = LoadMusicStream("assets/ost/murder_byte.mp3");
    manager->musicCutscene = LoadMusicStream("assets/ost/cutscene_byte1.mp3");

    // CARREGA MÚSICA FINAL
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

    // NOVO: SFX DE EXPLOSÃO
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

// --- FUNÇÃO DE REPRODUÇÃO DE MÚSICA ---
void PlayMusicTrack(AudioManager *manager, MusicType type) {
    // Retorno de segurança para músicas não carregadas
    if (type == MUSIC_CUTSCENE && manager->musicCutscene.frameCount == 0) return;
    if (type == MUSIC_SHOP && manager->musicShop.frameCount == 0) return;
    if (type == MUSIC_GAMEPLAY && manager->musicGameplay.frameCount == 0) return;
    if (type == MUSIC_ENDING && manager->musicEnding.frameCount == 0) return; // <--- Checa música final

    // Para a música atualmente tocando, se houver
    if (manager->currentMusic != NULL && IsMusicStreamPlaying(*manager->currentMusic)) {
        StopMusicStream(*manager->currentMusic);
    }

    // Seleciona a música e define o volume
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
        case MUSIC_ENDING: // <--- MÚSICA DO FINAL
            manager->currentMusic = &manager->musicEnding;
            SetMusicVolume(*manager->currentMusic, ENDING_VOLUME);
            break;
        default: manager->currentMusic = NULL; return;
    }

    // Inicia a nova música
    if (manager->currentMusic != NULL) {
        PlayMusicStream(*manager->currentMusic);
    }
}

// --- FUNÇÃO DE ATUALIZAÇÃO ---
void UpdateAudioManager(AudioManager *manager) {
    // Atualiza o stream de música para evitar interrupções (necessário na Raylib)
    if (manager->currentMusic != NULL) {
        UpdateMusicStream(*manager->currentMusic);
    }
}

// --- FUNÇÃO DE REPRODUÇÃO DE SFX DE ATAQUE ---
void PlayAttackSfx(AudioManager *manager, int attackType) {
    if (!IsAudioDeviceReady()) return;

    switch (attackType) {
        case ATTACK_WEAK: PlaySound(manager->sfxWeak); break;
        case ATTACK_MEDIUM: PlaySound(manager->sfxMedium); break;
        case ATTACK_STRONG: PlaySound(manager->sfxStrong); break;
    }
}

// --- FUNÇÃO DE REPRODUÇÃO DE SFX DE EXPLOSÃO ---
void PlayEnemyExplosionSfx(AudioManager *manager) {
    if (!IsAudioDeviceReady()) return;
    PlaySound(manager->sfxExplosionEnemy);
}

// --- FUNÇÃO DE DESCARREGAMENTO ---
void UnloadAudioManager(AudioManager *manager) {
    // Descarrega todas as músicas e SFX carregados
    UnloadMusicStream(manager->musicShop);
    UnloadMusicStream(manager->musicGameplay);
    UnloadMusicStream(manager->musicCutscene);
    UnloadMusicStream(manager->musicEnding);
    UnloadSound(manager->sfxWeak);
    UnloadSound(manager->sfxMedium);
    UnloadSound(manager->sfxStrong);
    UnloadSound(manager->sfxCharge);
    UnloadSound(manager->sfxExplosionEnemy);
}