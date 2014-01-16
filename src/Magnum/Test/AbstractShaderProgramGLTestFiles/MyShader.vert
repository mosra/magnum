attribute mediump vec4 position;

uniform mediump mat4 matrix;

void main() {
    gl_Position = matrix*position;
}
