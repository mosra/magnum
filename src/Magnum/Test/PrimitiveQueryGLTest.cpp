/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014
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

#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/Resource.h>

#include "Magnum/AbstractShaderProgram.h"
#include "Magnum/Buffer.h"
#include "Magnum/Framebuffer.h"
#include "Magnum/Mesh.h"
#include "Magnum/PrimitiveQuery.h"
#include "Magnum/Renderbuffer.h"
#include "Magnum/RenderbufferFormat.h"
#include "Magnum/Shader.h"
#include "Magnum/Test/AbstractOpenGLTester.h"

namespace Magnum { namespace Test {

class PrimitiveQueryGLTest: public AbstractOpenGLTester {
    public:
        explicit PrimitiveQueryGLTest();

        void query();
};

PrimitiveQueryGLTest::PrimitiveQueryGLTest() {
    addTests({&PrimitiveQueryGLTest::query});
}

void PrimitiveQueryGLTest::query() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::transform_feedback>())
        CORRADE_SKIP(Extensions::GL::EXT::transform_feedback::string() + std::string(" is not available."));
    #endif

    #ifndef MAGNUM_TARGET_GLES
    class MyShader: public AbstractShaderProgram {
        public:
            typedef Attribute<0, Vector2> Position;

            explicit MyShader() {
                Utility::Resource rs("QueryGLTest");

                Shader vert(Version::GL210, Shader::Type::Vertex);
                Shader frag(Version::GL210, Shader::Type::Fragment);

                vert.addSource(rs.get("MyShader.vert"));
                frag.addSource(rs.get("MyShader.frag"));

                CORRADE_INTERNAL_ASSERT_OUTPUT(Shader::compile({std::ref(vert), std::ref(frag)}));

                attachShaders({std::ref(vert), std::ref(frag)});

                CORRADE_INTERNAL_ASSERT_OUTPUT(link());
            }
    } shader;

    Renderbuffer renderbuffer;
    renderbuffer.setStorage(RenderbufferFormat::RGBA8, Vector2i(32));

    Framebuffer framebuffer({{}, Vector2i(32)});
    framebuffer.attachRenderbuffer(Framebuffer::ColorAttachment(0), renderbuffer);

    constexpr Vector2 data[9];
    Buffer vertices;
    vertices.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setPrimitive(MeshPrimitive::Triangles)
        .setCount(9)
        .addVertexBuffer(vertices, 0, MyShader::Position());

    MAGNUM_VERIFY_NO_ERROR();

    PrimitiveQuery q{PrimitiveQuery::Target::PrimitivesGenerated};
    q.begin();

    framebuffer.bind(FramebufferTarget::ReadDraw);
    mesh.draw(shader);

    q.end();
    const bool availableBefore = q.resultAvailable();
    const UnsignedInt count = q.result<UnsignedInt>();
    const bool availableAfter = q.resultAvailable();

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_VERIFY(!availableBefore);
    CORRADE_VERIFY(availableAfter);
    CORRADE_COMPARE(count, 3);
    #else
    CORRADE_SKIP("Not implemented in OpenGL ES 3.0 yet.");
    #endif
}

}}

CORRADE_TEST_MAIN(Magnum::Test::PrimitiveQueryGLTest)
