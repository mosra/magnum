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

#include "AbstractShapeRenderer.h"

#include "AbstractShaderProgram.h"
#include "Mesh.h"
#include "Physics/DebugDrawResourceManager.h"
#include "Shaders/FlatShader.h"

namespace Magnum { namespace Physics { namespace Implementation {

template<std::uint8_t dimensions> AbstractShapeRenderer<dimensions>::AbstractShapeRenderer(ResourceKey shader, ResourceKey mesh): shader(DebugDrawResourceManager::instance()->get<AbstractShaderProgram, Shaders::FlatShader<dimensions>>(shader)), mesh(DebugDrawResourceManager::instance()->get<Mesh>(mesh)) {}

template<std::uint8_t dimensions> AbstractShapeRenderer<dimensions>::~AbstractShapeRenderer() {}

template class AbstractShapeRenderer<2>;
template class AbstractShapeRenderer<3>;

}}}
