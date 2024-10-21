/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2018, 2019 Jonathan Hale <squareys@googlemail.com>
    Copyright © 2020 Pablo Escobar <mail@rvrs.in>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#include <Corrade/Containers/EnumSet.hpp>
#include <Corrade/Utility/Arguments.h>
#include <emscripten/html5.h>

#include "Magnum/Platform/EmscriptenApplication.h"
#include "Magnum/GL/Renderer.h"
#include "Magnum/GL/DefaultFramebuffer.h"
#include "Magnum/GL/Mesh.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/ConfigurationValue.h"

/* The __EMSCRIPTEN_major__ etc macros used to be passed implicitly, version
   3.1.4 moved them to a version header and version 3.1.23 dropped the
   backwards compatibility. To work consistently on all versions, including the
   header only if the version macros aren't present.
   https://github.com/emscripten-core/emscripten/commit/f99af02045357d3d8b12e63793cef36dfde4530a
   https://github.com/emscripten-core/emscripten/commit/f76ddc702e4956aeedb658c49790cc352f892e4c */
#if defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(__EMSCRIPTEN_major__)
#include <emscripten/version.h>
#endif

namespace Magnum { namespace Platform {

/* These cannot be in an anonymous namespace as enumSetDebugOutput() below
   wouldn't be able to pick them up */

static Debug& operator<<(Debug& debug, Application::Modifier value) {
    debug << "Modifier" << Debug::nospace;

    switch(value) {
        #define _c(value) case Application::Modifier::value: return debug << "::" #value;
        _c(Shift)
        _c(Ctrl)
        _c(Alt)
        _c(Super)
        #undef _c
    }

    return debug << "(" << Debug::nospace << UnsignedInt(value) << Debug::nospace << ")";
}

static Debug& operator<<(Debug& debug, Application::Pointer value) {
    debug << "Pointer" << Debug::nospace;

    switch(value) {
        #define _c(value) case Application::Pointer::value: return debug << "::" #value;
        _c(MouseLeft)
        _c(MouseMiddle)
        _c(MouseRight)
        _c(MouseButton4)
        _c(MouseButton5)
        #if __EMSCRIPTEN_major__*10000 + __EMSCRIPTEN_minor__*100 + __EMSCRIPTEN_tiny__ >= 20027
        _c(Finger)
        #endif
        #undef _c
    }

    return debug << "(" << Debug::nospace << UnsignedInt(value) << Debug::nospace << ")";
}

#ifdef MAGNUM_BUILD_DEPRECATED
CORRADE_IGNORE_DEPRECATED_PUSH
CORRADE_UNUSED static Debug& operator<<(Debug& debug, Application::MouseMoveEvent::Button value) {
    debug << "Button" << Debug::nospace;

    switch(value) {
        #define _c(value) case Application::MouseMoveEvent::Button::value: return debug << "::" #value;
        _c(Left)
        _c(Middle)
        _c(Right)
        #undef _c
    }

    return debug << "(" << Debug::nospace << UnsignedInt(value) << Debug::nospace << ")";
}
CORRADE_IGNORE_DEPRECATED_POP
#endif

namespace Test { namespace {

static Debug& operator<<(Debug& debug, Application::PointerEventSource value) {
    debug << "PointerEventSource" << Debug::nospace;

    switch(value) {
        #define _c(value) case Application::PointerEventSource::value: return debug << "::" #value;
        _c(Mouse)
        #if __EMSCRIPTEN_major__*10000 + __EMSCRIPTEN_minor__*100 + __EMSCRIPTEN_tiny__ >= 20027
        _c(Touch)
        #endif
        #undef _c
    }

    return debug << "(" << Debug::nospace << UnsignedInt(value) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, Application::Modifiers value) {
    return Containers::enumSetDebugOutput(debug, value, "Modifiers{}", {
        Application::Modifier::Shift,
        Application::Modifier::Ctrl,
        Application::Modifier::Alt,
        Application::Modifier::Super
    });
}

Debug& operator<<(Debug& debug, Application::Pointers value) {
    return Containers::enumSetDebugOutput(debug, value, "Pointers{}", {
        Application::Pointer::MouseLeft,
        Application::Pointer::MouseMiddle,
        Application::Pointer::MouseRight,
        Application::Pointer::MouseButton4,
        Application::Pointer::MouseButton5,
        #if __EMSCRIPTEN_major__*10000 + __EMSCRIPTEN_minor__*100 + __EMSCRIPTEN_tiny__ >= 20027
        Application::Pointer::Finger,
        #endif
    });
}

#ifdef MAGNUM_BUILD_DEPRECATED
CORRADE_IGNORE_DEPRECATED_PUSH
CORRADE_UNUSED Debug& operator<<(Debug& debug, Application::MouseEvent::Button value) {
    debug << "Button" << Debug::nospace;

    switch(value) {
        #define _c(value) case Application::MouseEvent::Button::value: return debug << "::" #value;
        _c(Left)
        _c(Middle)
        _c(Right)
        #undef _c
    }

    return debug << "(" << Debug::nospace << UnsignedInt(value) << Debug::nospace << ")";
}

CORRADE_UNUSED Debug& operator<<(Debug& debug, Application::MouseMoveEvent::Buttons value) {
    return Containers::enumSetDebugOutput(debug, value, "Buttons{}", {
        Application::MouseMoveEvent::Button::Left,
        Application::MouseMoveEvent::Button::Middle,
        Application::MouseMoveEvent::Button::Right,
    });
}
CORRADE_IGNORE_DEPRECATED_POP
#endif

Debug& operator<<(Debug& debug, const Application::Key value) {
    debug << "Key" << Debug::nospace;

    switch(value) {
        #define _c(value) case Application::Key::value: return debug << "::" #value;
        _c(Unknown)
        _c(LeftShift)
        _c(RightShift)
        _c(LeftCtrl)
        _c(RightCtrl)
        _c(LeftAlt)
        _c(RightAlt)
        _c(LeftSuper)
        _c(RightSuper)
        _c(Enter)
        _c(Esc)
        _c(Up)
        _c(Down)
        _c(Left)
        _c(Right)
        _c(Home)
        _c(End)
        _c(PageUp)
        _c(PageDown)
        _c(Backspace)
        _c(Insert)
        _c(Delete)
        _c(F1)
        _c(F2)
        _c(F3)
        _c(F4)
        _c(F5)
        _c(F6)
        _c(F7)
        _c(F8)
        _c(F9)
        _c(F10)
        _c(F11)
        _c(F12)
        _c(Zero)
        _c(One)
        _c(Two)
        _c(Three)
        _c(Four)
        _c(Five)
        _c(Six)
        _c(Seven)
        _c(Eight)
        _c(Nine)
        _c(A)
        _c(B)
        _c(C)
        _c(D)
        _c(E)
        _c(F)
        _c(G)
        _c(H)
        _c(I)
        _c(J)
        _c(K)
        _c(L)
        _c(M)
        _c(N)
        _c(O)
        _c(P)
        _c(Q)
        _c(R)
        _c(S)
        _c(T)
        _c(U)
        _c(V)
        _c(W)
        _c(X)
        _c(Y)
        _c(Z)
        _c(Space)
        _c(Tab)
        _c(Quote)
        _c(Comma)
        _c(Period)
        _c(Minus)
        _c(Plus)
        _c(Slash)
        _c(Percent)
        _c(Semicolon)
        _c(Equal)
        _c(LeftBracket)
        _c(RightBracket)
        _c(Backslash)
        _c(Backquote)
        _c(CapsLock)
        _c(ScrollLock)
        _c(NumLock)
        _c(PrintScreen)
        _c(Pause)
        _c(Menu)
        _c(NumZero)
        _c(NumOne)
        _c(NumTwo)
        _c(NumThree)
        _c(NumFour)
        _c(NumFive)
        _c(NumSix)
        _c(NumSeven)
        _c(NumEight)
        _c(NumNine)
        _c(NumDecimal)
        _c(NumDivide)
        _c(NumMultiply)
        _c(NumSubtract)
        _c(NumAdd)
        _c(NumEnter)
        _c(NumEqual)
        #undef _c
    }

    return debug << "(" << Debug::nospace << Debug::hex << UnsignedInt(value) << Debug::nospace << ")";
}

using namespace Containers::Literals;
using namespace Math::Literals;

struct EmscriptenApplicationTest: Platform::Application {
    /* For testing resize events */
    explicit EmscriptenApplicationTest(const Arguments& arguments);

    void drawEvent() override {
        Debug() << "draw event";
        #ifdef CUSTOM_CLEAR_COLOR
        GL::Renderer::setClearColor(CUSTOM_CLEAR_COLOR);
        #endif
        GL::defaultFramebuffer.clear(GL::FramebufferClear::Color);

        swapBuffers();

        if(_redraw)
            redraw();
    }

    #ifdef MAGNUM_TARGET_GL
    /* For testing HiDPI resize events */
    void viewportEvent(ViewportEvent& event) override {
        Debug{} << "viewport:" << event.windowSize() << event.framebufferSize() << event.dpiScaling() << event.devicePixelRatio();
    }
    #endif

    /* Set to 0 to test the deprecated mouse events instead */
    #if 1
    void pointerPressEvent(PointerEvent& event) override {
        Debug{} << "pointer press:" << event.source() << event.pointer() << (event.isPrimary() ? "primary" : "secondary") << event.id() << event.modifiers() << Debug::packed << event.position()
            #if __EMSCRIPTEN_major__*10000 + __EMSCRIPTEN_minor__*100 + __EMSCRIPTEN_tiny__ >= 20027
            /* Just to verify the access works for both cases */
            << (event.source() == PointerEventSource::Mouse ?
                event.event<EmscriptenMouseEvent>().timestamp :
                event.event<EmscriptenTouchEvent>().timestamp)
            #endif
            ;
    }
    void pointerReleaseEvent(PointerEvent& event) override {
        Debug{} << "pointer release:" << event.source() << event.pointer() << (event.isPrimary() ? "primary" : "secondary") << event.id() << event.modifiers() << Debug::packed << event.position()
            #if __EMSCRIPTEN_major__*10000 + __EMSCRIPTEN_minor__*100 + __EMSCRIPTEN_tiny__ >= 20027
            /* Just to verify the access works for both cases */
            << (event.source() == PointerEventSource::Mouse ?
                event.event<EmscriptenMouseEvent>().timestamp :
                event.event<EmscriptenTouchEvent>().timestamp)
            #endif
            ;
    }
    void pointerMoveEvent(PointerMoveEvent& event) override {
        Debug{} << "pointer move:" << event.source() << event.pointer() << event.pointers() << (event.isPrimary() ? "primary" : "secondary") << event.id() << event.modifiers() << Debug::packed << event.position() << Debug::packed << event.relativePosition()
            #if __EMSCRIPTEN_major__*10000 + __EMSCRIPTEN_minor__*100 + __EMSCRIPTEN_tiny__ >= 20027
            /* Just to verify the access works for both cases */
            << (event.source() == PointerEventSource::Mouse ?
                event.event<EmscriptenMouseEvent>().timestamp :
                event.event<EmscriptenTouchEvent>().timestamp)
            #endif
            ;
    }
    void scrollEvent(ScrollEvent& event) override {
        Debug{} << "scroll:" << event.modifiers() << Debug::packed << event.offset() << Debug::packed << event.position();
    }
    #else
    CORRADE_IGNORE_DEPRECATED_PUSH
    void mousePressEvent(MouseEvent& event) override {
        Debug{} << "mouse press:" << event.button() << event.modifiers() << Debug::packed << event.position();
    }
    void mouseReleaseEvent(MouseEvent& event) override {
        Debug{} << "mouse release:" << event.button() << event.modifiers() << Debug::packed << event.position();
    }
    void mouseMoveEvent(MouseMoveEvent& event) override {
        Debug{} << "mouse move:" << event.buttons() << event.modifiers() << Debug::packed << event.position() << Debug::packed << event.relativePosition();
    }
    void mouseScrollEvent(MouseScrollEvent& event) override {
        Debug{} << "mouse scroll:" << event.modifiers() << Debug::packed << event.offset() << Debug::packed << event.position();
    }
    CORRADE_IGNORE_DEPRECATED_POP
    #endif

    /* For testing keyboard capture */
    void keyPressEvent(KeyEvent& event) override {
        Debug{} << "key press:" << event.key() << event.keyName() << "scancode:" << event.scanCodeName() << event.modifiers();

        if(event.key() == Key::F1) {
            Debug{} << "starting text input";
            startTextInput();
        } else if(event.key() == Key::F2) {
            _redraw = !_redraw;
            Debug{} << "redrawing" << (_redraw ? "enabled" : "disabled");
            if(_redraw) redraw();
        } else if(event.key() == Key::Esc) {
            Debug{} << "stopping text input";
            stopTextInput();
        } else if(event.key() == Key::F) {
            Debug{} << "toggling fullscreen";
            setContainerCssClass((_fullscreen ^= true) ? "mn-fullsizeX"_s.exceptSuffix(1) : "");
        } else if(event.key() == Key::T) {
            Debug{} << "setting window title";
            setWindowTitle("This is a UTF-8 Window Title™ and it should have no exclamation mark!!"_s.exceptSuffix(2));
        } else if(event.key() == Key::H) {
            Debug{} << "toggling hand cursor";
            setCursor(cursor() == Cursor::Arrow ? Cursor::Hand : Cursor::Arrow);
        }

        event.setAccepted();
    }

    void keyReleaseEvent(KeyEvent& event) override {
        Debug{} << "key release:" << event.key() << event.keyName() << "scancode:" << event.scanCodeName() << event.modifiers();

        event.setAccepted();
    }

    void textInputEvent(TextInputEvent& event) override {
        Debug{} << "text input:" << event.text();

        event.setAccepted();
    }

    private:
        bool _fullscreen = false;
        bool _redraw = false;
};

EmscriptenApplicationTest::EmscriptenApplicationTest(const Arguments& arguments): Platform::Application{arguments, NoCreate} {
    Utility::Arguments args;
    args.addOption("dpi-scaling").setHelp("dpi-scaling", "DPI scaled passed via Configuration instead of --magnum-dpi-scaling, to test app overrides")
        .addSkippedPrefix("magnum", "engine-specific options")
        .addBooleanOption("exit-immediately").setHelp("exit-immediately", "exit the application immediately from the constructor, to test that the app doesn't run any event handlers after")
        .addBooleanOption("quiet").setHelp("quiet", "like --magnum-log quiet, but specified via a Context::Configuration instead")
        .parse(arguments.argc, arguments.argv);

    /* Useful for bisecting Emscripten regressions, because they happen WAY TOO
       OFTEN!!! */
    Debug{} << "Emscripten version:"
        << __EMSCRIPTEN_major__ << Debug::nospace << "." << Debug::nospace
        << __EMSCRIPTEN_minor__ << Debug::nospace << "." << Debug::nospace
        << __EMSCRIPTEN_tiny__ << Debug::nospace;

    if(args.isSet("exit-immediately")) {
        exit();
        return;
    }

    Configuration conf;
    conf.setWindowFlags(Configuration::WindowFlag::Resizable);
    if(!args.value("dpi-scaling").empty())
        conf.setSize({640, 480}, args.value<Vector2>("dpi-scaling"));
    GLConfiguration glConf;
    if(args.isSet("quiet"))
        glConf.addFlags(GLConfiguration::Flag::QuietLog);
    /* No GL-specific verbose log in EmscriptenApplication that we'd need to
       handle explicitly */
    /* No GPU validation on WebGL */
    create(conf, glConf);

    Debug{} << "window size" << windowSize()
        #ifdef MAGNUM_TARGET_GL
        << framebufferSize()
        #endif
        << dpiScaling() << devicePixelRatio();

    /* This uses a VAO on WebGL 1, so it will crash in case GL flags are
       missing EnableExtensionsByDefault (uncomment above) */
    GL::Mesh mesh;
}

}}}}

MAGNUM_APPLICATION_MAIN(Magnum::Platform::Test::EmscriptenApplicationTest)
