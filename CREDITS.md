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

Listing only people with code contributions, because otherwise there's too many
:) Big thanks to everyone involved!

-   Alan ([@Alan-FGR](https://github.com/Alan-FGR)) -- documentation fixes
-   Alexander F Rødseth ([@xyproto](https://github.com/xyproto)) -- packages in
    the ArchLinux community repository
-   Alexey Yurchenko ([@alexesDev](https://github.com/alexesDev)) -- static
    analyis, bug reports
-   Alice Margatroid ([@Alicemargatroid](https://github.com/Alicemargatroid)) --
    Multi-channel support in Audio library, standard conformance, float,
    double, A-Law and μ-Law format support in `WavAudioImporter` plugin
-   Andy Somogyi ([@andysomogyi](https://github.com/andysomogyi)) -- bugreports,
    API design improvement suggestions
-   [@ArEnSc](https://github.com/ArEnSc) -- initial implementation of
    windowless applications under macOS, macOS port improvements
-   Ashwin Ravichandran ([@ashrko619](https://github.com/ashrko619)) --
    Bézier curve implementation
-   Bill Robinson ([@wivlaro](https://github.com/wivlaroú) -- numerous bug
    reports, macOS fixes, feature improvements
-   [@biosek](https://github.com/biosek)) -- documentation fixes
-   Denis Igorevich Lobanov ([@denislobanov](https://github.com/denislobanov) --
    math fixes
-   [@dlardi](https://github.com/dlardi)) -- WGL-related bugfixes
-   Eliot Saba ([@staticfloat](https://github.com/staticfloat)) -- macOS-related
    GLFW updates
-   Émile Grégoire ([@emgre](https://github.com/emgre)) -- documentation
    updates, algorithm bugfixes
-   Gerhard de Clercq -- Windows RT (Store/Phone) port
-   Guillaume Giraud ([@Guillaume227](https://github.com/Guillaume227)) --
    algorithm bugfixes
-   Ivan P. ([@uzername](https://github.com/uzername)) --- documentation
    improvements
-   Jonathan Hale ([@Squareys](https://github.com/Squareys)) -- Audio library
    enhancements, GlfwApplication implementation, frustum and cone culling, bug
    reports, documentation improvements
-   Joel Clay ([@jclay](https://github.com/jclay)) --- Vcpkg packages
-   Konstantinos Chatzilygeroudis ([@costashatz](https://github.com/costashatz))
    -- Debian package
-   Krzysztof Szenk ([@Crisspl](https://github.com/Crisspl)) -- math fixes
-   Leon Moctezuma ([@leonidax](https://github.com/leonidax)) -- bugreports,
    API design direction
-   Miguel Martin ([@miguelmartin75](https://github.com/miguelmartin75)) --
    initial macOS port, various other improvements
-   Nathan Ollerenshaw ([@matjam](https://github.com/matjam)) --- Ubuntu
    packages in a PPA repository
-   Nicholas "LB" Branden ([@LB--](https://github.com/LB--)) -- warning fixes,
    Windows buildsystem improvements
-   Olga Turanksaya ([@olga-python](https://github.com/olga-python)) -- Gentoo
    ebuild
-   Sam Spilsbury ([@smspillaz](https://github.com/smspillaz)) -- WebGL and
    GLES fixes
-   Samuel Kogler ([@skogler](https://github.com/skogler)) -- GLFW application
    crash fixes
-   Séverin Lemaignan ([@severin-lemaignan](severin-lemaignan/)) -- lots of
    feedback and bug reports for the initial releases
-   [@sigman78](https://github.com/sigman78) -- Initial MSVC 2017 port, Vcpkg
    packages
-   Siim Kallas ([@seemk](https://github.com/seemk)) -- Sdl2Application
    improvements, bugfixes
-   Steeve Morin ([@steeve](https://github.com/steeve)) -- iOS buildsystem
    improvements
-   Stefan Wasilewski ([@smw](https://github.com/smw)) -- macOS port
    improvements
-   Thomas Tissot-Dupont ([@dolphineye](https://github.com/dolphineye)) ---
    OpenGL ES compatiblity improvements
-   Travis Watkins ([@amaranth](https://github.com/amaranth)) -- support for
    windowless applications under macOS

Is this list missing your name or something's wrong?
[Let us know!](https://magnum.graphics/contact/)
