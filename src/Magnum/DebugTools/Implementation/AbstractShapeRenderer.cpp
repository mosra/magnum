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

#define _MAGNUM_DO_NOT_WARN_DEPRECATED_SHAPES

#include "AbstractShapeRenderer.h"

#include <Corrade/Containers/Array.h>

#include "Magnum/DebugTools/ResourceManager.h"
#include "Magnum/GL/AbstractShaderProgram.h"
#include "Magnum/GL/Buffer.h"
#include "Magnum/GL/Mesh.h"
#include "Magnum/MeshTools/CompressIndices.h"
#include "Magnum/Shaders/Flat.h"
#include "Magnum/Trade/MeshData2D.h"
#include "Magnum/Trade/MeshData3D.h"

namespace Magnum { namespace DebugTools { namespace Implementation {

CORRADE_IGNORE_DEPRECATED_PUSH
namespace {

template<UnsignedInt dimensions> ResourceKey shaderKey();
template<> inline ResourceKey shaderKey<2>() { return ResourceKey("FlatShader2D"); }
template<> inline ResourceKey shaderKey<3>() { return ResourceKey("FlatShader3D"); }

template<UnsignedInt dimensions> void create(typename MeshData<dimensions>::Type&, Resource<GL::Mesh>&, Resource<GL::Buffer>&, Resource<GL::Buffer>&);

template<> void create<2>(Trade::MeshData2D& data, Resource<GL::Mesh>& meshResource, Resource<GL::Buffer>& vertexBufferResource, Resource<GL::Buffer>& indexBufferResource) {
    /* Vertex buffer */
    GL::Buffer* buffer = new GL::Buffer{GL::Buffer::TargetHint::Array};
    buffer->setData(data.positions(0), GL::BufferUsage::StaticDraw);
    ResourceManager::instance().set(vertexBufferResource.key(), buffer, ResourceDataState::Final, ResourcePolicy::Manual);

    /* Mesh configuration */
    GL::Mesh* mesh = new GL::Mesh;
    mesh->setPrimitive(data.primitive())
        .addVertexBuffer(*buffer, 0, Shaders::Flat2D::Position());
    ResourceManager::instance().set(meshResource.key(), mesh, ResourceDataState::Final, ResourcePolicy::Manual);

    /* Index buffer, if needed, if not, resource key doesn't have to be set */
    if(data.isIndexed()) {
        CORRADE_INTERNAL_ASSERT(indexBufferResource.key() != ResourceKey());

        Containers::Array<char> indexData;
        MeshIndexType indexType;
        UnsignedInt indexStart, indexEnd;
        std::tie(indexData, indexType, indexStart, indexEnd) = MeshTools::compressIndices(data.indices());

        GL::Buffer* indexBuffer = new GL::Buffer{GL::Buffer::TargetHint::ElementArray};
        indexBuffer->setData(indexData, GL::BufferUsage::StaticDraw);
        mesh->setCount(data.indices().size())
            .setIndexBuffer(*indexBuffer, 0, indexType, indexStart, indexEnd);

        ResourceManager::instance().set(indexBufferResource.key(), indexBuffer, ResourceDataState::Final, ResourcePolicy::Manual);

    /* The mesh is not indexed, set proper vertex count */
    } else mesh->setCount(data.positions(0).size());
}

template<> void create<3>(Trade::MeshData3D& data, Resource<GL::Mesh>& meshResource, Resource<GL::Buffer>& vertexBufferResource, Resource<GL::Buffer>& indexBufferResource) {
    /* Vertex buffer */
    GL::Buffer* vertexBuffer = new GL::Buffer{GL::Buffer::TargetHint::Array};
    vertexBuffer->setData(data.positions(0), GL::BufferUsage::StaticDraw);
    ResourceManager::instance().set(vertexBufferResource.key(), vertexBuffer, ResourceDataState::Final, ResourcePolicy::Manual);

    /* Mesh configuration */
    GL::Mesh* mesh = new GL::Mesh;
    mesh->setPrimitive(data.primitive())
        .addVertexBuffer(*vertexBuffer, 0, Shaders::Flat3D::Position());
    ResourceManager::instance().set(meshResource.key(), mesh, ResourceDataState::Final, ResourcePolicy::Manual);

    /* Index buffer, if needed, if not, resource key doesn't have to be set */
    if(data.isIndexed()) {
        CORRADE_INTERNAL_ASSERT(indexBufferResource.key() != ResourceKey());

        Containers::Array<char> indexData;
        MeshIndexType indexType;
        UnsignedInt indexStart, indexEnd;
        std::tie(indexData, indexType, indexStart, indexEnd) = MeshTools::compressIndices(data.indices());

        GL::Buffer* indexBuffer = new GL::Buffer{GL::Buffer::TargetHint::ElementArray};
        indexBuffer->setData(indexData, GL::BufferUsage::StaticDraw);
        mesh->setCount(data.indices().size())
            .setIndexBuffer(*indexBuffer, 0, indexType, indexStart, indexEnd);

        ResourceManager::instance().set(indexBufferResource.key(), indexBuffer, ResourceDataState::Final, ResourcePolicy::Manual);

    /* The mesh is not indexed, set proper vertex count */
    } else mesh->setCount(data.positions(0).size());
}

}

template<UnsignedInt dimensions> AbstractShapeRenderer<dimensions>::AbstractShapeRenderer(ResourceKey meshKey, ResourceKey vertexBufferKey, ResourceKey indexBufferKey) {
    wireframeShader = ResourceManager::instance().get<GL::AbstractShaderProgram, Shaders::Flat<dimensions>>(shaderKey<dimensions>());
    wireframeMesh = ResourceManager::instance().get<GL::Mesh>(meshKey);
    vertexBuffer = ResourceManager::instance().get<GL::Buffer>(vertexBufferKey);
    indexBuffer = ResourceManager::instance().get<GL::Buffer>(indexBufferKey);

    if(!wireframeShader) ResourceManager::instance().set<GL::AbstractShaderProgram>(shaderKey<dimensions>(),
        new Shaders::Flat<dimensions>, ResourceDataState::Final, ResourcePolicy::Resident);
}

template<UnsignedInt dimensions> AbstractShapeRenderer<dimensions>::~AbstractShapeRenderer() = default;

template<UnsignedInt dimensions> void AbstractShapeRenderer<dimensions>::createResources(typename MeshData<dimensions>::Type data) {
    create<dimensions>(data, wireframeMesh, vertexBuffer, indexBuffer);
}

template class AbstractShapeRenderer<2>;
template class AbstractShapeRenderer<3>;
CORRADE_IGNORE_DEPRECATED_POP

}}}
