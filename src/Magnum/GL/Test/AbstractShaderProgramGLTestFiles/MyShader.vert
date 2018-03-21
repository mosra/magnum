#if !defined(GL_ES) && __VERSION__ == 120
#define mediump
#endif

#if defined(GL_ES) || __VERSION__ == 120
#define in attribute
#endif

in mediump vec4 position;

uniform mediump mat4 matrix;

void main() {
    gl_Position = matrix*position;
}
