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

#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/Directory.h>

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

    #ifndef MAGNUM_TARGET_WEBGL
    void label();
    #endif

    void addSource();
    void addSourceNoVersion();
    void addFile();
    void compile();
    void compileUtf8();
    void compileNoVersion();
};

ShaderGLTest::ShaderGLTest() {
    addTests({&ShaderGLTest::construct,
              &ShaderGLTest::constructNoVersion,
              &ShaderGLTest::constructMove,

              #ifndef MAGNUM_TARGET_WEBGL
              &ShaderGLTest::label,
              #endif

              &ShaderGLTest::addSource,
              &ShaderGLTest::addSourceNoVersion,
              &ShaderGLTest::addFile,
              &ShaderGLTest::compile,
              &ShaderGLTest::compileUtf8,
              &ShaderGLTest::compileNoVersion});
}

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

    shader.setLabel("MyShader");
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
        "#line 1 1\n",
        "#define FOO BAR\n",
        "#line 1 2\n",
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

    shader.addFile(Utility::Directory::join(SHADERGLTEST_FILES_DIR, "shader.glsl"));

    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(shader.sources(), (std::vector<std::string>{
        "#version 120\n",
        "#line 1 1\n",
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

    Shader shader2(v, Shader::Type::Fragment);
    shader2.addSource("[fu] bleh error #:! stuff\n");
    CORRADE_VERIFY(!shader2.compile());
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
