#version 330

uniform sampler2D texture0;
in vec2 fragTexCoord;
out vec4 finalColor;

uniform vec2 resolution;
uniform float time; // Necessário para adicionar ruído/flicker (opcional)

vec3 applyContrast(vec3 color, float contrast) {
    return ((color - 0.5) * contrast + 0.5);
}

void main() {
    vec2 uv = fragTexCoord;
    vec2 warped_uv = uv;

    // --- 1. Curvatura CRT ("barriga") ---
    vec2 center = uv - 0.5;
    float dist = dot(center, center) * 0.35; 
    warped_uv += center * dist;

    // --- FIX CRÍTICO: NÃO DEIXAR A IMAGEM REPETIR/VAZAR ---
    // Se a coordenada curva sair da área [0, 1], paramos de desenhar.
    if (warped_uv.x < 0.0 || warped_uv.x > 1.0 || warped_uv.y < 0.0 || warped_uv.y > 1.0) {
        finalColor = vec4(0.0, 0.0, 0.0, 1.0);
        return; // Pula o restante do shader para este pixel (resolvendo o problema de repetição)
    }

    // --- 2. Amostragem ---
    // Usamos 'warped_uv' (a coordenada curva) para amostrar a textura.
    vec3 color = texture(texture0, warped_uv).rgb;

    // --- 3. Pixelização (Máscara de Fósforo) ---
    const float MASK_STRENGTH = 0.35; // Intensidade da pixelização
    vec3 mask = vec3(0.0);
    float x = mod(warped_uv.x * resolution.x, 3.0); 

    if (x < 1.0) mask = vec3(1.0, 0.0, 0.0);
    else if (x < 2.0) mask = vec3(0.0, 1.0, 0.0);
    else mask = vec3(0.0, 0.0, 1.0);
    
    color.rgb -= (color.rgb * (1.0 - mask)) * MASK_STRENGTH;

    // --- 4. Scanlines ---
    float scanFactor = 0.30;
    float scan = sin(warped_uv.y * resolution.y * 3.14159 * 2.0);
    float scanline_multiplier = 1.0 - scanFactor * (scan * 0.5 + 0.5);
    
    // --- 5. Contraste e Aplicação Final ---
    // Aplica Contraste (1.25)
    color = applyContrast(color, 1.25); 
    
    // Aplica Scanlines
    color *= scanline_multiplier;

    // --- 6. Vinheta SUAVE (Quase Invisível) ---
    // Adicionei uma vinheta muito sutil para dar o aspecto de tubo, mas sem escurecer muito.
    vec2 center_uv = fragTexCoord - 0.5;
    float len = length(center_uv);
    float vignette = smoothstep(0.5, 0.45, len); 
    color.rgb *= vignette;


    finalColor = vec4(color, 1.0);
}