/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
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
#include "Magnum/Shaders/Phong.h"

namespace Magnum { namespace Shaders { namespace Test {

struct PhongGLTest: GL::OpenGLTester {
    explicit PhongGLTest();

    void construct();
    void constructAmbientTexture();
    void constructDiffuseTexture();
    void constructSpecularTexture();
    void constructAmbientDiffuseTexture();
    void constructAmbientSpecularTexture();
    void constructDiffuseSpecularTexture();
    void constructAmbientDiffuseSpecularTexture();

    void constructMove();
};

PhongGLTest::PhongGLTest() {
    addTests({&PhongGLTest::construct,
              &PhongGLTest::constructAmbientTexture,
              &PhongGLTest::constructDiffuseTexture,
              &PhongGLTest::constructSpecularTexture,
              &PhongGLTest::constructAmbientDiffuseTexture,
              &PhongGLTest::constructAmbientSpecularTexture,
              &PhongGLTest::constructDiffuseSpecularTexture,
              &PhongGLTest::constructAmbientDiffuseSpecularTexture,

              &PhongGLTest::constructMove});
}

void PhongGLTest::construct() {
    Phong shader;
    {
        #ifdef CORRADE_TARGET_APPLE
        CORRADE_EXPECT_FAIL("macOS drivers need insane amount of state to validate properly.");
        #endif
        CORRADE_VERIFY(shader.id());
        CORRADE_VERIFY(shader.validate().first);
    }
}

void PhongGLTest::constructAmbientTexture() {
    Phong shader(Phong::Flag::AmbientTexture);
    {
        #ifdef CORRADE_TARGET_APPLE
        CORRADE_EXPECT_FAIL("macOS drivers need insane amount of state to validate properly.");
        #endif
        CORRADE_VERIFY(shader.id());
        CORRADE_VERIFY(shader.validate().first);
    }
}

void PhongGLTest::constructDiffuseTexture() {
    Phong shader(Phong::Flag::DiffuseTexture);
    {
        #ifdef CORRADE_TARGET_APPLE
        CORRADE_EXPECT_FAIL("macOS drivers need insane amount of state to validate properly.");
        #endif
        CORRADE_VERIFY(shader.id());
        CORRADE_VERIFY(shader.validate().first);
    }
}

void PhongGLTest::constructSpecularTexture() {
    Phong shader(Phong::Flag::SpecularTexture);
    {
        #ifdef CORRADE_TARGET_APPLE
        CORRADE_EXPECT_FAIL("macOS drivers need insane amount of state to validate properly.");
        #endif
        CORRADE_VERIFY(shader.id());
        CORRADE_VERIFY(shader.validate().first);
    }
}

void PhongGLTest::constructAmbientDiffuseTexture() {
    Phong shader(Phong::Flag::AmbientTexture|Phong::Flag::DiffuseTexture);
    {
        #ifdef CORRADE_TARGET_APPLE
        CORRADE_EXPECT_FAIL("macOS drivers need insane amount of state to validate properly.");
        #endif
        CORRADE_VERIFY(shader.id());
        CORRADE_VERIFY(shader.validate().first);
    }
}

void PhongGLTest::constructAmbientSpecularTexture() {
    Phong shader(Phong::Flag::AmbientTexture|Phong::Flag::SpecularTexture);
    {
        #ifdef CORRADE_TARGET_APPLE
        CORRADE_EXPECT_FAIL("macOS drivers need insane amount of state to validate properly.");
        #endif
        CORRADE_VERIFY(shader.id());
        CORRADE_VERIFY(shader.validate().first);
    }
}

void PhongGLTest::constructDiffuseSpecularTexture() {
    Phong shader(Phong::Flag::DiffuseTexture|Phong::Flag::SpecularTexture);
    {
        #ifdef CORRADE_TARGET_APPLE
        CORRADE_EXPECT_FAIL("macOS drivers need insane amount of state to validate properly.");
        #endif
        CORRADE_VERIFY(shader.id());
        CORRADE_VERIFY(shader.validate().first);
    }
}

void PhongGLTest::constructAmbientDiffuseSpecularTexture() {
    Phong shader(Phong::Flag::AmbientTexture|Phong::Flag::DiffuseTexture|Phong::Flag::SpecularTexture);
    {
        #ifdef CORRADE_TARGET_APPLE
        CORRADE_EXPECT_FAIL("macOS drivers need insane amount of state to validate properly.");
        #endif
        CORRADE_VERIFY(shader.id());
        CORRADE_VERIFY(shader.validate().first);
    }
}

void PhongGLTest::constructMove() {
    Phong a;
    const GLuint id = a.id();
    CORRADE_VERIFY(id);

    MAGNUM_VERIFY_NO_GL_ERROR();

    Phong b{std::move(a)};
    CORRADE_COMPARE(b.id(), id);
    CORRADE_VERIFY(!a.id());

    Phong c{NoCreate};
    c = std::move(b);
    CORRADE_COMPARE(c.id(), id);
    CORRADE_VERIFY(!b.id());
}

}}}

CORRADE_TEST_MAIN(Magnum::Shaders::Test::PhongGLTest)
