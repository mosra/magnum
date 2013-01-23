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

#include "ObjectRenderer.h"

#include "Buffer.h"
#include "DebugTools/ResourceManager.h"
#include "MeshTools/Interleave.h"
#include "SceneGraph/AbstractCamera.h"
#include "Shaders/VertexColorShader.h"

namespace Magnum { namespace DebugTools {

namespace {

template<std::uint8_t> struct Renderer;

template<> struct Renderer<2> {
    inline static ResourceKey shader() { return {"VertexColorShader2D"}; }
    inline static ResourceKey vertexBuffer() { return {"object2d-vertices"}; }
    inline static ResourceKey indexBuffer() { return {"object2d-indices"}; }
    inline static ResourceKey mesh() { return {"object2d"}; }

    static const std::array<Point2D, 8> positions;
    static const std::array<Color3<>, 8> colors;
    static const std::array<std::uint8_t, 12> indices;
};

const std::array<Point2D, 8> Renderer<2>::positions{{
    { 0.0f,  0.0f},
    { 1.0f,  0.0f}, /* X axis */
    { 0.9f,  0.1f},
    { 0.9f, -0.1f},

    { 0.0f,  0.0f},
    { 0.0f,  1.0f}, /* Y axis */
    { 0.1f,  0.9f},
    {-0.1f,  0.9f}
}};

const std::array<Color3<>, 8> Renderer<2>::colors{{
    {1.0f, 0.0f, 0.0f},
    {1.0f, 0.0f, 0.0f}, /* X axis */
    {1.0f, 0.0f, 0.0f},
    {1.0f, 0.0f, 0.0f},

    {0.0f, 1.0f, 0.0f},
    {0.0f, 1.0f, 0.0f}, /* Y axis */
    {0.0f, 1.0f, 0.0f},
    {0.0f, 1.0f, 0.0f},
}};

const std::array<std::uint8_t, 12> Renderer<2>::indices{{
    0, 1,
    1, 2, /* X axis */
    1, 3,

    4, 5,
    5, 6, /* Y axis */
    5, 7
}};

template<> struct Renderer<3> {
    inline static ResourceKey shader() { return {"VertexColorShader3D"}; }
    inline static ResourceKey vertexBuffer() { return {"object3d-vertices"}; }
    inline static ResourceKey indexBuffer() { return {"object3d-indices"}; }
    inline static ResourceKey mesh() { return {"object3d"}; }

    static const std::array<Point3D, 12> positions;
    static const std::array<Color3<>, 12> colors;
    static const std::array<uint8_t, 18> indices;
};

const std::array<Point3D, 12> Renderer<3>::positions{{
    { 0.0f,  0.0f,  0.0f},
    { 1.0f,  0.0f,  0.0f}, /* X axis */
    { 0.9f,  0.1f,  0.0f},
    { 0.9f, -0.1f,  0.0f},

    { 0.0f,  0.0f,  0.0f},
    { 0.0f,  1.0f,  0.0f}, /* Y axis */
    { 0.1f,  0.9f,  0.0f},
    {-0.1f,  0.9f,  0.0f},

    { 0.0f,  0.0f,  0.0f},
    { 0.0f,  0.0f,  1.0f}, /* Z axis */
    { 0.1f,  0.0f,  0.9f},
    {-0.1f,  0.0f,  0.9f}
}};

const std::array<Color3<>, 12> Renderer<3>::colors{{
    {1.0f, 0.0f, 0.0f},
    {1.0f, 0.0f, 0.0f}, /* X axis */
    {1.0f, 0.0f, 0.0f},
    {1.0f, 0.0f, 0.0f},

    {0.0f, 1.0f, 0.0f},
    {0.0f, 1.0f, 0.0f}, /* Y axis */
    {0.0f, 1.0f, 0.0f},
    {0.0f, 1.0f, 0.0f},

    {0.0f, 0.0f, 1.0f},
    {0.0f, 0.0f, 1.0f}, /* Z axis */
    {0.0f, 0.0f, 1.0f},
    {0.0f, 0.0f, 1.0f}
}};

const std::array<std::uint8_t, 18> Renderer<3>::indices{{
    0, 1,
    1, 2,  /* X axis */
    1, 3,

    4, 5,
    5, 6,  /* Y axis */
    5, 7,

    8, 9,
    9, 10, /* Z axis */
    9, 11
}};

}

template<std::uint8_t dimensions> ObjectRenderer<dimensions>::ObjectRenderer(SceneGraph::AbstractObject<dimensions>* object, ResourceKey options, SceneGraph::DrawableGroup<dimensions>* drawables): SceneGraph::Drawable<dimensions>(object, drawables), options(ResourceManager::instance()->get<ObjectRendererOptions>(options)) {
    /* Shader */
    shader = ResourceManager::instance()->get<AbstractShaderProgram, Shaders::VertexColorShader<dimensions>>(Renderer<dimensions>::shader());
    if(!shader) ResourceManager::instance()->set<AbstractShaderProgram>(shader.key(),
        new Shaders::VertexColorShader<dimensions>, ResourceDataState::Final, ResourcePolicy::Resident);

    /* Mesh and vertex buffer */
    mesh = ResourceManager::instance()->get<Mesh>(Renderer<dimensions>::mesh());
    vertexBuffer = ResourceManager::instance()->get<Buffer>(Renderer<dimensions>::vertexBuffer());
    indexBuffer = ResourceManager::instance()->get<Buffer>(Renderer<dimensions>::indexBuffer());
    if(mesh) return;

    /* Create the mesh */
    Buffer* vertexBuffer = new Buffer(Buffer::Target::Array);
    Buffer* indexBuffer = new Buffer(Buffer::Target::ElementArray);
    Mesh* mesh = new Mesh;

    MeshTools::interleave(mesh, vertexBuffer, Buffer::Usage::StaticDraw, Renderer<dimensions>::positions, Renderer<dimensions>::colors);
    ResourceManager::instance()->set(this->vertexBuffer.key(), vertexBuffer, ResourceDataState::Final, ResourcePolicy::Manual);

    indexBuffer->setData(Renderer<dimensions>::indices, Buffer::Usage::StaticDraw);
    ResourceManager::instance()->set(this->indexBuffer.key(), indexBuffer, ResourceDataState::Final, ResourcePolicy::Manual);

    mesh->setPrimitive(Mesh::Primitive::Lines)
        ->setIndexCount(Renderer<dimensions>::indices.size())
        ->addInterleavedVertexBuffer(vertexBuffer, 0,
            typename Shaders::VertexColorShader<dimensions>::Position(),
            typename Shaders::VertexColorShader<dimensions>::Color())
        ->setIndexBuffer(indexBuffer, 0, Mesh::IndexType::UnsignedByte, 0, Renderer<dimensions>::positions.size());
    ResourceManager::instance()->set<Mesh>(this->mesh.key(), mesh, ResourceDataState::Final, ResourcePolicy::Manual);
}

template<std::uint8_t dimensions> void ObjectRenderer<dimensions>::draw(const typename DimensionTraits<dimensions>::MatrixType& transformationMatrix, SceneGraph::AbstractCamera<dimensions>* camera) {
    shader->setTransformationProjectionMatrix(camera->projectionMatrix()*transformationMatrix*DimensionTraits<dimensions>::MatrixType::scaling(typename DimensionTraits<dimensions>::VectorType(options->size())))
        ->use();

    mesh->draw();
}

template class ObjectRenderer<2>;
template class ObjectRenderer<3>;

}}
