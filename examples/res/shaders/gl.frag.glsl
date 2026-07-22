#version 450

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTex;

layout(binding = 1) uniform sampler2D inTexture;

void main() {
    vec3 color = vec3(texture(inTexture, fragTex));
    outColor = vec4(color, 1.0);
}
