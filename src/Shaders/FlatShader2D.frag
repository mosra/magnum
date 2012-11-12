#ifndef NEW_GLSL
#define fragmentColor gl_FragColor
#endif

uniform lowp vec3 color;

#ifdef NEW_GLSL
out lowp vec4 fragmentColor;
#endif

void main() {
    fragmentColor = vec4(color, 1.0);
}
