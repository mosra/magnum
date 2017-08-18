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

#include "AbstractShapeRenderer.h"

#include <Corrade/Containers/Array.h>

#include "Magnum/AbstractShaderProgram.h"
#include "Magnum/Buffer.h"
#include "Magnum/Mesh.h"
#include "Magnum/DebugTools/ResourceManager.h"
#include "Magnum/MeshTools/CompressIndices.h"
#include "Magnum/Shaders/Flat.h"
#include "Magnum/Trade/MeshData2D.h"
#include "Magnum/Trade/MeshData3D.h"

namespace Magnum { namespace DebugTools { namespace Implementation {

namespace {

template<UnsignedInt dimensions> ResourceKey shaderKey();
template<> inline ResourceKey shaderKey<2>() { return ResourceKey("FlatShader2D"); }
template<> inline ResourceKey shaderKey<3>() { return ResourceKey("FlatShader3D"); }

template<UnsignedInt dimensions> void create(typename MeshData<dimensions>::Type&, Resource<Mesh>&, Resource<Buffer>&, Resource<Buffer>&);

template<> void create<2>(Trade::MeshData2D& data, Resource<Mesh>& meshResource, Resource<Buffer>& vertexBufferResource, Resource<Buffer>& indexBufferResource) {
    /* Vertex buffer */
    Buffer* buffer = new Buffer{Buffer::TargetHint::Array};
    buffer->setData(data.positions(0), BufferUsage::StaticDraw);
    ResourceManager::instance().set(vertexBufferResource.key(), buffer, ResourceDataState::Final, ResourcePolicy::Manual);

    /* Mesh configuration */
    Mesh* mesh = new Mesh;
    mesh->setPrimitive(data.primitive())
        .addVertexBuffer(*buffer, 0, Shaders::Flat2D::Position());
    ResourceManager::instance().set(meshResource.key(), mesh, ResourceDataState::Final, ResourcePolicy::Manual);

    /* Index buffer, if needed, if not, resource key doesn't have to be set */
    if(data.isIndexed()) {
        CORRADE_INTERNAL_ASSERT(indexBufferResource.key() != ResourceKey());

        Containers::Array<char> indexData;
        Mesh::IndexType indexType;
        UnsignedInt indexStart, indexEnd;
        std::tie(indexData, indexType, indexStart, indexEnd) = MeshTools::compressIndices(data.indices());

        Buffer* indexBuffer = new Buffer{Buffer::TargetHint::ElementArray};
        indexBuffer->setData(indexData, BufferUsage::StaticDraw);
        mesh->setCount(data.indices().size())
            .setIndexBuffer(*indexBuffer, 0, indexType, indexStart, indexEnd);

        ResourceManager::instance().set(indexBufferResource.key(), indexBuffer, ResourceDataState::Final, ResourcePolicy::Manual);

    /* The mesh is not indexed, set proper vertex count */
    } else mesh->setCount(data.positions(0).size());
}

template<> void create<3>(Trade::MeshData3D& data, Resource<Mesh>& meshResource, Resource<Buffer>& vertexBufferResource, Resource<Buffer>& indexBufferResource) {
    /* Vertex buffer */
    Buffer* vertexBuffer = new Buffer{Buffer::TargetHint::Array};
    vertexBuffer->setData(data.positions(0), BufferUsage::StaticDraw);
    ResourceManager::instance().set(vertexBufferResource.key(), vertexBuffer, ResourceDataState::Final, ResourcePolicy::Manual);

    /* Mesh configuration */
    Mesh* mesh = new Mesh;
    mesh->setPrimitive(data.primitive())
        .addVertexBuffer(*vertexBuffer, 0, Shaders::Flat3D::Position());
    ResourceManager::instance().set(meshResource.key(), mesh, ResourceDataState::Final, ResourcePolicy::Manual);

    /* Index buffer, if needed, if not, resource key doesn't have to be set */
    if(data.isIndexed()) {
        CORRADE_INTERNAL_ASSERT(indexBufferResource.key() != ResourceKey());

        Containers::Array<char> indexData;
        Mesh::IndexType indexType;
        UnsignedInt indexStart, indexEnd;
        std::tie(indexData, indexType, indexStart, indexEnd) = MeshTools::compressIndices(data.indices());

        Buffer* indexBuffer = new Buffer{Buffer::TargetHint::ElementArray};
        indexBuffer->setData(indexData, BufferUsage::StaticDraw);
        mesh->setCount(data.indices().size())
            .setIndexBuffer(*indexBuffer, 0, indexType, indexStart, indexEnd);

        ResourceManager::instance().set(indexBufferResource.key(), indexBuffer, ResourceDataState::Final, ResourcePolicy::Manual);

    /* The mesh is not indexed, set proper vertex count */
    } else mesh->setCount(data.positions(0).size());
}

}

template<UnsignedInt dimensions> AbstractShapeRenderer<dimensions>::AbstractShapeRenderer(ResourceKey meshKey, ResourceKey vertexBufferKey, ResourceKey indexBufferKey) {
    wireframeShader = ResourceManager::instance().get<AbstractShaderProgram, Shaders::Flat<dimensions>>(shaderKey<dimensions>());
    wireframeMesh = ResourceManager::instance().get<Mesh>(meshKey);
    vertexBuffer = ResourceManager::instance().get<Buffer>(vertexBufferKey);
    indexBuffer = ResourceManager::instance().get<Buffer>(indexBufferKey);

    if(!wireframeShader) ResourceManager::instance().set<AbstractShaderProgram>(shaderKey<dimensions>(),
        new Shaders::Flat<dimensions>, ResourceDataState::Final, ResourcePolicy::Resident);
}

template<UnsignedInt dimensions> AbstractShapeRenderer<dimensions>::~AbstractShapeRenderer() = default;

template<UnsignedInt dimensions> void AbstractShapeRenderer<dimensions>::createResources(typename MeshData<dimensions>::Type data) {
    create<dimensions>(data, wireframeMesh, vertexBuffer, indexBuffer);
}

template class AbstractShapeRenderer<2>;
template class AbstractShapeRenderer<3>;

}}}
