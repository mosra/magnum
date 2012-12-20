#ifndef Magnum_DebugTools_ResourceManager_h
#define Magnum_DebugTools_ResourceManager_h
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

/** @file
 * @brief Class Magnum::DebugTools::ResourceManager
 */

#include "Magnum.h"

#ifndef MAGNUM_RESOURCEMANAGER_DEFINE_INTERNALINSTANCE
#define MAGNUM_RESOURCEMANAGER_DONT_DEFINE_INTERNALINSTANCE
#endif
#include "../ResourceManager.h"

#include "SceneGraph/SceneGraph.h"
#include "Physics/Physics.h"
#include "DebugTools.h"

#include "magnumDebugToolsVisibility.h"

namespace Magnum {

extern template ResourceManager<AbstractShaderProgram, Buffer, Mesh, DebugTools::ShapeRendererOptions> MAGNUM_DEBUGTOOLS_EXPORT  *& ResourceManager<AbstractShaderProgram, Buffer, Mesh, DebugTools::ShapeRendererOptions>::internalInstance();

namespace DebugTools {

/**
@brief %Resource manager for debug tools

Stores various data used by debug renderers.
*/
class MAGNUM_DEBUGTOOLS_EXPORT ResourceManager: public Magnum::ResourceManager<AbstractShaderProgram, Buffer, Mesh, DebugTools::ShapeRendererOptions> {
    public:
        explicit ResourceManager();
        ~ResourceManager();
};

}}

#endif
