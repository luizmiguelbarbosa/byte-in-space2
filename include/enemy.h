#ifndef ENEMY_H
#define ENEMY_H

#include "raylib.h"
#include "audio.h"

// --- Constantes do Inimigo Normal ---
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
#define ENEMY_GAME_OVER_LINE_Y 550.0f // Linha verde restaurada

// --- Constantes de Partículas ---
#define MAX_PARTICLES 500
#define PARTICLE_LIFESPAN 0.8f

// --- Constantes do Boss ---
#define BOSS_FRAME_COUNT 30
#define BOSS_ANIMATION_SPEED 0.08f
#define BOSS_INITIAL_HEALTH 5500
#define BOSS_SIZE_WIDTH 128.0f
#define BOSS_SIZE_HEIGHT 128.0f

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

// --- Estrutura para o Boss ---
typedef struct Boss {
    bool active;
    Vector2 position;
    int health;
    int maxHealth;
    float hitTimer;
    Rectangle rect;

    // Animação
    int currentFrame;
    float frameTimer;

    // Movimento
    float movementTimer;
    Vector2 targetPosition;

} Boss;

// --- EnemyManager ---
typedef struct EnemyManager {
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

    // Boss
    Boss boss;
    Texture2D bossFrames[BOSS_FRAME_COUNT];
    bool bossActive;
} EnemyManager;

typedef struct BulletManager BulletManager;
typedef struct AudioManager AudioManager;

void InitEnemyManager(EnemyManager *manager, int screenWidth, int screenHeight);
void UpdateEnemies(EnemyManager *manager, float deltaTime, int screenWidth, int *playerLives, bool *gameOver);
void DrawEnemies(EnemyManager *manager);
void UnloadEnemyManager(EnemyManager *manager);

void CheckWaveCompletion(EnemyManager *manager, int screenWidth, int screenHeight);

void CheckBulletEnemyCollision(BulletManager *bulletManager, EnemyManager *enemyManager, int *playerGold, AudioManager *audioManager);

#endif