#ifndef HUD_H
#define HUD_H

#include "raylib.h"
#include <stdbool.h>

typedef struct {
    int score;
    int lives;
    Texture2D lifeIconTexture;
    Texture2D energyIconTexture;

    Texture2D shurikenTexture;
    Texture2D shieldTexture;
} Hud;

void InitHud(Hud *hud);
void UpdateHud(Hud *hud, float deltaTime);
void DrawHudSide(Hud *hud, bool isLeft, int marginHeight, float energyCharge, bool hasDoubleShot, bool hasShield, int extraLives);
void UnloadHud(Hud *hud);

#endif // HUD_H