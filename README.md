Magnum is 3D graphics engine written in C++11 and OpenGL 3 Core Profile.
Features:

 * Easy-to-use templated mathematical library for matrix/vector calculations
   and geometry.
 * Hierarchical scene graph which supports transformation caching for better
   performance, classes for convenient usage of shaders, buffers and textures.
   Access to framebuffer and occlusion queries.
 * Physics library for collision detection and rigid body dynamics. Mesh tools
   for cleaning, optimizing and generating meshes.
 * Plugin-based data exchange framework for importing image, mesh, material
   and scene data in various formats.
 * Collection of pre-made graphic primitives and shaders for testing purposes.
 * Classes for easy creation of OpenGL context with various toolkits.
 * Comprehensive use of C++11 features for safety, performance and ease of
   development. All code which doesn't directly interact with OpenGL is
   covered with unit tests.
 * Actively maintained Doxygen documentation. Occasionally updated snapshot is
   also available online at http://mosra.cz/blog/magnum-doc .

INSTALLATION
============

You can either use packaging scripts, which are stored in package/
subdirectory, or compile and install everything manually. Note that Doxygen
documentation (see above or build your own using instructions below) contains
more comprehensive guide for building, packaging and crosscompiling.

Minimal dependencies
--------------------

 * C++ compiler with good C++11 support. Currently there are two compilers
   which are tested to support everything needed: **GCC** >= 4.6 and **Clang**
   >= 3.1.
 * **CMake** >= 2.8.8 (needed for `OBJECT` library target)
 * **OpenGL headers**, on Linux most probably shipped with Mesa
 * **GLEW** - OpenGL extension wrangler
 * **Corrade** - Plugin management and utility library. You can get it at
   http://mosra.cz/blog/corrade.php

Compilation, installation
-------------------------

The library can be built and installed using these four commands:

    mkdir -p build
    cd build
    cmake -DCMAKE_INSTALL_PREFIX=/usr .. && make
    make install

Building and running unit tests
-------------------------------

If you want to build also unit tests (which are not built by default), pass
`-DBUILD_TESTS=True` to CMake. Unit tests use QtTest framework (you need at
least **Qt 4.6**) and can be run using

    ctest -V

in build directory. Everything should pass ;-)

Building documentation
----------------------

The documentation is written in **Doxygen** (version 1.8 with Markdown support
is used, but older versions should do good job too) and additionally uses
**Graphviz** for class diagrams and **TeX** for math formulas. The
documentation can be build by running:

    doxygen

in root directory (i.e. where `Doxyfile` is). Resulting HTML documentation
will be in `build/doc/` directory.

PLUGINS AND EXAMPLES
====================

Various importer plugins for image and 3D model formats are maintained in
separate repository, which can be found at
http://github.com/mosra/magnum-plugins .

There are also examples of engine usage, varying from simple *Hello
World*-like example to more advanced applications, such as viewer for complex
3D models. Example repository is at http://github.com/mosra/magnum-examples .

CONTACT
=======

Want to learn more about the library? Found a bug or want to tell me an
awesome idea? Feel free to visit my website or contact me at:

 * Website - http://mosra.cz/blog/
 * GitHub - https://github.com/mosra/magnum
 * E-mail - mosra@centrum.cz
 * Jabber - mosra@jabbim.cz
