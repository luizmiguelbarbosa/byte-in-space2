#version 330

uniform sampler2D texture0; 
in vec2 fragTexCoord; 
out vec4 finalColor; 

uniform vec2 resolution;
uniform float time; 

// --- FUNÇÕES AUXILIARES ---

// Ajusta o contraste da cor
vec3 applyContrast(vec3 color, float contrast) {
    // Contraste ALTO mantido em 1.8
    return ((color - 0.5) * contrast + 0.5);
}

// Aumenta a saturação da cor
vec3 applySaturation(vec3 color, float saturation) {
    // Calculamos o valor de luminosidade (média ponderada dos canais RGB)
    float luma = dot(color, vec3(0.2126, 0.7152, 0.0722));
    // Interpolamos entre a cor original e a luminosidade (cinza)
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

    // 1. Curvatura CRT (Barrel Distortion) APRIMORADA
    vec2 center = uv - 0.5;
    float dist = dot(center, center); 
    
    // NOVO FATOR DE CURVATURA: Reduzido para 0.25 (antes era 0.35) 
    // Isso limita o quanto as bordas se movem, evitando o vazamento.
    uv += center * dist * 0.25; 

    // O FIX de clamp deve permanecer, mas agora ele tem menos trabalho a fazer
    uv = clamp(uv, 0.001, 0.999); 

    // 2. Amostragem da Cor
    vec3 color = texture(texture0, uv).rgb;

    // 3. Ajuste de Saturação para CORES FORTES
    // Saturação ALTA para dar vida ao arcade (1.5 é bem forte)
    color = applySaturation(color, 1.5);

    // 4. Ajuste de Contraste
    color = applyContrast(color, 1.8); 

    // 5. Scanlines (Linhas horizontais)
    float scan = sin(uv.y * resolution.y * 3.1415); 
    float scanFactor = 0.10; 
    scan = 1.0 - scanFactor * (scan * 0.5 + 0.5); 
    
    // 6. Aplicar Scanlines
    color *= scan;

    // 7. Efeito de Máscara de Fósforo (Shadow Mask)
    color *= shadowMask(uv);
    
    // 8. Vinheta
    // Deixando a vinheta mais suave para não escurecer tanto
    float vignette = 1.0 - (length(originalUV - 0.5) * 0.75); // Coeficiente reduzido para 0.75
    vignette = pow(vignette, 0.8); // Potência mais alta torna a transição mais suave
    color *= vignette;
    
    // 9. Ruído Analógico Sutil (Flicker)
    float flicker = sin(time * 15.0) * 0.01 + 0.01;
    color += flicker * vec3(0.005);
    
    finalColor = vec4(color, 1.0);
}