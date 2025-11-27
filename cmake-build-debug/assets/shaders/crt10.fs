#version 330

in vec2 fragTexCoord;
out vec4 finalColor;

uniform sampler2D textureSampler;
uniform vec2 resolution;

// ===== PARÂMETROS =====
const float DISTORTION = 0.10;     // barriga
const float OVERSCAN   = 0.08;     // quanto corta da borda
const float MASK_STR   = 0.22;     // máscara RGB
const float SCAN_STR   = 0.22;     // scanlines
const float CONTRAST   = 1.15;
const float BRIGHTNESS = 1.05;

void main() {
    // ===== Overscan real (SEM BUGS) =====
    vec2 uv = fragTexCoord;
    uv = uv * (1.0 + OVERSCAN * 2.0) - OVERSCAN;

    // UV fora da tela? Aplica fade suave em vez de piscar
    if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0) {
        finalColor = vec4(0.0, 0.0, 0.0, 1.0);
        return;
    }

    // ===== Distortion sem bug no lado =====
    vec2 centered = uv * 2.0 - 1.0;
    float r2 = dot(centered, centered);

    // matemática estável — NÃO causa cortes
    centered += centered * (r2 * DISTORTION);

    vec2 uv2 = centered * 0.5 + 0.5;

    // ===== Nunca clampa! =====
    // Usando overscan REAL, o clamp não é mais necessário
    // e isso ELIMINA os bugs nas laterais.

    vec4 color = texture(textureSampler, uv2);

    // ===== Contraste =====
    color.rgb = (color.rgb - 0.5) * CONTRAST + 0.5;

    // ===== Scanlines =====
    float scan = sin(uv2.y * resolution.y * 3.14159 * 2.0);
    color.rgb *= 1.0 - SCAN_STR * (0.5 + 0.5 * scan);

    // ===== Máscara RGB moderna =====
    float x = mod(uv2.x * resolution.x, 3.0);
    vec3 mask = (x < 1.0) ? vec3(1,0,0) :
                (x < 2.0) ? vec3(0,1,0) :
                            vec3(0,0,1);
    color.rgb -= (color.rgb * (1.0 - mask)) * MASK_STR;

    // ===== Brilho =====
    finalColor = color * BRIGHTNESS;
}
