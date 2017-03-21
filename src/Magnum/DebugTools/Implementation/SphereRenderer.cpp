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

#include "SphereRenderer.h"

#include "Magnum/Mesh.h"
#include "Magnum/DebugTools/ShapeRenderer.h"
#include "Magnum/Shapes/Sphere.h"
#include "Magnum/Primitives/Circle.h"
#include "Magnum/Primitives/UVSphere.h"
#include "Magnum/Shaders/Flat.h"
#include "Magnum/Trade/MeshData2D.h"
#include "Magnum/Trade/MeshData3D.h"

namespace Magnum { namespace DebugTools { namespace Implementation {

AbstractSphereRenderer<2>::AbstractSphereRenderer(): AbstractShapeRenderer<2>("sphere2d", "sphere2d-vertices", {}) {
    if(!wireframeMesh) createResources(Primitives::Circle::wireframe(40));
}

AbstractSphereRenderer<3>::AbstractSphereRenderer(): AbstractShapeRenderer<3>("sphere3d", "sphere3d-vertices", "sphere3d-indices") {
    if(!wireframeMesh) createResources(Primitives::UVSphere::wireframe(20, 40));
}

template<UnsignedInt dimensions> SphereRenderer<dimensions>::SphereRenderer(const Shapes::Implementation::AbstractShape<dimensions>& sphere): sphere(static_cast<const Shapes::Implementation::Shape<Shapes::Sphere<dimensions>>&>(sphere).shape) {}

template<UnsignedInt dimensions> void SphereRenderer<dimensions>::draw(Resource<ShapeRendererOptions>& options, const MatrixTypeFor<dimensions, Float>& projectionMatrix) {
    AbstractShapeRenderer<dimensions>::wireframeShader->setTransformationProjectionMatrix(projectionMatrix*
        MatrixTypeFor<dimensions, Float>::translation(sphere.position())*
        MatrixTypeFor<dimensions, Float>::scaling(VectorTypeFor<dimensions, Float>{sphere.radius()}))
        .setColor(options->color());
    AbstractShapeRenderer<dimensions>::wireframeMesh->draw(*AbstractShapeRenderer<dimensions>::wireframeShader);
}

template class SphereRenderer<2>;
template class SphereRenderer<3>;

}}}
