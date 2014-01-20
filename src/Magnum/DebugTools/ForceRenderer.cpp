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

#include "ForceRenderer.h"

#include "Magnum/Buffer.h"
#include "Magnum/Mesh.h"
#include "Magnum/DebugTools/ResourceManager.h"
#include "Magnum/SceneGraph/AbstractCamera.h"
#include "Magnum/Shaders/Flat.h"

#include "Implementation/ForceRendererTransformation.h"

namespace Magnum { namespace DebugTools {

namespace {

template<UnsignedInt dimensions> ResourceKey shaderKey();
template<> inline ResourceKey shaderKey<2>() { return ResourceKey("FlatShader2D"); }
template<> inline ResourceKey shaderKey<3>() { return ResourceKey("FlatShader3D"); }

/** @bug Why this is not constexpr under GCC 4.6? */
#ifndef CORRADE_GCC46_COMPATIBILITY
constexpr std::array<Vector2, 4> positions{{
#else
const std::array<Vector2, 4> positions{{
#endif
    {0.0f,  0.0f},
    {1.0f,  0.0f},
    {0.9f,  0.1f},
    {0.9f, -0.1f}
}};

#ifndef CORRADE_GCC46_COMPATIBILITY
constexpr std::array<UnsignedByte, 6> indices{{
#else
const std::array<UnsignedByte, 6> indices{{
#endif
    0, 1,
    1, 2,
    1, 3
}};

}

template<UnsignedInt dimensions> ForceRenderer<dimensions>::ForceRenderer(SceneGraph::AbstractObject<dimensions, Float>& object, const typename DimensionTraits<dimensions, Float>::VectorType& forcePosition, const typename DimensionTraits<dimensions, Float>::VectorType& force, ResourceKey options, SceneGraph::DrawableGroup<dimensions, Float>* drawables): SceneGraph::Drawable<dimensions, Float>(object, drawables), forcePosition(forcePosition), force(force), options(ResourceManager::instance().get<ForceRendererOptions>(options)) {
    /* Shader */
    shader = ResourceManager::instance().get<AbstractShaderProgram, Shaders::Flat<dimensions>>(shaderKey<dimensions>());
    if(!shader) ResourceManager::instance().set<AbstractShaderProgram>(shader.key(), new Shaders::Flat<dimensions>);

    /* Mesh and vertex buffer */
    mesh = ResourceManager::instance().get<Mesh>("force");
    vertexBuffer = ResourceManager::instance().get<Buffer>("force-vertices");
    indexBuffer = ResourceManager::instance().get<Buffer>("force-indices");
    if(mesh) return;

    /* Create the mesh */
    Buffer* vertexBuffer = new Buffer(Buffer::Target::Array);
    Buffer* indexBuffer = new Buffer(Buffer::Target::ElementArray);

    vertexBuffer->setData(positions, BufferUsage::StaticDraw);
    ResourceManager::instance().set(this->vertexBuffer.key(), vertexBuffer, ResourceDataState::Final, ResourcePolicy::Manual);

    indexBuffer->setData(indices, BufferUsage::StaticDraw);
    ResourceManager::instance().set(this->indexBuffer.key(), indexBuffer, ResourceDataState::Final, ResourcePolicy::Manual);

    Mesh* mesh = new Mesh;
    mesh->setPrimitive(MeshPrimitive::Lines)
        .setIndexCount(indices.size())
        .addVertexBuffer(*vertexBuffer, 0,
            typename Shaders::Flat<dimensions>::Position(Shaders::Flat<dimensions>::Position::Components::Two))
        .setIndexBuffer(*indexBuffer, 0, Mesh::IndexType::UnsignedByte, 0, positions.size());
    ResourceManager::instance().set(this->mesh.key(), mesh, ResourceDataState::Final, ResourcePolicy::Manual);
}

template<UnsignedInt dimensions> void ForceRenderer<dimensions>::draw(const typename DimensionTraits<dimensions, Float>::MatrixType& transformationMatrix, SceneGraph::AbstractCamera<dimensions, Float>& camera) {
    shader->setTransformationProjectionMatrix(camera.projectionMatrix()*Implementation::forceRendererTransformation<dimensions>(transformationMatrix.transformPoint(forcePosition), force)*DimensionTraits<dimensions, Float>::MatrixType::scaling(typename DimensionTraits<dimensions, Float>::VectorType(options->scale())))
        .setColor(options->color())
        .use();
    mesh->draw();
}

template class ForceRenderer<2>;
template class ForceRenderer<3>;

}}
