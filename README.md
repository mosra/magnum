Magnum — *Lightweight and modular C++11/C++14 graphics middleware for games and data visualization*

Looking for an open-source library that gives you graphics abstraction and
platform independence on major desktop, mobile and web platforms? Do you want
it to have all the convenience utilities around yet stay small, powerful and
not give up on flexibility? *Here it is.* And it's free to use, even for
commercial purposes.

[![Join the chat at https://gitter.im/mosra/magnum](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/mosra/magnum?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)
[![Build Status](https://travis-ci.org/mosra/magnum.svg?branch=master)](https://travis-ci.org/mosra/magnum)
[![Build Status](https://ci.appveyor.com/api/projects/status/5b477m034cfaskse/branch/master?svg=true)](https://ci.appveyor.com/project/mosra/magnum/branch/master)
[![Coverage Status](https://codecov.io/gh/mosra/magnum/branch/master/graph/badge.svg)](https://codecov.io/gh/mosra/magnum)
[![MIT License](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

-   Project homepage — http://magnum.graphics/
-   Documentation — http://doc.magnum.graphics/
-   GitHub project page — https://github.com/mosra/magnum

Beauty of simplicity
====================

Among Magnum essentials is a UTF-8-aware OS, filesystem and console
abstraction, a feature-packed vector math library and a slim C++11 wrapper of
OpenGL / WebGL family of APIs. Build on top of that or opt-in for more.

[See all core features](http://magnum.graphics/features/)

With batteries included
=======================

Shaders and primitives for fast prototyping, algorithms, debugging and
automatic testing, asset management, integration with popular windowing
toolkits and a UI library. Everything fits together but you still have a
choice.

[List the extra features](http://magnum.graphics/features/extras/)

Screws are not glued in
=======================

There's always more than one way to do things. Enjoy the freedom of choice and
integrate your own asset loader, texture compressor, font format or math
library, if you feel the need. Or use any of the various plugins.

[View extension points](http://magnum.graphics/features/extensions/)

-------

*Wondering if Magnum is a good fit for your project?* We prepared
[a few case studies](http://magnum.graphics/is-magnum-what-i-am-looking-for/)
to help you decide.

SUPPORTED PLATFORMS
===================

-   **Linux** and embedded Linux
-   **Windows**, **Windows RT** (Store/Phone)
-   **macOS**, **iOS**
-   **Android**
-   **Web** ([asm.js](http://asmjs.org/) or [WebAssembly](http://webassembly.org/)),
    through [Emscripten](http://kripken.github.io/emscripten-site/)

Graphics APIs:

-   **OpenGL** 2.1 through 4.6, core profile functionality and modern
    extensions
-   **OpenGL ES** 2.0, 3.0–3.2 and extensions to match desktop OpenGL
    functionality
-   **WebGL** 1.0, 2.0 and extensions to match desktop OpenGL functionality

See the [Build Status page](http://magnum.graphics/build-status/) for detailed
per-platform build status.

WHAT'S NEW?
===========

Curious about what was added or improved recently? Check out the
[Changelog](http://doc.magnum.graphics/magnum/changelog.html#changelog-latest)
page in the documentation.

GETTING STARTED
===============

The best way to get started is to read the thorough
[download, build, install and start using Magnum](http://doc.magnum.graphics/magnum/getting-started.html)
in your project. There is also a complete
[building documentation](http://doc.magnum.graphics/magnum/building.html) — we
provide packages for many platforms, including Windows, Linux and macOS.

After that, there are various [tutorials and examples](http://doc.magnum.graphics/magnum/example-index.html)
and a complete [feature guide](http://doc.magnum.graphics/magnum/features.html)
explaining all aspects of the library.

RELATED PROJECTS
================

The engine itself is kept as small as possible with only a few dependencies.
Additional functionality, often depending on external libraries, is provided in
separate repositories.

-   **Corrade** — main Magnum dependency, a multiplatform utility library:
    https://github.com/mosra/corrade
-   **Magnum Bootstrap** — bootstrap projects for many use cases, helping you
    get up and running in no time: https://github.com/mosra/magnum-bootstrap
-   **Magnum Plugins** — various importer plugins for image, font, audio and
    3D model formats is at https://github.com/mosra/magnum-plugins-plugins/branch/master)
-   **Magnum Integration** — integration with various external math and
    physics, get it at https://github.com/mosra/magnum-integration
-   **Magnum Examples** — examples of engine usage, varying from simple
    *Hello World*-like example to more advanced applications, such as viewer
    for complex 3D models. See it at https://github.com/mosra/magnum-examples
-   **Magnum Extras** — playground for testing new APIs, specialized stuff
    that doesn't necessarily need to be a part of main Magnum repository or
    mutually exclusive functionality: https://github.com/mosra/magnum-extras

Outside of the project itself, there's also a lot of community contributions
— [check them out on the website](http://magnum.graphics/features/community/).

CONTACT & SUPPORT
=================

If you want to contribute to Magnum, if you spotted a bug, need a feature or
have an awesome idea, you can get a copy of the sources from GitHub and start
right away! There is the already mentioned guide about
[how to download and build Magnum](http://doc.magnum.graphics/magnum/building.html)
and also a guide about [coding style and best practices](http://doc.magnum.graphics/magnum/coding-style.html)
which you should follow to keep the library as consistent and maintainable as
possible.

-   Project homepage — http://magnum.graphics/
-   Documentation — http://doc.magnum.graphics/
-   GitHub project page — https://github.com/mosra/magnum
-   Gitter community chat — https://gitter.im/mosra/magnum
-   E-mail — info@magnum.graphics
-   IRC — join the `#magnum-engine` channel on freenode
-   Google Groups mailing list — magnum-engine@googlegroups.com ([archive](https://groups.google.com/forum/#!forum/magnum-engine))
-   Author's personal Twitter — https://twitter.com/czmosra

See also the Magnum Project [Contact & Support page](http://magnum.graphics/contact/)
for further information.

CREDITS
=======

See the [CREDITS.md](CREDITS.md) file for details. Big thanks to everyone
involved!

LICENSE
=======

Magnum is licensed under the MIT/Expat license, see the [COPYING](COPYING) file
for details.
