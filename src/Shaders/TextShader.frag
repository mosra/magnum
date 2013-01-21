#ifndef NEW_GLSL
#define in varying
#define fragmentColor gl_FragColor
#endif

#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 1) uniform vec3 color;
#else
uniform lowp vec3 color;
#endif

#ifdef EXPLICIT_TEXTURE_LAYER
layout(binding = 16) uniform sampler2D fontTexture;
#else
uniform lowp sampler2D fontTexture;
#endif

in vec2 fragmentTextureCoordinates;

#ifdef NEW_GLSL
out vec4 fragmentColor;
#endif

void main() {
    lowp float intensity = texture(fontTexture, fragmentTextureCoordinates).r;
    fragmentColor = vec4(intensity*color, intensity);
}
