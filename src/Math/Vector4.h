#ifndef Magnum_Math_Vector4_h
#define Magnum_Math_Vector4_h
/*
    Copyright © 2010 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class Magnum::Math::Vector4
 */

#include "Vector.h"

#include "Vector3.h"

namespace Magnum { namespace Math {

/** @brief Vector (four-component) */
template<class T> class Vector4: public Vector<T, 4> {
    public:
        /**
         * @copydoc Vector::Vector
         *
         * W / A component is set to one.
         */
        inline Vector4() {
            setW(T(1));
        }

        /** @copydoc Vector::Vector(const T*) */
        inline Vector4(const T* data): Vector<T, 4>(data) {}

        /** @copydoc Vector::Vector(const Vector&)  */
        inline Vector4(const Vector<T, 4>& other): Vector<T, 4>(other) {}

        /**
         * @brief Constructor
         * @param x     X / R value
         * @param y     Y / G value
         * @param z     Z / B value
         * @param w     W / A value
         */
        inline Vector4(T x, T y, T z, T w = T(1)) {
            setX(x); setY(y); setZ(z); setW(w);
        }

        inline T x() const { return Vector<T, 4>::at(0); } /**< @brief X component */
        inline T y() const { return Vector<T, 4>::at(1); } /**< @brief Y component */
        inline T z() const { return Vector<T, 4>::at(2); } /**< @brief Z component */
        inline T w() const { return Vector<T, 4>::at(3); } /**< @brief W component */

        inline void setX(T value) { set(0, value); } /**< @brief Set X component */
        inline void setY(T value) { set(1, value); } /**< @brief Set Y component */
        inline void setZ(T value) { set(2, value); } /**< @brief Set Z component */
        inline void setW(T value) { set(3, value); } /**< @brief Set W component */

        /**
         * @brief XYZ part of the vector
         * @return First three components of the vector
         */
        inline Vector3<T> xyz() const { return Vector3<T>(Vector<T, 4>::data()); }

        inline T r() const { return x(); } /**< @brief R component */
        inline T g() const { return y(); } /**< @brief G component */
        inline T b() const { return z(); } /**< @brief B component */
        inline T a() const { return w(); } /**< @brief A component */

        inline void setR(T value) { setX(value); } /**< @brief Set R component */
        inline void setG(T value) { setY(value); } /**< @brief Set G component */
        inline void setB(T value) { setZ(value); } /**< @brief Set B component */
        inline void setA(T value) { setA(value); } /**< @brief Set A component */

        /**
         * @brief RGB part of the vector
         * @return First three components of the vector
         */
        inline Vector3<T> rgb() const { return xyz(); }
};

}}

#endif
