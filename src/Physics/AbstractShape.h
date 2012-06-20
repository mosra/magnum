#ifndef Magnum_Physics_AbstractShape_h
#define Magnum_Physics_AbstractShape_h
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
 * @brief Class Magnum::Physics::AbstractShape
 */

#include "Magnum.h"
#include "visibilityPhysics.h"

namespace Magnum { namespace Physics {

/**
@brief Base class for shapes

See @ref collision-detection for brief introduction.
*/
class PHYSICS_EXPORT AbstractShape {
    public:
        /**
         * @brief Shape type
         *
         * @internal Sorted by complexity, so the shape which is later in
         *      the list provides collision detection for previous shapes, not
         *      the other way around.
         */
        enum class Type {
            Point,
            Line,
            LineSegment,
            Plane,
            Sphere,
            Capsule,
            AxisAlignedBox,
            Box,
            ShapeGroup
        };

        /** @brief Destructor */
        virtual inline ~AbstractShape() {}

        /** @brief Shape type */
        virtual Type type() const = 0;

        /**
         * @brief Apply transformation
         *
         * Applies transformation to user-defined shape properties and caches
         * them for later usage in collision detection.
         */
        virtual void applyTransformation(const Matrix4& transformation) = 0;

        /**
         * @brief Detect collision with other shape
         *
         * Default implementation returns false.
         *
         * @internal If other shape is more complex than this, returns
         *      `other->collides(this)`.
         */
        virtual bool collides(const AbstractShape* other) const;
};

}}

#endif
