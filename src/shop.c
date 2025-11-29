#include "shop.h"
#include "player.h"
#include "raylib.h"
#include <stdio.h>
#include <math.h>

#define VENDOR_BASE_FRAME_W 64.0f
#define VENDOR_BASE_FRAME_H 64.0f
#define VENDOR_DRAW_SCALE 6.0f
#define HORIZON_OFFSET_Y -50.0f
#define PARTICLE_LIFETIME 0.5f

#define ENERGY_POWERUP_PATH "assets/images/sprites/energy_icon.png"
#define SHURIKEN_PATH "assets/images/sprites/icone_powerup_shurikens.png"
#define SHIELD_PATH "assets/images/sprites/shield.png"
#define EXTRA_LIFE_PATH "assets/images/sprites/vidaextra.png"

#define TEXT_BOX_HEIGHT 100
#define DIALOG_FONT_SIZE 18
#define DIALOG_TEXT_Y_OFFSET 40

#define PORTAL_Z_DISTANCE 0.5f
#define PORTAL_BASE_Y_OFFSET 150.0f

#define PORTAL_BRIGHT_CYAN (Color){ 100, 255, 255, 255 }
#define PORTAL_DARK_BLUE (Color){ 0, 0, 100, 255 }

const float ITEM_SIZE_SCALED = 60.0f;


void InitShop(ShopScene *shop, Player *player, int gameWidth, int gameHeight) {
    shop->vendor.frameRec = (Rectangle){ 0.0f, 0.0f, VENDOR_BASE_FRAME_W, VENDOR_BASE_FRAME_H };
    shop->vendor.scale = VENDOR_DRAW_SCALE;
    shop->vendor.isHappy = false;
    shop->vendor.happyTimer = 0.0f;
    shop->portalParallaxOffset = 0.0f;

    float vendorDrawWidth = shop->vendor.frameRec.width * shop->vendor.scale;
    float vendorDrawHeight = shop->vendor.frameRec.height * shop->vendor.scale;
    float horizonY = (float)gameHeight / 2 + HORIZON_OFFSET_Y;

    float collisionY = horizonY + 50.0f;
    float collisionW = 40.0f;
    float collisionH = 20.0f;

    shop->exitArea = (Rectangle){
        (float)gameWidth/2 - collisionW / 2,
        collisionY,
        collisionW,
        collisionH
    };

    float playerH = player->texture.height * player->scale;
    float playerW = player->texture.width * player->scale;
    player->position = (Vector2){
        (float)gameWidth / 2 - playerW / 2,
        600.0f - TEXT_BOX_HEIGHT - playerH - 10.0f
    };

    shop->itemTextures[0] = LoadTexture(ENERGY_POWERUP_PATH);
    shop->itemTextures[1] = LoadTexture(SHURIKEN_PATH);
    shop->itemTextures[2] = LoadTexture(SHIELD_PATH);
    shop->itemTextures[3] = LoadTexture(EXTRA_LIFE_PATH);

    for (int i = 0; i < MAX_SHOP_ITEMS; i++) {
        if (shop->itemTextures[i].id != 0) SetTextureFilter(shop->itemTextures[i], TEXTURE_FILTER_POINT);
    }


    float midX = (float)gameWidth / 2;
    float floorY = (float)gameHeight / 2 + 60;

    float itemSpacing = 20.0f;
    float totalItemsWidth = (ITEM_SIZE_SCALED * MAX_SHOP_ITEMS) + (itemSpacing * (MAX_SHOP_ITEMS - 1));
    float startX = midX - totalItemsWidth / 2;

    // NOVOS PREÇOS AUMENTADOS
    shop->items[0] = (ShopItem){ { startX, floorY, ITEM_SIZE_SCALED, ITEM_SIZE_SCALED }, "Carga de Energia", 0, WHITE, true, ITEM_ENERGY_CHARGE };
    shop->items[1] = (ShopItem){ { startX + ITEM_SIZE_SCALED + itemSpacing, floorY, ITEM_SIZE_SCALED, ITEM_SIZE_SCALED }, "SHURIKENS CANINAS", 750, RED, true, ITEM_SHURIKEN };
    shop->items[2] = (ShopItem){ { startX + (ITEM_SIZE_SCALED + itemSpacing) * 2, floorY, ITEM_SIZE_SCALED, ITEM_SIZE_SCALED }, "ESCUDO", 1500, BLUE, true, ITEM_SHIELD };
    shop->items[3] = (ShopItem){ { startX + (ITEM_SIZE_SCALED + itemSpacing) * 3, floorY, ITEM_SIZE_SCALED, ITEM_SIZE_SCALED }, "VIDA EXTRA", 2250, GREEN, true, ITEM_EXTRA_LIFE };

    shop->particleTimer = 0.0f;
    shop->showParticles = false;

    sprintf(shop->dialogText, "SEJA BEM-VINDO, VIAJANTE! O UPGRADE DE ENERGIA E POR MINHA CONTA.");
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

    #ifndef __RAYMATH_H__
        #define CLAMP(v, min, max) ((v) < (min) ? (min) : ((v) > (max) ? (max) : (v)))
        #define Clamp(v, min, max) CLAMP((v), (min), (max))
    #endif

    float pW = player->texture.width * player->scale;
    float pH = player->texture.height * player->scale;
    Rectangle playerRect = { player->position.x, player->position.y, pW, pH };

    if (!shop->itemBought) {
        float speed = player->speed * deltaTime;
        if (IsKeyDown(KEY_LEFT)) player->position.x -= speed;
        if (IsKeyDown(KEY_RIGHT)) player->position.x += speed;
        if (IsKeyDown(KEY_UP)) player->position.y -= speed;
        if (IsKeyDown(KEY_DOWN)) player->position.y += speed;

        player->position.x = Clamp(player->position.x, 0.0f, 800.0f - pW);

        player->position.y = Clamp(player->position.y, shop->exitArea.y - pH, 600.0f - pH - TEXT_BOX_HEIGHT);

        float playerRelativeX = (player->position.x - (800.0f / 2.0f)) / (800.0f / 2.0f);
        shop->portalParallaxOffset = playerRelativeX * 50.0f * PORTAL_Z_DISTANCE;

        if (CheckCollisionRecs(playerRect, shop->exitArea)) {
            *state = STATE_GAMEPLAY;
            player->position = (Vector2){ 400 - pW/2, 600 - 100 };
            return;
        }
    }

    if (shop->vendor.isHappy) {
        shop->vendor.happyTimer += deltaTime;
        if (shop->vendor.happyTimer > 3.0f) {
            shop->vendor.isHappy = false;
            shop->vendor.happyTimer = 0.0f;
        }
    }

    if (shop->itemBought) {
        shop->showParticles = true;
        shop->particleTimer += deltaTime;

        if (shop->particleTimer > PARTICLE_LIFETIME) {
            shop->itemBought = false;
            shop->particleTimer = 0.0f;
            shop->showParticles = false;
            shop->vendor.isHappy = true;
        }
    }

    if (!shop->itemBought) {
        bool isPlayerNearItem = false;

        for (int i = 0; i < MAX_SHOP_ITEMS; i++) {
            bool shouldCheck = shop->items[i].active || (shop->items[i].type == ITEM_ENERGY_CHARGE && !player->canCharge);

            if (shouldCheck) {
                if (CheckCollisionRecs(playerRect, shop->items[i].rect)) {
                    isPlayerNearItem = true;

                    if (shop->items[i].type == ITEM_ENERGY_CHARGE) {
                        if (!player->canCharge) {
                            sprintf(shop->dialogText, "UPGRADE DE ENERGIA: HABILITA O TIRO CARREGADO! PRESSIONE E.");

                            if (IsKeyPressed(KEY_E)) {
                                player->canCharge = true;
                                shop->itemBought = true;
                                shop->items[i].active = false;
                                sprintf(shop->dialogText, "SISTEMAS ONLINE! CARGA DE ENERGIA HABILITADA.");
                            }
                        }
                    }
                    else if (shop->items[i].active) {
                        char priceText[32];
                        sprintf(priceText, "$%d", shop->items[i].price);

                        sprintf(shop->dialogText, "COMPRAR %s POR %s? PRESSIONE E.", shop->items[i].name, priceText);

                        if (IsKeyPressed(KEY_E)) {
                            if (player->gold >= shop->items[i].price) {

                                switch (shop->items[i].type) {
                                    case ITEM_SHURIKEN: player->hasDoubleShot = true; break;
                                    case ITEM_SHIELD: player->hasShield = true; break;
                                    case ITEM_EXTRA_LIFE: player->extraLives++; break;
                                    default: break;
                                }

                                player->gold -= shop->items[i].price;
                                shop->itemBought = true;
                                shop->items[i].active = false;
                                sprintf(shop->dialogText, "NEGOCIO FECHADO! %s ATIVADO! Use o PORTAL para sair.", shop->items[i].name);
                            } else {
                                sprintf(shop->dialogText, "CREDITOS INSUFICIENTES! Voce precisa de mais $%d para comprar %s.", shop->items[i].price, shop->items[i].name);
                            }
                        }
                    }
                }
            }
        }

        if (!isPlayerNearItem) {
            if (!player->canCharge) {
                 sprintf(shop->dialogText, "SEJA BEM-VINDO, VIAJANTE! O UPGRADE DE ENERGIA E POR MINHA CONTA.");
            } else {
                sprintf(shop->dialogText, "EXPLORE OS PRODUTOS! Mova-se ate o portal no centro para SAIR. CREDITOS: $%d", player->gold);
            }
        }
    }
}

void DrawShop(ShopScene *shop, Player *player, StarField *stars) {
    DrawStarField(stars);
    DrawShopEnvironment(800, 600);

    float vendorDrawWidth = VENDOR_BASE_FRAME_W * VENDOR_DRAW_SCALE;
    float vendorDrawHeight = VENDOR_BASE_FRAME_H * VENDOR_DRAW_SCALE;

    float portalBaseY = shop->exitArea.y + shop->exitArea.height / 2;
    float time = GetTime();
    float pulse = sin(time * 4.0f) * 0.1f + 0.9f;

    Rectangle portalVisualRect = {
        shop->exitArea.x + shop->portalParallaxOffset - (vendorDrawWidth - shop->exitArea.width) / 2,
        portalBaseY - vendorDrawHeight,
        vendorDrawWidth,
        vendorDrawHeight
    };
    float portalDrawCenterX = portalVisualRect.x + portalVisualRect.width / 2;


    Color topColor = Fade(SKYBLUE, 0.7f * pulse);
    Color bottomColor = Fade(BLUE, 0.9f * pulse);

    DrawCircleGradient(
        (int)portalDrawCenterX,
        (int)(portalVisualRect.y + portalVisualRect.height),
        portalVisualRect.width / 2.0f * pulse,
        PORTAL_BRIGHT_CYAN,
        Fade(PORTAL_DARK_BLUE, 0.0f)
    );

    DrawRectangleGradientV(
        (int)portalVisualRect.x,
        (int)portalVisualRect.y,
        (int)portalVisualRect.width,
        (int)portalVisualRect.height,
        topColor,
        bottomColor
    );

    DrawRectangleLinesEx(portalVisualRect, 3.0f, Fade((Color){ 0, 255, 255, 255 }, 0.8f * pulse));

    for (int i = 0; i < 5; i++) {
        float angle = time * (10 + i * 2) + i * 0.5f;
        float radius = portalVisualRect.width / 2.0f + sin(time * (3 + i)) * 10.0f;
        float xOffset = cos(angle) * radius;
        float yOffset = sin(angle) * radius * 0.5f;

        Color brightPulseColor = Fade(PORTAL_BRIGHT_CYAN, 0.5f + sin(time * (5 + i)) * 0.3f);
        DrawCircleV((Vector2){ portalDrawCenterX + xOffset, portalVisualRect.y + portalVisualRect.height/2 + yOffset }, 5.0f * pulse, brightPulseColor);
    }

    DrawText("PORTAL DE SAIDA",
        (int)portalDrawCenterX - MeasureText("PORTAL DE SAIDA", 20)/2,
        (int)(portalVisualRect.y + portalVisualRect.height) - 30,
        20,
        Fade(WHITE, 0.9f)
    );

    for (int i = 0; i < MAX_SHOP_ITEMS; i++) {
        bool shouldDrawItem = shop->items[i].active || (shop->items[i].type == ITEM_ENERGY_CHARGE && !player->canCharge);

        if (shouldDrawItem) {
            Texture2D itemTexture = shop->itemTextures[i];
            float floatY = sin(time * 3 + i) * 3;
            Rectangle drawRect = shop->items[i].rect;
            drawRect.y += floatY;

            DrawRectangleRec(drawRect, Fade(shop->items[i].color, 0.4f));
            DrawRectangleLinesEx(drawRect, 2, WHITE);

            if (itemTexture.id != 0) {
                float textureScale = drawRect.width / itemTexture.width;
                Vector2 pos = { drawRect.x, drawRect.y };
                DrawTextureEx(itemTexture, pos, 0.0f, textureScale, WHITE);
            }

            int nameWidth = MeasureText(shop->items[i].name, 10);
            DrawText(shop->items[i].name, (int)drawRect.x + (int)drawRect.width/2 - nameWidth/2, (int)drawRect.y - 25, 10, WHITE);

            char priceText[16];
            if (shop->items[i].price == 0) sprintf(priceText, "GRATIS");
            else sprintf(priceText, "$%d", shop->items[i].price);

            int priceWidth = MeasureText(priceText, 10);
            DrawText(priceText, (int)drawRect.x + (int)drawRect.width/2 - priceWidth/2, (int)drawRect.y + (int)drawRect.height + 5, 10, YELLOW);
        }
    }

    if (shop->showParticles) {
        float pW = player->texture.width * player->scale;
        float pH = player->texture.height * player->scale;
        Vector2 playerCenter = { player->position.x + pW/2, player->position.y + pH/2 };

        Color effectColor = LIME;

        float radius = (shop->particleTimer / PARTICLE_LIFETIME) * 60.0f;
        Color particleColor = Fade(effectColor, 1.0f - (shop->particleTimer / PARTICLE_LIFETIME));
        DrawCircleLines((int)playerCenter.x, (int)playerCenter.y, radius, particleColor);
    }

    DrawPlayer(player);

    DrawText(TextFormat("CREDITOS: $%d", player->gold), 10, 10, 20, GOLD);

    int boxH = TEXT_BOX_HEIGHT;
    DrawRectangle(0, 600 - boxH, 800, boxH, Fade(BLACK, 0.9f));
    DrawRectangleLines(0, 600 - boxH, 800, boxH, GREEN);

    DrawTextEx(GetFontDefault(), shop->dialogText, (Vector2){ 20, 600 - DIALOG_TEXT_Y_OFFSET }, DIALOG_FONT_SIZE, 1, GREEN);
}

void UnloadShop(ShopScene *shop) {
    for (int i = 0; i < MAX_SHOP_ITEMS; i++) {
        if (shop->itemTextures[i].id != 0) UnloadTexture(shop->itemTextures[i]);
    }
}