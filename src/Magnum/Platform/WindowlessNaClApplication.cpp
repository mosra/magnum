/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016
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

#include "WindowlessNaClApplication.h"

#include <ppapi/cpp/graphics_3d.h>
#include <ppapi/cpp/completion_callback.h>
#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/NaClStreamBuffer.h>

#include "Magnum/Platform/Context.h"

namespace Magnum { namespace Platform {

WindowlessNaClContext::WindowlessNaClContext(pp::Instance& instance, const Configuration&, Context*) {
    const std::int32_t attributes[] = {
        PP_GRAPHICS3DATTRIB_ALPHA_SIZE, 8,
        PP_GRAPHICS3DATTRIB_DEPTH_SIZE, 24,
        PP_GRAPHICS3DATTRIB_STENCIL_SIZE, 8,
        PP_GRAPHICS3DATTRIB_WIDTH, 1,
        PP_GRAPHICS3DATTRIB_HEIGHT, 1,
        PP_GRAPHICS3DATTRIB_NONE
    };

    std::unique_ptr<pp::Graphics3D> context{new pp::Graphics3D{*instance, attributes}};
    if(context->is_null())
        Error() << "Platform::WindowlessNaClContext: cannot create context";

    if(!BindGraphics(*context))
        Error() << "Platform::WindowlessNaClContext: cannot bind graphics";

    /* All went well, save the context */
    _context = std::move(context);
}

WindowlessNaClContext::WindowlessNaClContext(WindowlessNaClContext&&) = default;

WindowlessNaClContext::~WindowlessEglContext() = default;

WindowlessNaClContext& WindowlessNaClContext::operator=(WindowlessNaClContext&&) = default;

bool WindowlessNaClContext::makeCurrent() {
    if(!_context) return false;

    glSetCurrentContextPPAPI(_context->pp_resource());
    return true;
}

struct WindowlessNaClApplication::ConsoleDebugOutput {
    explicit ConsoleDebugOutput(pp::Instance* instance);

    Utility::NaClConsoleStreamBuffer debugBuffer, warningBuffer, errorBuffer;
    std::ostream debugOutput, warningOutput, errorOutput;
};

WindowlessNaClApplication::ConsoleDebugOutput::ConsoleDebugOutput(pp::Instance* instance): debugBuffer(instance, Utility::NaClConsoleStreamBuffer::LogLevel::Log), warningBuffer(instance, Utility::NaClConsoleStreamBuffer::LogLevel::Warning), errorBuffer(instance, Utility::NaClConsoleStreamBuffer::LogLevel::Error), debugOutput(&debugBuffer), warningOutput(&warningBuffer), errorOutput(&errorBuffer) {
    /* Inform about this change on standard output */
    Debug() << "Platform::WindowlessNaClApplication: redirecting Debug, Warning and Error output to JavaScript console";

    Debug::setOutput(&debugOutput);
    Warning::setOutput(&warningOutput);
    Error::setOutput(&errorOutput);
}

#ifndef DOXYGEN_GENERATING_OUTPUT
WindowlessNaClApplication::WindowlessNaClApplication(const Arguments& arguments): WindowlessNaClApplication{arguments, Configuration{}} {}
#endif

WindowlessNaClApplication::WindowlessNaClApplication(const Arguments& arguments, const Configuration& configuration):
WindowlessNaClApplication{arguments, NoCreate} {
    createContext(configuration);
}

WindowlessNaClApplication::WindowlessNaClApplication(const Arguments& arguments, NoCreate): Instance(arguments), Graphics3DClient(this), _glContext{NoCreate}, _debugOutput{new ConsoleDebugOutput{this}} {}

void WindowlessNaClApplication::createContext() { createContext({}); }

void WindowlessNaClApplication::createContext(const Configuration& configuration) {
    if(!tryCreateContext(configuration)) std::exit(1);
}

bool WindowlessNaClApplication::tryCreateContext(const Configuration& configuration) {
    CORRADE_ASSERT(_context->version() == Version::None, "Platform::WindowlessNaClApplication::tryCreateContext(): context already created", false);

    WindowlessNaClContext glContext{*this, configuration, _context.get()};
    if(!glContext.isCreated() || !glContext.makeCurrent() || !_context->tryCreate())
        return false;

    _glContext = std::move(glContext);
    return true;
}

WindowlessNaClApplication::~WindowlessNaClApplication() = default;

bool WindowlessNaClApplication::Init(uint32_t , const char* , const char*) {
    return exec() == 0;
}

void WindowlessNaClApplication::Graphics3DContextLost() {
    CORRADE_ASSERT(false, "NaClApplication: context unexpectedly lost", );
}

}}
