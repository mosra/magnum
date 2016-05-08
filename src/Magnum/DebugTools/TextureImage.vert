void main() {
    gl_Position = vec4((gl_VertexID == 2) ?  3.0 : -1.0,
                       (gl_VertexID == 1) ? -3.0 :  1.0, 0.0, 1.0);
}
