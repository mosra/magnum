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

#include "FullScreenTriangle.h"

#include "Math/Vector2.h"
#include "AbstractShaderProgram.h"
#include "Buffer.h"
#include "Context.h"
#include "Mesh.h"

namespace Magnum { namespace MeshTools {

std::pair<Buffer*, Mesh> fullScreenTriangle() {
    Mesh mesh;
    mesh.setPrimitive(Mesh::Primitive::Triangles)
        .setVertexCount(3);

    Buffer* buffer = nullptr;
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isVersionSupported(Version::GL300))
    #else
    if(!Context::current()->isVersionSupported(Version::GLES300))
    #endif
    {
        buffer = new Buffer;
        constexpr Vector2 triangle[] = {
            Vector2(-1.0,  1.0),
            Vector2(-1.0, -3.0),
            Vector2( 3.0,  1.0)
        };
        buffer->setData(triangle, Buffer::Usage::StaticDraw);
        /** @todo Is it possible to attach moveable buffer here to avoid heap
           allocation? OTOH this is more effective in most (modern) cases */
        mesh.addVertexBuffer(*buffer, 0, AbstractShaderProgram::Attribute<0, Vector2>());
    }

    return {std::move(buffer), std::move(mesh)};
}

}}
