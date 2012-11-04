#ifndef NEW_GLSL
#define fragmentColor gl_FragColor
#endif

uniform vec3 color;

#ifndef NEW_GLSL
out vec4 fragmentColor;
#endif

void main() {
    fragmentColor = vec4(color, 1.0);
}
