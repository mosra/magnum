uniform lowp float multiplier;
uniform lowp vec4 color;
uniform lowp vec4 additions[3];

void main() {
    gl_FragColor = color*multiplier + additions[0] + additions[1] + additions[2];
}
