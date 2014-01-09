/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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

#include "Context.h"
#include "Extensions.h"
#include "Shaders/MeshVisualizer.h"
#include "Test/AbstractOpenGLTester.h"

#ifdef MAGNUM_BUILD_STATIC
#include "Shaders/magnumShadersResourceImport.hpp"
#endif

namespace Magnum { namespace Shaders { namespace Test {

class MeshVisualizerGLTest: public Magnum::Test::AbstractOpenGLTester {
    public:
        explicit MeshVisualizerGLTest();

        void compile();
        void compileWireframeGeometryShader();
        void compileWireframeNoGeometryShader();
};

MeshVisualizerGLTest::MeshVisualizerGLTest() {
    addTests({&MeshVisualizerGLTest::compile,
              &MeshVisualizerGLTest::compileWireframeGeometryShader,
              &MeshVisualizerGLTest::compileWireframeNoGeometryShader});
}

void MeshVisualizerGLTest::compile() {
    Shaders::MeshVisualizer shader;
    CORRADE_VERIFY(shader.validate().first);
}

void MeshVisualizerGLTest::compileWireframeGeometryShader() {
    #ifdef MAGNUM_TARGET_GLES
    CORRADE_SKIP("Geometry shader is not available in OpenGL ES");
    #else
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::geometry_shader4>())
        CORRADE_SKIP(Extensions::GL::ARB::geometry_shader4::string() + std::string(" is not supported"));

    Shaders::MeshVisualizer shader(Shaders::MeshVisualizer::Flag::Wireframe);
    CORRADE_VERIFY(shader.validate().first);
    #endif
}

void MeshVisualizerGLTest::compileWireframeNoGeometryShader() {
    Shaders::MeshVisualizer shader(Shaders::MeshVisualizer::Flag::Wireframe|Shaders::MeshVisualizer::Flag::NoGeometryShader);
    CORRADE_VERIFY(shader.validate().first);
}

}}}

CORRADE_TEST_MAIN(Magnum::Shaders::Test::MeshVisualizerGLTest)
