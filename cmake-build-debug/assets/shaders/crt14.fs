#version 330

uniform sampler2D texture0;
in vec2 fragTexCoord;
out vec4 finalColor;

uniform vec2 resolution;

vec2 mirrorClamp(vec2 uv) {
    uv = abs(uv);
    uv = mod(uv, 2.0);
    uv = 1.0 - abs(uv - 1.0);
    return uv;
}

vec3 applyContrast(vec3 color, float contrast) {
    return ((color - 0.5) * contrast + 0.5);
}

void main() {
    vec2 uv = fragTexCoord;

    // ===== OVERSCAN =====
    const float overscan = 0.07;
    uv = uv * (1.0 + overscan * 2.0) - overscan;

    // ===== Mirror clamp (elimina completamente vazamento) =====
    uv = mirrorClamp(uv);

    // ===== Curvatura =====
    vec2 center = uv - 0.5;
    float dist = dot(center, center) * 0.35;
    uv += center * dist;

    // ===== Mirror clamp novamente após a distorção =====
    uv = mirrorClamp(uv);

    // ===== Scanlines =====
    float scan = sin(uv.y * resolution.y * 3.1415);
    float scanFactor = 0.13;
    scan = 1.0 - scanFactor * (scan * 0.5 + 0.5);

    // ===== Cor =====
    vec3 color = texture(texture0, uv).rgb;

    // ===== Contraste =====
    color = applyContrast(color, 1.28);

    // ===== Scanlines =====
    color *= scan;

    finalColor = vec4(color, 1.0);
}
