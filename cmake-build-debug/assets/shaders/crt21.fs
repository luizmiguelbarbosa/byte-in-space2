#version 330

uniform sampler2D texture0; 
in vec2 fragTexCoord; 
out vec4 finalColor; 

uniform vec2 resolution;
uniform float time; 

// --- FUNÇÕES AUXILIARES ---

vec3 applyContrast(vec3 color, float contrast) {
    return ((color - 0.5) * contrast + 0.5);
}

vec3 applySaturation(vec3 color, float saturation) {
    float luma = dot(color, vec3(0.2126, 0.7152, 0.0722));
    return mix(vec3(luma), color, saturation);
}

vec3 shadowMask(vec2 uv) {
    vec3 mask = vec3(1.0);
    uv.x *= resolution.y * 0.75; 
    float x = mod(floor(uv.x), 3.0);
    float s = 0.3; 
    
    if (x == 0.0) mask = vec3(1.0 + s, 1.0 - s, 1.0 - s); 
    else if (x == 1.0) mask = vec3(1.0 - s, 1.0 + s, 1.0 - s); 
    else if (x == 2.0) mask = vec3(1.0 - s, 1.0 - s, 1.0 + s); 

    return mask * 0.95; 
}

// --- FUNÇÃO PRINCIPAL ---

void main() {
    vec2 uv = fragTexCoord;
    vec2 originalUV = uv; 

    // 1. Curvatura CRT (Barrel Distortion) - REMOVIDA
    // A coordenada UV agora é usada diretamente (uv = fragTexCoord)

    // 2. Verificação de Limites/Bugs - REMOVIDA
    
    // 3. Amostragem da Cor
    // A coordenada 'uv' é igual a 'fragTexCoord' (coordenada original da tela)
    vec3 color = texture(texture0, uv).rgb;

    // 4. Ajuste de Saturação (CORES FORTES)
    color = applySaturation(color, 1.5);

    // 5. Ajuste de Contraste (ALTO CONTRASTE)
    color = applyContrast(color, 1.8); 

    // 6. Scanlines (Linhas horizontais)
    float scan = sin(uv.y * resolution.y * 3.1415); 
    float scanFactor = 0.10; 
    scan = 1.0 - scanFactor * (scan * 0.5 + 0.5); 
    
    // 7. Aplicar Scanlines
    color *= scan;

    // 8. Efeito de Máscara de Fósforo (Shadow Mask)
    color *= shadowMask(uv);
    
    // 9. Vinheta (Mantida, mas suave)
    float vignette = 1.0 - (length(originalUV - 0.5) * 0.75);
    vignette = pow(vignette, 0.8);
    color *= vignette;
    
    // 10. Ruído Analógico Sutil (Flicker)
    float flicker = sin(time * 15.0) * 0.01 + 0.01;
    color += flicker * vec3(0.005);
    
    finalColor = vec4(color, 1.0);
}