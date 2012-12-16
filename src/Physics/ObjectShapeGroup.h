#ifndef Magnum_Physics_ObjectShapeGroup_h
#define Magnum_Physics_ObjectShapeGroup_h
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
 * @brief Class Magnum::Physics::ObjectShapeGroup
 */

#include <cstdint>
#include <vector>

#include "SceneGraph/FeatureGroup.h"
#include "Physics.h"

#include "magnumPhysicsVisibility.h"

namespace Magnum { namespace Physics {

/**
@brief Group of object shapes

@see ObjectShapeGroup2D, ObjectShapeGroup3D
*/
template<std::uint8_t dimensions> class MAGNUM_PHYSICS_EXPORT ObjectShapeGroup: public SceneGraph::FeatureGroup<dimensions, ObjectShape<dimensions>> {
    friend class ObjectShape<dimensions>;

    public:
        /**
         * @brief Constructor
         *
         * Marks the group as dirty.
         */
        inline explicit ObjectShapeGroup(): dirty(true) {}

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
        bool dirty;
};

/** @brief Group of two-dimensional shaped objects */
typedef ObjectShapeGroup<2> ObjectShapeGroup2D;

/** @brief Group of three-dimensional shaped objects */
typedef ObjectShapeGroup<3> ObjectShapeGroup3D;

}}

/* Make the definition complete */
#include "ObjectShape.h"

#endif
