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

#include "Magnum/Shaders/DistanceFieldVector.h"

namespace Magnum { namespace Shaders { namespace Test { namespace {

struct DistanceFieldVectorTest: TestSuite::Tester {
    explicit DistanceFieldVectorTest();

    template<UnsignedInt dimensions> void constructNoCreate();
    template<UnsignedInt dimensions> void constructCopy();

    void debugFlag();
    void debugFlags();
};

DistanceFieldVectorTest::DistanceFieldVectorTest() {
    addTests({&DistanceFieldVectorTest::constructNoCreate<2>,
              &DistanceFieldVectorTest::constructNoCreate<3>,

              &DistanceFieldVectorTest::constructCopy<2>,
              &DistanceFieldVectorTest::constructCopy<3>,

              &DistanceFieldVectorTest::debugFlag,
              &DistanceFieldVectorTest::debugFlags});
}

template<UnsignedInt dimensions> void DistanceFieldVectorTest::constructNoCreate() {
    setTestCaseTemplateName(std::to_string(dimensions));

    {
        DistanceFieldVector<dimensions> shader{NoCreate};
        CORRADE_COMPARE(shader.id(), 0);
    }

    CORRADE_VERIFY(true);
}

template<UnsignedInt dimensions> void DistanceFieldVectorTest::constructCopy() {
    setTestCaseTemplateName(std::to_string(dimensions));

    CORRADE_VERIFY((std::is_constructible<DistanceFieldVector<dimensions>, DistanceFieldVector<dimensions>&&>{}));
    CORRADE_VERIFY(!(std::is_constructible<DistanceFieldVector<dimensions>, const DistanceFieldVector<dimensions>&>{}));

    CORRADE_VERIFY((std::is_assignable<DistanceFieldVector<dimensions>, DistanceFieldVector<dimensions>&&>{}));
    CORRADE_VERIFY(!(std::is_assignable<DistanceFieldVector<dimensions>, const DistanceFieldVector<dimensions>&>{}));
}

void DistanceFieldVectorTest::debugFlag() {
    std::ostringstream out;

    Debug{&out} << DistanceFieldVector2D::Flag::TextureTransformation << DistanceFieldVector2D::Flag(0xf0);
    CORRADE_COMPARE(out.str(), "Shaders::DistanceFieldVector::Flag::TextureTransformation Shaders::DistanceFieldVector::Flag(0xf0)\n");
}

void DistanceFieldVectorTest::debugFlags() {
    std::ostringstream out;

    Debug{&out} << DistanceFieldVector3D::Flags{DistanceFieldVector3D::Flag::TextureTransformation|DistanceFieldVector3D::Flag(0xf0)} << DistanceFieldVector3D::Flags{};
    CORRADE_COMPARE(out.str(), "Shaders::DistanceFieldVector::Flag::TextureTransformation|Shaders::DistanceFieldVector::Flag(0xf0) Shaders::DistanceFieldVector::Flags{}\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Shaders::Test::DistanceFieldVectorTest)
