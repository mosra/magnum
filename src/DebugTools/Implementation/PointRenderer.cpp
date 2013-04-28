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

#include "PointRenderer.h"

#include "Mesh.h"
#include "DebugTools/ShapeRenderer.h"
#include "Physics/Point.h"
#include "Primitives/Crosshair.h"
#include "Shaders/Flat.h"
#include "Trade/MeshData2D.h"
#include "Trade/MeshData3D.h"

namespace Magnum { namespace DebugTools { namespace Implementation {

namespace {
    template<UnsignedInt dimensions> ResourceKey meshKey();
    template<> inline ResourceKey meshKey<2>() { return ResourceKey("point2d"); }
    template<> inline ResourceKey meshKey<3>() { return ResourceKey("point3d"); }

    template<UnsignedInt dimensions> ResourceKey vertexBufferKey();
    template<> inline ResourceKey vertexBufferKey<2>() { return ResourceKey("point2d-vertices"); }
    template<> inline ResourceKey vertexBufferKey<3>() { return ResourceKey("point3d-vertices"); }

    template<UnsignedInt dimensions> typename MeshData<dimensions>::Type meshData();
    template<> inline Trade::MeshData2D meshData<2>() { return Primitives::Crosshair2D::wireframe(); }
    template<> inline Trade::MeshData3D meshData<3>() { return Primitives::Crosshair3D::wireframe(); }
}

template<UnsignedInt dimensions> PointRenderer<dimensions>::PointRenderer(Physics::Point<dimensions>& point): AbstractShapeRenderer<dimensions>(meshKey<dimensions>(), vertexBufferKey<dimensions>(), {}), point(point) {
    if(!this->wireframeMesh) this->createResources(meshData<dimensions>());
}

template<UnsignedInt dimensions> void PointRenderer<dimensions>::draw(Resource<ShapeRendererOptions>& options, const typename DimensionTraits<dimensions>::MatrixType& projectionMatrix) {
    /* Half scale, because the point is 2x2(x2) */
    this->wireframeShader->setTransformationProjectionMatrix(projectionMatrix*
        DimensionTraits<dimensions>::MatrixType::translation(point.transformedPosition())*
        DimensionTraits<dimensions>::MatrixType::scaling(typename DimensionTraits<dimensions>::VectorType(options->pointSize()/2)))
        ->setColor(options->color())
        ->use();
    this->wireframeMesh->draw();
}

template class PointRenderer<2>;
template class PointRenderer<3>;

}}}
