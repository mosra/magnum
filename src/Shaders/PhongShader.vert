#version 330

uniform mat4 transformationMatrix;
uniform mat4 projectionMatrix;
uniform vec3 light;

layout(location = 0) in vec4 vertex;
layout(location = 1) in vec3 normal;

out vec3 transformedNormal;
out vec3 lightDirection;
out vec3 cameraDirection;

void main() {
    /* Transformed vertex position */
    vec4 transformedVertex4 = transformationMatrix*vertex;
    vec3 transformedVertex = transformedVertex4.xyz/transformedVertex4.w;

    /* Transformed normal vector */
    transformedNormal = normalize(mat3x3(transformationMatrix)*normal);

    /* Direction to the light */
    lightDirection = normalize(light - transformedVertex);

    /* Direction to the camera */
    cameraDirection = -transformedVertex;

    /* Transform the vertex */
    gl_Position = projectionMatrix*transformedVertex4;
}
