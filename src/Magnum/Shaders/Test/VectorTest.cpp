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

#include "Magnum/Shaders/Vector.h"

namespace Magnum { namespace Shaders { namespace Test { namespace {

struct VectorTest: TestSuite::Tester {
    explicit VectorTest();

    void constructNoCreate2D();
    void constructNoCreate3D();

    void constructCopy2D();
    void constructCopy3D();

    void debugFlag();
    void debugFlags();
};

VectorTest::VectorTest() {
    addTests({&VectorTest::constructNoCreate2D,
              &VectorTest::constructNoCreate3D,

              &VectorTest::constructCopy2D,
              &VectorTest::constructCopy3D,

              &VectorTest::debugFlag,
              &VectorTest::debugFlags});
}

void VectorTest::constructNoCreate2D() {
    {
        Vector2D shader{NoCreate};
        CORRADE_COMPARE(shader.id(), 0);
    }

    CORRADE_VERIFY(true);
}

void VectorTest::constructNoCreate3D() {
    {
        Vector3D shader{NoCreate};
        CORRADE_COMPARE(shader.id(), 0);
    }

    CORRADE_VERIFY(true);
}

void VectorTest::constructCopy2D() {
    CORRADE_VERIFY(!(std::is_constructible<Vector2D, const Vector2D&>{}));
    CORRADE_VERIFY(!(std::is_assignable<Vector2D, const Vector2D&>{}));
}

void VectorTest::constructCopy3D() {
    CORRADE_VERIFY(!(std::is_constructible<Vector3D, const Vector3D&>{}));
    CORRADE_VERIFY(!(std::is_assignable<Vector3D, const Vector3D&>{}));
}

void VectorTest::debugFlag() {
    std::ostringstream out;

    Debug{&out} << Vector2D::Flag::TextureTransformation << Vector2D::Flag(0xf0);
    CORRADE_COMPARE(out.str(), "Shaders::Vector::Flag::TextureTransformation Shaders::Vector::Flag(0xf0)\n");
}

void VectorTest::debugFlags() {
    std::ostringstream out;

    Debug{&out} << Vector3D::Flags{Vector3D::Flag::TextureTransformation|Vector3D::Flag(0xf0)} << Vector3D::Flags{};
    CORRADE_COMPARE(out.str(), "Shaders::Vector::Flag::TextureTransformation|Shaders::Vector::Flag(0xf0) Shaders::Vector::Flags{}\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Shaders::Test::VectorTest)
