OpenGL header and extension loader is generated using flextGL, get it at
[](https://github.com/ginkgo/flextGL).

See [](extensions.txt) for requested version and a list of non-core extensions.
Call `flextGLgen.py` in this directory with the following arguments:

    .../flextGLgen.py -D . -t . extensions.txt

It will generate `flextGL.h`, `flextGL.cpp`, `flextGLPlatform.cpp` and
`flextGLPlatformIOS.cpp` files.

Emscripten doesn't have the ability to manually load extension pointers,
thus it has only header files:

    .../flextGLgen.py -D . -t Emscripten/ Emscripten/extensions.txt

This will generate stripped-down `flextGLEmscripten.h` file.

As usual, be sure to check the diff for suspicious changes and
whitespace-at-EOL (although there shouldn't be any).
