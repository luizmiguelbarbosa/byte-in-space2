#include "enemy.h"
#include "raylib.h"
#include "raymath.h"
#include "audio.h"
#include "bullet.h"
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

#define COLOR_NEON_BLUE (CLITERAL(Color){ 0, 191, 255, 255 })
#define COLOR_NEON_PURPLE (CLITERAL(Color){ 128, 0, 255, 255 })
#define COLOR_NEON_RED (CLITERAL(Color){ 255, 69, 0, 255 })
#define COLOR_EXPLOSION_ORANGE (CLITERAL(Color){ 255, 165, 0, 255 })
#define COLOR_NEON_GREEN (CLITERAL(Color){ 0, 255, 0, 255 })

#define EXPLOSION_PARTICLE_COUNT 100

#define WAVE_START_DURATION 3.0f
#define ENEMY_INITIAL_HEALTH_T1 1
#define ENEMY_INITIAL_HEALTH_T2 2
#define ENEMY_INITIAL_HEALTH_T3 4

void InitParticleManager(ParticleManager *manager) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        manager->particles[i].active = false;
    }
    manager->nextParticleIndex = 0;
}

Color GetRandomNeonColor() {
    int r = GetRandomValue(0, 5);
    switch (r) {
        case 0: return RED;
        case 1: return COLOR_EXPLOSION_ORANGE;
        case 2: return YELLOW;
        case 3: return VIOLET;
        case 4: return COLOR_NEON_BLUE;
        case 5: return LIME;
        default: return WHITE;
    }
}

void ExplodeEnemy(EnemyManager *manager, Vector2 position, int particleCount) {
    ParticleManager *pm = &manager->particleManager;
    for (int i = 0; i < particleCount; i++) {
        Particle *p = &pm->particles[pm->nextParticleIndex];

        p->active = true;
        p->position = position;
        p->life = PARTICLE_LIFESPAN;

        float speed = (float)GetRandomValue(150, 400);
        float angle = (float)GetRandomValue(0, 359);

        p->velocity.x = cosf(angle * DEG2RAD) * speed;
        p->velocity.y = sinf(angle * DEG2RAD) * speed;

        p->color = GetRandomNeonColor();

        pm->nextParticleIndex = (pm->nextParticleIndex + 1) % MAX_PARTICLES;
    }
}

void UpdateParticles(ParticleManager *manager, float deltaTime) {
    const int GAME_WIDTH = 800;
    const int GAME_HEIGHT = 600;

    for (int i = 0; i < MAX_PARTICLES; i++) {
        Particle *p = &manager->particles[i];
        if (!p->active) continue;

        p->life -= deltaTime;

        if (p->life <= 0.0f) {
            p->active = false;
        } else {
            p->position.x += p->velocity.x * deltaTime;
            p->position.y += p->velocity.y * deltaTime;

            if (p->position.x < 0 || p->position.x > GAME_WIDTH) {
                p->velocity.x *= -1;
                p->position.x = fmaxf(0, fminf(p->position.x, (float)GAME_WIDTH));
            }
            if (p->position.y < 0 || p->position.y > GAME_HEIGHT) {
                p->velocity.y *= -1;
                p->position.y = fmaxf(0, fminf(p->position.y, (float)GAME_HEIGHT));
            }

            p->velocity = Vector2Scale(p->velocity, 1.0f - (0.8f * deltaTime));
        }
    }
}

void DrawParticles(ParticleManager *manager) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        Particle *p = &manager->particles[i];
        if (!p->active) continue;

        float alpha = p->life / PARTICLE_LIFESPAN;
        Color drawColor = Fade(p->color, alpha);

        DrawCircleV(p->position, 3.0f, drawColor);
    }
}

void DrawEnemy(Enemy *enemy, Texture2D texture) {
    if (!enemy->active) return;

    const float BASE_AURA_RADIUS = ENEMY_SIZE * 0.8f;
    float time = (float)GetTime();
    float pulse = (sinf(time * 6.0f) + 1.0f) * 0.5f;
    float currentRadius = BASE_AURA_RADIUS + (pulse * 2.0f);

    Color outerColor = Fade(enemy->neonColor, 0.15f + (pulse * 0.05f));
    DrawRing(enemy->position, currentRadius * 0.85f, currentRadius * 1.0f, 0, 360, 30, outerColor);

    Color middleColor = Fade(enemy->neonColor, 0.4f);
    DrawRing(enemy->position, currentRadius * 0.75f, currentRadius * 0.85f, 0, 360, 30, middleColor);

    Color innerColor = Fade(WHITE, 0.6f);
    DrawRing(enemy->position, currentRadius * 0.7f, currentRadius * 0.75f, 0, 360, 30, innerColor);

    float w = ENEMY_SIZE;
    float h = ENEMY_SIZE;

    Rectangle destRec = { enemy->position.x - w / 2, enemy->position.y - h / 2, w, h };
    Vector2 origin = { 0.0f, 0.0f };

    Color tintColor = WHITE;
    if (enemy->hitTimer > 0.0f) {
        tintColor = RED;
    }

    DrawTexturePro(
        texture,
        (Rectangle){ 0.0f, 0.0f, (float)texture.width, (float)texture.height },
        destRec,
        origin,
        0.0f,
        tintColor
    );
}

void DrawExplosion(Enemy *enemy) {
    float progress = 1.0f - (enemy->explosionTimer / ENEMY_EXPLOSION_DURATION);
    float maxRadius = ENEMY_SIZE * 1.5f;
    float currentRadius = maxRadius * progress;

    Color outerColor = Fade(COLOR_EXPLOSION_ORANGE, 1.0f - progress);
    DrawCircle(
        (int)enemy->position.x,
        (int)enemy->position.y,
        currentRadius,
        outerColor
    );

    Color centerColor = Fade(YELLOW, 1.0f - progress * 0.5f);
    DrawCircle(
        (int)enemy->position.x,
        (int)enemy->position.y,
        currentRadius * 0.6f,
        centerColor
    );
}

void InitEnemiesForWave(EnemyManager *manager, int screenWidth, int screenHeight, int waveNumber) {
    float speedMultiplier = 1.0f + (waveNumber - 1) * 0.15f;
    manager->speed = ENEMY_SPEED_INITIAL * speedMultiplier;

    int healthBoost = (waveNumber - 1) / 3;

    float totalWidth = ENEMY_COLS * ENEMY_SIZE + (ENEMY_COLS - 1) * ENEMY_PADDING_X;
    float startX = (screenWidth - totalWidth) / 2.0f;
    float startY = 20.0f;

    manager->direction = 1;
    manager->activeCount = ENEMY_COUNT;
    manager->gameOver = false;
    manager->gameHeight = screenHeight;

    const float COLLISION_MARGIN = 5.0f;
    const float COLLISION_DIMENSION = ENEMY_SIZE + COLLISION_MARGIN * 2;

    for (int i = 0; i < ENEMY_COUNT; i++) {
        Enemy *enemy = &manager->enemies[i];
        int row = i / ENEMY_COLS;
        int col = i % ENEMY_COLS;

        enemy->position.x = startX + col * (ENEMY_SIZE + ENEMY_PADDING_X) + ENEMY_SIZE / 2.0f;
        enemy->position.y = startY + row * (ENEMY_SIZE + ENEMY_PADDING_Y) + ENEMY_SIZE / 2.0f;

        enemy->rect.width = COLLISION_DIMENSION;
        enemy->rect.height = COLLISION_DIMENSION;
        enemy->rect.x = enemy->position.x - COLLISION_DIMENSION / 2.0f;
        enemy->rect.y = enemy->position.y - COLLISION_DIMENSION / 2.0f;

        enemy->active = true;
        enemy->hitTimer = 0.0f;
        enemy->isExploding = false;
        enemy->explosionTimer = 0.0f;

        if (row == 0) {
            enemy->neonColor = COLOR_NEON_RED;
            enemy->type = 3;
            enemy->health = ENEMY_INITIAL_HEALTH_T3 + healthBoost;
        } else if (row <= 2) {
            enemy->neonColor = COLOR_NEON_PURPLE;
            enemy->type = 2;
            enemy->health = ENEMY_INITIAL_HEALTH_T2 + healthBoost;
        } else {
            enemy->neonColor = COLOR_NEON_BLUE;
            enemy->type = 1;
            enemy->health = ENEMY_INITIAL_HEALTH_T1 + healthBoost;
        }
    }
}

void InitEnemyManager(EnemyManager *manager, int screenWidth, int screenHeight) {
    manager->enemyTextures[0] = LoadTexture("assets/images/sprites/inimigo_1.png");
    manager->enemyTextures[1] = LoadTexture("assets/images/sprites/inimigo_2.png");
    manager->enemyTextures[2] = LoadTexture("assets/images/sprites/inimigo_3.png");

    manager->currentWave = 1;
    manager->waveStartTimer = WAVE_START_DURATION;
    manager->gameHeight = screenHeight;

    manager->wavesCompletedCount = 0;
    manager->triggerShopReturn = false;

    InitParticleManager(&manager->particleManager);

    InitEnemiesForWave(manager, screenWidth, screenHeight, manager->currentWave);
}

void CheckWaveCompletion(EnemyManager *manager, int screenWidth, int screenHeight) {
    if (manager->activeCount == 0) {

        manager->wavesCompletedCount++;

        if (manager->wavesCompletedCount % 3 == 0) {
            manager->triggerShopReturn = true;
        } else {
            manager->currentWave++;
            manager->waveStartTimer = WAVE_START_DURATION;
            InitEnemiesForWave(manager, screenWidth, screenHeight, manager->currentWave);
        }
    }
}

void UpdateEnemies(EnemyManager *manager, float deltaTime, int screenWidth, int *playerLives, bool *gameOver) {
    int screenHeight = manager->gameHeight;

    if (manager->waveStartTimer > 0.0f) {
        manager->waveStartTimer -= deltaTime;
        UpdateParticles(&manager->particleManager, deltaTime);
        if (manager->waveStartTimer > 0.0f) {
             return;
        }
    }

    if (manager->activeCount == 0 && !manager->triggerShopReturn) {
        CheckWaveCompletion(manager, screenWidth, screenHeight);
    }

    if (manager->triggerShopReturn || manager->waveStartTimer > 0.0f) {
        UpdateParticles(&manager->particleManager, deltaTime);
        return;
    }

    if (manager->gameOver) {
        UpdateParticles(&manager->particleManager, deltaTime);
        return;
    }

    UpdateParticles(&manager->particleManager, deltaTime);

    float moveAmount = manager->speed * manager->direction * deltaTime;
    bool shouldDrop = false;

    float minX = (float)screenWidth;
    float maxX = 0.0f;

    for (int i = 0; i < ENEMY_COUNT; i++) {
        Enemy *enemy = &manager->enemies[i];

        if (enemy->hitTimer > 0.0f) {
            enemy->hitTimer -= deltaTime;
        }

        if (enemy->isExploding) {
            enemy->explosionTimer -= deltaTime;

            if (enemy->explosionTimer <= 0.0f) {
                enemy->isExploding = false;
            }
        }

        if (!enemy->active && !enemy->isExploding) continue;

        if (enemy->active) {
            minX = fminf(minX, enemy->rect.x);
            maxX = fmaxf(maxX, enemy->rect.x + enemy->rect.width);
        }
    }

    const float margin = 2.0f;

    if (manager->direction == 1) {
        if (maxX >= screenWidth - margin) {
            shouldDrop = true;
        }
    } else {
        if (minX <= margin) {
            shouldDrop = true;
        }
    }

    for (int i = 0; i < ENEMY_COUNT; i++) {
        Enemy *enemy = &manager->enemies[i];
        if (!enemy->active) continue;

        enemy->position.x += moveAmount;
        enemy->rect.x = enemy->position.x - enemy->rect.width / 2.0f;
    }

    if (shouldDrop) {
        manager->direction *= -1;
        manager->speed *= 1.02f;

        for (int i = 0; i < ENEMY_COUNT; i++) {
            Enemy *enemy = &manager->enemies[i];
            if (!enemy->active) continue;

            enemy->position.y += ENEMY_DROP_AMOUNT;
            enemy->rect.y = enemy->position.y - enemy->rect.height / 2.0f;

            if (enemy->position.y >= ENEMY_GAME_OVER_LINE_Y) {

                enemy->active = false;
                manager->activeCount--;

                if (*playerLives > 0) {
                    (*playerLives)--;
                }

                if (*playerLives <= 0) {
                    *playerLives = 0;
                    manager->gameOver = true;
                    (*gameOver) = true;
                }
            }
        }
    }
}

void DrawEnemies(EnemyManager *manager) {
    DrawParticles(&manager->particleManager);

    for (int i = 0; i < ENEMY_COUNT; i++) {
        Enemy *enemy = &manager->enemies[i];

        if (enemy->isExploding) {
            DrawExplosion(enemy);

        } else if (enemy->active) {
            Texture2D texture;
            switch (enemy->type) {
                case 1: texture = manager->enemyTextures[0]; break;
                case 2: texture = manager->enemyTextures[1]; break;
                case 3: texture = manager->enemyTextures[2]; break;
                default: texture = manager->enemyTextures[0]; break;
            }
            DrawEnemy(enemy, texture);
        }
    }
}

void UnloadEnemyManager(EnemyManager *manager) {
    for (int i = 0; i < 3; i++) {
        if (manager->enemyTextures[i].id != 0) UnloadTexture(manager->enemyTextures[i]);
    }
}

void CheckBulletEnemyCollision(BulletManager *bulletManager, EnemyManager *enemyManager, int *playerGold, AudioManager *audioManager) {
    for (int i = 0; i < MAX_PLAYER_BULLETS; i++) {
        Bullet *bullet = &bulletManager->bullets[i];

        if (!bullet->active) continue;

        Vector2 bulletCenter = {
            bullet->rect.x + bullet->rect.width / 2.0f,
            bullet->rect.y + bullet->rect.height / 2.0f
        };
        float bulletRadius = bullet->rect.width / 2.0f;

        for (int j = 0; j < ENEMY_COUNT; j++) {
            Enemy *enemy = &enemyManager->enemies[j];

            if (!enemy->active || enemy->isExploding) continue;

            float enemyRadius = ENEMY_SIZE / 2.0f;

            if (CheckCollisionCircles(bulletCenter, bulletRadius, enemy->position, enemyRadius)) {

                bullet->active = false;

                enemy->health--;
                enemy->hitTimer = ENEMY_FLASH_DURATION;

                if (enemy->health <= 0) {
                    enemy->active = false;
                    enemy->isExploding = true;
                    enemy->explosionTimer = ENEMY_EXPLOSION_DURATION;
                    enemyManager->activeCount--;

                    // Ouro reduzido: Gold = 2 + (Tipo do Inimigo * 2)
                    *playerGold += 2 + (enemy->type * 2);

                    PlaySound(audioManager->sfxExplosionEnemy);

                    ExplodeEnemy(enemyManager, enemy->position, EXPLOSION_PARTICLE_COUNT);

                } else {
                    PlaySound(audioManager->sfxWeak);
                }

                break;
            }
        }
    }
}