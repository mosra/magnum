OpenGL header and extension loader is generated using flextGL, get it at
https://github.com/ginkgo/flextGL.

See [extensions.txt](extensions.txt) for requested version and a list of
non-core extensions. Call `flextGLgen.py` in this directory with the following
arguments:

    .../flextGLgen.py -D . -t . extensions.txt

It will generate `flextGL.h`, `flextGL.cpp` and `flextGLPlatform.cpp` files. As
usual, be sure to check the diff for suspicious changes and whitespace-at-EOL
(although there shouldn't be any).
