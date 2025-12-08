#ifndef AUDIO_H
#define AUDIO_H

#include "raylib.h" // Inclui tipos de dados de áudio da Raylib: Music e Sound

// --- CONSTANTES DE REFERÊNCIA ---

// Tipos de ataque (usados para selecionar o SFX de disparo correto)
#define ATTACK_WEAK 1
#define ATTACK_MEDIUM 2
#define ATTACK_STRONG 3

// --- ENUMERAÇÃO DE MÚSICAS ---

/**
 * @brief Define os diferentes tipos de trilhas sonoras disponíveis no jogo.
 */
typedef enum {
    MUSIC_SHOP,     // Música de Fundo da Loja.
    MUSIC_GAMEPLAY, // Música de Fundo da Fase de Ação.
    MUSIC_CUTSCENE, // Música da Cutscene de Abertura (Introdução).
    MUSIC_ENDING    // Música da Cena Final (Pós-vitória/Quadrinhos).
} MusicType;

// --- ESTRUTURA DE DADOS ---

/**
 * @brief Gerenciador principal de todos os recursos de áudio.
 */
typedef struct AudioManager {
    // Músicas (Stream)
    Music musicShop;
    Music musicGameplay;
    Music musicCutscene;
    Music musicEnding;  //Recurso para a música do final.
    Music* currentMusic; // Ponteiro para a música que está sendo reproduzida atualmente.

    // Efeitos Sonoros (Sound)
    Sound sfxWeak;      // SFX para ataque fraco.
    Sound sfxMedium;    // SFX para ataque médio.
    Sound sfxStrong;    // SFX para ataque forte.
    Sound sfxCharge;    // SFX para o carregamento do ataque.
    Sound sfxExplosionEnemy; // SFX para a explosão de um inimigo.
} AudioManager;

// --- DECLARAÇÕES DE FUNÇÕES PÚBLICAS ---

/**
 * @brief Inicializa o Gerenciador de Áudio, carregando todas as músicas e SFX.
 */
void InitAudioManager(AudioManager *manager);

/**
 * @brief Atualiza o stream de música atual (necessário para que o áudio continue tocando).
 */
void UpdateAudioManager(AudioManager *manager);

/**
 * @brief Para a música atual e inicia uma nova trilha sonora baseada no tipo fornecido.
 */
void PlayMusicTrack(AudioManager *manager, MusicType type);

/**
 * @brief Reproduz o SFX de disparo apropriado com base no tipo de ataque.
 */
void PlayAttackSfx(AudioManager *manager, int attackType);

/**
 * @brief Reproduz o SFX de explosão de inimigos.
 */
void PlayEnemyExplosionSfx(AudioManager *manager);

/**
 * @brief Descarrega todos os recursos de áudio (músicas e SFX) para liberar memória.
 */
void UnloadAudioManager(AudioManager *manager);

#endif // AUDIO_H