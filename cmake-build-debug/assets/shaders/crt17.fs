#version 330

// Textura da imagem de entrada (o que seria renderizado normalmente)
uniform sampler2D texture0; 
// Coordenadas de textura interpoladas (por pixel)
in vec2 fragTexCoord; 
// Cor final do pixel de saída
out vec4 finalColor; 

// Resolução da tela (usada para calcular scanlines e fosforescência)
uniform vec2 resolution;
// Valor de tempo (usado para animação sutil do ruído CRT)
uniform float time; 

// --- FUNÇÕES AUXILIARES ---

// Ajusta o contraste da cor
vec3 applyContrast(vec3 color, float contrast) {
    return ((color - 0.5) * contrast + 0.5);
}

// Simula o padrão de pontos/linhas dos fósforos CRT (Shadow Mask)
vec3 shadowMask(vec2 uv) {
    vec3 mask = vec3(1.0);
    
    // Multiplicamos pela resolução Y para obter a escala correta na tela
    // O valor 0.5 ajusta a densidade da máscara
    uv.x *= resolution.y * 0.5; 
    
    float x = mod(floor(uv.x), 3.0);
    
    // Fator de saturação/escurecimento para o efeito de máscara
    float s = 0.5; 
    
    // Padrão RGB (simula a grade de fósforo)
    if (x == 0.0) mask = vec3(1.0 + s, 1.0 - s, 1.0 - s); 
    else if (x == 1.0) mask = vec3(1.0 - s, 1.0 + s, 1.0 - s); 
    else if (x == 2.0) mask = vec3(1.0 - s, 1.0 - s, 1.0 + s); 

    return mask * 0.9; // Escurece levemente o efeito geral
}

// --- FUNÇÃO PRINCIPAL ---

void main() {
    vec2 uv = fragTexCoord;
    vec2 originalUV = uv; // Guarda as coordenadas originais para a vinheta

    // 1. Curvatura CRT (Barrel Distortion)
    vec2 center = uv - 0.5;
    float dist = dot(center, center); 
    uv += center * dist * 0.35; 

    // FIX PARA NÃO VAZAR A IMAGEM
    uv = clamp(uv, 0.001, 0.999); 

    // 2. Amostragem da Cor
    vec3 color = texture(texture0, uv).rgb;

    // 3. Ajuste de Contraste
    color = applyContrast(color, 1.25); 

    // 4. Scanlines (Linhas horizontais)
    float scan = sin(uv.y * resolution.y * 3.1415); 
    float scanFactor = 0.13; 
    scan = 1.0 - scanFactor * (scan * 0.5 + 0.5); 
    
    // 5. Aplicar Scanlines
    color *= scan;

    // 6. Efeito de Máscara de Fósforo (Shadow Mask)
    color *= shadowMask(uv);
    
    // 7. Vinheta (Escurece as bordas)
    // Distância do centro: length(originalUV - 0.5)
    float vignette = 1.0 - (length(originalUV - 0.5) * 1.0);
    vignette = pow(vignette, 0.5); 
    color *= vignette;
    
    // 8. Ruído Analógico Sutil (Flicker)
    float flicker = sin(time * 15.0) * 0.01 + 0.01;
    color += flicker * vec3(0.005);
    
    finalColor = vec4(color, 1.0);
}