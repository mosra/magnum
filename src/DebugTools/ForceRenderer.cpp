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

#include "ForceRenderer.h"

#include "Buffer.h"
#include "Mesh.h"
#include "DebugTools/ResourceManager.h"
#include "SceneGraph/AbstractCamera.h"
#include "Shaders/FlatShader.h"

#include "DebugTools/Implementation/ForceRendererTransformation.h"

namespace Magnum { namespace DebugTools {

namespace {

template<UnsignedInt dimensions> ResourceKey shaderKey();
template<> inline ResourceKey shaderKey<2>() { return ResourceKey("FlatShader2D"); }
template<> inline ResourceKey shaderKey<3>() { return ResourceKey("FlatShader3D"); }

constexpr std::array<Vector2, 4> positions{{
    {0.0f,  0.0f},
    {1.0f,  0.0f},
    {0.9f,  0.1f},
    {0.9f, -0.1f}
}};

constexpr std::array<UnsignedByte, 6> indices{{
    0, 1,
    1, 2,
    1, 3
}};

}

template<UnsignedInt dimensions> ForceRenderer<dimensions>::ForceRenderer(SceneGraph::AbstractObject<dimensions>* object, const typename DimensionTraits<dimensions, Float>::VectorType& forcePosition, const typename DimensionTraits<dimensions, Float>::VectorType* force, ResourceKey options, SceneGraph::DrawableGroup<dimensions>* drawables): SceneGraph::Drawable<dimensions>(object, drawables), forcePosition(forcePosition), force(force), options(ResourceManager::instance()->get<ForceRendererOptions>(options)) {
    /* Shader */
    shader = ResourceManager::instance()->get<AbstractShaderProgram, Shaders::FlatShader<dimensions>>(shaderKey<dimensions>());
    if(!shader) ResourceManager::instance()->set<AbstractShaderProgram>(shader.key(), new Shaders::FlatShader<dimensions>);

    /* Mesh and vertex buffer */
    mesh = ResourceManager::instance()->get<Mesh>("force");
    vertexBuffer = ResourceManager::instance()->get<Buffer>("force-vertices");
    indexBuffer = ResourceManager::instance()->get<Buffer>("force-indices");
    if(mesh) return;

    /* Create the mesh */
    Buffer* vertexBuffer = new Buffer(Buffer::Target::Array);
    Buffer* indexBuffer = new Buffer(Buffer::Target::ElementArray);

    vertexBuffer->setData(positions, Buffer::Usage::StaticDraw);
    ResourceManager::instance()->set(this->vertexBuffer.key(), vertexBuffer, ResourceDataState::Final, ResourcePolicy::Manual);

    indexBuffer->setData(indices, Buffer::Usage::StaticDraw);
    ResourceManager::instance()->set(this->indexBuffer.key(), indexBuffer, ResourceDataState::Final, ResourcePolicy::Manual);

    Mesh* mesh = new Mesh;
    mesh->setPrimitive(Mesh::Primitive::Lines)
        ->setIndexCount(indices.size())
        ->addVertexBuffer(vertexBuffer, 0,
            typename Shaders::FlatShader<dimensions>::Position(Shaders::FlatShader<dimensions>::Position::Components::Two))
        ->setIndexBuffer(indexBuffer, 0, Mesh::IndexType::UnsignedByte, 0, positions.size());
    ResourceManager::instance()->set<Mesh>(this->mesh.key(), mesh, ResourceDataState::Final, ResourcePolicy::Manual);
}

template<UnsignedInt dimensions> void ForceRenderer<dimensions>::draw(const typename DimensionTraits<dimensions>::MatrixType& transformationMatrix, SceneGraph::AbstractCamera<dimensions>* camera) {
    shader->setTransformationProjectionMatrix(camera->projectionMatrix()*Implementation::forceRendererTransformation<dimensions>(transformationMatrix.translation()+forcePosition, *force)*DimensionTraits<dimensions>::MatrixType::scaling(typename DimensionTraits<dimensions>::VectorType(options->scale())))
        ->setColor(options->color())
        ->use();
    mesh->draw();
}

template class ForceRenderer<2>;
template class ForceRenderer<3>;

}}
