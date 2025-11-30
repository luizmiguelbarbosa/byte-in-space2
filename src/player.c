#include "player.h"
#include "raylib.h"
#include "raymath.h"
#include "audio.h"
#include "bullet.h"
#include <stdio.h>

#define PLAYER_MOVE_SPEED 400.0f
#define BLACK_AREA_START_Y 480.0f

#define CHARGE_RATE 14.286f
#define MAX_CHARGE 100.0f
#define MEDIUM_THRESHOLD 50.0f

#define AURA_MAX_RADIUS_FACTOR 0.8f
#define AURA_MIN_ALPHA 0.3f
#define AURA_MAX_ALPHA 0.8f
#define AURA_PULSE_SPEED 5.0f
#define FIRE_AURA_RADIUS_INCREASE 1.1f

#define BASE_SPRITE_PATH "assets/images/sprites/byte_1.png"
#define SHURIKEN_SPRITE_PATH "assets/images/sprites/byte_2.png"
#define SHIELD_SPRITE_PATH "assets/images/sprites/byte_shield.png"
#define EXTRA_LIFE_SPRITE_PATH "assets/images/sprites/byte_4.png"

#define COLOR_WEAK (CLITERAL(Color){ 0, 191, 255, 255 })
#define COLOR_MEDIUM (CLITERAL(Color){ 128, 0, 255, 255 })
#define COLOR_FIRE (CLITERAL(Color){ 255, 69, 0, 255 })

int CalculateAttackType(float charge) {
    if (charge >= MAX_CHARGE) {
        return ATTACK_STRONG;
    }
    else if (charge >= MEDIUM_THRESHOLD) {
        return ATTACK_MEDIUM;
    }
    else {
        return ATTACK_WEAK;
    }
}

void InitPlayer(Player *player) {
    player->baseTexture = LoadTexture(BASE_SPRITE_PATH);
    player->shurikenTexture = LoadTexture(SHURIKEN_SPRITE_PATH);
    player->shieldTextureAppearance = LoadTexture(SHIELD_SPRITE_PATH);
    player->extraLifeTextureAppearance = LoadTexture(EXTRA_LIFE_SPRITE_PATH);

    player->texture = player->baseTexture;

    player->scale = PLAYER_SCALE;
    player->speed = PLAYER_MOVE_SPEED;

    player->gold = 0;

    player->energyCharge = 0.0f;
    player->isCharging = false;
    player->canCharge = false;

    player->hasDoubleShot = false;
    player->hasShield = false;
    player->extraLives = 0;

    float player_width_scaled = player->texture.width * player->scale;
    float player_height_scaled = (float)player->texture.height * player->scale;

    player->position = (Vector2){
        (float)GetScreenWidth()/2 - player_width_scaled/2,
        (float)GetScreenHeight() - player_height_scaled - 10.0f
    };

    if (player->baseTexture.id != 0) SetTextureFilter(player->baseTexture, TEXTURE_FILTER_POINT);
    if (player->shurikenTexture.id != 0) SetTextureFilter(player->shurikenTexture, TEXTURE_FILTER_POINT);
    if (player->shieldTextureAppearance.id != 0) SetTextureFilter(player->shieldTextureAppearance, TEXTURE_FILTER_POINT);
    if (player->extraLifeTextureAppearance.id != 0) SetTextureFilter(player->extraLifeTextureAppearance, TEXTURE_FILTER_POINT);

    player->texture = player->baseTexture;

    player->auraRadius = player_width_scaled * 0.5f;
    player->auraAlpha = 0.0f;
    player->auraPulseSpeed = AURA_PULSE_SPEED;

}

void UpdatePlayer(Player *player, BulletManager *bulletManager, AudioManager *audioManager, Hud *hud, float deltaTime, int screenWidth, int screenHeight) {
    float move_dist = player->speed * deltaTime;

    if (IsKeyDown(KEY_LEFT)) player->position.x -= move_dist;
    if (IsKeyDown(KEY_RIGHT)) player->position.x += move_dist;
    if (IsKeyDown(KEY_UP)) player->position.y -= move_dist;
    if (IsKeyDown(KEY_DOWN)) player->position.y += move_dist;

    float ship_width = player->texture.width * player->scale;
    float ship_height = player->texture.height * player->scale;
    Vector2 playerCenter = {
        player->position.x + ship_width / 2,
        player->position.y + ship_height / 2
    };

    if (audioManager != NULL && bulletManager != NULL) {
        if (IsKeyDown(KEY_SPACE)) {
            player->isCharging = true;
            player->energyCharge = Clamp(player->energyCharge + CHARGE_RATE * deltaTime, 0.0f, MAX_CHARGE);

            if (!IsSoundPlaying(audioManager->sfxCharge)) {
                PlaySound(audioManager->sfxCharge);
            }

            float baseRadius = ship_width / 2.0f;
            float maxIncrease = (ship_width * AURA_MAX_RADIUS_FACTOR) - baseRadius;
            player->auraRadius = baseRadius + (maxIncrease * (player->energyCharge / MAX_CHARGE));

            float pulseFactor = (sin(GetTime() * player->auraPulseSpeed) * 0.5f + 0.5f);
            player->auraAlpha = AURA_MIN_ALPHA + pulseFactor * (AURA_MAX_ALPHA - AURA_MIN_ALPHA);

        } else if (IsKeyReleased(KEY_SPACE) && player->energyCharge > 0.0f) {
            player->isCharging = false;

            int attackType = CalculateAttackType(player->energyCharge);

            // Chamada atualizada para incluir o status do power-up
            ShootChargedAttack(bulletManager, playerCenter, ship_height, attackType, player->hasDoubleShot);

            PlayAttackSfx(audioManager, attackType);

            player->energyCharge = 0.0f;
        }

        if (!IsKeyDown(KEY_SPACE)) {
            player->isCharging = false;
            player->energyCharge = 0.0f;
            StopSound(audioManager->sfxCharge);
        }
    }

    player->position.x = Clamp(player->position.x, 0.0f, (float)screenWidth - ship_width);
    player->position.y = Clamp(player->position.y, BLACK_AREA_START_Y, (float)screenHeight - ship_height);
}

void DrawPlayer(Player *player) {
    if (player->texture.id == 0) return;

    float ship_width = player->texture.width * player->scale;
    float ship_height = player->texture.height * player->scale;
    Vector2 playerCenter = {
        player->position.x + ship_width / 2,
        player->position.y + ship_height / 2
    };

    float firePulse = (sin(GetTime() * 20.0f) * 0.1f + 0.9f);
    float fireRadius = ship_width * 0.08f / player->scale * firePulse;

    Vector2 firePos = { playerCenter.x, player->position.y + ship_height - (ship_height / 10.0f) };

    DrawCircleGradient(
        (int)firePos.x, (int)firePos.y, fireRadius * 1.5f,
        Fade(RED, 0.5f), Fade(RED, 0.0f)
    );
    DrawCircleGradient(
        (int)firePos.x, (int)firePos.y, fireRadius,
        Fade(YELLOW, 0.9f), Fade(RED, 0.0f)
    );

    if (player->isCharging) {
        Color baseColor;
        bool isMaxCharge = (player->energyCharge >= MAX_CHARGE);

        if (player->energyCharge >= MEDIUM_THRESHOLD) {
            baseColor = COLOR_MEDIUM;
        } else {
            baseColor = COLOR_WEAK;
        }

        Color auraColor = {
            baseColor.r,
            baseColor.g,
            baseColor.b,
            (unsigned char)(player->auraAlpha * 255.0f)
        };

        if (isMaxCharge) {
            Color fireColor = {
                COLOR_FIRE.r,
                COLOR_FIRE.g,
                COLOR_FIRE.b,
                (unsigned char)(player->auraAlpha * 255.0f)
            };

            DrawCircleGradient(
                (int)playerCenter.x,
                (int)playerCenter.y,
                player->auraRadius * FIRE_AURA_RADIUS_INCREASE,
                Fade(fireColor, 0.7f),
                Fade(fireColor, 0.0f)
            );
        }

        DrawCircleGradient(
            (int)playerCenter.x,
            (int)playerCenter.y,
            player->auraRadius,
            Fade(auraColor, 0.8f),
            Fade(auraColor, 0.0f)
        );
    }

    DrawTextureEx(player->texture, player->position, 0.0f, player->scale, WHITE);
}

void UnloadPlayer(Player *player) {
    if (player->baseTexture.id != 0) UnloadTexture(player->baseTexture);
    if (player->shurikenTexture.id != 0) UnloadTexture(player->shurikenTexture);
    if (player->shieldTextureAppearance.id != 0) UnloadTexture(player->shieldTextureAppearance);
    if (player->extraLifeTextureAppearance.id != 0) UnloadTexture(player->extraLifeTextureAppearance);
}