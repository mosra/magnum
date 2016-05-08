OpenGL header and extension loader is generated using flextGL, get it at
https://github.com/ginkgo/flextGL.

See [extensions.txt](extensions.txt) for requested version and a list of non-core extensions.
Call `flextGLgen.py` in this directory with the following arguments to generate
files for generic GLES2 implementations:

    .../flextGLgen.py -D . -t . extensions.txt

It will generate `flextGL.h`, `flextGL.cpp`, `flextGLPlatform.cpp` and
`flextGLPlatformIOS.cpp` files.

NaCl and Emscripten don't have the ability to manually load extension pointers,
thus they have only header files:

    .../flextGLgen.py -D . -t NaCl/ extensions.txt
    .../flextGLgen.py -D . -t Emscripten/ Emscripten/extensions.txt

This will generate stripped-down `flextGLNaCl.h` and `flextGLEmscripten.h`
files.

As usual, be sure to check the diff for suspicious changes and
whitespace-at-EOL (although there shouldn't be any).
