/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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

#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Platform/GlfwApplication.h"

namespace Magnum { namespace Platform { namespace Test { namespace {

struct GlfwApplicationTest: Platform::Application {
    explicit GlfwApplicationTest(const Arguments& arguments): Platform::Application{arguments} {
        Debug{} << "window size" << windowSize()
            #ifdef MAGNUM_TARGET_GL
            << framebufferSize()
            #endif
            << dpiScaling();
    }

    /* For testing HiDPI resize events */
    void viewportEvent(ViewportEvent& event) override {
        Debug{} << "viewport event" << event.windowSize()
            #ifdef MAGNUM_TARGET_GL
            << event.framebufferSize()
            #endif
            << event.dpiScaling();
    }

    void keyPressEvent(KeyEvent& event) override {
        #if GLFW_VERSION_MAJOR*100 + GLFW_VERSION_MINOR >= 302
        Debug{} << "key press event:" << int(event.key()) << event.keyName();
        #endif

        if(event.key() == KeyEvent::Key::F1) {
            Debug{} << "starting text input";
            startTextInput();
        } else if(event.key() == KeyEvent::Key::Esc) {
            Debug{} << "stopping text input";
            stopTextInput();
        } else if(event.key() == KeyEvent::Key::T) {
            Debug{} << "setting window title";
            setWindowTitle("This is a UTF-8 Window Title™!");
        } else if(event.key() == KeyEvent::Key::H) {
            Debug{} << "toggling hand cursor";
            setCursor(cursor() == Cursor::Arrow ? Cursor::Hand : Cursor::Arrow);
        } else if(event.key() == KeyEvent::Key::L) {
            Debug{} << "toggling locked mouse";
            setCursor(cursor() == Cursor::Arrow ? Cursor::HiddenLocked : Cursor::Arrow);
        }
    }

    void mouseMoveEvent(MouseMoveEvent& event) override {
        Debug{} << "mouse move event:" << event.position() << event.relativePosition() << UnsignedInt(event.buttons());
    }

    void textInputEvent(TextInputEvent& event) override {
        Debug{} << "text input event:" << std::string{event.text(), event.text().size()};
    }

    void exitEvent(ExitEvent& event) override {
        Debug{} << "application exiting";
        event.setAccepted(); /* Comment-out to test app exit suppression */
    }

    void drawEvent() override {}
};

}}}}

MAGNUM_APPLICATION_MAIN(Magnum::Platform::Test::GlfwApplicationTest)
