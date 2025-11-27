#version 330

uniform sampler2D texture0;
in vec2 fragTexCoord;
out vec4 finalColor;

uniform vec2 resolution;

vec3 applyContrast(vec3 color, float contrast) {
    return ((color - 0.5) * contrast + 0.5);
}

void main() {
    vec2 uv = fragTexCoord;

    // --- Curvatura CRT ("barriga") ---
    vec2 center = uv - 0.5;
    float dist = dot(center, center) * 0.35;
    uv += center * dist;

    // ---- FIX PARA NÃO VAZAR A IMAGEM ----
    uv = clamp(uv, 0.001, 0.999);

    // --- Scanlines ---
    float scan = sin(uv.y * resolution.y * 3.1415);
    float scanFactor = 0.13;
    scan = 1.0 - scanFactor * (scan * 0.5 + 0.5);

    // --- Amostragem ---
    vec3 color = texture(texture0, uv).rgb;

    // --- Contraste ---
    color = applyContrast(color, 1.25);

    // --- Aplicar scanlines ---
    color *= scan;

    finalColor = vec4(color, 1.0);
}
