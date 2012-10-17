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
#include "ResourceManager.h"

#include "magnumPhysicsVisibility.h"

namespace Magnum {

class AbstractShaderProgram;
class Mesh;

#ifndef DOXYGEN_GENERATING_OUTPUT
namespace Physics { namespace Implementation {
    struct Options {
        Color3<GLfloat> color;
    };
    template<std::uint8_t> class AbstractDebugRenderer;
}}

extern template class PHYSICS_EXPORT ResourceManager<AbstractShaderProgram, Mesh, Physics::Implementation::Options>;
#endif

namespace SceneGraph {
    class Object2D;
    class Object3D;
}

namespace Physics {

template<std::uint8_t> class AbstractShape;
typedef AbstractShape<2> AbstractShape2D;
typedef AbstractShape<3> AbstractShape3D;

/**
@brief %Resource manager for physics debug draw

Can create objects which draw object collision shape for debugging purposes.

@section DebugDrawResourceManager-usage Basic usage
The manager must be instanced for the whole lifetime of debug draw objects.
To create debug draw objects, call createDebugRenderer() and add the resulting
object to the scene. You can specify options via Options struct - add it to
the manager and then create debug renderer with the same options key. This way
you can easily share the same options with more objects. If no options for
given key exist, default is used.

Example code:
@code
// Instance the manager at first
DebugDrawResourceManager manager;

// Create some options
auto o = new DebugDrawResourceManager::Options {
    {1.0f, 0.0f, 0.0f} // Red color
};
manager->set<DebugDrawResourceManager::Options>("red", o, ResourceDataState::Final, ResourcePolicy::Persistent);

// Add debug draw object for given shape, use "red" options for it, don't
// forget to add it to the scene
ShapedObject2D* object;
DebugDrawResourceManager::createDebugRenderer(object->shape(), "red")
    ->setParent(object);
@endcode
*/
class PHYSICS_EXPORT DebugDrawResourceManager: public ResourceManager<AbstractShaderProgram, Mesh, Physics::Implementation::Options> {
    public:
        #ifdef DOXYGEN_GENERATING_OUTPUT
        /** @brief %Options */
        struct Options {
            Color3<GLfloat> color; /**< @brief Color */
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
         * Returned object is not parented to anything, you have to add it to
         * desired scene yourself.
         */
        static SceneGraph::Object2D* createDebugRenderer(AbstractShape2D* shape, ResourceKey options = ResourceKey());

        DebugDrawResourceManager();

        ~DebugDrawResourceManager();

    private:
        static SceneGraph::Object2D* createDebugMesh(SceneGraph::Object2D* parent, AbstractShape2D* shape, ResourceKey options);
};

}}

#endif
