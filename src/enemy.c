#include "enemy.h"
#include <stdio.h>
#include <math.h> // Para fminf e fmaxf

#define ENEMY_SPACING_X 35.0f
#define ENEMY_SPACING_Y 35.0f
#define START_OFFSET_X 50.0f  // Offset inicial da esquerda
// CORREÇÃO: Posição inicial no espaço (parte preta)
#define START_OFFSET_Y 50.0f

#define INITIAL_MOVE_SPEED 10.0f
#define MOVE_INTERVAL_SECONDS 0.5f

void InitEnemyManager(EnemyManager *manager, int screenWidth, int screenHeight) {
    Texture2D enemyTexture = LoadTexture(ENEMY_SPRITE_PATH);

    if (enemyTexture.id == 0) {
        printf("[ERRO] Sprite do Inimigo (%s) nao encontrado! Usando quadrado azul temporario.\n", ENEMY_SPRITE_PATH);
        Image checked = GenImageColor(32, 32, BLUE);
        enemyTexture = LoadTextureFromImage(checked);
        UnloadImage(checked);
    } else {
        SetTextureFilter(enemyTexture, TEXTURE_FILTER_POINT);
    }

    // Configurações do Gerenciador
    manager->moveDirection = (Vector2){ 1.0f, 0.0f }; // Começa movendo para a direita
    manager->moveSpeed = INITIAL_MOVE_SPEED;
    manager->moveInterval = MOVE_INTERVAL_SECONDS;
    manager->timeSinceLastMove = 0.0f;
    manager->horizontalLimit = (float)screenWidth - START_OFFSET_X;

    // Inicializa a Grade de Inimigos
    float scaledWidth = enemyTexture.width * ENEMY_SCALE;
    float scaledHeight = enemyTexture.height * ENEMY_SCALE;

    for (int row = 0; row < ENEMY_ROWS; row++) {
        for (int col = 0; col < ENEMY_COLS; col++) {
            int index = row * ENEMY_COLS + col;
            Enemy *enemy = &manager->enemies[index];

            enemy->texture = enemyTexture;
            enemy->active = true;
            enemy->type = 1;
            enemy->points = 10;

            // Posição na grade (usa o START_OFFSET_Y corrigido)
            enemy->position.x = START_OFFSET_X + (float)col * ENEMY_SPACING_X;
            enemy->position.y = START_OFFSET_Y + (float)row * ENEMY_SPACING_Y;

            // HitBox inicial
            enemy->hitBox = (Rectangle){ enemy->position.x, enemy->position.y, scaledWidth, scaledHeight };
        }
    }
}

void UpdateEnemyManager(EnemyManager *manager, float deltaTime, int screenWidth) {
    manager->timeSinceLastMove += deltaTime;

    // Verifica se é hora de mover
    if (manager->timeSinceLastMove >= manager->moveInterval) {
        manager->timeSinceLastMove = 0.0f; // Reseta o timer

        bool shouldMoveDown = false;

        // 1. Determinar a Borda Extrema da Grade
        float minX = (float)screenWidth;
        float maxX = 0.0f;
        int activeCount = 0;

        float enemyWidth = manager->enemies[0].texture.width * ENEMY_SCALE;

        for (int i = 0; i < MAX_ENEMIES; i++) {
            if (manager->enemies[i].active) {
                activeCount++;
                minX = fminf(minX, manager->enemies[i].position.x);
                maxX = fmaxf(maxX, manager->enemies[i].position.x + enemyWidth);
            }
        }

        if (activeCount == 0) return;

        // 2. Lógica de Inversão de Direção
        // Adiciona um padding de 10 pixels para a borda
        float padding = 10.0f;

        // Se estiver indo para a direita e atingiu a borda direita
        if (manager->moveDirection.x > 0 && maxX >= (float)screenWidth - padding) {
            shouldMoveDown = true;
        }
        // Se estiver indo para a esquerda e atingiu a borda esquerda
        else if (manager->moveDirection.x < 0 && minX <= padding) {
            shouldMoveDown = true;
        }

        // 3. Mover todos os Inimigos Ativos
        float moveX = manager->moveDirection.x * manager->moveSpeed;
        float moveY = shouldMoveDown ? manager->moveSpeed : 0.0f; // Mover para baixo apenas na inversão

        for (int i = 0; i < MAX_ENEMIES; i++) {
            if (manager->enemies[i].active) {
                manager->enemies[i].position.x += moveX;
                manager->enemies[i].position.y += moveY;

                // Atualiza a HitBox junto com a posição
                manager->enemies[i].hitBox.x = manager->enemies[i].position.x;
                manager->enemies[i].hitBox.y = manager->enemies[i].position.y;
            }
        }

        // Se inverteu, também inverte a direção horizontal
        if (shouldMoveDown) {
            manager->moveDirection.x *= -1.0f;
        }
    }
}

void DrawEnemyManager(EnemyManager *manager) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (manager->enemies[i].active) {
            Enemy *enemy = &manager->enemies[i];

            DrawTextureEx(
                enemy->texture,
                enemy->position,
                0.0f,
                ENEMY_SCALE,
                WHITE
            );

            // Opcional: Desenha a hitbox para depuração
            // DrawRectangleLinesEx(enemy->hitBox, 1, RED);
        }
    }
}

void UnloadEnemyManager(EnemyManager *manager) {
    // Apenas um inimigo por textura (todos compartilham a mesma)
    if (manager->enemies[0].texture.id != 0) {
        UnloadTexture(manager->enemies[0].texture);
    }
}