#ifndef Magnum_Math_Point2D_h
#define Magnum_Math_Point2D_h
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
 * @brief Class Magnum::Math::Point2D
 */

#include "Vector3.h"

namespace Magnum { namespace Math {

/**
@brief Two-dimensional homogeneous coordinates
@tparam T   Data type

Same as Vector3, except that constructors have default value for Z component
set to one. See also @ref matrix-vector for brief introduction.
@configurationvalueref{Magnum::Math::Point2D}
*/
template<class T> class Point2D: public Vector3<T> {
    public:
        /**
         * @brief Default constructor
         *
         * X and Y components are set to zero, Z is set to one.
         */
        inline constexpr Point2D(): Vector3<T>(T(0), T(0), T(1)) {}

        /** @brief Copy constructor */
        inline constexpr Point2D(const RectangularMatrix<1, 3, T>& other): Vector3<T>(other) {}

        /**
         * @brief Constructor
         * @param x     X component
         * @param y     Y component
         * @param z     Z component
         */
        inline constexpr Point2D(T x, T y, T z = T(1)): Vector3<T>(x, y, z) {}

        /**
         * @brief Constructor
         * @param xy    Two-component vector
         * @param z     Z component
         */
        inline constexpr Point2D(const Vector<2, T>& xy, T z = T(1)): Vector3<T>(xy, z) {}

        MAGNUM_VECTOR_SUBCLASS_IMPLEMENTATION(Point2D, 3)
        MAGNUM_RECTANGULARMATRIX_SUBCLASS_OPERATOR_IMPLEMENTATION(1, 3, Point2D<T>)
};

MAGNUM_VECTOR_SUBCLASS_OPERATOR_IMPLEMENTATION(Point2D, 3)

/** @debugoperator{Magnum::Math::Point2D} */
template<class T> Corrade::Utility::Debug operator<<(Corrade::Utility::Debug debug, const Magnum::Math::Point2D<T>& value) {
    return debug << static_cast<const Magnum::Math::Vector<3, T>&>(value);
}

}}

namespace Corrade { namespace Utility {
    /** @configurationvalue{Magnum::Math::Point2D} */
    template<class T> struct ConfigurationValue<Magnum::Math::Point2D<T>>: public ConfigurationValue<Magnum::Math::Vector<3, T>> {};
}}

#endif
