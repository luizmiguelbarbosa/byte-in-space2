// crt.fs - Curvatura Forte e Pixels Máximos (SEM Vinhetas)
#version 330

in vec2 fragTexCoord;
in vec4 fragColor;
out vec4 finalColor;

uniform sampler2D textureSampler; 
uniform float time;
uniform vec2 resolution; // 800x600

// --- PARÂMETROS DE AJUSTE EXTREMO ---
const float CURVATURE_AMOUNT = 0.35;      // Curvatura EXTREMA ("barriga")
const float CHROMA_SHIFT_AMOUNT = 0.006;  // Aberração Cromática forte
const float SCANLINE_STRENGTH = 0.35;     // Linhas de Varredura Nítidas
const float MASK_STRENGTH = 0.30;         // Máscara de Fósforo MÁXIMA (pixels visíveis)
const float NOISE_STRENGTH = 0.01;        // Ruído/Flicker sutil
const float BRIGHTNESS_BOOST = 1.25;      // Compensa o escurecimento das scanlines/máscara

// Função de distorção de barril (Curvatura)
vec2 warp(vec2 uv) {
    vec2 pos = uv - 0.5;
    float r2 = pos.x * pos.x + pos.y * pos.y;
    // CURVATURE_AMOUNT alto empurra o centro da tela para frente (barriga)
    return uv + pos * r2 * CURVATURE_AMOUNT;
}

// Função de Ruído (Random)
float random(vec2 uv) {
    return fract(sin(dot(uv, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
    vec2 uv = fragTexCoord;
    vec4 color;

    // --- 1. Curvatura, Aberração Cromática e Amostragem ---
    vec2 uv_r = warp(uv + vec2(CHROMA_SHIFT_AMOUNT));
    vec2 uv_b = warp(uv - vec2(CHROMA_SHIFT_AMOUNT));
    vec2 uv_g = warp(uv);

    // Amostra as cores
    float r = texture(textureSampler, uv_r).r;
    float g = texture(textureSampler, uv_g).g;
    float b = texture(textureSampler, uv_b).b;
    
    color = vec4(r, g, b, 1.0) * BRIGHTNESS_BOOST; 

    // --- 2. Simulação de Bloom (Glow) ---
    float luminance = dot(color.rgb, vec3(0.299, 0.587, 0.114));
    float bloom = smoothstep(0.8, 1.0, luminance); 
    color.rgb += color.rgb * bloom * 0.15; 

    // --- 3. Linhas de Varredura (Scanlines) ---
    float scanline = sin(uv_g.y * resolution.y * 3.14159 * 2.0);
    color.rgb *= 1.0 - SCANLINE_STRENGTH * (0.5 + 0.5 * scanline);

    // --- 4. Máscara de Fósforo (Shadow Mask - MÁXIMA PIXELIZAÇÃO) ---
    vec3 mask = vec3(0.0);
    float x = mod(uv_g.x * resolution.x, 3.0); 

    if (x < 1.0) mask = vec3(1.0, 0.0, 0.0);
    else if (x < 2.0) mask = vec3(0.0, 1.0, 0.0);
    else mask = vec3(0.0, 0.0, 1.0);
    
    color.rgb -= (color.rgb * (1.0 - mask)) * MASK_STRENGTH;

    // --- 5. Ruído Analógico (Flicker) ---
    float flicker = random(uv_g * time);
    color.rgb -= vec3(flicker * NOISE_STRENGTH); 

    // -----------------------------------------------------------------
    // REMOVIDA A SEÇÃO DE VINHETAS (Escurecimento de bordas)
    // -----------------------------------------------------------------
    
    // --- 6. Remoção Suave de Artefatos de Borda ---
    // Usamos o "smoothstep" para garantir que a imagem não "corte" de forma
    // brusca onde ela passa da borda da tela curva, sem escurecer o centro.
    vec2 pos_uv = uv - 0.5;
    float len = length(pos_uv);
    // Cria um fade sutil e distante (começa a 0.8 e termina a 0.9 nas bordas)
    float edge_softness = smoothstep(0.8, 0.9, len); 
    color.rgb *= 1.0 - edge_softness; // Subtrai o "fade"

    // Multiplicamos pela cor original (quase 1.0) apenas se a amostragem estiver fora
    float border_cut = 1.0;
    if (uv_g.x < 0.0 || uv_g.x > 1.0 || uv_g.y < 0.0 || uv_g.y > 1.0) {
        // Se a amostra sair, use o fade suave que criamos acima.
        border_cut = 1.0 - edge_softness;
    }
    color.rgb *= border_cut;

    finalColor = color;
}