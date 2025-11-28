#ifndef GAME_STATE_H
#define GAME_STATE_H

typedef enum GameState {
    STATE_TITLE = 0,
    STATE_GAMEPLAY,
    STATE_SHOP,
    STATE_GAME_OVER,
    STATE_CUTSCENE
} GameState;

#endif // GAME_STATE_H