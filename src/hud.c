#include "hud.h"
#include "raylib.h"
#include <stdio.h>
#include <math.h>

#define LIFE_ICON_PATH "assets/images/sprites/life_icon_large.png"
#define ENERGY_ICON_PATH "assets/images/sprites/energy_icon.png"

#define SHURIKEN_PATH "assets/images/sprites/icone_powerup_shurikens.png"
#define SHIELD_PATH "assets/images/sprites/shield.png"

#define ICON_HUD_SCALE 0.12f

void InitHud(Hud *hud) {
    hud->score = 0;
    // O campo 'hud->lives' foi removido e a vida agora é gerenciada pelo Player.

    hud->lifeIconTexture = LoadTexture(LIFE_ICON_PATH);
    if (hud->lifeIconTexture.id != 0) SetTextureFilter(hud->lifeIconTexture, TEXTURE_FILTER_POINT);
    else printf("[ERRO] Icone de Vida nao encontrado: %s\n", LIFE_ICON_PATH);

    hud->energyIconTexture = LoadTexture(ENERGY_ICON_PATH);
    if (hud->energyIconTexture.id != 0) SetTextureFilter(hud->energyIconTexture, TEXTURE_FILTER_POINT);
    else printf("[ERRO] Icone de Energia nao encontrado: %s\n", ENERGY_ICON_PATH);

    hud->shurikenTexture = LoadTexture(SHURIKEN_PATH);
    if (hud->shurikenTexture.id != 0) SetTextureFilter(hud->shurikenTexture, TEXTURE_FILTER_POINT);
    else printf("[ERRO] Icone de Shurikens nao encontrado: %s\n", SHURIKEN_PATH);

    hud->shieldTexture = LoadTexture(SHIELD_PATH);
    if (hud->shieldTexture.id != 0) SetTextureFilter(hud->shieldTexture, TEXTURE_FILTER_POINT);
    else printf("[ERRO] Icone de Escudo nao encontrado: %s\n", SHIELD_PATH);

}

void UpdateHud(Hud *hud, float deltaTime) {
}

// --- CORREÇÃO: Adicionando 'drawLives' ao final da lista de parâmetros ---
void DrawHudSide(Hud *hud, bool isLeft, int marginHeight, float energyCharge, bool hasDoubleShot, bool hasShield, int extraLives, int drawLives) {
// -------------------------------------------------------------------------
    int fontSize = 20;
    int screenW = GetScreenWidth();
    int targetY = 10;

    float iconTextureWidth = 64.0f;
    float iconTextureHeight = 64.0f;
    if (hud->lifeIconTexture.id != 0) iconTextureWidth = (float)hud->lifeIconTexture.width;
    if (hud->lifeIconTexture.id != 0) iconTextureHeight = (float)hud->lifeIconTexture.height;

    float iconDrawWidth = iconTextureWidth * ICON_HUD_SCALE;
    float iconDrawHeight = iconTextureHeight * ICON_HUD_SCALE;

    if (isLeft) {
        int iconX = 10;
        float currentY = (float)targetY;

        if (hud->lifeIconTexture.id != 0) {
            Rectangle sourceRecLife = { 0.0f, 0.0f, iconTextureWidth, iconTextureHeight };
            Rectangle destRecLife = { (float)iconX, currentY, iconDrawWidth, iconDrawHeight };
            DrawTexturePro(hud->lifeIconTexture, sourceRecLife, destRecLife, (Vector2){ 0.0f, 0.0f }, 0.0f, WHITE);
        }

        // --- CORREÇÃO: Usa drawLives (vida base 3 ou 5) + extraLives ---
        char livesText[10];
        sprintf(livesText, "x%02d", drawLives + extraLives);
        // ----------------------------------------------------------------

        int textX = iconX + (int)iconDrawWidth + 5;
        int textY = (int)currentY + (int)iconDrawHeight / 2 - fontSize / 2;
        DrawText(livesText, textX, textY, fontSize, WHITE);
        currentY += iconDrawHeight + 15.0f;

        if (hud->energyIconTexture.id != 0) {
            Rectangle energySourceRec = { 0.0f, 0.0f, (float)hud->energyIconTexture.width, (float)hud->energyIconTexture.height };
            Rectangle energyDestRec = { (float)iconX, currentY, iconDrawWidth, iconDrawHeight };

            Color energyColor = (energyCharge > 0.0f) ? WHITE : Fade(WHITE, 0.5f);
            DrawTexturePro(hud->energyIconTexture, energySourceRec, energyDestRec, (Vector2){ 0.0f, 0.0f }, 0.0f, energyColor);

            char chargeText[10];
            sprintf(chargeText, "%d%%", (int)round(energyCharge));

            int textEnergyX = iconX + (int)iconDrawWidth + 5;
            int textEnergyY = (int)currentY + (int)iconDrawHeight / 2 - fontSize / 2;
            DrawText(chargeText, textEnergyX, textEnergyY, fontSize, WHITE);
            currentY += iconDrawHeight + 15.0f;
        }

        if (hasDoubleShot) {
            if (hud->shurikenTexture.id != 0) {
                Rectangle source = { 0.0f, 0.0f, (float)hud->shurikenTexture.width, (float)hud->shurikenTexture.height };
                Rectangle dest = { (float)iconX, currentY, iconDrawWidth, iconDrawHeight };
                DrawTexturePro(hud->shurikenTexture, source, dest, (Vector2){ 0.0f, 0.0f }, 0.0f, WHITE);
                currentY += iconDrawHeight + 10.0f;
            }
        }

        if (hasShield) {
            if (hud->shieldTexture.id != 0) {
                Rectangle source = { 0.0f, 0.0f, (float)hud->shieldTexture.width, (float)hud->shieldTexture.height };
                Rectangle dest = { (float)iconX, currentY, iconDrawWidth, iconDrawHeight };
                DrawTexturePro(hud->shieldTexture, source, dest, (Vector2){ 0.0f, 0.0f }, 0.0f, WHITE);
                currentY += iconDrawHeight + 10.0f;
            }
        }

    } else {
        char scoreText[32];
        sprintf(scoreText, "SCORE: %06d", hud->score);
        int scoreTextWidth = MeasureText(scoreText, fontSize);
        int scoreTextX = screenW - scoreTextWidth - 10;
        int scoreTextY = targetY;
        DrawText(scoreText, scoreTextX, scoreTextY, fontSize, WHITE);
    }
}

void UnloadHud(Hud *hud) {
    if (hud->lifeIconTexture.id != 0) UnloadTexture(hud->lifeIconTexture);
    if (hud->energyIconTexture.id != 0) UnloadTexture(hud->energyIconTexture);
    if (hud->shurikenTexture.id != 0) UnloadTexture(hud->shurikenTexture);
    if (hud->shieldTexture.id != 0) UnloadTexture(hud->shieldTexture);

}