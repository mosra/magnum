/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023 Vladimír Vondruš <mosra@centrum.cz>

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

#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/Arguments.h>
#include <Corrade/Utility/Format.h>
#include <Corrade/Utility/Resource.h>

#include "Magnum/ImageView.h"
#include "Magnum/Math/ConfigurationValue.h"
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

#ifdef MAGNUM_TARGET_GL
#include "Magnum/GL/DefaultFramebuffer.h"
#endif

namespace Magnum { namespace Platform { namespace Test { namespace {

Debug& operator<<(Debug& debug, const Application::KeyEvent::Key value) {
    debug << "Key" << Debug::nospace;

    switch(value) {
        #define _c(value) case Application::KeyEvent::Key::value: return debug << "::" #value;
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

using namespace Containers::Literals;

#ifndef CORRADE_TARGET_EMSCRIPTEN
struct Sdl2ApplicationTestWindow: Platform::ApplicationWindow {
    explicit Sdl2ApplicationTestWindow(Platform::Application& application, std::size_t id): Platform::ApplicationWindow{application, Configuration{}
        .setTitle(Utility::format("Window {}", id))
        .setSize({400, 300})
        .addWindowFlags(Configuration::WindowFlag::Resizable)
    }, _id{id} {}

    void viewportEvent(ViewportEvent& event) override {
        Debug{} << "window" << _id << "viewport event" << event.windowSize()
            #ifdef MAGNUM_TARGET_GL
            << event.framebufferSize()
            #endif
            << event.dpiScaling();
#warning query the GL fb size here and compare
        // GL::defaultFramebuffer.setViewport({{}, event.framebufferSize()});
    }

    void drawEvent() override {
        Debug{} << "window" << _id << "draw event";
        #ifdef MAGNUM_TARGET_GL
        GL::defaultFramebuffer.clear(GL::FramebufferClear::Color);
        #endif

        swapBuffers();
    }

    /* For testing event coordinates */
    void mousePressEvent(MouseEvent& event) override {
        Debug{} << "window" << _id << "mouse press event:" << event.position() << Int(event.button());
    }

    void mouseReleaseEvent(MouseEvent& event) override {
        Debug{} << "window" << _id << "mouse release event:" << event.position() << Int(event.button());
    }

    void mouseMoveEvent(MouseMoveEvent& event) override {
        Debug{} << "window" << _id << "mouse move event:" << event.position() << event.relativePosition() << Uint32(event.buttons());
    }

    void mouseScrollEvent(MouseScrollEvent& event) override {
        Debug{} << "window" << _id << "mouse scroll event:" << event.offset() << event.position();
    }

    void keyPressEvent(KeyEvent& event) override {
        Debug{} << "window" << _id << "key press event:" << SDL_Keycode(event.key()) << event.keyName();

        if(event.key() == KeyEvent::Key::F1) {
            Debug{} << "starting text input";
            application().startTextInput();
        } else if(event.key() == KeyEvent::Key::Esc) {
            Debug{} << "stopping text input";
            application().stopTextInput();
        } else if(event.key() == KeyEvent::Key::T) {
            Debug{} << "setting window title";
            setWindowTitle("This is a UTF-8 Window Title™ and it should have no exclamation mark!!"_s.exceptSuffix(2));
        }
        #ifndef CORRADE_TARGET_EMSCRIPTEN
        else if(event.key() == KeyEvent::Key::S) {
            Debug{} << "setting window size, which should trigger a viewport event";
            setWindowSize(Vector2i{300, 200});
        } else if(event.key() == KeyEvent::Key::W) {
            Debug{} << "setting max window size, which should trigger a viewport event";
            setMaxWindowSize(Vector2i{700, 500});
        }
        #endif
        else if(event.key() == KeyEvent::Key::H) {
            Debug{} << "toggling hand cursor";
            setCursor(cursor() == Cursor::Arrow ? Cursor::Hand : Cursor::Arrow);
        }
        #ifndef CORRADE_TARGET_EMSCRIPTEN
        else if(event.key() == KeyEvent::Key::L) {
            Debug{} << "toggling locked mouse";
            setCursor(cursor() == Cursor::Arrow ? Cursor::HiddenLocked : Cursor::Arrow);
        }
        #else
        else if(event.key() == KeyEvent::Key::F) {
            Debug{} << "toggling fullscreen";
            setContainerCssClass((_fullscreen ^= true) ? "mn-fullsize" : "");
        }
        #endif
        else if(event.key() == KeyEvent::Key::X) {
            Debug{} << "requesting an exit with code 5";
            exit(5);
        }
    }

    void keyReleaseEvent(KeyEvent& event) override {
        Debug{} << "window" << _id << "key release event:" << SDL_Keycode(event.key()) << event.keyName();
    }

    void textInputEvent(TextInputEvent& event) override {
        Debug{} << "window" << _id << "text input event:" << event.text();
    }

    private:
        std::size_t _id;
};
#endif

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
    }

    /* For testing event coordinates */
    void mousePressEvent(MouseEvent& event) override {
        Debug{} << "mouse press event:" << event.position() << Int(event.button());
    }

    void mouseReleaseEvent(MouseEvent& event) override {
        Debug{} << "mouse release event:" << event.position() << Int(event.button());
    }

    void mouseMoveEvent(MouseMoveEvent& event) override {
        Debug{} << "mouse move event:" << event.position() << event.relativePosition() << Uint32(event.buttons());
    }

    void mouseScrollEvent(MouseScrollEvent& event) override {
        Debug{} << "mouse scroll event:" << event.offset() << event.position();
    }

    void keyPressEvent(KeyEvent& event) override {
        Debug{} << "key press event:" << event.key() << "keycode:" << SDL_Keycode(event.key()) << event.keyName() << "scancode:" << event.event().key.keysym.scancode
            #ifndef CORRADE_TARGET_EMSCRIPTEN
            << SDL_GetScancodeName(event.event().key.keysym.scancode)
            #endif
            ;

        if(event.key() == KeyEvent::Key::F1) {
            Debug{} << "starting text input";
            startTextInput();
        } else if(event.key() == KeyEvent::Key::Esc) {
            Debug{} << "stopping text input";
            stopTextInput();
        } else if(event.key() == KeyEvent::Key::T) {
            Debug{} << "setting window title";
            setWindowTitle("This is a UTF-8 Window Title™ and it should have no exclamation mark!!"_s.exceptSuffix(2));
        }
        #ifndef CORRADE_TARGET_EMSCRIPTEN
        else if(event.key() == KeyEvent::Key::S) {
            Debug{} << "setting window size, which should trigger a viewport event";
            setWindowSize(Vector2i{300, 200});
        } else if(event.key() == KeyEvent::Key::W) {
            Debug{} << "setting max window size, which should trigger a viewport event";
            setMaxWindowSize(Vector2i{700, 500});
        }
        else if(event.key() == KeyEvent::Key::O) {
            Debug{} << "opening window" << _windows.size();
            arrayAppend(_windows, Containers::pointer<Sdl2ApplicationTestWindow>(*this, _windows.size()));
        }
        #endif
        else if(event.key() == KeyEvent::Key::H) {
            Debug{} << "toggling hand cursor";
            setCursor(cursor() == Cursor::Arrow ? Cursor::Hand : Cursor::Arrow);
        }
        #ifndef CORRADE_TARGET_EMSCRIPTEN
        else if(event.key() == KeyEvent::Key::L) {
            Debug{} << "toggling locked mouse";
            setCursor(cursor() == Cursor::Arrow ? Cursor::HiddenLocked : Cursor::Arrow);
        }
        #else
        else if(event.key() == KeyEvent::Key::F) {
            Debug{} << "toggling fullscreen";
            setContainerCssClass((_fullscreen ^= true) ? "mn-fullsize" : "");
        }
        #endif
        else if(event.key() == KeyEvent::Key::X) {
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
        Debug{} << "key release event:" << event.key() << "keycode:" << SDL_Keycode(event.key()) << event.keyName() << "scancode:" << event.event().key.keysym.scancode
            #ifndef CORRADE_TARGET_EMSCRIPTEN
            << SDL_GetScancodeName(event.event().key.keysym.scancode)
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

    private:
        #ifndef CORRADE_TARGET_EMSCRIPTEN
        Containers::Array<Containers::Pointer<Sdl2ApplicationTestWindow>> _windows;
        #else
        bool _fullscreen = false;
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
