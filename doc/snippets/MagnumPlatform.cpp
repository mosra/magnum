/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
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

/* [windowed] */
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/Platform/Context.h>

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
    Renderer::setClearColor(0x000066_rgbf);
}

void MyApplication::drawEvent() {
    /* Clear the window */
    defaultFramebuffer.clear(FramebufferClear::Color);

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
        void viewportEvent(const Vector2i& size) override;
};

// ...

void MyApplication::viewportEvent(const Vector2i& size) {
    defaultFramebuffer.setViewport({{}, size});
}
/* [size] */

}

namespace C {

struct MyApplication: Platform::Application {
    MyApplication(const Arguments& arguments);
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

    createContext(Configuration{}
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
    conf.setTitle("My Application")
        .setSampleCount(16);

    if(!tryCreateContext(conf))
        createContext(conf.setSampleCount(0));

    // ...
}
/* [trycreatecontext] */

}
