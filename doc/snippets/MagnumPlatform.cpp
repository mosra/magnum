/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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

/* [windowed] */
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Platform/ScreenedApplication.h>
#include <Magnum/Platform/Screen.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/Platform/GLContext.h>

using namespace Magnum;

class MyApplication: public Platform::Application {
    public:
        MyApplication(const Arguments& arguments);

    private:
        void drawEvent() override;
};

MyApplication::MyApplication(const Arguments& arguments): Platform::Application{arguments} {
    using namespace Math::Literals;
    /* Set clear color to dark blue */
    GL::Renderer::setClearColor(0x000066_rgbf);
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

namespace B {

/* [size] */
class MyApplication: public Platform::Application {
    // ...

    private:
        void viewportEvent(ViewportEvent& event) override;
};

// ...

void MyApplication::viewportEvent(ViewportEvent& event) {
    GL::defaultFramebuffer.setViewport({{}, event.framebufferSize()});
}
/* [size] */

}

namespace C {

struct MyApplication: Platform::Application {
    MyApplication(const Arguments& arguments);

    void foo();
};

/* [configuration] */
MyApplication::MyApplication(const Arguments& arguments):
    Platform::Application(arguments, Configuration{}
        .setTitle("My Application")
        .setSize({800, 600}))
{
    // ...
}
/* [configuration] */

void MyApplication::foo() {
/* [Sdl2Application-dpi-scaling] */
Vector2 scaling = Vector2{framebufferSize()}*dpiScaling()/Vector2{windowSize()};
/* [Sdl2Application-dpi-scaling] */
static_cast<void>(scaling);
}

}

namespace D {

constexpr Vector2i size;

struct MyApplication: Platform::Application {
    MyApplication(const Arguments& arguments);
};

/* [createcontext] */
MyApplication::MyApplication(const Arguments& arguments):
    Platform::Application{arguments, NoCreate}
{
    // ...

    create(Configuration{}
        .setTitle("My Application")
        .setSize(size));

    // ...
}
/* [createcontext] */

}

namespace E {

struct MyApplication: Platform::Application {
    MyApplication(const Arguments& arguments);
};

/* [trycreatecontext] */
MyApplication::MyApplication(const Arguments& arguments):
    Platform::Application{arguments, NoCreate}
{
    // ...

    Configuration conf;
    conf.setTitle("My Application");
    GLConfiguration glConf;
    glConf.setSampleCount(16);

    if(!tryCreate(conf, glConf))
        create(conf, glConf.setSampleCount(0));

    // ...
}
/* [trycreatecontext] */

}

namespace F {

int argc = 0;
struct MyApplication: Platform::ScreenedApplication {
    MyApplication(): Platform::ScreenedApplication{Arguments{argc, nullptr}} {}

    void globalViewportEvent(ViewportEvent& event) override;
    void globalDrawEvent() override;
};

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
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif
/* [ScreenedApplication-for-range] */
MyApplication app;
for(Platform::Screen& screen: app.screens()) {
    // ...
}
/* [ScreenedApplication-for-range] */

/* [ScreenedApplication-for] */
for(Platform::Screen* s = app.screens().first(); s; s = s->nextFartherScreen()) {
    // ...
}
/* [ScreenedApplication-for] */
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
}

}

namespace G {

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
