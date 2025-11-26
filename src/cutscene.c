#include "cutscene.h"
#include "raylib.h"
#include "raymath.h"
#include "game_state.h"
#include <string.h>
#include <stdio.h>

#define TITLE_FONT_SIZE 70 // Tamanho ajustado para caber na tela
#define INSTRUCTION_FONT_SIZE 30

// Cores base para o efeito Neon
#define NEON_COLOR_BASE (Color){ 255, 0, 255, 255 }     // Cor principal: Magenta/Roxo

// Cor da aura do título principal: Azul Ciano
#define TITLE_GLOW_COLOR (Color){ 0, 150, 255, 255 }

// Constantes para o ciclo de explosão do planeta
#define PLANET_CYCLE_DURATION 15.0f // O ciclo completo dura 15 segundos
#define EXPLOSION_START_TIME 10.0f  // Começa a explodir em 10s
#define EXPLOSION_END_TIME 12.0f    // Fim da explosão (só brilho) em 12s

void InitCutscene(CutsceneScene *cs) {
    // A cutscene agora tem apenas duas "páginas": o título e a instrução.
    cs->pages[0]  = (CutscenePage){ "BYTE IN SPACE 2", 0.0f }; // Título principal
    cs->pages[1]  = (CutscenePage){ "Pressione ENTER para comecar", 0.0f }; // Instrução

    cs->currentPage = 0;
    cs->currentTimer = 0.0f;
    cs->textTimer = 0.0f;
    cs->visibleChars = 0;
    cs->isFadingOut = false;
    cs->titleAlpha = 0.0f;
    cs->showTitle = false;
}

void UpdateCutscene(CutsceneScene *cs, GameState *state, float deltaTime) {
    // A única lógica é avançar a instrução
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        *state = STATE_SHOP;
        return;
    }

    // Se estiver no título, avance para a instrução após um tempo de introdução
    if (cs->currentPage == 0) {
        cs->currentTimer += deltaTime;
        // Tempo para o título estabilizar antes de mostrar a instrução
        if (cs->currentTimer >= 3.0f) {
            cs->currentPage = 1;
        }
    }
}

// Função auxiliar para desenhar o fundo com efeito parallax (estrelas, planeta e meteoros)
static void DrawParallaxBackground(int screenWidth, int screenHeight, float time) {

    // Calcula o ponto do ciclo do planeta (0.0 a PLANET_CYCLE_DURATION)
    float cycleTime = fmodf(time, PLANET_CYCLE_DURATION);

    // --- Estrelas: 3 camadas de movimento e cor diferentes (Mais estrelas) ---

    // Camada 1: Lentas e pequenas (200 estrelas, tons quentes/âmbar)
    for (int i = 0; i < 200; i++) {
        int x = (i * 73) % screenWidth;
        int y = (i * 59) % screenHeight;
        float movementX = sin(time * 0.05f) * 10.0f;
        float movementY = cos(time * 0.03f) * 5.0f;

        int finalX = (x + (int)movementX) % screenWidth;
        int finalY = (y + (int)movementY) % screenHeight;
        if (finalX < 0) finalX += screenWidth;
        if (finalY < 0) finalY += screenHeight;

        DrawCircle(finalX, finalY, 1, (Color){ 150, 100, 100, 100 }); // Vermelho escuro/âmbar
    }

    // Camada 2: Médias e mais rápidas (100 estrelas, tons frios)
    for (int i = 0; i < 100; i++) {
        int x = (i * 97) % screenWidth;
        int y = (i * 83) % screenHeight;

        float movementX = cos(time * 0.15f) * 20.0f;
        float movementY = sin(time * 0.10f) * 15.0f;

        int finalX = (x + (int)movementX) % screenWidth;
        int finalY = (y + (int)movementY) % screenHeight;
        if (finalX < 0) finalX += screenWidth;
        if (finalY < 0) finalY += screenHeight;

        DrawCircle(finalX, finalY, (i % 2) + 1, (Color){ 150, 150, 200, 150 });
    }

    // Camada 3: Mais próximas e rápidas (50 estrelas, tons brancos)
    for (int i = 0; i < 50; i++) {
        int x = (i * 121) % screenWidth;
        int y = (i * 107) % screenHeight;

        float movementX = sin(time * 0.25f) * 30.0f;
        float movementY = cos(time * 0.20f) * 25.0f;

        int finalX = (x + (int)movementX) % screenWidth;
        int finalY = (y + (int)movementY) % screenHeight;
        if (finalX < 0) finalX += screenWidth;
        if (finalY < 0) finalY += screenHeight;

        DrawCircle(finalX, finalY, 1, (Color){ 255, 255, 255, 200 });
    }

    // --- O PLANETA (Ciclo de Explodir/Reformar) ---
    Vector2 planetPos = { screenWidth * 0.7f, screenHeight * 0.3f };
    float planetBaseSize = 80.0f;
    Color planetBaseColor = { 50, 50, 100, 255 };

    float currentPlanetSize = planetBaseSize;
    float atmosphereAlpha = 1.0f;

    // ESTADO 1: Explosão (10.0s a 12.0s)
    if (cycleTime >= EXPLOSION_START_TIME && cycleTime < EXPLOSION_END_TIME) {
        float explosionPhase = (cycleTime - EXPLOSION_START_TIME) / (EXPLOSION_END_TIME - EXPLOSION_START_TIME); // 0.0 a 1.0

        // Simula o crescimento da explosão (onda de choque)
        float blastRadius = planetBaseSize * (1.0f + explosionPhase * 2.0f);
        DrawCircleV(planetPos, blastRadius, Fade(RED, 1.0f - explosionPhase));

        // Simula o desaparecimento do planeta
        currentPlanetSize = planetBaseSize * (1.0f - explosionPhase);
        atmosphereAlpha = 1.0f - explosionPhase;

    }
    // ESTADO 2: Reconstrução/Aura Pós-Explosão (12.0s a 15.0s)
    else if (cycleTime >= EXPLOSION_END_TIME) {
        float reformPhase = (cycleTime - EXPLOSION_END_TIME) / (PLANET_CYCLE_DURATION - EXPLOSION_END_TIME); // 0.0 a 1.0

        // Planeta retorna ao tamanho normal
        currentPlanetSize = planetBaseSize * reformPhase;

        // Pulsação de energia durante a reforma
        float pulse = (sin(time * 8.0f) + 1.0f) / 2.0f;
        float glowSize = planetBaseSize * 1.5f;
        DrawCircleV(planetPos, glowSize, Fade(TITLE_GLOW_COLOR, 0.4f * pulse * (1.0f - reformPhase)));

        atmosphereAlpha = reformPhase;

    }
    // ESTADO 3: Normal (0.0s a 10.0s)

    // Desenha o Planeta (se o tamanho for maior que zero)
    if (currentPlanetSize > 1.0f) {
        Color atmosphereColor = { 100, 100, 150, (unsigned char)(120 * atmosphereAlpha) };

        // Desenha atmosfera/aura
        DrawCircleV(planetPos, currentPlanetSize * 1.3f, Fade(atmosphereColor, 0.5f));
        DrawCircleV(planetPos, currentPlanetSize * 1.1f, Fade(atmosphereColor, 0.8f * atmosphereAlpha));
        // Desenha planeta
        DrawCircleV(planetPos, currentPlanetSize, Fade(planetBaseColor, atmosphereAlpha));
    }


    // --- Meteoros/Debris (Cores Variadas, direção consistente: Top-Right para Bottom-Left) ---
    Color colors[] = { RED, GREEN, YELLOW, TITLE_GLOW_COLOR, NEON_COLOR_BASE, ORANGE, LIME, VIOLET };
    int numDebris = 10;

    for (int i = 0; i < numDebris; i++) {

        // Velocidade e posição base para movimento diagonal consistente
        float speed = 250.0f + (i * 20.0f);

        // Cria a posição inicial e move o rastro de forma contínua
        float totalMovement = fmodf(time * speed, (float)screenWidth * 1.5f);

        // Posição inicial (i*150 para espaçamento vertical)
        float startOffset = (float)(i * 150);

        // xPos e yPos se movem de forma consistente
        float xPos = (float)screenWidth + 100.0f - totalMovement + startOffset * 0.2f;
        float yPos = 0.0f - 50.0f + totalMovement * 0.7f + startOffset * 0.4f;

        // Wrap-around: quando o meteoro sai pelo canto inferior/esquerdo, ele volta pelo superior/direito
        if (xPos < -100.0f || yPos > (float)screenHeight + 100.0f) {
            // Reinicia o meteoro
            xPos += (float)screenWidth * 2.0f + 200.0f;
            yPos -= (float)screenHeight * 1.5f;
        }

        Vector2 currentPos = {xPos, yPos};
        Color debrisColor = colors[i % 8];

        // Desenha Rastro (linha)
        Vector2 trailEnd = { xPos + 20, yPos - 15 };
        DrawLineEx(currentPos, trailEnd, 3.0f, Fade(debrisColor, 0.7f));
        // Desenha Núcleo
        DrawCircleV(currentPos, 3, Fade(debrisColor, 1.0f));
    }
}


// Função auxiliar para desenhar o texto com efeito neon
static void DrawNeonText(const char *text, int posX, int posY, int fontSize, float pulseSpeed, Color glowAura) {
    // Calcula pulsação do brilho (Alpha)
    float pulse = 1.0f;
    if (pulseSpeed > 0) {
        // Pulsação mais lenta (velocidade de 1.0f ou 2.0f)
        pulse = (sin(GetTime() * pulseSpeed) + 1.0f) / 2.0f; // Varia entre 0.0 e 1.0
    }

    // Cor Base do texto (opacidade 100%) - Cor principal (Magenta)
    Color baseColor = NEON_COLOR_BASE;

    // Cor do Brilho (a aura que pulsa)
    Color glowColor = glowAura;
    glowColor.a = (unsigned char)(glowAura.a * (0.3f + pulse * 0.5f)); // Alpha varia entre 30% e 80%

    // 1. Camada de Brilho Fundo (simulando desfoque, desenhando 8 vezes em volta)
    DrawText(text, posX - 4, posY, fontSize, glowColor);
    DrawText(text, posX + 4, posY, fontSize, glowColor);
    DrawText(text, posX, posY - 4, fontSize, glowColor);
    DrawText(text, posX, posY + 4, fontSize, glowColor);

    DrawText(text, posX - 2, posY - 2, fontSize, glowColor);
    DrawText(text, posX + 2, posY + 2, fontSize, glowColor);
    DrawText(text, posX - 2, posY + 2, fontSize, glowColor);
    DrawText(text, posX + 2, posY - 2, fontSize, glowColor);

    // 2. Camada Central (cor mais sólida)
    DrawText(text, posX, posY, fontSize, baseColor);
}

void DrawCutscene(CutsceneScene *cs, int screenWidth, int screenHeight) {

    // 1. Fundo Espacial (Preto + Efeitos)
    DrawRectangle(0, 0, screenWidth, screenHeight, BLACK);
    DrawParallaxBackground(screenWidth, screenHeight, GetTime());

    const char *titleText = cs->pages[0].text;
    const char *instructionText = cs->pages[1].text;

    // --- DESENHO DO TÍTULO PRINCIPAL (Página 0) ---
    int titleWidth = MeasureText(titleText, TITLE_FONT_SIZE);
    int titlePosX = screenWidth / 2 - titleWidth / 2;
    int titlePosY = screenHeight / 2 - TITLE_FONT_SIZE / 2 - 50;

    // Pulsação MUITO LENTA (2.0f) com aura AZUL CIANO
    DrawNeonText(titleText, titlePosX, titlePosY, TITLE_FONT_SIZE, 2.0f, TITLE_GLOW_COLOR);


    // --- DESENHO DA INSTRUÇÃO (Página 1, exibida após 3 segundos) ---
    if (cs->currentPage == 1) {
        int instructionWidth = MeasureText(instructionText, INSTRUCTION_FONT_SIZE);
        int instructionPosX = screenWidth / 2 - instructionWidth / 2;
        int instructionPosY = screenHeight / 2 + 50;

        // Pulsação Média (4.0f) com aura NEON_COLOR_BASE (Magenta)
        DrawNeonText(instructionText, instructionPosX, instructionPosY, INSTRUCTION_FONT_SIZE, 4.0f, NEON_COLOR_BASE);
    }
}