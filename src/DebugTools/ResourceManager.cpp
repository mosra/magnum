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

#define MAGNUM_RESOURCEMANAGER_DEFINE_INTERNALINSTANCE

#include "ResourceManager.h"

#include "Buffer.h"
#include "Mesh.h"
#include "DebugTools/ObjectRenderer.h"
#include "DebugTools/ShapeRenderer.h"

namespace Magnum {

template class ResourceManager<AbstractShaderProgram, Buffer, Mesh, DebugTools::ObjectRendererOptions, DebugTools::ShapeRendererOptions>;

namespace DebugTools {

ResourceManager::ResourceManager() {
    setFallback(new ObjectRendererOptions);
    setFallback(new ShapeRendererOptions);
}

ResourceManager::~ResourceManager() {}

}}
