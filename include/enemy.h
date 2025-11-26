#ifndef ENEMY_H
#define ENEMY_H

#include "raylib.h"

// Dimensões da Grade (Space Invaders)
#define ENEMY_ROWS 5
#define ENEMY_COLS 10
#define MAX_ENEMIES (ENEMY_ROWS * ENEMY_COLS)

// Propriedades do Sprite do Inimigo
#define ENEMY_SPRITE_PATH "assets/images/sprites/alien_invader.png" // Caminho de exemplo
#define ENEMY_SCALE 0.4f // Escala do inimigo

typedef struct {
    Texture2D texture;
    Vector2 position;
    bool active;
    int type;
    Rectangle hitBox;
    int points;
} Enemy;

typedef struct {
    Enemy enemies[MAX_ENEMIES];
    Vector2 moveDirection;
    float moveSpeed;
    float timeSinceLastMove;
    float moveInterval;
    float horizontalLimit;
} EnemyManager;

void InitEnemyManager(EnemyManager *manager, int screenWidth, int screenHeight);
void UpdateEnemyManager(EnemyManager *manager, float deltaTime, int screenWidth);
void DrawEnemyManager(EnemyManager *manager);
void UnloadEnemyManager(EnemyManager *manager);

#endif // ENEMY_H