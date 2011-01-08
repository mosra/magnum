#version 330

uniform sampler2D textureData;

in vec2 varyingTextureCoordinates;

out vec4 fragmentColor;

void main() {
    fragmentColor = texture(textureData, varyingTextureCoordinates);
}
