Magnum is 2D/3D graphics engine written in C++11 and modern OpenGL. Its goal is
to simplify low-level graphics development and interaction with OpenGL using
recent C++11 features and to abstract away platform-specific issues.

DESIGN GOALS
============

*   **2D is not an ugly stepchild**
    Many engines out there were created as pure 2D or 3D and the alternative is
    usually just an afterthought, if it is present at all. If you want to do
    your next project in 2D only, you have to either relearn another engine
    from scratch or emulate it in 3D, leaving many things overly complicated.
    Magnum treats 2D equivalently to 3D so you can reuse what you already
    learned for 3D and even combine 2D and 3D in one project.

*   **Forward compatibility**
    If newer technology makes things faster, simpler or more intuitive, it is
    the way to go. Magnum by default relies on decent C++11 support and modern
    OpenGL features and if some feature isn't available, it tries to emulate it
    using older functionality. However, you are not restricted to use the older
    functionality directly, if you really want to.

*   **Intuitive, but not restrictive API**
    Scripting languages are often preferred to C/C++ because they are designed
    to have less complicated APIs and less boilerplate code. Magnum is
    designed with intuitivity in mind, but also with speed and static checks
    that strongly-typed native code offers. It wraps OpenGL into less verbose
    and more type-safe API, which is easier to use. Usually the most common way
    is the most simple, but if you need full control, you can have it.

*   **Extensible and replaceable components**
    If you want to use different mathematical library for specific purposes,
    that new windowing toolkit, your own file formats or another physics
    library, you can. Conversion of math structures between different libraries
    can be done on top of pre-made skeleton classes, support for file formats
    is done using plugins and platform support is done by writing simple
    wrapper class.

SUPPORTED PLATFORMS
===================

Graphics APIs:

*   **OpenGL** 2.1 through 4.4, core profile functionality and modern
    extensions
*   **OpenGL ES** 2.0, 3.0 and extensions to match desktop OpenGL functionality
*   **WebGL** 1.0 and extensions to match desktop OpenGL functionality

Platforms:

*   **Linux** and embedded Linux (natively using GLX/EGL and Xlib or through
    SDL2 or GLUT toolkit)
*   **Windows** (through SDL2 or GLUT toolkit)
*   **OS X** (through SDL2 toolkit, thanks to [Miguel Martin](https://github.com/miguelishawt))
*   **Android** 2.3 (API Level 9) and higher
*   **Google Chrome** (through [Native Client](https://developers.google.com/native-client/),
    both `newlib` and `glibc` toolchains are supported)
*   **HTML5/JavaScript** (through [Emscripten](https://github.com/kripken/emscripten/wiki))

FEATURES
========

*   Actively maintained Doxygen documentation with tutorials and examples.
    Snapshot is available at http://mosra.cz/blog/magnum-doc/.
*   Vector and matrix library with implementation of complex numbers,
    quaternions and their dual counterparts for representing transformations.
*   Classes wrapping OpenGL using RAII principle and simplifying its usage with
    direct state access and automatic fallback for unavailable features.
*   Extensible scene graph which can be modified for each specific usage.
*   Plugin-based data exchange framework, tools for manipulating meshes,
    textures and images.
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

*   C++ compiler with good C++11 support. Currently there are two compilers
    which are tested to have everything needed: **GCC** >= 4.7 and **Clang**
    >= 3.1. On Windows you can use **MinGW**. GCC 4.6, 4.5, 4.4 and **MSVC**
    2013 support involves some ugly workarounds and thus is available only in
    `compatibility` branch.
*   **CMake** >= 2.8.8
*   **Corrade** - Plugin management and utility library. You can get it at
    https://github.com/mosra/corrade.

Note that full feature set is available only on GCC 4.8.1 and Clang 3.1.

Compilation, installation
-------------------------

The library (for example with support for SDL2 applications) can be built and
installed using these four commands:

    mkdir -p build && cd build
    cmake .. \
        -DCMAKE_INSTALL_PREFIX=/usr \
        -DWITH_SDL2APPLICATION=ON
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

GETTING STARTED
===============

The Doxygen documentation has a thorough [guide how to start using Magnum](http://mosra.cz/blog/magnum-doc/getting-started.html)
in your project.

RELATED PROJECTS
================

The engine itself is kept as small as possible with only little dependencies.
Additional functionality, often depending on external libraries, is provided in
separate repositories. Integration with various external math and physics
libraries can be found at https://github.com/mosra/magnum-integration. Various
importer plugins for image, audio and 3D model formats are maintained in
separate repository, which can be found at https://github.com/mosra/magnum-plugins.

There are also examples of engine usage, varying from simple *Hello World*-like
example to more advanced applications, such as viewer for complex 3D models.
Example repository is at https://github.com/mosra/magnum-examples.

Repository with bootstrap projects for many use cases, helping you get up and
running in no time is located at https://github.com/mosra/magnum-bootstrap.

CONTACT
=======

Want to learn more about the library? Found a bug or want to tell me an awesome
idea? Feel free to visit my website or contact me at:

*   Website -- http://mosra.cz/blog/magnum.php
*   GitHub -- https://github.com/mosra/magnum
*   Google Groups -- https://groups.google.com/forum/#!forum/magnum-engine
*   Twitter -- https://twitter.com/czmosra
*   E-mail -- mosra@centrum.cz
*   Jabber -- mosra@jabbim.cz

LICENSE
=======

Magnum is licensed under MIT/Expat license, see [COPYING](COPYING) file for
details.
