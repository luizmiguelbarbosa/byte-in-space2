#ifndef BULLET_H
#define BULLET_H

#include "raylib.h"
#include <stdbool.h>

#define MAX_PLAYER_BULLETS 20

#define ATTACK_WEAK 1
#define ATTACK_MEDIUM 2
#define ATTACK_STRONG 3

typedef struct {
    Rectangle rect;
    Vector2 speed;
    bool active;
    Color color;
    int type;
} Bullet;

typedef struct {
    Bullet bullets[MAX_PLAYER_BULLETS];
    Texture2D weakTexture;
    Texture2D mediumTexture;
    Texture2D strongTexture;
} BulletManager;

void InitBulletManager(BulletManager *manager);
void ShootChargedAttack(BulletManager *manager, Vector2 playerCenter, float playerHeight, int attackType);
void UpdatePlayerBullets(BulletManager *manager, float deltaTime);
void DrawPlayerBullets(BulletManager *manager);
void UnloadBulletManager(BulletManager *manager);

#endif // BULLET_H