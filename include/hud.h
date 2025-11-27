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

    // NOVOS: Texturas dos power-ups
    Texture2D doubleShotTexture;
    Texture2D shieldTexture;
} Hud;

// --- FUNÇÕES ---
void InitHud(Hud *hud);
void UpdateHud(Hud *hud, float deltaTime);
// NOVO: Adicionado argumentos para os power-ups para desenhar na HUD
void DrawHudSide(Hud *hud, bool isLeft, int marginHeight, float energyCharge, bool hasDoubleShot, bool hasShield, int extraLives);
void UnloadHud(Hud *hud);

#endif // HUD_H