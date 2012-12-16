#ifndef Magnum_Physics_DebugDrawResourceManager_h
#define Magnum_Physics_DebugDrawResourceManager_h
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
 * @brief Class Magnum::Physics::DebugDrawResourceManager
 */

#include "Magnum.h"
#include "Color.h"

#ifndef MAGNUM_RESOURCEMANAGER_DEFINE_INTERNALINSTANCE
#define MAGNUM_RESOURCEMANAGER_DONT_DEFINE_INTERNALINSTANCE
#endif
#include "ResourceManager.h"

#include "SceneGraph/SceneGraph.h"
#include "Physics.h"

#include "magnumPhysicsVisibility.h"

namespace Magnum {

#ifndef DOXYGEN_GENERATING_OUTPUT
namespace Physics { namespace Implementation {
    struct Options {
        Color3<> color;
    };
    template<std::uint8_t> class DebugRenderer;
}}
#endif

extern template ResourceManager<AbstractShaderProgram, Buffer, Mesh, Physics::Implementation::Options> MAGNUM_PHYSICS_EXPORT *& ResourceManager<AbstractShaderProgram, Buffer, Mesh, Physics::Implementation::Options>::internalInstance();

namespace Physics {

/**
@brief %Resource manager for physics debug draw

Can create objects which draw object collision shape for debugging purposes.

@section DebugDrawResourceManager-usage Basic usage
The manager must be instanced for the whole lifetime of debug draw objects.
To create debug renderers, call createDebugRenderer() and add the resulting
drawable to some group. You can specify options via Options struct - add it to
the manager and then create debug renderer with the same options key. This way
you can easily share the same options with more renderers. If no options for
given key exist, default is used.

Example code:
@code
// Group of drawables,preferrably dedicated for debug renderers, so you can
// easily enable or disable debug draw
DrawableGroup2D group;

// Instance the manager at first
DebugDrawResourceManager manager;

// Create some options
auto o = new DebugDrawResourceManager::Options {
    {1.0f, 0.0f, 0.0f} // Red color
};
manager->set<DebugDrawResourceManager::Options>("red", o, ResourceDataState::Final, ResourcePolicy::Persistent);

// Create debug renderer for given shape, use "red" options for it. Don't
// forget to add it to some drawable group.
ObjectShape2D* shape;
group.add(DebugDrawResourceManager::createDebugRenderer(shape, "red"));
@endcode
*/
class MAGNUM_PHYSICS_EXPORT DebugDrawResourceManager: public ResourceManager<AbstractShaderProgram, Buffer, Mesh, Physics::Implementation::Options> {
    public:
        #ifdef DOXYGEN_GENERATING_OUTPUT
        /** @brief %Options */
        struct Options {
            Color3<> color; /**< @brief Color */
        };
        #else
        typedef Implementation::Options Options;
        #endif

        /**
         * @brief Create debug renderer for given shape
         * @param shape     Shape for which to create debug renderer
         * @param options   Options resource key. See
         *      @ref DebugDrawResourceManager-usage "class documentation" for
         *      more information.
         *
         * Returned drawable is not part of any group, you have to add it to
         * one yourself.
         */
        template<std::uint8_t dimensions> static SceneGraph::Drawable<dimensions>* createDebugRenderer(ObjectShape<dimensions>* shape, ResourceKey options = ResourceKey());

        explicit DebugDrawResourceManager();
        ~DebugDrawResourceManager();

    private:
        static void createDebugMesh(Implementation::DebugRenderer<2>* renderer, AbstractShape2D* shape);
        static void createDebugMesh(Implementation::DebugRenderer<3>* renderer, AbstractShape3D* shape);
};

}}

#endif
