/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/GL/Context.h"
#include "Magnum/GL/Version.h"

namespace Magnum { namespace GL { namespace Test { namespace {

struct ContextTest: TestSuite::Tester {
    explicit ContextTest();

    void constructNoCreate();
    void constructCopyMove();

    void makeCurrentNoOp();

    void extensions();

    void debugFlag();
    void debugFlags();

    void debugDetectedDriver();
    void debugDetectedDrivers();
};

ContextTest::ContextTest() {
    addTests({&ContextTest::constructNoCreate,
              &ContextTest::constructCopyMove,

              &ContextTest::makeCurrentNoOp,

              &ContextTest::extensions,

              &ContextTest::debugFlag,
              &ContextTest::debugFlags,

              &ContextTest::debugDetectedDriver,
              &ContextTest::debugDetectedDrivers});
}

void ContextTest::constructNoCreate() {
    {
        /* Shouldn't crash during construction, shouldn't attempt to access GL,
           shouldn't crash when destructing */
        struct MyContext: Context {
            explicit MyContext(): Context{NoCreate, 0, nullptr, nullptr} {}
        } context;

        CORRADE_VERIFY(!Context::hasCurrent());
    }

    CORRADE_VERIFY(!Context::hasCurrent());
}

void ContextTest::constructCopyMove() {
    /* Only move-construction allowed */
    CORRADE_VERIFY(!(std::is_constructible<Context, const Context&>{}));
    CORRADE_VERIFY((std::is_constructible<Context, Context&&>{}));
    CORRADE_VERIFY(!(std::is_assignable<Context, const Context&>{}));
    CORRADE_VERIFY(!(std::is_assignable<Context, Context&&>{}));
}

void ContextTest::makeCurrentNoOp() {
    CORRADE_VERIFY(!Context::hasCurrent());
    Context::makeCurrent(nullptr);
    CORRADE_VERIFY(!Context::hasCurrent());
}

void ContextTest::extensions() {
    const char* used[GL::Implementation::ExtensionCount]{};

    /* Check that all extension indices are unique */
    for(Version version: {
        #ifndef MAGNUM_TARGET_GLES
        Version::GL300,
        Version::GL310,
        Version::GL320,
        Version::GL330,
        Version::GL400,
        Version::GL410,
        Version::GL420,
        Version::GL430,
        Version::GL440,
        Version::GL450,
        Version::GL460,
        #else
        Version::GLES200,
        Version::GLES300,
        #ifndef MAGNUM_TARGET_WEBGL
        Version::GLES310,
        Version::GLES320,
        #endif
        #endif
        Version::None})
    {
        for(const Extension& e: Extension::extensions(version)) {
            if(e.index() >= GL::Implementation::ExtensionCount) {
                Error{} << "Index" << e.index() << "used by" << e.string()
                        << "larger than" << GL::Implementation::ExtensionCount;
                CORRADE_VERIFY(false);
            }

            if(used[e.index()]) {
                Error{} << "Index" << e.index() << "used by both"
                        << used[e.index()] << "and" << e.string();
                CORRADE_VERIFY(false);
            }

            used[e.index()] = e.string();
        }
    }

    CORRADE_VERIFY(true);
}

void ContextTest::debugFlag() {
    #ifdef MAGNUM_TARGET_WEBGL
    CORRADE_SKIP("No context flags on Emscripten yet.");
    #else
    std::ostringstream out;
    Debug(&out) << Context::Flag::Debug << Context::Flag(0xdead);
    CORRADE_COMPARE(out.str(), "GL::Context::Flag::Debug GL::Context::Flag(0xdead)\n");
    #endif
}

void ContextTest::debugFlags() {
    #ifdef MAGNUM_TARGET_WEBGL
    CORRADE_SKIP("No context flags on Emscripten yet.");
    #else
    std::ostringstream out;
    Debug(&out) << Context::Flags{} << (Context::Flag::Debug|Context::Flag::NoError) << (Context::Flag::Debug|Context::Flag(0xded0));
    CORRADE_COMPARE(out.str(), "GL::Context::Flags{} GL::Context::Flag::Debug|GL::Context::Flag::NoError GL::Context::Flag::Debug|GL::Context::Flag(0xded0)\n");
    #endif
}

void ContextTest::debugDetectedDriver() {
    std::ostringstream out;
    #ifndef MAGNUM_TARGET_WEBGL
    Debug{&out} << Context::DetectedDriver::Amd << Context::DetectedDriver(0xdead);
    CORRADE_COMPARE(out.str(), "GL::Context::DetectedDriver::Amd GL::Context::DetectedDriver(0xdead)\n");
    #else
    Debug{&out} << Context::DetectedDriver::Angle << Context::DetectedDriver(0xdead);
    CORRADE_COMPARE(out.str(), "GL::Context::DetectedDriver::Angle GL::Context::DetectedDriver(0xdead)\n");
    #endif
}

void ContextTest::debugDetectedDrivers() {
    std::ostringstream out;
    #ifndef MAGNUM_TARGET_WEBGL
    Debug{&out} << Context::DetectedDrivers{} << (Context::DetectedDriver::Amd|Context::DetectedDriver::NVidia) << (Context::DetectedDriver::Mesa|Context::DetectedDriver(0xde00));
    CORRADE_COMPARE(out.str(), "GL::Context::DetectedDrivers{} GL::Context::DetectedDriver::Amd|GL::Context::DetectedDriver::NVidia GL::Context::DetectedDriver::Mesa|GL::Context::DetectedDriver(0xde00)\n");
    #else
    Debug{&out} << Context::DetectedDrivers{} << (Context::DetectedDriver::Angle) << (Context::DetectedDriver::Angle|Context::DetectedDriver(0xde00));
    CORRADE_COMPARE(out.str(), "GL::Context::DetectedDrivers{} GL::Context::DetectedDriver::Angle GL::Context::DetectedDriver::Angle|GL::Context::DetectedDriver(0xde00)\n");
    #endif
}

}}}}

CORRADE_TEST_MAIN(Magnum::GL::Test::ContextTest)
