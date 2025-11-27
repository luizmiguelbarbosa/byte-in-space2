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

// Shader global
Shader crtShader;

// Uniform locations
int locResolution;
int locTime;

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
    crtShader = LoadShader(0, "assets/shaders/crt16.fs");

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
                    StopMusicStream(audioManager.musicShop);
                    PlayMusicTrack(&audioManager, MUSIC_GAMEPLAY);
                }
                break;

            case STATE_GAMEPLAY:
                UpdateStarField(&starField, dt);
                UpdateHud(&hud, dt);
                UpdatePlayer(&player, &bulletManager, &audioManager, &hud, dt, GAME_WIDTH, GAME_HEIGHT);
                UpdatePlayerBullets(&bulletManager, dt);
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
                    DrawPlayer(&player);
                    DrawPlayerBullets(&bulletManager);
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
                DrawHudSide(&hud, true, offsetY, player.energyCharge, player.hasDoubleShot, player.hasShield, player.extraLives);
                DrawHudSide(&hud, false, offsetY, 0.0f, false, false, 0);
            }

            DrawFPS(10, 50);

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
