#ifndef Magnum_Shapes_Sphere_h
#define Magnum_Shapes_Sphere_h
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
 * @brief Class Magnum::Shapes::Sphere, typedef Magnum::Shapes::Sphere2D, Magnum::Shapes::Sphere3D
 */

#include "Math/Vector3.h"
#include "DimensionTraits.h"
#include "Shapes/Shapes.h"
#include "Shapes/magnumShapesVisibility.h"

namespace Magnum { namespace Shapes {

/**
@brief %Sphere defined by position and radius

Unlike other elements the sphere doesn't support asymmetric scaling. When
applying transformation, the scale factor is averaged from all axes. See
@ref shapes for brief introduction.
@see Sphere2D, Sphere3D
@todo Assert for asymmetric scaling
*/
template<UnsignedInt dimensions> class MAGNUM_SHAPES_EXPORT Sphere {
    public:
        enum: UnsignedInt {
            Dimensions = dimensions /**< Dimension count */
        };

        /**
         * @brief Default constructor
         *
         * Creates zero-sized sphere at origin.
         */
        inline constexpr /*implicit*/ Sphere(): _radius(0.0f) {}

        /** @brief Constructor */
        inline constexpr /*implicit*/ Sphere(const typename DimensionTraits<dimensions>::VectorType& position, Float radius): _position(position), _radius(radius) {}

        /** @brief Transformed shape */
        Sphere<dimensions> transformed(const typename DimensionTraits<dimensions>::MatrixType& matrix) const;

        /** @brief Position */
        inline constexpr typename DimensionTraits<dimensions>::VectorType position() const {
            return _position;
        }

        /** @brief Set position */
        inline void setPosition(const typename DimensionTraits<dimensions>::VectorType& position) {
            _position = position;
        }

        /** @brief Radius */
        inline constexpr Float radius() const { return _radius; }

        /** @brief Set radius */
        inline void setRadius(Float radius) { _radius = radius; }

        /** @brief Collision with point */
        bool operator%(const Point<dimensions>& other) const;

        /** @brief Collision with line */
        bool operator%(const Line<dimensions>& other) const;

        /** @brief Collision with line segment */
        bool operator%(const LineSegment<dimensions>& other) const;

        /** @brief Collision with sphere */
        bool operator%(const Sphere<dimensions>& other) const;

    private:
        typename DimensionTraits<dimensions>::VectorType _position;
        Float _radius;
};

/** @brief Two-dimensional sphere */
typedef Sphere<2> Sphere2D;

/** @brief Three-dimensional sphere */
typedef Sphere<3> Sphere3D;

/** @collisionoperator{Point,Sphere} */
template<UnsignedInt dimensions> inline bool operator%(const Point<dimensions>& a, const Sphere<dimensions>& b) { return b % a; }

/** @collisionoperator{Line,Sphere} */
template<UnsignedInt dimensions> inline bool operator%(const Line<dimensions>& a, const Sphere<dimensions>& b) { return b % a; }

/** @collisionoperator{LineSegment,Sphere} */
template<UnsignedInt dimensions> inline bool operator%(const LineSegment<dimensions>& a, const Sphere<dimensions>& b) { return b % a; }

}}

#endif
