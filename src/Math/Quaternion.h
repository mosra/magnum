#ifndef Magnum_Math_Quaternion_h
#define Magnum_Math_Quaternion_h
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
 * @brief Class Magnum::Math::Quaternion
 */

#include <cmath>
#include <Utility/Debug.h>

#include "Vector3.h"
#include "MathTypeTraits.h"

namespace Magnum { namespace Math {

/** @brief %Quaternion */
template<class T> class Quaternion {
    public:
        /** @brief Default constructor */
        inline constexpr Quaternion(): _scalar(T(1)) {}

        /** @brief Create quaternion from vector and scalar */
        inline constexpr Quaternion(const Vector3<T>& vector, T scalar): _vector(vector), _scalar(scalar) {}

        /** @brief Equality comparison */
        inline bool operator==(const Quaternion<T>& other) const {
            return _vector == other._vector && MathTypeTraits<T>::equals(_scalar, other._scalar);
        }

        /** @brief Non-equality comparison */
        inline bool operator!=(const Quaternion<T>& other) const {
            return !operator==(other);
        }

        /** @brief %Vector part */
        inline constexpr Vector3<T> vector() const { return _vector; }

        /** @brief %Scalar part */
        inline constexpr T scalar() const { return _scalar; }

        /**
         * @brief Multiply with scalar and assign
         *
         * The computation is done in-place.
         */
        inline Quaternion<T>& operator*=(T number) {
            _vector *= number;
            _scalar *= number;
            return *this;
        }

        /**
         * @brief Divide with scalar and assign
         *
         * The computation is done in-place.
         */
        inline Quaternion<T>& operator/=(T number) {
            _vector /= number;
            _scalar /= number;
            return *this;
        }

        /**
         * @brief Multiply with scalar
         *
         * @see operator*=(T)
         */
        inline constexpr Quaternion<T> operator*(T scalar) const {
            return Quaternion<T>(*this)*=scalar;
        }

        /**
         * @brief Divide with scalar
         *
         * @see operator/=(T)
         */
        inline constexpr Quaternion<T> operator/(T scalar) const {
            return Quaternion<T>(*this)/=scalar;
        }

        /**
         * @brief Multiply with quaternion
         *
         * The computation is *not* done in-place.
         */
        inline constexpr Quaternion<T> operator*(const Quaternion<T>& other) const {
            return {_scalar*other._vector + other._scalar*_vector + Vector3<T>::cross(_vector, other._vector),
                    _scalar*other._scalar - Vector3<T>::dot(_vector, other._vector)};
        }

        /**
         * @brief Multiply with quaternion and assign
         *
         * @see operator*(const Quaternion<T>&) const
         */
        inline Quaternion<T>& operator*=(const Quaternion<T>& other) {
            return (*this = *this * other);
        }

        /** @brief %Quaternion length squared */
        inline constexpr T lengthSquared() const {
            return _vector.dot() + _scalar*_scalar;
        }

        /** @brief %Quaternion length */
        inline constexpr T length() const {
            return std::sqrt(lengthSquared());
        }

        /** @brief Normalized quaternion */
        inline constexpr Quaternion<T> normalized() const {
            return (*this)/length();
        }

        /** @brief Conjugated quaternion */
        inline constexpr Quaternion<T> conjugated() const {
            return {-_vector, _scalar};
        }

        /**
         * @brief Inverted quaternion
         *
         * If the quaternion is already normalized, this function is
         * equivalent to conjugated().
         */
        inline constexpr Quaternion<T> inverted() const {
            return conjugated()/lengthSquared();
        }

    private:
        Vector3<T> _vector;
        T _scalar;
};

/** @debugoperator{Magnum::Math::Geometry::Rectangle} */
template<class T> Corrade::Utility::Debug operator<<(Corrade::Utility::Debug debug, const Quaternion<T>& value) {
    debug << "Quaternion({";
    debug.setFlag(Corrade::Utility::Debug::SpaceAfterEachValue, false);
    debug << value.vector().x() << ", " << value.vector().y() << ", " << value.vector().z() << "}, " << value.scalar() << ")";
    debug.setFlag(Corrade::Utility::Debug::SpaceAfterEachValue, true);
    return debug;
}

/* Explicit instantiation for commonly used types */
#ifndef DOXYGEN_GENERATING_OUTPUT
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const Quaternion<float>&);
#ifndef MAGNUM_TARGET_GLES
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const Quaternion<double>&);
#endif
#endif

}}

#endif
