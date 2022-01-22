// This is a header file serving only to hold GLSL code.
R"(
#version 330 core

in vec4 fColor;

out vec4 color;

void main() {
  color = fColor;
}
)";