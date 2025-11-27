#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include "bullet.h"
#include "hud.h"

// --- DECLARAÇÃO FORWARD para AudioManager ---
typedef struct AudioManager AudioManager;

#define PLAYER_SCALE 0.2f
#define ATTACK_WEAK 1
#define ATTACK_MEDIUM 2
#define ATTACK_STRONG 3

typedef struct {
    Texture2D texture;
    Vector2 position;
    float speed;
    float scale;

    int gold; // DINHEIRO DO JOGADOR

    float energyCharge;
    bool isCharging;
    bool canCharge; // NOVO: Habilita o tiro carregado (o power-up FREE)

    // NOVOS: Power-ups comprados
    bool hasDoubleShot;
    bool hasShield;
    int extraLives;

    // --- Propriedades da Aura ---
    float auraRadius;
    float auraAlpha;
    float auraPulseSpeed;

} Player;

void InitPlayer(Player *player);
// Nota: Passamos menos argumentos para UpdatePlayer na STATE_SHOP
void UpdatePlayer(Player *player, BulletManager *bulletManager, AudioManager *audioManager, Hud *hud, float deltaTime, int screenWidth, int screenHeight);
void DrawPlayer(Player *player);
void UnloadPlayer(Player *player);

int CalculateAttackType(float charge);

#endif // PLAYER_H