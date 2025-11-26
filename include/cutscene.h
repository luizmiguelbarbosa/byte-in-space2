#ifndef CUTSCENE_H
#define CUTSCENE_H

#include "raylib.h"
#include "game_state.h"
#include <stdbool.h>

// O array agora usa 34 páginas (de 0 a 33). Definido para 40 para ter margem de seguranca.
#define MAX_CUTSCENE_PAGES 40

// Constante faltante: Velocidade da digitação: caracteres por segundo
#define CHARS_PER_SECOND 30

typedef struct {
    const char *text;
    float duration; // Duração mínima em segundos da exibição
} CutscenePage;

typedef struct {
    CutscenePage pages[MAX_CUTSCENE_PAGES];
    int currentPage;
    float currentTimer;

    // VARIÁVEIS ADICIONADAS PARA CORRIGIR OS ERROS:
    float textTimer;    // Timer para controlar a digitação
    int visibleChars;   // Quantidade de caracteres visíveis
    bool isFadingOut;   // Controla o fade out entre as páginas

    // Variáveis para o Fade-in/Out do Título final
    float titleAlpha;
    bool showTitle;
} CutsceneScene;

void InitCutscene(CutsceneScene *cs);
void UpdateCutscene(CutsceneScene *cs, GameState *state, float deltaTime);
void DrawCutscene(CutsceneScene *cs, int screenWidth, int screenHeight);

#endif // CUTSCENE_H