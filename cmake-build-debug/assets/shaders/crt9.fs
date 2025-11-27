#version 330

in vec2 fragTexCoord;
out vec4 finalColor;

uniform sampler2D textureSampler;
uniform vec2 resolution;

// ===== PARÂMETROS DO EFEITO =====
const float DISTORTION = 0.12;      // Força da barriga
const float MASK_STRENGTH = 0.25;   // Mascara RGB
const float SCANLINE_STR = 0.25;    // Scanlines
const float CONTRAST = 1.20;        // Contraste extra
const float BRIGHTNESS = 1.05;      // Brilho final

void main() {
    // ===== EXPANSÃO DE UV PARA EVITAR BORDAS PRETAS =====
    vec2 uv = fragTexCoord * 0.98 + 0.01;

    // ===== REMOVER SALTOS — CENTRALIZA E NORMALIZA UV =====
    vec2 centered = uv * 2.0 - 1.0;  // [-1,1]
    float r2 = dot(centered, centered);

    // ===== DISTORÇÃO ESTÁVEL (SEM PISCAR) =====
    vec2 distorted = centered + centered * r2 * DISTORTION;

    // Volta para [0,1]
    vec2 uv2 = distorted * 0.5 + 0.5;

    // ===== SOFT CLAMP — impede linhas pretas nas bordas =====
    uv2 = clamp(uv2, 0.02, 0.98);

    // ===== AMOSTRA A COR =====
    vec4 color = texture(textureSampler, uv2);

    // ===== CONTRASTE =====
    color.rgb = (color.rgb - 0.5) * CONTRAST + 0.5;

    // ===== SCANLINES =====
    float scan = sin(uv2.y * resolution.y * 3.14159 * 2.0);
    color.rgb *= 1.0 - SCANLINE_STR * (0.5 + 0.5 * scan);

    // ===== MÁSCARA RGB =====
    float x = mod(uv2.x * resolution.x, 3.0);
    vec3 mask = (x < 1.0) ? vec3(1,0,0) : (x < 2.0 ? vec3(0,1,0) : vec3(0,0,1));
    color.rgb -= (color.rgb * (1.0 - mask)) * MASK_STRENGTH;

    // ===== BRILHO FINAL =====
    finalColor = color * BRIGHTNESS;
}
