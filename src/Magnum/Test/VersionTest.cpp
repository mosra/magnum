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

#include <sstream>
#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Version.h"

namespace Magnum { namespace Test {

struct VersionTest: TestSuite::Tester {
    explicit VersionTest();

    void fromNumber();
    void toNumber();
    void compare();

    void debug();
};

VersionTest::VersionTest() {
    addTests({&VersionTest::fromNumber,
              &VersionTest::toNumber,
              &VersionTest::compare,

              &VersionTest::debug});
}

void VersionTest::fromNumber() {
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(version(4, 3), Version::GL430);
    #else
    CORRADE_COMPARE(version(3, 0), Version::GLES300);
    #endif
}

void VersionTest::toNumber() {
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(version(Version::GL430), std::make_pair(4, 3));
    #else
    CORRADE_COMPARE(version(Version::GLES300), std::make_pair(3, 0));
    #endif
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
    Debug(&out) << Version::GL210;
    #else
    Debug(&out) << Version::GLES200;
    #endif

    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(out.str(), "OpenGL 2.1\n");
    #else
    CORRADE_COMPARE(out.str(), "OpenGL ES 2.0\n");
    #endif
}

}}

CORRADE_TEST_MAIN(Magnum::Test::VersionTest)
