/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014
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
#include <Corrade/TestSuite/Tester.h>

#include "Magnum/DebugMessage.h"

namespace Magnum { namespace Test {

class DebugMessageTest: public TestSuite::Tester {
    public:
        explicit DebugMessageTest();

        void debugSource();
        void debugType();
        void debugSeverity();
};

DebugMessageTest::DebugMessageTest() {
    addTests({&DebugMessageTest::debugSource,
              &DebugMessageTest::debugType,
              &DebugMessageTest::debugSeverity});
}

void DebugMessageTest::debugSource() {
    std::ostringstream o;
    Debug(&o) << DebugMessage::Source::ShaderCompiler;
    CORRADE_COMPARE(o.str(), "DebugMessage::Source::ShaderCompiler\n");
}

void DebugMessageTest::debugType() {
    std::ostringstream o;
    Debug(&o) << DebugMessage::Type::DeprecatedBehavior;
    CORRADE_COMPARE(o.str(), "DebugMessage::Type::DeprecatedBehavior\n");
}

void DebugMessageTest::debugSeverity() {
    std::ostringstream o;
    Debug(&o) << DebugMessage::Severity::Notification;
    CORRADE_COMPARE(o.str(), "DebugMessage::Severity::Notification\n");
}

}}

CORRADE_TEST_MAIN(Magnum::Test::DebugMessageTest)
