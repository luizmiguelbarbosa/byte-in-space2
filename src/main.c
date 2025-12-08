#include "raylib.h"
// Inclui os cabeçalhos de todos os módulos do jogo
#include "player.h"
#include "hud.h"
#include "bullet.h"
#include "audio.h"
#include "star.h"
#include "game_state.h" // Enum com os estados do jogo (GAMEPLAY, SHOP, etc.)
#include "shop.h"
#include "cutscene.h"
#include "enemy.h"
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

// Definições da resolução interna do jogo (onde o jogo é renderizado antes de escalar)
#define GAME_WIDTH 800
#define GAME_HEIGHT 600

// Constante para a quantidade de estrelas
const int STAR_COUNT = 150;

// Campo de estrelas lateral (usado para preencher as laterais da tela cheia)
StarField sideStarField = { 0 };

// Variáveis globais para o Shader CRT
Shader crtShader;

// Localizações dos uniformes (variáveis que passamos para o shader)
int locResolution;
int locTime;

// Declarando a audioManager como global (para que InitEnding possa acessá-la)
AudioManager audioManager;


// --- FUNÇÃO PARA DESENHAR A INTERFACE DE TRANSIÇÃO SHOP/WAVE ---
// Aparece ao final de uma wave, permitindo que o jogador escolha ir para a Loja ou continuar.
void DrawShopTransitionUI(EnemyManager *manager) {
    // Se a flag não estiver ativada, não desenha nada
    if (!manager->triggerShopReturn) return;

    // Fundo escurecido (mais escuro para focar no painel)
    DrawRectangle(0, 0, GAME_WIDTH, GAME_HEIGHT, Fade(BLACK, 0.9f));

    // Painel Central
    Rectangle panel = {
        GAME_WIDTH / 2.0f - 220,
        GAME_HEIGHT / 2.0f - 110,
        440,
        220
    };

    // Definição de Cores Neon
    Color NEON_MAGENTA = (Color){255, 0, 150, 255}; // Rosa forte
    Color NEON_CYAN = (Color){0, 255, 255, 255};    // Ciano
    Color NEON_GREEN = (Color){0, 255, 0, 255};     // Verde

    // Fundo do Painel (Arredondado)
    DrawRectangleRounded(panel, 0.15f, 8, Fade(BLACK, 0.7f));

    // Contorno (Efeito Neon Duplo para Glow)
    // Contorno 1 (Mais grosso, para o "Glow")
    DrawRectangleRoundedLines(panel, 0.15f, 8, 5, Fade(NEON_MAGENTA, 0.8f));
    // Contorno 2 (Principal, mais fino)
    DrawRectangleRoundedLines(panel, 0.15f, 8, 3, NEON_CYAN);


    // Texto de Título
    const char *waveCompleteText = TextFormat("WAVE %d CONCLUÍDA!", manager->currentWave);
    int titleFontSize = 32;
    int titleX = (int)panel.x + (int)(panel.width - MeasureText(waveCompleteText, titleFontSize)) / 2;
    int titleY = (int)panel.y + 30;

    // Sombra/Glow do Título (para o efeito neon)
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
// Desenha a contagem regressiva e a mensagem "WAVE X INICIADA!"
void DrawWaveStartUI(EnemyManager *manager) {
    // Pega o tempo restante
    float t = manager->waveStartTimer;
    if (t <= 0) return; // Se o timer zerou, não desenha

    // Escurece o fundo levemente para focar na mensagem
    DrawRectangle(0, 0, GAME_WIDTH, GAME_HEIGHT, Fade(BLACK, 0.4f));

    // Lógica para Pulso e Fade-out:
    float pulseRate = 6.0f; // Frequência do pulso
    // Calcula o Alpha (transparência) base + um pulso usando seno
    float alpha = 0.5f + (sinf((float)GetTime() * pulseRate) + 1.0f) * 0.25f;

    // Aplica um Fade-out rápido no final do contador (último segundo)
    if (t < 1.0f) {
        alpha *= (t / 1.0f);
    }
    // Garante que alpha esteja entre 0 e 1
    alpha = fmaxf(0.0f, fminf(1.0f, alpha));

    const char *text = TextFormat("WAVE %d INICIADA!", manager->currentWave);
    int fontSize = 50;
    int textWidth = MeasureText(text, fontSize);

    Color color = Fade(RAYWHITE, alpha); // Cor da mensagem com o alpha calculado

    // Desenha a mensagem principal
    DrawText(text, (GAME_WIDTH - textWidth) / 2, GAME_HEIGHT / 2 - fontSize, fontSize, color);

    // Desenha o contador regressivo
    const char *timerText = TextFormat("%.1f", t);
    int timerFontSize = 30;
    int timerTextWidth = MeasureText(timerText, timerFontSize);

    // O contador é amarelo e usa o mesmo alpha
    DrawText(timerText, (GAME_WIDTH - timerTextWidth) / 2, GAME_HEIGHT / 2 + 20, timerFontSize, Fade(YELLOW, alpha));
}

int main(void) {
    // FULLSCREEN antes da janela
    SetConfigFlags(FLAG_FULLSCREEN_MODE);

    // Pega a resolução do monitor atual
    int monitorWidth  = GetMonitorWidth(GetCurrentMonitor());
    int monitorHeight = GetMonitorHeight(GetCurrentMonitor());

    // Inicializa a janela
    InitWindow(monitorWidth, monitorHeight, "Byte in Space 2");
    SetTargetFPS(60); // Define o FPS alvo

    // --- RENDER TEXTURE (O Jogo em 800x600) ---
    // Cria uma textura para renderizar todo o jogo em baixa resolução (800x600)
    // Depois, essa textura será esticada e o shader CRT será aplicado nela.
    RenderTexture2D target = LoadRenderTexture(GAME_WIDTH, GAME_HEIGHT);
    InitAudioDevice();

    // --- SHADER CRT ---
    // Carrega o shader que dá o efeito de tela de tubo (CRT)
    crtShader = LoadShader(0, "assets/shaders/crt22.fs");

    // Pega a localização das variáveis 'uniform' no shader
    locResolution = GetShaderLocation(crtShader, "resolution");
    locTime       = GetShaderLocation(crtShader, "time");

    // Passa a resolução inicial do monitor para o shader
    float initialRes[2] = { (float)monitorWidth, (float)monitorHeight };
    SetShaderValue(crtShader, locResolution, initialRes, SHADER_UNIFORM_VEC2);
    // ----------------------------------------------------

    // --- INICIALIZAÇÃO DOS COMPONENTES DO JOGO ---

    // Inicializa o campo de estrelas principal (800x600)
    StarField starField = { 0 };
    InitStarField(&starField, STAR_COUNT, GAME_WIDTH, GAME_HEIGHT);

    // Inicializa o campo de estrelas lateral (preenche as bordas pretas)
    InitStarField(&sideStarField, STAR_COUNT / 2, monitorWidth, monitorHeight);

    // Inicializa Jogador
    Player player;
    InitPlayer(&player);
    player.gold = 0; // Começa sem ouro

    // Inicializa HUD
    Hud hud;
    InitHud(&hud);

    // Inicializa Gerenciador de Balas
    BulletManager bulletManager;
    InitBulletManager(&bulletManager);

    // Inicialização do Gerenciador de Inimigos
    EnemyManager enemyManager;
    InitEnemyManager(&enemyManager, GAME_WIDTH, GAME_HEIGHT);

    // Inicialização do Gerenciador de Áudio (Global)
    InitAudioManager(&audioManager);

    // Inicializa Loja
    ShopScene shop;
    InitShop(&shop, &player, GAME_WIDTH, GAME_HEIGHT);

    // Inicializa Cutscene (usada para intro e final)
    CutsceneScene cutscene;
    InitCutscene(&cutscene);

    // --- MÁQUINA DE ESTADOS ---
    GameState currentState = STATE_CUTSCENE; // O jogo começa na cutscene de introdução

    PlayMusicTrack(&audioManager, MUSIC_CUTSCENE); // Toca a música da cutscene

    // Posicionamento inicial do jogador no centro-inferior
    float player_width_scaled  = player.texture.width * player.scale;
    float player_height_scaled = player.texture.height * player.scale;

    player.position.x = GAME_WIDTH/2 - player_width_scaled/2;
    player.position.y = GAME_HEIGHT - player_height_scaled - 100.0f;


    // --- LOOP PRINCIPAL DO JOGO ---
    while (!WindowShouldClose()) {

        // Verifica se ESC foi pressionado para fechar
        if (IsKeyPressed(KEY_ESCAPE)) break;

        float dt = GetFrameTime(); // Delta Time (tempo desde o último frame)

        // --- CÁLCULO DE ESCALA PARA RENDERIZAÇÃO NA TELA ---
        int screenW = GetScreenWidth();
        int screenH = GetScreenHeight();
        // Calcula a proporção de escala (largura e altura)
        float scaleX = (float)screenW / GAME_WIDTH;
        float scaleY = (float)screenH / GAME_HEIGHT;
        // Usa a menor escala para manter a proporção (letterbox/pillarbox)
        float scale = (scaleX < scaleY) ? scaleX : scaleY;

        // Calcula os offsets (margens) para centralizar a imagem
        int offsetX = (screenW - (int)(GAME_WIDTH * scale)) / 2;
        int offsetY = (screenH - (int)(GAME_HEIGHT * scale)) / 2;

        // --- UPDATE DO SHADER ---
        float t = GetTime();
        // Passa o tempo atual para o shader (usado para animações/efeitos de varredura)
        SetShaderValue(crtShader, locTime, &t, SHADER_UNIFORM_FLOAT);

        // Passa a resolução da tela real para o shader (pode ser útil para alguns efeitos)
        float res[2] = { (float)screenW, (float)screenH };
        SetShaderValue(crtShader, locResolution, res, SHADER_UNIFORM_VEC2);
        // ---------------------------

        // --- UPDATE GLOBAL ---
        UpdateAudioManager(&audioManager);
        UpdateStarField(&sideStarField, dt); // Atualiza o campo de estrelas lateral

        // --- MÁQUINA DE ESTADOS: LÓGICA DE ATUALIZAÇÃO ---
        switch (currentState) {
            case STATE_CUTSCENE:
                // Atualiza a cutscene de introdução
                UpdateCutscene(&cutscene, &currentState, dt);

                // Se a janela fechou no UpdateCutscene (caso o jogo permita fechar a cutscene), pare o loop.
                if (WindowShouldClose()) break;

                // Transição: Cutscene -> Loja
                if (currentState == STATE_SHOP) {
                    StopMusicStream(audioManager.musicCutscene);
                    PlayMusicTrack(&audioManager, MUSIC_SHOP);
                }
                break;

            // --- ESTADO FINAL (COMICS) ---
            case STATE_ENDING:
                // Atualiza a cutscene de final
                UpdateCutscene(&cutscene, &currentState, dt);
                // Se o jogo for fechado pela cutscene (após o último quadrinho), saia do loop principal
                if (WindowShouldClose()) break;
                break;


            case STATE_SHOP:
                // Atualiza a Loja e o campo de estrelas
                UpdateShop(&shop, &player, &starField, &currentState, dt);
                // Permite atirar na loja (para manter o sistema de balas ativo)
                UpdatePlayerBullets(&bulletManager, dt);

                // Transição: Loja -> Gameplay
                if (currentState == STATE_GAMEPLAY) {
                    // Ao retornar da loja, a wave será iniciada pelo loop de gameplay.
                    StopMusicStream(audioManager.musicShop);
                    PlayMusicTrack(&audioManager, MUSIC_GAMEPLAY);
                }
                break;

            case STATE_GAMEPLAY:
                UpdateStarField(&starField, dt);
                UpdateHud(&hud, dt);

                // Variável de controle: Pausa de Ação
                // O movimento do jogador e colisões são pausados se:
                // 1. A transição Loja/Wave está ativa (após o fim da wave).
                // 2. O contador de início da wave está rodando.
                // 3. O jogo acabou (Game Over).
                bool isActionPaused = enemyManager.triggerShopReturn || enemyManager.waveStartTimer > 0 || enemyManager.gameOver;

                // 1. Atualização e Movimento do Player (só se não estiver pausado)
                if (!isActionPaused) {
                    UpdatePlayer(&player, &bulletManager, &audioManager, &hud, dt, GAME_WIDTH, GAME_HEIGHT);
                }

                // 2. Movimento das Balas (sempre atualiza, mesmo pausado, para que as balas existentes sumam)
                UpdatePlayerBullets(&bulletManager, dt);

                // 3. Atualização de Inimigos e Timers
                // O gerenciador de inimigos lida com o início de waves, spawn e movimento.
                UpdateEnemies(&enemyManager, dt, GAME_WIDTH, &player.currentLives, &enemyManager.gameOver);

                // 4. Lógica de Colisão (só se não estiver pausado)
                if (!isActionPaused) {
                    CheckBulletEnemyCollision(&bulletManager, &enemyManager, &player.gold, &audioManager);
                }


                // LÓGICA DE TRANSIÇÃO SHOP/WAVE/ENDING (Teclas E e F)
                if (enemyManager.triggerShopReturn) { // Se a wave acabou

                    // --- CONDIÇÃO DE VITÓRIA FINAL (WAVE 10) ---
                    if (enemyManager.currentWave == 10) {
                        InitEnding(&cutscene); // Inicializa a cutscene de final
                        currentState = STATE_ENDING; // Muda para o estado de final
                        enemyManager.triggerShopReturn = false;
                        StopMusicStream(audioManager.musicGameplay); // Para a música de combate
                    }
                    // --- FIM VERIFICAÇÃO DE FINAL DE JOGO ---

                    else {
                        // [E] para ir para a loja
                        if (IsKeyPressed(KEY_E)) {
                            currentState = STATE_SHOP;
                            enemyManager.triggerShopReturn = false;
                            StopMusicStream(audioManager.musicGameplay);
                            PlayMusicTrack(&audioManager, MUSIC_SHOP);
                        }
                        // [F] para continuar a próxima wave
                        if (IsKeyPressed(KEY_F)) {
                            enemyManager.triggerShopReturn = false; // Desativa a interface
                            // O gerenciador de inimigos iniciará o timer da próxima wave automaticamente.
                        }
                    }
                }
                // --- FIM LÓGICA DE TRANSIÇÃO SHOP/WAVE/ENDING ---
                break;
        }

        // --- RENDER PARA O TARGET (800x600) ---
        // Tudo que está aqui será desenhado na pequena textura de 800x600.
        BeginTextureMode(target);
            ClearBackground(BLACK);

            // MÁQUINA DE ESTADOS: LÓGICA DE DESENHO
            switch (currentState) {
                case STATE_CUTSCENE:
                case STATE_ENDING: // Desenha a mesma cutscene/ending
                    DrawCutscene(&cutscene, GAME_WIDTH, GAME_HEIGHT);
                    break;

                case STATE_SHOP:
                    DrawShop(&shop, &player, &starField);
                    DrawPlayerBullets(&bulletManager); // Desenha as balas na loja
                    break;

                case STATE_GAMEPLAY:
                    DrawStarField(&starField);

                    // Desenha a linha verde neon (Limite de Game Over para inimigos)
                    const Color NEON_GREEN_LINE = (Color){ 0, 255, 0, 255 };
                    DrawRectangle(0, (int)ENEMY_GAME_OVER_LINE_Y, GAME_WIDTH, 2, NEON_GREEN_LINE);
                    // Efeito glow na linha
                    DrawRectangle(0, (int)ENEMY_GAME_OVER_LINE_Y - 2, GAME_WIDTH, 2, Fade(NEON_GREEN_LINE, 0.4f));
                    DrawRectangle(0, (int)ENEMY_GAME_OVER_LINE_Y + 2, GAME_WIDTH, 2, Fade(NEON_GREEN_LINE, 0.4f));

                    // Desenha os inimigos, o jogador e as balas
                    DrawEnemies(&enemyManager);
                    DrawPlayer(&player);
                    DrawPlayerBullets(&bulletManager);

                    // Desenha a mensagem de início da wave (se o timer estiver ativo)
                    DrawWaveStartUI(&enemyManager);

                    // Desenha a interface de transição (se a wave terminou e NÃO for a última wave)
                    if (enemyManager.triggerShopReturn && enemyManager.currentWave < 10) {
                        DrawShopTransitionUI(&enemyManager);
                    }

                    // Tela de Game Over
                    if (enemyManager.gameOver) {
                        DrawRectangle(0, 0, GAME_WIDTH, GAME_HEIGHT, Fade(BLACK, 0.8f));
                        DrawText("GAME OVER", GAME_WIDTH/2 - MeasureText("GAME OVER", 40)/2, GAME_HEIGHT/2 - 20, 40, RED);
                        DrawText("OS INIMIGOS TE ALCANÇARAM!", GAME_WIDTH/2 - MeasureText("OS INIMIGOS TE ALCANÇARAM!", 20)/2, GAME_HEIGHT/2 + 30, 20, WHITE);
                    }
                    break;
            }
        EndTextureMode(); // Termina a renderização para a textura 'target'

        // --- RENDER FINAL PARA A TELA ---
        BeginDrawing();
            ClearBackground(BLACK);

            // Se houver margens pretas (pillarbox ou letterbox), desenha o campo de estrelas lateral nelas
            if (offsetX > 0 || offsetY > 0) {
                DrawStarField(&sideStarField);
            }

            // Aplica o shader CRT à imagem final
            BeginShaderMode(crtShader);

            // Desenha a textura 'target' esticada e centralizada na tela
            DrawTexturePro(
                target.texture,
                // Fonte (origem): Inverte o eixo Y para corrigir a renderização do target
                (Rectangle){0, 0, (float)target.texture.width, -(float)target.texture.height},
                // Destino: Posiciona usando offsets e escala
                (Rectangle){(float)offsetX, (float)offsetY, GAME_WIDTH * scale, GAME_HEIGHT * scale},
                (Vector2){0, 0}, // Origem da rotação
                0.0f,            // Rotação
                WHITE
            );

            EndShaderMode();

            // Desenho do HUD Lateral (se houver espaço nas laterais - offsetX > 0)
            if (currentState == STATE_GAMEPLAY && offsetX > 0) {
                // Desenha o HUD no lado esquerdo
                DrawHudSide(&hud, true, offsetY, player.energyCharge, player.hasDoubleShot, player.hasShield, player.extraLives, player.currentLives, player.gold);
                // Desenha o HUD no lado direito (passa 'false')
                DrawHudSide(&hud, false, offsetY, 0.0f, false, false, 0, player.currentLives, player.gold);
            }

        EndDrawing(); // Termina o desenho
    }

    // --- FINALIZAÇÃO (CLEANUP) ---
    // Descarrega todos os recursos da memória
    UnloadShop(&shop);
    UnloadPlayer(&player);
    UnloadRenderTexture(target);
    UnloadBulletManager(&bulletManager);
    UnloadAudioManager(&audioManager);

    UnloadShader(crtShader);

    // Fecha os dispositivos
    CloseAudioDevice();
    CloseWindow();
    return 0;
}