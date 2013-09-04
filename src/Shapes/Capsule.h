#ifndef Magnum_Shapes_Capsule_h
#define Magnum_Shapes_Capsule_h
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
 * @brief Class Magnum::Shapes::Capsule, typedef Magnum::Shapes::Capsule2D, Magnum::Shapes::Capsule3D
 */

#include "Math/Vector3.h"
#include "DimensionTraits.h"
#include "Shapes/Shapes.h"
#include "Shapes/magnumShapesVisibility.h"

namespace Magnum { namespace Shapes {

/**
@brief %Capsule defined by cylinder start and end point and radius

Unlike other elements the capsule expects uniform scaling. See @ref shapes for
brief introduction.
@see Capsule2D, Capsule3D, Cylinder
@todo Store the radius as squared value to avoid sqrt/pow? Will complicate
    collision detection with sphere.
*/
template<UnsignedInt dimensions> class MAGNUM_SHAPES_EXPORT Capsule {
    public:
        enum: UnsignedInt {
            Dimensions = dimensions /**< Dimension count */
        };

        /**
         * @brief Constructor
         *
         * Creates zero-sized capsule at origin.
         */
        constexpr /*implicit*/ Capsule(): _radius(0.0f) {}

        /** @brief Constructor */
        constexpr /*implicit*/ Capsule(const typename DimensionTraits<dimensions, Float>::VectorType& a, const typename DimensionTraits<dimensions, Float>::VectorType& b, Float radius): _a(a), _b(b), _radius(radius) {}

        /** @brief Transformed shape */
        Capsule<dimensions> transformed(const typename DimensionTraits<dimensions, Float>::MatrixType& matrix) const;

        /** @brief Start point */
        constexpr typename DimensionTraits<dimensions, Float>::VectorType a() const {
            return _a;
        }

        /** @brief Set start point */
        void setA(const typename DimensionTraits<dimensions, Float>::VectorType& a) {
            _a = a;
        }

        /** @brief End point */
        constexpr typename DimensionTraits<dimensions, Float>::VectorType b() const {
            return _b;
        }

        /** @brief Set end point */
        void setB(const typename DimensionTraits<dimensions, Float>::VectorType& b) {
            _b = b;
        }

        /** @brief Radius */
        constexpr Float radius() const { return _radius; }

        /** @brief Set radius */
        void setRadius(Float radius) { _radius = radius; }

        /** @brief %Collision occurence with point */
        bool operator%(const Point<dimensions>& other) const;

        /** @brief %Collision occurence with sphere */
        bool operator%(const Sphere<dimensions>& other) const;

    private:
        typename DimensionTraits<dimensions, Float>::VectorType _a, _b;
        Float _radius;
};

/** @brief Two-dimensional capsule */
typedef Capsule<2> Capsule2D;

/** @brief Three-dimensional capsule */
typedef Capsule<3> Capsule3D;

/** @collisionoccurenceoperator{Point,Capsule} */
template<UnsignedInt dimensions> inline bool operator%(const Point<dimensions>& a, const Capsule<dimensions>& b) { return b % a; }

/** @collisionoccurenceoperator{Sphere,Capsule} */
template<UnsignedInt dimensions> inline bool operator%(const Sphere<dimensions>& a, const Capsule<dimensions>& b) { return b % a; }

}}

#endif
