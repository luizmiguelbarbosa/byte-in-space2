#ifndef CUTSCENE_H
#define CUTSCENE_H

#include "raylib.h"    // Inclui tipos de dados como Texture2D
#include "game_state.h" // Necessário para transição de estado (e.g., para STATE_SHOP ou fechar o jogo)
#include <stdbool.h>

// --- CONSTANTES ---
#define MAX_CUTSCENE_PAGES 40 // Máximo de páginas/linhas de texto que a cutscene pode ter (não muito usado no modo Intro)
#define CHARS_PER_SECOND 30   // Velocidade de digitação do texto (se o efeito typewriter for usado)

// --- ESTRUTURAS DE DADOS ---

/**
 * @brief Define uma única página de texto da cutscene.
 */
typedef struct {
    const char *text;   // O conteúdo da linha de texto.
    float duration;     // Duração que esta página deve permanecer na tela (se aplicável).
} CutscenePage;

/**
 * @brief Estrutura principal que gerencia o estado da Cutscene (Intro e Ending).
 */
typedef struct {

    // --- Variáveis da Intro/Título ---
    CutscenePage pages[MAX_CUTSCENE_PAGES];
    int currentPage;    // Qual página de texto está sendo exibida.
    float currentTimer; // Temporizador para transição entre páginas.
    float textTimer;    // Temporizador para o efeito de digitação.
    int visibleChars;   // Quantidade de caracteres visíveis atualmente.
    bool isFadingOut;   // Flag para transição de saída.
    float titleAlpha;   // Transparência do título.
    bool showTitle;     // Flag para mostrar o título.

    // --- Variáveis do Ending (Modo Quadrinhos) ---
    bool isEnding;              // Flag: Se TRUE, o módulo está no modo Final (Ending), se FALSE, está no modo Intro/Título.
    Texture2D endingImages[5];  // Array de texturas (quadros de quadrinhos) para a cena final.
    int endingImageIndex;       // Índice do quadro atual sendo exibido (o jogador avança este índice).

} CutsceneScene;

// --- DECLARAÇÕES DE FUNÇÕES PÚBLICAS ---

/**
 * @brief Inicializa a cena no modo Intro/Título.
 */
void InitCutscene(CutsceneScene *cs);

/**
 * @brief Inicializa a cena no modo Final (Ending), carregando os quadros de quadrinhos.
 */
void InitEnding(CutsceneScene *cs);

/**
 * @brief Atualiza a lógica da cutscene: temporizadores, entrada do usuário e transição de estado.
 */
void UpdateCutscene(CutsceneScene *cs, GameState *state, float deltaTime);

/**
 * @brief Desenha a cutscene, seja a Intro (com efeitos néon/parallax) ou o Ending (com layout de quadrinhos).
 */
void DrawCutscene(CutsceneScene *cs, int screenWidth, int screenHeight);

#endif // CUTSCENE_H