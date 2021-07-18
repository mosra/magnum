/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021 Vladimír Vondruš <mosra@centrum.cz>

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

#include "Magnum/GL/Buffer.h"
#include "Magnum/GL/DefaultFramebuffer.h"
#include "Magnum/GL/Framebuffer.h"
#include "Magnum/GL/Mesh.h"
#include "Magnum/Platform/Sdl2Application.h"
#include "Magnum/Platform/GLContext.h"
#include "Magnum/Shaders/PhongGL.h"

using namespace Magnum;

#define DOXYGEN_IGNORE(...) __VA_ARGS__

/* [opengl-wrapping-nocreate] */
class MyApplication: public Platform::Application {
    DOXYGEN_IGNORE(explicit MyApplication(const Arguments& arguments);)

    private:
        /* Placeholders without an underlying GL object */
        GL::Mesh _mesh{NoCreate};
        Shaders::PhongGL _shader{NoCreate};
        DOXYGEN_IGNORE()
};

MyApplication::MyApplication(const Arguments& arguments):
    Platform::Application{arguments, NoCreate}
{
    DOXYGEN_IGNORE()
    create();

    /* GL context is ready, now it's safe to populate the GL objects */
    _mesh = GL::Mesh{};
    _shader = Shaders::PhongGL{};
}
/* [opengl-wrapping-nocreate] */

struct A: Platform::Sdl2Application {
/* [DefaultFramebuffer-usage-viewport] */
void viewportEvent(ViewportEvent& event) override {
    GL::defaultFramebuffer.setViewport({{}, event.framebufferSize()});

    // ...
}
/* [DefaultFramebuffer-usage-viewport] */

/* [DefaultFramebuffer-usage-clear] */
void drawEvent() override {
    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color|
                                 GL::FramebufferClear::Depth);

    // ...
}
/* [DefaultFramebuffer-usage-clear] */
};

struct B: Platform::Sdl2Application {

GL::Framebuffer framebuffer;

/* [Framebuffer-usage-draw] */
void drawEvent() override {
    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color);
    framebuffer.clear(GL::FramebufferClear::Color|
                      GL::FramebufferClear::Depth|
                      GL::FramebufferClear::Stencil);

    framebuffer.bind();
    // ...

    GL::defaultFramebuffer.bind();
    // ...

    swapBuffers();
}
/* [Framebuffer-usage-draw] */
};

int main() {

{
SDL_Window* _window{};
SDL_GLContext* _otherGLContext{};
/* [Context-makeCurrent-nullptr] */
Platform::GLContext context;

SDL_GL_MakeCurrent(_window, _otherGLContext); // or other platform-specific API
Platform::GLContext::makeCurrent(nullptr);

Platform::GLContext other;
/* [Context-makeCurrent-nullptr] */

/* [Context-makeCurrent] */
Platform::GLContext::makeCurrent(&context);

GL::Buffer a; // implicitly tied to `context`

Platform::GLContext::makeCurrent(&other);

GL::Buffer b; // implicitly tied to `other`
/* [Context-makeCurrent] */
}

return 0; /* on iOS SDL redefines main to SDL_main and then return is needed */
}
