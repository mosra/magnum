/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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

#include "Magnum/GL/Mesh.h"
#include "Magnum/DebugTools/ResourceManager.h"
#include "Magnum/SceneGraph/Camera.h"
#include "Magnum/Shaders/Flat.h"

#include "Magnum/DebugTools/Implementation/ForceRendererTransformation.h"

namespace Magnum { namespace DebugTools {

namespace {

template<UnsignedInt dimensions> ResourceKey shaderKey();
template<> inline ResourceKey shaderKey<2>() { return ResourceKey("FlatShader2D"); }
template<> inline ResourceKey shaderKey<3>() { return ResourceKey("FlatShader3D"); }

constexpr Vector2 positions[]{
    {0.0f,  0.0f},
    {1.0f,  0.0f},
    {0.9f,  0.1f},
    {0.9f, -0.1f}
};

constexpr UnsignedByte indices[]{
    0, 1,
    1, 2,
    1, 3
};

}

template<UnsignedInt dimensions> ForceRenderer<dimensions>::ForceRenderer(ResourceManager& manager, SceneGraph::AbstractObject<dimensions, Float>& object, const VectorTypeFor<dimensions, Float>& forcePosition, const VectorTypeFor<dimensions, Float>& force, ResourceKey options, SceneGraph::DrawableGroup<dimensions, Float>* drawables): SceneGraph::Drawable<dimensions, Float>(object, drawables), _forcePosition(forcePosition), _force(force), _options(manager.get<ForceRendererOptions>(options)) {
    /* Shader */
    _shader = manager.get<GL::AbstractShaderProgram, Shaders::Flat<dimensions>>(shaderKey<dimensions>());
    if(!_shader) manager.set<GL::AbstractShaderProgram>(_shader.key(), new Shaders::Flat<dimensions>);

    /* Mesh and vertex buffer */
    _mesh = manager.get<GL::Mesh>("force");
    if(_mesh) return;

    /* Create the mesh */
    GL::Buffer vertexBuffer{GL::Buffer::TargetHint::Array};
    vertexBuffer.setData(positions, GL::BufferUsage::StaticDraw);
    GL::Buffer indexBuffer{GL::Buffer::TargetHint::ElementArray};
    indexBuffer.setData(indices, GL::BufferUsage::StaticDraw);
    GL::Mesh mesh{GL::MeshPrimitive::Lines};
    mesh.setCount(Containers::arraySize(indices))
        .addVertexBuffer(std::move(vertexBuffer), 0,
            typename Shaders::Flat<dimensions>::Position(Shaders::Flat<dimensions>::Position::Components::Two))
        .setIndexBuffer(std::move(indexBuffer), 0, GL::MeshIndexType::UnsignedByte, 0, Containers::arraySize(positions));
    manager.set(_mesh.key(), std::move(mesh), ResourceDataState::Final, ResourcePolicy::Manual);
}

/* To avoid deleting pointers to incomplete type on destruction of Resource members */
template<UnsignedInt dimensions> ForceRenderer<dimensions>::~ForceRenderer() = default;

template<UnsignedInt dimensions> void ForceRenderer<dimensions>::draw(const MatrixTypeFor<dimensions, Float>& transformationMatrix, SceneGraph::Camera<dimensions, Float>& camera) {
    _shader->setTransformationProjectionMatrix(camera.projectionMatrix()*Implementation::forceRendererTransformation<dimensions>(transformationMatrix.transformPoint(_forcePosition), _force)*MatrixTypeFor<dimensions, Float>::scaling(VectorTypeFor<dimensions, Float>{_options->size()}))
        .setColor(_options->color())
        .draw(*_mesh);
}

template class MAGNUM_DEBUGTOOLS_EXPORT ForceRenderer<2>;
template class MAGNUM_DEBUGTOOLS_EXPORT ForceRenderer<3>;

}}
