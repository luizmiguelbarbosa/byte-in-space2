#include "hud.h"
#include "raylib.h"
#include <stdio.h>
#include <math.h>

// --- DEFINIÇÕES DE CAMINHOS ---
#define LIFE_ICON_PATH "assets/images/sprites/life_icon_large.png"
#define ENERGY_ICON_PATH "assets/images/sprites/energy_icon.png"

// --- PATHS DOS POWER-UPS ---
// #define DOUBLE_SHOT_PATH "assets/images/sprites/tiroduplo.png" // REMOVIDO
#define SHURIKEN_PATH "assets/images/sprites/icone_powerup_shurikens.png" // NOVO: Shurikens
#define SHIELD_PATH "assets/images/sprites/shield.png"
// O ícone de vida extra é o mesmo do ícone de vida


// --- ESCALAS PEQUENAS PARA DESENHO DA HUD (AJUSTE FINAL) ---
#define ICON_HUD_SCALE 0.12f

void InitHud(Hud *hud) {
    hud->score = 0;
    hud->lives = 3;

    // Carrega Texturas Base
    hud->lifeIconTexture = LoadTexture(LIFE_ICON_PATH);
    if (hud->lifeIconTexture.id != 0) SetTextureFilter(hud->lifeIconTexture, TEXTURE_FILTER_POINT);
    else printf("[ERRO] Icone de Vida nao encontrado: %s\n", LIFE_ICON_PATH);

    hud->energyIconTexture = LoadTexture(ENERGY_ICON_PATH);
    if (hud->energyIconTexture.id != 0) SetTextureFilter(hud->energyIconTexture, TEXTURE_FILTER_POINT);
    else printf("[ERRO] Icone de Energia nao encontrado: %s\n", ENERGY_ICON_PATH);

    // Carrega Texturas dos Power-ups
    // MUDANÇA: Carregando Shurikens
    hud->shurikenTexture = LoadTexture(SHURIKEN_PATH);
    if (hud->shurikenTexture.id != 0) SetTextureFilter(hud->shurikenTexture, TEXTURE_FILTER_POINT);
    else printf("[ERRO] Icone de Shurikens nao encontrado: %s\n", SHURIKEN_PATH);

    hud->shieldTexture = LoadTexture(SHIELD_PATH);
    if (hud->shieldTexture.id != 0) SetTextureFilter(hud->shieldTexture, TEXTURE_FILTER_POINT);
    else printf("[ERRO] Icone de Escudo nao encontrado: %s\n", SHIELD_PATH);

}

void UpdateHud(Hud *hud, float deltaTime) {
    // Vazio.
}

// NOVO: Adicionei os estados dos power-ups
void DrawHudSide(Hud *hud, bool isLeft, int marginHeight, float energyCharge, bool hasDoubleShot, bool hasShield, int extraLives) {
    int fontSize = 20;
    int screenW = GetScreenWidth();
    int targetY = 10;

    // --- Cálculo das dimensões dos ícones (Todos usarão a mesma escala para padronizar) ---
    float iconTextureWidth = 64.0f; // Exemplo de largura de ícone base
    float iconTextureHeight = 64.0f; // Exemplo de altura de ícone base
    // Assume-se que a textura de vida/energia será a base para o tamanho
    if (hud->lifeIconTexture.id != 0) iconTextureWidth = (float)hud->lifeIconTexture.width;
    if (hud->lifeIconTexture.id != 0) iconTextureHeight = (float)hud->lifeIconTexture.height;

    // Dimensões FINAIS (escaladas) para o desenho
    float iconDrawWidth = iconTextureWidth * ICON_HUD_SCALE;
    float iconDrawHeight = iconTextureHeight * ICON_HUD_SCALE;

    if (isLeft) {
        // --- LADO ESQUERDO: Coração/Vidas, Carga de Energia e Power-ups ---
        int iconX = 10;
        float currentY = (float)targetY;

        // 1. CORAÇÃO/VIDAS (Topo)
        if (hud->lifeIconTexture.id != 0) {
            Rectangle sourceRecLife = { 0.0f, 0.0f, iconTextureWidth, iconTextureHeight };
            Rectangle destRecLife = { (float)iconX, currentY, iconDrawWidth, iconDrawHeight };
            DrawTexturePro(hud->lifeIconTexture, sourceRecLife, destRecLife, (Vector2){ 0.0f, 0.0f }, 0.0f, WHITE);
        }

        char livesText[10];
        // Adiciona as vidas base + vidas extras compradas
        sprintf(livesText, "x%02d", hud->lives + extraLives);

        int textX = iconX + (int)iconDrawWidth + 5;
        int textY = (int)currentY + (int)iconDrawHeight / 2 - fontSize / 2;
        DrawText(livesText, textX, textY, fontSize, WHITE);
        currentY += iconDrawHeight + 15.0f;


        // 2. ÍCONE DE ENERGIA E PORCENTAGEM
        if (hud->energyIconTexture.id != 0) {
            // Desenha o Relâmpago
            Rectangle energySourceRec = { 0.0f, 0.0f, (float)hud->energyIconTexture.width, (float)hud->energyIconTexture.height };
            Rectangle energyDestRec = { (float)iconX, currentY, iconDrawWidth, iconDrawHeight };

            // Desenha o ícone de energia, mas ajusta a cor/alfa se não estiver carregando
            Color energyColor = (energyCharge > 0.0f) ? WHITE : Fade(WHITE, 0.5f);
            DrawTexturePro(hud->energyIconTexture, energySourceRec, energyDestRec, (Vector2){ 0.0f, 0.0f }, 0.0f, energyColor);

            // Desenha o texto de porcentagem
            char chargeText[10];
            sprintf(chargeText, "%d%%", (int)round(energyCharge));

            int textEnergyX = iconX + (int)iconDrawWidth + 5;
            int textEnergyY = (int)currentY + (int)iconDrawHeight / 2 - fontSize / 2;
            DrawText(chargeText, textEnergyX, textEnergyY, fontSize, WHITE);
            currentY += iconDrawHeight + 15.0f;
        }

        // 3. POWER-UPS ATIVOS (SOLICITAÇÃO 3: HUD)
        // (O ícone de Carga de Energia já foi desenhado acima)

        // SHURIKENS (Substituindo TIRO DUPLO)
        if (hasDoubleShot) { // Mantém a flag hasDoubleShot, que agora representa as Shurikens
            if (hud->shurikenTexture.id != 0) {
                Rectangle source = { 0.0f, 0.0f, (float)hud->shurikenTexture.width, (float)hud->shurikenTexture.height };
                Rectangle dest = { (float)iconX, currentY, iconDrawWidth, iconDrawHeight };
                DrawTexturePro(hud->shurikenTexture, source, dest, (Vector2){ 0.0f, 0.0f }, 0.0f, WHITE);
                currentY += iconDrawHeight + 10.0f;
            }
        }

        // ESCUDO
        if (hasShield) {
            if (hud->shieldTexture.id != 0) {
                Rectangle source = { 0.0f, 0.0f, (float)hud->shieldTexture.width, (float)hud->shieldTexture.height };
                Rectangle dest = { (float)iconX, currentY, iconDrawWidth, iconDrawHeight };
                DrawTexturePro(hud->shieldTexture, source, dest, (Vector2){ 0.0f, 0.0f }, 0.0f, WHITE);
                currentY += iconDrawHeight + 10.0f;
            }
        }

    } else {
        // --- LADO DIREITO: Apenas Score ---
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
    // MUDANÇA: Descarregando Shurikens
    if (hud->shurikenTexture.id != 0) UnloadTexture(hud->shurikenTexture);
    if (hud->shieldTexture.id != 0) UnloadTexture(hud->shieldTexture);
}