/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015
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
#include <Corrade/Utility/Resource.h>

#include "Magnum/AbstractShaderProgram.h"
#include "Magnum/Context.h"
#include "Magnum/Extensions.h"
#include "Magnum/Shader.h"
#include "Magnum/Math/Matrix.h"
#include "Magnum/Math/Vector4.h"
#include "Magnum/Test/AbstractOpenGLTester.h"

namespace Magnum { namespace Test {

struct AbstractShaderProgramGLTest: AbstractOpenGLTester {
    explicit AbstractShaderProgramGLTest();

    void construct();
    void constructCopy();
    void constructMove();

    void label();

    void create();
    void createMultipleOutputs();
    #ifndef MAGNUM_TARGET_GLES
    void createMultipleOutputsIndexed();
    #endif

    void uniformLocationOptimizedOut();
    void uniform();
    void uniformVector();
    void uniformMatrix();
    void uniformArray();
};

AbstractShaderProgramGLTest::AbstractShaderProgramGLTest() {
    addTests({&AbstractShaderProgramGLTest::construct,
              &AbstractShaderProgramGLTest::constructCopy,
              &AbstractShaderProgramGLTest::constructMove,

              &AbstractShaderProgramGLTest::label,

              &AbstractShaderProgramGLTest::create,
              &AbstractShaderProgramGLTest::createMultipleOutputs,
              #ifndef MAGNUM_TARGET_GLES
              &AbstractShaderProgramGLTest::createMultipleOutputsIndexed,
              #endif

              &AbstractShaderProgramGLTest::uniformLocationOptimizedOut,
              &AbstractShaderProgramGLTest::uniform,
              &AbstractShaderProgramGLTest::uniformVector,
              &AbstractShaderProgramGLTest::uniformMatrix,
              &AbstractShaderProgramGLTest::uniformArray});
}

namespace {
    class DummyShader: public AbstractShaderProgram {
        public:
            explicit DummyShader() {}
    };
}

void AbstractShaderProgramGLTest::construct() {
    {
        const DummyShader shader;

        MAGNUM_VERIFY_NO_ERROR();
        CORRADE_VERIFY(shader.id() > 0);
    }

    MAGNUM_VERIFY_NO_ERROR();
}

void AbstractShaderProgramGLTest::constructCopy() {
    CORRADE_VERIFY(!(std::is_constructible<DummyShader, const DummyShader&>{}));
    CORRADE_VERIFY(!(std::is_assignable<DummyShader, const DummyShader&>{}));
}

void AbstractShaderProgramGLTest::constructMove() {
    DummyShader a;
    const Int id = a.id();

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_VERIFY(id > 0);

    DummyShader b(std::move(a));

    CORRADE_COMPARE(a.id(), 0);
    CORRADE_COMPARE(b.id(), id);

    DummyShader c;
    const Int cId = c.id();
    c = std::move(b);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_VERIFY(cId > 0);
    CORRADE_COMPARE(b.id(), cId);
    CORRADE_COMPARE(c.id(), id);
}

void AbstractShaderProgramGLTest::label() {
    /* No-Op version is tested in AbstractObjectGLTest */
    if(!Context::current().isExtensionSupported<Extensions::GL::KHR::debug>() &&
       !Context::current().isExtensionSupported<Extensions::GL::EXT::debug_label>())
        CORRADE_SKIP("Required extension is not available");

    DummyShader shader;
    CORRADE_COMPARE(shader.label(), "");

    shader.setLabel("DummyShader");
    CORRADE_COMPARE(shader.label(), "DummyShader");

    MAGNUM_VERIFY_NO_ERROR();
}

namespace {
    struct MyPublicShader: AbstractShaderProgram {
        using AbstractShaderProgram::attachShaders;
        using AbstractShaderProgram::bindAttributeLocation;
        #ifndef MAGNUM_TARGET_GLES
        using AbstractShaderProgram::bindFragmentDataLocationIndexed;
        using AbstractShaderProgram::bindFragmentDataLocation;
        #endif
        using AbstractShaderProgram::link;
        using AbstractShaderProgram::uniformLocation;
    };
}

void AbstractShaderProgramGLTest::create() {
    Utility::Resource rs("AbstractShaderProgramGLTest");

    #ifndef MAGNUM_TARGET_GLES
    Shader vert(
        #ifndef CORRADE_TARGET_APPLE
        Version::GL210
        #else
        Version::GL310
        #endif
        , Shader::Type::Vertex);
    #else
    Shader vert(Version::GLES200, Shader::Type::Vertex);
    #endif
    vert.addSource(rs.get("MyShader.vert"));
    const bool vertCompiled = vert.compile();

    #ifndef MAGNUM_TARGET_GLES
    Shader frag(
        #ifndef CORRADE_TARGET_APPLE
        Version::GL210
        #else
        Version::GL310
        #endif
        , Shader::Type::Fragment);
    #else
    Shader frag(Version::GLES200, Shader::Type::Fragment);
    #endif
    frag.addSource(rs.get("MyShader.frag"));
    const bool fragCompiled = frag.compile();

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_VERIFY(vertCompiled);
    CORRADE_VERIFY(fragCompiled);

    MyPublicShader program;
    program.attachShaders({vert, frag});

    MAGNUM_VERIFY_NO_ERROR();

    program.bindAttributeLocation(0, "position");
    const bool linked = program.link();
    const bool valid = program.validate().first;

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_VERIFY(linked);
    CORRADE_VERIFY(valid);

    const Int matrixUniform = program.uniformLocation("matrix");
    const Int multiplierUniform = program.uniformLocation("multiplier");
    const Int colorUniform = program.uniformLocation("color");
    const Int additionsUniform = program.uniformLocation("additions");

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_VERIFY(matrixUniform >= 0);
    CORRADE_VERIFY(multiplierUniform >= 0);
    CORRADE_VERIFY(colorUniform >= 0);
    CORRADE_VERIFY(additionsUniform >= 0);
}

void AbstractShaderProgramGLTest::createMultipleOutputs() {
    #ifndef MAGNUM_TARGET_GLES
    Utility::Resource rs("AbstractShaderProgramGLTest");

    Shader vert(
        #ifndef CORRADE_TARGET_APPLE
        Version::GL210
        #else
        Version::GL310
        #endif
        , Shader::Type::Vertex);
    vert.addSource(rs.get("MyShader.vert"));
    const bool vertCompiled = vert.compile();

    Shader frag(
        #ifndef CORRADE_TARGET_APPLE
        Version::GL300
        #else
        Version::GL310
        #endif
        , Shader::Type::Fragment);
    frag.addSource(rs.get("MyShaderFragmentOutputs.frag"));
    const bool fragCompiled = frag.compile();

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_VERIFY(vertCompiled);
    CORRADE_VERIFY(fragCompiled);

    MyPublicShader program;
    program.attachShaders({vert, frag});

    MAGNUM_VERIFY_NO_ERROR();

    program.bindAttributeLocation(0, "position");
    program.bindFragmentDataLocation(0, "first");
    program.bindFragmentDataLocation(1, "second");
    const bool linked = program.link();
    const bool valid = program.validate().first;

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_VERIFY(linked);
    CORRADE_VERIFY(valid);
    #elif !defined(MAGNUM_TARGET_GLES2)
    CORRADE_SKIP("Only explicit location specification supported in ES 3.0.");
    #else
    CORRADE_SKIP("Only gl_FragData[n] supported in ES 2.0.");
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void AbstractShaderProgramGLTest::createMultipleOutputsIndexed() {
    Utility::Resource rs("AbstractShaderProgramGLTest");

    Shader vert(
        #ifndef CORRADE_TARGET_APPLE
        Version::GL210
        #else
        Version::GL310
        #endif
        , Shader::Type::Vertex);
    vert.addSource(rs.get("MyShader.vert"));
    const bool vertCompiled = vert.compile();

    Shader frag(
        #ifndef CORRADE_TARGET_APPLE
        Version::GL300
        #else
        Version::GL310
        #endif
        , Shader::Type::Fragment);
    frag.addSource(rs.get("MyShaderFragmentOutputs.frag"));
    const bool fragCompiled = frag.compile();

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_VERIFY(vertCompiled);
    CORRADE_VERIFY(fragCompiled);

    MyPublicShader program;
    program.attachShaders({vert, frag});

    MAGNUM_VERIFY_NO_ERROR();

    program.bindAttributeLocation(0, "position");
    program.bindFragmentDataLocationIndexed(0, 0, "first");
    program.bindFragmentDataLocationIndexed(0, 1, "second");
    const bool linked = program.link();
    const bool valid = program.validate().first;

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_VERIFY(linked);
    CORRADE_VERIFY(valid);
}
#endif

void AbstractShaderProgramGLTest::uniformLocationOptimizedOut() {
    MyPublicShader program;

    #ifndef MAGNUM_TARGET_GLES
    Shader vert(
        #ifndef CORRADE_TARGET_APPLE
        Version::GL210
        #else
        Version::GL310
        #endif
        , Shader::Type::Vertex);
    Shader frag(
        #ifndef CORRADE_TARGET_APPLE
        Version::GL210
        #else
        Version::GL310
        #endif
        , Shader::Type::Fragment);
    #else
    Shader vert(Version::GLES200, Shader::Type::Vertex);
    Shader frag(Version::GLES200, Shader::Type::Fragment);
    #endif
    vert.addSource("void main() { gl_Position = vec4(0.0); }");
    frag.addSource(
        #ifndef CORRADE_TARGET_APPLE
        "void main() { gl_FragColor = vec4(1.0); }"
        #else
        "out vec4 color;\n"
        "void main() { color = vec4(1.0); }"
        #endif
        );

    CORRADE_VERIFY(Shader::compile({vert, frag}));
    program.attachShaders({vert, frag});
    CORRADE_VERIFY(program.link());

    std::ostringstream out;
    Warning::setOutput(&out);
    program.uniformLocation("nonexistent");
    program.uniformLocation(std::string{"another"});
    CORRADE_COMPARE(out.str(),
        "AbstractShaderProgram: location of uniform 'nonexistent' cannot be retrieved!\n"
        "AbstractShaderProgram: location of uniform 'another' cannot be retrieved!\n");
}

namespace {
    struct MyShader: AbstractShaderProgram {
        explicit MyShader();

        using AbstractShaderProgram::setUniform;

        Int matrixUniform,
            multiplierUniform,
            colorUniform,
            additionsUniform;
    };
}

#ifndef DOXYGEN_GENERATING_OUTPUT
MyShader::MyShader() {
    Utility::Resource rs("AbstractShaderProgramGLTest");

    #ifndef MAGNUM_TARGET_GLES
    Shader vert(
        #ifndef CORRADE_TARGET_APPLE
        Version::GL210
        #else
        Version::GL310
        #endif
        , Shader::Type::Vertex);
    Shader frag(
        #ifndef CORRADE_TARGET_APPLE
        Version::GL210
        #else
        Version::GL310
        #endif
        , Shader::Type::Fragment);
    #else
    Shader vert(Version::GLES200, Shader::Type::Vertex);
    Shader frag(Version::GLES200, Shader::Type::Fragment);
    #endif
    vert.addSource(rs.get("MyShader.vert"));
    frag.addSource(rs.get("MyShader.frag"));

    Shader::compile({vert, frag});

    attachShaders({vert, frag});

    bindAttributeLocation(0, "position");
    link();

    matrixUniform = uniformLocation("matrix");
    multiplierUniform = uniformLocation("multiplier");
    colorUniform = uniformLocation("color");
    additionsUniform = uniformLocation("additions");
}
#endif

void AbstractShaderProgramGLTest::uniform() {
    MyShader shader;

    MAGNUM_VERIFY_NO_ERROR();

    shader.setUniform(shader.multiplierUniform, 0.35f);

    MAGNUM_VERIFY_NO_ERROR();
}

void AbstractShaderProgramGLTest::uniformVector() {
    MyShader shader;

    MAGNUM_VERIFY_NO_ERROR();

    shader.setUniform(shader.colorUniform, Vector4(0.3f, 0.7f, 1.0f, 0.25f));

    MAGNUM_VERIFY_NO_ERROR();
}

void AbstractShaderProgramGLTest::uniformMatrix() {
    MyShader shader;

    MAGNUM_VERIFY_NO_ERROR();

    shader.setUniform(shader.matrixUniform, Matrix4x4::fromDiagonal({0.3f, 0.7f, 1.0f, 0.25f}));

    MAGNUM_VERIFY_NO_ERROR();
}

void AbstractShaderProgramGLTest::uniformArray() {
    MyShader shader;

    MAGNUM_VERIFY_NO_ERROR();

    /* Testing also implicit conversion to base type (Vector4[] -> Math::Vector<4, Float>[]) */
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* Causes ICE */
    constexpr
    #endif
    const Vector4 values[] = {
        {0.5f, 1.0f, 0.4f, 0.0f},
        {0.0f, 0.1f, 0.7f, 0.3f},
        {0.9f, 0.8f, 0.3f, 0.1f}
    };
    shader.setUniform(shader.additionsUniform, values);

    MAGNUM_VERIFY_NO_ERROR();
}

}}

MAGNUM_GL_TEST_MAIN(Magnum::Test::AbstractShaderProgramGLTest)
