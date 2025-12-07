#include "hud.h"
#include "raylib.h"
#include <stdio.h>
#include <math.h>

#define LIFE_ICON_PATH "assets/images/sprites/life_icon_large.png"
#define ENERGY_ICON_PATH "assets/images/sprites/energy_icon.png"

#define SHURIKEN_PATH "assets/images/sprites/icone_powerup_shurikens.png"
#define SHIELD_PATH "assets/images/sprites/shield.png"

// --- ADICIONADO: Caminho para a textura do Gold (Ouro) ---
#define GOLD_PATH "assets/images/sprites/gold.png"
// ---------------------------------------------------------

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

    // --- ADICIONADO: Carregar Textura do Gold (Ouro) ---
    hud->goldTexture = LoadTexture(GOLD_PATH);
    if (hud->goldTexture.id != 0) SetTextureFilter(hud->goldTexture, TEXTURE_FILTER_POINT);
    else printf("[ERRO] Icone de Gold nao encontrado: %s\n", GOLD_PATH);
    // ----------------------------------------------------
}

void UpdateHud(Hud *hud, float deltaTime) {
}

// --- CORREÇÃO: Assinatura da função DrawHudSide com o parâmetro 'currentGold' ---
void DrawHudSide(Hud *hud, bool isLeft, int marginHeight, float energyCharge, bool hasDoubleShot, bool hasShield, int extraLives, int drawLives, int currentGold) {
// ------------------------------------------------------------------------------------------------------------------------------------------------------
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

        char livesText[10];
        sprintf(livesText, "x%02d", drawLives + extraLives);

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

    } else { // Lado direito da tela (Apenas Gold)
        float currentY = (float)targetY;

        // --- REMOVIDO: Desenho do Score (bloco removido) ---

        // --- ADICIONADO/AJUSTADO: Desenho do Gold (Ouro) no topo ---
        if (hud->goldTexture.id != 0) {
            // 1. Prepara o texto do Gold (Ouro)
            char goldText[32];
            sprintf(goldText, "%d", currentGold);
            int goldTextWidth = MeasureText(goldText, fontSize);

            // 2. Calcula a posição do texto (alinha à direita)
            int textGoldX = screenW - goldTextWidth - 10;
            int textGoldY = (int)currentY + (int)iconDrawHeight / 2 - fontSize / 2;

            // 3. Calcula a posição do ícone (o ícone fica à esquerda do texto)
            int iconX = textGoldX - (int)iconDrawWidth - 5; // 5 é a margem entre o ícone e o texto

            // 4. Desenha o ícone
            Rectangle sourceRecGold = { 0.0f, 0.0f, (float)hud->goldTexture.width, (float)hud->goldTexture.height };
            Rectangle destRecGold = { (float)iconX, currentY, iconDrawWidth, iconDrawHeight };
            DrawTexturePro(hud->goldTexture, sourceRecGold, destRecGold, (Vector2){ 0.0f, 0.0f }, 0.0f, WHITE);

            // 5. Desenha o texto do Gold
            DrawText(goldText, textGoldX, textGoldY, fontSize, WHITE);
            currentY += iconDrawHeight + 10.0f;
        }
        // -----------------------------------------------------------
    }
}

void UnloadHud(Hud *hud) {
    if (hud->lifeIconTexture.id != 0) UnloadTexture(hud->lifeIconTexture);
    if (hud->energyIconTexture.id != 0) UnloadTexture(hud->energyIconTexture);
    if (hud->shurikenTexture.id != 0) UnloadTexture(hud->shurikenTexture);
    if (hud->shieldTexture.id != 0) UnloadTexture(hud->shieldTexture);
    // --- ADICIONADO: Descarregar Textura do Gold (Ouro) ---
    if (hud->goldTexture.id != 0) UnloadTexture(hud->goldTexture);
    // ------------------------------------------------------
}