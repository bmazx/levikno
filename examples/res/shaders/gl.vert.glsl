#version 450

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexUV;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTex;

layout (std140, binding = 0) uniform ObjectBuffer
{
    mat4 matrix;
} ubo;

void main() {
    gl_Position = ubo.matrix * vec4(inPos, 0.0, 1.0);
    gl_Position.y = -gl_Position.y;
    fragColor = inColor;
    fragTex = inTexUV;
}
