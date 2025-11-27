#version 330

in vec2 fragTexCoord;
out vec4 finalColor;

uniform sampler2D textureSampler;
uniform vec2 resolution;

// ------------------------------
// PARÂMETROS DO FLIPERAMA
// ------------------------------
const float SCANLINE_STRENGTH = 0.22;     // Intensidade das scanlines
const float MASK_STRENGTH     = 0.25;     // Máscara RGB estilo arcade
const float PHOSPHOR_GLOW     = 0.20;     // Glow suave (phosphor bleed)
const float BLOOM_STRENGTH    = 0.10;     // Estouro leve igual arcade
const float ABERRATION_AMOUNT = 0.0015;   // Chromatic aberration fraco
const float SHARPNESS         = 0.75;     // Nitidez geral
const float BRIGHTNESS        = 1.10;     // Brilho do fliperama
// ------------------------------

void main() {
    vec2 uv = fragTexCoord;

    // Aberração cromática mínima (arcade high-end)
    float ca = ABERRATION_AMOUNT;
    vec3 color;
    color.r = texture(textureSampler, uv + vec2(-ca, 0.0)).r;
    color.g = texture(textureSampler, uv).g;
    color.b = texture(textureSampler, uv + vec2(ca, 0.0)).b;

    // Blur horizontal leve (CRT “tubo” horizontal)
    float blur = 1.0 / resolution.x;
    vec3 horizBlur =
          texture(textureSampler, uv + vec2(-blur, 0)).rgb * 0.25
        + texture(textureSampler, uv).rgb                 * 0.50
        + texture(textureSampler, uv + vec2( blur, 0)).rgb * 0.25;
    color = mix(color, horizBlur, 0.25);

    // Scanlines (arcade real é mais suave que TV)
    float scanline = sin(uv.y * resolution.y * 3.14159);
    float scanFactor = 1.0 - (SCANLINE_STRENGTH * (0.5 + 0.5 * scanline));
    color *= scanFactor;

    // RGB mask — fliperama (não pixel gigante)
    float mx = uv.x * resolution.x;
    vec3 mask;
    mask.r = 0.9 + 0.1 * sin(mx * 1.0);
    mask.g = 0.9 + 0.1 * sin(mx * 1.0 + 2.09);
    mask.b = 0.9 + 0.1 * sin(mx * 1.0 + 4.18);
    color = mix(color, color * mask, MASK_STRENGTH);

    // Phosphor glow (brilho difuso de fósforo)
    vec3 glow =
          texture(textureSampler, uv + vec2(0, -blur*2)).rgb * 0.15 +
          texture(textureSampler, uv).rgb * 0.70 +
          texture(textureSampler, uv + vec2(0, blur*2)).rgb * 0.15;

    color = mix(color, glow, PHOSPHOR_GLOW);

    // Bloom leve — explosão arcade
    color += color * BLOOM_STRENGTH;

    // Nitidez final (menos digital)
    color = mix(color, pow(color, vec3(SHARPNESS)), 0.5);

    color *= BRIGHTNESS;

    finalColor = vec4(color, 1.0);
}
