layout(std140) uniform material {
    lowp vec4 color;
    lowp vec4 additions[3];
    lowp float multiplier;
};

out lowp vec4 fragColor;

void main() {
    fragColor = color*multiplier + additions[0] + additions[1] + additions[2];
}
