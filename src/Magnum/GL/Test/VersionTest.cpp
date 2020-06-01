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
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/GL/Version.h"

namespace Magnum { namespace GL { namespace Test { namespace {

struct VersionTest: TestSuite::Tester {
    explicit VersionTest();

    void fromNumber();
    void toNumber();
    #ifndef MAGNUM_TARGET_GLES
    void toNumberES();
    #endif
    void isES();
    void compare();

    void debug();
    #ifndef MAGNUM_TARGET_GLES
    void debugES();
    #endif
};

VersionTest::VersionTest() {
    addTests({&VersionTest::fromNumber,
              &VersionTest::toNumber,
              #ifndef MAGNUM_TARGET_GLES
              &VersionTest::toNumberES,
              #endif
              &VersionTest::isES,
              &VersionTest::compare,

              &VersionTest::debug,
              #ifndef MAGNUM_TARGET_GLES
              &VersionTest::debugES
              #endif
              });
}

void VersionTest::fromNumber() {
    #ifndef MAGNUM_TARGET_GLES
    constexpr const Version v = version(4, 3);
    CORRADE_COMPARE(v, Version::GL430);
    #else
    constexpr const Version v = version(3, 0);
    CORRADE_COMPARE(v, Version::GLES300);
    #endif
}

void VersionTest::toNumber() {
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(version(Version::GL430), std::make_pair(4, 3));
    #else
    CORRADE_COMPARE(version(Version::GLES300), std::make_pair(3, 0));
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void VersionTest::toNumberES() {
    CORRADE_COMPARE(version(Version::GLES310), std::make_pair(3, 1));
}
#endif

void VersionTest::isES() {
    CORRADE_VERIFY(isVersionES(Version::GLES200));
}

void VersionTest::compare() {
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_VERIFY(version(1, 1) < Version::GL210);
    #else
    CORRADE_VERIFY(version(1, 1) < Version::GLES200);
    #endif
}

void VersionTest::debug() {
    std::ostringstream out;

    #ifndef MAGNUM_TARGET_GLES
    Debug(&out) << Version::GL210 << Version(0xdead);
    #else
    Debug(&out) << Version::GLES200 << Version(0xdead);
    #endif

    #ifdef MAGNUM_TARGET_WEBGL
    CORRADE_COMPARE(out.str(), "WebGL 1.0 Invalid(0xdead)\n");
    #elif defined(MAGNUM_TARGET_GLES)
    CORRADE_COMPARE(out.str(), "OpenGL ES 2.0 Invalid(0xdead)\n");
    #else
    CORRADE_COMPARE(out.str(), "OpenGL 2.1 Invalid(0xdead)\n");
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void VersionTest::debugES() {
    std::ostringstream out;

    Debug{&out} << Version::GLES310;
    CORRADE_COMPARE(out.str(), "OpenGL ES 3.1\n");
}
#endif

}}}}

CORRADE_TEST_MAIN(Magnum::GL::Test::VersionTest)
