#include "enemy.h"
#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <math.h> // Para cosf e sinf

// --- Cores Neon (Baseadas nos tipos de ataque que você já usa) ---
#define COLOR_NEON_BLUE (CLITERAL(Color){ 0, 191, 255, 255 })     // Tipo 1 - Fraco
#define COLOR_NEON_PURPLE (CLITERAL(Color){ 128, 0, 255, 255 })  // Tipo 2 - Médio
#define COLOR_NEON_RED (CLITERAL(Color){ 255, 69, 0, 255 })      // Tipo 3 - Forte/Líder
#define COLOR_EXPLOSION_ORANGE (CLITERAL(Color){ 255, 165, 0, 255 })


// NOVO: CONSTANTE DE PARTÍCULAS
#define EXPLOSION_PARTICLE_COUNT 100

// NOVOS VALORES PARA O SISTEMA DE ONDAS (WAVES)
#define WAVE_START_DURATION 3.0f // Tempo de exibição da mensagem "Wave X"
#define ENEMY_INITIAL_HEALTH_T1 1
#define ENEMY_INITIAL_HEALTH_T2 2
#define ENEMY_INITIAL_HEALTH_T3 4


// --- IMPLEMENTAÇÃO DO SISTEMA DE PARTÍCULAS ---

// Inicializa o gerenciador de partículas
void InitParticleManager(ParticleManager *manager) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        manager->particles[i].active = false;
    }
    manager->nextParticleIndex = 0;
}

// Retorna uma cor vibrante para as partículas
Color GetRandomNeonColor() {
    int r = GetRandomValue(0, 5);
    switch (r) {
        case 0: return RED;
        case 1: return COLOR_EXPLOSION_ORANGE;
        case 2: return YELLOW;
        case 3: return VIOLET; // Roxo
        case 4: return COLOR_NEON_BLUE;
        case 5: return LIME; // Verde Neon
        default: return WHITE;
    }
}

// Cria um conjunto de partículas no momento da explosão
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

// Atualiza a posição e o tempo de vida das partículas
void UpdateParticles(ParticleManager *manager, float deltaTime) {
    // Estas constantes são usadas apenas para o ricochete das partículas
    const int GAME_WIDTH = 800;
    const int GAME_HEIGHT = 600;

    for (int i = 0; i < MAX_PARTICLES; i++) {
        Particle *p = &manager->particles[i];
        if (!p->active) continue;

        p->life -= deltaTime;

        if (p->life <= 0.0f) {
            p->active = false;
        } else {
            // Movimento: Posição += Velocidade * Tempo
            p->position.x += p->velocity.x * deltaTime;
            p->position.y += p->velocity.y * deltaTime;

            // Ricochete nas bordas da tela
            if (p->position.x < 0 || p->position.x > GAME_WIDTH) {
                p->velocity.x *= -1;
                p->position.x = fmaxf(0, fminf(p->position.x, (float)GAME_WIDTH));
            }
            if (p->position.y < 0 || p->position.y > GAME_HEIGHT) {
                p->velocity.y *= -1;
                p->position.y = fmaxf(0, fminf(p->position.y, (float)GAME_HEIGHT));
            }

            // Simulação de atrito/desaceleração
            p->velocity = Vector2Scale(p->velocity, 1.0f - (0.8f * deltaTime));
        }
    }
}

// Desenha as partículas ativas
void DrawParticles(ParticleManager *manager) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        Particle *p = &manager->particles[i];
        if (!p->active) continue;

        float alpha = p->life / PARTICLE_LIFESPAN;
        Color drawColor = Fade(p->color, alpha);

        // Raio das partículas aumentado para 3.0f
        DrawCircleV(p->position, 3.0f, drawColor);
    }
}


// --- FUNÇÕES AUXILIARES DE DESENHO E LÓGICA ---

void DrawEnemy(Enemy *enemy, Texture2D texture) {
    if (!enemy->active) return;

    // Lógica da Aura (Brilho Retrowave)
    const float BASE_AURA_RADIUS = ENEMY_SIZE * 0.8f;
    float time = (float)GetTime();
    float pulse = (sinf(time * 6.0f) + 1.0f) * 0.5f;
    float currentRadius = BASE_AURA_RADIUS + (pulse * 2.0f);

    // Anéis de brilho
    Color outerColor = Fade(enemy->neonColor, 0.15f + (pulse * 0.05f));
    DrawRing(enemy->position, currentRadius * 0.85f, currentRadius * 1.0f, 0, 360, 30, outerColor);

    Color middleColor = Fade(enemy->neonColor, 0.4f);
    DrawRing(enemy->position, currentRadius * 0.75f, currentRadius * 0.85f, 0, 360, 30, middleColor);

    Color innerColor = Fade(WHITE, 0.6f);
    DrawRing(enemy->position, currentRadius * 0.7f, currentRadius * 0.75f, 0, 360, 30, innerColor);

    // Desenho do Sprite
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


// Função para inicializar a grade de inimigos para uma wave específica
void InitEnemiesForWave(EnemyManager *manager, int screenWidth, int screenHeight, int waveNumber) {
    // 1. Dificuldade e Multiplicadores
    // Aumenta a velocidade base a cada wave (15% por wave)
    float speedMultiplier = 1.0f + (waveNumber - 1) * 0.15f;
    manager->speed = ENEMY_SPEED_INITIAL * speedMultiplier;

    // Aumenta a vida base dos inimigos a cada 3 waves (exemplo)
    int healthBoost = (waveNumber - 1) / 3;

    // 2. Setup visual e geométrico
    float totalWidth = ENEMY_COLS * ENEMY_SIZE + (ENEMY_COLS - 1) * ENEMY_PADDING_X;
    float startX = (screenWidth - totalWidth) / 2.0f;
    float startY = 20.0f;

    manager->direction = 1;
    manager->activeCount = ENEMY_COUNT;
    manager->gameOver = false;

    const float COLLISION_MARGIN = 5.0f;
    const float COLLISION_DIMENSION = ENEMY_SIZE + COLLISION_MARGIN * 2;

    for (int i = 0; i < ENEMY_COUNT; i++) {
        Enemy *enemy = &manager->enemies[i];
        int row = i / ENEMY_COLS;
        int col = i % ENEMY_COLS;

        // Posição central inicial
        enemy->position.x = startX + col * (ENEMY_SIZE + ENEMY_PADDING_X) + ENEMY_SIZE / 2.0f;
        enemy->position.y = startY + row * (ENEMY_SIZE + ENEMY_PADDING_Y) + ENEMY_SIZE / 2.0f;

        // Retângulo de colisão
        enemy->rect.width = COLLISION_DIMENSION;
        enemy->rect.height = COLLISION_DIMENSION;
        enemy->rect.x = enemy->position.x - COLLISION_DIMENSION / 2.0f;
        enemy->rect.y = enemy->position.y - COLLISION_DIMENSION / 2.0f;

        enemy->active = true;
        enemy->hitTimer = 0.0f;
        enemy->isExploding = false;
        enemy->explosionTimer = 0.0f;

        // Atribui cor, tipo e HEALTH baseados na linha (Dificuldade da Wave aplicada)
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


// --- Funções Principais ---

void InitEnemyManager(EnemyManager *manager, int screenWidth, int screenHeight) {
    // --- CARREGAMENTO DE TEXTURAS ---
    // (Presume-se que o código de carregamento de texturas esteja correto)
    manager->enemyTextures[0] = LoadTexture("assets/images/sprites/inimigo_1.png"); // Tipo 1
    manager->enemyTextures[1] = LoadTexture("assets/images/sprites/inimigo_2.png"); // Tipo 2
    manager->enemyTextures[2] = LoadTexture("assets/images/sprites/inimigo_3.png"); // Tipo 3

    // Configuração inicial do sistema de Waves
    manager->currentWave = 1;
    manager->waveStartTimer = WAVE_START_DURATION;

    InitParticleManager(&manager->particleManager);

    // Inicializa os inimigos para a primeira wave
    InitEnemiesForWave(manager, screenWidth, screenHeight, manager->currentWave);
}

void UpdateEnemies(EnemyManager *manager, float deltaTime, int screenWidth) {

    // 1. Lógica do Timer de Início da Wave
    if (manager->waveStartTimer > 0.0f) {
        manager->waveStartTimer -= deltaTime;
        // Atualiza apenas as partículas (se houver explosões remanescentes da wave anterior)
        UpdateParticles(&manager->particleManager, deltaTime);
        return; // Impede o movimento e colisão enquanto a mensagem da wave é exibida
    }

    // 2. Verificação de Fim de Wave e Transição
    if (manager->activeCount == 0) {
        // Todos os inimigos destruídos!
        manager->currentWave++;
        manager->waveStartTimer = WAVE_START_DURATION;
        // Reinicializa a grade de inimigos com dificuldade maior
        InitEnemiesForWave(manager, screenWidth, GetScreenHeight(), manager->currentWave);
        return;
    }

    if (manager->gameOver) return;

    // 3. Atualiza o sistema de partículas (movimento e fade)
    UpdateParticles(&manager->particleManager, deltaTime);

    // --- Movimento Horizontal ---
    float moveAmount = manager->speed * manager->direction * deltaTime;
    bool shouldDrop = false;

    float minX = (float)screenWidth;
    float maxX = 0.0f;

    // Atualiza temporizadores e encontra limites da formação
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
        if (!enemy->active) continue;

        enemy->position.x += moveAmount;
        enemy->rect.x = enemy->position.x - enemy->rect.width / 2.0f;
    }


    // --- Lógica de Descida e Mudança de Direção ---
    if (shouldDrop) {
        manager->direction *= -1;
        manager->speed *= 1.02f;

        for (int i = 0; i < ENEMY_COUNT; i++) {
            Enemy *enemy = &manager->enemies[i];
            if (!enemy->active) continue;

            enemy->position.y += ENEMY_DROP_AMOUNT;
            enemy->rect.y = enemy->position.y - enemy->rect.height / 2.0f;

            // Verificação de Game Over
            if (enemy->rect.y + enemy->rect.height >= ENEMY_GAME_OVER_Y) {
                manager->gameOver = true;
            }
        }
    }
}

void DrawEnemies(EnemyManager *manager) {
    // 1. Desenha as partículas (fundo)
    DrawParticles(&manager->particleManager);

    // 2. Desenha os inimigos (ativos ou em explosão)
    for (int i = 0; i < ENEMY_COUNT; i++) {
        Enemy *enemy = &manager->enemies[i];

        if (enemy->isExploding) {
            DrawExplosion(enemy);

        } else if (enemy->active) {
            Texture2D texture = manager->enemyTextures[enemy->type - 1];
            DrawEnemy(enemy, texture);
        }
    }

    // 3. NOVO: Desenha a tela de transição da Wave
    if (manager->waveStartTimer > 0.0f) {
        char waveText[64];
        sprintf(waveText, "WAVE %d", manager->currentWave);

        // --- CORREÇÃO DE CENTRALIZAÇÃO ---
        // Usamos 800x600, pois é o tamanho mais provável do Render Target do jogo,
        // que é escalado para caber na tela 1920x1080.
        const int GAME_LOGIC_WIDTH = 800;
        const int GAME_LOGIC_HEIGHT = 600;

        int fontSize = 100;
        int textWidth = MeasureText(waveText, fontSize);

        // Centralização usando o viewport lógico (800x600)
        int textX = GAME_LOGIC_WIDTH / 2 - textWidth / 2;
        int textY = GAME_LOGIC_HEIGHT / 2 - fontSize / 2;

        // Calcula a cor e o fade-out
        float fadeOut = manager->waveStartTimer / WAVE_START_DURATION;

        // --- 1. DESENHA FUNDO (GARANTIA DE VISIBILIDADE) ---

        int backgroundPaddingX = 40;
        int backgroundPaddingY = 20;

        // Cria um retângulo de fundo baseado na posição e tamanho do texto
        Rectangle backgroundRect = {
            (float)(textX - backgroundPaddingX),
            (float)(textY - backgroundPaddingY),
            (float)(textWidth + backgroundPaddingX * 2),
            (float)(fontSize + backgroundPaddingY * 2)
        };

        Color backgroundColor = Fade(BLACK, fadeOut * 0.85f); // Fundo preto semi-transparente
        DrawRectangleRec(backgroundRect, backgroundColor);

        // --- 2. DESENHA O TEXTO ---

        Color waveColor = Fade(GOLD, fadeOut);

        // Desenha a sombra (outline) primeiro (deslocamento de 3 pixels)
        Color shadowColor = Fade(BLACK, fadeOut * 0.7f);
        DrawText(
            waveText,
            textX + 3,
            textY + 3,
            fontSize,
            shadowColor
        );

        // Desenha o texto principal
        DrawText(
            waveText,
            textX,
            textY,
            fontSize,
            waveColor
        );
    }
}

// Função para descarregar as texturas da EnemyManager
void UnloadEnemyManager(EnemyManager *manager) {
    for (int i = 0; i < 3; i++) {
        UnloadTexture(manager->enemyTextures[i]);
    }
}

void CheckBulletEnemyCollision(BulletManager *bulletManager, EnemyManager *enemyManager, int *playerGold) {
    // Só verifica a colisão se o timer da wave não estiver ativo
    if (enemyManager->activeCount == 0 || enemyManager->waveStartTimer > 0.0f) return;

    for (int i = 0; i < MAX_PLAYER_BULLETS; i++) {
        Bullet *bullet = &bulletManager->bullets[i];
        if (!bullet->active) continue;

        for (int j = 0; j < ENEMY_COUNT; j++) {
            Enemy *enemy = &enemyManager->enemies[j];
            if (!enemy->active || enemy->isExploding) continue;

            if (CheckCollisionRecs(bullet->rect, enemy->rect)) {
                bullet->active = false;
                enemy->health -= 1;
                enemy->hitTimer = ENEMY_FLASH_DURATION;

                if (enemy->health <= 0) {
                    enemy->isExploding = true;
                    enemy->explosionTimer = ENEMY_EXPLOSION_DURATION;

                    ExplodeEnemy(enemyManager, enemy->position, EXPLOSION_PARTICLE_COUNT);

                    enemy->active = false;
                    enemyManager->activeCount--;

                    *playerGold += enemy->type * 5;
                }
                break;
            }
        }
    }
}