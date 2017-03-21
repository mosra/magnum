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

#include "CapsuleRenderer.h"

#include "Magnum/Mesh.h"
#include "Magnum/MeshView.h"
#include "Magnum/DebugTools/ResourceManager.h"
#include "Magnum/DebugTools/ShapeRenderer.h"
#include "Magnum/Primitives/Capsule.h"
#include "Magnum/Shapes/Capsule.h"
#include "Magnum/Shaders/Flat.h"
#include "Magnum/Trade/MeshData2D.h"
#include "Magnum/Trade/MeshData3D.h"

#include "CapsuleRendererTransformation.h"

namespace Magnum { namespace DebugTools { namespace Implementation {

AbstractCapsuleRenderer<2>::AbstractCapsuleRenderer(): AbstractShapeRenderer<2>("capsule2d", "capsule2d-vertices", "capsule2d-indices") {
    constexpr UnsignedInt rings = 10;
    if(!wireframeMesh) createResources(Primitives::Capsule2D::wireframe(rings, 1, 1.0f));

    /* Bottom hemisphere */
    if(!(bottom = ResourceManager::instance().get<MeshView>("capsule2d-bottom"))) {
        auto view = new MeshView(*wireframeMesh);
        view->setCount(rings*4)
            .setIndexRange(0, 0, rings*2+1);
        ResourceManager::instance().set(bottom.key(), view, ResourceDataState::Final, ResourcePolicy::Manual);
    }

    /* Cylinder */
    if(!(cylinder = ResourceManager::instance().get<MeshView>("capsule2d-cylinder"))) {
        auto view = new MeshView(*wireframeMesh);
        view->setCount(4)
            .setIndexRange(rings*4, rings*2+1, rings*2+3);
        ResourceManager::instance().set(cylinder.key(), view, ResourceDataState::Final, ResourcePolicy::Manual);
    }

    /* Top hemisphere */
    if(!(top = ResourceManager::instance().get<MeshView>("capsule2d-top"))) {
        auto view = new MeshView(*wireframeMesh);
        view->setCount(rings*4)
            .setIndexRange(rings*4+4, rings*2+3, rings*4+4);
        ResourceManager::instance().set(top.key(), view, ResourceDataState::Final, ResourcePolicy::Manual);
    }
}

AbstractCapsuleRenderer<3>::AbstractCapsuleRenderer(): AbstractShapeRenderer<3>("capsule3d", "capsule3d-vertices", "capsule3d-indices") {
    constexpr UnsignedInt rings = 10;
    constexpr UnsignedInt segments = 40;
    if(!wireframeMesh) createResources(Primitives::Capsule3D::wireframe(rings, 1, segments, 1.0f));

    /* Bottom hemisphere */
    if(!(bottom = ResourceManager::instance().get<MeshView>("capsule3d-bottom"))) {
        auto view = new MeshView(*wireframeMesh);
        view->setCount(rings*8)
            .setIndexRange(0, 0, rings*4+1);
        ResourceManager::instance().set(bottom.key(), view, ResourceDataState::Final, ResourcePolicy::Manual);
    }

    /* Cylinder */
    if(!(cylinder = ResourceManager::instance().get<MeshView>("capsule3d-cylinder"))) {
        auto view = new MeshView(*wireframeMesh);
        view->setCount(segments*4+8)
            .setIndexRange(rings*8, rings*4+1, rings*4+segments*2+5);
        ResourceManager::instance().set(cylinder.key(), view, ResourceDataState::Final, ResourcePolicy::Manual);
    }

    /* Top */
    if(!(top = ResourceManager::instance().get<MeshView>("capsule3d-top"))) {
        auto view = new MeshView(*wireframeMesh);
        view->setCount(rings*8)
            .setIndexRange(rings*8+segments*4+8, rings*4+segments*2+5, rings*8+segments*2+6);
        ResourceManager::instance().set(top.key(), view, ResourceDataState::Final, ResourcePolicy::Manual);
    }
}

AbstractCapsuleRenderer<2>::~AbstractCapsuleRenderer() = default;

AbstractCapsuleRenderer<3>::~AbstractCapsuleRenderer() = default;

template<UnsignedInt dimensions> CapsuleRenderer<dimensions>::CapsuleRenderer(const Shapes::Implementation::AbstractShape<dimensions>& capsule): capsule(static_cast<const Shapes::Implementation::Shape<Shapes::Capsule<dimensions>>&>(capsule).shape) {}

template<UnsignedInt dimensions> void CapsuleRenderer<dimensions>::draw(Resource<ShapeRendererOptions>& options, const MatrixTypeFor<dimensions, Float>& projectionMatrix) {
    std::array<MatrixTypeFor<dimensions, Float>, 3> transformations = Implementation::capsuleRendererTransformation<dimensions>(capsule.a(), capsule.b(), capsule.radius());
    AbstractShapeRenderer<dimensions>::wireframeShader->setColor(options->color());

    /* Bottom */
    AbstractShapeRenderer<dimensions>::wireframeShader->setTransformationProjectionMatrix(projectionMatrix*transformations[0]);
    AbstractCapsuleRenderer<dimensions>::bottom->draw(*AbstractShapeRenderer<dimensions>::wireframeShader);

    /* Cylinder */
    AbstractShapeRenderer<dimensions>::wireframeShader->setTransformationProjectionMatrix(projectionMatrix*transformations[1]);
    AbstractCapsuleRenderer<dimensions>::cylinder->draw(*AbstractShapeRenderer<dimensions>::wireframeShader);

    /* Top */
    AbstractShapeRenderer<dimensions>::wireframeShader->setTransformationProjectionMatrix(projectionMatrix*transformations[2]);
    AbstractCapsuleRenderer<dimensions>::top->draw(*AbstractShapeRenderer<dimensions>::wireframeShader);
}

template class CapsuleRenderer<2>;
template class CapsuleRenderer<3>;

}}}
