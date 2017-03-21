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

#include "FullScreenTriangle.h"

#include "Magnum/Attribute.h"
#include "Magnum/Buffer.h"
#include "Magnum/Context.h"
#include "Magnum/Mesh.h"
#include "Magnum/Version.h"
#include "Magnum/Math/Vector2.h"

namespace Magnum { namespace MeshTools {

std::pair<std::unique_ptr<Buffer>, Mesh> fullScreenTriangle(Version version) {
    Mesh mesh;
    mesh.setPrimitive(MeshPrimitive::Triangles)
        .setCount(3);

    std::unique_ptr<Buffer> buffer;
    #ifndef MAGNUM_TARGET_GLES
    if(version < Version::GL300)
    #else
    if(version < Version::GLES300)
    #endif
    {
        buffer.reset(new Buffer);
        constexpr Vector2 triangle[] = {
            {-1.0f,  1.0f},
            {-1.0f, -3.0f},
            { 3.0f,  1.0f}
        };
        buffer->setData(triangle, BufferUsage::StaticDraw);
        /** @todo Is it possible to attach moveable buffer here to avoid heap
           allocation? OTOH this is more effective in most (modern) cases */
        mesh.addVertexBuffer(*buffer, 0, Attribute<0, Vector2>{});
    }

    return {std::move(buffer), std::move(mesh)};
}

std::pair<std::unique_ptr<Buffer>, Mesh> fullScreenTriangle() {
    return fullScreenTriangle(Context::current().version());
}

}}
