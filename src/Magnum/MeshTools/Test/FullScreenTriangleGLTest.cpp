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

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/Reference.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/Utility/Resource.h>

#include "Magnum/Image.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/GL/AbstractShaderProgram.h"
#include "Magnum/GL/Framebuffer.h"
#include "Magnum/GL/Mesh.h"
#include "Magnum/GL/OpenGLTester.h"
#include "Magnum/GL/Renderbuffer.h"
#include "Magnum/GL/RenderbufferFormat.h"
#include "Magnum/GL/Shader.h"
#include "Magnum/GL/Version.h"
#include "Magnum/Math/Color.h"
#include "Magnum/MeshTools/FullScreenTriangle.h"
#include "Magnum/Shaders/Implementation/CreateCompatibilityShader.h"

namespace Magnum { namespace MeshTools { namespace Test { namespace {

struct FullScreenTriangleGLTest: GL::OpenGLTester {
    explicit FullScreenTriangleGLTest();

    void test();
};

constexpr struct {
    const char* name;
    GL::Version version;
} VersionData[] {
    #ifndef MAGNUM_TARGET_GLES
    {"GL 2.1", GL::Version::GL210},
    {"GL 3.0", GL::Version::GL300}
    #else
    {"GLES 2.0", GL::Version::GLES200},
    #ifndef MAGNUM_TARGET_GLES2
    {"GLES 3.0", GL::Version::GLES300}
    #endif
    #endif
};

FullScreenTriangleGLTest::FullScreenTriangleGLTest() {
    addInstancedTests({&FullScreenTriangleGLTest::test},
        Containers::arraySize(VersionData));
}

void FullScreenTriangleGLTest::test() {
    auto&& data = VersionData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    if(!GL::Context::current().isVersionSupported(data.version))
        CORRADE_SKIP("Version not supported");

    struct FullscreenFlatShader: GL::AbstractShaderProgram {
        FullscreenFlatShader(GL::Version version) {
            Utility::Resource rs{"FullScreenTriangleTest"};

            GL::Shader vert = Shaders::Implementation::createCompatibilityShader(rs, version, GL::Shader::Type::Vertex);
            vert.addSource(rs.get("FullScreenTriangle.glsl"))
                .addSource(R"(
void main() {
    fullScreenTriangle();
}
                )");

            GL::Shader frag = Shaders::Implementation::createCompatibilityShader(rs, version, GL::Shader::Type::Fragment);
            frag.addSource(R"(
#ifdef NEW_GLSL
out lowp vec4 fragmentColor;
#else
#define fragmentColor gl_FragColor
#endif

void main() {
    fragmentColor = vec4(1.0, 0.5, 1.0, 0.5);
}
            )");

            CORRADE_INTERNAL_ASSERT_OUTPUT(GL::Shader::compile({vert, frag}));

            attachShaders({vert, frag});

            CORRADE_INTERNAL_ASSERT_OUTPUT(link());
        }
    } shader{data.version};

    MAGNUM_VERIFY_NO_GL_ERROR();

    GL::Renderbuffer color;
    color.setStorage(
        #ifndef MAGNUM_TARGET_GLES2
        GL::RenderbufferFormat::RGBA8,
        #else
        GL::RenderbufferFormat::RGBA4,
        #endif
        Vector2i{4});
    GL::Framebuffer framebuffer{{{}, Vector2i{4}}};
    framebuffer.attachRenderbuffer(GL::Framebuffer::ColorAttachment{0}, color)
        .bind();

    MAGNUM_VERIFY_NO_GL_ERROR();

    shader.draw(fullScreenTriangle(data.version));

    MAGNUM_VERIFY_NO_GL_ERROR();

    using namespace Math::Literals;

    Image2D image = framebuffer.read({{}, Vector2i{4}}, PixelFormat::RGBA8Unorm);
    CORRADE_COMPARE_AS(Containers::arrayCast<Color4ub>(image.data()), Containers::arrayView(Containers::Array<Color4ub>{Containers::InPlaceInit, {
        0xff80ff80_rgba, 0xff80ff80_rgba, 0xff80ff80_rgba, 0xff80ff80_rgba,
        0xff80ff80_rgba, 0xff80ff80_rgba, 0xff80ff80_rgba, 0xff80ff80_rgba,
        0xff80ff80_rgba, 0xff80ff80_rgba, 0xff80ff80_rgba, 0xff80ff80_rgba,
        0xff80ff80_rgba, 0xff80ff80_rgba, 0xff80ff80_rgba, 0xff80ff80_rgba
    }}), TestSuite::Compare::Container);
}

}}}}

CORRADE_TEST_MAIN(Magnum::MeshTools::Test::FullScreenTriangleGLTest)
