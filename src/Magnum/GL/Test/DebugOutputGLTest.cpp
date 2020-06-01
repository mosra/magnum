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

#include <sstream>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/GL/Context.h"
#include "Magnum/GL/DebugOutput.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/OpenGLTester.h"

namespace Magnum { namespace GL { namespace Test { namespace {

struct DebugOutputGLTest: OpenGLTester {
    explicit DebugOutputGLTest();

    void setCallbackDefault();

    void setup();
    void teardown();

    void setEnabled();

    void messageNoOp();
    void message();
    void messageFallback();

    void groupNoOp();
    void group();
    void groupFallback();

    std::ostringstream _out;
};

DebugOutputGLTest::DebugOutputGLTest() {
    addTests({&DebugOutputGLTest::setCallbackDefault});

    addTests({&DebugOutputGLTest::setEnabled,

              &DebugOutputGLTest::messageNoOp,
              &DebugOutputGLTest::message,
              &DebugOutputGLTest::messageFallback,

              &DebugOutputGLTest::groupNoOp,
              &DebugOutputGLTest::group,
              &DebugOutputGLTest::groupFallback},
        &DebugOutputGLTest::setup, &DebugOutputGLTest::teardown);
}

void DebugOutputGLTest::setCallbackDefault() {
    if(!Context::current().isExtensionSupported<Extensions::KHR::debug>())
        CORRADE_SKIP(Extensions::KHR::debug::string() + std::string(" is not supported"));

    DebugOutput::setDefaultCallback();

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void DebugOutputGLTest::setup() {
    _out.str({});

    if(!Context::current().isExtensionSupported<Extensions::KHR::debug>())
        return;

    Renderer::enable(Renderer::Feature::DebugOutput);
    Renderer::enable(Renderer::Feature::DebugOutputSynchronous);
    DebugOutput::setCallback([](DebugOutput::Source source, DebugOutput::Type type, UnsignedInt id, DebugOutput::Severity severity, const std::string& string, const void* userPtr) {
        Implementation::defaultDebugCallback(source, type, id, severity, string, static_cast<std::ostringstream*>(const_cast<void*>(userPtr)));
    }, &_out);
}

void DebugOutputGLTest::teardown() {
    if(!Context::current().isExtensionSupported<Extensions::KHR::debug>())
        return;

    Renderer::disable(Renderer::Feature::DebugOutput);
    Renderer::disable(Renderer::Feature::DebugOutputSynchronous);
    DebugOutput::setDefaultCallback();
}

void DebugOutputGLTest::setEnabled() {
    if(!Context::current().isExtensionSupported<Extensions::KHR::debug>())
        CORRADE_SKIP(Extensions::KHR::debug::string() + std::string(" is not supported"));

    /* Try at least some combinations. Calling a less-specific version will
       reset the more-specific setting from earlier. */
    DebugOutput::setEnabled(true);
    DebugOutput::setEnabled(DebugOutput::Source::Application, true);
    DebugOutput::setEnabled(DebugOutput::Source::Application, DebugOutput::Type::UndefinedBehavior, {3168, 35487, 234487}, false);

    MAGNUM_VERIFY_NO_GL_ERROR();

    DebugMessage::insert(DebugMessage::Source::Application, DebugMessage::Type::UndefinedBehavior,
        35487, DebugOutput::Severity::Notification, "This message should get ignored");

    /* Reset everything back */
    DebugOutput::setEnabled(true);

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(_out.str(), "");
}

void DebugOutputGLTest::messageNoOp() {
    if(Context::current().isExtensionSupported<Extensions::KHR::debug>() ||
       Context::current().isExtensionSupported<Extensions::EXT::debug_marker>()
       #ifndef MAGNUM_TARGET_GLES
       || Context::current().isExtensionSupported<Extensions::GREMEDY::string_marker>()
       #endif
    )
        CORRADE_SKIP("The extensions are supported, cannot test.");

    DebugMessage::insert(DebugMessage::Source::Application, DebugMessage::Type::Marker,
        1337, DebugOutput::Severity::Notification, "Hello from OpenGL command stream!");

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(_out.str(), "");
}

void DebugOutputGLTest::message() {
    if(!Context::current().isExtensionSupported<Extensions::KHR::debug>())
        CORRADE_SKIP(Extensions::KHR::debug::string() + std::string(" is not supported"));

    /* Need to be careful, because the test runner is using debug output too */
    DebugMessage::insert(DebugMessage::Source::Application, DebugMessage::Type::Marker,
        1337, DebugOutput::Severity::High, "Hello from OpenGL command stream!");

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(_out.str(),
        "Debug output: high severity application marker (1337): Hello from OpenGL command stream!\n");
}

void DebugOutputGLTest::messageFallback() {
    if(Context::current().isExtensionSupported<Extensions::KHR::debug>() ||
     (!Context::current().isExtensionSupported<Extensions::EXT::debug_marker>()
   #ifndef MAGNUM_TARGET_GLES
   && !Context::current().isExtensionSupported<Extensions::GREMEDY::string_marker>()
   #endif
    ))
        CORRADE_SKIP("No proper extension is supported");

    DebugMessage::insert(DebugMessage::Source::Application, DebugMessage::Type::Marker,
        1337, DebugOutput::Severity::Notification, "Hello from OpenGL command stream!");

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(_out.str(), "");
}

void DebugOutputGLTest::groupNoOp() {
    if(Context::current().isExtensionSupported<Extensions::KHR::debug>() ||
       Context::current().isExtensionSupported<Extensions::EXT::debug_marker>())
        CORRADE_SKIP("The extensions are supported, cannot test.");

    {
        DebugGroup g{DebugGroup::Source::Application, 1337, "Debug group"};
    }

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(_out.str(), "");
}

void DebugOutputGLTest::group() {
    if(!Context::current().isExtensionSupported<Extensions::KHR::debug>())
        CORRADE_SKIP(Extensions::KHR::debug::string() + std::string(" is not supported"));

    /* Need to be careful, because the test runner is using debug output too */
    {
        DebugGroup g1{DebugGroup::Source::Application, 42, "Automatic debug group"};
        DebugGroup g2;
        g2.push(DebugGroup::Source::ThirdParty, 1337, "Manual debug group");
        g2.pop();
    }

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(_out.str(),
        "Debug output: application debug group enter (42): Automatic debug group\n"
        "Debug output: third party debug group enter (1337): Manual debug group\n"
        "Debug output: third party debug group leave (1337): Manual debug group\n"
        "Debug output: application debug group leave (42): Automatic debug group\n");
}

void DebugOutputGLTest::groupFallback() {
    if(Context::current().isExtensionSupported<Extensions::KHR::debug>() ||
      !Context::current().isExtensionSupported<Extensions::EXT::debug_marker>())
        CORRADE_SKIP("No proper extension is supported");

    {
        DebugGroup g{DebugGroup::Source::Application, 1337, "Debug group"};
    }

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(_out.str(), "");
}

}}}}

CORRADE_TEST_MAIN(Magnum::GL::Test::DebugOutputGLTest)
