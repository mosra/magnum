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

namespace Magnum { namespace Physics {

template<std::uint8_t> class ShapedObject;

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
        /**
         * @brief Constructor
         *
         * Marks the group as dirty.
         */
        inline constexpr ShapedObjectGroup(): dirty(true) {}

        /**
         * @brief Destructor
         *
         * Deletes all objects belogning to the group.
         */
        inline virtual ~ShapedObjectGroup() {
            for(auto i: objects) delete i;
        }

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
        std::vector<ShapedObject<dimensions>*> objects;
        bool dirty;
};

/** @brief Group of two-dimensional shaped objects */
typedef ShapedObjectGroup<2> ShapedObjectGroup2D;

/** @brief Group of three-dimensional shaped objects */
typedef ShapedObjectGroup<3> ShapedObjectGroup3D;

}}

#endif
