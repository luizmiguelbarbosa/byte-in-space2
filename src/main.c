#include "raylib.h"
#include "player.h"
#include "hud.h"
#include "bullet.h"
#include "audio.h"
#include "star.h"
#include "game_state.h"
#include "shop.h"
#include "cutscene.h"
#include "enemy.h"
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#define GAME_WIDTH 800
#define GAME_HEIGHT 600

const int STAR_COUNT = 150;

StarField sideStarField = { 0 };

// Shader global
Shader crtShader;

// Uniform locations
int locResolution;
int locTime;

// --- FUNÇÃO PARA DESENHAR A INTERFACE DE TRANSIÇÃO SHOP/WAVE ---
void DrawShopTransitionUI(EnemyManager *manager) {
    if (!manager->triggerShopReturn) return;

    // Fundo escurecido (mais escuro para focar no painel)
    DrawRectangle(0, 0, GAME_WIDTH, GAME_HEIGHT, Fade(BLACK, 0.9f));

    // Painel Central (Ajustado o tamanho para caber o texto)
    Rectangle panel = {
        GAME_WIDTH / 2.0f - 220,
        GAME_HEIGHT / 2.0f - 110,
        440,
        220
    };

    // Cores Neon
    Color NEON_MAGENTA = (Color){255, 0, 150, 255}; // Rosa forte
    Color NEON_CYAN = (Color){0, 255, 255, 255};    // Ciano
    Color NEON_GREEN = (Color){0, 255, 0, 255};     // Verde

    // Fundo do Painel (Preto/Cinza escuro, quase invisível)
    DrawRectangleRounded(panel, 0.15f, 8, Fade(BLACK, 0.7f));

    // Contorno (Efeito Neon Duplo)
    DrawRectangleRoundedLines(panel, 0.15f, 8, 5, Fade(NEON_MAGENTA, 0.8f)); // Contorno 1 (Glow)
    DrawRectangleRoundedLines(panel, 0.15f, 8, 3, NEON_CYAN);               // Contorno 2 (Principal)


    // Texto de Título com Efeito Glow
    const char *waveCompleteText = TextFormat("WAVE %d CONCLUÍDA!", manager->currentWave);
    int titleFontSize = 32;
    int titleX = (int)panel.x + (int)(panel.width - MeasureText(waveCompleteText, titleFontSize)) / 2;
    int titleY = (int)panel.y + 30;

    // Sombra/Glow do Título
    DrawText(waveCompleteText, titleX + 2, titleY + 2, titleFontSize, Fade(NEON_MAGENTA, 0.5f));
    DrawText(waveCompleteText, titleX, titleY, titleFontSize, NEON_CYAN);

    // Pergunta (Centralizada)
    const char *shopPromptText = "PRONTO PARA A PRÓXIMA MISSÃO?";
    int promptFontSize = 20;
    int promptX = (int)panel.x + (int)(panel.width - MeasureText(shopPromptText, promptFontSize)) / 2;

    DrawText(shopPromptText, promptX, (int)panel.y + 85, promptFontSize, RAYWHITE);

    // Opção Loja (Tecla E)
    const char *continueText = "Pressione [E] para visitar a LOJA de MELHORIAS";
    int optionFontSize = 18;
    int optionX1 = (int)panel.x + (int)(panel.width - MeasureText(continueText, optionFontSize)) / 2;

    DrawText(continueText, optionX1, (int)panel.y + 140, optionFontSize, NEON_GREEN);

    // Opção Continuar (Tecla F)
    const char *skipText = TextFormat("Pressione [F] para CONTINUAR para a WAVE %d", manager->currentWave + 1);
    int optionX2 = (int)panel.x + (int)(panel.width - MeasureText(skipText, optionFontSize)) / 2;

    DrawText(skipText, optionX2, (int)panel.y + 170, optionFontSize, NEON_MAGENTA);
}

// --- FUNÇÃO PARA DESENHAR O INÍCIO DA WAVE ---
void DrawWaveStartUI(EnemyManager *manager) {
    // Pega o tempo restante
    float t = manager->waveStartTimer;
    if (t <= 0) return;

    // Escurece o fundo levemente
    DrawRectangle(0, 0, GAME_WIDTH, GAME_HEIGHT, Fade(BLACK, 0.4f));

    // Pulso e Fade-out:
    float pulseRate = 6.0f; // Frequência do pulso
    float alpha = 0.5f + (sinf((float)GetTime() * pulseRate) + 1.0f) * 0.25f; // Alpha base + pulso

    // Fade-out no final do contador (garantindo que não seja < 0)
    if (t < 1.0f) {
        alpha *= (t / 1.0f);
    }
    alpha = fmaxf(0.0f, fminf(1.0f, alpha)); // Garante que alpha esteja entre 0 e 1

    const char *text = TextFormat("WAVE %d INICIADA!", manager->currentWave);
    int fontSize = 50;
    int textWidth = MeasureText(text, fontSize);

    Color color = Fade(RAYWHITE, alpha);

    DrawText(text, (GAME_WIDTH - textWidth) / 2, GAME_HEIGHT / 2 - fontSize, fontSize, color);

    // Desenha o contador regressivo
    const char *timerText = TextFormat("%.1f", t);
    int timerFontSize = 30;
    int timerTextWidth = MeasureText(timerText, timerFontSize);

    // O contador sempre deve ser amarelo e visível
    DrawText(timerText, (GAME_WIDTH - timerTextWidth) / 2, GAME_HEIGHT / 2 + 20, timerFontSize, Fade(YELLOW, alpha));
}

int main(void) {
    // FULLSCREEN antes da janela
    SetConfigFlags(FLAG_FULLSCREEN_MODE);

    int monitorWidth  = GetMonitorWidth(GetCurrentMonitor());
    int monitorHeight = GetMonitorHeight(GetCurrentMonitor());

    InitWindow(monitorWidth, monitorHeight, "Byte in Space 2");
    SetTargetFPS(60);

    RenderTexture2D target = LoadRenderTexture(GAME_WIDTH, GAME_HEIGHT);
    InitAudioDevice();

    // --- SHADER CRT ---
    crtShader = LoadShader(0, "assets/shaders/crt22.fs");

    locResolution = GetShaderLocation(crtShader, "resolution");
    locTime       = GetShaderLocation(crtShader, "time");

    float initialRes[2] = { (float)monitorWidth, (float)monitorHeight };
    SetShaderValue(crtShader, locResolution, initialRes, SHADER_UNIFORM_VEC2);
    // ----------------------------------------------------

    // --- INICIALIZAÇÃO DO JOGO ---
    StarField starField = { 0 };
    InitStarField(&starField, STAR_COUNT, GAME_WIDTH, GAME_HEIGHT);
    InitStarField(&sideStarField, STAR_COUNT / 2, monitorWidth, monitorHeight);

    Player player;
    InitPlayer(&player);
    player.gold = 0;

    Hud hud;
    InitHud(&hud);

    BulletManager bulletManager;
    InitBulletManager(&bulletManager);

    // NOVO: Inicialização do Gerenciador de Inimigos
    EnemyManager enemyManager;
    InitEnemyManager(&enemyManager, GAME_WIDTH, GAME_HEIGHT);

    AudioManager audioManager;
    InitAudioManager(&audioManager);

    ShopScene shop;
    InitShop(&shop, &player, GAME_WIDTH, GAME_HEIGHT);

    CutsceneScene cutscene;
    InitCutscene(&cutscene);

    GameState currentState = STATE_CUTSCENE;

    PlayMusicTrack(&audioManager, MUSIC_CUTSCENE);

    float player_width_scaled  = player.texture.width * player.scale;
    float player_height_scaled = player.texture.height * player.scale;

    player.position.x = GAME_WIDTH/2 - player_width_scaled/2;
    player.position.y = GAME_HEIGHT - player_height_scaled - 100.0f;


    while (!WindowShouldClose()) {

        if (IsKeyPressed(KEY_ESCAPE)) break;

        float dt = GetFrameTime();

        // ESCALA DO JOGO NA TELA REAL
        int screenW = GetScreenWidth();
        int screenH = GetScreenHeight();
        float scaleX = (float)screenW / GAME_WIDTH;
        float scaleY = (float)screenH / GAME_HEIGHT;
        float scale = (scaleX < scaleY) ? scaleX : scaleY;

        int offsetX = (screenW - (int)(GAME_WIDTH * scale)) / 2;
        int offsetY = (screenH - (int)(GAME_HEIGHT * scale)) / 2;

        // --- UPDATE DO SHADER ---
        float t = GetTime();
        SetShaderValue(crtShader, locTime, &t, SHADER_UNIFORM_FLOAT);

        float res[2] = { (float)screenW, (float)screenH };
        SetShaderValue(crtShader, locResolution, res, SHADER_UNIFORM_VEC2);
        // ---------------------------

        // --- UPDATE GLOBAL ---
        UpdateAudioManager(&audioManager);
        UpdateStarField(&sideStarField, dt);

        // MÁQUINA DE ESTADOS
        switch (currentState) {
            case STATE_CUTSCENE:
                UpdateCutscene(&cutscene, &currentState, dt);

                if (currentState == STATE_SHOP) {
                    StopMusicStream(audioManager.musicCutscene);
                    PlayMusicTrack(&audioManager, MUSIC_SHOP);
                }
                break;

            case STATE_SHOP:
                UpdateShop(&shop, &player, &starField, &currentState, dt);
                UpdatePlayerBullets(&bulletManager, dt);

                if (currentState == STATE_GAMEPLAY) {
                    // Ao retornar da loja, não chamamos InitEnemyManager para manter o número da wave atual.
                    // A wave será iniciada pelo loop de gameplay usando o estado já existente.
                    StopMusicStream(audioManager.musicShop);
                    PlayMusicTrack(&audioManager, MUSIC_GAMEPLAY);
                }
                break;

            case STATE_GAMEPLAY:
                UpdateStarField(&starField, dt);
                UpdateHud(&hud, dt);

                // Variável de controle: Pausa de Ação (Bloqueia player, disparo, colisão)
                // É TRUE se estiver na tela de transição ou no countdown de início de wave.
                bool isActionPaused = enemyManager.triggerShopReturn || enemyManager.waveStartTimer > 0 || enemyManager.gameOver;

                // 1. Atualização e Movimento do Player (só se não estiver pausado)
                if (!isActionPaused) {
                    UpdatePlayer(&player, &bulletManager, &audioManager, &hud, dt, GAME_WIDTH, GAME_HEIGHT);
                }

                // 2. Movimento das Balas (Sempre move, mesmo pausado, para que as balas existentes saiam da tela)
                UpdatePlayerBullets(&bulletManager, dt);

                // 3. Atualização de Inimigos e Timers (Sempre atualiza para gerenciar o spawn e o countdown)
                // CHAMADA CORRIGIDA: Passa os ponteiros para hud.lives e enemyManager.gameOver
                UpdateEnemies(&enemyManager, dt, GAME_WIDTH, &player.currentLives, &enemyManager.gameOver);

                // 4. Lógica de Colisão (só ocorre quando o jogo está ativo, para evitar tiros acidentais)
                if (!isActionPaused) {
                    CheckBulletEnemyCollision(&bulletManager, &enemyManager, &player.gold, &audioManager);
                }


                // LÓGICA DE TRANSIÇÃO SHOP/WAVE (Teclas E e F)
                if (enemyManager.triggerShopReturn) {
                    // [E] para ir para a loja
                    if (IsKeyPressed(KEY_E)) {
                        currentState = STATE_SHOP;
                        enemyManager.triggerShopReturn = false;
                        StopMusicStream(audioManager.musicGameplay);
                        PlayMusicTrack(&audioManager, MUSIC_SHOP);
                    }
                    // [F] para continuar a próxima wave
                    if (IsKeyPressed(KEY_F)) {
                        enemyManager.triggerShopReturn = false;
                        // UpdateEnemies no próximo frame irá iniciar a próxima wave (N+1)
                    }
                }
                // --- FIM LÓGICA DE TRANSIÇÃO SHOP/WAVE ---
                break;
        }

        // --- RENDER PARA O TARGET (800x600) ---
        BeginTextureMode(target);
            ClearBackground(BLACK);

            switch (currentState) {
                case STATE_CUTSCENE:
                    DrawCutscene(&cutscene, GAME_WIDTH, GAME_HEIGHT);
                    break;

                case STATE_SHOP:
                    DrawShop(&shop, &player, &starField);
                    DrawPlayerBullets(&bulletManager);
                    break;

                case STATE_GAMEPLAY:
                    DrawStarField(&starField);

                    // NOVO: Desenha a linha verde neon (Limite de Game Over)
                    const Color NEON_GREEN_LINE = (Color){ 0, 255, 0, 255 };
                    DrawRectangle(0, (int)ENEMY_GAME_OVER_LINE_Y, GAME_WIDTH, 2, NEON_GREEN_LINE);
                    // Efeito de Glow (semi-transparente acima e abaixo)
                    DrawRectangle(0, (int)ENEMY_GAME_OVER_LINE_Y - 2, GAME_WIDTH, 2, Fade(NEON_GREEN_LINE, 0.4f));
                    DrawRectangle(0, (int)ENEMY_GAME_OVER_LINE_Y + 2, GAME_WIDTH, 2, Fade(NEON_GREEN_LINE, 0.4f));


                    // NOVO: Desenha os inimigos
                    DrawEnemies(&enemyManager);
                    DrawPlayer(&player);
                    // IMPORTANTE: As balas devem ser desenhadas mesmo se o jogo estiver pausado
                    DrawPlayerBullets(&bulletManager);

                    // Desenha a mensagem de início da wave (sobrepõe o jogo)
                    DrawWaveStartUI(&enemyManager);

                    // Desenha a interface de transição (sobrepõe o jogo e o wave start)
                    DrawShopTransitionUI(&enemyManager);

                    // NOVO: Tela de Game Over
                    if (enemyManager.gameOver) {
                        DrawRectangle(0, 0, GAME_WIDTH, GAME_HEIGHT, Fade(BLACK, 0.8f));
                        DrawText("GAME OVER", GAME_WIDTH/2 - MeasureText("GAME OVER", 40)/2, GAME_HEIGHT/2 - 20, 40, RED);
                        DrawText("OS INIMIGOS TE ALCANÇARAM!", GAME_WIDTH/2 - MeasureText("OS INIMIGOS TE ALCANÇARAM!", 20)/2, GAME_HEIGHT/2 + 30, 20, WHITE);
                    }
                    break;
            }
        EndTextureMode();

        // --- RENDER FINAL ---
        BeginDrawing();
            ClearBackground(BLACK);

            if (offsetX > 0 || offsetY > 0) {
                DrawStarField(&sideStarField);
            }

            BeginShaderMode(crtShader);

            DrawTexturePro(
                target.texture,
                (Rectangle){0, 0, (float)target.texture.width, -(float)target.texture.height},
                (Rectangle){(float)offsetX, (float)offsetY, GAME_WIDTH * scale, GAME_HEIGHT * scale},
                (Vector2){0, 0},
                0.0f,
                WHITE
            );

            EndShaderMode();

            if (currentState == STATE_GAMEPLAY && offsetX > 0) {
                // --- CHAMADA LADO ESQUERDO (VIDA, ENERGIA, POWER-UPS) ---
                DrawHudSide(&hud,
                            true,
                            offsetY,
                            player.energyCharge,
                            player.hasDoubleShot,
                            player.hasShield,
                            player.extraLives,
                            player.currentLives,
                            player.gold);

                // --- CHAMADA LADO DIREITO (GOLD) ---
                DrawHudSide(&hud,
                            false,
                            offsetY,
                            0.0f, // Ignorado
                            false, // Ignorado
                            false, // Ignorado
                            0, // Ignorado
                            player.currentLives, // Necessário para a assinatura
                            player.gold);
            }

            // O DrawFPS(10, 50) foi removido.

        EndDrawing();
    }

    // --- FINALIZAÇÃO ---
    UnloadShop(&shop);
    UnloadPlayer(&player);
    UnloadRenderTexture(target);
    UnloadBulletManager(&bulletManager);
    UnloadAudioManager(&audioManager);

    UnloadShader(crtShader);

    CloseAudioDevice();
    CloseWindow();
    return 0;
}