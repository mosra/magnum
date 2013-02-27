/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

#include "AbstractShapeRenderer.h"

#include "AbstractShaderProgram.h"
#include "Buffer.h"
#include "Mesh.h"
#include "DebugTools/ResourceManager.h"
#include "MeshTools/CompressIndices.h"
#include "Shaders/FlatShader.h"
#include "Trade/MeshData2D.h"
#include "Trade/MeshData3D.h"

namespace Magnum { namespace DebugTools { namespace Implementation {

namespace {

template<UnsignedInt dimensions> ResourceKey shaderKey();
template<> inline ResourceKey shaderKey<2>() { return ResourceKey("FlatShader2D"); }
template<> inline ResourceKey shaderKey<3>() { return ResourceKey("FlatShader3D"); }

template<UnsignedInt dimensions> void create(typename MeshData<dimensions>::Type&, Resource<Mesh>&, Resource<Buffer>&, Resource<Buffer>&);

template<> void create<2>(Trade::MeshData2D& data, Resource<Mesh>& meshResource, Resource<Buffer>& vertexBufferResource, Resource<Buffer>& indexBufferResource) {
    /* Vertex buffer */
    Buffer* buffer = new Buffer(Buffer::Target::Array);
    buffer->setData(*data.positions(0), Buffer::Usage::StaticDraw);
    ResourceManager::instance()->set(vertexBufferResource.key(), buffer, ResourceDataState::Final, ResourcePolicy::Manual);

    /* Mesh configuration */
    Mesh* mesh = new Mesh;
    mesh->setPrimitive(data.primitive())
        ->setVertexCount(data.positions(0)->size())
        ->addVertexBuffer(buffer, 0, Shaders::FlatShader2D::Position());
    ResourceManager::instance()->set(meshResource.key(), mesh, ResourceDataState::Final, ResourcePolicy::Manual);

    /* Index buffer, if needed, if not, resource key doesn't have to be set */
    if(data.indices()) {
        CORRADE_INTERNAL_ASSERT(indexBufferResource.key() != ResourceKey());
        Buffer* indexBuffer = new Buffer(Buffer::Target::ElementArray);
        MeshTools::compressIndices(mesh, indexBuffer, Buffer::Usage::StaticDraw, *data.indices());
        ResourceManager::instance()->set(indexBufferResource.key(), indexBuffer, ResourceDataState::Final, ResourcePolicy::Manual);
    }
}

template<> void create<3>(Trade::MeshData3D& data, Resource<Mesh>& meshResource, Resource<Buffer>& vertexBufferResource, Resource<Buffer>& indexBufferResource) {
    /* Vertex buffer */
    Buffer* vertexBuffer = new Buffer(Buffer::Target::Array);
    vertexBuffer->setData(*data.positions(0), Buffer::Usage::StaticDraw);
    ResourceManager::instance()->set(vertexBufferResource.key(), vertexBuffer, ResourceDataState::Final, ResourcePolicy::Manual);

    /* Mesh configuration */
    Mesh* mesh = new Mesh;
    mesh->setPrimitive(data.primitive())
        ->setVertexCount(data.positions(0)->size())
        ->addVertexBuffer(vertexBuffer, 0, Shaders::FlatShader3D::Position());
    ResourceManager::instance()->set(meshResource.key(), mesh, ResourceDataState::Final, ResourcePolicy::Manual);

    /* Index buffer, if needed, if not, resource key doesn't have to be set */
    if(data.indices()) {
        CORRADE_INTERNAL_ASSERT(indexBufferResource.key() != ResourceKey());
        Buffer* indexBuffer = new Buffer(Buffer::Target::ElementArray);
        MeshTools::compressIndices(mesh, indexBuffer, Buffer::Usage::StaticDraw, *data.indices());
        ResourceManager::instance()->set(indexBufferResource.key(), indexBuffer, ResourceDataState::Final, ResourcePolicy::Manual);
    }
}

}

template<UnsignedInt dimensions> AbstractShapeRenderer<dimensions>::AbstractShapeRenderer(ResourceKey meshKey, ResourceKey vertexBufferKey, ResourceKey indexBufferKey) {
    shader = ResourceManager::instance()->get<AbstractShaderProgram, Shaders::FlatShader<dimensions>>(shaderKey<dimensions>());
    mesh = ResourceManager::instance()->get<Mesh>(meshKey);
    vertexBuffer = ResourceManager::instance()->get<Buffer>(vertexBufferKey);
    indexBuffer = ResourceManager::instance()->get<Buffer>(indexBufferKey);

    if(!shader) ResourceManager::instance()->set<AbstractShaderProgram>(shaderKey<dimensions>(),
        new Shaders::FlatShader<dimensions>, ResourceDataState::Final, ResourcePolicy::Resident);
}

template<UnsignedInt dimensions> AbstractShapeRenderer<dimensions>::~AbstractShapeRenderer() {}

template<UnsignedInt dimensions> void AbstractShapeRenderer<dimensions>::createResources(typename MeshData<dimensions>::Type data) {
    create<dimensions>(data, this->mesh, this->vertexBuffer, this->indexBuffer);
}

template class AbstractShapeRenderer<2>;
template class AbstractShapeRenderer<3>;

}}}
