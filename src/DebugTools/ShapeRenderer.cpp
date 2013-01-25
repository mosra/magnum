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

#include "ShapeRenderer.h"

#include "ResourceManager.h"
#include "Physics/AbstractShape.h"
#include "Physics/AxisAlignedBox.h"
#include "Physics/Box.h"
#include "Physics/ObjectShape.h"
#include "Physics/ShapeGroup.h"
#include "SceneGraph/AbstractCamera.h"

#include "Implementation/AxisAlignedBoxRenderer.h"
#include "Implementation/BoxRenderer.h"

namespace Magnum { namespace DebugTools {

#ifndef DOXYGEN_GENERATING_OUTPUT
namespace Implementation {

template<> void createDebugMesh(ShapeRenderer<2>* renderer, Physics::AbstractShape<2>* shape) {
    switch(shape->type()) {
        case Physics::AbstractShape2D::Type::AxisAlignedBox:
            renderer->renderers.push_back(new Implementation::AxisAlignedBoxRenderer<2>(*static_cast<Physics::AxisAlignedBox2D*>(shape)));
        case Physics::AbstractShape2D::Type::Box:
            renderer->renderers.push_back(new Implementation::BoxRenderer<2>(*static_cast<Physics::Box2D*>(shape)));
            break;
        case Physics::AbstractShape2D::Type::ShapeGroup: {
            Physics::ShapeGroup2D* group = static_cast<Physics::ShapeGroup2D*>(shape);
            if(group->first()) createDebugMesh(renderer, group->first());
            if(group->second()) createDebugMesh(renderer, group->second());
        } break;
        default:
            Warning() << "DebugTools::ShapeRenderer2D::createShapeRenderer(): type" << shape->type() << "not implemented";
    }
}

template<> void createDebugMesh(ShapeRenderer<3>* renderer, Physics::AbstractShape<3>* shape) {
    switch(shape->type()) {
        case Physics::AbstractShape3D::Type::AxisAlignedBox:
            renderer->renderers.push_back(new Implementation::AxisAlignedBoxRenderer<3>(*static_cast<Physics::AxisAlignedBox3D*>(shape)));
        case Physics::AbstractShape3D::Type::Box:
            renderer->renderers.push_back(new Implementation::BoxRenderer<3>(*static_cast<Physics::Box3D*>(shape)));
            break;
        case Physics::AbstractShape3D::Type::ShapeGroup: {
            Physics::ShapeGroup3D* group = static_cast<Physics::ShapeGroup3D*>(shape);
            if(group->first()) createDebugMesh(renderer, group->first());
            if(group->second()) createDebugMesh(renderer, group->second());
        } break;
        default:
            Warning() << "DebugTools::ShapeRenderer3D::createShapeRenderer(): type" << shape->type() << "not implemented";
    }
}

}
#endif

template<std::uint8_t dimensions> ShapeRenderer<dimensions>::ShapeRenderer(Physics::ObjectShape<dimensions>* shape, ResourceKey options, SceneGraph::DrawableGroup<dimensions>* drawables): SceneGraph::Drawable<dimensions>(shape->object(), drawables), options(ResourceManager::instance()->get<ShapeRendererOptions>(options)) {
    CORRADE_ASSERT(shape->shape() != nullptr, "DebugTools::ShapeRenderer: cannot create renderer for empty shape", );
    Implementation::createDebugMesh(this, shape->shape());
}

template<std::uint8_t dimensions> ShapeRenderer<dimensions>::~ShapeRenderer() {
    for(auto i: renderers) delete i;
}

template<std::uint8_t dimensions> void ShapeRenderer<dimensions>::draw(const typename DimensionTraits<dimensions>::MatrixType&, SceneGraph::AbstractCamera<dimensions>* camera) {
    typename DimensionTraits<dimensions>::MatrixType projectionMatrix = camera->projectionMatrix()*camera->cameraMatrix();
    for(auto i: renderers) i->draw(options, projectionMatrix);
}

template class ShapeRenderer<2>;
template class ShapeRenderer<3>;

}}
