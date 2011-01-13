Magnum is currently an simple OpenGL 3 graphics engine, not using any deprecated
functionality. It's currently used in Kompas 3D map view plugin.

INSTALLATION
============

Dependencies
------------

 * CMake    - for building
 * OpenGL headers (on Linux most probably shipped with Mesa)
 * GLEW     - OpenGL extension wrangler
 * GLUT     - optionally, for examples
 * Qt       - optionally, for unit tests

Compilation, installation
-------------------------

    mkdir -p build
    cd build
    cmake -DCMAKE_INSTALL_PREFIX=/usr .. && make
    make install

Some examples of engine usage are stored in examples/ directory, if you want to
build them, pass -DBUILD_EXAMPLES=True to CMake. Examples use the GLUT library.
If you want to build also unit tests (which are not built by default),
pass -DBUILD_TESTS=True to CMake. Unit tests use QtTest framework.

CONTACT
=======

Want to learn more about the library? Found a bug or want to tell me an
awesome idea? Feel free to visit my website or contact me at:

 * Website - http://mosra.cz/blog/
 * GitHub - http://github.com/mosra
 * E-mail - mosra@centrum.cz
 * Jabber - mosra@jabbim.cz
