#ifndef NEW_GLSL
#define in attribute
#define out varying
#endif

#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 0) uniform mat3 transformationProjectionMatrix;
#else
uniform highp mat3 transformationProjectionMatrix;
#endif

#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = 0) in highp vec2 position;
layout(location = 1) in mediump vec2 textureCoordinates;
#else
in highp vec2 position;
in mediump vec2 textureCoordinates;
#endif

out vec2 fragmentTextureCoordinates;

void main() {
    gl_Position.xywz = vec4(transformationProjectionMatrix*vec3(position, 1.0), 0.0);
    fragmentTextureCoordinates = textureCoordinates;
}
