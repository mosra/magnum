/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2022 Vladislav Oleshko <vladislav.oleshko@gmail.com>

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

#include <Corrade/Containers/Iterable.h>
#include <Corrade/Containers/Pair.h>
#include <Corrade/Containers/Reference.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/TestSuite/Compare/Numeric.h>
#include <Corrade/TestSuite/Compare/String.h>
#include <Corrade/Utility/Resource.h>
#include <Corrade/Utility/System.h>

#include "Magnum/Image.h"
#include "Magnum/ImageView.h"
#include "Magnum/GL/AbstractShaderProgram.h"
#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
#include "Magnum/GL/ImageFormat.h"
#endif
#include "Magnum/GL/Mesh.h"
#include "Magnum/GL/MeshView.h"
#include "Magnum/GL/OpenGLTester.h"
#include "Magnum/GL/PixelFormat.h"
#include "Magnum/GL/Shader.h"
#include "Magnum/GL/Texture.h"
#include "Magnum/GL/TextureFormat.h"
#ifndef MAGNUM_TARGET_GLES
#include "Magnum/GL/TransformFeedback.h"
#endif
#include "Magnum/Math/Matrix.h"
#include "Magnum/Math/Vector4.h"
#include "Magnum/Math/Color.h"

#ifndef MAGNUM_TARGET_WEBGL
#include <Corrade/Containers/String.h>
#endif

namespace Magnum { namespace GL { namespace Test { namespace {

struct AbstractShaderProgramGLTest: OpenGLTester {
    explicit AbstractShaderProgramGLTest();

    void construct();
    void constructMove();

    #ifndef MAGNUM_TARGET_WEBGL
    void label();
    #endif

    void create();
    void createMultipleOutputs();
    #ifndef MAGNUM_TARGET_GLES2
    void createMultipleOutputsIndexed();
    #endif
    void createAsync();

    void linkFailure();
    void linkFailureAsync();
    void linkFailureAsyncShaderList();

    void validateFailure();

    void uniformNotFound();

    void uniform();
    void uniformVector();
    void uniformMatrix();
    void uniformArray();
    #ifndef MAGNUM_TARGET_GLES
    void uniformDouble();
    void uniformDoubleVector();
    void uniformDoubleMatrix();
    void uniformDoubleArray();
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    void createUniformBlocks();
    void uniformBlockIndexNotFound();
    void uniformBlock();

    #ifndef MAGNUM_TARGET_WEBGL
    void compute();
    #endif
    #endif

    void subclassDraw();
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    void subclassDispatch();
    #endif
};

using namespace Containers::Literals;

const struct {
    const char* name;
    Containers::StringView positionName, matrixName, multiplierName, colorName, additionsName;
} CreateData[]{
    {"",
        "position",
        "matrix",
        "multiplier",
        "color",
        "additions"},
    {"non-null-terminated strings",
        "position!"_s.exceptSuffix(1),
        "matrix!"_s.exceptSuffix(1),
        "multiplier!"_s.exceptSuffix(1),
        "color!"_s.exceptSuffix(1),
        "additions!"_s.exceptSuffix(1)},
};

const struct {
    const char* name;
    Containers::StringView firstName, secondName;
} CreateMultipleOutputsData[]{
    {"",
        "first",
        "second"},
    {"non-null-terminated strings",
        "first!"_s.exceptSuffix(1),
        "second!"_s.exceptSuffix(1)}
};

#ifndef MAGNUM_TARGET_GLES2
const struct {
    const char* name;
    Containers::StringView matricesName, materialName;
} CreateUniformBlocksData[]{
    {"",
        "matrices",
        "material"},
    {"non-null-terminated strings",
        "matrices!"_s.exceptSuffix(1),
        "material!"_s.exceptSuffix(1)}
};
#endif

AbstractShaderProgramGLTest::AbstractShaderProgramGLTest() {
    addTests({&AbstractShaderProgramGLTest::construct,
              &AbstractShaderProgramGLTest::constructMove,

              #ifndef MAGNUM_TARGET_WEBGL
              &AbstractShaderProgramGLTest::label,
              #endif
              });

    addInstancedTests({&AbstractShaderProgramGLTest::create},
        Containers::arraySize(CreateData));

    addTests({&AbstractShaderProgramGLTest::createAsync});

    addInstancedTests({
        &AbstractShaderProgramGLTest::createMultipleOutputs,
        #ifndef MAGNUM_TARGET_GLES2
        &AbstractShaderProgramGLTest::createMultipleOutputsIndexed,
        #endif
    }, Containers::arraySize(CreateMultipleOutputsData));

    addTests({&AbstractShaderProgramGLTest::linkFailure,
              &AbstractShaderProgramGLTest::linkFailureAsync,
              &AbstractShaderProgramGLTest::linkFailureAsyncShaderList,

              &AbstractShaderProgramGLTest::validateFailure,

              &AbstractShaderProgramGLTest::uniformNotFound,

              &AbstractShaderProgramGLTest::uniform,
              &AbstractShaderProgramGLTest::uniformVector,
              &AbstractShaderProgramGLTest::uniformMatrix,
              &AbstractShaderProgramGLTest::uniformArray,
              #ifndef MAGNUM_TARGET_GLES
              &AbstractShaderProgramGLTest::uniformDouble,
              &AbstractShaderProgramGLTest::uniformDoubleVector,
              &AbstractShaderProgramGLTest::uniformDoubleMatrix,
              &AbstractShaderProgramGLTest::uniformDoubleArray,
              #endif
              });

    #ifndef MAGNUM_TARGET_GLES2
    addInstancedTests({&AbstractShaderProgramGLTest::createUniformBlocks},
        Containers::arraySize(CreateUniformBlocksData));
    #endif

    addTests({
              #ifndef MAGNUM_TARGET_GLES2
              &AbstractShaderProgramGLTest::uniformBlockIndexNotFound,
              &AbstractShaderProgramGLTest::uniformBlock,

              #ifndef MAGNUM_TARGET_WEBGL
              &AbstractShaderProgramGLTest::compute,
              #endif
              #endif

              &AbstractShaderProgramGLTest::subclassDraw,
              #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
              &AbstractShaderProgramGLTest::subclassDispatch
              #endif
              });
}

using namespace Containers::Literals;

class DummyShader: public AbstractShaderProgram {
    public:
        explicit DummyShader() {}
};

void AbstractShaderProgramGLTest::construct() {
    {
        const DummyShader shader;

        MAGNUM_VERIFY_NO_GL_ERROR();
        CORRADE_VERIFY(shader.id() > 0);
    }

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void AbstractShaderProgramGLTest::constructMove() {
    DummyShader a;
    const Int id = a.id();

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_VERIFY(id > 0);

    DummyShader b(Utility::move(a));

    CORRADE_COMPARE(a.id(), 0);
    CORRADE_COMPARE(b.id(), id);

    DummyShader c;
    const Int cId = c.id();
    c = Utility::move(b);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_VERIFY(cId > 0);
    CORRADE_COMPARE(b.id(), cId);
    CORRADE_COMPARE(c.id(), id);

    CORRADE_VERIFY(std::is_nothrow_move_constructible<DummyShader>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<DummyShader>::value);
}

#ifndef MAGNUM_TARGET_WEBGL
void AbstractShaderProgramGLTest::label() {
    /* No-Op version is tested in AbstractObjectGLTest */
    if(!Context::current().isExtensionSupported<Extensions::KHR::debug>() &&
       !Context::current().isExtensionSupported<Extensions::EXT::debug_label>())
        CORRADE_SKIP("Required extension is not available");

    DummyShader shader;
    CORRADE_COMPARE(shader.label(), "");

    /* Test the string size gets correctly used, instead of relying on null
       termination */
    shader.setLabel("DummyShader!"_s.exceptSuffix(1));
    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(shader.label(), "DummyShader");
    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

struct MyPublicShader: AbstractShaderProgram {
    using AbstractShaderProgram::attachShaders;
    using AbstractShaderProgram::bindAttributeLocation;
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    using AbstractShaderProgram::bindFragmentDataLocationIndexed;
    using AbstractShaderProgram::bindFragmentDataLocation;
    #endif
    using AbstractShaderProgram::link;
    using AbstractShaderProgram::submitLink;
    using AbstractShaderProgram::checkLink;
    using AbstractShaderProgram::uniformLocation;
    #ifndef MAGNUM_TARGET_GLES2
    using AbstractShaderProgram::uniformBlockIndex;
    #endif
    using AbstractShaderProgram::setUniform;
};

void AbstractShaderProgramGLTest::create() {
    auto&& data = CreateData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Utility::Resource rs("AbstractShaderProgramGLTest");

    Shader vert(
        #ifndef MAGNUM_TARGET_GLES
        #ifndef CORRADE_TARGET_APPLE
        Version::GL210
        #else
        Version::GL310
        #endif
        #else
        Version::GLES200
        #endif
        , Shader::Type::Vertex);
    vert.addSource(rs.getString("MyShader.vert"));
    const bool vertCompiled = vert.compile();

    Shader frag(
        #ifndef MAGNUM_TARGET_GLES
        #ifndef CORRADE_TARGET_APPLE
        Version::GL210
        #else
        Version::GL310
        #endif
        #else
        Version::GLES200
        #endif
        , Shader::Type::Fragment);
    frag.addSource(rs.getString("MyShader.frag"));
    const bool fragCompiled = frag.compile();

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_VERIFY(vertCompiled);
    CORRADE_VERIFY(fragCompiled);

    MyPublicShader program;
    program.attachShaders({vert, frag});

    MAGNUM_VERIFY_NO_GL_ERROR();

    program.bindAttributeLocation(0, data.positionName);
    const bool linked = program.link();
    const bool valid = program.validate().first();

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_VERIFY(linked);

    /* Some drivers need a bit of time to update this result */
    Utility::System::sleep(200);
    CORRADE_VERIFY(program.isLinkFinished());
    {
        #if defined(CORRADE_TARGET_APPLE) && !defined(MAGNUM_TARGET_GLES)
        CORRADE_EXPECT_FAIL("macOS drivers need insane amount of state to validate properly.");
        #endif
        CORRADE_VERIFY(valid);
    }

    const Int matrixUniform = program.uniformLocation(data.matrixName);
    const Int multiplierUniform = program.uniformLocation(data.multiplierName);
    const Int colorUniform = program.uniformLocation(data.colorName);
    const Int additionsUniform = program.uniformLocation(data.additionsName);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_VERIFY(matrixUniform >= 0);
    CORRADE_VERIFY(multiplierUniform >= 0);
    CORRADE_VERIFY(colorUniform >= 0);
    CORRADE_VERIFY(additionsUniform >= 0);
}

void AbstractShaderProgramGLTest::createAsync() {
    Utility::Resource rs("AbstractShaderProgramGLTest");

    Shader vert(
        #ifndef MAGNUM_TARGET_GLES
        #ifndef CORRADE_TARGET_APPLE
        Version::GL210
        #else
        Version::GL310
        #endif
        #else
        Version::GLES200
        #endif
        , Shader::Type::Vertex);
    vert.addSource(rs.getString("MyShader.vert"));
    const bool vertCompiled = vert.compile();

    Shader frag(
        #ifndef MAGNUM_TARGET_GLES
        #ifndef CORRADE_TARGET_APPLE
        Version::GL210
        #else
        Version::GL310
        #endif
        #else
        Version::GLES200
        #endif
        , Shader::Type::Fragment);
    frag.addSource(rs.getString("MyShader.frag"));
    const bool fragCompiled = frag.compile();

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_VERIFY(vertCompiled);
    CORRADE_VERIFY(fragCompiled);

    MyPublicShader program;
    program.attachShaders({vert, frag});

    MAGNUM_VERIFY_NO_GL_ERROR();

    program.bindAttributeLocation(0, "position");
    program.submitLink();

    while(!program.isLinkFinished())
        Utility::System::sleep(100);

    CORRADE_VERIFY(program.checkLink({vert, frag}));
    CORRADE_VERIFY(program.isLinkFinished());
    const bool valid = program.validate().first();

    MAGNUM_VERIFY_NO_GL_ERROR();
    {
        #if defined(CORRADE_TARGET_APPLE) && !defined(MAGNUM_TARGET_GLES)
        CORRADE_EXPECT_FAIL("macOS drivers need insane amount of state to validate properly.");
        #endif
        CORRADE_VERIFY(valid);
    }

    const Int matrixUniform = program.uniformLocation("matrix");
    const Int multiplierUniform = program.uniformLocation("multiplier");
    const Int colorUniform = program.uniformLocation("color");
    const Int additionsUniform = program.uniformLocation("additions");

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_VERIFY(matrixUniform >= 0);
    CORRADE_VERIFY(multiplierUniform >= 0);
    CORRADE_VERIFY(colorUniform >= 0);
    CORRADE_VERIFY(additionsUniform >= 0);
}

void AbstractShaderProgramGLTest::createMultipleOutputs() {
    auto&& data = CreateMultipleOutputsData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #else
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::blend_func_extended>())
        CORRADE_SKIP(GL::Extensions::EXT::blend_func_extended::string() << "is not supported.");
    #endif

    Utility::Resource rs("AbstractShaderProgramGLTest");

    Shader vert(
        #ifdef MAGNUM_TARGET_GLES
        Version::GLES300
        #elif !defined(CORRADE_TARGET_APPLE)
        Version::GL210
        #else
        Version::GL310
        #endif
        , Shader::Type::Vertex);
    vert.addSource(rs.getString("MyShader.vert"));
    const bool vertCompiled = vert.compile();

    Shader frag(
        #ifdef MAGNUM_TARGET_GLES
        Version::GLES300
        #elif !defined(CORRADE_TARGET_APPLE)
        Version::GL300
        #else
        Version::GL310
        #endif
        , Shader::Type::Fragment);
    frag.addSource(rs.getString("MyShaderFragmentOutputs.frag"));
    const bool fragCompiled = frag.compile();

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_VERIFY(vertCompiled);
    CORRADE_VERIFY(fragCompiled);

    MyPublicShader program;
    program.attachShaders({vert, frag});

    MAGNUM_VERIFY_NO_GL_ERROR();

    program.bindAttributeLocation(0, "position");
    /* Testing just what wasn't verified for non-null-terminated strings in
       create() already */
    program.bindFragmentDataLocation(0, data.firstName);
    program.bindFragmentDataLocation(1, data.secondName);
    const bool linked = program.link();
    const bool valid = program.validate().first();

    MAGNUM_VERIFY_NO_GL_ERROR();
    {
        #ifdef MAGNUM_TARGET_GLES
        CORRADE_EXPECT_FAIL_IF(Context::current().detectedDriver() & Context::DetectedDriver::NVidia,
            "NVidia drivers don't take glBindFragDataLocationEXT() into account on ES.");
        #endif
        CORRADE_VERIFY(linked);
        if(!linked)
            return;
    } {
        #ifdef CORRADE_TARGET_APPLE
        CORRADE_EXPECT_FAIL("macOS drivers need insane amount of state to validate properly.");
        #endif
        CORRADE_VERIFY(valid);
    }
    #elif defined(MAGNUM_TARGET_WEBGL)
    CORRADE_SKIP("Only explicit location specification supported in WebGL 2.0.");
    #else
    CORRADE_SKIP("Only gl_FragData[n] supported in ES 2.0.");
    #endif
}

#ifndef MAGNUM_TARGET_GLES2
void AbstractShaderProgramGLTest::createMultipleOutputsIndexed() {
    auto&& data = CreateMultipleOutputsData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_WEBGL
    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::blend_func_extended>())
        CORRADE_SKIP(GL::Extensions::ARB::blend_func_extended::string() << "is not supported.");
    #else
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::blend_func_extended>())
        CORRADE_SKIP(GL::Extensions::EXT::blend_func_extended::string() << "is not supported.");
    #endif

    Utility::Resource rs("AbstractShaderProgramGLTest");

    Shader vert(
        #ifdef MAGNUM_TARGET_GLES
        Version::GLES300
        #elif !defined(CORRADE_TARGET_APPLE)
        Version::GL210
        #else
        Version::GL310
        #endif
        , Shader::Type::Vertex);
    vert.addSource(rs.getString("MyShader.vert"));
    const bool vertCompiled = vert.compile();

    Shader frag(
        #ifdef MAGNUM_TARGET_GLES
        Version::GLES300
        #elif !defined(CORRADE_TARGET_APPLE)
        Version::GL300
        #else
        Version::GL310
        #endif
        , Shader::Type::Fragment);
    frag.addSource(rs.getString("MyShaderFragmentOutputs.frag"));
    const bool fragCompiled = frag.compile();

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_VERIFY(vertCompiled);
    CORRADE_VERIFY(fragCompiled);

    MyPublicShader program;
    program.attachShaders({vert, frag});

    MAGNUM_VERIFY_NO_GL_ERROR();

    program.bindAttributeLocation(0, "position");
    program.bindFragmentDataLocationIndexed(0, 0, data.firstName);
    program.bindFragmentDataLocationIndexed(0, 1, data.secondName);
    const bool linked = program.link();
    const bool valid = program.validate().first();

    MAGNUM_VERIFY_NO_GL_ERROR();
    {
        #ifdef MAGNUM_TARGET_GLES
        CORRADE_EXPECT_FAIL_IF(Context::current().detectedDriver() & Context::DetectedDriver::NVidia,
            "NVidia drivers don't take glBindFragDataLocationEXT() into account on ES.");
        #endif
        CORRADE_VERIFY(linked);
        if(!linked)
            return;
    } {
        #ifdef CORRADE_TARGET_APPLE
        CORRADE_EXPECT_FAIL("macOS drivers need insane amount of state to validate properly.");
        #endif
        CORRADE_VERIFY(valid);
    }
    #else
    CORRADE_SKIP("Only explicit location specification supported in WebGL 2.0.");
    #endif
}
#endif

void AbstractShaderProgramGLTest::linkFailure() {
    Shader shader(
        #ifndef MAGNUM_TARGET_GLES
        #ifndef CORRADE_TARGET_APPLE
        Version::GL210
        #else
        Version::GL310
        #endif
        #else
        Version::GLES200
        #endif
        , Shader::Type::Fragment);
    shader.addSource("[fu] bleh error #:! stuff\n");

    /* The compilation should fail */
    {
        Error redirectError{nullptr};
        CORRADE_VERIFY(!shader.compile());
    }

    MyPublicShader program;
    program.attachShaders({shader});

    /* And thus linking as well, saying something like "error: linking with
       uncompiled/unspecialized shader" */
    Containers::String out;
    {
        Error redirectError{&out};
        CORRADE_VERIFY(!program.link());
    }

    Utility::System::sleep(200);
    CORRADE_VERIFY(program.isLinkFinished());

    /* There's a driver-specific message after */
    CORRADE_COMPARE_AS(out, "GL::AbstractShaderProgram::link(): linking failed with the following message:",
        TestSuite::Compare::StringHasPrefix);
    /* No stray \0 should be anywhere */
    CORRADE_COMPARE_AS(out, "\0"_s, TestSuite::Compare::StringNotContains);
    /* The message should end with a newline */
    CORRADE_COMPARE_AS(out, "\n"_s, TestSuite::Compare::StringHasSuffix);
}

void AbstractShaderProgramGLTest::linkFailureAsync() {
    Shader shader(
        #ifndef MAGNUM_TARGET_GLES
        #ifndef CORRADE_TARGET_APPLE
        Version::GL210
        #else
        Version::GL310
        #endif
        #else
        Version::GLES200
        #endif
        , Shader::Type::Fragment);
    shader.addSource("[fu] bleh error #:! stuff\n");

    /* The compilation should fail */
    {
        Error redirectError{nullptr};
        CORRADE_VERIFY(!shader.compile());
    }

    MyPublicShader program;
    program.attachShaders({shader});

    /* The link submission should not print anything ... */
    Containers::String out;
    {
        Error redirectError{&out};
        program.submitLink();
    }

    while(!program.isLinkFinished())
        Utility::System::sleep(100);

    CORRADE_COMPARE(out, "");

    /* ... only the final check should. In this case it's "error: linking with
       uncompiled/unspecialized shader" as well, but if the shaders would be
       supplied like in linkFailureAsyncShaderList() below, it'd print the
       shader failure instead. */
    {
        Error redirectError{&out};
        CORRADE_VERIFY(!program.checkLink({}));
    }
    CORRADE_VERIFY(program.isLinkFinished());
    CORRADE_COMPARE_AS(out, "GL::AbstractShaderProgram::link(): linking failed with the following message:",
        TestSuite::Compare::StringHasPrefix);

    /* Not testing presence of \0 etc., as that's tested well enough in
       linkFailure() above already and both cases use the same error printing
       code path */
}

void AbstractShaderProgramGLTest::linkFailureAsyncShaderList() {
    Shader vert(
        #ifndef MAGNUM_TARGET_GLES
        #ifndef CORRADE_TARGET_APPLE
        Version::GL210
        #else
        Version::GL310
        #endif
        #else
        Version::GLES200
        #endif
        , Shader::Type::Vertex);
    vert.addSource("void main() {}\n");

    Shader frag(
        #ifndef MAGNUM_TARGET_GLES
        #ifndef CORRADE_TARGET_APPLE
        Version::GL210
        #else
        Version::GL310
        #endif
        #else
        Version::GLES200
        #endif
        , Shader::Type::Fragment);
    frag.addSource("[fu] bleh error #:! stuff\n");

    vert.submitCompile();
    frag.submitCompile();

    MyPublicShader program;
    program.attachShaders({vert, frag});

    /* The link submission should not print anything ... */
    {
        Containers::String out;
        Error redirectError{&out};
        program.submitLink();
        CORRADE_COMPARE(out, "");
    }

    /* ... only the final check should. Vertex shader should be fine, but
       fragment should fail. */
    Containers::String out;
    {
        Error redirectError{&out};
        CORRADE_VERIFY(!program.checkLink({vert, frag}));
    }
    CORRADE_COMPARE_AS(out, "GL::Shader::compile(): compilation of fragment shader failed with the following message:",
        TestSuite::Compare::StringHasPrefix);

    /* The linker error (which would most probably say something like "error:
       linking with uncompiled/unspecialized shader") should not be even
       printed */
    CORRADE_COMPARE_AS(out, "GL::AbstractShaderProgram::link(): linking failed with the following message:",
        TestSuite::Compare::StringNotContains);
}

void AbstractShaderProgramGLTest::validateFailure() {
    #ifdef MAGNUM_TARGET_GLES2
    CORRADE_SKIP("No known case where glValidateProgram() would fail on ES2.");
    #else
    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isVersionSupported(Version::GL300))
        CORRADE_SKIP(Version::GL300 << "is not supported");
    #endif

    Shader vert(
        #ifndef MAGNUM_TARGET_GLES
        #ifndef CORRADE_TARGET_APPLE
        Version::GL300
        #else
        Version::GL310
        #endif
        #else
        Version::GLES300
        #endif
        , Shader::Type::Vertex);
    vert.addSource(R"(void main() {
    gl_Position = vec4(0.0);
})");
    CORRADE_VERIFY(vert.compile());

    Shader frag(
        #ifndef MAGNUM_TARGET_GLES
        #ifndef CORRADE_TARGET_APPLE
        Version::GL300
        #else
        Version::GL310
        #endif
        #else
        Version::GLES300
        #endif
        , Shader::Type::Fragment);
    frag.addSource(R"(
uniform highp sampler2D textureData2D;
uniform highp sampler3D textureData3D;

out highp vec4 fragmentColor;

void main() {
    fragmentColor =
        texture(textureData2D, vec2(0.0))*
        texture(textureData3D, vec3(0.0));
})");
    CORRADE_VERIFY(frag.compile());

    MyPublicShader program;
    program.attachShaders({vert, frag});
    CORRADE_VERIFY(program.link());
    MAGNUM_VERIFY_NO_GL_ERROR();

    /* Set both samplers to the same location. On Mesa at least, this is done
       implicitly (and so validation fails right after linking the shader), but
       it won't hurt to be explicitly. Also, funnily enough, if I'd set those
       to 0 and 1 after and validate() again, the validation returns true but
       the message still contains the original message. Heh. */
    program.setUniform(program.uniformLocation("textureData2D"), 0);
    program.setUniform(program.uniformLocation("textureData3D"), 0);

    Containers::Pair<bool, Containers::String> result = program.validate();
    MAGNUM_VERIFY_NO_GL_ERROR();
    {
        CORRADE_EXPECT_FAIL_IF(Context::current().detectedDriver() & Context::DetectedDriver::NVidia, "NVidia doesn't treat conflicting sampler locations as a failure.");
        CORRADE_VERIFY(!result.first());
        /* The message shouldn't be empty */
        CORRADE_COMPARE_AS(result.second(),
            "",
            TestSuite::Compare::NotEqual);
    }
    /* No stray \0 or \n should be anywhere */
    CORRADE_COMPARE_AS(result.second(), "\0"_s, TestSuite::Compare::StringNotContains);
    CORRADE_COMPARE_AS(result.second(), "\n"_s, TestSuite::Compare::StringNotContains);
    #endif
}

void AbstractShaderProgramGLTest::uniformNotFound() {
    MyPublicShader program;

    Shader vert(
        #ifndef MAGNUM_TARGET_GLES
        #ifndef CORRADE_TARGET_APPLE
        Version::GL210
        #else
        Version::GL310
        #endif
        #else
        Version::GLES200
        #endif
        , Shader::Type::Vertex);
    Shader frag(
        #ifndef MAGNUM_TARGET_GLES
        #ifndef CORRADE_TARGET_APPLE
        Version::GL210
        #else
        Version::GL310
        #endif
        #else
        Version::GLES200
        #endif
        , Shader::Type::Fragment);
    vert.addSource("void main() { gl_Position = vec4(0.0); }");
    frag.addSource(
        #if !defined(CORRADE_TARGET_APPLE) || defined(MAGNUM_TARGET_GLES)
        "void main() { gl_FragColor = vec4(1.0); }"
        #else
        "out vec4 color;\n"
        "void main() { color = vec4(1.0); }"
        #endif
        );

    CORRADE_VERIFY(vert.compile() && frag.compile());

    program.attachShaders({vert, frag});
    CORRADE_VERIFY(program.link());

    Containers::String out;
    Warning redirectWarning{&out};
    program.uniformLocation("nonexistent");
    CORRADE_COMPARE(out,
        "GL::AbstractShaderProgram: location of uniform 'nonexistent' cannot be retrieved\n");
}

struct MyShader: AbstractShaderProgram {
    explicit MyShader();

    using AbstractShaderProgram::setUniform;

    Int matrixUniform,
        multiplierUniform,
        colorUniform,
        additionsUniform;
};

MyShader::MyShader() {
    Utility::Resource rs("AbstractShaderProgramGLTest");

    Shader vert(
        #ifndef MAGNUM_TARGET_GLES
        #ifndef CORRADE_TARGET_APPLE
        Version::GL210
        #else
        Version::GL310
        #endif
        #else
        Version::GLES200
        #endif
        , Shader::Type::Vertex);
    Shader frag(
        #ifndef MAGNUM_TARGET_GLES
        #ifndef CORRADE_TARGET_APPLE
        Version::GL210
        #else
        Version::GL310
        #endif
        #else
        Version::GLES200
        #endif
        , Shader::Type::Fragment);
    vert.addSource(rs.getString("MyShader.vert"))
        .compile();
    frag.addSource(rs.getString("MyShader.frag"))
        .compile();

    attachShaders({vert, frag});

    bindAttributeLocation(0, "position");
    link();

    matrixUniform = uniformLocation("matrix");
    multiplierUniform = uniformLocation("multiplier");
    colorUniform = uniformLocation("color");
    additionsUniform = uniformLocation("additions");
}

void AbstractShaderProgramGLTest::uniform() {
    MyShader shader;

    MAGNUM_VERIFY_NO_GL_ERROR();

    shader.setUniform(shader.multiplierUniform, 0.35f);

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void AbstractShaderProgramGLTest::uniformVector() {
    MyShader shader;

    MAGNUM_VERIFY_NO_GL_ERROR();

    shader.setUniform(shader.colorUniform, Vector4(0.3f, 0.7f, 1.0f, 0.25f));

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void AbstractShaderProgramGLTest::uniformMatrix() {
    MyShader shader;

    MAGNUM_VERIFY_NO_GL_ERROR();

    shader.setUniform(shader.matrixUniform, Matrix4x4::fromDiagonal({0.3f, 0.7f, 1.0f, 0.25f}));

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void AbstractShaderProgramGLTest::uniformArray() {
    MyShader shader;

    MAGNUM_VERIFY_NO_GL_ERROR();

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

    MAGNUM_VERIFY_NO_GL_ERROR();
}

#ifndef MAGNUM_TARGET_GLES
struct MyDoubleShader: AbstractShaderProgram {
    explicit MyDoubleShader();

    using AbstractShaderProgram::setUniform;

    Int matrixUniform,
        multiplierUniform,
        colorUniform,
        additionsUniform;
};

MyDoubleShader::MyDoubleShader() {
    Utility::Resource rs("AbstractShaderProgramGLTest");

    Shader vert(Version::GL320, Shader::Type::Vertex);
    Shader frag(Version::GL320, Shader::Type::Fragment);
    vert.addSource(rs.getString("MyDoubleShader.vert"))
        .compile();
    frag.addSource(rs.getString("MyDoubleShader.frag"))
        .compile();

    attachShaders({vert, frag});

    bindAttributeLocation(0, "position");
    link();

    matrixUniform = uniformLocation("matrix");
    multiplierUniform = uniformLocation("multiplier");
    colorUniform = uniformLocation("color");
    additionsUniform = uniformLocation("additions");
}

void AbstractShaderProgramGLTest::uniformDouble() {
    if(!Context::current().isExtensionSupported<Extensions::ARB::gpu_shader_fp64>())
        CORRADE_SKIP(Extensions::ARB::gpu_shader_fp64::string() << "is not supported.");

    MyDoubleShader shader;

    MAGNUM_VERIFY_NO_GL_ERROR();

    shader.setUniform(shader.multiplierUniform, 0.35);

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void AbstractShaderProgramGLTest::uniformDoubleVector() {
    if(!Context::current().isExtensionSupported<Extensions::ARB::gpu_shader_fp64>())
        CORRADE_SKIP(Extensions::ARB::gpu_shader_fp64::string() << "is not supported.");

    MyDoubleShader shader;

    MAGNUM_VERIFY_NO_GL_ERROR();

    shader.setUniform(shader.colorUniform, Vector4d{0.3, 0.7, 1.0, 0.25});

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void AbstractShaderProgramGLTest::uniformDoubleMatrix() {
    if(!Context::current().isExtensionSupported<Extensions::ARB::gpu_shader_fp64>())
        CORRADE_SKIP(Extensions::ARB::gpu_shader_fp64::string() << "is not supported.");

    MyDoubleShader shader;

    MAGNUM_VERIFY_NO_GL_ERROR();

    shader.setUniform(shader.matrixUniform, Matrix4x4d::fromDiagonal({0.3, 0.7, 1.0, 0.25}));

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void AbstractShaderProgramGLTest::uniformDoubleArray() {
    if(!Context::current().isExtensionSupported<Extensions::ARB::gpu_shader_fp64>())
        CORRADE_SKIP(Extensions::ARB::gpu_shader_fp64::string() << "is not supported.");

    MyDoubleShader shader;

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* Testing also implicit conversion to base type (Vector4d[] -> Math::Vector<4, Double>[]) */
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* Causes ICE */
    constexpr
    #endif
    const Vector4d values[] = {
        {0.5, 1.0, 0.4, 0.0},
        {0.0, 0.1, 0.7, 0.3},
        {0.9, 0.8, 0.3, 0.1}
    };
    shader.setUniform(shader.additionsUniform, values);

    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void AbstractShaderProgramGLTest::createUniformBlocks() {
    auto&& data = CreateUniformBlocksData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    Utility::Resource rs("AbstractShaderProgramGLTest");

    Shader vert(
        #ifndef MAGNUM_TARGET_GLES
        Version::GL310
        #else
        Version::GLES300
        #endif
        , Shader::Type::Vertex);
    vert.addSource(rs.getString("UniformBlockShader.vert"));
    const bool vertCompiled = vert.compile();

    Shader frag(
        #ifndef MAGNUM_TARGET_GLES
        Version::GL310
        #else
        Version::GLES300
        #endif
        , Shader::Type::Fragment);
    frag.addSource(rs.getString("UniformBlockShader.frag"));
    const bool fragCompiled = frag.compile();

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_VERIFY(vertCompiled);
    CORRADE_VERIFY(fragCompiled);

    MyPublicShader program;
    program.attachShaders({vert, frag});

    MAGNUM_VERIFY_NO_GL_ERROR();

    const bool linked = program.link();
    const bool valid = program.validate().first();

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_VERIFY(linked);
    {
        #if defined(CORRADE_TARGET_APPLE) && !defined(MAGNUM_TARGET_GLES)
        CORRADE_EXPECT_FAIL("macOS drivers need insane amount of state to validate properly.");
        #endif
        CORRADE_VERIFY(valid);
    }

    const Int matricesUniformBlock = program.uniformBlockIndex(data.matricesName);
    const Int materialUniformBlock = program.uniformBlockIndex(data.materialName);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_VERIFY(matricesUniformBlock >= 0);
    CORRADE_VERIFY(materialUniformBlock >= 0);
}

void AbstractShaderProgramGLTest::uniformBlockIndexNotFound() {
    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    MyPublicShader program;

    Shader vert(
        #ifndef MAGNUM_TARGET_GLES
        Version::GL310
        #else
        Version::GLES300
        #endif
        , Shader::Type::Vertex);
    Shader frag(
        #ifndef MAGNUM_TARGET_GLES
        Version::GL310
        #else
        Version::GLES300
        #endif
        , Shader::Type::Fragment);
    vert.addSource("void main() { gl_Position = vec4(0.0); }");
    frag.addSource("out lowp vec4 color;\n"
                   "void main() { color = vec4(1.0); }");
    CORRADE_VERIFY(vert.compile() && frag.compile());

    program.attachShaders({vert, frag});
    CORRADE_VERIFY(program.link());

    Containers::String out;
    Warning redirectWarning{&out};
    program.uniformBlockIndex("nonexistent");
    CORRADE_COMPARE(out,
        "GL::AbstractShaderProgram: index of uniform block 'nonexistent' cannot be retrieved\n");
}

struct UniformBlockShader: AbstractShaderProgram {
    explicit UniformBlockShader();

    using AbstractShaderProgram::setUniformBlockBinding;

    Int matricesUniformBlock,
        materialUniformBlock;
};

UniformBlockShader::UniformBlockShader() {
    Utility::Resource rs("AbstractShaderProgramGLTest");

    Shader vert(
        #ifndef MAGNUM_TARGET_GLES
        Version::GL310
        #else
        Version::GLES300
        #endif
        , Shader::Type::Vertex);
    Shader frag(
        #ifndef MAGNUM_TARGET_GLES
        Version::GL310
        #else
        Version::GLES300
        #endif
        , Shader::Type::Fragment);
    vert.addSource(rs.getString("UniformBlockShader.vert"))
        .compile();
    frag.addSource(rs.getString("UniformBlockShader.frag"))
        .compile();

    attachShaders({vert, frag});

    link();

    matricesUniformBlock = uniformBlockIndex("matrices");
    materialUniformBlock = uniformBlockIndex("material");
}

void AbstractShaderProgramGLTest::uniformBlock() {
    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    UniformBlockShader shader;

    MAGNUM_VERIFY_NO_GL_ERROR();

    shader.setUniformBlockBinding(shader.matricesUniformBlock, 0);
    shader.setUniformBlockBinding(shader.materialUniformBlock, 1);

    MAGNUM_VERIFY_NO_GL_ERROR();
}

#ifndef MAGNUM_TARGET_WEBGL
void AbstractShaderProgramGLTest::compute() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::compute_shader>())
        CORRADE_SKIP(Extensions::ARB::compute_shader::string() << "is not supported.");
    #else
    if(!Context::current().isVersionSupported(Version::GLES310))
        CORRADE_SKIP("OpenGL ES 3.1 is not supported.");
    #endif

    struct ComputeShader: AbstractShaderProgram {
        explicit ComputeShader() {
            Utility::Resource rs("AbstractShaderProgramGLTest");

            Shader compute(
                #ifndef MAGNUM_TARGET_GLES
                Version::GL430,
                #else
                Version::GLES310,
                #endif
                Shader::Type::Compute);
            compute.addSource(rs.getString("ComputeShader.comp"));
            CORRADE_INTERNAL_ASSERT_OUTPUT(compute.compile());

            attachShader(compute);
            link();
        }

        ComputeShader& setImages(Texture2D& input, Texture2D& output) {
            input.bindImage(0, 0, ImageAccess::ReadOnly, ImageFormat::RGBA8UI);
            output.bindImage(1, 0, ImageAccess::WriteOnly, ImageFormat::RGBA8UI);
            return *this;
        }
    } shader;

    MAGNUM_VERIFY_NO_GL_ERROR();

    const Color4ub inData[] = {
        { 10,  20,  30,  40},
        { 50,  60,  70,  80},
        { 90, 100, 110, 120},
        {130, 140, 150, 160}
    };

    #ifndef MAGNUM_TARGET_GLES
    const Color4ub outData[] = {
        { 15,  30,  45,  60},
        { 75,  90, 105, 120},
        {135, 150, 165, 180},
        {195, 210, 225, 240}
    };
    #endif

    Texture2D in;
    in.setStorage(1, TextureFormat::RGBA8UI, {2, 2})
        .setSubImage(0, {}, ImageView2D{PixelFormat::RGBAInteger, PixelType::UnsignedByte, {2, 2}, inData});

    Texture2D out;
    out.setStorage(1, TextureFormat::RGBA8UI, {2, 2});

    MAGNUM_VERIFY_NO_GL_ERROR();

    shader.setImages(in, out)
        .dispatchCompute({1, 1, 1});

    MAGNUM_VERIFY_NO_GL_ERROR();

    /** @todo Test on ES */
    #ifndef MAGNUM_TARGET_GLES
    const auto data = out.image(0, {PixelFormat::RGBAInteger, PixelType::UnsignedByte}).release();

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE_AS(Containers::arrayCast<const Color4ub>(data),
        Containers::arrayView(outData),
        TestSuite::Compare::Container);
    #endif
}
#endif
#endif

}}}}

/* These are outside of any namespace to verify the macros fully qualify all
   names */
namespace {
    /* Yes, Clang, I know, the macros add unused members. That's fine. */
    #ifdef CORRADE_TARGET_CLANG
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wunused-member-function"
    #endif
    struct ShaderSubclassDraw: Magnum::GL::AbstractShaderProgram {
        MAGNUM_GL_ABSTRACTSHADERPROGRAM_SUBCLASS_DRAW_IMPLEMENTATION(ShaderSubclassDraw)
    };
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    struct ShaderSubclassDispatch: Magnum::GL::AbstractShaderProgram {
        MAGNUM_GL_ABSTRACTSHADERPROGRAM_SUBCLASS_DISPATCH_IMPLEMENTATION(ShaderSubclassDispatch)
    };
    #endif
    #ifdef CORRADE_TARGET_CLANG
    #pragma GCC diagnostic pop
    #endif
}

namespace Magnum { namespace GL { namespace Test { namespace {

void AbstractShaderProgramGLTest::subclassDraw() {
    ShaderSubclassDraw shader;
    Mesh mesh;
    mesh
        .setCount(0);
    MeshView meshView{mesh};
    meshView
        .setCount(0);
    Mesh meshNoInstances;
    meshNoInstances
        .setInstanceCount(0);
    Mesh meshViewNoInstances;
    meshViewNoInstances
        .setInstanceCount(0);
    #ifndef MAGNUM_TARGET_GLES
    TransformFeedback xfb{NoCreate};
    #endif

    Containers::StridedArrayView1D<const UnsignedInt> counts;
    #ifdef MAGNUM_TARGET_GLES
    Containers::StridedArrayView1D<const UnsignedInt> instanceCounts;
    #endif
    Containers::StridedArrayView1D<const UnsignedInt> vertexOffsets;
    Containers::StridedArrayView1D<const UnsignedInt> indexOffsets;
    #ifndef CORRADE_TARGET_32BIT
    Containers::StridedArrayView1D<const UnsignedLong> indexOffsetsLong;
    #endif
    #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_GLES2)
    Containers::StridedArrayView1D<const UnsignedInt> instanceOffsets;
    #endif

    /* These should all be a no-op because the mesh is empty and/or because we
       specify no multidraw items. And if everything is alright, the returned
       type should still be ShaderSubclassDraw& even after all these. */
    ShaderSubclassDraw& out = shader
        .draw(mesh)
        .draw(Utility::move(mesh))
        .draw(meshView)
        .draw(Utility::move(meshView))
        .draw(mesh, counts, vertexOffsets, indexOffsets)
        #ifndef CORRADE_TARGET_32BIT
        .draw(mesh, counts, vertexOffsets, indexOffsetsLong)
        .draw(mesh, counts, vertexOffsets, nullptr)
        #endif
        #ifdef MAGNUM_TARGET_GLES
        #ifndef MAGNUM_TARGET_GLES2
        .draw(mesh, counts, instanceCounts, vertexOffsets, indexOffsets, instanceOffsets)
        #ifndef CORRADE_TARGET_32BIT
        .draw(mesh, counts, instanceCounts, vertexOffsets, indexOffsetsLong, instanceOffsets)
        .draw(mesh, counts, instanceCounts, vertexOffsets, nullptr, instanceOffsets)
        #endif
        #endif
        .draw(mesh, counts, instanceCounts, vertexOffsets, indexOffsets)
        #ifndef CORRADE_TARGET_32BIT
        .draw(mesh, counts, instanceCounts, vertexOffsets, indexOffsetsLong)
        .draw(mesh, counts, instanceCounts, vertexOffsets, nullptr)
        #endif
        #endif
        .draw(Containers::Iterable<MeshView>{})
        #ifndef MAGNUM_TARGET_GLES
        .drawTransformFeedback(meshNoInstances, xfb)
        .drawTransformFeedback(meshNoInstances, xfb, 0)
        .drawTransformFeedback(meshViewNoInstances, xfb)
        .drawTransformFeedback(meshViewNoInstances, xfb, 0)
        #endif
        ;

    CORRADE_VERIFY(out.id());
    MAGNUM_VERIFY_NO_GL_ERROR();
}

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void AbstractShaderProgramGLTest::subclassDispatch() {
    ShaderSubclassDispatch shader;

    /* These should all be a no-op because the count is empty. And if
       everything is alright, the returned type should still be
       ShaderSubclassDispatch& again. */
    ShaderSubclassDispatch& out = shader.dispatchCompute({});

    CORRADE_VERIFY(out.id());
    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

}}}}

CORRADE_TEST_MAIN(Magnum::GL::Test::AbstractShaderProgramGLTest)
