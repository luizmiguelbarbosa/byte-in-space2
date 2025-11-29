#ifndef ENEMY_H
#define ENEMY_H

#include "raylib.h"
#include "audio.h"

#define ENEMY_COLS 11
#define ENEMY_ROWS 5
#define ENEMY_COUNT (ENEMY_COLS * ENEMY_ROWS)
#define ENEMY_SIZE 40.0f
#define ENEMY_PADDING_X 15.0f
#define ENEMY_PADDING_Y 15.0f
#define ENEMY_SPEED_INITIAL 100.0f
#define ENEMY_DROP_AMOUNT 20.0f
#define ENEMY_FLASH_DURATION 0.1f
#define ENEMY_EXPLOSION_DURATION 0.4f
#define ENEMY_GAME_OVER_Y 550.0f
#define ENEMY_GAME_OVER_LINE_Y 500.0f

#define MAX_PARTICLES 500
#define PARTICLE_LIFESPAN 0.8f

typedef struct {
    Vector2 position;
    Vector2 velocity;
    Color color;
    float life;
    bool active;
} Particle;

typedef struct {
    Particle particles[MAX_PARTICLES];
    int nextParticleIndex;
} ParticleManager;

typedef struct {
    Vector2 position;
    Rectangle rect;
    int type;
    int health;
    bool active;
    float hitTimer;
    Color neonColor;
    bool isExploding;
    float explosionTimer;
} Enemy;

typedef struct {
    Enemy enemies[ENEMY_COUNT];
    Texture2D enemyTextures[3];
    float speed;
    int direction;
    int activeCount;
    bool gameOver;

    int currentWave;
    float waveStartTimer;

    int gameHeight;

    int wavesCompletedCount;
    bool triggerShopReturn;

    ParticleManager particleManager;
} EnemyManager;

typedef struct BulletManager BulletManager;

void InitEnemyManager(EnemyManager *manager, int screenWidth, int screenHeight);
void UpdateEnemies(EnemyManager *manager, float deltaTime, int screenWidth, int *playerLives, bool *gameOver);
void DrawEnemies(EnemyManager *manager);
void UnloadEnemyManager(EnemyManager *manager);

void CheckWaveCompletion(EnemyManager *manager, int screenWidth, int screenHeight);

void CheckBulletEnemyCollision(BulletManager *bulletManager, EnemyManager *enemyManager, int *playerGold, AudioManager *audioManager);

#endif