Third-party components
----------------------

-   Magnum makes use of the **OpenGL** and **WebGL** APIs --
    https://www.opengl.org/, https://www.khronos.org/webgl/
-   The `GL` and `Vk` namespaces internally use code generated using the
    **flextGL** extension loader generator -- https://github.com/mosra/flextgl.
    Copyright © 2011-2018 Thomas Weber, licensed under the
    [MIT license](https://raw.githubusercontent.com/mosra/flextgl/master/COPYING)
-   The `Audio` namespace depends on the **OpenAL** API --
    http://www.openal.org.
-   The `Platform::GlfwApplication` class uses the **GLFW** library --
    http://www.glfw.org/, licensed under the
    [zlib/libpng](http://www.glfw.org/license.html) license.
-   The `Platform::GlutApplication` class uses **freeGLUT** library --
    http://freeglut.sourceforge.net/, licensed under the MIT license.
-   The `Platform::Sdl2Application` class uses the **SDL2** library --
    https://www.libsdl.org/, licensed under [ZLIB license](http://www.gzip.org/zlib/zlib_license.html)

Contributors to Magnum library
------------------------------

-   Miguel Martin ([@miguelmartin75](https://github.com/miguelmartin75)) --
    initial macOS port, various other improvements
-   Travis Watkins ([@amaranth](https://github.com/amaranth)) -- support for
    windowless applications under macOS
-   [@ArEnSc](https://github.com/ArEnSc) / [@michael-chung](https://github.com/michael-chung) --
    initial implementation of windowless applications under macOS, macOS
    port improvements
-   Stefan Wasilewski ([@smw](https://github.com/smw)) -- macOS port
    improvements
-   Konstantinos Chatzilygeroudis ([@costashatz](https://github.com/costashatz)) --
    Debian package
-   Olga Turanksaya ([@olga-python](https://github.com/olga-python)) -- Gentoo
    ebuild
-   [@wivlaro](https://github.com/wivlaro) -- numerous bug reports, macOS
    fixes, feature improvements
-   Jonathan Hale ([@Squareys](https://github.com/Squareys)) -- Audio library
    enhancements, GlfwApplication implementation, frustum and cone culling, bug
    reports, documentation improvements
-   Gerhard de Clercq -- Windows RT (Store/Phone) port
-   Ashwin Ravichandran ([@](ashrko619)[https://github.com/ashrko619]) --
    Bézier curve implementation
-   Alice Margatroid ([@Alicemargatroid](https://github.com/Alicemargatroid)) --
    Multi-channel support in Audio library, standard conformance, float,
    double, A-Law and μ-Law format support in `WavAudioImporter` plugin
-   [@sigman78](https://github.com/sigman78) -- Initial MSVC 2017 port, Vcpkg
    packages
-   Alexander F Rødseth ([@xyproto](https://github.com/xyproto)) -- packages in
    the ArchLinux community repository
-   Nathan Ollerenshaw ([@matjam](https://github.com/matjam)) --- Ubuntu
    packages in a PPA repository

Big thanks to everyone involved!
