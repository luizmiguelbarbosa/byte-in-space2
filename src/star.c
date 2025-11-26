#include "star.h"
#include "raylib.h"
#include "raymath.h"
#include <stdlib.h>

void InitStarField(StarField *field, int count, int screenWidth, int screenHeight) {
    field->count = count;
    field->screenWidth = screenWidth;
    field->screenHeight = screenHeight;
    field->stars = (Star *)MemAlloc(sizeof(Star) * count);
    for (int i = 0; i < count; i++) {
        field->stars[i].position = (Vector2){ (float)GetRandomValue(0, screenWidth), (float)GetRandomValue(0, screenHeight) };
        field->stars[i].color = WHITE;
        field->stars[i].size = (float)GetRandomValue(MIN_STAR_SIZE * 10, MAX_STAR_SIZE * 10) / 10.0f;
        field->stars[i].blinkTimer = (float)GetRandomValue(0, 100) / 100.0f;
        field->stars[i].blinkDuration = (float)GetRandomValue(50, 200) / 100.0f;
        field->stars[i].currentAlpha = 1.0f;
        field->stars[i].isFadingIn = true;
    }
}

void UpdateStarField(StarField *field, float deltaTime) {
    for (int i = 0; i < field->count; i++) {
        field->stars[i].position.y += STAR_FIELD_SPEED * deltaTime;
        if (field->stars[i].position.y > field->screenHeight) {
            field->stars[i].position = (Vector2){ (float)GetRandomValue(0, field->screenWidth), 0.0f };
        }

        field->stars[i].blinkTimer += deltaTime;
        if (field->stars[i].blinkTimer >= field->stars[i].blinkDuration) {
            field->stars[i].blinkTimer = 0.0f;
            field->stars[i].isFadingIn = !field->stars[i].isFadingIn;
        }

        if (field->stars[i].isFadingIn) {
            field->stars[i].currentAlpha = Clamp(field->stars[i].currentAlpha + deltaTime * 2.0f, 0.0f, 1.0f);
        } else {
            field->stars[i].currentAlpha = Clamp(field->stars[i].currentAlpha - deltaTime * 2.0f, 0.0f, 1.0f);
        }
    }
}

void DrawStarField(StarField *field) {
    for (int i = 0; i < field->count; i++) {
        Color starColor = Fade(field->stars[i].color, field->stars[i].currentAlpha);
        DrawCircleV(field->stars[i].position, field->stars[i].size, starColor);
    }
}

void UnloadStarField(StarField *field) {
    MemFree(field->stars);
    field->stars = NULL;
    field->count = 0;
}