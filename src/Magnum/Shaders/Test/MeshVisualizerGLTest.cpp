/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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

#include "Magnum/Context.h"
#include "Magnum/Extensions.h"
#include "Magnum/OpenGLTester.h"
#include "Magnum/Shaders/MeshVisualizer.h"

namespace Magnum { namespace Shaders { namespace Test {

struct MeshVisualizerGLTest: OpenGLTester {
    explicit MeshVisualizerGLTest();

    void compile();
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    void compileWireframeGeometryShader();
    #endif
    void compileWireframeNoGeometryShader();
};

MeshVisualizerGLTest::MeshVisualizerGLTest() {
    addTests({&MeshVisualizerGLTest::compile,
              #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
              &MeshVisualizerGLTest::compileWireframeGeometryShader,
              #endif
              &MeshVisualizerGLTest::compileWireframeNoGeometryShader});
}

void MeshVisualizerGLTest::compile() {
    Shaders::MeshVisualizer shader;
    {
        #ifdef CORRADE_TARGET_APPLE
        CORRADE_EXPECT_FAIL("macOS drivers need insane amount of state to validate properly.");
        #endif
        CORRADE_VERIFY(shader.validate().first);
    }
}

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void MeshVisualizerGLTest::compileWireframeGeometryShader() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::geometry_shader4>())
        CORRADE_SKIP(Extensions::GL::ARB::geometry_shader4::string() + std::string(" is not supported"));
    #else
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::geometry_shader>())
        CORRADE_SKIP(Extensions::GL::EXT::geometry_shader::string() + std::string(" is not supported"));
    #endif

    #ifdef MAGNUM_TARGET_GLES
    if(Context::current().isExtensionSupported<Extensions::GL::NV::shader_noperspective_interpolation>())
        Debug() << "Using" << Extensions::GL::NV::shader_noperspective_interpolation::string();
    #endif

    Shaders::MeshVisualizer shader(Shaders::MeshVisualizer::Flag::Wireframe);
    {
        #ifdef CORRADE_TARGET_APPLE
        CORRADE_EXPECT_FAIL("macOS drivers need insane amount of state to validate properly.");
        #endif
        CORRADE_VERIFY(shader.validate().first);
    }
}
#endif

void MeshVisualizerGLTest::compileWireframeNoGeometryShader() {
    Shaders::MeshVisualizer shader(Shaders::MeshVisualizer::Flag::Wireframe|Shaders::MeshVisualizer::Flag::NoGeometryShader);
    {
        #ifdef CORRADE_TARGET_APPLE
        CORRADE_EXPECT_FAIL("macOS drivers need insane amount of state to validate properly.");
        #endif
        CORRADE_VERIFY(shader.validate().first);
    }
}

}}}

CORRADE_TEST_MAIN(Magnum::Shaders::Test::MeshVisualizerGLTest)
