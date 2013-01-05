#ifndef NEW_GLSL
#define in varying
#define fragmentColor gl_FragColor
#endif

in lowp vec3 interpolatedColor;

#ifdef NEW_GLSL
out lowp vec4 fragmentColor;
#endif

void main() {
    fragmentColor = vec4(interpolatedColor, 1.0);
}
