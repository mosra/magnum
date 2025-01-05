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
#include "Magnum/Math/Time.h"
#include "Magnum/Math/ConfigurationValue.h"
#include "Magnum/Platform/GlfwApplication.h"
#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/ImageData.h"

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
        _c(MouseButton6)
        _c(MouseButton7)
        _c(MouseButton8)
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
        Application::Pointer::MouseButton6,
        Application::Pointer::MouseButton7,
        Application::Pointer::MouseButton8,
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
        _c(Button4)
        _c(Button5)
        _c(Button6)
        _c(Button7)
        _c(Button8)
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
        _c(World1)
        _c(World2)
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

using namespace Containers::Literals;
using namespace Math::Literals;

struct GlfwApplicationTest: Platform::Application {
    explicit GlfwApplicationTest(const Arguments& arguments);

    /* For testing HiDPI resize events */
    void viewportEvent(ViewportEvent& event) override {
        Debug{} << "viewport:" << event.windowSize()
            #ifdef MAGNUM_TARGET_GL
            << event.framebufferSize()
            #endif
            << event.dpiScaling();
    }

    void exitEvent(ExitEvent& event) override {
        Debug{} << "application exiting";
        event.setAccepted(); /* Comment-out to test app exit suppression */
    }

    void drawEvent() override {
        Debug{} << "draw";
        swapBuffers();

        if(_redraw)
            redraw();
    }

    void keyPressEvent(KeyEvent& event) override {
        Debug{} << "key press:" << event.key() << int(event.key()) << event.keyName() << "scancode:" << event.scanCode() << event.modifiers()
            #if GLFW_VERSION_MAJOR*100 + GLFW_VERSION_MINOR >= 303
            << "converted:" << keyToScanCode(event.key())
            #endif
            ;

        if(event.key() == Key::F1) {
            Debug{} << "starting text input";
            startTextInput();
        } else if(event.key() == Key::F2) {
            _redraw = !_redraw;
            Debug{} << "redrawing" << (_redraw ? "enabled" : "disabled");
            if(_redraw) redraw();
        } else if(event.key() == Key::V) {
            _vsync = !_vsync;
            Debug{} << "vsync" << (_vsync? "on" : "off");
            setSwapInterval(_vsync ? 1 : 0);
        } else if(event.key() == Key::Esc) {
            Debug{} << "stopping text input";
            stopTextInput();
        } else if(event.key() == Key::T) {
            Debug{} << "setting window title";
            setWindowTitle("This is a UTF-8 Window Title™ and it should have no exclamation mark!!"_s.exceptSuffix(2));
        } else if(event.key() == Key::S) {
            Debug{} << "setting window size, which should trigger a viewport event";
            setWindowSize(Vector2i{300, 200});
        } else if(event.key() == Key::W) {
            Debug{} << "setting max window size, which should trigger a viewport event if the size changes";
            setMaxWindowSize(Vector2i{700, 500});
        } else if(event.key() == Key::H) {
            Debug{} << "toggling hand cursor";
            setCursor(cursor() == Cursor::Arrow ? Cursor::Hand : Cursor::Arrow);
        } else if(event.key() == Key::L) {
            Debug{} << "toggling locked mouse";
            setCursor(cursor() == Cursor::Arrow ? Cursor::HiddenLocked : Cursor::Arrow);
        } else if(event.key() == Key::X) {
            Debug{} << "requesting an exit with code 5";
            exit(5);
        }
    }

    void keyReleaseEvent(KeyEvent& event) override {
        Debug{} << "key release:" << event.key() << int(event.key()) << event.keyName() << "scancode:" << event.scanCode() << event.modifiers()
            #if GLFW_VERSION_MAJOR*100 + GLFW_VERSION_MINOR >= 303
            << "converted:" << keyToScanCode(event.key())
            #endif
            ;
    }

    /* Set to 0 to test the deprecated mouse events instead */
    #if 1
    void pointerPressEvent(PointerEvent& event) override {
        Debug{} << "pointer press:" << event.pointer() << event.modifiers() << Debug::packed << event.position();
    }
    void pointerReleaseEvent(PointerEvent& event) override {
        Debug{} << "pointer release:" << event.pointer() << event.modifiers() << Debug::packed << event.position();
    }
    void pointerMoveEvent(PointerMoveEvent& event) override {
        Debug{} << "pointer move:" << event.pointer() << event.pointers() << event.modifiers() << Debug::packed << event.position() << Debug::packed << event.relativePosition();
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

    void textInputEvent(TextInputEvent& event) override {
        Debug{} << "text input:" << event.text();
    }

    /* Uncomment to test the tick event. It should run at given minimal loop
       period even if not redrawing, it should not run at a different period
       when redrawing constantly. */
    #if 0
    void tickEvent() override {
        setMinimalLoopPeriod(250.0_msec);
        Debug{} << "tick event:" << Seconds{glfwGetTime()*1.0_sec};
    }
    #endif

    private:
        bool _redraw = false;
        bool _vsync = false;
};

GlfwApplicationTest::GlfwApplicationTest(const Arguments& arguments): Platform::Application{arguments, NoCreate} {
    Utility::Arguments args;
    args.addOption("dpi-scaling").setHelp("dpi-scaling", "DPI scaled passed via Configuration instead of --magnum-dpi-scaling, to test app overrides")
        .addSkippedPrefix("magnum", "engine-specific options")
        .addBooleanOption("exit-immediately").setHelp("exit-immediately", "exit the application immediately from the constructor, to test that the app doesn't run any event handlers after")
        .addBooleanOption("borderless").setHelp("borderless", "no window decoration")
        .addBooleanOption("always-on-top").setHelp("always-on-top", "always on top")
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
    if(args.isSet("borderless"))
        conf.addWindowFlags(Configuration::WindowFlag::Borderless);
    if(args.isSet("always-on-top"))
        conf.addWindowFlags(Configuration::WindowFlag::AlwaysOnTop);
    #ifdef MAGNUM_TARGET_GL
    GLConfiguration glConf;
    if(args.isSet("quiet"))
        glConf.addFlags(GLConfiguration::Flag::QuietLog);
    /* No GL-specific verbose log in GlfwApplication that we'd need to handle
       explicitly */
    if(args.isSet("gpu-validation"))
        glConf.addFlags(GLConfiguration::Flag::GpuValidation);
    create(conf, glConf);
    #else
    create(conf);
    #endif

    #ifdef MAGNUM_TARGET_GL
    Debug{} << "GL context flags:" << GL::Context::current().flags();
    #endif

    /* For testing resize events */
    Debug{} << "window size" << windowSize()
        #ifdef MAGNUM_TARGET_GL
        << framebufferSize()
        #endif
        << dpiScaling();

    Utility::Resource rs{"icons"};
    PluginManager::Manager<Trade::AbstractImporter> manager;
    Containers::Pointer<Trade::AbstractImporter> importer;
    Containers::Optional<Trade::ImageData2D> image16, image32, image64;
    if((importer = manager.loadAndInstantiate("AnyImageImporter")) &&
        importer->openData(rs.getRaw("icon-16.tga")) && (image16 = importer->image2D(0)) &&
        importer->openData(rs.getRaw("icon-32.tga")) && (image32 = importer->image2D(0)) &&
        importer->openData(rs.getRaw("icon-64.tga")) && (image64 = importer->image2D(0))) setWindowIcon({*image16, *image32, *image64});
    else Warning{} << "Can't load the plugin / images, not setting window icon";
}

}}}}

MAGNUM_APPLICATION_MAIN(Magnum::Platform::Test::GlfwApplicationTest)
