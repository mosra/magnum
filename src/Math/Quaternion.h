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
#include <Utility/Assert.h>
#include <Utility/Debug.h>

#include "Math/Math.h"
#include "Math/MathTypeTraits.h"
#include "Math/Vector3.h"

namespace Magnum { namespace Math {

/** @brief %Quaternion */
template<class T> class Quaternion {
    public:
        /**
         * @brief Create quaternion from rotation
         * @param angle             Rotation angle (counterclockwise, in radians)
         * @param normalizedAxis    Normalized rotation axis
         *
         * Assumes that the rotation axis is normalized.
         */
        inline static Quaternion<T> fromRotation(T angle, const Vector3<T>& normalizedAxis) {
            CORRADE_ASSERT(MathTypeTraits<T>::equals(normalizedAxis.dot(), T(1)),
                           "Math::Quaternion::fromRotation(): axis must be normalized", {});

            return {normalizedAxis*std::sin(angle/2), std::cos(angle/2)};
        }

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
         * @brief Rotation angle of unit quaternion
         *
         * Assumes that the quaternion is normalized.
         */
        inline T rotationAngle() const {
            CORRADE_ASSERT(MathTypeTraits<T>::equals(lengthSquared(), T(1)),
                           "Math::Quaternion::rotationAngle(): quaternion must be normalized",
                           std::numeric_limits<T>::quiet_NaN());
            return T(2)*std::acos(_scalar);
        }

        /**
         * @brief Rotation axis of unit quaternion
         *
         * Assumes that the quaternion is normalized.
         */
        inline Vector3<T> rotationAxis() const {
            CORRADE_ASSERT(MathTypeTraits<T>::equals(lengthSquared(), T(1)),
                           "Math::Quaternion::rotationAxis(): quaternion must be normalized",
                           {});
            return _vector/std::sqrt(1-pow<2>(_scalar));
        }

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
        inline Quaternion<T> operator*(T scalar) const {
            return Quaternion<T>(*this)*=scalar;
        }

        /**
         * @brief Divide with scalar
         *
         * @see operator/=(T)
         */
        inline Quaternion<T> operator/(T scalar) const {
            return Quaternion<T>(*this)/=scalar;
        }

        /**
         * @brief Multiply with quaternion
         *
         * The computation is *not* done in-place.
         */
        inline Quaternion<T> operator*(const Quaternion<T>& other) const {
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
        inline T lengthSquared() const {
            return _vector.dot() + _scalar*_scalar;
        }

        /** @brief %Quaternion length */
        inline T length() const {
            return std::sqrt(lengthSquared());
        }

        /** @brief Normalized quaternion */
        inline Quaternion<T> normalized() const {
            return (*this)/length();
        }

        /** @brief Conjugated quaternion */
        inline Quaternion<T> conjugated() const {
            return {-_vector, _scalar};
        }

        /**
         * @brief Inverted quaternion
         *
         * See invertedNormalized() which is faster for normalized
         * quaternions.
         */
        inline Quaternion<T> inverted() const {
            return conjugated()/lengthSquared();
        }

        /**
         * @brief Inverted normalized quaternion
         *
         * Equivalent to conjugated(). Assumes that the quaternion is
         * normalized.
         */
        inline Quaternion<T> invertedNormalized() const {
            CORRADE_ASSERT(MathTypeTraits<T>::equals(lengthSquared(), T(1)),
                           "Math::Quaternion::invertedNormalized(): quaternion must be normalized",
                           Quaternion({}, std::numeric_limits<T>::quiet_NaN()));
            return conjugated();
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
