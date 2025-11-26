#ifndef SHOP_H
#define SHOP_H

#include "raylib.h"
#include "player.h"
#include "star.h"
#include "game_state.h"
#include <stdbool.h> // Incluído para garantir que bool seja reconhecido em todos os compiladores

#define MAX_ITEMS 3 // Definindo um limite máximo para o array de itens

// --- ESTRUTURA DO VENDEDOR ---
typedef struct {
    Texture2D texture;
    Vector2 position;
    Rectangle frameRec;
    int currentFrame;
    float frameTimer;
    bool isHappy;
    float happyTimer;
    float scale;       // nova: escala do sprite do vendedor
    int frameCountX;   // colunas do sprite sheet
    int frameCountY;   // linhas do sprite sheet (idle, happy, etc)
} Vendor;

// --- ESTRUTURA DO ITEM À VENDA ---
typedef struct {
    Rectangle rect;
    const char* name;
    int price;            // Preço do item
    Color color;
    bool active;
} ShopItem;

// --- GERENCIADOR DA CENA DA LOJA ---
typedef struct {
    Vendor vendor;
    ShopItem items[MAX_ITEMS];
    char dialogText[256]; // Aumentei o tamanho para 256 para o texto de diálogo mais longo
    bool itemBought;

    // CORREÇÃO: Membro 'itemFocused' adicionado para resolver o erro de compilação
    bool itemFocused;

} ShopScene;

void InitShop(ShopScene *shop, int gameWidth, int gameHeight);
void UpdateShop(ShopScene *shop, Player *player, StarField *stars, GameState *state, float deltaTime);
void DrawShop(ShopScene *shop, Player *player, StarField *stars);
void UnloadShop(ShopScene *shop);

#endif // SHOP_H