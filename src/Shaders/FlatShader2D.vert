#ifndef NEW_GLSL
#define in attribute
#endif

uniform mat3 transformationProjection;

#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = 0) in vec3 position;
#else
in vec3 position;
#endif

void main() {
    gl_Position.xywz = vec4(transformationProjection*position, 0.0);
}
