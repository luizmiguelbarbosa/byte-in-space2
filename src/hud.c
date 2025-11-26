#include "hud.h"
#include "raylib.h"
#include <stdio.h>
#include <math.h>

// --- DEFINIÇÕES DE CAMINHOS ---
#define LIFE_ICON_PATH "assets/images/sprites/life_icon_large.png"
#define ENERGY_ICON_PATH "assets/images/sprites/energy_icon.png"

// --- ESCALAS PEQUENAS PARA DESENHO DA HUD (AJUSTE FINAL) ---
#define LIFE_ICON_HUD_SCALE 0.12f
#define ENERGY_ICON_HUD_SCALE 0.12f

void InitHud(Hud *hud) {
    hud->score = 0;
    hud->lives = 3;

    hud->lifeIconTexture = LoadTexture(LIFE_ICON_PATH);
    if (hud->lifeIconTexture.id != 0) SetTextureFilter(hud->lifeIconTexture, TEXTURE_FILTER_POINT);
    else printf("[ERRO] Icone de Vida nao encontrado: %s\n", LIFE_ICON_PATH);

    hud->energyIconTexture = LoadTexture(ENERGY_ICON_PATH);
    if (hud->energyIconTexture.id != 0) SetTextureFilter(hud->energyIconTexture, TEXTURE_FILTER_POINT);
    else printf("[ERRO] Icone de Energia nao encontrado: %s\n", ENERGY_ICON_PATH);
}

void UpdateHud(Hud *hud, float deltaTime) {
    // Vazio.
}

void DrawHudSide(Hud *hud, bool isLeft, int marginHeight, float energyCharge) {
    int fontSize = 20;
    int screenW = GetScreenWidth();
    int targetY = 10;

    // --- Calcula as dimensões que serão usadas no desenho ---
    float lifeTextureWidth = (hud->lifeIconTexture.id != 0) ? (float)hud->lifeIconTexture.width : 0.0f;
    float lifeTextureHeight = (hud->lifeIconTexture.id != 0) ? (float)hud->lifeIconTexture.height : 0.0f;

    float energyTextureWidth = (hud->energyIconTexture.id != 0) ? (float)hud->energyIconTexture.width : 0.0f;
    float energyTextureHeight = (hud->energyIconTexture.id != 0) ? (float)hud->energyIconTexture.height : 0.0f;

    // Dimensões FINAIS (escaladas) para o desenho
    float lifeDrawWidth = lifeTextureWidth * LIFE_ICON_HUD_SCALE;
    float lifeDrawHeight = lifeTextureHeight * LIFE_ICON_HUD_SCALE;

    float energyDrawWidth = energyTextureWidth * ENERGY_ICON_HUD_SCALE;
    float energyDrawHeight = energyTextureHeight * ENERGY_ICON_HUD_SCALE;

    if (isLeft) {
        // --- LADO ESQUERDO: Coração/Vidas e Ícone de Energia/Porcentagem ---

        // 1. CORAÇÃO/VIDAS (Topo)
        int iconX = 10;
        float iconY = (float)targetY;

        // Desenha o Coração
        if (hud->lifeIconTexture.id != 0) {
            Rectangle sourceRecLife = { 0.0f, 0.0f, lifeTextureWidth, lifeTextureHeight };
            Rectangle destRecLife = { (float)iconX, iconY, lifeDrawWidth, lifeDrawHeight };
            DrawTexturePro(hud->lifeIconTexture, sourceRecLife, destRecLife, (Vector2){ 0.0f, 0.0f }, 0.0f, WHITE);
        }

        char livesText[10];
        sprintf(livesText, "x%02d", hud->lives);

        int textX = iconX + (int)lifeDrawWidth + 5;
        int textY = (int)iconY + (int)lifeDrawHeight / 2 - fontSize / 2;
        DrawText(livesText, textX, textY, fontSize, WHITE);


        // 2. ÍCONE DE ENERGIA E PORCENTAGEM (Embaixo do Coração)
        if (hud->energyIconTexture.id == 0) return;

        float nominalIconY = iconY + lifeDrawHeight + 15.0f;
        float finalIconX = 10.0f;

        int textEnergyY = (int)nominalIconY;

        float finalIconY = (float)textEnergyY + (float)fontSize / 2.0f - energyDrawHeight / 2.0f;

        // Desenha o Relâmpago
        Rectangle energySourceRec = { 0.0f, 0.0f, energyTextureWidth, energyTextureHeight };
        Rectangle energyDestRec = { finalIconX, finalIconY, energyDrawWidth, energyDrawHeight };

        DrawTexturePro(hud->energyIconTexture, energySourceRec, energyDestRec, (Vector2){ 0.0f, 0.0f }, 0.0f, WHITE);

        // --- Desenha o texto de porcentagem ---
        char chargeText[10];
        sprintf(chargeText, "%d%%", (int)round(energyCharge));

        int textEnergyX = (int)(finalIconX + energyDrawWidth + 5);

        DrawText(chargeText, textEnergyX, textEnergyY, fontSize, WHITE);

    } else {
        // --- LADO DIREITO: Apenas Score ---
        char scoreText[32];
        sprintf(scoreText, "SCORE: %06d", hud->score);
        int scoreTextWidth = MeasureText(scoreText, fontSize);
        int scoreTextX = screenW - scoreTextWidth - 10;
        int scoreTextY = targetY;
        DrawText(scoreText, scoreTextX, scoreTextY, fontSize, WHITE);
    }

    // Lógica de GAME OVER
    if (hud->lives <= 0) {
        int gameOverFontSize = 50;
        const char *gameOverText = "GAME OVER";
        int textWidth = MeasureText(gameOverText, gameOverFontSize);

        DrawText(gameOverText,
                 screenW / 2 - textWidth / 2,
                 GetScreenHeight() / 2 - gameOverFontSize / 2,
                 gameOverFontSize,
                 RED);
    }
}

void UnloadHud(Hud *hud) {
    if (hud->lifeIconTexture.id != 0) UnloadTexture(hud->lifeIconTexture);
    if (hud->energyIconTexture.id != 0) UnloadTexture(hud->energyIconTexture);
}