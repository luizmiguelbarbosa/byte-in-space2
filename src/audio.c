#include "audio.h"
#include <stdio.h>
#include "raylib.h"

#define BACKGROUND_VOLUME 0.5f
#define SFX_VOLUME 0.3f
#define CHARGE_VOLUME 0.25f
#define CUTSCENE_VOLUME 0.6f // Volume um pouco mais alto para a cutscene

void InitAudioManager(AudioManager *manager) {
    if (!IsAudioDeviceReady()) {
        InitAudioDevice();
    }

    // --- CORREÇÃO DE CAMINHO ---
    // Todos os caminhos devem ser relativos ao executável para portabilidade.
    // O caminho absoluto foi removido.
    manager->musicShop = LoadMusicStream("assets/ost/shop.mp3");
    manager->musicGameplay = LoadMusicStream("assets/ost/murder_byte.mp3");
    manager->musicCutscene = LoadMusicStream("assets/ost/cutscene_byte1.mp3"); // CORRIGIDO PARA CAMINHO RELATIVO

    // Verificação simples para debug
    if (manager->musicCutscene.frameCount == 0) {
        printf("[AUDIO ERROR] Nao foi possivel carregar cutscene_byte1.mp3. Verifique o caminho 'assets/ost/cutscene_byte1.mp3' relativo ao executavel.\n");
    }

    manager->currentMusic = NULL;

    manager->sfxWeak = LoadSound("assets/ost/biscoito_laser_1.mp3");
    manager->sfxMedium = LoadSound("assets/ost/biscoito_laser_2.mp3");
    manager->sfxStrong = LoadSound("assets/ost/biscoito_laser_3.mp3");
    manager->sfxCharge = LoadSound("assets/ost/charge_byte.mp3");

    SetSoundVolume(manager->sfxWeak, SFX_VOLUME);
    SetSoundVolume(manager->sfxMedium, SFX_VOLUME);
    SetSoundVolume(manager->sfxStrong, SFX_VOLUME);
    SetSoundVolume(manager->sfxCharge, CHARGE_VOLUME);
}

void PlayMusicTrack(AudioManager *manager, MusicType type) {
    // Adicionado um retorno de segurança caso o áudio não tenha sido carregado
    if (type == MUSIC_CUTSCENE && manager->musicCutscene.frameCount == 0) return;
    if (type == MUSIC_SHOP && manager->musicShop.frameCount == 0) return;
    if (type == MUSIC_GAMEPLAY && manager->musicGameplay.frameCount == 0) return;


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
        case MUSIC_CUTSCENE: // Música da Cutscene
            manager->currentMusic = &manager->musicCutscene;
            SetMusicVolume(*manager->currentMusic, CUTSCENE_VOLUME);
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

void UnloadAudioManager(AudioManager *manager) {
    UnloadMusicStream(manager->musicShop);
    UnloadMusicStream(manager->musicGameplay);
    UnloadMusicStream(manager->musicCutscene); // Descarrega Cutscene
    UnloadSound(manager->sfxWeak);
    UnloadSound(manager->sfxMedium);
    UnloadSound(manager->sfxStrong);
    UnloadSound(manager->sfxCharge);
}