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

#include "Magnum/Shaders/Vector.h"

namespace Magnum { namespace Shaders { namespace Test { namespace {

struct VectorTest: TestSuite::Tester {
    explicit VectorTest();

    template<UnsignedInt dimensions> void constructNoCreate();
    template<UnsignedInt dimensions> void constructCopy();

    void debugFlag();
    void debugFlags();
};

VectorTest::VectorTest() {
    addTests({&VectorTest::constructNoCreate<2>,
              &VectorTest::constructNoCreate<3>,

              &VectorTest::constructCopy<2>,
              &VectorTest::constructCopy<3>,

              &VectorTest::debugFlag,
              &VectorTest::debugFlags});
}

template<UnsignedInt dimensions> void VectorTest::constructNoCreate() {
    setTestCaseTemplateName(std::to_string(dimensions));

    {
        Vector<dimensions> shader{NoCreate};
        CORRADE_COMPARE(shader.id(), 0);
    }

    CORRADE_VERIFY(true);
}

template<UnsignedInt dimensions> void VectorTest::constructCopy() {
    setTestCaseTemplateName(std::to_string(dimensions));

    CORRADE_VERIFY((std::is_constructible<Vector<dimensions>, Vector<dimensions>&&>{}));
    CORRADE_VERIFY(!(std::is_constructible<Vector<dimensions>, const Vector<dimensions>&>{}));

    CORRADE_VERIFY((std::is_assignable<Vector<dimensions>, Vector<dimensions>&&>{}));
    CORRADE_VERIFY(!(std::is_assignable<Vector<dimensions>, const Vector<dimensions>&>{}));
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
