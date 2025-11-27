#version 330

uniform sampler2D texture0;
in vec2 fragTexCoord;
out vec4 finalColor;

uniform vec2 resolution;

// Função para aumentar contraste
vec3 applyContrast(vec3 color, float contrast) {
    return ((color - 0.5) * contrast + 0.5);
}

void main() {
    vec2 uv = fragTexCoord;

    // --- Curvatura ("barriga da tela") ---
    vec2 center = uv - 0.5;
    float dist = dot(center, center) * 0.35; // força da barriga
    uv += center * dist;

    // --- Scanlines ---
    float scan = sin(uv.y * resolution.y * 3.1415);
    float scanFactor = 0.13; // escurecimento das linhas
    scan = 1.0 - scanFactor * (scan * 0.5 + 0.5);

    // --- Amostragem ---
    vec3 color = texture(texture0, uv).rgb;

    // --- Contraste (ajuste seu valor ideal entre 1.0 e 2.0) ---
    color = applyContrast(color, 1.25);

    // --- Vincular scanlines ---
    color *= scan;

    finalColor = vec4(color, 1.0);
}
