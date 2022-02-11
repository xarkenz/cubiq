// This is a header file serving only to hold GLSL code.
// NOTE: The #version directive is automatically inserted at runtime according to user's GLSL version.
const char* BASIC_VERTEX_GLSL = R"(
#if __VERSION__ >= 130
    #define attribute in
    #define varying out
#endif

attribute vec3 vPos;
attribute vec4 vColor;

uniform mat4 uProjection;

varying vec4 fColor;

void main() {
    fColor = vColor;
    gl_Position = uProjection * vec4(vPos, 1);
}
)";