/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
              Vladimír Vondruš <mosra@centrum.cz>

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
#include <Corrade/Containers/Optional.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/Arguments.h>
#include <Corrade/Utility/Resource.h>

#include "Magnum/ImageView.h"
#include "Magnum/Math/ConfigurationValue.h"
#include "Magnum/Math/Time.h"
#include "Magnum/Platform/Sdl2Application.h"
#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/ImageData.h"

#ifdef CORRADE_TARGET_CLANG_CL
/* SDL does #pragma pack(push,8) and #pragma pack(pop,8) in different headers
   (begin_code.h and end_code.h) and clang-cl doesn't like that, even though it
   is completely fine. Silence the warning. */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpragma-pack"
#endif
#include <SDL_events.h>
#ifdef CORRADE_TARGET_CLANG_CL
#pragma clang diagnostic pop
#endif
#include <SDL_timer.h>

#ifdef MAGNUM_TARGET_GL
#include "Magnum/GL/DefaultFramebuffer.h"
#endif

namespace Magnum { namespace Platform {

/* These cannot be in an anonymous namespace as enumSetDebugOutput() /
   Optional<Key> below wouldn't be able to pick them up */

static Debug& operator<<(Debug& debug, Application::Modifier value) {
    debug << "Modifier" << Debug::nospace;

    switch(value) {
        #define _c(value) case Application::Modifier::value: return debug << "::" #value;
        _c(Shift)
        _c(Ctrl)
        _c(Alt)
        _c(Super)
        _c(AltGr)
        _c(CapsLock)
        _c(NumLock)
        #undef _c
    }

    return debug << "(" << Debug::nospace << UnsignedInt(value) << Debug::nospace << ")";
}

static Debug& operator<<(Debug& debug, Application::Key value) {
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
        _c(AltGr)
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
        #ifndef CORRADE_TARGET_EMSCRIPTEN
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
        _c(X1)
        _c(X2)
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
        #ifndef CORRADE_TARGET_EMSCRIPTEN
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
        Application::Modifier::Super,
        Application::Modifier::AltGr,
        Application::Modifier::CapsLock,
        Application::Modifier::NumLock,
    });
}

Debug& operator<<(Debug& debug, Application::Pointers value) {
    return Containers::enumSetDebugOutput(debug, value, "Pointers{}", {
        Application::Pointer::MouseLeft,
        Application::Pointer::MouseMiddle,
        Application::Pointer::MouseRight,
        Application::Pointer::MouseButton4,
        Application::Pointer::MouseButton5,
        #ifndef CORRADE_TARGET_EMSCRIPTEN
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
        _c(X1)
        _c(X2)
        #undef _c
    }

    return debug << "(" << Debug::nospace << UnsignedInt(value) << Debug::nospace << ")";
}

CORRADE_UNUSED Debug& operator<<(Debug& debug, Application::MouseMoveEvent::Buttons value) {
    return Containers::enumSetDebugOutput(debug, value, "Buttons{}", {
        Application::MouseMoveEvent::Button::Left,
        Application::MouseMoveEvent::Button::Middle,
        Application::MouseMoveEvent::Button::Right,
        Application::MouseMoveEvent::Button::X1,
        Application::MouseMoveEvent::Button::X2,
    });
}
CORRADE_IGNORE_DEPRECATED_POP
#endif

using namespace Containers::Literals;
using namespace Math::Literals;

struct Sdl2ApplicationTest: Platform::Application {
    explicit Sdl2ApplicationTest(const Arguments& arguments);

    void exitEvent(ExitEvent& event) override {
        Debug{} << "application exiting";
        event.setAccepted(); /* Comment-out to test app exit suppression */
    }

    /* For testing HiDPI resize events */
    void viewportEvent(ViewportEvent& event) override {
        Debug{} << "viewport event" << event.windowSize()
            #ifdef MAGNUM_TARGET_GL
            << event.framebufferSize()
            #endif
            << event.dpiScaling();
    }

    void drawEvent() override {
        Debug{} << "draw event";
        #ifdef MAGNUM_TARGET_GL
        GL::defaultFramebuffer.clear(GL::FramebufferClear::Color);
        #endif

        swapBuffers();

        if(_redraw)
            redraw();
    }

    /* Set to 0 to test the deprecated mouse events instead */
    #if 1
    void pointerPressEvent(PointerEvent& event) override {
        Debug{} << "pointer press:" << event.source() << event.pointer() << (event.isPrimary() ? "primary" : "secondary") << event.id() << event.modifiers() << Debug::packed << event.position();
    }
    void pointerReleaseEvent(PointerEvent& event) override {
        Debug{} << "pointer release:" << event.source() << event.pointer() << (event.isPrimary() ? "primary" : "secondary") << event.id() << event.modifiers() << Debug::packed << event.position();
    }
    void pointerMoveEvent(PointerMoveEvent& event) override {
        Debug{} << "pointer move:" << event.source() << event.pointer() << event.pointers() << (event.isPrimary() ? "primary" : "secondary") << event.id() << event.modifiers() << Debug::packed << event.position() << Debug::packed << event.relativePosition();
    }
    void scrollEvent(ScrollEvent& event) override {
        Debug{} << "scroll:" << event.modifiers() << Debug::packed << event.offset() << Debug::packed << event.position();
    }
    #else
    CORRADE_IGNORE_DEPRECATED_PUSH
    void mousePressEvent(MouseEvent& event) override {
        Debug{} << "mouse press:" << event.button() << Debug::packed << event.position() << event.modifiers();
    }
    void mouseReleaseEvent(MouseEvent& event) override {
        Debug{} << "mouse release:" << event.button() << Debug::packed << event.position() << event.modifiers();
    }
    void mouseMoveEvent(MouseMoveEvent& event) override {
        Debug{} << "mouse move:" << event.buttons() << Debug::packed << event.position() << Debug::packed << event.relativePosition() << event.modifiers();
    }
    void mouseScrollEvent(MouseScrollEvent& event) override {
        Debug{} << "mouse scroll:" << event.modifiers() << Debug::packed << event.offset() << Debug::packed << event.position();
    }
    CORRADE_IGNORE_DEPRECATED_POP
    #endif

    void keyPressEvent(KeyEvent& event) override {
        Debug{} << "key press:" << event.key() << event.keyName() << "scancode:" << event.scanCode()
            #ifndef CORRADE_TARGET_EMSCRIPTEN
            << event.scanCodeName()
            #endif
            << event.modifiers()
            #if !defined(CORRADE_TARGET_EMSCRIPTEN) || __EMSCRIPTEN_major__*10000 + __EMSCRIPTEN_minor__*100 + __EMSCRIPTEN_tiny__ >= 30125
            << "converted:"
            #ifndef CORRADE_TARGET_EMSCRIPTEN
            << scanCodeToKey(event.scanCode())
            #endif
            << keyToScanCode(event.key())
            #endif
            ;

        if(event.key() == Key::F1) {
            Debug{} << "starting text input";
            startTextInput();
        } else if(event.key() == Key::F2) {
            _redraw = !_redraw;
            Debug{} << "redrawing" << (_redraw ? "enabled" : "disabled");
            if(_redraw) redraw();
        }
        #ifndef CORRADE_TARGET_EMSCRIPTEN
        else if(event.key() == Key::V) {
            _vsync = !_vsync;
            Debug{} << "vsync" << (_vsync? "on" : "off");
            setSwapInterval(_vsync ? 1 : 0);
        }
        #endif
        else if(event.key() == Key::Esc) {
            Debug{} << "stopping text input";
            stopTextInput();
        } else if(event.key() == Key::T) {
            Debug{} << "setting window title";
            setWindowTitle("This is a UTF-8 Window Title™ and it should have no exclamation mark!!"_s.exceptSuffix(2));
        }
        #ifndef CORRADE_TARGET_EMSCRIPTEN
        else if(event.key() == Key::S) {
            Debug{} << "setting window size, which should trigger a viewport event";
            setWindowSize(Vector2i{300, 200});
        } else if(event.key() == Key::W) {
            Debug{} << "setting max window size, which should trigger a viewport event";
            setMaxWindowSize(Vector2i{700, 500});
        }
        #endif
        else if(event.key() == Key::H) {
            Debug{} << "toggling hand cursor";
            setCursor(cursor() == Cursor::Arrow ? Cursor::Hand : Cursor::Arrow);
        }
        #ifndef CORRADE_TARGET_EMSCRIPTEN
        else if(event.key() == Key::L) {
            Debug{} << "toggling locked mouse";
            setCursor(cursor() == Cursor::Arrow ? Cursor::HiddenLocked : Cursor::Arrow);
        }
        #else
        else if(event.key() == Key::F) {
            Debug{} << "toggling fullscreen";
            setContainerCssClass((_fullscreen ^= true) ? "mn-fullsize" : "");
        }
        #endif
        else if(event.key() == Key::X) {
            Debug{} << "requesting an exit with code 5";
            exit(5);
        }

        /* With EmscriptenApplication, this makes the event stop from
           propagating further to the page (such as when pressing F1).
           Unfortunately on SDL this doesn't as there's no API to actually mark
           events as accepted. */
        event.setAccepted();
    }

    void keyReleaseEvent(KeyEvent& event) override {
        Debug{} << "key release:" << event.key() << event.keyName() << "scancode:" << event.scanCode()
            #ifndef CORRADE_TARGET_EMSCRIPTEN
            << event.scanCodeName()
            #endif
            << event.modifiers()
            #if !defined(CORRADE_TARGET_EMSCRIPTEN) || __EMSCRIPTEN_major__*10000 + __EMSCRIPTEN_minor__*100 + __EMSCRIPTEN_tiny__ >= 30125
            << "converted:"
            #ifndef CORRADE_TARGET_EMSCRIPTEN
            << scanCodeToKey(event.scanCode())
            #endif
            << keyToScanCode(event.key())
            #endif
            ;

        /* With EmscriptenApplication, this makes the event stop from
           propagating further to the page (such as when pressing F1).
           Unfortunately on SDL this doesn't as there's no API to actually mark
           events as accepted. */
        event.setAccepted();
    }

    void textInputEvent(TextInputEvent& event) override {
        Debug{} << "text input event:" << event.text();

        /* With EmscriptenApplication, this makes the event stop from
           propagating further to the page (such as when pressing F1).
           Unfortunately on SDL this doesn't as there's no API to actually mark
           events as accepted. */
        event.setAccepted();
    }

    /* Should fire on currently not handled events, such as minimize/maximize
       or window focus/blur. Comment out to verify correct behavior with the
       override not present. */
    void anyEvent(SDL_Event& event) override {
        Debug d;
        d << "any event:" << event.type;
        if(event.type == SDL_WINDOWEVENT) d << event.window.event;
    }

    /* Uncomment to test the tick event. It should run at given minimal loop
       period even if not redrawing, it should not run at a different period
       when redrawing constantly. */
    #if 0
    void tickEvent() override {
        setMinimalLoopPeriod(250.0_msec);
        Debug{} << "tick event:" << Seconds{SDL_GetTicks()*1.0_msec};
    }
    #endif

    private:
        #ifdef CORRADE_TARGET_EMSCRIPTEN
        bool _fullscreen = false;
        #endif
        bool _redraw = false;
        #ifndef CORRADE_TARGET_EMSCRIPTEN
        bool _vsync = false;
        #endif
};

Sdl2ApplicationTest::Sdl2ApplicationTest(const Arguments& arguments): Platform::Application{arguments, NoCreate} {
    Utility::Arguments args;
    args.addOption("dpi-scaling").setHelp("dpi-scaling", "DPI scaled passed via Configuration instead of --magnum-dpi-scaling, to test app overrides")
        .addSkippedPrefix("magnum", "engine-specific options")
        .addBooleanOption("exit-immediately").setHelp("exit-immediately", "exit the application immediately from the constructor, to test that the app doesn't run any event handlers after")
        #ifndef CORRADE_TARGET_EMSCRIPTEN
        .addBooleanOption("borderless").setHelp("borderless", "no window decoration")
        #if SDL_MAJOR_VERSION*1000 + SDL_MINOR_VERSION*100 + SDL_PATCHLEVEL >= 2005
        .addBooleanOption("always-on-top").setHelp("always-on-top", "always on top")
        #endif
        #endif
        #ifdef MAGNUM_TARGET_GL
        .addBooleanOption("quiet").setHelp("quiet", "like --magnum-log quiet, but specified via a Context::Configuration instead")
        .addBooleanOption("gpu-validation").setHelp("gpu-validation", "like --magnum-gpu-validation, but specified via a Context::Configuration instead")
        #endif
        .parse(arguments.argc, arguments.argv);

    if(args.isSet("exit-immediately")) {
        exit();
        return;
    }

    Configuration conf;
    conf.setTitle("Window title that should have no exclamation mark!!"_s.exceptSuffix(2))
        .setWindowFlags(Configuration::WindowFlag::Resizable);
    if(!args.value("dpi-scaling").empty())
        conf.setSize({800, 600}, args.value<Vector2>("dpi-scaling"));
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    if(args.isSet("borderless"))
        conf.addWindowFlags(Configuration::WindowFlag::Borderless);
    #if SDL_MAJOR_VERSION*1000 + SDL_MINOR_VERSION*100 + SDL_PATCHLEVEL >= 2005
    if(args.isSet("always-on-top"))
        conf.addWindowFlags(Configuration::WindowFlag::AlwaysOnTop);
    #endif
    #endif
    #ifdef MAGNUM_TARGET_GL
    GLConfiguration glConf;
    if(args.isSet("quiet"))
        glConf.addFlags(GLConfiguration::Flag::QuietLog);
    /* No GL-specific verbose log in Sdl2Application that we'd need to handle
       explicitly */
    if(args.isSet("gpu-validation"))
        glConf.addFlags(GLConfiguration::Flag::GpuValidation);
    create(conf, glConf);
    #else
    create(conf);
    #endif

    #if defined(MAGNUM_TARGET_GL) && !defined(MAGNUM_TARGET_WEBGL)
    Debug{} << "GL context flags:" << GL::Context::current().flags();
    #endif

    /* For testing resize events */
    Debug{} << "window size" << windowSize()
        #ifdef MAGNUM_TARGET_GL
        << framebufferSize()
        #endif
        << dpiScaling();

    #ifndef CORRADE_TARGET_EMSCRIPTEN
    #if SDL_MAJOR_VERSION*1000 + SDL_MINOR_VERSION*100 + SDL_PATCHLEVEL >= 2005
    Utility::Resource rs{"icons"};
    PluginManager::Manager<Trade::AbstractImporter> manager;
    Containers::Pointer<Trade::AbstractImporter> importer;
    Containers::Optional<Trade::ImageData2D> image;
    if((importer = manager.loadAndInstantiate("AnyImageImporter")) &&
       importer->openData(rs.getRaw("icon-64.tga")) &&
       (image = importer->image2D(0))) setWindowIcon(*image);
    else Warning{} << "Can't load the plugin / file, not setting window icon";
    #else
    Debug{} << "SDL too old, can't set window icon";
    #endif
    #endif
}

}}}}

MAGNUM_APPLICATION_MAIN(Magnum::Platform::Test::Sdl2ApplicationTest)
