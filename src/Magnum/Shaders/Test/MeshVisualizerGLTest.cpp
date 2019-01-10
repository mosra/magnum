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

#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/OpenGLTester.h"
#include "Magnum/Shaders/MeshVisualizer.h"

namespace Magnum { namespace Shaders { namespace Test {

struct MeshVisualizerGLTest: GL::OpenGLTester {
    explicit MeshVisualizerGLTest();

    void construct();
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    void constructWireframeGeometryShader();
    #endif
    void constructWireframeNoGeometryShader();

    void constructMove();
};

MeshVisualizerGLTest::MeshVisualizerGLTest() {
    addTests({&MeshVisualizerGLTest::construct,
              #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
              &MeshVisualizerGLTest::constructWireframeGeometryShader,
              #endif
              &MeshVisualizerGLTest::constructWireframeNoGeometryShader,

              &MeshVisualizerGLTest::constructMove});
}

void MeshVisualizerGLTest::construct() {
    MeshVisualizer shader;
    CORRADE_COMPARE(shader.flags(), MeshVisualizer::Flags{});
    {
        #ifdef CORRADE_TARGET_APPLE
        CORRADE_EXPECT_FAIL("macOS drivers need insane amount of state to validate properly.");
        #endif
        CORRADE_VERIFY(shader.id());
        CORRADE_VERIFY(shader.validate().first);
    }
}

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void MeshVisualizerGLTest::constructWireframeGeometryShader() {
    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::geometry_shader4>())
        CORRADE_SKIP(GL::Extensions::ARB::geometry_shader4::string() + std::string(" is not supported"));
    #else
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::geometry_shader>())
        CORRADE_SKIP(GL::Extensions::EXT::geometry_shader::string() + std::string(" is not supported"));
    #endif

    #ifdef MAGNUM_TARGET_GLES
    if(GL::Context::current().isExtensionSupported<GL::Extensions::NV::shader_noperspective_interpolation>())
        Debug() << "Using" << GL::Extensions::NV::shader_noperspective_interpolation::string();
    #endif

    MeshVisualizer shader{MeshVisualizer::Flag::Wireframe};
    CORRADE_COMPARE(shader.flags(), MeshVisualizer::Flag::Wireframe);
    {
        #ifdef CORRADE_TARGET_APPLE
        CORRADE_EXPECT_FAIL("macOS drivers need insane amount of state to validate properly.");
        #endif
        CORRADE_VERIFY(shader.id());
        CORRADE_VERIFY(shader.validate().first);
    }
}
#endif

void MeshVisualizerGLTest::constructWireframeNoGeometryShader() {
    MeshVisualizer shader{MeshVisualizer::Flag::Wireframe|MeshVisualizer::Flag::NoGeometryShader};
    CORRADE_COMPARE(shader.flags(), MeshVisualizer::Flag::Wireframe|MeshVisualizer::Flag::NoGeometryShader);
    {
        #ifdef CORRADE_TARGET_APPLE
        CORRADE_EXPECT_FAIL("macOS drivers need insane amount of state to validate properly.");
        #endif
        CORRADE_VERIFY(shader.id());
        CORRADE_VERIFY(shader.validate().first);
    }
}

void MeshVisualizerGLTest::constructMove() {
    MeshVisualizer a{MeshVisualizer::Flag::Wireframe|MeshVisualizer::Flag::NoGeometryShader};
    const GLuint id = a.id();
    CORRADE_VERIFY(id);

    MAGNUM_VERIFY_NO_GL_ERROR();

    MeshVisualizer b{std::move(a)};
    CORRADE_COMPARE(b.id(), id);
    CORRADE_COMPARE(b.flags(), MeshVisualizer::Flag::Wireframe|MeshVisualizer::Flag::NoGeometryShader);
    CORRADE_VERIFY(!a.id());

    MeshVisualizer c{NoCreate};
    c = std::move(b);
    CORRADE_COMPARE(c.id(), id);
    CORRADE_COMPARE(c.flags(), MeshVisualizer::Flag::Wireframe|MeshVisualizer::Flag::NoGeometryShader);
    CORRADE_VERIFY(!b.id());
}

}}}

CORRADE_TEST_MAIN(Magnum::Shaders::Test::MeshVisualizerGLTest)
