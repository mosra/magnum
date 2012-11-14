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

#include "DebugDrawResourceManager.h"

#include "AbstractShaderProgram.h"
#include "Buffer.h"
#include "Mesh.h"
#include "ResourceManager.h"
#include "Shaders/FlatShader.h"
#include "AbstractShape.h"
#include "Box.h"
#include "ObjectShape.h"
#include "ShapeGroup.h"
#include "Implementation/BoxRenderer.h"
#include "Implementation/DebugRenderer.h"

namespace Magnum {

template class ResourceManager<AbstractShaderProgram, Buffer, Mesh, Physics::Implementation::Options>;

namespace Physics {

template<std::uint8_t dimensions> SceneGraph::Drawable<dimensions>* DebugDrawResourceManager::createDebugRenderer(ObjectShape<dimensions>* shape, ResourceKey options) {
    Implementation::DebugRenderer<dimensions>* renderer = new Implementation::DebugRenderer<dimensions>(shape->object(), instance()->get<Options>(options));
    createDebugMesh(renderer, shape->shape());
    return renderer;
}

template SceneGraph::Drawable<2> PHYSICS_EXPORT * DebugDrawResourceManager::createDebugRenderer(ObjectShape<2>* shape, ResourceKey options);
template SceneGraph::Drawable<3> PHYSICS_EXPORT * DebugDrawResourceManager::createDebugRenderer(ObjectShape<3>* shape, ResourceKey options);

void DebugDrawResourceManager::createDebugMesh(Implementation::DebugRenderer<2>* renderer, AbstractShape2D* shape) {
    switch(shape->type()) {
        case AbstractShape2D::Type::Box:
            renderer->addRenderer(new Implementation::BoxRenderer<2>(*static_cast<Box2D*>(shape)));
            break;
        case AbstractShape2D::Type::ShapeGroup: {
            ShapeGroup2D* group = static_cast<ShapeGroup2D*>(shape);
            if(group->first()) createDebugMesh(renderer, group->first());
            if(group->second()) createDebugMesh(renderer, group->second());
            break;
        }
        default:
            Warning() << "Physics::DebugDrawResourceManager::createDebugRenderer(): type" << shape->type() << "not implemented";
    }
}

void DebugDrawResourceManager::createDebugMesh(Implementation::DebugRenderer<3>*, AbstractShape3D*) {}

DebugDrawResourceManager::DebugDrawResourceManager() {
    setFallback<Options>(new Options);
    set<AbstractShaderProgram>("shader2d", new Shaders::FlatShader<2>, ResourceDataState::Final, ResourcePolicy::Resident);
}

DebugDrawResourceManager::~DebugDrawResourceManager() {}

}}
