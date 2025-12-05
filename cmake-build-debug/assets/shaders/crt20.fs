#version 330

uniform sampler2D texture0; 
in vec2 fragTexCoord; 
out vec4 finalColor; 

uniform vec2 resolution;
uniform float time; 

// --- FUNÇÕES AUXILIARES ---

// Ajusta o contraste da cor
vec3 applyContrast(vec3 color, float contrast) {
    return ((color - 0.5) * contrast + 0.5);
}

// Aumenta a saturação da cor
vec3 applySaturation(vec3 color, float saturation) {
    float luma = dot(color, vec3(0.2126, 0.7152, 0.0722));
    return mix(vec3(luma), color, saturation);
}

// Simula o padrão de pontos/linhas dos fósforos CRT (Shadow Mask)
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

    // 1. Curvatura CRT (Barrel Distortion) - NOVO MÉTODO
    
    // Distância do centro do UV, remapeada para [-0.5, 0.5]
    vec2 center = uv - 0.5;
    
    // Distância radial do centro
    float distSq = dot(center, center); 
    
    // Coeficiente de distorção (ajuste de 0.25 para 0.20 para ser mais seguro)
    float distortion = 0.20; 
    
    // Fator de escala/distorção: Quanto mais longe do centro, mais o fator afeta.
    // O valor 1.0 é o ponto de curvatura máxima, garantindo que o centro da imagem se curve.
    float factor = 1.0 + distSq * distortion;

    // Aplicamos o fator para curvar a imagem internamente (distorção reversa)
    uv = center * factor + 0.5; 

    // O truque para não ter bugs é fazer o CLAMP antes de amostrarmos a textura.
    // Se a coordenada UV após a distorção cair fora da faixa [0, 1], o pixel deve ser preto 
    // ou simplesmente não amostrar, mas o CLAMP garante que ele seja seguro.
    // IMPORTANTE: Devido ao novo fator, o clamp é menos crítico, mas ainda é uma boa prática.
    // REMOVEMOS O CLAMP SEPARADO: o próximo passo fará o mesmo de forma mais elegante.
    
    // 2. CORREÇÃO DE BORDAS: Se a coordenada UV final estiver fora de [0, 1], 
    // usamos uma cor preta (ou background) em vez de amostrar a textura.
    if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0) {
        finalColor = vec4(0.0, 0.0, 0.0, 1.0);
        return; 
    }
    
    // 3. Amostragem da Cor
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
    
    // 9. Vinheta
    float vignette = 1.0 - (length(originalUV - 0.5) * 0.75);
    vignette = pow(vignette, 0.8);
    color *= vignette;
    
    // 10. Ruído Analógico Sutil (Flicker)
    float flicker = sin(time * 15.0) * 0.01 + 0.01;
    color += flicker * vec3(0.005);
    
    finalColor = vec4(color, 1.0);
}