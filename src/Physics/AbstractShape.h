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
#include "DimensionTraits.h"

#include "magnumPhysicsVisibility.h"

namespace Magnum { namespace Physics {

#ifndef DOXYGEN_GENERATING_OUTPUT
namespace Implementation {
    template<std::uint8_t dimensions> struct ShapeDimensionTraits {};

    template<> struct ShapeDimensionTraits<2> {
        enum class Type {
            Point,
            Line,
            LineSegment,
            Sphere,
            Capsule,
            AxisAlignedBox,
            Box,
            ShapeGroup
        };
    };

    template<> struct ShapeDimensionTraits<3> {
        enum class Type {
            Point,
            Line,
            LineSegment,
            Sphere,
            Capsule,
            AxisAlignedBox,
            Box,
            ShapeGroup,
            Plane
        };
    };
}
#endif

/**
@brief Base class for shapes

See @ref collision-detection for brief introduction.
@see AbstractShape2D, AbstractShape3D
*/
template<std::uint8_t dimensions> class PHYSICS_EXPORT AbstractShape {
    public:
        /** @brief Dimension count */
        static const std::uint8_t Dimensions = dimensions;

        /**
         * @brief Shape type
         *
         * @internal Sorted by complexity, so the shape which is later in
         *      the list provides collision detection for previous shapes, not
         *      the other way around.
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        enum class Type {
            Point,          /**< Point */
            Line,           /**< Line */
            LineSegment,    /**< @ref LineSegment "Line segment" */
            Sphere,         /**< Sphere */
            Capsule,        /**< Capsule */
            AxisAlignedBox, /**< @ref AxisAlignedBox "Axis aligned box" */
            Box,            /**< Box */
            ShapeGroup,     /**< @ref ShapeGroup "Shape group" */
            Plane           /**< Plane (3D only) */
        };
        #else
        typedef typename Implementation::ShapeDimensionTraits<dimensions>::Type Type;
        #endif

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
        virtual void applyTransformation(const typename DimensionTraits<dimensions, GLfloat>::MatrixType& transformation) = 0;

        /**
         * @brief Detect collision with other shape
         *
         * Default implementation returns false.
         *
         * @internal If other shape is more complex than this, returns
         *      `other->collides(this)`.
         */
        virtual bool collides(const AbstractShape<dimensions>* other) const;
};

#ifndef DOXYGEN_GENERATING_OUTPUT
extern template class PHYSICS_EXPORT AbstractShape<2>;
extern template class PHYSICS_EXPORT AbstractShape<3>;
#endif

/** @brief Abstract two-dimensional shape */
typedef AbstractShape<2> AbstractShape2D;

/** @brief Abstract three-dimensional shape */
typedef AbstractShape<3> AbstractShape3D;

}}

#endif
