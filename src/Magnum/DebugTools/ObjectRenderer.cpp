/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014
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

#include "ObjectRenderer.h"

#include "Magnum/Buffer.h"
#include "Magnum/DebugTools/ResourceManager.h"
#include "Magnum/MeshTools/Interleave.h"
#include "Magnum/SceneGraph/AbstractCamera.h"
#include "Magnum/Shaders/VertexColor.h"

namespace Magnum { namespace DebugTools {

namespace {

template<UnsignedInt> struct Renderer;

template<> struct Renderer<2> {
    static ResourceKey shader() { return {"VertexColorShader2D"}; }
    static ResourceKey vertexBuffer() { return {"object2d-vertices"}; }
    static ResourceKey indexBuffer() { return {"object2d-indices"}; }
    static ResourceKey mesh() { return {"object2d"}; }

    static const std::array<Vector2, 8> positions;
    static const std::array<Color3, 8> colors;
    static const std::array<UnsignedByte, 12> indices;
};

const std::array<Vector2, 8> Renderer<2>::positions{{
    { 0.0f,  0.0f},
    { 1.0f,  0.0f}, /* X axis */
    { 0.9f,  0.1f},
    { 0.9f, -0.1f},

    { 0.0f,  0.0f},
    { 0.0f,  1.0f}, /* Y axis */
    { 0.1f,  0.9f},
    {-0.1f,  0.9f}
}};

const std::array<Color3, 8> Renderer<2>::colors{{
    {1.0f, 0.0f, 0.0f},
    {1.0f, 0.0f, 0.0f}, /* X axis */
    {1.0f, 0.0f, 0.0f},
    {1.0f, 0.0f, 0.0f},

    {0.0f, 1.0f, 0.0f},
    {0.0f, 1.0f, 0.0f}, /* Y axis */
    {0.0f, 1.0f, 0.0f},
    {0.0f, 1.0f, 0.0f},
}};

const std::array<UnsignedByte, 12> Renderer<2>::indices{{
    0, 1,
    1, 2, /* X axis */
    1, 3,

    4, 5,
    5, 6, /* Y axis */
    5, 7
}};

template<> struct Renderer<3> {
    static ResourceKey shader() { return {"VertexColorShader3D"}; }
    static ResourceKey vertexBuffer() { return {"object3d-vertices"}; }
    static ResourceKey indexBuffer() { return {"object3d-indices"}; }
    static ResourceKey mesh() { return {"object3d"}; }

    static const std::array<Vector3, 12> positions;
    static const std::array<Color3, 12> colors;
    static const std::array<uint8_t, 18> indices;
};

const std::array<Vector3, 12> Renderer<3>::positions{{
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

const std::array<Color3, 12> Renderer<3>::colors{{
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

const std::array<UnsignedByte, 18> Renderer<3>::indices{{
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

template<UnsignedInt dimensions> ObjectRenderer<dimensions>::ObjectRenderer(SceneGraph::AbstractObject<dimensions, Float>& object, ResourceKey options, SceneGraph::DrawableGroup<dimensions, Float>* drawables): SceneGraph::Drawable<dimensions, Float>(object, drawables), options(ResourceManager::instance().get<ObjectRendererOptions>(options)) {
    /* Shader */
    shader = ResourceManager::instance().get<AbstractShaderProgram, Shaders::VertexColor<dimensions>>(Renderer<dimensions>::shader());
    if(!shader) ResourceManager::instance().set<AbstractShaderProgram>(shader.key(), new Shaders::VertexColor<dimensions>);

    /* Mesh and vertex buffer */
    mesh = ResourceManager::instance().get<Mesh>(Renderer<dimensions>::mesh());
    vertexBuffer = ResourceManager::instance().get<Buffer>(Renderer<dimensions>::vertexBuffer());
    indexBuffer = ResourceManager::instance().get<Buffer>(Renderer<dimensions>::indexBuffer());
    if(mesh) return;

    /* Create the mesh */
    Buffer* vertexBuffer = new Buffer(Buffer::Target::Array);
    Buffer* indexBuffer = new Buffer(Buffer::Target::ElementArray);
    Mesh* mesh = new Mesh;

    MeshTools::interleave(*mesh, *vertexBuffer, BufferUsage::StaticDraw, Renderer<dimensions>::positions, Renderer<dimensions>::colors);
    ResourceManager::instance().set(this->vertexBuffer.key(), vertexBuffer, ResourceDataState::Final, ResourcePolicy::Manual);

    indexBuffer->setData(Renderer<dimensions>::indices, BufferUsage::StaticDraw);
    ResourceManager::instance().set(this->indexBuffer.key(), indexBuffer, ResourceDataState::Final, ResourcePolicy::Manual);

    mesh->setPrimitive(MeshPrimitive::Lines)
        .setCount(Renderer<dimensions>::indices.size())
        .addVertexBuffer(*vertexBuffer, 0,
            typename Shaders::VertexColor<dimensions>::Position(),
            typename Shaders::VertexColor<dimensions>::Color())
        .setIndexBuffer(*indexBuffer, 0, Mesh::IndexType::UnsignedByte, 0, Renderer<dimensions>::positions.size());
    ResourceManager::instance().set<Mesh>(this->mesh.key(), mesh, ResourceDataState::Final, ResourcePolicy::Manual);
}

template<UnsignedInt dimensions> void ObjectRenderer<dimensions>::draw(const typename DimensionTraits<dimensions, Float>::MatrixType& transformationMatrix, SceneGraph::AbstractCamera<dimensions, Float>& camera) {
    shader->setTransformationProjectionMatrix(camera.projectionMatrix()*transformationMatrix*DimensionTraits<dimensions, Float>::MatrixType::scaling(typename DimensionTraits<dimensions, Float>::VectorType(options->size())));
    mesh->draw(*shader);
}

template class ObjectRenderer<2>;
template class ObjectRenderer<3>;

}}
