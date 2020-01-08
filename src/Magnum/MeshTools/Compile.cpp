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

#include "Compile.h"

#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Containers/ArrayViewStl.h> /** @todo remove once MeshData is sane */

#include "Magnum/GL/Buffer.h"
#include "Magnum/GL/Mesh.h"
#include "Magnum/Math/Vector3.h"
#include "Magnum/Math/Color.h"
#include "Magnum/MeshTools/CompressIndices.h"
#include "Magnum/MeshTools/GenerateNormals.h"
#include "Magnum/MeshTools/Duplicate.h"
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
    UnsignedInt textureCoordsOffset = sizeof(Shaders::Generic2D::Position::Type);
    UnsignedInt colorsOffset = sizeof(Shaders::Generic2D::Position::Type);
    if(meshData.hasTextureCoords2D()) {
        stride += sizeof(Shaders::Generic2D::TextureCoordinates::Type);
        colorsOffset += sizeof(Shaders::Generic2D::TextureCoordinates::Type);
    }
    if(meshData.hasColors())
        stride += sizeof(Shaders::Generic2D::Color4::Type);

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
            textureCoordsOffset,
            meshData.textureCoords2D(0),
            stride - textureCoordsOffset - sizeof(Shaders::Generic2D::TextureCoordinates::Type));
        mesh.addVertexBuffer(vertexBufferRef, 0,
            textureCoordsOffset,
            Shaders::Generic2D::TextureCoordinates(),
            stride - textureCoordsOffset - sizeof(Shaders::Generic2D::TextureCoordinates::Type));
    }

    /* Add also colors, if present */
    if(meshData.hasColors()) {
        MeshTools::interleaveInto(data,
            colorsOffset,
            meshData.colors(0),
            stride - colorsOffset - sizeof(Shaders::Generic3D::Color4::Type));
        mesh.addVertexBuffer(vertexBufferRef, 0,
            colorsOffset,
            Shaders::Generic3D::Color4(),
            stride - colorsOffset - sizeof(Shaders::Generic3D::Color4::Type));
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

GL::Mesh compile(const Trade::MeshData3D& meshData, CompileFlags flags) {
    GL::Mesh mesh;
    mesh.setPrimitive(meshData.primitive());

    const bool generateNormals = flags & (CompileFlag::GenerateFlatNormals|CompileFlag::GenerateSmoothNormals) && meshData.primitive() == MeshPrimitive::Triangles;

    /* Decide about stride and offsets */
    UnsignedInt stride = sizeof(Shaders::Generic3D::Position::Type);
    const UnsignedInt normalOffset = sizeof(Shaders::Generic3D::Position::Type);
    UnsignedInt textureCoordsOffset = sizeof(Shaders::Generic3D::Position::Type);
    UnsignedInt colorsOffset = sizeof(Shaders::Generic3D::Position::Type);
    if(meshData.hasNormals() || generateNormals) {
        stride += sizeof(Shaders::Generic3D::Normal::Type);
        textureCoordsOffset += sizeof(Shaders::Generic3D::Normal::Type);
        colorsOffset += sizeof(Shaders::Generic3D::Normal::Type);
    }
    if(meshData.hasTextureCoords2D()) {
        stride += sizeof(Shaders::Generic3D::TextureCoordinates::Type);
        colorsOffset += sizeof(Shaders::Generic3D::TextureCoordinates::Type);
    }
    if(meshData.hasColors())
        stride += sizeof(Shaders::Generic3D::Color4::Type);

    /* Create vertex buffer */
    GL::Buffer vertexBuffer{GL::Buffer::TargetHint::Array};
    GL::Buffer vertexBufferRef = GL::Buffer::wrap(vertexBuffer.id(), GL::Buffer::TargetHint::Array);

    /* Indirect reference to the mesh data -- either directly the original mesh
       data or processed ones */
    Containers::StridedArrayView1D<const Vector3> positions;
    Containers::StridedArrayView1D<const Vector3> normals;
    Containers::StridedArrayView1D<const Vector2> textureCoords2D;
    Containers::StridedArrayView1D<const Color4> colors;
    bool useIndices; /**< @todo turn into a view once compressIndices() takes views */

    /* If the mesh has no normals, we want to generate them and the mesh is an
       indexed triangle mesh, duplicate all attributes, otherwise just
       reference the original data */
    Containers::Array<Vector3> positionStorage;
    Containers::Array<Vector3> normalStorage;
    Containers::Array<Vector2> textureCoords2DStorage;
    Containers::Array<Color4> colorStorage;
    if(generateNormals) {
        /* If we want flat normals and the mesh is indexed, duplicate all
           attributes */
        if(flags & CompileFlag::GenerateFlatNormals && meshData.isIndexed()) {
            positionStorage = duplicate(
                Containers::stridedArrayView(meshData.indices()), Containers::stridedArrayView(meshData.positions(0)));
            positions = Containers::arrayView(positionStorage);
            if(meshData.hasTextureCoords2D()) {
                textureCoords2DStorage = duplicate(
                    Containers::stridedArrayView(meshData.indices()),
                    Containers::stridedArrayView(meshData.textureCoords2D(0)));
                textureCoords2D = Containers::arrayView(textureCoords2DStorage);
            }
            if(meshData.hasColors()) {
                colorStorage = duplicate(
                    Containers::stridedArrayView(meshData.indices()),
                    Containers::stridedArrayView(meshData.colors(0)));
                colors = Containers::arrayView(colorStorage);
            }
        } else {
            positions = meshData.positions(0);
            if(meshData.hasTextureCoords2D())
                textureCoords2D = meshData.textureCoords2D(0);
            if(meshData.hasColors())
                colors = meshData.colors(0);
        }

        if(flags & CompileFlag::GenerateFlatNormals || !meshData.isIndexed()) {
            normalStorage = generateFlatNormals(positions);
            useIndices = false;
        } else {
            normalStorage = generateSmoothNormals(meshData.indices(), positions);
            useIndices = true;
        }

        normals = Containers::arrayView(normalStorage);

    } else {
        positions = meshData.positions(0);
        if(meshData.hasNormals()) normals = meshData.normals(0);
        if(meshData.hasTextureCoords2D()) textureCoords2D = meshData.textureCoords2D(0);
        if(meshData.hasColors()) colors = meshData.colors(0);
        useIndices = meshData.isIndexed();
    }

    /* Interleave positions and put them in with ownership transfer, use the
       ref for the rest */
    Containers::Array<char> data = MeshTools::interleave(
        positions,
        stride - sizeof(Shaders::Generic3D::Position::Type));
    mesh.addVertexBuffer(std::move(vertexBuffer), 0,
        Shaders::Generic3D::Position(),
        stride - sizeof(Shaders::Generic3D::Position::Type));

    /* Add also normals, if present */
    if(normals) {
        MeshTools::interleaveInto(data,
            normalOffset,
            normals,
            stride - normalOffset - sizeof(Shaders::Generic3D::Normal::Type));
        mesh.addVertexBuffer(vertexBufferRef, 0,
            normalOffset,
            Shaders::Generic3D::Normal(),
            stride - normalOffset - sizeof(Shaders::Generic3D::Normal::Type));
    }

    /* Add also texture coordinates, if present */
    if(textureCoords2D) {
        MeshTools::interleaveInto(data,
            textureCoordsOffset,
            textureCoords2D,
            stride - textureCoordsOffset - sizeof(Shaders::Generic3D::TextureCoordinates::Type));
        mesh.addVertexBuffer(vertexBufferRef, 0,
            textureCoordsOffset,
            Shaders::Generic3D::TextureCoordinates(),
            stride - textureCoordsOffset - sizeof(Shaders::Generic3D::TextureCoordinates::Type));
    }

    /* Add also colors, if present */
    if(colors) {
        MeshTools::interleaveInto(data,
            colorsOffset,
            colors,
            stride - colorsOffset - sizeof(Shaders::Generic3D::Color4::Type));
        mesh.addVertexBuffer(vertexBufferRef, 0,
            colorsOffset,
            Shaders::Generic3D::Color4(),
            stride - colorsOffset - sizeof(Shaders::Generic3D::Color4::Type));
    }

    /* Fill vertex buffer with interleaved data */
    vertexBufferRef.setData(data, GL::BufferUsage::StaticDraw);

    /* If indexed (and the mesh didn't have the vertex data duplicated for flat
       normals), fill index buffer and configure indexed mesh */
    if(useIndices) {
        Containers::Array<char> indexData;
        MeshIndexType indexType;
        UnsignedInt indexStart, indexEnd;
        std::tie(indexData, indexType, indexStart, indexEnd) = MeshTools::compressIndices(meshData.indices());

        GL::Buffer indexBuffer{GL::Buffer::TargetHint::ElementArray};
        indexBuffer.setData(indexData, GL::BufferUsage::StaticDraw);
        mesh.setCount(meshData.indices().size())
            .setIndexBuffer(std::move(indexBuffer), 0, indexType, indexStart, indexEnd);

    /* Else set vertex count */
    } else mesh.setCount(positions.size());

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
