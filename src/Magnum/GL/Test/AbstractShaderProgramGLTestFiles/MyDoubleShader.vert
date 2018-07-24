in vec4 position;

uniform dmat4 matrix;

void main() {
    gl_Position = mat4(matrix)*position;
}
