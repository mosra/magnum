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
#include "AbstractShape.h"
#include "Box.h"
#include "ShapedObject.h"
#include "ShapeGroup.h"
#include "Implementation/AbstractDebugRenderer.h"
#include "Implementation/BoxRenderer.h"
#include "Implementation/ShapeShader.h"

namespace Magnum {

template class ResourceManager<AbstractShaderProgram, Buffer, Mesh, Physics::Implementation::Options>;

namespace Physics {

SceneGraph::Object2D* DebugDrawResourceManager::createDebugRenderer(AbstractShape2D* shape, ResourceKey options) {
    return createDebugMesh(nullptr, shape, options);
}

SceneGraph::Object2D* DebugDrawResourceManager::createDebugMesh(SceneGraph::Object2D* parent, AbstractShape2D* shape, ResourceKey options) {
    switch(shape->type()) {
        case AbstractShape2D::Type::Box:
            return new Implementation::BoxRenderer<2>(*static_cast<Box2D*>(shape), options, parent);
        case AbstractShape2D::Type::ShapeGroup: {
            if(!parent) parent = new SceneGraph::Object2D;
            ShapeGroup2D* group = static_cast<ShapeGroup2D*>(shape);
            if(group->first()) createDebugMesh(parent, group->first(), options);
            if(group->second()) createDebugMesh(parent, group->second(), options);
            return parent;
        } default: return nullptr;
    }
}

DebugDrawResourceManager::DebugDrawResourceManager() {
    setFallback<Options>(new Options);
    set<AbstractShaderProgram>("shader2d", new Implementation::ShapeShader<2>, ResourceDataState::Final, ResourcePolicy::Resident);
}

DebugDrawResourceManager::~DebugDrawResourceManager() {}

}}
