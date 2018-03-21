in mediump vec4 position;

layout(std140) uniform matrices {
    mediump mat4 transformation;
    mediump mat4 projection;
};

void main() {
    gl_Position = projection*transformation*position;
}
