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

#include <Corrade/Utility/Arguments.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Platform/EmscriptenApplication.h"
#include "Magnum/GL/Renderer.h"
#include "Magnum/GL/DefaultFramebuffer.h"
#include "Magnum/GL/Mesh.h"
#include "Magnum/Math/Color.h"

namespace Magnum { namespace Platform { namespace Test {

using namespace Math::Literals;

struct EmscriptenApplicationTest: Platform::Application {
    /* For testing resize events */
    explicit EmscriptenApplicationTest(const Arguments& arguments);

    virtual void drawEvent() override {
        Debug() << "draw event";
        #ifdef CUSTOM_CLEAR_COLOR
        GL::Renderer::setClearColor(CUSTOM_CLEAR_COLOR);
        #endif
        GL::defaultFramebuffer.clear(GL::FramebufferClear::Color);

        swapBuffers();

        if(_redraw) {
            redraw();
        }
    }

    #ifdef MAGNUM_TARGET_GL
    /* For testing HiDPI resize events */
    void viewportEvent(ViewportEvent& event) override {
        Debug{} << "viewport event" << event.windowSize() << event.framebufferSize() << event.dpiScaling() << event.devicePixelRatio();
    }
    #endif

    /* For testing event coordinates */
    void mousePressEvent(MouseEvent& event) override {
        Debug{} << "mouse press event:" << event.position() << Int(event.button());
    }

    void mouseReleaseEvent(MouseEvent& event) override {
        Debug{} << "mouse release event:" << event.position() << Int(event.button());
    }

    void mouseMoveEvent(MouseMoveEvent& event) override {
        Debug{} << "mouse move event:" << event.position() << event.relativePosition() << Int(event.buttons());
    }

    void mouseScrollEvent(MouseScrollEvent& event) override {
        Debug{} << "mouse scroll event:" << event.offset() << event.position();
    }

    /* For testing keyboard capture */
    void keyPressEvent(KeyEvent& event) override {
        {
            Debug d;
            if(event.key() != KeyEvent::Key::Unknown) {
                d << "keyPressEvent(" << Debug::nospace << event.keyName().data() << Debug::nospace << "): ✔";
            } else {
                d << "keyPressEvent(" << Debug::nospace << event.keyName().data() << Debug::nospace << "): ✘";
            }

            if(event.modifiers() & KeyEvent::Modifier::Shift) d << "Shift";
            if(event.modifiers() & KeyEvent::Modifier::Ctrl) d << "Ctrl";
            if(event.modifiers() & KeyEvent::Modifier::Alt) d << "Alt";
            if(event.modifiers() & KeyEvent::Modifier::Super) d << "Super";
        }

        if(event.key() == KeyEvent::Key::F1) {
            Debug{} << "starting text input";
            startTextInput();
        } else if(event.key() == KeyEvent::Key::F2) {
            _redraw = !_redraw;
            Debug{} << "redrawing" << (_redraw ? "enabled" : "disabled");
            if(_redraw) redraw();
        } else if(event.key() == KeyEvent::Key::Esc) {
            Debug{} << "stopping text input";
            stopTextInput();
        } else if(event.key() == KeyEvent::Key::F) {
            Debug{} << "toggling fullscreen";
            setContainerCssClass((_fullscreen ^= true) ? "mn-fullsize" : "");
        } else if(event.key() == KeyEvent::Key::T) {
            Debug{} << "setting window title";
            setWindowTitle("This is a UTF-8 Window Title™!");
        } else if(event.key() == KeyEvent::Key::H) {
            Debug{} << "toggling hand cursor";
            setCursor(cursor() == Cursor::Arrow ? Cursor::Hand : Cursor::Arrow);
        }

        event.setAccepted();
    }

    void keyReleaseEvent(KeyEvent& event) override {
        {
            Debug d;
            if(event.key() != KeyEvent::Key::Unknown) {
                d << "keyReleaseEvent(" << Debug::nospace << event.keyName().data() << Debug::nospace << "): ✔";
            } else {
                d << "keyReleaseEvent(" << Debug::nospace << event.keyName().data() << Debug::nospace << "): ✘";
            }

            if(event.modifiers() & KeyEvent::Modifier::Shift) d << "Shift";
            if(event.modifiers() & KeyEvent::Modifier::Ctrl) d << "Ctrl";
            if(event.modifiers() & KeyEvent::Modifier::Alt) d << "Alt";
            if(event.modifiers() & KeyEvent::Modifier::Super) d << "Super";
        }

        event.setAccepted();
    }

    void textInputEvent(TextInputEvent& event) override {
        Debug{} << "text input event:" << std::string{event.text(), event.text().size()};

        event.setAccepted();
    }

    private:
        bool _fullscreen = false;
        bool _redraw = false;
};

EmscriptenApplicationTest::EmscriptenApplicationTest(const Arguments& arguments): Platform::Application{arguments, NoCreate} {
    Utility::Arguments args;
    args.addSkippedPrefix("magnum", "engine-specific options")
        .addBooleanOption("exit-immediately").setHelp("exit-immediately", "exit the application immediately from the constructor, to test that the app doesn't run any event handlers after")
        .parse(arguments.argc, arguments.argv);

    if(args.isSet("exit-immediately")) {
        exit();
        return;
    }

    create(Configuration{}.setWindowFlags(Configuration::WindowFlag::Resizable)
        //, GLConfiguration{}.setFlags({})
    );

    Debug{} << "window size" << windowSize()
        #ifdef MAGNUM_TARGET_GL
        << framebufferSize()
        #endif
        << dpiScaling() << devicePixelRatio();

    /* This uses a VAO on WebGL 1, so it will crash in case GL flags are
       missing EnableExtensionsByDefault (uncomment above) */
    GL::Mesh mesh;
}

}}}

MAGNUM_APPLICATION_MAIN(Magnum::Platform::Test::EmscriptenApplicationTest)
