OpenGL header and extension loader is generated using glLoadGen, originally
from [BitBucket](https://bitbucket.org/alfonse/glloadgen), with Magnum-related
changes at [](https://github.com/mosra/glloadgen.git).

See `extensions.txt` for a list of requested non-core extensions. Copy the file
into glLoadGen directory and generate the files using Lua:

    lua LoadGen.lua -style=pointer_c -indent=space -spec=gl -version=4.4 -profile=core -extfile=extensions.txt magnum

Then copy generated `gl_magnum.c` and `gl_magnum.h` to this directory. You may
want to remove trailing spaces to make the diff cleaner. Be sure to check the
diff for suspicious changes.
