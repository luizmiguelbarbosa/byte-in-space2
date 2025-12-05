#version 330

uniform sampler2D texture0; 
in vec2 fragTexCoord; 
out vec4 finalColor; 

uniform vec2 resolution;
uniform float time; 

// --- FUNÇÕES AUXILIARES ---

// Ajusta o contraste da cor
vec3 applyContrast(vec3 color, float contrast) {
    // Fator de Contraste AUMENTADO para 1.8 (antes era 1.25)
    return ((color - 0.5) * contrast + 0.5);
}

// Simula o padrão de pontos/linhas dos fósforos CRT (Shadow Mask)
vec3 shadowMask(vec2 uv) {
    vec3 mask = vec3(1.0);
    
    // Aumentamos a densidade da máscara (0.75 ao invés de 0.5)
    // Isso a torna mais "fina" e menos visível, reduzindo a sensação pixelada
    uv.x *= resolution.y * 0.75; 
    
    float x = mod(floor(uv.x), 3.0);
    
    // Fator de saturação/escurecimento (reduzido para 0.3 para ser mais sutil)
    float s = 0.3; 
    
    if (x == 0.0) mask = vec3(1.0 + s, 1.0 - s, 1.0 - s); 
    else if (x == 1.0) mask = vec3(1.0 - s, 1.0 + s, 1.0 - s); 
    else if (x == 2.0) mask = vec3(1.0 - s, 1.0 - s, 1.0 + s); 

    // Fator de multiplicação geral (0.95 ao invés de 0.9) - menos escuro
    return mask * 0.95; 
}

// --- FUNÇÃO PRINCIPAL ---

void main() {
    vec2 uv = fragTexCoord;
    vec2 originalUV = uv; 

    // 1. Curvatura CRT (Barrel Distortion)
    vec2 center = uv - 0.5;
    float dist = dot(center, center); 
    uv += center * dist * 0.35; 

    // FIX PARA NÃO VAZAR A IMAGEM
    uv = clamp(uv, 0.001, 0.999); 

    // 2. Amostragem da Cor
    vec3 color = texture(texture0, uv).rgb;

    // 3. Ajuste de Contraste AUMENTADO
    color = applyContrast(color, 1.8); 

    // 4. Scanlines (Linhas horizontais)
    float scan = sin(uv.y * resolution.y * 3.1415); 
    // Fator de Scanline REDUZIDO para 0.10 (antes era 0.13) - mais suave
    float scanFactor = 0.10; 
    scan = 1.0 - scanFactor * (scan * 0.5 + 0.5); 
    
    // 5. Aplicar Scanlines
    color *= scan;

    // 6. Efeito de Máscara de Fósforo (Shadow Mask) SUAVIZADO
    color *= shadowMask(uv);
    
    // 7. Vinheta
    float vignette = 1.0 - (length(originalUV - 0.5) * 1.0);
    vignette = pow(vignette, 0.5); 
    color *= vignette;
    
    // 8. Ruído Analógico Sutil (Flicker)
    float flicker = sin(time * 15.0) * 0.01 + 0.01;
    color += flicker * vec3(0.005);
    
    finalColor = vec4(color, 1.0);
}