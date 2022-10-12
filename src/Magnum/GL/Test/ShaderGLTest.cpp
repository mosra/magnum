/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022 Vladimír Vondruš <mosra@centrum.cz>

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
#include <Corrade/Containers/String.h>
#include <Corrade/Containers/StringStl.h> /** @todo remove once Shader is <string>-free */
#include <Corrade/TestSuite/Compare/String.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/System.h>
#include <Corrade/Utility/Path.h>

#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/Shader.h"
#include "Magnum/GL/OpenGLTester.h"

#include "configure.h"

namespace Magnum { namespace GL { namespace Test { namespace {

struct ShaderGLTest: OpenGLTester {
    explicit ShaderGLTest();

    void construct();
    void constructNoVersion();
    void constructMove();
    void wrap();

    #ifndef MAGNUM_TARGET_WEBGL
    void label();
    #endif

    void addSource();
    void addSourceNoVersion();
    void addFile();
    void compile();
    void compileAsync();
    void compileFailure();
    void compileFailureAsync();
    void compileUtf8();
    void compileNoVersion();
};

const struct {
    const char* name;
    Version version;
} CompileFailureData[]{
    #ifndef MAGNUM_TARGET_GLES /* GLES has the new semantics always */
    {"old GLSL #line semantics, affects next line",
        #ifndef CORRADE_TARGET_APPLE
        Version::GL210
        #else
        Version::GL310
        #endif
        },
    #endif
    {"new GLSL #line semantics, affects current line",
        #ifndef MAGNUM_TARGET_GLES
        Version::GL330
        #else
        Version::GLES200
        #endif
        },
};

ShaderGLTest::ShaderGLTest() {
    addTests({&ShaderGLTest::construct,
              &ShaderGLTest::constructNoVersion,
              &ShaderGLTest::constructMove,
              &ShaderGLTest::wrap,

              #ifndef MAGNUM_TARGET_WEBGL
              &ShaderGLTest::label,
              #endif

              &ShaderGLTest::addSource,
              &ShaderGLTest::addSourceNoVersion,
              &ShaderGLTest::addFile,
              &ShaderGLTest::compile,
              &ShaderGLTest::compileAsync});

    addInstancedTests({&ShaderGLTest::compileFailure},
        Containers::arraySize(CompileFailureData));

    addTests({&ShaderGLTest::compileFailureAsync,
              &ShaderGLTest::compileUtf8,
              &ShaderGLTest::compileNoVersion});
}

using namespace Containers::Literals;

void ShaderGLTest::construct() {
    {
        #ifndef MAGNUM_TARGET_GLES
        const Shader shader(Version::GL300, Shader::Type::Fragment);
        #else
        const Shader shader(Version::GLES300, Shader::Type::Fragment);
        #endif

        MAGNUM_VERIFY_NO_GL_ERROR();
        CORRADE_VERIFY(shader.id() > 0);
        CORRADE_COMPARE(shader.type(), Shader::Type::Fragment);
        #ifndef MAGNUM_TARGET_GLES
        CORRADE_COMPARE(shader.sources(), std::vector<std::string>{"#version 130\n"});
        #else
        CORRADE_COMPARE(shader.sources(), std::vector<std::string>{"#version 300 es\n"});
        #endif
    }

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void ShaderGLTest::constructNoVersion() {
    const Shader shader(Version::None, Shader::Type::Fragment);
    CORRADE_VERIFY(shader.sources().empty());
}

void ShaderGLTest::constructMove() {
    #ifndef MAGNUM_TARGET_GLES
    Shader a(Version::GL300, Shader::Type::Fragment);
    #else
    Shader a(Version::GLES300, Shader::Type::Fragment);
    #endif
    const Int id = a.id();

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_VERIFY(id > 0);

    Shader b(std::move(a));

    CORRADE_COMPARE(a.id(), 0);
    CORRADE_COMPARE(b.id(), id);
    CORRADE_COMPARE(b.type(), Shader::Type::Fragment);
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(b.sources(), std::vector<std::string>{"#version 130\n"});
    #else
    CORRADE_COMPARE(b.sources(), std::vector<std::string>{"#version 300 es\n"});
    #endif

    #ifndef MAGNUM_TARGET_GLES
    Shader c(Version::GL210, Shader::Type::Vertex);
    #else
    Shader c(Version::GLES200, Shader::Type::Vertex);
    #endif
    const Int cId = c.id();
    c = std::move(b);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_VERIFY(cId > 0);
    CORRADE_COMPARE(b.id(), cId);
    CORRADE_COMPARE(c.id(), id);
    CORRADE_COMPARE(c.type(), Shader::Type::Fragment);
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(c.sources(), std::vector<std::string>{"#version 130\n"});
    #else
    CORRADE_COMPARE(c.sources(), std::vector<std::string>{"#version 300 es\n"});
    #endif

    CORRADE_VERIFY(std::is_nothrow_move_constructible<Shader>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<Shader>::value);
}

void ShaderGLTest::wrap() {
    GLuint id = glCreateShader(GL_FRAGMENT_SHADER);

    /* Releasing won't delete anything */
    {
        auto shader = Shader::wrap(Shader::Type::Fragment, id, ObjectFlag::DeleteOnDestruction);
        CORRADE_COMPARE(shader.release(), id);
    }

    /* ...so we can wrap it again */
    Shader::wrap(Shader::Type::Fragment, id);
    glDeleteShader(id);
}

#ifndef MAGNUM_TARGET_WEBGL
void ShaderGLTest::label() {
    /* No-Op version is tested in AbstractObjectGLTest */
    if(!Context::current().isExtensionSupported<Extensions::KHR::debug>() &&
       !Context::current().isExtensionSupported<Extensions::EXT::debug_label>())
        CORRADE_SKIP("Required extension is not available");

    #ifndef MAGNUM_TARGET_GLES
    Shader shader(Version::GL210, Shader::Type::Vertex);
    #else
    Shader shader(Version::GLES200, Shader::Type::Vertex);
    #endif
    CORRADE_COMPARE(shader.label(), "");
    MAGNUM_VERIFY_NO_GL_ERROR();

    /* Test the string size gets correctly used, instead of relying on null
       termination */
    shader.setLabel("MyShader!"_s.exceptSuffix(1));
    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(shader.label(), "MyShader");
    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

void ShaderGLTest::addSource() {
    #ifndef MAGNUM_TARGET_GLES
    Shader shader(Version::GL210, Shader::Type::Fragment);
    #else
    Shader shader(Version::GLES200, Shader::Type::Fragment);
    #endif

    shader.addSource("#define FOO BAR\n")
          .addSource("void main() {}\n");

    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(shader.sources(), (std::vector<std::string>{
        "#version 120\n",
        /* On (desktop) GLSL < 330 the #line affect next line, not current
           line; see compileFailure() for a correctness verification */
        "#line 0 1\n",
        "#define FOO BAR\n",
        "#line 0 2\n",
        "void main() {}\n"
    }));
    #else
    CORRADE_COMPARE(shader.sources(), (std::vector<std::string>{
        "#version 100\n",
        "#line 1 1\n",
        "#define FOO BAR\n",
        "#line 1 2\n",
        "void main() {}\n"
    }));
    #endif
}

void ShaderGLTest::addSourceNoVersion() {
    Shader shader(Version::None, Shader::Type::Fragment);

    #ifndef MAGNUM_TARGET_GLES
    shader.addSource("#version 120\n");
    #else
    shader.addSource("#version 100\n");
    #endif
    shader.addSource("#define FOO BAR\n")
          .addSource("void main() {}\n");

    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(shader.sources(), (std::vector<std::string>{
        "",
        /* Here, even though there's #version 120 eventually added by the user,
           it assumes the specified version was new GLSL, not old. Explicitly
           specified old GLSL is such a rare use case that I don't bother
           looking for the #version directive and adjusting. */
        "#version 120\n",
        "#line 1 1\n",
        "#define FOO BAR\n",
        "#line 1 2\n",
        "void main() {}\n"
    }));
    #else
    CORRADE_COMPARE(shader.sources(), (std::vector<std::string>{
        "",
        "#version 100\n",
        "#line 1 1\n",
        "#define FOO BAR\n",
        "#line 1 2\n",
        "void main() {}\n"
    }));
    #endif
}

void ShaderGLTest::addFile() {
    #ifndef MAGNUM_TARGET_GLES
    Shader shader(Version::GL210, Shader::Type::Fragment);
    #else
    Shader shader(Version::GLES200, Shader::Type::Fragment);
    #endif

    shader.addFile(Utility::Path::join(SHADERGLTEST_FILES_DIR, "shader.glsl"));

    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(shader.sources(), (std::vector<std::string>{
        "#version 120\n",
        /* On (desktop) GLSL < 330 the #line affect next line, not current
           line; see compileFailure() for a correctness verification */
        "#line 0 1\n",
        "void main() {}\n"
    }));
    #else
    CORRADE_COMPARE(shader.sources(), (std::vector<std::string>{
        "#version 100\n",
        "#line 1 1\n",
        "void main() {}\n"
    }));
    #endif
}

void ShaderGLTest::compile() {
    #ifndef MAGNUM_TARGET_GLES
    constexpr Version v =
        #ifndef CORRADE_TARGET_APPLE
        Version::GL210
        #else
        Version::GL310
        #endif
        ;
    #else
    constexpr Version v = Version::GLES200;
    #endif

    Shader shader(v, Shader::Type::Fragment);
    shader.addSource("void main() {}\n");

    CORRADE_VERIFY(shader.compile());
    CORRADE_VERIFY(shader.isCompileFinished());
}

void ShaderGLTest::compileAsync() {
    #ifndef MAGNUM_TARGET_GLES
    constexpr Version v =
        #ifndef CORRADE_TARGET_APPLE
        Version::GL210
        #else
        Version::GL310
        #endif
        ;
    #else
    constexpr Version v = Version::GLES200;
    #endif

    Shader shader(v, Shader::Type::Fragment);
    shader.addSource("void main() {}\n");
    shader.submitCompile();

    while(!shader.isCompileFinished())
        Utility::System::sleep(100);

    CORRADE_VERIFY(shader.checkCompile());
    CORRADE_VERIFY(shader.isCompileFinished());
}

void ShaderGLTest::compileFailure() {
    auto&& data = CompileFailureData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    if(!Context::current().isVersionSupported(data.version))
        CORRADE_SKIP(data.version << "is not supported");

    Shader shader{data.version, Shader::Type::Vertex};

    /* First source is 1, so 11 sources means the error will be in source 12 */
    for(std::size_t i = 0; i != 11; ++i)
        shader.addSource("// something\n");
    /* First line is 1, so 175 newlines means the error is on line 176 */
    shader.addSource("void main() {" + "\n"_s*175 + "someOutputVariable = ERROR_ERROR();\n}\n");

    std::ostringstream out;
    {
        Error redirectError{&out};
        CORRADE_VERIFY(!shader.compile());
    }
    CORRADE_VERIFY(shader.isCompileFinished());
    CORRADE_COMPARE_AS(out.str(), "GL::Shader::compile(): compilation of vertex shader failed with the following message:",
        TestSuite::Compare::StringHasPrefix);

    /* The error message should contain the correct source number */
    CORRADE_COMPARE_AS(out.str(), "11", TestSuite::Compare::StringNotContains);
    {
        #ifndef MAGNUM_TARGET_WEBGL
        CORRADE_EXPECT_FAIL_IF(Context::current().detectedDriver() & Context::DetectedDriver::Mesa,
            "Mesa reports source number only in some cases.");
        #endif
        CORRADE_COMPARE_AS(out.str(), "12", TestSuite::Compare::StringContains);
    }
    CORRADE_COMPARE_AS(out.str(), "13", TestSuite::Compare::StringNotContains);

    /* The error message should contain the correct line number */
    CORRADE_COMPARE_AS(out.str(), "175", TestSuite::Compare::StringNotContains);
    CORRADE_COMPARE_AS(out.str(), "176", TestSuite::Compare::StringContains);
    CORRADE_COMPARE_AS(out.str(), "177", TestSuite::Compare::StringNotContains);
}

void ShaderGLTest::compileFailureAsync() {
    #ifndef MAGNUM_TARGET_GLES
    constexpr Version v =
        #ifndef CORRADE_TARGET_APPLE
        Version::GL210
        #else
        Version::GL310
        #endif
        ;
    #else
    constexpr Version v = Version::GLES200;
    #endif

    Shader shader(v, Shader::Type::Fragment);
    shader.addSource("[fu] bleh error #:! stuff\n");

    /* The compile submission should not print anything ... */
    std::ostringstream out;
    {
        Error redirectError{&out};
        shader.submitCompile();
    }

    while(!shader.isCompileFinished())
        Utility::System::sleep(100);

    CORRADE_VERIFY(out.str().empty());

    /* ... only the final check should */
    {
        Error redirectError{&out};
        CORRADE_VERIFY(!shader.checkCompile());
    }
    CORRADE_VERIFY(shader.isCompileFinished());
    CORRADE_COMPARE_AS(out.str(), "GL::Shader::compile(): compilation of fragment shader failed with the following message:",
        TestSuite::Compare::StringHasPrefix);
}

void ShaderGLTest::compileUtf8() {
    #ifndef MAGNUM_TARGET_GLES
    constexpr Version v =
        #ifndef CORRADE_TARGET_APPLE
        Version::GL210
        #else
        Version::GL310
        #endif
        ;
    #else
    constexpr Version v = Version::GLES200;
    #endif

    Shader shader(v, Shader::Type::Fragment);
    shader.addSource("/* hýždě */ void main() {} \n");
    CORRADE_VERIFY(shader.compile());
}

void ShaderGLTest::compileNoVersion() {
    Shader shader(Version::None, Shader::Type::Fragment);
    #ifndef MAGNUM_TARGET_GLES
    #ifndef CORRADE_TARGET_APPLE
    shader.addSource("#version 120\nvoid main() {}\n");
    #else
    shader.addSource("#version 400\nvoid main() {}\n");
    #endif
    #else
    shader.addSource("#version 100\nvoid main() {}\n");
    #endif
    CORRADE_VERIFY(shader.compile());
}

}}}}

CORRADE_TEST_MAIN(Magnum::GL::Test::ShaderGLTest)
