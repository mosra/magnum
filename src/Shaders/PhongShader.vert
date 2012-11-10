#ifndef NEW_GLSL
#define in attribute
#define out varying
#endif

uniform highp mat4 transformationMatrix;
uniform highp mat4 projectionMatrix;
uniform highp vec3 light;

#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = 0) in highp vec4 position;
layout(location = 1) in mediump vec3 normal;
#else
in highp vec4 position;
in mediump vec3 normal;
#endif

out mediump vec3 transformedNormal;
out highp vec3 lightDirection;
out highp vec3 cameraDirection;

void main() {
    /* Transformed vertex position */
    highp vec4 transformedPosition4 = transformationMatrix*position;
    highp vec3 transformedPosition = transformedPosition4.xyz/transformedPosition4.w;

    /* Transformed normal vector */
    transformedNormal = normalize(mat3x3(transformationMatrix)*normal);

    /* Direction to the light */
    lightDirection = normalize(light - transformedPosition);

    /* Direction to the camera */
    cameraDirection = -transformedPosition;

    /* Transform the position */
    gl_Position = projectionMatrix*transformedPosition4;
}
