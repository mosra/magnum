/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2018, 2019 Jonathan Hale <squareys@googlemail.com>

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

#include "Magnum/Platform/EmscriptenApplication.h"
#include "Magnum/GL/Renderer.h"
#include "Magnum/GL/DefaultFramebuffer.h"
#include "Magnum/GL/Mesh.h"

namespace Magnum { namespace Platform { namespace Test {

struct EmscriptenApplicationTest: Platform::Application {
    /* For testing resize events */
    explicit EmscriptenApplicationTest(const Arguments& arguments):
        Platform::Application{arguments,
            Configuration{}.setWindowFlags(Configuration::WindowFlag::Resizable)
            //, GLConfiguration{}.setFlags({})
        } {

        /* This uses a VAO on WebGL 1, so it will crash in case GL flags are
           missing EnableExtensionsByDefault (uncomment above) */
        GL::Mesh mesh;
    }

    virtual void drawEvent() override {
        GL::defaultFramebuffer.clear(GL::FramebufferClear::Color);

        swapBuffers();
        redraw();
    }

    #ifdef MAGNUM_TARGET_GL
    /* For testing HiDPI resize events */
    void viewportEvent(ViewportEvent& event) override {
        Debug{} << "viewport event" << event.windowSize() << event.framebufferSize() << event.dpiScaling();
    }
    #endif

    /* For testing event coordinates */
    void mousePressEvent(MouseEvent& event) override {
        Debug{} << "press" << event.position() << Int(event.button());
    }

    /* For testing event coordinates */
    void mouseReleaseEvent(MouseEvent& event) override {
        Debug{} << "release" << event.position() << Int(event.button());
    }

    /* For testing event coordinates */
    void mouseMoveEvent(MouseMoveEvent& event) override {
        Debug{} << "move" << event.position();
    }

    /* For testing keyboard capture */
    void keyPressEvent(KeyEvent& event) override {
        if(event.key() != KeyEvent::Key::Unknown) {
            Debug{} << "keyPressEvent(" << event.keyName().c_str() << "): ✓";
        } else {
            Debug{} << "keyPressEvent(" << event.keyName().c_str() << "): x";
        }

        if(event.modifiers() & KeyEvent::Modifier::Shift) Debug{} << "Shift";
        if(event.modifiers() & KeyEvent::Modifier::Ctrl) Debug{} << "Ctrl";
        if(event.modifiers() & KeyEvent::Modifier::Alt) Debug{} << "Alt";
        if(event.modifiers() & KeyEvent::Modifier::Super) Debug{} << "Super";
    }

    void keyReleaseEvent(KeyEvent& event) override {
        if(event.key() != KeyEvent::Key::Unknown) {
            Debug{} << "keyReleaseEvent(" << event.keyName().c_str() << "): ✓";
        } else {
            Debug{} << "keyReleaseEvent(" << event.keyName().c_str() << "): x";
        }

        if(event.modifiers() & KeyEvent::Modifier::Shift) Debug{} << "Shift";
        if(event.modifiers() & KeyEvent::Modifier::Ctrl) Debug{} << "Ctrl";
        if(event.modifiers() & KeyEvent::Modifier::Alt) Debug{} << "Alt";
        if(event.modifiers() & KeyEvent::Modifier::Super) Debug{} << "Super";
    }
};

}}}

MAGNUM_APPLICATION_MAIN(Magnum::Platform::Test::EmscriptenApplicationTest)
