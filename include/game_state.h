#ifndef GAME_STATE_H
#define GAME_STATE_H

typedef enum {
    STATE_CUTSCENE = 0, // Estado inicial: Cutscene
    STATE_SHOP,         // Loja
    STATE_GAMEPLAY,     // Jogabilidade principal
    STATE_PAUSE,        // Jogo pausado
    STATE_MENU          // Menu Principal (se implementado)
} GameState;

#endif // GAME_STATE_H