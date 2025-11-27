#version 330

uniform sampler2D texture0;
uniform vec2 resolution;
uniform float time;

in vec2 fragTexCoord;
out vec4 finalColor;

// Função utilitária: clamp seguro
vec2 clampUV(vec2 uv) {
    return clamp(uv, vec2(0.0), vec2(1.0));
}

void main() {
    //-------------------------------------------------------
    // 1) Pixelização fiel (pixel perfeito)
    //-------------------------------------------------------
    float pixelSize = 3.0;   // Aumente para ficar mais pixelado (3–5 é ótimo)
    vec2 uv = clampUV(fragTexCoord);
    vec2 uvPixel = floor(uv * resolution / pixelSize) * pixelSize / resolution;
    uvPixel = clampUV(uvPixel);


    //-------------------------------------------------------
    // 2) Aberração cromática suave (leve deslocamento RGB)
    //-------------------------------------------------------
    float shift = 1.5 / resolution.x;     // 1.5 é o deslocamento ideal
    float vibration = sin(time * 4.0) * 0.002;

    float r = texture(texture0, clampUV(uvPixel + vec2( shift + vibration, 0.0))).r;
    float g = texture(texture0, clampUV(uvPixel)).g;
    float b = texture(texture0, clampUV(uvPixel + vec2(-shift - vibration, 0.0))).b;

    vec3 color = vec3(r, g, b);


    //-------------------------------------------------------
    // 3) Saturação aumentada (cores vibrantes de arcade)
    //-------------------------------------------------------
    float satBoost = 1.45;
    float luma = dot(color, vec3(0.299, 0.587, 0.114));
    color = mix(vec3(luma), color, satBoost);


    //-------------------------------------------------------
    // 4) Scanlines claras (não escurecem a tela)
    //-------------------------------------------------------
    float scan = sin(uv.y * resolution.y * 1.2);
    color += scan * 0.03;


    //-------------------------------------------------------
    // 5) Brilho pulsante leve (fosforescência CRT)
    //-------------------------------------------------------
    float glow = sin(time * 40.0 + uv.y * 400.0) * 0.015;
    color += glow;


    //-------------------------------------------------------
    // 6) Ruído suave (grain quente de CRT)
    //-------------------------------------------------------
    float noise = fract(sin(dot(uv * time, vec2(12.9898,78.233))) * 43758.5453);
    color += (noise - 0.5) * 0.02;


    finalColor = vec4(color, 1.0);
}
