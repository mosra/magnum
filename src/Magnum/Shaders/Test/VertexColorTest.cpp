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

#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Shaders/VertexColor.h"

namespace Magnum { namespace Shaders { namespace Test { namespace {

struct VertexColorTest: TestSuite::Tester {
    explicit VertexColorTest();

    template<UnsignedInt dimensions> void constructNoCreate();
    template<UnsignedInt dimensions> void constructCopy();
};

VertexColorTest::VertexColorTest() {
    addTests<VertexColorTest>({
        &VertexColorTest::constructNoCreate<2>,
        &VertexColorTest::constructNoCreate<3>,

        &VertexColorTest::constructCopy<2>,
        &VertexColorTest::constructCopy<3>});
}

template<UnsignedInt dimensions> void VertexColorTest::constructNoCreate() {
    setTestCaseTemplateName(std::to_string(dimensions));

    {
        VertexColor<dimensions> shader{NoCreate};
        CORRADE_COMPARE(shader.id(), 0);
    }

    CORRADE_VERIFY(true);
}

template<UnsignedInt dimensions> void VertexColorTest::constructCopy() {
    setTestCaseTemplateName(std::to_string(dimensions));

    CORRADE_VERIFY((std::is_constructible<VertexColor<dimensions>, VertexColor<dimensions>&&>{}));
    CORRADE_VERIFY(!(std::is_constructible<VertexColor<dimensions>, const VertexColor<dimensions>&>{}));

    CORRADE_VERIFY((std::is_assignable<VertexColor<dimensions>, VertexColor<dimensions>&&>{}));
    CORRADE_VERIFY(!(std::is_assignable<VertexColor<dimensions>, const VertexColor<dimensions>&>{}));
}

}}}}

CORRADE_TEST_MAIN(Magnum::Shaders::Test::VertexColorTest)
