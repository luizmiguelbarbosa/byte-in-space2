#ifndef HUD_H
#define HUD_H

#include "raylib.h"
#include <stdbool.h>

// --- ESTRUTURA ---
typedef struct {
    int score;
    int lives;
    Texture2D lifeIconTexture;
    Texture2D energyIconTexture;
} Hud;

// --- FUNÇÕES ---
void InitHud(Hud *hud);
void UpdateHud(Hud *hud, float deltaTime);
void DrawHudSide(Hud *hud, bool isLeft, int marginHeight, float energyCharge);
void UnloadHud(Hud *hud);

#endif // HUD_H