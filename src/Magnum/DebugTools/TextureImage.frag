uniform highp int level;
uniform highp sampler2D textureData;

#ifndef GL_ES
layout(pixel_center_integer) in highp vec4 gl_FragCoord;
#endif

out highp uvec4 fragmentOutput;

void main() {
    #ifndef GL_ES
    ivec2 pos = ivec2(gl_FragCoord.xy);
    #else
    ivec2 pos = ivec2(gl_FragCoord.xy - vec2(0.5));
    #endif

    fragmentOutput = floatBitsToUint(texelFetch(textureData, pos, level));
}
