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

#define DOXYGEN_ELLIPSIS(...) __VA_ARGS__

#include <Corrade/configure.h>
#if defined(CORRADE_TARGET_UNIX) || (defined(CORRADE_TARGET_WINDOWS) && !defined(CORRADE_TARGET_WINDOWS_RT)) || defined(CORRADE_TARGET_EMSCRIPTEN)
#include <Corrade/Utility/Tweakable.h>
#endif

#include "Magnum/Platform/Gesture.h"

/* [windowed] */
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Platform/ScreenedApplication.h>
#include <Magnum/Platform/Screen.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/Platform/GLContext.h>

using namespace Magnum;
using namespace Magnum::Math::Literals;

class MyApplication: public Platform::Application {
    public:
        explicit MyApplication(const Arguments& arguments);

    private:
        void drawEvent() override;
};

MyApplication::MyApplication(const Arguments& arguments):
    Platform::Application{arguments}
{
    /* Set clear color to a nice blue */
    GL::Renderer::setClearColor(0x2f83cc_rgbf);
}

void MyApplication::drawEvent() {
    /* Clear the window */
    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color);

    /* The context is double-buffered, swap buffers */
    swapBuffers();
}

/* main() function implementation */
MAGNUM_APPLICATION_MAIN(MyApplication)
/* [windowed] */
/* In this case `int main()` can't be avoided, but other snippets compiled into
   the same static library all use a different name so it shouldn't conflict */

/* Sdl2Application is included unconditionally as part of a snippet above, and
   it doesn't implement Pointer::Finger on Emscripten */
#ifndef CORRADE_TARGET_EMSCRIPTEN
namespace B {

/* [windowed-pointer-events] */
class MyApplication: public Platform::Application {
    DOXYGEN_ELLIPSIS()

    private:
        void pointerPressEvent(PointerEvent& event) override {
            /* Handling just left mouse press or equivalent */
            if(!event.isPrimary() ||
               !(event.pointer() & (Pointer::MouseLeft|Pointer::Finger)))
                return;

            DOXYGEN_ELLIPSIS()
            event.setAccepted();
        }

        void pointerReleaseEvent(PointerEvent& event) override {
            /* Handling just left mouse press or equivalent */
            if(!event.isPrimary() ||
               !(event.pointer() & (Pointer::MouseLeft|Pointer::Finger)))
                return;

            DOXYGEN_ELLIPSIS()
            event.setAccepted();
        }

        void pointerMoveEvent(PointerMoveEvent& event) override {
            /* Handling just left mouse drag or equivalent */
            if(!event.isPrimary() ||
               !(event.pointers() & (Pointer::MouseLeft|Pointer::Finger)))
                return;

            DOXYGEN_ELLIPSIS()
            event.setAccepted();
        }
};
/* [windowed-pointer-events] */

}
#endif

namespace C {

/* [windowed-key-events] */
class MyApplication: public Platform::Application {
    DOXYGEN_ELLIPSIS(void performUndo() {} void performRedo() {} void performInput(int, Containers::StringView) {})

    private:
        void keyPressEvent(KeyEvent& event) override {
            /* Editing shortcuts */
            if(event.key() == Key::Z &&
               event.modifiers() == Modifier::Ctrl)
                performUndo(DOXYGEN_ELLIPSIS());
            else if(event.key() == Key::Z &&
                    event.modifiers() == (Modifier::Shift|Modifier::Ctrl))
                performRedo(DOXYGEN_ELLIPSIS());
            DOXYGEN_ELLIPSIS()
            else return;

            event.setAccepted();
        }

        void textInputEvent(TextInputEvent& event) override {
            /* Assuming text input is currently active */
            performInput(DOXYGEN_ELLIPSIS(0), event.text());

            event.setAccepted();
        }
};
/* [windowed-key-events] */

}

namespace D {

struct MyApplication: Platform::Application {
    explicit MyApplication(const Arguments& arguments);

    void foo();
};

/* [windowed-configuration] */
MyApplication::MyApplication(const Arguments& arguments):
    Platform::Application{arguments, Configuration{}
        .setTitle("My Application")
        .setSize({12800, 800})}
{
    DOXYGEN_ELLIPSIS()
}
/* [windowed-configuration] */

void MyApplication::foo() {
/* [Sdl2Application-dpi-scaling] */
Vector2 scaling = Vector2{framebufferSize()}*dpiScaling()/Vector2{windowSize()};
/* [Sdl2Application-dpi-scaling] */
static_cast<void>(scaling);
}

}

namespace E {

/* [windowed-viewport-events] */
class MyApplication: public Platform::Application {
    public:
        explicit MyApplication(const Arguments& arguments):
            Platform::Application{arguments, Configuration{}
                .addWindowFlags(Configuration::WindowFlag::Resizable)}
        {
            DOXYGEN_ELLIPSIS()
        }

    private:
        void viewportEvent(ViewportEvent& event) override {
            GL::defaultFramebuffer.setViewport({{}, event.framebufferSize()});
            DOXYGEN_ELLIPSIS()
        }
};
/* [windowed-viewport-events] */

}

namespace F {

struct MyApplication: Platform::Application {
    MyApplication(const Arguments& arguments);
};

/* [createcontext] */
MyApplication::MyApplication(const Arguments& arguments):
    Platform::Application{arguments, NoCreate}
{
    DOXYGEN_ELLIPSIS(Vector2i size;)

    create(Configuration{}
        .setTitle("My Application")
        .setSize(size));

    DOXYGEN_ELLIPSIS()
}
/* [createcontext] */

}

namespace G {

struct MyApplication: Platform::Application {
    MyApplication(const Arguments& arguments);
};

/* [trycreatecontext] */
MyApplication::MyApplication(const Arguments& arguments):
    Platform::Application{arguments, NoCreate}
{
    DOXYGEN_ELLIPSIS()

    Configuration conf;
    conf.setTitle("My Application");
    GLConfiguration glConf;
    glConf.setSampleCount(16);

    if(!tryCreate(conf, glConf))
        create(conf, glConf.setSampleCount(0));

    DOXYGEN_ELLIPSIS()
}
/* [trycreatecontext] */

}

namespace H {

/* On MSVC 2017, deprecation warning suppression doesn't work on virtual
   function overrides, so ScreenedApplication overriding mousePressEvent(),
   mouseReleaseEvent() mouseMoveEvent() and mouseScrollEvent() causes warnings.
   Disable them at a higher level instead. */
#if defined(MAGNUM_BUILD_DEPRECATED) && defined(CORRADE_TARGET_MSVC) && !defined(CORRADE_TARGET_CLANG) && _MSC_VER < 1920
CORRADE_IGNORE_DEPRECATED_PUSH
#endif
int argc = 0;
struct MyApplication: Platform::ScreenedApplication {
    MyApplication(): Platform::ScreenedApplication{Arguments{argc, nullptr}} {}

    void globalViewportEvent(ViewportEvent& event) override;
    void globalDrawEvent() override;
};
#if defined(MAGNUM_BUILD_DEPRECATED) && defined(CORRADE_TARGET_MSVC) && !defined(CORRADE_TARGET_CLANG) && _MSC_VER < 1920
CORRADE_IGNORE_DEPRECATED_POP
#endif

/* [ScreenedApplication-global-events] */
void MyApplication::globalViewportEvent(ViewportEvent& event) {
    GL::defaultFramebuffer.setViewport({{}, event.framebufferSize()});

    // Other stuff that should be done *before* all other event handlers ...
}

void MyApplication::globalDrawEvent() {
    // Other stuff that should be done *after* all other event handlers ...

    swapBuffers();
}
/* [ScreenedApplication-global-events] */

void foo();
void foo() {
/* [ScreenedApplication-for-range] */
MyApplication app;
for(Platform::Screen& screen: app.screens()) {
    DOXYGEN_ELLIPSIS(static_cast<void>(screen));
}
/* [ScreenedApplication-for-range] */

/* [ScreenedApplication-for] */
for(Platform::Screen* s = app.screens().first(); s; s = s->nextFartherScreen()) {
    // ...
}
/* [ScreenedApplication-for] */
}

}

namespace I {

struct MyApplication: Platform::Application {
    MyApplication(const Arguments& arguments);
    bool everythingGoingAsExpected = false;
};

/* [exit-from-constructor] */
MyApplication::MyApplication(const Arguments& arguments):
    Platform::Application{arguments, NoCreate}
{
    // …

    if(!everythingGoingAsExpected) {
        exit(1);
        return;
    }

    // …
}
/* [exit-from-constructor] */

}

#if defined(CORRADE_TARGET_UNIX) || (defined(CORRADE_TARGET_WINDOWS) && !defined(CORRADE_TARGET_WINDOWS_RT)) || defined(CORRADE_TARGET_EMSCRIPTEN)
namespace J {

struct MyApplication: Platform::Application {
    MyApplication(const Arguments& arguments);

    void tickEvent() override;

    Utility::Tweakable _tweakable;
};

/* [tickEvent-conditional] */
void MyApplication::tickEvent() {
    if(!_tweakable.isEnabled()) {
        Platform::Application::tickEvent();
        return;
    }

    _tweakable.update();
}
/* [tickEvent-conditional] */

}
#endif

namespace K {

struct MyApplication: Platform::Application {
    void pointerPressEvent(PointerEvent& event) override;
    void pointerReleaseEvent(PointerEvent& event) override;
    void pointerMoveEvent(PointerMoveEvent& event) override;

    void translateSomething(const Vector2&) {}
    void rotateSomething(const Complex&) {}
    void scaleSomething(Float) {}

    Platform::TwoFingerGesture _gesture;
};

/* [TwoFingerGesture] */
void MyApplication::pointerPressEvent(PointerEvent& event) {
    _gesture.pressEvent(event);

    DOXYGEN_ELLIPSIS()
}

void MyApplication::pointerReleaseEvent(PointerEvent& event) {
    _gesture.releaseEvent(event);

    DOXYGEN_ELLIPSIS()
}

void MyApplication::pointerMoveEvent(PointerMoveEvent& event) {
    _gesture.moveEvent(event);

    /* A gesture is recognized, perform appropriate action */
    if(_gesture) {
        translateSomething(_gesture.relativeTranslation());
        rotateSomething(_gesture.relativeRotation());
        scaleSomething(_gesture.relativeScaling());

        event.setAccepted();
        redraw();
        return;
    }

    DOXYGEN_ELLIPSIS()
}
/* [TwoFingerGesture] */

}
