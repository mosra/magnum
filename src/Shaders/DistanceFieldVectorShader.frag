#ifndef NEW_GLSL
#define in varying
#define fragmentColor gl_FragColor
#endif

#ifndef GL_ES
#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 1) uniform lowp vec4 color;
layout(location = 2) uniform lowp vec4 outlineColor;
layout(location = 3) uniform lowp vec2 outlineRange = vec2(0.5, 0.0);
layout(location = 4) uniform lowp float smoothness = 0.04;
#else
uniform lowp vec4 color;
uniform lowp vec4 outlineColor;
uniform lowp vec2 outlineRange = vec2(0.5, 0.0);
uniform lowp float smoothness = 0.04;
#endif
#else
uniform lowp vec4 color;
uniform lowp vec4 outlineColor;
uniform lowp vec2 outlineRange;
uniform lowp float smoothness;
#endif

#ifdef EXPLICIT_TEXTURE_LAYER
layout(binding = 16) uniform sampler2D vectorTexture;
#else
uniform lowp sampler2D vectorTexture;
#endif

in vec2 fragmentTextureCoordinates;

#ifdef NEW_GLSL
out vec4 fragmentColor;
#endif

void main() {
    lowp float intensity = texture(vectorTexture, fragmentTextureCoordinates).r;

    /* Fill color */
    fragmentColor = smoothstep(outlineRange.x-smoothness, outlineRange.x+smoothness, intensity)*color;

    /* Outline */
    if(outlineRange.x < outlineRange.y) {
        lowp float mid = (outlineRange.x + outlineRange.y)/2.0;
        lowp float half = (outlineRange.y - outlineRange.x)/2.0;
        fragmentColor += smoothstep(half+smoothness, half-smoothness, distance(mid, intensity))*outlineColor;
    }
}
