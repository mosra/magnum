#ifndef NEW_GLSL
#define in attribute
#endif

#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 0) uniform mat4 transformationProjectionMatrix;
#else
uniform highp mat4 transformationProjectionMatrix;
#endif

#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = 0) in highp vec4 position;
#else
in highp vec4 position;
#endif

void main() {
    gl_Position = transformationProjectionMatrix*position;
}
