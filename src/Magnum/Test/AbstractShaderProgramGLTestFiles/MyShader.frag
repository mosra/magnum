#if !defined(GL_ES) && __VERSION__ == 120
#define lowp
#endif

#if defined(GL_ES) || __VERSION__ == 120
#define fragColor gl_FragColor
#endif

uniform lowp float multiplier;
uniform lowp vec4 color;
uniform lowp vec4 additions[3];

#if !defined(GL_ES) && __VERSION__ >= 130
out lowp vec4 fragColor;
#endif

void main() {
    fragColor = color*multiplier + additions[0] + additions[1] + additions[2];
}
