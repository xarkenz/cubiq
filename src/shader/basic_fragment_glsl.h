// This is a header file serving only to hold GLSL code.
// NOTE: The #version directive is automatically inserted at runtime according to user's GLSL version.
const char* BASIC_FRAGMENT_GLSL = R"(
#if __VERSION__ >= 130
    #define varying in
    out vec4 color;
#else
    #define color gl_FragColor
#endif

varying vec4 fColor;

void main() {
    color = fColor;
}
)";