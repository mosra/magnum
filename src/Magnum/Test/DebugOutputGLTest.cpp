/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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

#include <sstream>

#include "Magnum/Context.h"
#include "Magnum/DebugOutput.h"
#include "Magnum/Extensions.h"
#include "Magnum/OpenGLTester.h"

namespace Magnum { namespace Test {

struct DebugOutputGLTest: OpenGLTester {
    explicit DebugOutputGLTest();

    void setCallback();
    void setEnabled();

    void messageNoOp();
    void message();
    void messageFallback();

    void groupNoOp();
    void group();
    void groupFallback();
};

DebugOutputGLTest::DebugOutputGLTest() {
    addTests({&DebugOutputGLTest::setCallback,
              &DebugOutputGLTest::setEnabled,

              &DebugOutputGLTest::messageNoOp,
              &DebugOutputGLTest::message,
              &DebugOutputGLTest::messageFallback,

              &DebugOutputGLTest::groupNoOp,
              &DebugOutputGLTest::group,
              &DebugOutputGLTest::groupFallback});
}

void DebugOutputGLTest::setCallback() {
    if(!Context::current().isExtensionSupported<Extensions::GL::KHR::debug>())
        CORRADE_SKIP(Extensions::GL::KHR::debug::string() + std::string(" is not supported"));

    /* Need to be careful, because the test runner is using debug output too */
    DebugOutput::setDefaultCallback();

    MAGNUM_VERIFY_NO_ERROR();
}

void DebugOutputGLTest::setEnabled() {
    if(!Context::current().isExtensionSupported<Extensions::GL::KHR::debug>())
        CORRADE_SKIP(Extensions::GL::KHR::debug::string() + std::string(" is not supported"));

    /* Try at least some combinations */
    DebugOutput::setEnabled(DebugOutput::Source::Application, true);
    DebugOutput::setEnabled(DebugOutput::Source::Application, DebugOutput::Type::UndefinedBehavior, {3168, 35487, 234487}, false);
    DebugOutput::setEnabled(true);

    MAGNUM_VERIFY_NO_ERROR();
}

void DebugOutputGLTest::messageNoOp() {
    if(Context::current().isExtensionSupported<Extensions::GL::KHR::debug>() ||
       Context::current().isExtensionSupported<Extensions::GL::EXT::debug_marker>()
       #ifndef MAGNUM_TARGET_GLES
       || Context::current().isExtensionSupported<Extensions::GL::GREMEDY::string_marker>()
       #endif
    )
        CORRADE_SKIP("The extensions are supported, cannot test.");

    DebugMessage::insert(DebugMessage::Source::Application, DebugMessage::Type::Marker,
        1337, DebugOutput::Severity::Notification, "Hello from OpenGL command stream!");

    MAGNUM_VERIFY_NO_ERROR();
}

void DebugOutputGLTest::message() {
    if(!Context::current().isExtensionSupported<Extensions::GL::KHR::debug>())
        CORRADE_SKIP(Extensions::GL::KHR::debug::string() + std::string(" is not supported"));

    /* Need to be careful, because the test runner is using debug output too */
    std::ostringstream out;
    Debug redirectDebug{&out};
    DebugMessage::insert(DebugMessage::Source::Application, DebugMessage::Type::Marker,
        1337, DebugOutput::Severity::High, "Hello from OpenGL command stream!");

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(out.str(),
        "Debug output: high severity application marker (1337): Hello from OpenGL command stream!\n");
}

void DebugOutputGLTest::messageFallback() {
    if(Context::current().isExtensionSupported<Extensions::GL::KHR::debug>() ||
     (!Context::current().isExtensionSupported<Extensions::GL::EXT::debug_marker>()
   #ifndef MAGNUM_TARGET_GLES
   && !Context::current().isExtensionSupported<Extensions::GL::GREMEDY::string_marker>()
   #endif
    ))
        CORRADE_SKIP("No proper extension is supported");

    DebugMessage::insert(DebugMessage::Source::Application, DebugMessage::Type::Marker,
        1337, DebugOutput::Severity::Notification, "Hello from OpenGL command stream!");

    MAGNUM_VERIFY_NO_ERROR();
}

void DebugOutputGLTest::groupNoOp() {
    if(Context::current().isExtensionSupported<Extensions::GL::KHR::debug>() ||
       Context::current().isExtensionSupported<Extensions::GL::EXT::debug_marker>())
        CORRADE_SKIP("The extensions are supported, cannot test.");

    {
        DebugGroup g{DebugGroup::Source::Application, 1337, "Debug group"};
    }

    MAGNUM_VERIFY_NO_ERROR();
}

void DebugOutputGLTest::group() {
    if(!Context::current().isExtensionSupported<Extensions::GL::KHR::debug>())
        CORRADE_SKIP(Extensions::GL::KHR::debug::string() + std::string(" is not supported"));

    /* Need to be careful, because the test runner is using debug output too */
    std::ostringstream out;
    Debug redirectDebug{&out};
    {
        DebugGroup g1{DebugGroup::Source::Application, 42, "Automatic debug group"};
        DebugGroup g2;
        g2.push(DebugGroup::Source::ThirdParty, 1337, "Manual debug group");
        g2.pop();
    }

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(out.str(),
        "Debug output: application debug group enter (42): Automatic debug group\n"
        "Debug output: third party debug group enter (1337): Manual debug group\n"
        "Debug output: third party debug group leave (1337): Manual debug group\n"
        "Debug output: application debug group leave (42): Automatic debug group\n");
}

void DebugOutputGLTest::groupFallback() {
    if(Context::current().isExtensionSupported<Extensions::GL::KHR::debug>() ||
      !Context::current().isExtensionSupported<Extensions::GL::EXT::debug_marker>())
        CORRADE_SKIP("No proper extension is supported");

    {
        DebugGroup g{DebugGroup::Source::Application, 1337, "Debug group"};
    }

    MAGNUM_VERIFY_NO_ERROR();
}

}}

CORRADE_TEST_MAIN(Magnum::Test::DebugOutputGLTest)
