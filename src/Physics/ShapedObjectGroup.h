#ifndef Magnum_Physics_ShapedObjectGroup_h
#define Magnum_Physics_ShapedObjectGroup_h
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
 * @brief Class Magnum::Physics::ShapedObjectGroup
 */

#include <cstdint>
#include <vector>

#include "magnumPhysicsVisibility.h"

namespace Magnum {

class AbstractShaderProgram;
template<class T, class U> class Resource;

namespace Physics {

template<std::uint8_t> class ShapedObject;

#ifndef DOXYGEN_GENERATING_OUTPUT
namespace Implementation {
    enum class DebugMode {
        None,
        Wireframe,
        SolidWireframe
    };
}
#endif

/**
@brief Group of shaped objects

@ref ShapedObject "ShapedObject*D" instances are added to the group by
specifying it in the constructor. When the group is deleted, all objects
belogning to it are deleted too.
@see ShapedObjectGroup2D, ShapedObjectGroup3D
*/
template<std::uint8_t dimensions> class PHYSICS_EXPORT ShapedObjectGroup {
    friend class ShapedObject<dimensions>;

    public:
        #ifdef DOXYGEN_GENERATING_OUTPUT
        /**
         * @brief Debug mode
         *
         * @see setDebugMode()
         */
        enum class DebugMode {
            None,               /**< @brief Nothing is rendered */
            Wireframe,          /**< @brief Wireframe of the shape is rendered */
            SolidWireframe      /**< @brief Solid with wireframe is rendered */
        };
        #else
        typedef Implementation::DebugMode DebugMode;
        #endif

        /**
         * @brief Destructor
         *
         * Deletes all objects belogning to the group.
         */
        inline virtual ~ShapedObjectGroup() {
            for(auto i: objects) delete i;
        }

        /** @brief Debug mode */
        inline DebugMode debugMode() const { return _debugMode; }

        /** @brief Set debug mode */
        inline void setDebugMode(DebugMode mode) { _debugMode = mode; }

        /**
         * @brief Whether the group is dirty
         * @return True if any object in the group is dirty, false otherwise.
         */
        inline bool isDirty() const { return dirty; }

        /**
         * @brief Set the group as dirty
         *
         * If some body in the group changes its transformation, it sets dirty
         * status also on the group to indicate that the body and maybe also
         * group state needs to be cleaned before computing collisions.
         *
         * @see setClean()
         */
        inline void setDirty() { dirty = true; }

        /**
         * @brief Set the group and all bodies as clean
         *
         * This function is called before computing any collisions to ensure
         * all objects are cleaned.
         */
        void setClean();

    private:
        Resource<AbstractShaderProgram, AbstractShaderProgram>& shader();

        DebugMode _debugMode;
        std::vector<ShapedObject<dimensions>*> objects;
        bool dirty;
};

#ifndef DOXYGEN_GENERATING_OUTPUT
extern template class PHYSICS_EXPORT ShapedObjectGroup<2>;
extern template class PHYSICS_EXPORT ShapedObjectGroup<3>;
#endif

/** @brief Group of two-dimensional shaped objects */
typedef ShapedObjectGroup<2> ShapedObjectGroup2D;

/** @brief Group of three-dimensional shaped objects */
typedef ShapedObjectGroup<3> ShapedObjectGroup3D;

}}

#endif
