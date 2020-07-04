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

#include <tuple> /* for std::tie() :( */
#include <Corrade/Containers/StridedArrayView.h>

#include "Magnum/GL/Buffer.h"
#include "Magnum/GL/Mesh.h"
#include "Magnum/Math/Vector3.h"
#include "Magnum/MeshTools/Compile.h"
#include "Magnum/MeshTools/CompressIndices.h"
#include "Magnum/MeshTools/Interleave.h"
#include "Magnum/Trade/MeshData.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <vector>
#endif

using namespace Magnum;

int main() {

{
Trade::MeshData meshData{MeshPrimitive::Lines, 5};
/* [compile-external] */
GL::Buffer indices, vertices;
indices.setData(meshData.indexData());
vertices.setData(meshData.vertexData());

GL::Mesh mesh = MeshTools::compile(meshData, indices, vertices);
/* [compile-external] */
}

{
Trade::MeshData meshData{MeshPrimitive::Lines, 5};
Trade::MeshAttribute myCustomAttribute{};
/* [compile-external-attributes] */
GL::Buffer indices, vertices;
indices.setData(meshData.indexData());
vertices.setData(meshData.vertexData());

/* Let compile() handle the usual attributes and configure custom ones after */
GL::Mesh mesh = MeshTools::compile(meshData, std::move(indices), vertices);
mesh.addVertexBuffer(std::move(vertices),
    meshData.attributeOffset(myCustomAttribute),
    meshData.attributeStride(myCustomAttribute),
    GL::DynamicAttribute{
        GL::DynamicAttribute::Kind::Generic, 7,
        GL::DynamicAttribute::Components::One,
        GL::DynamicAttribute::DataType::Float});
/* [compile-external-attributes] */
}

{
/* [compressIndices] */
Containers::Array<UnsignedInt> indices;

Containers::Array<char> indexData;
MeshIndexType indexType;
std::tie(indexData, indexType) = MeshTools::compressIndices(indices);

GL::Buffer indexBuffer;
indexBuffer.setData(indexData);

GL::Mesh mesh;
mesh.setCount(indices.size())
    .setIndexBuffer(indexBuffer, 0, indexType);
/* [compressIndices] */
}

#ifdef MAGNUM_BUILD_DEPRECATED
{
CORRADE_IGNORE_DEPRECATED_PUSH
/* [compressIndices-stl] */
std::vector<UnsignedInt> indices;

Containers::Array<char> indexData;
MeshIndexType indexType;
UnsignedInt indexStart, indexEnd;
std::tie(indexData, indexType, indexStart, indexEnd) =
    MeshTools::compressIndices(indices);

GL::Buffer indexBuffer;
indexBuffer.setData(indexData, GL::BufferUsage::StaticDraw);

GL::Mesh mesh;
mesh.setCount(indices.size())
    .setIndexBuffer(indexBuffer, 0, indexType, indexStart, indexEnd);
/* [compressIndices-stl] */
}
CORRADE_IGNORE_DEPRECATED_POP
#endif

{
struct MyShader {
    typedef GL::Attribute<0, Vector3> Position;
    typedef GL::Attribute<0, Vector2> TextureCoordinates;
};
/* [interleave1] */
Containers::ArrayView<const Vector3> positions;
Containers::ArrayView<const Vector2> textureCoordinates;

GL::Buffer vertexBuffer;
vertexBuffer.setData(MeshTools::interleave(positions, textureCoordinates));

GL::Mesh mesh;
mesh.setCount(positions.size())
    .addVertexBuffer(vertexBuffer, 0, MyShader::Position{},
                                      MyShader::TextureCoordinates{});
/* [interleave1] */
}

}
