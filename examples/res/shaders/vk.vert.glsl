#version 450

layout(location = 0) in vec3 inPos;
layout(location = 2) in vec2 inTexUV;

layout(location = 1) out vec2 fragTex;

layout (binding = 0) uniform ObjectBuffer
{
    mat4 matrix;
} ubo;

void main() {
    gl_Position = ubo.matrix * vec4(inPos, 1.0);
    fragTex = inTexUV;
}
