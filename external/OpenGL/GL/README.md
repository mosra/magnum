OpenGL header and extension loader is generated using [glLoadGen](https://bitbucket.org/alfonse/glloadgen),
preferrably from tip of the development branch, currently `2.0`. For it to work
flawlessly on OpenGL 2.1 and simultaenously not containing all that pre-core
stuff, you need to patch it with included `glloadgen.patch`, which will then
force it to use pre-3.0 `glGetString(GL_EXTENSIONS)` even if we request OpenGL
4.4. Otherwise it would use `glGetStringi(GL_EXTENSIONS, i)`, which is not
available on OpenGL 2.1 and extension loading will fail on these systems.

See `extensions.txt` for a list of requested non-core extensions. Copy the file
into glLoadGen directory and generate the files using Lua:

    lua LoadGen.lua -style=pointer_c -indent=space -spec=gl -version=4.4 -profile=core -extfile=extensions.txt magnum

Then copy generated `gl_magnum.c` and `gl_magnum.h` to this directory. You may
want to remove trailing spaces to make the diff cleaner. Be sure to check the
diff for suspicious changes.
