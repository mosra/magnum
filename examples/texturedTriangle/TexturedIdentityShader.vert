#version 330

in vec4 vertex;
in vec2 textureCoordinates;

out vec2 varyingTextureCoordinates;

void main() {
    varyingTextureCoordinates = textureCoordinates;

    gl_Position = vertex;
}
