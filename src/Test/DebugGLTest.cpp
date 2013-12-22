/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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

#include "Test/AbstractOpenGLTester.h"

#include "Context.h"
#include "DebugMessage.h"
#include "Extensions.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include "DebugMarker.h"
#endif

namespace Magnum { namespace Test {

class DebugGLTest: public AbstractOpenGLTester {
    public:
        explicit DebugGLTest();

        void insertMessageNoOp();
        void insertMessage();
        void insertMessageFallback();

        void deprecated();
};

DebugGLTest::DebugGLTest() {
    addTests({&DebugGLTest::insertMessageNoOp,
              &DebugGLTest::insertMessage,
              &DebugGLTest::insertMessageFallback,

              #ifdef MAGNUM_BUILD_DEPRECATED
              &DebugGLTest::deprecated
              #endif
              });
}

void DebugGLTest::insertMessageNoOp() {
    if(Context::current()->isExtensionSupported<Extensions::GL::KHR::debug>() ||
       Context::current()->isExtensionSupported<Extensions::GL::EXT::debug_marker>() ||
       Context::current()->isExtensionSupported<Extensions::GL::GREMEDY::string_marker>())
        CORRADE_SKIP("The extensions are supported, cannot test.");

    DebugMessage::insert(DebugMessage::Source::Application, DebugMessage::Type::Marker,
        1337, DebugMessage::Severity::Notification, "Hello from OpenGL command stream!");

    MAGNUM_VERIFY_NO_ERROR();
}

void DebugGLTest::insertMessage() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::KHR::debug>())
        CORRADE_SKIP(Extensions::GL::KHR::debug::string() + std::string(" is not supported"));

    DebugMessage::insert(DebugMessage::Source::Application, DebugMessage::Type::Marker,
        1337, DebugMessage::Severity::Notification, "Hello from OpenGL command stream!");

    MAGNUM_VERIFY_NO_ERROR();
}

void DebugGLTest::insertMessageFallback() {
    if(Context::current()->isExtensionSupported<Extensions::GL::KHR::debug>() ||
     (!Context::current()->isExtensionSupported<Extensions::GL::EXT::debug_marker>() &&
      !Context::current()->isExtensionSupported<Extensions::GL::GREMEDY::string_marker>()))
        CORRADE_SKIP("No proper extension is supported");

    DebugMessage::insert(DebugMessage::Source::Application, DebugMessage::Type::Marker,
        1337, DebugMessage::Severity::Notification, "Hello from OpenGL command stream!");

    MAGNUM_VERIFY_NO_ERROR();
}

#ifdef MAGNUM_BUILD_DEPRECATED
void DebugGLTest::deprecated() {
    DebugMarker::mark("hello");

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

}}

CORRADE_TEST_MAIN(Magnum::Test::DebugGLTest)
