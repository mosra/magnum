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

#include "FullScreenTriangle.h"

#include "Magnum/GL/Attribute.h"
#include "Magnum/GL/Buffer.h"
#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/Mesh.h"
#include "Magnum/GL/Version.h"
#include "Magnum/Math/Vector2.h"

namespace Magnum { namespace MeshTools {

GL::Mesh fullScreenTriangle(const GL::Version version) {
    GL::Mesh mesh;
    mesh.setPrimitive(GL::MeshPrimitive::Triangles)
        .setCount(3);

    #ifndef MAGNUM_TARGET_GLES2
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::MAGNUM::shader_vertex_id>(version))
    #endif
    {
        constexpr Vector2 triangle[]{
            {-1.0f,  1.0f},
            {-1.0f, -3.0f},
            { 3.0f,  1.0f}
        };
        GL::Buffer buffer{GL::Buffer::TargetHint::Array};
        buffer.setData(triangle, GL::BufferUsage::StaticDraw);
        mesh.addVertexBuffer(std::move(buffer), 0, GL::Attribute<0, Vector2>{});
    }

    #ifdef MAGNUM_TARGET_GLES2
    static_cast<void>(version);
    #endif

    return mesh;
}

GL::Mesh fullScreenTriangle() {
    return fullScreenTriangle(GL::Context::current().version());
}

}}
