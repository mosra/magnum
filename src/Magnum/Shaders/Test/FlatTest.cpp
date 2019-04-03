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

#include "Magnum/Shaders/Flat.h"

namespace Magnum { namespace Shaders { namespace Test { namespace {

struct FlatTest: TestSuite::Tester {
    explicit FlatTest();

    void constructNoCreate2D();
    void constructNoCreate3D();

    void constructCopy2D();
    void constructCopy3D();

    void debugFlag();
    void debugFlags();
};

FlatTest::FlatTest() {
    addTests({&FlatTest::constructNoCreate2D,
              &FlatTest::constructNoCreate3D,

              &FlatTest::constructCopy2D,
              &FlatTest::constructCopy3D,

              &FlatTest::debugFlag,
              &FlatTest::debugFlags});
}

void FlatTest::constructNoCreate2D() {
    {
        Flat2D shader{NoCreate};
        CORRADE_COMPARE(shader.id(), 0);
    }

    CORRADE_VERIFY(true);
}

void FlatTest::constructNoCreate3D() {
    {
        Flat3D shader{NoCreate};
        CORRADE_COMPARE(shader.id(), 0);
    }

    CORRADE_VERIFY(true);
}

void FlatTest::constructCopy2D() {
    CORRADE_VERIFY(!(std::is_constructible<Flat2D, const Flat2D&>{}));
    CORRADE_VERIFY(!(std::is_assignable<Flat2D, const Flat2D&>{}));
}

void FlatTest::constructCopy3D() {
    CORRADE_VERIFY(!(std::is_constructible<Flat3D, const Flat3D&>{}));
    CORRADE_VERIFY(!(std::is_assignable<Flat3D, const Flat3D&>{}));
}

void FlatTest::debugFlag() {
    std::ostringstream out;

    Debug{&out} << Flat3D::Flag::Textured << Flat3D::Flag(0xf0);
    CORRADE_COMPARE(out.str(), "Shaders::Flat::Flag::Textured Shaders::Flat::Flag(0xf0)\n");
}

void FlatTest::debugFlags() {
    std::ostringstream out;

    Debug{&out} << (Flat3D::Flag::Textured|Flat3D::Flag::AlphaMask) << Flat3D::Flags{};
    CORRADE_COMPARE(out.str(), "Shaders::Flat::Flag::Textured|Shaders::Flat::Flag::AlphaMask Shaders::Flat::Flags{}\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Shaders::Test::FlatTest)
