uniform double multiplier;
uniform dvec4 color;
uniform dvec4 additions[3];

out vec4 fragColor;

void main() {
    fragColor = vec4(color*multiplier + additions[0] + additions[1] + additions[2]);
}
