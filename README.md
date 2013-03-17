Magnum is 2D/3D graphics engine written in C++11 and OpenGL 3 Core Profile.

DESIGN GOALS
============

*   **2D is not an ugly stepchild**
    Many engines out there are either purely 2D or 3D and if you want to do
    your next project in 2D only, you have to either relearn another engine
    from scratch or emulate it in 3D, leaving many things overly complicated.
    Magnum treats 2D equivalently to 3D so you can reuse what you already
    learned for 3D and even combine 2D and 3D in one project.

*   **Forward compatibility**
    If newer technology makes things faster, simpler or more intuitive, it is
    the way to go. If you then really need to, you can selectively backport
    some features and it will be easier than maintaining full backward
    compatibility by default. Magnum by default relies on decent C++11 support
    and modern OpenGL features, but compatibility functions for older hardware
    and compatibility branch for older compilers are available if you need
    them.

*   **Intuitive, but not restrictive API**
    Scripting languages are often preferred to C/C++ because they tend to do
    more with less -- less complicated APIs, nicer syntax and less boilerplate
    code. Magnum is designed with scripting language intuitivity in mind, but
    also with speed and static checks that native code and strong typing
    offers. Usually the most common way is the most simple, but if you need
    full control, you can have it.

*   **Extensible and replaceable components**
    If you want to use different mathematical library for specific purposes,
    that new windowing toolkit, your own file formats or another physics
    library, you can. Conversion of data between different libraries can be
    done on top of pre-made skeleton classes, support for file formats is done
    using plugins and platform support is done by writing simple wrapper class.

FEATURES
========

*   Actively maintained Doxygen documentation with tutorials and examples.
    Snapshot is available at http://mosra.cz/blog/magnum-doc/.
*   Vector and matrix library with implementation of complex numbers,
    quaternions and their dual counterparts for representing transformations.
*   Classes wrapping OpenGL and simplifying its usage with direct state access
    and automatic fallback for unavailable features.
*   Extensible scene graph which can be modified for each specific usage.
*   Plugin-based data exchange framework, tools for manipulating meshes,
    textures and images.
*   Integration with various windowing toolkits and also ability to create
    windowless contexts.
*   Pre-made shaders, primitives and other tools for easy prototyping and
    debugging.

INSTALLATION
============

You can either use packaging scripts, which are stored in `package/`
subdirectory, or compile and install everything manually. Note that Doxygen
documentation (see above or build your own using instructions below) contains
more comprehensive guide for building, packaging and crosscompiling.

Minimal dependencies
--------------------

 * C++ compiler with good C++11 support. Currently there are two compilers
   which are tested to support everything needed: **GCC** >= 4.6 and **Clang**
   >= 3.1.
 * **CMake** >= 2.8.8 (needed for `OBJECT` library target)
 * **GLEW** - OpenGL extension wrangler (only if targeting desktop OpenGL)
 * **Corrade** - Plugin management and utility library. You can get it at
   https://github.com/mosra/corrade.

Compilation, installation
-------------------------

The library (for example with support for GLUT applications) can be built and
installed using these four commands:

    mkdir -p build && cd build
    cmake .. \
        -DCMAKE_INSTALL_PREFIX=/usr \
        -DWITH_GLUTAPPLICATION=ON
    make
    make install

See Doxygen documentation for more information about enabling or disabling
additional features and targeting different platforms such as OpenGL ES.

Building and running unit tests
-------------------------------

If you want to build also unit tests (which are not built by default), pass
`-DBUILD_TESTS=ON` to CMake. Unit tests use Corrade's TestSuite framework and
can be run using

    ctest --output-on-failure

in build directory. Everything should pass ;-)

Building documentation
----------------------

The documentation is written in **Doxygen** (version 1.8 with Markdown support
is used, but older versions should do good job too) and additionally uses
**Graphviz** for class diagrams and **TeX** for math formulas. The
documentation can be build by running:

    doxygen

in root directory (i.e. where `Doxyfile` is). Resulting HTML documentation
will be in `build/doc/` directory. You might need to create `build/` directory
if it doesn't exist yet.

PLUGINS AND EXAMPLES
====================

Various importer plugins for image and 3D model formats are maintained in
separate repository, which can be found at https://github.com/mosra/magnum-plugins.

There are also examples of engine usage, varying from simple *Hello World*-like
example to more advanced applications, such as viewer for complex 3D models.
Example repository is at https://github.com/mosra/magnum-examples.

CONTACT
=======

Want to learn more about the library? Found a bug or want to tell me an
awesome idea? Feel free to visit my website or contact me at:

 * Website - http://mosra.cz/blog/
 * GitHub - https://github.com/mosra/magnum
 * E-mail - mosra@centrum.cz
 * Jabber - mosra@jabbim.cz

LICENSE
=======

Magnum is licensed under MIT/Expat license, see [COPYING](COPYING) file for
details.
