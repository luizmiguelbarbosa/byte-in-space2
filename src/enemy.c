#include "enemy.h"
#include "raylib.h"
#include "raymath.h"
#include <stdio.h>

// --- Cores Neon (Baseadas nos tipos de ataque que você já usa) ---
#define COLOR_NEON_BLUE (CLITERAL(Color){ 0, 191, 255, 255 })     // Tipo 1 - Fraco
#define COLOR_NEON_PURPLE (CLITERAL(Color){ 128, 0, 255, 255 })  // Tipo 2 - Médio
#define COLOR_NEON_RED (CLITERAL(Color){ 255, 69, 0, 255 })      // Tipo 3 - Forte/Líder
#define COLOR_EXPLOSION_ORANGE (CLITERAL(Color){ 255, 165, 0, 255 })

// --- Funções Auxiliares ---

// Desenha um único inimigo usando o sprite correspondente ao seu tipo.
void DrawEnemy(Enemy *enemy, Texture2D texture) {
    if (!enemy->active) return;

    // O desenho usa o tamanho ENEMY_SIZE definido em enemy.h
    float w = ENEMY_SIZE;
    float h = ENEMY_SIZE;

    // Calcula o retângulo de destino para o desenho
    // O centro do desenho deve ser o mesmo do centro da colisão (enemy->position).
    Rectangle destRec = { enemy->position.x - w / 2, enemy->position.y - h / 2, w, h };

    // Calcula a origem da textura (canto superior esquerdo)
    Vector2 origin = { 0.0f, 0.0f };

    // NOVO: Determina a cor de tintura. Se hitTimer > 0, usa VERMELHO, senão usa BRANCO.
    Color tintColor = WHITE;
    if (enemy->hitTimer > 0.0f) {
        tintColor = RED;
    }

    // Desenha o Sprite
    DrawTexturePro(
        texture,
        (Rectangle){ 0.0f, 0.0f, (float)texture.width, (float)texture.height }, // Retângulo de origem (textura inteira)
        destRec, // Retângulo de destino (ENEMY_SIZE)
        origin, // Origem (canto superior esquerdo)
        0.0f, // Rotação
        tintColor // CORRIGIDO: Usa a cor de tintura (RED para flash)
    );
}

// Desenha o efeito de explosão
void DrawExplosion(Enemy *enemy) {
    // Calcula a progressão da explosão (0.0 a 1.0)
    // Se o timer for 0.3s e o tempo restante for 0.15s, a progressão é 0.5 (meio do caminho)
    float progress = 1.0f - (enemy->explosionTimer / ENEMY_EXPLOSION_DURATION);
    float maxRadius = ENEMY_SIZE * 1.5f;
    float currentRadius = maxRadius * progress;

    // Fundo da explosão (Laranja/Amarelo, que desvanece)
    Color outerColor = Fade(COLOR_EXPLOSION_ORANGE, 1.0f - progress);
    DrawCircle(
        (int)enemy->position.x,
        (int)enemy->position.y,
        currentRadius,
        outerColor
    );

    // Centro da explosão (Branco/Amarelo, mais intenso)
    Color centerColor = Fade(YELLOW, 1.0f - progress * 0.5f);
    DrawCircle(
        (int)enemy->position.x,
        (int)enemy->position.y,
        currentRadius * 0.6f,
        centerColor
    );
}


// --- Funções Principais ---

void InitEnemyManager(EnemyManager *manager, int screenWidth, int screenHeight) {
    // --- CARREGAMENTO DE TEXTURAS ---
    manager->enemyTextures[0] = LoadTexture("assets/images/sprites/inimigo_1.png"); // Tipo 1
    manager->enemyTextures[1] = LoadTexture("assets/images/sprites/inimigo_2.png"); // Tipo 2
    manager->enemyTextures[2] = LoadTexture("assets/images/sprites/inimigo_3.png"); // Tipo 3 (corrigido o caminho)

    // Cálculo do ponto de início (para centralizar a grade de 8 colunas na tela 800x600)
    float totalWidth = ENEMY_COLS * ENEMY_SIZE + (ENEMY_COLS - 1) * ENEMY_PADDING_X;
    float startX = (screenWidth - totalWidth) / 2.0f;
    float startY = 20.0f; // Começa mais acima

    manager->speed = ENEMY_SPEED_INITIAL;
    manager->direction = 1; // Começa movendo para a direita
    manager->activeCount = ENEMY_COUNT;
    manager->gameOver = false; // Inicializa o flag de Game Over como falso

    // Adiciona uma margem para a colisão, tornando o hitbox maior que o sprite
    const float COLLISION_MARGIN = 5.0f; // 5 pixels extra em cada lado
    const float COLLISION_DIMENSION = ENEMY_SIZE + COLLISION_MARGIN * 2; // Tamanho total (ex: 48 + 10 = 58)

    for (int i = 0; i < ENEMY_COUNT; i++) {
        Enemy *enemy = &manager->enemies[i];
        int row = i / ENEMY_COLS;
        int col = i % ENEMY_COLS;

        // Posição central inicial
        enemy->position.x = startX + col * (ENEMY_SIZE + ENEMY_PADDING_X) + ENEMY_SIZE / 2.0f;
        enemy->position.y = startY + row * (ENEMY_SIZE + ENEMY_PADDING_Y) + ENEMY_SIZE / 2.0f;

        // Retângulo de colisão (AUMENTADO para facilitar o acerto)
        enemy->rect.width = COLLISION_DIMENSION;
        enemy->rect.height = COLLISION_DIMENSION;
        // Centraliza o retângulo de colisão na enemy->position
        enemy->rect.x = enemy->position.x - COLLISION_DIMENSION / 2.0f;
        enemy->rect.y = enemy->position.y - COLLISION_DIMENSION / 2.0f;

        enemy->active = true;

        // NOVO: Inicializa os campos de efeito visual
        enemy->hitTimer = 0.0f;
        enemy->isExploding = false;
        enemy->explosionTimer = 0.0f;

        // Atribui cor, tipo e HEALTH baseados na linha (Top-down)
        if (row == 0) {
            enemy->neonColor = COLOR_NEON_RED;
            enemy->type = 3; // Mais forte e duro
            enemy->health = 4; // Tipo 3: 4 hits
        } else if (row <= 2) {
            enemy->neonColor = COLOR_NEON_PURPLE;
            enemy->type = 2; // Um pouco mais forte
            enemy->health = 2; // Tipo 2: 2 hits
        } else {
            enemy->neonColor = COLOR_NEON_BLUE;
            enemy->type = 1; // Mais fraco
            enemy->health = 1; // Tipo 1: 1 hit
        }
    }
}

void UpdateEnemies(EnemyManager *manager, float deltaTime, int screenWidth) {
    if (manager->activeCount == 0 || manager->gameOver) return;

    // --- Movimento Horizontal ---
    float moveAmount = manager->speed * manager->direction * deltaTime;
    bool shouldDrop = false;

    // Variáveis para rastrear os limites extremos dos inimigos ativos
    float minX = (float)screenWidth;
    float maxX = 0.0f;

    // Calcula os limites extremos da formação de inimigos ativos E atualiza os temporizadores
    for (int i = 0; i < ENEMY_COUNT; i++) {
        Enemy *enemy = &manager->enemies[i];

        // NOVO: Atualiza hitTimer (para o flash vermelho)
        if (enemy->hitTimer > 0.0f) {
            enemy->hitTimer -= deltaTime;
        }

        // NOVO: Atualiza explosionTimer (para a explosão)
        if (enemy->isExploding) {
            enemy->explosionTimer -= deltaTime;

            if (enemy->explosionTimer <= 0.0f) {
                enemy->isExploding = false; // Fim da explosão, remove o inimigo do estado 'exploding'
                // Nota: O inimigo ativo já foi desativado em CheckBulletEnemyCollision
            }
        }

        // Ignora inimigos inativos e aqueles no meio da explosão para cálculo de limites
        if (!enemy->active && !enemy->isExploding) continue;

        // Se estiver explodindo, o inimigo não contribui para o cálculo de limites de movimento
        if (enemy->active) {
            // Usa o retângulo de colisão para calcular o limite
            minX = fminf(minX, enemy->rect.x);
            maxX = fmaxf(maxX, enemy->rect.x + enemy->rect.width);
        }
    }

    // Verifica se a formação precisa descer e mudar de direção
    const float margin = 2.0f;

    if (manager->direction == 1) { // Movendo para a direita
        if (maxX >= screenWidth - margin) {
            shouldDrop = true;
        }
    } else { // Movendo para a esquerda
        if (minX <= margin) {
            shouldDrop = true;
        }
    }

    // Atualiza posições
    for (int i = 0; i < ENEMY_COUNT; i++) {
        Enemy *enemy = &manager->enemies[i];
        // O movimento deve ocorrer apenas para inimigos ATIVOS
        if (!enemy->active) continue;

        // Atualiza posição X
        enemy->position.x += moveAmount;
        // Centraliza usando enemy->rect.width para o cálculo correto
        enemy->rect.x = enemy->position.x - enemy->rect.width / 2.0f;
    }


    // --- Lógica de Descida e Mudança de Direção ---
    if (shouldDrop) {
        // Inverte a direção
        manager->direction *= -1;

        // Aumenta a velocidade (A dificuldade aumenta a cada descida)
        manager->speed *= 1.02f;

        // Move todos os inimigos ativos para baixo
        for (int i = 0; i < ENEMY_COUNT; i++) {
            Enemy *enemy = &manager->enemies[i];
            if (!enemy->active) continue;

            enemy->position.y += ENEMY_DROP_AMOUNT;
            // Centraliza usando enemy->rect.height para o cálculo correto
            enemy->rect.y = enemy->position.y - enemy->rect.height / 2.0f;

            // Verificação de Game Over
            if (enemy->rect.y + enemy->rect.height >= ENEMY_GAME_OVER_Y) {
                manager->gameOver = true;
            }
        }
    }
}

void DrawEnemies(EnemyManager *manager) {
    for (int i = 0; i < ENEMY_COUNT; i++) {
        Enemy *enemy = &manager->enemies[i];

        if (enemy->isExploding) {
            // Desenha a explosão (e não o sprite)
            DrawExplosion(enemy);

        } else if (enemy->active) {
            // Seleciona a textura correta
            Texture2D texture = manager->enemyTextures[enemy->type - 1];

            // Desenha o sprite com possível flash vermelho
            DrawEnemy(enemy, texture);
        }

        // Inimigos inativos que não estão explodindo não são desenhados.
    }
}

// Função para descarregar as texturas da EnemyManager
void UnloadEnemyManager(EnemyManager *manager) {
    for (int i = 0; i < 3; i++) {
        UnloadTexture(manager->enemyTextures[i]);
    }
}

void CheckBulletEnemyCollision(BulletManager *bulletManager, EnemyManager *enemyManager, int *playerGold) {
    if (enemyManager->activeCount == 0) return;

    for (int i = 0; i < MAX_PLAYER_BULLETS; i++) {
        Bullet *bullet = &bulletManager->bullets[i];
        if (!bullet->active) continue;

        for (int j = 0; j < ENEMY_COUNT; j++) {
            Enemy *enemy = &enemyManager->enemies[j];
            // Verifica a colisão apenas com inimigos ativos (e que não estejam explodindo)
            if (!enemy->active || enemy->isExploding) continue;

            // Verifica colisão entre a bala e o inimigo (usando a caixa de colisão expandida)
            if (CheckCollisionRecs(bullet->rect, enemy->rect)) {
                // Desativa a bala
                bullet->active = false;

                // Reduz a vida do inimigo
                enemy->health -= 1;

                // NOVO: Ativa o flash vermelho ao ser atingido
                enemy->hitTimer = ENEMY_FLASH_DURATION;

                if (enemy->health <= 0) {
                    // NOVO: Inicia a explosão
                    enemy->isExploding = true;
                    enemy->explosionTimer = ENEMY_EXPLOSION_DURATION;

                    // Desativa a colisão e o movimento do inimigo (mas mantém a posição para o efeito visual)
                    enemy->active = false;
                    enemyManager->activeCount--;

                    // Adiciona gold ao jogador
                    *playerGold += enemy->type * 5;
                }
                // Sai do loop de inimigos após a colisão, pois a bala foi desativada
                break;
            }
        }
    }
}