#ifndef Magnum_Physics_AbstractShape_h
#define Magnum_Physics_AbstractShape_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

/** @file
 * @brief Class Magnum::Physics::AbstractShape, typedef Magnum::Physics::AbstractShape2D, Magnum::Physics::AbstractShape3D
 */

#include "Magnum.h"
#include "DimensionTraits.h"

#include "magnumPhysicsVisibility.h"

namespace Magnum { namespace Physics {

#ifndef DOXYGEN_GENERATING_OUTPUT
namespace Implementation {
    template<UnsignedInt dimensions> struct ShapeDimensionTraits {};

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

    Debug MAGNUM_PHYSICS_EXPORT operator<<(Debug debug, ShapeDimensionTraits<2>::Type value);
    Debug MAGNUM_PHYSICS_EXPORT operator<<(Debug debug, ShapeDimensionTraits<3>::Type value);
}
#endif

/**
@brief Base class for shapes

See @ref collision-detection for brief introduction.
@see AbstractShape2D, AbstractShape3D
*/
template<UnsignedInt dimensions> class MAGNUM_PHYSICS_EXPORT AbstractShape {
    public:
        /** @brief Dimension count */
        static const UnsignedInt Dimensions = dimensions;

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

        explicit AbstractShape() = default;
        virtual inline ~AbstractShape() {}

        /** @brief Shape type */
        virtual Type type() const = 0;

        /**
         * @brief Apply transformation matrix
         *
         * Applies transformation matrix to user-defined shape properties and
         * caches them for later usage in collision detection.
         */
        virtual void applyTransformationMatrix(const typename DimensionTraits<dimensions>::MatrixType& matrix) = 0;

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

/** @brief Abstract two-dimensional shape */
typedef AbstractShape<2> AbstractShape2D;

/** @brief Abstract three-dimensional shape */
typedef AbstractShape<3> AbstractShape3D;

#ifdef DOXYGEN_GENERATING_OUTPUT
/** @debugoperator{Magnum::Physics::AbstractShape} */
template<UnsignedInt dimensions> Debug operator<<(Debug debug, typename AbstractShape<dimensions>::Type value);
#endif

}}

#endif
