/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024
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

#include "Magnum/GL/DefaultFramebuffer.h"
#include "Magnum/Platform/AndroidApplication.h"

namespace Magnum { namespace Platform {

/* Cannot be in an anonymous namespace as enumSetDebugOutput() below wouldn't
   be able to pick it up */
static Debug& operator<<(Debug& debug, Application::MouseMoveEvent::Button value) {
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

namespace Test { namespace {

Debug& operator<<(Debug& debug, Application::MouseEvent::Button value) {
    debug << "Button" << Debug::nospace;

    switch(value) {
        #define _c(value) case Application::MouseEvent::Button::value: return debug << "::" #value;
        _c(None)
        _c(Left)
        _c(Middle)
        _c(Right)
        #undef _c
    }

    return debug << "(" << Debug::nospace << UnsignedInt(value) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, Application::MouseMoveEvent::Buttons value) {
    return Containers::enumSetDebugOutput(debug, value, "Buttons{}", {
        Application::MouseMoveEvent::Button::Left,
        Application::MouseMoveEvent::Button::Middle,
        Application::MouseMoveEvent::Button::Right,
    });
}

struct AndroidApplicationTest: Platform::Application {
    explicit AndroidApplicationTest(const Arguments& arguments): Platform::Application{arguments} {
        Debug{} << "window size" << windowSize() << framebufferSize() << dpiScaling();
    }

    void drawEvent() override {
        GL::defaultFramebuffer.clear(GL::FramebufferClear::Color);

        swapBuffers();
    }

    /* For testing HiDPI resize events */
    void viewportEvent(ViewportEvent& event) override {
        Debug{} << "viewport:" << event.windowSize() << event.framebufferSize() << event.dpiScaling();
    }

    void mousePressEvent(MouseEvent& event) override {
        Debug{} << "mouse press:" << event.button() << Debug::packed << event.position();
    }

    void mouseReleaseEvent(MouseEvent& event) override {
        Debug{} << "mouse release:" << event.button() << Debug::packed << event.position();
    }

    void mouseMoveEvent(MouseMoveEvent& event) override {
        Debug{} << "mouse move:" << event.buttons() << Debug::packed << event.position() << Debug::packed << event.relativePosition();
    }
};

}}}}

MAGNUM_APPLICATION_MAIN(Magnum::Platform::Test::AndroidApplicationTest)
