#include "raylib.h"
#include "player.h"
#include "hud.h"
#include "bullet.h"
#include "audio.h"
#include "star.h"
#include "game_state.h"
#include "shop.h"
#include "cutscene.h"
#include <stdio.h>

#define GAME_WIDTH 800
#define GAME_HEIGHT 600

const int STAR_COUNT = 150;

StarField sideStarField = { 0 };

// --- FUNÇÃO MAIN ---
int main(void) {
    // ConfigFlags deve ser chamado antes de InitWindow
    SetConfigFlags(FLAG_FULLSCREEN_MODE);
    int monitorWidth = GetMonitorWidth(GetCurrentMonitor());
    int monitorHeight = GetMonitorHeight(GetCurrentMonitor());

    InitWindow(monitorWidth, monitorHeight, "Byte in Space 2");

    // É importante definir o FPS alvo logo após a inicialização da janela
    SetTargetFPS(60);

    RenderTexture2D target = LoadRenderTexture(GAME_WIDTH, GAME_HEIGHT);
    InitAudioDevice();

    // --- 1. INICIALIZAÇÃO DOS COMPONENTES ---
    StarField starField = { 0 };
    InitStarField(&starField, STAR_COUNT, GAME_WIDTH, GAME_HEIGHT);
    InitStarField(&sideStarField, STAR_COUNT / 2, monitorWidth, monitorHeight);

    Player player;
    InitPlayer(&player);
    player.gold = 50; // GOLD INICIAL PARA TESTAR COMPRAS

    Hud hud;
    InitHud(&hud);

    BulletManager bulletManager;
    InitBulletManager(&bulletManager);

    AudioManager audioManager;
    InitAudioManager(&audioManager);

    ShopScene shop;
    InitShop(&shop, GAME_WIDTH, GAME_HEIGHT);

    CutsceneScene cutscene;
    InitCutscene(&cutscene);

    GameState currentState = STATE_CUTSCENE; // Inicia na cutscene

    // TOCA MÚSICA DA CUTSCENE AO INICIAR
    PlayMusicTrack(&audioManager, MUSIC_CUTSCENE);

    // Reposiciona Player para a loja
    float player_width_scaled = player.texture.width * player.scale;
    float player_height_scaled = (float)player.texture.height * player.scale;
    player.position.x = (float)GAME_WIDTH/2 - player_width_scaled/2;
    player.position.y = (float)GAME_HEIGHT - player_height_scaled - 100.0f;


    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_ESCAPE)) { break; }

        float dt = GetFrameTime();

        // --- CÁLCULOS DE ESCALA DA TELA ---
        int screenW = GetScreenWidth();
        int screenH = GetScreenHeight();
        float scaleX = (float)screenW / GAME_WIDTH;
        float scaleY = (float)screenH / GAME_HEIGHT;
        float scale = (scaleX < scaleY) ? scaleX : scaleY;
        int offsetX = (screenW - (int)((float)GAME_WIDTH * scale)) / 2;
        int offsetY = (screenH - (int)((float)GAME_HEIGHT * scale)) / 2;

        // --- UPDATE ---
        UpdateAudioManager(&audioManager);
        UpdateStarField(&sideStarField, dt);

        // MÁQUINA DE ESTADOS: LÓGICA
        switch (currentState) {
            case STATE_CUTSCENE: {
                UpdateCutscene(&cutscene, &currentState, dt);
                if (currentState == STATE_SHOP) {
                    StopMusicStream(audioManager.musicCutscene);
                    PlayMusicTrack(&audioManager, MUSIC_SHOP);
                }
                break;
            }

            case STATE_SHOP: {
                UpdateShop(&shop, &player, &starField, &currentState, dt);
                UpdatePlayerBullets(&bulletManager, dt);

                if (currentState == STATE_GAMEPLAY) {
                    PlayMusicTrack(&audioManager, MUSIC_GAMEPLAY);
                }
                break;
            }

            case STATE_GAMEPLAY:
                UpdateStarField(&starField, dt);
                UpdateHud(&hud, dt);
                UpdatePlayer(&player, &bulletManager, &audioManager, &hud, dt, GAME_WIDTH, GAME_HEIGHT);
                UpdatePlayerBullets(&bulletManager, dt);
                break;
        }

        // --- 3. DESENHO NA RENDER TEXTURE (800x600) ---
        BeginTextureMode(target);
            ClearBackground(BLACK);

            // MÁQUINA DE ESTADOS: DESENHO
            switch (currentState) {
                case STATE_CUTSCENE:
                    DrawCutscene(&cutscene, GAME_WIDTH, GAME_HEIGHT);
                    break;

                case STATE_SHOP:
                    DrawShop(&shop, &player, &starField);
                    DrawPlayerBullets(&bulletManager);
                    // IMPORTANTE: Não chame DrawHud ou DrawDialog aqui. O diálogo é feito dentro de DrawShop.
                    break;

                case STATE_GAMEPLAY:
                    DrawStarField(&starField);
                    DrawPlayer(&player);
                    DrawPlayerBullets(&bulletManager);
                    // DrawHud(&hud, &player); // (Comentada pois não está implementada em hud.c)
                    break;
            }
        EndTextureMode();

        // --- 4. DESENHO NA TELA REAL (USANDO ESCALA) ---
        BeginDrawing();
            ClearBackground(BLACK);

            // Desenha o StarField Lateral para preencher as bordas
            if (offsetX > 0 || offsetY > 0) {
                DrawStarField(&sideStarField);
            }

            // Desenha a área de jogo 800x600 escalonada
            DrawTexturePro(target.texture,
                (Rectangle){ 0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height },
                (Rectangle){ (float)offsetX, (float)offsetY, (float)GAME_WIDTH * scale, (float)GAME_HEIGHT * scale },
                (Vector2){ 0, 0 }, 0.0f, WHITE);

            // Desenha a HUD nas barras laterais
            if (currentState == STATE_GAMEPLAY && offsetX > 0) {
                DrawHudSide(&hud, true, offsetY, player.energyCharge);
                DrawHudSide(&hud, false, offsetY, 0.0f);
            }

            DrawFPS(10, 50);

        EndDrawing();
    }

    // --- DESCARREGAMENTO (UNLOAD) ---
    UnloadShop(&shop);
    UnloadPlayer(&player);
    UnloadRenderTexture(target);
    UnloadBulletManager(&bulletManager);
    UnloadAudioManager(&audioManager);

    CloseAudioDevice();
    CloseWindow();
    return 0;
}