OpenGL header and extension loader is generated using flextGL, get it at
https://github.com/ginkgo/flextGL.

See [extensions.txt](extensions.txt) for requested version and a list of
non-core extensions. Call `flextGLgen.py` in this directory with the following
arguments:

    .../flextGLgen.py -D . -t . extensions.txt

It will generate `flextGL.h`, `flextGL.cpp`, `flextGLPlatform.cpp`,
`flextGLWindowsDesktop.h`, `flextGLWindowsDesktop.cpp`,
`flextGLPlatformWindowsDesktop.cpp` and `flextGLPlatformIOS.cpp` files.

Desktop GLES on Windows still links to the ancient `opengl32.dll` which exports
only OpenGL 1.1 symbols, so we have a special set of headers that queries
pointers for everything above OpenGL 1.1 (instead of everything above OpenGL ES
3.0).

iOS, on the other hand, doesn't have any extension loader mechanism and all
supported entrypoints are exported from the library, so we set the function
pointers to those exported symbols in case the system GL header defines them.

Emscripten doesn't have the ability to manually load extension pointers,
thus it has only header files:

    .../flextGLgen.py -D . -t Emscripten/ Emscripten/extensions.txt

This will generate stripped-down `flextGLEmscripten.h` file.

As usual, be sure to check the diff for suspicious changes and
whitespace-at-EOL (although there shouldn't be any).
