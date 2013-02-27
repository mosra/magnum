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

#include "PointRenderer.h"

#include "Mesh.h"
#include "DebugTools/ShapeRenderer.h"
#include "Physics/Point.h"
#include "Primitives/Crosshair.h"
#include "Shaders/FlatShader.h"
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
    if(!this->mesh) this->createResources(meshData<dimensions>());
}

template<UnsignedInt dimensions> void PointRenderer<dimensions>::draw(Resource<ShapeRendererOptions>& options, const typename DimensionTraits<dimensions>::MatrixType& projectionMatrix) {
    /* Half scale, because the point is 2x2(x2) */
    this->shader->setTransformationProjectionMatrix(projectionMatrix*
        DimensionTraits<dimensions>::MatrixType::translation(point.transformedPosition())*
        DimensionTraits<dimensions>::MatrixType::scaling(typename DimensionTraits<dimensions>::VectorType(options->pointSize()/2)))
        ->setColor(options->color())
        ->use();
    this->mesh->draw();
}

template class PointRenderer<2>;
template class PointRenderer<3>;

}}}
