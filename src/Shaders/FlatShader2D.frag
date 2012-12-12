#ifndef NEW_GLSL
#define fragmentColor gl_FragColor
#endif

#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 1) uniform vec3 color;
#else
uniform lowp vec3 color;
#endif

#ifdef NEW_GLSL
out lowp vec4 fragmentColor;
#endif

void main() {
    fragmentColor = vec4(color, 1.0);
}
