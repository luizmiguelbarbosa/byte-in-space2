// crt.fs - Pixelização Pura (Máscara de Fósforo e Scanlines)
#version 330

in vec2 fragTexCoord;
in vec4 fragColor;
out vec4 finalColor;

uniform sampler2D textureSampler;
uniform float time;
uniform vec2 resolution; // 800x600

// --- PARÂMETROS DE AJUSTE ---
const float MASK_STRENGTH = 0.35;         // Intensidade da Máscara de Fósforo (MAX PIXELS)
const float SCANLINE_STRENGTH = 0.40;     // Linhas de Varredura Nítidas
const float BRIGHTNESS_BOOST = 1.3;       // Aumenta o brilho para compensar as perdas

// --- Efeito Pixel Art Retro ---
void main() {
    vec2 uv = fragTexCoord;
    vec4 color = texture(textureSampler, uv) * BRIGHTNESS_BOOST;

    // O jogo é amostrado diretamente (sem 'warp'), então a tela é PLANA.

    // 1. Linhas de Varredura (Scanlines)
    // Linhas horizontais escuras que dão o efeito de varredura vertical.
    float scanline = sin(uv.y * resolution.y * 3.14159 * 2.0);
    color.rgb *= 1.0 - SCANLINE_STRENGTH * (0.5 + 0.5 * scanline);

    // 2. Máscara de Fósforo (Shadow Mask - PIXELIZAÇÃO)
    // Cria o padrão de listras verticais RGB. É o que simula os pixels.
    vec3 mask = vec3(0.0);
    // Usa o resto da divisão por 3 para ciclar entre R, G, B a cada 3 pixels
    float x = mod(uv.x * resolution.x, 3.0);

    if (x < 1.0) mask = vec3(1.0, 0.0, 0.0); // Canal R (Vermelho)
    else if (x < 2.0) mask = vec3(0.0, 1.0, 0.0); // Canal G (Verde)
    else mask = vec3(0.0, 0.0, 1.0); // Canal B (Azul)

    // Subtrai a cor onde não deveria haver fósforo, escurecendo a grade.
    color.rgb -= (color.rgb * (1.0 - mask)) * MASK_STRENGTH;

    // 3. Ruído Analógico (Flicker/Estática Sutil - Opcional)
    // Se quiser adicionar um toque de vida analógica
    float noise_strength = 0.01;
    float flicker = fract(sin(dot(uv, vec2(12.9898, 78.233))) * 43758.5453);
    color.rgb -= vec3(flicker * noise_strength);

    finalColor = color;
}