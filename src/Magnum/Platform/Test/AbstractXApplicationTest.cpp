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
#include <Corrade/Utility/Arguments.h>

#ifdef BUILD_GLXAPPLICATION
#include "Magnum/Platform/GlxApplication.h"
#elif defined(BUILD_XEGLAPPLICATION)
#include "Magnum/Platform/XEglApplication.h"
#else
#error
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
        _c(AltGr)
        _c(CapsLock)
        _c(NumLock)
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
        _c(WheelUp)
        _c(WheelDown)
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

struct AbstractXApplicationTest: Platform::Application {
    explicit AbstractXApplicationTest(const Arguments& arguments);

    void drawEvent() override {
        Debug{} << "draw event";
        swapBuffers();
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
        Debug{} << "pointer move:" << event.pointer() << event.pointers() << event.modifiers() << Debug::packed << event.position();
    }
    #else
    CORRADE_IGNORE_DEPRECATED_PUSH
    void mousePressEvent(MouseEvent& event) override {
        Debug{} << "mouse press:" << event.button() << event.buttons() << event.modifiers() << Debug::packed << event.position();
    }
    void mouseReleaseEvent(MouseEvent& event) override {
        Debug{} << "mouse release:" << event.button() << event.buttons() << event.modifiers() << Debug::packed << event.position();
    }
    void mouseMoveEvent(MouseMoveEvent& event) override {
        Debug{} << "mouse move:" << event.buttons() << event.modifiers() << Debug::packed << event.position();
    }
    CORRADE_IGNORE_DEPRECATED_POP
    #endif

    /* Comment out to test the deprecated scroll as press/release reporting */
    #if 1
    void scrollEvent(ScrollEvent& event) override {
        Debug{} << "scroll:" << event.pointers() << event.modifiers() << Debug::packed << event.offset() << Debug::packed << event.position();
    }
    #endif

    void keyPressEvent(KeyEvent& event) override {
        Debug{} << "key press:" << event.key() << int(event.key()) << event.modifiers() << event.pointers() << Debug::packed << event.position();
    }

    void keyReleaseEvent(KeyEvent& event) override {
        Debug{} << "key release:" << event.key() << int(event.key()) << event.modifiers() << event.pointers() << Debug::packed << event.position();
    }
};

AbstractXApplicationTest::AbstractXApplicationTest(const Arguments& arguments): Platform::Application{arguments, NoCreate} {
    Utility::Arguments args;
    args.addSkippedPrefix("magnum", "engine-specific options")
        .addBooleanOption("exit-immediately").setHelp("exit-immediately", "exit the application immediately from the constructor, to test that the app doesn't run any event handlers after")
        .addBooleanOption("quiet").setHelp("quiet", "like --magnum-log quiet, but specified via a Context::Configuration instead")
        .parse(arguments.argc, arguments.argv);

    if(args.isSet("exit-immediately")) {
        exit();
        return;
    }

    Configuration conf;
    conf.setTitle("Window title that should have no exclamation mark!!"_s.exceptSuffix(2));
    if(args.isSet("quiet"))
        create(conf, GLConfiguration{}.addFlags(GLConfiguration::Flag::QuietLog));
    else
        create(conf);
}

}}}}

MAGNUM_APPLICATION_MAIN(Magnum::Platform::Test::AbstractXApplicationTest)
