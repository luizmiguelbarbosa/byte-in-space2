#ifndef STAR_H
#define STAR_H

#include "raylib.h"
#include <stdbool.h>

// Velocidade base de rolagem para simular movimento (Paralaxe)
#define STAR_FIELD_SPEED 50.0f
#define MIN_STAR_SIZE 1.0f
#define MAX_STAR_SIZE 3.0f

// Definição da estrela individual
typedef struct {
    Vector2 position;
    Color color;
    float size;
    float blinkTimer;
    float blinkDuration;
    float currentAlpha;
    bool isFadingIn;
} Star;

// Definição do Gerenciador de Estrelas
typedef struct {
    Star *stars;
    int count;
    int screenWidth;
    int screenHeight;
} StarField;

void InitStarField(StarField *field, int count, int screenWidth, int screenHeight);
void UpdateStarField(StarField *field, float deltaTime);
void DrawStarField(StarField *field);
void UnloadStarField(StarField *field);

#endif // STAR_H