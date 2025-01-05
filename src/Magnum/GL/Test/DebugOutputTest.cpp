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

#include <Corrade/Containers/String.h>
#include <Corrade/TestSuite/Tester.h>

#include "Magnum/GL/DebugOutput.h"

namespace Magnum { namespace GL { namespace Test { namespace {

struct DebugOutputTest: TestSuite::Tester {
    explicit DebugOutputTest();

    void debugSource();
    void debugType();
    void debugSeverity();

    void debugMessageSource();
    void debugMessageType();

    void debugGroupSource();
};

DebugOutputTest::DebugOutputTest() {
    addTests({&DebugOutputTest::debugSource,
              &DebugOutputTest::debugType,
              &DebugOutputTest::debugSeverity,

              &DebugOutputTest::debugMessageSource,
              &DebugOutputTest::debugMessageType,

              &DebugOutputTest::debugGroupSource});
}

void DebugOutputTest::debugSource() {
    Containers::String out;
    Debug{&out} << DebugOutput::Source::ShaderCompiler << DebugOutput::Source(0xdead);
    CORRADE_COMPARE(out, "GL::DebugOutput::Source::ShaderCompiler GL::DebugOutput::Source(0xdead)\n");
}

void DebugOutputTest::debugType() {
    Containers::String out;
    Debug{&out} << DebugOutput::Type::PushGroup << DebugOutput::Type(0xdead);
    CORRADE_COMPARE(out, "GL::DebugOutput::Type::PushGroup GL::DebugOutput::Type(0xdead)\n");
}

void DebugOutputTest::debugSeverity() {
    Containers::String out;
    Debug{&out} << DebugOutput::Severity::Notification << DebugOutput::Severity(0xdead);
    CORRADE_COMPARE(out, "GL::DebugOutput::Severity::Notification GL::DebugOutput::Severity(0xdead)\n");
}

void DebugOutputTest::debugMessageSource() {
    Containers::String out;
    Debug{&out} << DebugMessage::Source::Application << DebugMessage::Source(0xdead);
    CORRADE_COMPARE(out, "GL::DebugMessage::Source::Application GL::DebugMessage::Source(0xdead)\n");
}

void DebugOutputTest::debugMessageType() {
    Containers::String out;
    Debug{&out} << DebugMessage::Type::DeprecatedBehavior << DebugMessage::Type(0xdead);
    CORRADE_COMPARE(out, "GL::DebugMessage::Type::DeprecatedBehavior GL::DebugMessage::Type(0xdead)\n");
}

void DebugOutputTest::debugGroupSource() {
    Containers::String out;
    Debug{&out} << DebugGroup::Source::ThirdParty << DebugGroup::Source(0xdead);
    CORRADE_COMPARE(out, "GL::DebugGroup::Source::ThirdParty GL::DebugGroup::Source(0xdead)\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::GL::Test::DebugOutputTest)
