#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include "bullet.h"
#include "hud.h"

typedef struct AudioManager AudioManager;

#define PLAYER_SCALE 0.2f
#define ATTACK_WEAK 1
#define ATTACK_MEDIUM 2
#define ATTACK_STRONG 3

typedef struct {
    Texture2D texture;
    Texture2D baseTexture;
    Texture2D shurikenTexture;
    Texture2D shieldTextureAppearance; // NOVA
    Texture2D extraLifeTextureAppearance; // NOVA
    Vector2 position;
    float speed;
    float scale;

    int gold;

    float energyCharge;
    bool isCharging;
    bool canCharge;

    bool hasDoubleShot;
    bool hasShield;
    int extraLives;

    float auraRadius;
    float auraAlpha;
    float auraPulseSpeed;

} Player;

void InitPlayer(Player *player);
void UpdatePlayer(Player *player, BulletManager *bulletManager, AudioManager *audioManager, Hud *hud, float deltaTime, int screenWidth, int screenHeight);
void DrawPlayer(Player *player);
void UnloadPlayer(Player *player);

int CalculateAttackType(float charge);

#endif // PLAYER_H