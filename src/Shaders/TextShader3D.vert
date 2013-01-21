#ifndef NEW_GLSL
#define in attribute
#define out varying
#endif

#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 0) uniform mat4 transformationProjectionMatrix;
#else
uniform highp mat4 transformationProjectionMatrix;
#endif

#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = 0) in highp vec4 position;
layout(location = 1) in mediump vec2 textureCoordinates;
#else
in highp vec4 position;
in mediump vec2 textureCoordinates;
#endif

out vec2 fragmentTextureCoordinates;

void main() {
    gl_Position = transformationProjectionMatrix*position;
    fragmentTextureCoordinates = textureCoordinates;
}
