#include "bullet.h"
#include "raylib.h"
#include <stdio.h>
#include <stdbool.h>

// --- ESCALAS PROGRESSIVAS (Fraca < Média < Forte) ---
#define BULLET_SCALE_WEAK 0.08f
#define BULLET_SCALE_MEDIUM 0.12f
#define BULLET_SCALE_STRONG 0.16f

#define WEAK_SPRITE_PATH "assets/images/sprites/ataque_fraco.png"
#define MEDIUM_SPRITE_PATH "assets/images/sprites/ataque_medio.png"
#define STRONG_SPRITE_PATH "assets/images/sprites/ataque_forte.png"


void InitBulletManager(BulletManager *manager) {
    manager->weakTexture = LoadTexture(WEAK_SPRITE_PATH);
    manager->mediumTexture = LoadTexture(MEDIUM_SPRITE_PATH);
    manager->strongTexture = LoadTexture(STRONG_SPRITE_PATH);

    if (manager->weakTexture.id != 0) SetTextureFilter(manager->weakTexture, TEXTURE_FILTER_POINT);
    if (manager->mediumTexture.id != 0) SetTextureFilter(manager->mediumTexture, TEXTURE_FILTER_POINT);
    if (manager->strongTexture.id != 0) SetTextureFilter(manager->strongTexture, TEXTURE_FILTER_POINT);

    for (int i = 0; i < MAX_PLAYER_BULLETS; i++) {
        manager->bullets[i].active = false;
        manager->bullets[i].color = WHITE;
        manager->bullets[i].speed = (Vector2){ 0, 0 };
    }
}

void ShootChargedAttack(BulletManager *manager, Vector2 playerCenter, float playerHeight, int attackType) {
    for (int i = 0; i < MAX_PLAYER_BULLETS; i++) {
        if (!manager->bullets[i].active) {
            Bullet *bullet = &manager->bullets[i];

            bullet->active = true;
            bullet->type = attackType;

            Texture2D currentTexture;
            float speed = 0.0f;
            float currentScale = 0.0f;

            switch (attackType) {
                case ATTACK_STRONG:
                    currentTexture = manager->strongTexture;
                    speed = 700.0f;
                    currentScale = BULLET_SCALE_STRONG;
                    break;
                case ATTACK_MEDIUM:
                    currentTexture = manager->mediumTexture;
                    speed = 600.0f;
                    currentScale = BULLET_SCALE_MEDIUM;
                    break;
                case ATTACK_WEAK: default:
                    currentTexture = manager->weakTexture;
                    speed = 500.0f;
                    currentScale = BULLET_SCALE_WEAK;
                    break;
            }

            bullet->speed = (Vector2){ 0, -speed };

            bullet->rect.width = (float)currentTexture.width * currentScale;
            bullet->rect.height = (float)currentTexture.height * currentScale;

            bullet->rect.x = playerCenter.x - (bullet->rect.width / 2);
            bullet->rect.y = playerCenter.y - (playerHeight / 2) - bullet->rect.height;

            return;
        }
    }
}

void UpdatePlayerBullets(BulletManager *manager, float deltaTime) {
    for (int i = 0; i < MAX_PLAYER_BULLETS; i++) {
        if (manager->bullets[i].active) {
            Bullet *bullet = &manager->bullets[i];

            bullet->rect.y += bullet->speed.y * deltaTime;

            if (bullet->rect.y < -bullet->rect.height) {
                bullet->active = false;
            }
        }
    }
}

void DrawPlayerBullets(BulletManager *manager) {
    Texture2D currentTexture;

    for (int i = 0; i < MAX_PLAYER_BULLETS; i++) {
        if (manager->bullets[i].active) {
            Bullet *bullet = &manager->bullets[i];

            switch (bullet->type) {
                case ATTACK_STRONG: currentTexture = manager->strongTexture; break;
                case ATTACK_MEDIUM: currentTexture = manager->mediumTexture; break;
                case ATTACK_WEAK: default: currentTexture = manager->weakTexture; break;
            }

            Rectangle sourceRec = { 0.0f, 0.0f, (float)currentTexture.width, (float)currentTexture.height };

            DrawTexturePro(
                currentTexture,
                sourceRec,
                (Rectangle){ bullet->rect.x, bullet->rect.y, bullet->rect.width, bullet->rect.height },
                (Vector2){ 0.0f, 0.0f },
                0.0f,
                WHITE
            );
        }
    }
}

void UnloadBulletManager(BulletManager *manager) {
    if (manager->weakTexture.id != 0) UnloadTexture(manager->weakTexture);
    if (manager->mediumTexture.id != 0) UnloadTexture(manager->mediumTexture);
    if (manager->strongTexture.id != 0) UnloadTexture(manager->strongTexture);
}