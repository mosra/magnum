#version 330

in vec4 vertex;
in vec4 color;

out vec4 varyingColor;

void main() {
    varyingColor = color;

    gl_Position = vertex;
}
