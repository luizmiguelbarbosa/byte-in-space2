#ifndef SHOP_H
#define SHOP_H

#include "raylib.h"
#include "player.h"
#include "star.h"
#include "game_state.h"

// Definições de itens
#define MAX_SHOP_ITEMS 4

typedef enum {
    ITEM_ENERGY_CHARGE = 0,
    ITEM_SHURIKEN,
    ITEM_SHIELD,
    ITEM_EXTRA_LIFE
} ItemType;

typedef struct {
    Rectangle rect;
    const char *name;
    int price;
    Color color;
    bool active;
    ItemType type;
} ShopItem;

typedef struct {
    Texture2D texture;
    Rectangle frameRec;
    Vector2 position;
    float scale;
    int currentFrame;
    float frameTimer;
    bool isHappy;
    float happyTimer;
    int frameCountX;
    int frameCountY;
} Vendor;


typedef struct {
    Vendor vendor;
    ShopItem items[MAX_SHOP_ITEMS];
    Texture2D itemTextures[MAX_SHOP_ITEMS];

    Rectangle exitArea;

    float portalParallaxOffset;

    char dialogText[256];
    bool itemBought;
    float particleTimer;
    bool showParticles;
    bool itemFocused;
} ShopScene;

// AJUSTE: Adicionando Player* à assinatura
void InitShop(ShopScene *shop, Player *player, int gameWidth, int gameHeight);
void UpdateShop(ShopScene *shop, Player *player, StarField *stars, GameState *state, float deltaTime);
void DrawShop(ShopScene *shop, Player *player, StarField *stars);
void UnloadShop(ShopScene *shop);

#endif // SHOP_H