uniform mat3 transformationProjection;

layout(location = 0) in vec3 position;

void main() {
    gl_Position.xywz = vec4(transformationProjection*position, 0.0);
}
