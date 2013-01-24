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

#include "AbstractBoxRenderer.h"

#include "Buffer.h"
#include "DebugTools/ResourceManager.h"
#include "MeshTools/CompressIndices.h"
#include "Primitives/Cube.h"
#include "Primitives/Square.h"
#include "Shaders/FlatShader.h"
#include "Trade/MeshData2D.h"
#include "Trade/MeshData3D.h"

namespace Magnum { namespace DebugTools { namespace Implementation {

AbstractBoxRenderer<2>::AbstractBoxRenderer() {
    /* Shader */
    shader = ResourceManager::instance()->get<AbstractShaderProgram, Shaders::FlatShader2D>("FlatShader2D");
    if(!shader) ResourceManager::instance()->set<AbstractShaderProgram>(shader.key(),
        new Shaders::FlatShader2D, ResourceDataState::Final, ResourcePolicy::Resident);

    /* Mesh and vertex buffer */
    mesh = ResourceManager::instance()->get<Mesh>("box2d");
    buffer = ResourceManager::instance()->get<Buffer>("box2d");
    if(mesh) return;

    /* Create the mesh */
    Trade::MeshData2D square = Primitives::Square::wireframe();
    Buffer* buffer = new Buffer(Buffer::Target::Array);
    Mesh* mesh = new Mesh;

    buffer->setData(*square.positions(0), Buffer::Usage::StaticDraw);
    ResourceManager::instance()->set(this->buffer.key(), buffer, ResourceDataState::Final, ResourcePolicy::Manual);

    mesh->setPrimitive(square.primitive())
        ->setVertexCount(square.positions(0)->size())
        ->addVertexBuffer(buffer, 0, Shaders::FlatShader2D::Position());
    ResourceManager::instance()->set(this->mesh.key(), mesh, ResourceDataState::Final, ResourcePolicy::Manual);
}

AbstractBoxRenderer<3>::AbstractBoxRenderer() {
    /* Shader */
    shader = ResourceManager::instance()->get<AbstractShaderProgram, Shaders::FlatShader3D>("FlatShader3D");
    if(!shader) ResourceManager::instance()->set<AbstractShaderProgram>(shader.key(),
        new Shaders::FlatShader3D, ResourceDataState::Final, ResourcePolicy::Resident);

    /* Mesh and vertex buffer */
    mesh = ResourceManager::instance()->get<Mesh>("box3d");
    vertexBuffer = ResourceManager::instance()->get<Buffer>("box3d-vertices");
    indexBuffer = ResourceManager::instance()->get<Buffer>("box3d-indices");
    if(mesh) return;

    /* Create the mesh */
    Trade::MeshData3D cube = Primitives::Cube::wireframe();
    Buffer* vertexBuffer = new Buffer(Buffer::Target::Array);
    Buffer* indexBuffer = new Buffer(Buffer::Target::ElementArray);
    Mesh* mesh = new Mesh;

    vertexBuffer->setData(*cube.positions(0), Buffer::Usage::StaticDraw);
    ResourceManager::instance()->set(this->vertexBuffer.key(), vertexBuffer, ResourceDataState::Final, ResourcePolicy::Manual);

    MeshTools::compressIndices(mesh, indexBuffer, Buffer::Usage::StaticDraw, *cube.indices());
    ResourceManager::instance()->set(this->indexBuffer.key(), indexBuffer, ResourceDataState::Final, ResourcePolicy::Manual);

    mesh->setPrimitive(cube.primitive())
        ->setVertexCount(cube.positions(0)->size())
        ->addVertexBuffer(vertexBuffer, 0, Shaders::FlatShader3D::Position());
    ResourceManager::instance()->set<Mesh>(this->mesh.key(), mesh, ResourceDataState::Final, ResourcePolicy::Manual);
}

AbstractBoxRenderer<2>::~AbstractBoxRenderer() {}
AbstractBoxRenderer<3>::~AbstractBoxRenderer() {}

}}}
