uniform highp int level;
uniform highp sampler2D textureData;

layout(pixel_center_integer) in highp vec4 gl_FragCoord;

out highp uvec4 fragmentOutput;

void main() {
    ivec2 pos = ivec2(gl_FragCoord.xy);

    fragmentOutput = floatBitsToUint(texelFetch(textureData, pos, level));
}
