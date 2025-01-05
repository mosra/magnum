/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
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
#include "Magnum/GL/Shader.h"
#include "Magnum/GL/Version.h"
#include "Magnum/Shaders/glShaderWrapper.h"

namespace Magnum { namespace Shaders { namespace Test { namespace {

struct GLShaderWrapperGLTest: GL::OpenGLTester {
    explicit GLShaderWrapperGLTest();

    void construct();
    void constructMove();

    void convert();
    void convertRvalue();
};

GLShaderWrapperGLTest::GLShaderWrapperGLTest() {
    addTests({&GLShaderWrapperGLTest::construct,
              &GLShaderWrapperGLTest::constructMove,

              &GLShaderWrapperGLTest::convert,
              &GLShaderWrapperGLTest::convertRvalue});
}

void GLShaderWrapperGLTest::construct() {
    {
        GL::Shader glShader{
            #ifndef MAGNUM_TARGET_GLES
            GL::Version::GL300,
            #else
            GL::Version::GLES300,
            #endif
            GL::Shader::Type::Fragment};

        GLuint id = glShader.id();
        MAGNUM_VERIFY_NO_GL_ERROR();
        CORRADE_VERIFY(id > 0);

        Implementation::GLShaderWrapper shader{Utility::move(glShader)};
        MAGNUM_VERIFY_NO_GL_ERROR();
        CORRADE_COMPARE(shader.id, id);
        CORRADE_COMPARE(shader.type, GL_FRAGMENT_SHADER);
        CORRADE_VERIFY(!glShader.id());
    }

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void GLShaderWrapperGLTest::constructMove() {
    GL::Shader glShaderA{
        #ifndef MAGNUM_TARGET_GLES
        GL::Version::GL300,
        #else
        GL::Version::GLES300,
        #endif
        GL::Shader::Type::Fragment};
    Implementation::GLShaderWrapper a{Utility::move(glShaderA)};

    GLuint id = a.id;
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_VERIFY(id > 0);

    Implementation::GLShaderWrapper b{Utility::move(a)};
    CORRADE_VERIFY(!a.id);
    CORRADE_COMPARE(b.id, id);
    CORRADE_COMPARE(b.type, GL_FRAGMENT_SHADER);

    GL::Shader glShaderB{
        #ifndef MAGNUM_TARGET_GLES
        GL::Version::GL210,
        #else
        GL::Version::GLES200,
        #endif
        GL::Shader::Type::Vertex};
    Implementation::GLShaderWrapper c{Utility::move(glShaderB)};

    GLuint cId = c.id;
    c = Utility::move(b);
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_VERIFY(cId > 0);
    CORRADE_COMPARE(b.id, cId);
    CORRADE_COMPARE(c.id, id);
    CORRADE_COMPARE(c.type, GL_FRAGMENT_SHADER);

    CORRADE_VERIFY(std::is_nothrow_move_constructible<Implementation::GLShaderWrapper>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<Implementation::GLShaderWrapper>::value);
}

void GLShaderWrapperGLTest::convert() {
    {
        GL::Shader glShader{
            #ifndef MAGNUM_TARGET_GLES
            GL::Version::GL300,
            #else
            GL::Version::GLES300,
            #endif
            GL::Shader::Type::Fragment};
        Implementation::GLShaderWrapper shader{Utility::move(glShader)};

        GLuint id = shader.id;
        MAGNUM_VERIFY_NO_GL_ERROR();
        CORRADE_VERIFY(id > 0);

        GL::Shader glShader2 = shader;
        MAGNUM_VERIFY_NO_GL_ERROR();

        MAGNUM_VERIFY_NO_GL_ERROR();
        CORRADE_COMPARE(glShader2.id(), id);
        CORRADE_COMPARE(glShader2.type(), GL::Shader::Type::Fragment);
        CORRADE_VERIFY(shader.id);
    }

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void GLShaderWrapperGLTest::convertRvalue() {
    {
        GL::Shader glShader{
            #ifndef MAGNUM_TARGET_GLES
            GL::Version::GL300,
            #else
            GL::Version::GLES300,
            #endif
            GL::Shader::Type::Fragment};
        Implementation::GLShaderWrapper shader{Utility::move(glShader)};

        GLuint id = shader.id;
        MAGNUM_VERIFY_NO_GL_ERROR();
        CORRADE_VERIFY(id > 0);

        GL::Shader glShader2 = Utility::move(shader);
        MAGNUM_VERIFY_NO_GL_ERROR();

        MAGNUM_VERIFY_NO_GL_ERROR();
        CORRADE_COMPARE(glShader2.id(), id);
        CORRADE_COMPARE(glShader2.type(), GL::Shader::Type::Fragment);
        CORRADE_VERIFY(!shader.id);
    }

    MAGNUM_VERIFY_NO_GL_ERROR();
}

}}}}

CORRADE_TEST_MAIN(Magnum::Shaders::Test::GLShaderWrapperGLTest)
