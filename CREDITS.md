Third-party components
----------------------

Not mentioning OpenGL API and platform-specific toolkits.

*   Magnum internally uses code generated using **flextGL** extension loader
    generator -- https://github.com/ginkgo/flextGL. Copyright © 2011 Thomas
    Weber, licensed under [MIT license](https://raw.githubusercontent.com/ginkgo/flextGL/master/COPYING)
*   The `Audio` namespace depends on the **OpenAL** API --
    http://www.openal.org.
*   The `Platform::GlutApplication` class uses **freeGLUT** library --
    http://freeglut.sourceforge.net/, licensed under MIT license.
*   The `Platform::Sdl2Application` class uses **SDL2** library --
    https://www.libsdl.org/, licensed under [ZLIB license](http://www.gzip.org/zlib/zlib_license.html)

Contributors to Magnum library
------------------------------

*   Miguel Martin ([@miguelmartin75](https://github.com/miguelmartin75)) --
    initial Mac OS X port, various other improvements
*   Travis Watkins ([@amaranth](https://github.com/amaranth)) -- support for
    windowless applications under Mac OS X
*   [@ArEnSc](https://github.com/ArEnSc) / [@michael-chung](https://github.com/michael-chung) --
    initial implementation of windowless applications under Mac OS X, Mac OS X
    port improvements
*   Stefan Wasilewski ([@smw](https://github.com/smw)) -- Mac OS X port
    improvements
*   Konstantinos Chatzilygeroudis ([@costashatz](https://github.com/costashatz)) --
    Debian package
*   Olga Turanksaya ([@olga-python](https://github.com/olga-python)) -- Gentoo
    ebuild
*   [@wivlaro](https://github.com/wivlaro) -- numerous bug reports, Mac OS X
    fixes, feature improvements
*   Jonathan Hale ([@Squareys](https://github.com/Squareys)) -- Audio library
    enhancements, GlfwApplication implementation, bug reports, documentation
    improvements
*   Gerhard de Clercq -- Windows RT (Store/Phone) port

Big thanks to everyone involved!
