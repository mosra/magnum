#ifndef NEW_GLSL
#define in attribute
#endif

#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 0) uniform mat3 transformationProjectionMatrix;
#else
uniform highp mat3 transformationProjectionMatrix;
#endif

#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = 0) in highp vec2 position;
#else
in highp vec2 position;
#endif

void main() {
    gl_Position.xywz = vec4(transformationProjectionMatrix*vec3(position, 1.0), 0.0);
}
