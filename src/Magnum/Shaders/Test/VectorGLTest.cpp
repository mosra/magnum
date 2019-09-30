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

#include "Magnum/GL/OpenGLTester.h"
#include "Magnum/Shaders/Vector.h"

namespace Magnum { namespace Shaders { namespace Test { namespace {

struct VectorGLTest: GL::OpenGLTester {
    explicit VectorGLTest();

    template<UnsignedInt dimensions> void construct();
    template<UnsignedInt dimensions> void constructMove();
};

VectorGLTest::VectorGLTest() {
    addTests<VectorGLTest>({
        &VectorGLTest::construct<2>,
        &VectorGLTest::construct<3>,
        &VectorGLTest::constructMove<2>,
        &VectorGLTest::constructMove<3>});
}

template<UnsignedInt dimensions> void VectorGLTest::construct() {
    setTestCaseTemplateName(std::to_string(dimensions));

    Vector<dimensions> shader;
    CORRADE_VERIFY(shader.id());
    {
        #ifdef CORRADE_TARGET_APPLE
        CORRADE_EXPECT_FAIL("macOS drivers need insane amount of state to validate properly.");
        #endif
        CORRADE_VERIFY(shader.validate().first);
    }

    MAGNUM_VERIFY_NO_GL_ERROR();
}

template<UnsignedInt dimensions> void VectorGLTest::constructMove() {
    setTestCaseTemplateName(std::to_string(dimensions));

    Vector<dimensions> a;
    const GLuint id = a.id();
    CORRADE_VERIFY(id);

    MAGNUM_VERIFY_NO_GL_ERROR();

    Vector<dimensions> b{std::move(a)};
    CORRADE_COMPARE(b.id(), id);
    CORRADE_VERIFY(!a.id());

    Vector<dimensions> c{NoCreate};
    c = std::move(b);
    CORRADE_COMPARE(c.id(), id);
    CORRADE_VERIFY(!b.id());
}

}}}}

CORRADE_TEST_MAIN(Magnum::Shaders::Test::VectorGLTest)
