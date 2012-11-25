#ifndef NEW_GLSL
#define in attribute
#endif

uniform highp mat3 transformationProjectionMatrix;

#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = 0) in highp vec3 position;
#else
in highp vec3 position;
#endif

void main() {
    gl_Position.xywz = vec4(transformationProjectionMatrix*position, 0.0);
}
