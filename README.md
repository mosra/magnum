Magnum is 2D/3D graphics engine written in C++11/C++14 and modern OpenGL. Its
goal is to simplify low-level graphics development and interaction with OpenGL
using recent C++11/C++14 features and to abstract away platform-specific
issues.

[![Join the chat at https://gitter.im/mosra/magnum](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/mosra/magnum?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

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

*   **OpenGL** 2.1 through 4.5, core profile functionality and modern
    extensions
*   **OpenGL ES** 2.0, 3.0, 3.1 and extensions to match desktop OpenGL
    functionality
*   **WebGL** 1.0, 2.0 and extensions to match desktop OpenGL functionality

Platforms:

*   **Linux** and embedded Linux (natively using GLX/EGL and Xlib or through
    SDL2, GLFW or GLUT toolkit) [![Build Status](https://travis-ci.org/mosra/magnum.svg?branch=master)](https://travis-ci.org/mosra/magnum) [![Coverage Status](https://coveralls.io/repos/github/mosra/magnum/badge.svg?branch=master)](https://coveralls.io/github/mosra/magnum?branch=master)
*   **Windows** on both MSVC and MinGW, natively or using ANGLE (through SDL2, GLFW or GLUT toolkit) [![Build Status](https://ci.appveyor.com/api/projects/status/5b477m034cfaskse/branch/master?svg=true)](https://ci.appveyor.com/project/mosra/magnum/branch/master)
*   **macOS** (through SDL2 or GLFW toolkit) [![Build Status](https://travis-ci.org/mosra/magnum.svg?branch=master)](https://travis-ci.org/mosra/magnum)
*   **iOS** (through SDL2 toolkit) [![Build Status](https://travis-ci.org/mosra/magnum.svg?branch=master)](https://travis-ci.org/mosra/magnum)
*   **Android** 2.3 (API Level 9) and higher [![Build Status](https://travis-ci.org/mosra/magnum.svg?branch=master)](https://travis-ci.org/mosra/magnum)
*   **Windows RT** (Store/Phone) using ANGLE (through SDL2 toolkit) [![Build Status](https://ci.appveyor.com/api/projects/status/5b477m034cfaskse/branch/master?svg=true)](https://ci.appveyor.com/project/mosra/magnum/branch/master)
*   **Web** (asm.js or WebAssembly), through [Emscripten](http://kripken.github.io/emscripten-site/) [![Build Status](https://travis-ci.org/mosra/magnum.svg?branch=master)](https://travis-ci.org/mosra/magnum)

FEATURES
========

*   Actively maintained Doxygen documentation with tutorials and examples.
    Snapshot is available at http://doc.magnum.graphics/magnum/.
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
subdirectory, or compile and install everything manually. Note that
[Magnum documentation](http://doc.magnum.graphics/magnum/) contains more
comprehensive guide for building, packaging and crosscompiling.

Minimal dependencies
--------------------

*   C++ compiler with good C++11 support. Compilers which are tested to have
    everything needed are **GCC** >= 4.7, **Clang** >= 3.1 and **MSVC** >= 2015.
    On Windows you can also use **MinGW-w64**.
*   **CMake** >= 2.8.12
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

The Doxygen documentation has a thorough [guide how to start using Magnum](http://doc.magnum.graphics/magnum/getting-started.html)
in your project.

RELATED PROJECTS
================

The engine itself is kept as small as possible with only little dependencies.
Additional functionality, often depending on external libraries, is provided in
separate repositories.

*   **Corrade** -- main Magnum dependency, multiplatform utility library:
    https://github.com/mosra/corrade
    [![Linux/macOS/iOS/Android/Emscripten Build Status](https://travis-ci.org/mosra/corrade.svg?branch=master)](https://travis-ci.org/mosra/corrade) [![Windows Build Status](https://ci.appveyor.com/api/projects/status/afjjlsgtk6jjxulp/branch/master?svg=true)](https://ci.appveyor.com/project/mosra/corrade/branch/master)
*   **Magnum Bootstrap** -- bootstrap projects for many use cases, helping you
    get up and running in no time: https://github.com/mosra/magnum-bootstrap
*   **Magnum Plugins** -- various importer plugins for image, font, audio and
    3D model formats is at https://github.com/mosra/magnum-plugins
    [![Linux/macOS/iOS/Android/Emscripten Build Status](https://travis-ci.org/mosra/magnum-plugins.svg?branch=master)](https://travis-ci.org/mosra/magnum-plugins) [![Windows Build status](https://ci.appveyor.com/api/projects/status/nkdlwaxm2i9d6vpx/branch/master?svg=true)](https://ci.appveyor.com/project/mosra/magnum-plugins/branch/master)
*   **Magnum Integration** -- integration with various external math and
    physics, get it at https://github.com/mosra/magnum-integration
    [![Linux/macOS/iOS/Android/Emscripten Build Status](https://travis-ci.org/mosra/magnum-integration.svg?branch=master)](https://travis-ci.org/mosra/magnum-integration) [![Windows Build Status](https://ci.appveyor.com/api/projects/status/hs6ykva1ld74vavr/branch/master?svg=true)](https://ci.appveyor.com/project/mosra/magnum-integration/branch/master)
*   **Magnum Examples** -- examples of engine usage, varying from simple
    *Hello World*-like example to more advanced applications, such as viewer
    for complex 3D models. See it at https://github.com/mosra/magnum-examples
    [![LLinux/macOS/iOS/Android/Emscripten Build Status](https://travis-ci.org/mosra/magnum-examples.svg?branch=master)](https://travis-ci.org/mosra/magnum-examples) [![Windows Build Status](https://ci.appveyor.com/api/projects/status/33qdqpdc5n0au3ou/branch/master?svg=true)](https://ci.appveyor.com/project/mosra/magnum-examples/branch/master)
*   **Magnum Extras** -- playground for testing new APIs, specialized stuff
    that doesn't necessarily need to be a part of main Magnum repository or
    mutually exclusive functionality: https://github.com/mosra/magnum-extras
    [![Linux/macOS/iOS/Android/Emscripten Build Status](https://travis-ci.org/mosra/magnum-extras.svg?branch=master)](https://travis-ci.org/mosra/magnum-extras) [![Windows Build status](https://ci.appveyor.com/api/projects/status/f75u5eow2qiso7m5/branch/master?svg=true)](https://ci.appveyor.com/project/mosra/magnum-extras/branch/master)
*   **libRocket integration** -- integrates Magnum as rendering backend into
    [libRocket](https://github.com/libRocket/libRocket) GUI library:
    https://github.com/miguelmartin75/Magnum-libRocket
*   **Dear IMGUI integration** -- integrated Magnum as rendering backend into
    [Dear IMGUI](https://github.com/ocornut/imgui) library:
    https://github.com/denesik/MagnumImguiPort
*   **Magnum Inspector** -- Gtk-based inspector window running alongside Magnum
    https://github.com/wivlaro/magnum-inspector

CONTACT
=======

Want to learn more about the library? Found a bug or want to share an awesome
idea? Feel free to visit the project website or contact the team at:

*   Website -- http://magnum.graphics
*   GitHub -- https://github.com/mosra/magnum
*   Gitter -- https://gitter.im/mosra/magnum
*   IRC -- join `#magnum-engine` channel on freenode
*   Google Groups -- https://groups.google.com/forum/#!forum/magnum-engine
*   Twitter -- https://twitter.com/czmosra
*   E-mail -- mosra@centrum.cz
*   Jabber -- mosra@jabbim.cz

CREDITS
=======

See [CREDITS.md](CREDITS.md) file for details. Big thanks to everyone involved!

LICENSE
=======

Magnum is licensed under MIT/Expat license, see [COPYING](COPYING) file for
details.
