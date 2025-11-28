#ifndef ENEMY_H
#define ENEMY_H

#include "raylib.h"
#include "audio.h" // Incluído para a nova função de colisão

// Definições e constantes para inimigos
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
#define ENEMY_GAME_OVER_Y 550.0f // Posição Y que define o Game Over

// Partículas
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

// Estrutura do Inimigo
typedef struct {
    Vector2 position;
    Rectangle rect;
    int type; // 1 (fraco), 2 (médio), 3 (líder)
    int health;
    bool active;
    float hitTimer;
    Color neonColor;
    bool isExploding;
    float explosionTimer;
} Enemy;

// Estrutura do Gerenciador de Inimigos
typedef struct {
    Enemy enemies[ENEMY_COUNT];
    Texture2D enemyTextures[3];
    float speed;
    int direction; // 1 (direita), -1 (esquerda)
    int activeCount;
    bool gameOver;

    // Sistema de Waves
    int currentWave;
    float waveStartTimer;

    ParticleManager particleManager;
} EnemyManager;

// Forward declaration para BulletManager.
// Isso permite usar 'BulletManager *' nas assinaturas de função abaixo.
// NOTA: Se 'bullet.h' define BulletManager como 'typedef struct { ... } BulletManager;',
// esta declaração não causará conflito, mas é importante que 'bullet.h' não use 'typedef struct BulletManager BulletManager;'
typedef struct BulletManager BulletManager;


// --- Funções ---

// Gerenciador de Inimigos
void InitEnemyManager(EnemyManager *manager, int screenWidth, int screenHeight);
void UpdateEnemies(EnemyManager *manager, float deltaTime, int screenWidth);
void DrawEnemies(EnemyManager *manager);
void UnloadEnemyManager(EnemyManager *manager);

// Lógica de Colisão
// ASSINATURA CORRIGIDA: Usa o 'typedef' BulletManager que foi forward-declared acima.
void CheckBulletEnemyCollision(BulletManager *bulletManager, EnemyManager *enemyManager, int *playerGold, AudioManager *audioManager);

#endif // ENEMY_H