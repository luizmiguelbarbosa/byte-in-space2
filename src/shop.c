#include "shop.h"
#include <stdio.h>
#include <math.h>

// ATUALIZADO: Novo caminho e nome de arquivo
#define VENDOR_PATH "assets/images/sprites/vendedor_sheet2.png"

// ATUALIZADO: Novas dimensões do frame
#define VENDOR_BASE_FRAME_W 476.0f
#define VENDOR_BASE_FRAME_H 450.0f
#define VENDOR_DRAW_SCALE 0.16f   // AJUSTADO: Escala para 0.16f (Sprite final terá ~76x72 pixels, ótimo para a cena)
#define HORIZON_OFFSET_Y -50.0f
// Velocidade da animação (ajuste se for muito rápido/lento)
#define VENDOR_ANIM_SPEED 0.3f

void InitShop(ShopScene *shop, int gameWidth, int gameHeight) {
    // 1. Carregar Sprite do Vendedor
    shop->vendor.texture = LoadTexture(VENDOR_PATH);
    if (shop->vendor.texture.id == 0) {
        printf("[ERRO] Nao foi possivel carregar o sprite do vendedor em: %s\n", VENDOR_PATH);
    }
    SetTextureFilter(shop->vendor.texture, TEXTURE_FILTER_POINT);

    // Calcula o número de frames
    shop->vendor.frameCountX = shop->vendor.texture.width / (int)VENDOR_BASE_FRAME_W;
    shop->vendor.frameCountY = shop->vendor.texture.height / (int)VENDOR_BASE_FRAME_H;

    if (shop->vendor.frameCountX == 0) shop->vendor.frameCountX = 1;
    if (shop->vendor.frameCountY == 0) shop->vendor.frameCountY = 1;

    shop->vendor.frameRec = (Rectangle){ 0.0f, 0.0f, VENDOR_BASE_FRAME_W, VENDOR_BASE_FRAME_H };

    // Posiciona o vendedor no horizonte
    float vendorDrawHeight = VENDOR_BASE_FRAME_H * VENDOR_DRAW_SCALE;
    float horizonY = (float)gameHeight / 2 + HORIZON_OFFSET_Y;
    shop->vendor.position = (Vector2){
        (float)gameWidth/2 - (VENDOR_BASE_FRAME_W * VENDOR_DRAW_SCALE)/2,
        horizonY - vendorDrawHeight + 10
    };

    shop->vendor.currentFrame = 0;
    shop->vendor.frameTimer = 0.0f;
    shop->vendor.isHappy = false;
    shop->vendor.happyTimer = 0.0f;
    shop->vendor.scale = VENDOR_DRAW_SCALE;

    // 2. Configurar Itens
    float midX = (float)gameWidth / 2;
    float floorY = (float)gameHeight / 2 + 60;

    shop->items[0] = (ShopItem){ { midX - 160, floorY, 40, 40 }, "TIRO RAPIDO", 0, RED, true };
    shop->items[1] = (ShopItem){ { midX - 20, floorY, 40, 40 }, "ESCUDO", 0, BLUE, true };
    shop->items[2] = (ShopItem){ { midX + 120, floorY, 40, 40 }, "VIDA EXTRA", 0, GREEN, true };

    sprintf(shop->dialogText, "OLA VIAJANTE! ESTE PRIMEIRO UPGRADE E POR MINHA CONTA.");
    shop->itemBought = false;
    shop->itemFocused = false;
}

void DrawShopEnvironment(int width, int height) {
    int horizonY = height / 2 + (int)HORIZON_OFFSET_Y;
    int centerX = width / 2;

    DrawRectangleGradientV(0, horizonY, width, height - horizonY, (Color){10, 10, 30, 255}, BLACK);
    DrawLine(0, horizonY, width, horizonY, GREEN);

    for (int i = -10; i <= 10; i++) {
        Vector2 start = { centerX + (i * 20), horizonY };
        Vector2 end = { centerX + (i * 100), height };
        DrawLineEx(start, end, 1.0f, Fade(GREEN, 0.3f));
    }

    for (int i = 0; i < 10; i++) {
        float y = horizonY + (i * i * 4) + 10;
        if (y > height) break;
        DrawLine(0, (int)y, width, (int)y, Fade(GREEN, 0.3f));
    }

    DrawEllipse(centerX, horizonY - 10, 100, 30, Fade(BLACK, 0.8f));
}

void UpdateShop(ShopScene *shop, Player *player, StarField *stars, GameState *state, float deltaTime) {
    UpdateStarField(stars, deltaTime);

    // --- Movimentação Livre do Player NA LOJA ---
    if (!shop->itemBought) {
        float speed = player->speed * deltaTime;
        if (IsKeyDown(KEY_LEFT)) player->position.x -= speed;
        if (IsKeyDown(KEY_RIGHT)) player->position.x += speed;
        if (IsKeyDown(KEY_UP)) player->position.y -= speed;
        if (IsKeyDown(KEY_DOWN)) player->position.y += speed;

        // Limite da área caminhável na loja
        if (player->position.x < 0) player->position.x = 0;
        if (player->position.x > 800 - player->texture.width * player->scale) player->position.x = 800 - player->texture.width * player->scale;
        if (player->position.y < (float)600/2 - 50) player->position.y = (float)600/2 - 50;
        if (player->position.y > 600 - player->texture.height * player->scale - 10) player->position.y = 600 - player->texture.height * player->scale - 10;
    }

    // --- Animação do Vendedor ---
    shop->vendor.frameTimer += deltaTime;
    int framesPerRow = shop->vendor.frameCountX;
    int maxFramesIdle = framesPerRow * 5; // Assumindo as primeiras 5 linhas são Idle/Looping
    int maxFramesHappy = framesPerRow * 2; // Assumindo duas linhas para Happy

    if (!shop->vendor.isHappy) {
        // IDLE (loop pelas primeiras N linhas)
        if (shop->vendor.frameTimer >= VENDOR_ANIM_SPEED) {
            shop->vendor.frameTimer = 0.0f;
            shop->vendor.currentFrame = (shop->vendor.currentFrame + 1) % maxFramesIdle;

            // Calcula a linha e coluna
            int row = shop->vendor.currentFrame / framesPerRow;
            int col = shop->vendor.currentFrame % framesPerRow;

            shop->vendor.frameRec.x = (float)col * shop->vendor.frameRec.width;
            shop->vendor.frameRec.y = (float)row * shop->vendor.frameRec.height;
        }
    } else {
        // HAPPY (loop pelas últimas linhas)
        // Isso assume que os frames Happy começam na linha 6 (índice 5)
        int startRow = 5;

        if (shop->vendor.frameTimer >= VENDOR_ANIM_SPEED / 2.0f) {
            shop->vendor.frameTimer = 0.0f;
            shop->vendor.currentFrame = (shop->vendor.currentFrame + 1) % maxFramesHappy;

            // Calcula a linha e coluna (a partir da linha de início)
            int row = startRow + (shop->vendor.currentFrame / framesPerRow);
            int col = shop->vendor.currentFrame % framesPerRow;

            // Limita a linha para não sair do spritesheet
            if (row < shop->vendor.frameCountY) {
                shop->vendor.frameRec.x = (float)col * shop->vendor.frameRec.width;
                shop->vendor.frameRec.y = (float)row * shop->vendor.frameRec.height;
            } else {
                 // Se sair do limite, volta para o último frame válido da animação Happy
                 shop->vendor.frameRec.x = (float)(framesPerRow - 1) * shop->vendor.frameRec.width;
                 shop->vendor.frameRec.y = (float)(shop->vendor.frameCountY - 1) * shop->vendor.frameRec.height;
            }
        }

        shop->vendor.happyTimer += deltaTime;
        if (shop->vendor.happyTimer > 3.0f) {
            *state = STATE_GAMEPLAY;
            float pW = player->texture.width * player->scale;
            player->position = (Vector2){ 400 - pW/2, 600 - 100 };
            shop->vendor.isHappy = false;
            shop->vendor.happyTimer = 0.0f;
        }
    }

    // --- Lógica de Compra e Dinheiro ---
    if (!shop->itemBought) {
        float pW = player->texture.width * player->scale;
        float pH = player->texture.height * player->scale;
        Rectangle playerRect = { player->position.x, player->position.y, pW, pH };

        bool isPlayerNearItem = false;

        for (int i = 0; i < 3; i++) {
            if (shop->items[i].active) {
                if (CheckCollisionRecs(playerRect, shop->items[i].rect)) {
                    isPlayerNearItem = true;

                    char priceText[32];
                    if (shop->items[i].price == 0) {
                        sprintf(priceText, "GRATUITO");
                    } else {
                        sprintf(priceText, "$%d", shop->items[i].price);
                    }

                    sprintf(shop->dialogText, "COMPRAR %s POR %s? PRESSIONE E.", shop->items[i].name, priceText);

                    if (IsKeyPressed(KEY_E)) {
                        if (player->gold >= shop->items[i].price) {
                            player->gold -= shop->items[i].price;
                            shop->itemBought = true;
                            shop->vendor.isHappy = true;
                            shop->vendor.currentFrame = 0;
                            shop->items[i].active = false;
                            sprintf(shop->dialogText, "NEGOCIO FECHADO! SISTEMAS ONLINE... INICIANDO...");
                        } else {
                            sprintf(shop->dialogText, "VOCE NAO TEM CREDITOS SUFICIENTES! (%s)", shop->items[i].name);
                        }
                    }
                }
            }
        }

        if (!isPlayerNearItem) {
            if (shop->items[0].active) {
                 sprintf(shop->dialogText, "OLA VIAJANTE! ESTE PRIMEIRO UPGRADE E POR MINHA CONTA.");
            } else {
                sprintf(shop->dialogText, "EXPLORE A LOJA! USE AS SETAS PARA SE MOVER.");
            }
        }
    }
}

void DrawShop(ShopScene *shop, Player *player, StarField *stars) {
    // 1. Estrelas e Cenário
    DrawStarField(stars);
    DrawShopEnvironment(800, 600);

    // 2. Itens
    if (!shop->itemBought) {
        for (int i = 0; i < 3; i++) {
            if (shop->items[i].active) {
                float floatY = sin(GetTime() * 3 + i) * 3;
                Rectangle drawRect = shop->items[i].rect;
                drawRect.y += floatY;

                DrawRectangleRec(drawRect, shop->items[i].color);
                DrawRectangleLinesEx(drawRect, 2, WHITE);

                DrawText(shop->items[i].name, (int)drawRect.x - 20, (int)drawRect.y - 25, 10, GREEN);

                char priceText[16];
                if (shop->items[i].price == 0) sprintf(priceText, "FREE");
                else sprintf(priceText, "$%d", shop->items[i].price);

                DrawText(priceText, (int)drawRect.x + 5, (int)drawRect.y + 45, 10, YELLOW);
            }
        }
    }

    // 3. Vendedor
    if (shop->vendor.texture.id != 0) {
        Rectangle src = shop->vendor.frameRec;

        float destW = shop->vendor.frameRec.width * shop->vendor.scale;
        float destH = shop->vendor.frameRec.height * shop->vendor.scale;
        Rectangle dest = { shop->vendor.position.x, shop->vendor.position.y, destW, destH };

        Vector2 origin = { 0, 0 };

        DrawTexturePro(shop->vendor.texture, src, dest, origin, 0.0f, WHITE);
    } else {
        DrawRectangle(shop->vendor.position.x, shop->vendor.position.y, VENDOR_BASE_FRAME_W*shop->vendor.scale, VENDOR_BASE_FRAME_H*shop->vendor.scale, PURPLE);
        DrawText("SPRITE ERROR", shop->vendor.position.x, shop->vendor.position.y, 10, WHITE);
    }

    // 4. Player
    DrawPlayer(player);

    // 5. HUD de Dinheiro (Local da Loja)
    DrawText(TextFormat("CREDITOS: $%d", player->gold), 10, 10, 20, GOLD);

    // 6. Dialog Box (rodapé) - DIÁLOGO PRINCIPAL
    int boxH = 100;
    DrawRectangle(0, 600 - boxH, 800, boxH, Fade(BLACK, 0.9f));
    DrawRectangleLines(0, 600 - boxH, 800, boxH, GREEN);
    DrawText(shop->dialogText, 20, 600 - 70, 20, GREEN);
}

void UnloadShop(ShopScene *shop) {
    if (shop->vendor.texture.id != 0) UnloadTexture(shop->vendor.texture);
}