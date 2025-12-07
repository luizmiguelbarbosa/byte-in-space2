#ifndef HUD_H
#define HUD_H

#include "raylib.h"
#include <stdbool.h>

typedef struct {
    int score; // Mantido para referência interna, mas não mais desenhado.
    // Removido 'int lives;' pois a vida será lida diretamente do Player.
    Texture2D lifeIconTexture;
    Texture2D energyIconTexture;

    Texture2D shurikenTexture;
    Texture2D shieldTexture;
    // --- ADICIONADO: Textura para o ícone de Gold (Ouro) ---
    Texture2D goldTexture;
    // -------------------------------------------------------
} Hud;

void InitHud(Hud *hud);
void UpdateHud(Hud *hud, float deltaTime);
// --- CORREÇÃO: Adicionado 'currentGold' no final da lista de parâmetros ---
void DrawHudSide(Hud *hud, bool isLeft, int marginHeight, float energyCharge, bool hasDoubleShot, bool hasShield, int extraLives, int drawLives, int currentGold);
// -------------------------------------------------------------------------

void UnloadHud(Hud *hud);

#endif // HUD_H