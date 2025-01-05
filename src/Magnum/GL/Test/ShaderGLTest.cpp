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

#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Containers/String.h>
#include <Corrade/Containers/StringIterable.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/TestSuite/Compare/String.h>
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
    void addFileNonexistent();
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
              &ShaderGLTest::addFileNonexistent,
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
        /* There may be various workaround defines after, so check just that
           the first source is the version definition */
        CORRADE_VERIFY(!shader.sources().isEmpty());
        #ifndef MAGNUM_TARGET_GLES
        CORRADE_COMPARE(shader.sources()[0], "#version 130\n");
        #else
        CORRADE_COMPARE(shader.sources()[0], "#version 300 es\n");
        #endif
    }

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void ShaderGLTest::constructNoVersion() {
    const Shader shader(Version::None, Shader::Type::Fragment);
    CORRADE_VERIFY(shader.sources().isEmpty());
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

    Shader b(Utility::move(a));

    CORRADE_COMPARE(a.id(), 0);
    CORRADE_COMPARE(b.id(), id);
    CORRADE_COMPARE(b.type(), Shader::Type::Fragment);
    /* There may be various workaround defines after, so check just that the
       first source is the version definition */
    CORRADE_VERIFY(!b.sources().isEmpty());
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(b.sources()[0], "#version 130\n");
    #else
    CORRADE_COMPARE(b.sources()[0], "#version 300 es\n");
    #endif

    #ifndef MAGNUM_TARGET_GLES
    Shader c(Version::GL210, Shader::Type::Vertex);
    #else
    Shader c(Version::GLES200, Shader::Type::Vertex);
    #endif
    const Int cId = c.id();
    c = Utility::move(b);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_VERIFY(cId > 0);
    CORRADE_COMPARE(b.id(), cId);
    CORRADE_COMPARE(c.id(), id);
    CORRADE_COMPARE(c.type(), Shader::Type::Fragment);
    /* There may be various workaround defines after, so check just that the
       first source is the version definition */
    CORRADE_VERIFY(!c.sources().isEmpty());
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(c.sources()[0], "#version 130\n");
    #else
    CORRADE_COMPARE(c.sources()[0], "#version 300 es\n");
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

    /* The initial shader sources contain driver-specific workarounds, so
       just copy them to the expected array */
    /** @todo some StringIterable slicing during comparison instead of this? */
    Containers::Array<Containers::StringView> expected;
    for(Containers::StringView i: shader.sources())
        arrayAppend(expected, i);
    const UnsignedInt workaroundCount = expected.size() - 1;

    const char* data = "// r-value String\n";

    shader.addSource("// global, null-terminated\n"_s)
          .addSource("// global, non-null-terminated\n!"_s.exceptSuffix(1))
          .addSource("// local, null-terminated\n")
          .addSource(Containers::StringView{"// local, non-null-terminated\n!"}.exceptSuffix(1))
          .addSource(Containers::String{data, [](char*, std::size_t) {}})
          .addSource("") /* gets ignored */
          .addSource("void main() {}\n"_s);

    /* On (desktop) GLSL < 330 the #line affect next line, not current
       line; see compileFailure() for a correctness verification */
    #ifndef MAGNUM_TARGET_GLES
    #define _zero " 0 "
    #else
    #define _zero " 1 "
    #endif
    Containers::StringView expectedSuffix[]{
        "#line" _zero "1\n",
        "// global, null-terminated\n",     // 2
        "#line" _zero "2\n",
        "// global, non-null-terminated\n",
        "#line" _zero "3\n",
        "// local, null-terminated\n",
        "#line" _zero "4\n",
        "// local, non-null-terminated\n",
        "#line" _zero "5\n",
        "// r-value String\n",              // isn't global but is moved
        "#line" _zero "6\n",
        /* Empty source gets ignored */
        "void main() {}\n"                  // 12
    };
    #undef _zero
    arrayAppend(expected, expectedSuffix);

    CORRADE_COMPARE_AS(shader.sources(),
        Containers::StringIterable{expected},
        TestSuite::Compare::Container);

    /* Verify that strings get copied only when not null terminated or not
       global, and when not moved. Exclude the workaround defines added at the
       front when comparing. */
    for(std::size_t i: {0, 2, 12}) {
        CORRADE_ITERATION(i);
        CORRADE_COMPARE(shader.sources()[i + workaroundCount].flags(), Containers::StringViewFlag::NullTerminated|Containers::StringViewFlag::Global);
    }
    for(std::size_t i: {1, 3, 4, 5, 6, 7, 8, 9, 10, 11}) {
        CORRADE_ITERATION(i);
        CORRADE_COMPARE(shader.sources()[i + workaroundCount].flags(), Containers::StringViewFlag::NullTerminated);
    }
    CORRADE_VERIFY(shader.sources()[10 + workaroundCount].data() == data);
}

void ShaderGLTest::addSourceNoVersion() {
    Shader shader(Version::None, Shader::Type::Fragment);

    /* Unlike above, the initial shader sources are empty in this case */

    #ifndef MAGNUM_TARGET_GLES
    shader.addSource("#version 120\n"_s);
    #else
    shader.addSource("#version 100\n"_s);
    #endif
    shader.addSource("#define FOO BAR\n"_s)
          .addSource("void main() {}\n"_s);

    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE_AS(shader.sources(), (Containers::StringIterable{
        "",
        /* Here, even though there's #version 120 eventually added by the user,
           it assumes the specified version was new GLSL, not old. Explicitly
           specified old GLSL is such a rare use case that I don't bother
           looking for the #version directive and adjusting. */
        "#version 120\n",
        "#line 1 1\n",          // 2
        "#define FOO BAR\n",
        "#line 1 2\n",          // 4
        "void main() {}\n"
    }), TestSuite::Compare::Container);
    #else
    CORRADE_COMPARE_AS(shader.sources(), (Containers::StringIterable{
        "",
        "#version 100\n",
        "#line 1 1\n",
        "#define FOO BAR\n",
        "#line 1 2\n",
        "void main() {}\n"
    }), TestSuite::Compare::Container);
    #endif

    /* Everything except the line numbers should be global in this case,
       including the empty string */
    for(std::size_t i: {0, 1, 3, 5}) {
        CORRADE_ITERATION(i);
        CORRADE_COMPARE(shader.sources()[i].flags(), Containers::StringViewFlag::NullTerminated|Containers::StringViewFlag::Global);
    }
    for(std::size_t i: {2, 4}) {
        CORRADE_ITERATION(i);
        CORRADE_COMPARE(shader.sources()[i].flags(), Containers::StringViewFlag::NullTerminated);
    }
}

void ShaderGLTest::addFile() {
    #ifndef MAGNUM_TARGET_GLES
    Shader shader(Version::GL210, Shader::Type::Fragment);
    #else
    Shader shader(Version::GLES200, Shader::Type::Fragment);
    #endif

    /* The initial shader sources contain driver-specific workarounds, so
       just copy them to the expected array */
    /** @todo some StringIterable slicing during comparison instead of this? */
    Containers::Array<Containers::StringView> expected;
    for(Containers::StringView i: shader.sources())
        arrayAppend(expected, i);
    const UnsignedInt workaroundCount = expected.size() - 1;

    shader.addFile(Utility::Path::join(SHADERGLTEST_FILES_DIR, "shader.glsl"));

    arrayAppend(expected, {
        #ifndef MAGNUM_TARGET_GLES
        /* On (desktop) GLSL < 330 the #line affect next line, not current
           line; see compileFailure() for a correctness verification */
        "#line 0 1\n",
        #else
        "#line 1 1\n",
        #endif
        "void main() {}\n"
    });
    CORRADE_COMPARE_AS(shader.sources(), expected,
        TestSuite::Compare::Container);

    /* The file source and the line number isn't global */
    CORRADE_COMPARE(shader.sources()[0 + workaroundCount].flags(), Containers::StringViewFlag::NullTerminated|Containers::StringViewFlag::Global);
    CORRADE_COMPARE(shader.sources()[1 + workaroundCount].flags(), Containers::StringViewFlag::NullTerminated);
    CORRADE_COMPARE(shader.sources()[2 + workaroundCount].flags(), Containers::StringViewFlag::NullTerminated);
}

void ShaderGLTest::addFileNonexistent() {
    CORRADE_SKIP_IF_NO_ASSERT();

    #ifndef MAGNUM_TARGET_GLES
    Shader shader(Version::GL210, Shader::Type::Fragment);
    #else
    Shader shader(Version::GLES200, Shader::Type::Fragment);
    #endif

    Containers::String out;
    Error redirectError{&out};
    shader.addFile("nonexistent");
    /* There's an error message from Path::read() before */
    CORRADE_COMPARE_AS(out,
        "\nGL::Shader::addFile(): can't read nonexistent\n",
        TestSuite::Compare::StringHasSuffix);
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

    Containers::String out;
    {
        Error redirectError{&out};
        CORRADE_VERIFY(!shader.compile());
    }
    CORRADE_VERIFY(shader.isCompileFinished());

    /* There's a driver-specific message after */
    CORRADE_COMPARE_AS(out, "GL::Shader::compile(): compilation of vertex shader failed with the following message:",
        TestSuite::Compare::StringHasPrefix);

    /* The error message should contain the correct source number */
    CORRADE_COMPARE_AS(out, "11", TestSuite::Compare::StringNotContains);
    {
        #ifndef MAGNUM_TARGET_WEBGL
        /* Fixed in 23.1.4, https://gitlab.freedesktop.org/mesa/mesa/-/commit/d5ff432d7d08c8bb644594ccf28b83be4b521ffe
           Since I don't have any builtin way to compare version strings yet,
           XFAILing only optionally, if it actually doesn't contain the
           string. */
        CORRADE_EXPECT_FAIL_IF(Context::current().detectedDriver() & Context::DetectedDriver::Mesa && !out.contains("12"),
            "Mesa reports source number only in some cases.");
        #endif
        CORRADE_COMPARE_AS(out, "12", TestSuite::Compare::StringContains);
    }
    CORRADE_COMPARE_AS(out, "13", TestSuite::Compare::StringNotContains);

    /* The error message should contain the correct line number */
    CORRADE_COMPARE_AS(out, "175", TestSuite::Compare::StringNotContains);
    CORRADE_COMPARE_AS(out, "176", TestSuite::Compare::StringContains);
    CORRADE_COMPARE_AS(out, "177", TestSuite::Compare::StringNotContains);

    /* No stray \0 should be anywhere */
    CORRADE_COMPARE_AS(out, "\0"_s, TestSuite::Compare::StringNotContains);
    /* The message should end with a newline */
    CORRADE_COMPARE_AS(out, "\n"_s, TestSuite::Compare::StringHasSuffix);
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
    Containers::String out;
    {
        Error redirectError{&out};
        shader.submitCompile();
    }

    while(!shader.isCompileFinished())
        Utility::System::sleep(100);

    CORRADE_COMPARE(out, "");

    /* ... only the final check should */
    {
        Error redirectError{&out};
        CORRADE_VERIFY(!shader.checkCompile());
    }
    CORRADE_VERIFY(shader.isCompileFinished());
    CORRADE_COMPARE_AS(out, "GL::Shader::compile(): compilation of fragment shader failed with the following message:",
        TestSuite::Compare::StringHasPrefix);

    /* Not testing presence of \0 etc., as that's tested well enough in
       compileFailure() above already and both cases use the same error printing
       code path */
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
