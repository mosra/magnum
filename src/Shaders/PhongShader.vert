uniform mat4 transformationMatrix;
uniform mat4 projectionMatrix;
uniform vec3 light;

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normal;

out vec3 transformedNormal;
out vec3 lightDirection;
out vec3 cameraDirection;

void main() {
    /* Transformed vertex position */
    vec4 transformedPosition4 = transformationMatrix*position;
    vec3 transformedPosition = transformedPosition4.xyz/transformedPosition4.w;

    /* Transformed normal vector */
    transformedNormal = normalize(mat3x3(transformationMatrix)*normal);

    /* Direction to the light */
    lightDirection = normalize(light - transformedPosition);

    /* Direction to the camera */
    cameraDirection = -transformedPosition;

    /* Transform the position */
    gl_Position = projectionMatrix*transformedPosition4;
}
