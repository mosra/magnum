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
    initial macOS port, various other improvements
*   Travis Watkins ([@amaranth](https://github.com/amaranth)) -- support for
    windowless applications under macOS
*   [@ArEnSc](https://github.com/ArEnSc) / [@michael-chung](https://github.com/michael-chung) --
    initial implementation of windowless applications under macOS, macOS
    port improvements
*   Stefan Wasilewski ([@smw](https://github.com/smw)) -- macOS port
    improvements
*   Konstantinos Chatzilygeroudis ([@costashatz](https://github.com/costashatz)) --
    Debian package
*   Olga Turanksaya ([@olga-python](https://github.com/olga-python)) -- Gentoo
    ebuild
*   [@wivlaro](https://github.com/wivlaro) -- numerous bug reports, macOS
    fixes, feature improvements
*   Jonathan Hale ([@Squareys](https://github.com/Squareys)) -- Audio library
    enhancements, GlfwApplication implementation, frustum culling, bug reports,
    documentation improvements
*   Gerhard de Clercq -- Windows RT (Store/Phone) port
*   Ashwin Ravichandran ([@](ashrko619)[https://github.com/ashrko619]) --
    Bézier curve implementation
*   Alice Margatroid ([@Alicemargatroid](https://github.com/Alicemargatroid)) --
    Multi-channel support in Audio library, standard conformance, float,
    double, A-Law and μ-Law format support in `WavAudioImporter` plugin
*   [@sigman78](https://github.com/sigman78) -- Initial MSVC 2017 port

Big thanks to everyone involved!
