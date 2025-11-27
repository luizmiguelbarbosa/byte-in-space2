#version 330

in vec2 fragTexCoord;
out vec4 finalColor;

uniform sampler2D textureSampler;
uniform vec2 resolution;

// ===== PARÂMETROS =====
const float DISTORTION = 0.10;        // barriga
const float OVERSCAN_X = 0.08;        // overscan horizontal
const float OVERSCAN_Y = 0.12;        // overscan vertical (AUMENTADO)
const float MASK_STR   = 0.22;        
const float SCAN_STR   = 0.22;        
const float CONTRAST   = 1.22;        // CONTRASTE AUMENTADO
const float BRIGHTNESS = 1.08;        // um pouco mais de brilho

void main() {
    // ===== Overscan real sem bug =====
    vec2 uv = fragTexCoord;

    // overscan independente por eixo
    uv.x = uv.x * (1.0 + OVERSCAN_X * 2.0) - OVERSCAN_X;
    uv.y = uv.y * (1.0 + OVERSCAN_Y * 2.0) - OVERSCAN_Y;

    // fora da área segura → preto estável, sem piscar
    if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0) {
        finalColor = vec4(0,0,0,1);
        return;
    }

    // ===== Distortion sem bug =====
    vec2 centered = uv * 2.0 - 1.0;
    float r2 = dot(centered, centered);
    centered += centered * (r2 * DISTORTION);
    vec2 uv2 = centered * 0.5 + 0.5;

    // ===== Amostragem =====
    vec4 color = texture(textureSampler, uv2);

    // ===== Contraste aumentado =====
    color.rgb = (color.rgb - 0.5) * CONTRAST + 0.5;

    // ===== Scanlines =====
    float scan = sin(uv2.y * resolution.y * 3.14159 * 2.0);
    color.rgb *= 1.0 - SCAN_STR * (0.5 + 0.5 * scan);

    // ===== Máscara RGB =====
    float x = mod(uv2.x * resolution.x, 3.0);
    vec3 mask = (x < 1.0) ? vec3(1,0,0) :
                (x < 2.0) ? vec3(0,1,0) :
                            vec3(0,0,1);

    color.rgb -= (color.rgb * (1.0 - mask)) * MASK_STR;

    // ===== Brilho =====
    finalColor = color * BRIGHTNESS;
}
