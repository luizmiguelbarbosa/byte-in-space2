#version 330

in vec2 fragTexCoord;
out vec4 finalColor;

uniform sampler2D textureSampler;
uniform vec2 resolution;

// ------------------------------
// PARÂMETROS
// ------------------------------
const float BRIGHTNESS = 0.95;          // Menos claro
const float SCANLINE_STRENGTH = 0.18;   // Fino igual fliperama
const float MASK_STRENGTH = 0.20;
const float CURVE_AMOUNT = 0.12;        // "Barriga" da tela
const float EDGE_FADE = 0.35;           // Escurecimento suave das bordas
// ------------------------------

vec2 applyCurve(vec2 uv) {
    // Centraliza o UV na origem (-1 a +1)
    uv = uv * 2.0 - 1.0;

    // Distorção tipo "barriga" (pincushion invertido)
    uv.x *= 1.0 + (uv.y * uv.y) * CURVE_AMOUNT;
    uv.y *= 1.0 + (uv.x * uv.x) * CURVE_AMOUNT;

    // Retorna UV para 0..1
    uv = (uv + 1.0) * 0.5;
    return uv;
}

void main() {
    vec2 uv = fragTexCoord;
    vec2 curvedUV = applyCurve(uv);

    // Fora da tela → preto suave
    if (curvedUV.x < 0.0 || curvedUV.x > 1.0 ||
        curvedUV.y < 0.0 || curvedUV.y > 1.0) {
        finalColor = vec4(0.0, 0.0, 0.0, 1.0);
        return;
    }

    // Cor base
    vec3 color = texture(textureSampler, curvedUV).rgb;

    // ------------------------------
    // Scanline leve (arcade real)
    // ------------------------------
    float scan = sin(curvedUV.y * resolution.y * 3.14159);
    color *= 1.0 - SCANLINE_STRENGTH * (0.5 + 0.5 * scan);

    // ------------------------------
    // Máscara RGB suave
    // ------------------------------
    float px = curvedUV.x * resolution.x;
    vec3 mask;
    mask.r = 1.0 - MASK_STRENGTH * (mod(px, 3.0) != 0.0 ? 1.0 : 0.0);
    mask.g = 1.0 - MASK_STRENGTH * (mod(px, 3.0) != 1.0 ? 1.0 : 0.0);
    mask.b = 1.0 - MASK_STRENGTH * (mod(px, 3.0) != 2.0 ? 1.0 : 0.0);

    color *= mask;

    // ------------------------------
    // Bordas escurecidas (sem vinheta dura)
    // ------------------------------
    float dx = abs(uv.x - 0.5);
    float dy = abs(uv.y - 0.5);
    float dist = sqrt(dx*dx + dy*dy);

    float fade = 1.0 - smoothstep(0.45, 0.70, dist);
    fade = mix(1.0, fade, EDGE_FADE);

    color *= fade;

    // Ajuste final de brilho
    color *= BRIGHTNESS;

    finalColor = vec4(color, 1.0);
}
