/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
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

#include "Compile.h"

#include "Magnum/GL/Buffer.h"
#include "Magnum/GL/Mesh.h"
#include "Magnum/Math/Vector3.h"
#include "Magnum/MeshTools/CompressIndices.h"
#include "Magnum/MeshTools/Interleave.h"
#include "Magnum/Trade/MeshData2D.h"
#include "Magnum/Trade/MeshData3D.h"

/* This header is included only privately and doesn't introduce any linker
   dependency, thus it's completely safe */
#include "Magnum/Shaders/Generic.h"

namespace Magnum { namespace MeshTools {

GL::Mesh compile(const Trade::MeshData2D& meshData) {
    GL::Mesh mesh;
    mesh.setPrimitive(meshData.primitive());

    /* Decide about stride and offsets */
    UnsignedInt stride = sizeof(Shaders::Generic2D::Position::Type);
    const UnsignedInt normalOffset = sizeof(Shaders::Generic2D::Position::Type);
    if(meshData.hasTextureCoords2D())
        stride += sizeof(Shaders::Generic2D::TextureCoordinates::Type);

    /* Create vertex buffer */
    GL::Buffer vertexBuffer{GL::Buffer::TargetHint::Array};
    GL::Buffer vertexBufferRef = GL::Buffer::wrap(vertexBuffer.id(), GL::Buffer::TargetHint::Array);

    /* Interleave positions and put them in with ownership transfer, use the
       ref for the rest */
    Containers::Array<char> data = MeshTools::interleave(
        meshData.positions(0),
        stride - sizeof(Shaders::Generic2D::Position::Type));
    mesh.addVertexBuffer(std::move(vertexBuffer), 0,
        Shaders::Generic2D::Position(),
        stride - sizeof(Shaders::Generic2D::Position::Type));

    /* Add also texture coordinates, if present */
    if(meshData.hasTextureCoords2D()) {
        MeshTools::interleaveInto(data,
            normalOffset,
            meshData.textureCoords2D(0),
            stride - normalOffset - sizeof(Shaders::Generic2D::TextureCoordinates::Type));
        mesh.addVertexBuffer(vertexBufferRef, 0,
            normalOffset,
            Shaders::Generic2D::TextureCoordinates(),
            stride - normalOffset - sizeof(Shaders::Generic2D::TextureCoordinates::Type));
    }

    /* Fill vertex buffer with interleaved data */
    vertexBufferRef.setData(data, GL::BufferUsage::StaticDraw);

    /* If indexed, fill index buffer and configure indexed mesh */
    if(meshData.isIndexed()) {
        Containers::Array<char> indexData;
        MeshIndexType indexType;
        UnsignedInt indexStart, indexEnd;
        std::tie(indexData, indexType, indexStart, indexEnd) = MeshTools::compressIndices(meshData.indices());

        GL::Buffer indexBuffer{GL::Buffer::TargetHint::ElementArray};
        indexBuffer.setData(indexData, GL::BufferUsage::StaticDraw);
        mesh.setCount(meshData.indices().size())
            .setIndexBuffer(std::move(indexBuffer), 0, indexType, indexStart, indexEnd);

    /* Else set vertex count */
    } else mesh.setCount(meshData.positions(0).size());

    return mesh;
}

#ifdef MAGNUM_BUILD_DEPRECATED
std::tuple<GL::Mesh, std::unique_ptr<GL::Buffer>, std::unique_ptr<GL::Buffer>> compile(const Trade::MeshData2D& meshData, GL::BufferUsage) {
    return std::make_tuple(compile(meshData),
        std::unique_ptr<GL::Buffer>{new GL::Buffer{NoCreate}},
        std::unique_ptr<GL::Buffer>{meshData.isIndexed() ? new GL::Buffer{NoCreate} : nullptr});
}
#endif

GL::Mesh compile(const Trade::MeshData3D& meshData) {
    GL::Mesh mesh;
    mesh.setPrimitive(meshData.primitive());

    /* Decide about stride and offsets */
    UnsignedInt stride = sizeof(Shaders::Generic3D::Position::Type);
    const UnsignedInt normalOffset = sizeof(Shaders::Generic3D::Position::Type);
    UnsignedInt textureCoordsOffset = sizeof(Shaders::Generic3D::Position::Type);
    if(meshData.hasNormals()) {
        stride += sizeof(Shaders::Generic3D::Normal::Type);
        textureCoordsOffset += sizeof(Shaders::Generic3D::Normal::Type);
    }
    if(meshData.hasTextureCoords2D())
        stride += sizeof(Shaders::Generic3D::TextureCoordinates::Type);

    /* Create vertex buffer */
    GL::Buffer vertexBuffer{GL::Buffer::TargetHint::Array};
    GL::Buffer vertexBufferRef = GL::Buffer::wrap(vertexBuffer.id(), GL::Buffer::TargetHint::Array);

    /* Interleave positions and put them in with ownership transfer, use the
       ref for the rest */
    Containers::Array<char> data = MeshTools::interleave(
        meshData.positions(0),
        stride - sizeof(Shaders::Generic3D::Position::Type));
    mesh.addVertexBuffer(std::move(vertexBuffer), 0,
        Shaders::Generic3D::Position(),
        stride - sizeof(Shaders::Generic3D::Position::Type));

    /* Add also normals, if present */
    if(meshData.hasNormals()) {
        MeshTools::interleaveInto(data,
            normalOffset,
            meshData.normals(0),
            stride - normalOffset - sizeof(Shaders::Generic3D::Normal::Type));
        mesh.addVertexBuffer(vertexBufferRef, 0,
            normalOffset,
            Shaders::Generic3D::Normal(),
            stride - normalOffset - sizeof(Shaders::Generic3D::Normal::Type));
    }

    /* Add also texture coordinates, if present */
    if(meshData.hasTextureCoords2D()) {
        MeshTools::interleaveInto(data,
            textureCoordsOffset,
            meshData.textureCoords2D(0),
            stride - textureCoordsOffset - sizeof(Shaders::Generic3D::TextureCoordinates::Type));
        mesh.addVertexBuffer(vertexBufferRef, 0,
            textureCoordsOffset,
            Shaders::Generic3D::TextureCoordinates(),
            stride - textureCoordsOffset - sizeof(Shaders::Generic3D::TextureCoordinates::Type));
    }

    /* Fill vertex buffer with interleaved data */
    vertexBufferRef.setData(data, GL::BufferUsage::StaticDraw);

    /* If indexed, fill index buffer and configure indexed mesh */
    if(meshData.isIndexed()) {
        Containers::Array<char> indexData;
        MeshIndexType indexType;
        UnsignedInt indexStart, indexEnd;
        std::tie(indexData, indexType, indexStart, indexEnd) = MeshTools::compressIndices(meshData.indices());

        GL::Buffer indexBuffer{GL::Buffer::TargetHint::ElementArray};
        indexBuffer.setData(indexData, GL::BufferUsage::StaticDraw);
        mesh.setCount(meshData.indices().size())
            .setIndexBuffer(std::move(indexBuffer), 0, indexType, indexStart, indexEnd);

    /* Else set vertex count */
    } else mesh.setCount(meshData.positions(0).size());

    return mesh;
}

#ifdef MAGNUM_BUILD_DEPRECATED
std::tuple<GL::Mesh, std::unique_ptr<GL::Buffer>, std::unique_ptr<GL::Buffer>> compile(const Trade::MeshData3D& meshData, GL::BufferUsage) {
    return std::make_tuple(compile(meshData),
        std::unique_ptr<GL::Buffer>{new GL::Buffer{NoCreate}},
        std::unique_ptr<GL::Buffer>{meshData.isIndexed() ? new GL::Buffer{NoCreate} : nullptr});
}
#endif

}}
