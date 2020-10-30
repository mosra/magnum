#version 140

#ifdef SHOULD_BE_UNDEFINED
#error no, this should not be defined
#endif

void main() {
    /* Should get potentially redefined to something else, causing a different
       validation message */
    float reserved__identifier = 3.0;
    gl_FragColor = vec4(reserved__identifier);
}
