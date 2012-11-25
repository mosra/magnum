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

#include "BoxRenderer.h"

#include "Mesh.h"
#include "Physics/DebugDrawResourceManager.h"
#include "SceneGraph/AbstractCamera.h"
#include "Shaders/FlatShader.h"

namespace Magnum { namespace Physics { namespace Implementation {

template<std::uint8_t dimensions> void BoxRenderer<dimensions>::draw(Resource<Options>& options, const typename DimensionTraits<dimensions>::MatrixType&, typename SceneGraph::AbstractCamera<dimensions>* camera) {
    this->shader->setTransformationProjectionMatrix(camera->projectionMatrix()*camera->cameraMatrix()*box.transformedTransformation())
        ->setColor(options->color)
        ->use();
    this->mesh->draw();
}

template class BoxRenderer<2>;
template class BoxRenderer<3>;

}}}
