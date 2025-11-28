#ifndef ENEMY_H
#define ENEMY_H

#include "raylib.h"
#include "bullet.h" // Para verificar a colisão com balas do jogador

// --- CONSTANTES DA FORMAÇÃO ---
#define ENEMY_ROWS 4             // AJUSTADO: Reduzido de 5 para 4 (remove a linha de baixo)
#define ENEMY_COLS 8             // Ajustado para 8 (remoção da primeira e última coluna)
#define ENEMY_COUNT (ENEMY_ROWS * ENEMY_COLS) // Agora 32 inimigos
#define ENEMY_SIZE 50.0f         // AJUSTADO: Reduzido de 55.0f para 50.0f
#define ENEMY_PADDING_X 50.0f    // Espaçamento horizontal
#define ENEMY_PADDING_Y 40.0f    // Espaçamento vertical

#define ENEMY_SPEED_INITIAL 30.0f // Velocidade horizontal inicial (pixels/segundo) - Ajustado para ser mais lento
#define ENEMY_DROP_AMOUNT 15.0f   // Distância que o inimigo desce - Ajustado para ser mais suave
#define ENEMY_GAME_OVER_Y 550.0f  // AJUSTADO: Aumentado para 550.0f para dar mais espaço (limite mais baixo)

// NOVO: Constantes de efeito visual
#define ENEMY_FLASH_DURATION 0.1f // Duração do flash vermelho ao ser atingido
#define ENEMY_EXPLOSION_DURATION 0.3f // Duração da animação de explosão

// NOVO: CONSTANTES DE PARTÍCULAS
#define MAX_PARTICLES 1000          // AJUSTADO: Aumentado para 1000 para suportar mais explosões
#define PARTICLE_LIFESPAN 0.8f      // Vida máxima das partículas (em segundos)

// --- ESTRUTURAS ---

// NOVO: Estrutura para uma única partícula
typedef struct {
    Vector2 position;
    Vector2 velocity;
    Color color;
    float life; // Tempo restante de vida
    bool active;
} Particle;

// NOVO: Estrutura para gerenciar todas as partículas
typedef struct {
    Particle particles[MAX_PARTICLES];
    int nextParticleIndex; // Próximo slot disponível no array
} ParticleManager;


typedef struct {
    Vector2 position; // Centro do inimigo
    Rectangle rect;   // Retângulo de colisão
    bool active;
    int health;
    int type; // Determina qual sprite e pontuação (1, 2 ou 3)
    Color neonColor;

    // Campos para efeitos visuais
    float hitTimer;       // Temporizador para o flash vermelho
    bool isExploding;     // Se o inimigo está explodindo
    float explosionTimer; // Temporizador para a animação da explosão

} Enemy;

typedef struct {
    Enemy enemies[ENEMY_COUNT];
    float speed;
    int direction;      // 1 (direita), -1 (esquerda)
    int activeCount;    // Quantos inimigos ainda estão ativos
    bool gameOver;      // Flag para Game Over
    // Array para armazenar as texturas dos 3 tipos de inimigos (index 0=tipo1, 1=tipo2, 2=tipo3)
    Texture2D enemyTextures[3];

    // NOVO: Gerenciador de Partículas
    ParticleManager particleManager;

    // NOVO: Campos para o sistema de Waves (Ondas)
    int currentWave;        // Número da onda atual (e multiplicador de dificuldade)
    float waveStartTimer;   // Temporizador para a tela de transição "WAVE X"

} EnemyManager;

// --- PROTÓTIPOS ---

void InitEnemyManager(EnemyManager *manager, int screenWidth, int screenHeight);
void UpdateEnemies(EnemyManager *manager, float deltaTime, int screenWidth);
void DrawEnemies(EnemyManager *manager);
// Função para tratar a colisão de balas do jogador com inimigos
void CheckBulletEnemyCollision(BulletManager *bulletManager, EnemyManager *enemyManager, int *playerGold);
// Função para descarregar as texturas dos inimigos
void UnloadEnemyManager(EnemyManager *manager);

// NOVO: Protótipos para o sistema de partículas
void InitParticleManager(ParticleManager *manager);
void ExplodeEnemy(EnemyManager *manager, Vector2 position, int particleCount);
void UpdateParticles(ParticleManager *manager, float deltaTime);
void DrawParticles(ParticleManager *manager);

#endif // ENEMY_H