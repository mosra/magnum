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

#include "AxisAlignedBoxRenderer.h"

#include "Mesh.h"
#include "DebugTools/ShapeRenderer.h"
#include "Physics/AxisAlignedBox.h"
#include "Shaders/FlatShader.h"

namespace Magnum { namespace DebugTools { namespace Implementation {

template<std::uint8_t dimensions> AxisAlignedBoxRenderer<dimensions>::AxisAlignedBoxRenderer(Physics::AxisAlignedBox<dimensions>& axisAlignedBox): axisAlignedBox(axisAlignedBox) {}

template<std::uint8_t dimensions> void AxisAlignedBoxRenderer<dimensions>::draw(Resource<ShapeRendererOptions>& options, const typename DimensionTraits<dimensions>::MatrixType& projectionMatrix) {
    /* Half scale, because the box is 2x2(x2) */
    typename DimensionTraits<dimensions>::MatrixType transformation =
        DimensionTraits<dimensions>::MatrixType::translation((axisAlignedBox.transformedMin()+axisAlignedBox.transformedMax())/2)*
        DimensionTraits<dimensions>::MatrixType::scaling((axisAlignedBox.transformedMax()-axisAlignedBox.transformedMin())/2);
    this->shader->setTransformationProjectionMatrix(projectionMatrix*transformation)
        ->setColor(options->color())
        ->use();
    this->mesh->draw();
}

template class AxisAlignedBoxRenderer<2>;
template class AxisAlignedBoxRenderer<3>;

}}}
