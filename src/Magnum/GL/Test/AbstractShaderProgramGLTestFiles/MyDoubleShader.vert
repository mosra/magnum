#extension GL_ARB_gpu_shader_fp64: require

in vec4 position;

uniform dmat4 matrix;

void main() {
    gl_Position = mat4(matrix)*position;
}
