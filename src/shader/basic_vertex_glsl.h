// This is a header file serving only to hold GLSL code.
R"(
#version 330 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec4 vColor;

uniform mat4 uProjection;

out vec4 fColor;

void main() {
  fColor = vColor;
  gl_Position = uProjection * vec4(vPos, 1);
}
)";