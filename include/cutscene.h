#ifndef CUTSCENE_H
#define CUTSCENE_H

#include "raylib.h"
#include "game_state.h"
#include <stdbool.h>

// Definições antigas mantidas
#define MAX_CUTSCENE_PAGES 40
#define CHARS_PER_SECOND 30

typedef struct {
    const char *text;
    float duration;
} CutscenePage;

typedef struct {
    // --- Variáveis da Intro (Existentes) ---
    CutscenePage pages[MAX_CUTSCENE_PAGES];
    int currentPage;
    float currentTimer;
    float textTimer;
    int visibleChars;
    bool isFadingOut;
    float titleAlpha;
    bool showTitle;

    // --- NOVAS VARIÁVEIS PARA O FINAL (COMICS) ---
    bool isEnding;           // Se true, roda a lógica do final. Se false, roda a intro.
    Texture2D endingImages[5]; // Array para as 5 imagens
    int endingImageIndex;    // Qual imagem está mostrando (0 a 4)

} CutsceneScene;

void InitCutscene(CutsceneScene *cs);
// Nova função para iniciar especificamente o final
void InitEnding(CutsceneScene *cs);

void UpdateCutscene(CutsceneScene *cs, GameState *state, float deltaTime);
void DrawCutscene(CutsceneScene *cs, int screenWidth, int screenHeight);

#endif // CUTSCENE_H