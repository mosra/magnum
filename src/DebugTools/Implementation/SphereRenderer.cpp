/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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

#include "SphereRenderer.h"

#include "Mesh.h"
#include "DebugTools/ShapeRenderer.h"
#include "Physics/Sphere.h"
#include "Primitives/Circle.h"
#include "Shaders/FlatShader.h"
#include "Trade/MeshData2D.h"

namespace Magnum { namespace DebugTools { namespace Implementation {

AbstractSphereRenderer<2>::AbstractSphereRenderer(): AbstractShapeRenderer<2>("sphere2d", "sphere2d-vertices", {}) {
    if(!wireframeMesh) createResources(Primitives::Circle::wireframe(40));
}

template<UnsignedInt dimensions> SphereRenderer<dimensions>::SphereRenderer(Physics::Sphere<dimensions>& sphere): sphere(sphere) {}

template<UnsignedInt dimensions> void SphereRenderer<dimensions>::draw(Resource<ShapeRendererOptions>& options, const typename DimensionTraits<dimensions>::MatrixType& projectionMatrix) {
    this->wireframeShader->setTransformationProjectionMatrix(projectionMatrix*
        DimensionTraits<dimensions>::MatrixType::translation(sphere.transformedPosition())*
        DimensionTraits<dimensions>::MatrixType::scaling(typename DimensionTraits<dimensions>::VectorType(sphere.transformedRadius())))
        ->setColor(options->color())
        ->use();
    this->wireframeMesh->draw();
}

template class SphereRenderer<2>;

}}}
