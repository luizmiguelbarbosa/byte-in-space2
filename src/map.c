#include "map.h"
#include "raylib.h"

void InitMap(GameMap* map) {

    map->texture = LoadTexture("C:/dev/byte_in_space2/assets/images/sprites/bg1_sheet.png");

    map->frameCount = 25;
    map->currentFrame = 0;

    // Usa altura correta (82 px) para evitar erro de arredondamento
    map->frameWidth  = map->texture.width;      // 145 px
    map->frameHeight = 82;                      // FIXADO

    map->frameTime = 0.0f;
    map->frameSpeed = 0.12f;   // velocidade da animação
}

void UpdateMap(GameMap* map, float dt) {

    map->frameTime += dt;

    if (map->frameTime >= map->frameSpeed) {
        map->frameTime = 0.0f;

        map->currentFrame++;
        if (map->currentFrame >= map->frameCount) {
            map->currentFrame = 0;
        }
    }
}

void DrawMap(GameMap* map) {

    Rectangle src = {
        0,
        map->currentFrame * map->frameHeight,
        map->frameWidth,
        map->frameHeight
    };

    Rectangle dest = {
        0, 0,
        800,
        600
    };

    DrawTexturePro(
        map->texture,
        src,
        dest,
        (Vector2){0, 0},
        0.0f,
        WHITE
    );
}

void UnloadGameMap(GameMap* map) {
    UnloadTexture(map->texture);
}
