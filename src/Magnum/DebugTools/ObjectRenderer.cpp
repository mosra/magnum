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

#include "ObjectRenderer.h"

#include "Magnum/DebugTools/ResourceManager.h"
#include "Magnum/GL/Mesh.h"
#include "Magnum/MeshTools/Compile.h"
#include "Magnum/Primitives/Axis.h"
#include "Magnum/SceneGraph/Camera.h"
#include "Magnum/Shaders/VertexColor.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace DebugTools {

namespace {

template<UnsignedInt> struct Renderer;

template<> struct Renderer<2> {
    static ResourceKey shader() { return {"VertexColorShader2D"}; }
    static ResourceKey mesh() { return {"object2d"}; }
    static Trade::MeshData meshData() { return Primitives::axis2D(); }
};

template<> struct Renderer<3> {
    static ResourceKey shader() { return {"VertexColorShader3D"}; }
    static ResourceKey mesh() { return {"object3d"}; }
    static Trade::MeshData meshData() { return Primitives::axis3D(); }
};

}

/* Doxygen gets confused when using {} to initialize parent object */
template<UnsignedInt dimensions> ObjectRenderer<dimensions>::ObjectRenderer(ResourceManager& manager, SceneGraph::AbstractObject<dimensions, Float>& object, ResourceKey options, SceneGraph::DrawableGroup<dimensions, Float>* drawables): SceneGraph::Drawable<dimensions, Float>(object, drawables), _options{manager.get<ObjectRendererOptions>(options)} {
    /* Shader */
    _shader = manager.get<GL::AbstractShaderProgram, Shaders::VertexColor<dimensions>>(Renderer<dimensions>::shader());
    if(!_shader) manager.set<GL::AbstractShaderProgram>(_shader.key(), new Shaders::VertexColor<dimensions>);

    /* Mesh */
    _mesh = manager.get<GL::Mesh>(Renderer<dimensions>::mesh());
    if(!_mesh) manager.set<GL::Mesh>(_mesh.key(), MeshTools::compile(Renderer<dimensions>::meshData()));
}

/* To avoid deleting pointers to incomplete type on destruction of Resource members */
template<UnsignedInt dimensions> ObjectRenderer<dimensions>::~ObjectRenderer() = default;

template<UnsignedInt dimensions> void ObjectRenderer<dimensions>::draw(const MatrixTypeFor<dimensions, Float>& transformationMatrix, SceneGraph::Camera<dimensions, Float>& camera) {
    _shader->setTransformationProjectionMatrix(camera.projectionMatrix()*transformationMatrix*MatrixTypeFor<dimensions, Float>::scaling(VectorTypeFor<dimensions, Float>{_options->size()}))
        .draw(*_mesh);
}

template class MAGNUM_DEBUGTOOLS_EXPORT ObjectRenderer<2>;
template class MAGNUM_DEBUGTOOLS_EXPORT ObjectRenderer<3>;

}}
