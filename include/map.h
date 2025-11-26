#ifndef MAP_H
#define MAP_H

#include "raylib.h"

typedef struct GameMap {
    Texture2D texture;
    int frameCount;
    int currentFrame;
    float frameTime;
    float frameSpeed;
    int frameWidth;
    int frameHeight;
} GameMap;

void InitMap(GameMap* map);
void UpdateMap(GameMap* map, float dt);
void DrawMap(GameMap* map);
void UnloadGameMap(GameMap* map);

#endif
