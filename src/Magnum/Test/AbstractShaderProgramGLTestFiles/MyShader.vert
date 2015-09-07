#if !defined(GL_ES) && __VERSION__ == 120
#define mediump
#endif

attribute mediump vec4 position;

uniform mediump mat4 matrix;

void main() {
    gl_Position = matrix*position;
}
