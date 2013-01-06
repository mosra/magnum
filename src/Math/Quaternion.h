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
#include "Math/Matrix.h"
#include "Math/Vector3.h"

namespace Magnum { namespace Math {

/** @brief %Quaternion */
template<class T> class Quaternion {
    public:
        /**
         * @brief Dot product
         *
         * @f[
         * p \cdot q = \boldsymbol p_V \cdot \boldsymbol q_V + p_S q_S
         * @f]
         * @see dot() const
         */
        inline static T dot(const Quaternion<T>& a, const Quaternion<T>& b) {
            /** @todo Use four-component SIMD implementation when available */
            return Vector3<T>::dot(a.vector(), b.vector()) + a.scalar()*b.scalar();
        }

        /**
         * @brief Linear interpolation of two quaternions
         * @param normalizedA   First quaternion
         * @param normalizedB   Second quaternion
         * @param t             Interpolation phase (from range @f$ [0; 1] @f$)
         *
         * Expects that both quaternions are normalized. @f[
         *      q_{LERP} = \frac{(1 - t) q_A + t q_B}{|(1 - t) q_A + t q_B|}
         * @f]
         */
        inline static Quaternion<T> lerp(const Quaternion<T>& normalizedA, const Quaternion<T>& normalizedB, T t) {
            CORRADE_ASSERT(MathTypeTraits<T>::equals(normalizedA.dot(), T(1)) && MathTypeTraits<T>::equals(normalizedB.dot(), T(1)),
                           "Math::Quaternion::lerp(): quaternions must be normalized", Quaternion<T>({}, std::numeric_limits<T>::quiet_NaN()));
            return ((T(1) - t)*normalizedA + t*normalizedB).normalized();
        }

        /**
         * @brief Create quaternion from rotation
         * @param angle             Rotation angle (counterclockwise, in radians)
         * @param normalizedAxis    Normalized rotation axis
         *
         * Expects that the rotation axis is normalized. @f[
         *      q = [\boldsymbol a \cdot sin \frac \theta 2, cos \frac \theta 2]
         * @f]
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
         * Expects that the quaternion is normalized. @f[
         *      \theta = 2 \cdot acos q_S
         * @f]
         * @see rotationAxis(), fromRotation()
         */
        inline T rotationAngle() const {
            CORRADE_ASSERT(MathTypeTraits<T>::equals(dot(), T(1)),
                           "Math::Quaternion::rotationAngle(): quaternion must be normalized",
                           std::numeric_limits<T>::quiet_NaN());
            return T(2)*std::acos(_scalar);
        }

        /**
         * @brief Rotation axis of unit quaternion
         *
         * Expects that the quaternion is normalized. @f[
         *      \boldsymbol a = \frac{\boldsymbol q_V}{\sqrt{1 - q_S^2}}
         * @f]
         * @see rotationAngle(), fromRotation()
         */
        inline Vector3<T> rotationAxis() const {
            CORRADE_ASSERT(MathTypeTraits<T>::equals(dot(), T(1)),
                           "Math::Quaternion::rotationAxis(): quaternion must be normalized",
                           {});
            return _vector/std::sqrt(1-pow<2>(_scalar));
        }

        /**
         * @brief Convert quaternion to rotation matrix
         *
         * @see Matrix4::from(const Matrix<3, T>&, const Vector3<T>&)
         */
        Matrix<3, T> matrix() const {
            return { /* Column-major! */
                T(1) - 2*pow<2>(_vector.y()) - 2*pow<2>(_vector.z()),
                    2*_vector.x()*_vector.y() + 2*_vector.z()*_scalar,
                        2*_vector.x()*_vector.z() - 2*_vector.y()*_scalar,
                2*_vector.x()*_vector.y() - 2*_vector.z()*_scalar,
                    T(1) - 2*pow<2>(_vector.x()) - 2*pow<2>(_vector.z()),
                        2*_vector.y()*_vector.z() + 2*_vector.x()*_scalar,
                2*_vector.x()*_vector.z() + 2*_vector.y()*_scalar,
                    2*_vector.y()*_vector.z() - 2*_vector.x()*_scalar,
                        T(1) - 2*pow<2>(_vector.x()) - 2*pow<2>(_vector.y())
            };
        }

        /**
         * @brief Add and assign quaternion
         *
         * The computation is done in-place. @f[
         *      p + q = [\boldsymbol p_V + \boldsymbol q_V, p_S + q_S]
         * @f]
         */
        inline Quaternion<T>& operator+=(const Quaternion<T>& other) {
            _vector += other._vector;
            _scalar += other._scalar;
            return *this;
        }

        /**
         * @brief Add quaternion
         *
         * @see operator+=()
         */
        inline Quaternion<T> operator+(const Quaternion<T>& other) const {
            return Quaternion<T>(*this)+=other;
        }

        /**
         * @brief Negated quaternion
         *
         * @f[
         *      -q = [-\boldsymbol q_V, -q_S]
         * @f]
         */
        inline Quaternion<T> operator-() const {
            return {-_vector, -_scalar};
        }

        /**
         * @brief Subtract and assign quaternion
         *
         * The computation is done in-place. @f[
         *      p - q = [\boldsymbol p_V - \boldsymbol q_V, p_S - q_S]
         * @f]
         */
        inline Quaternion<T>& operator-=(const Quaternion<T>& other) {
            _vector -= other._vector;
            _scalar -= other._scalar;
            return *this;
        }

        /**
         * @brief Subtract quaternion
         *
         * @see operator-=()
         */
        inline Quaternion<T> operator-(const Quaternion<T>& other) const {
            return Quaternion<T>(*this)-=other;
        }

        /**
         * @brief Multiply with scalar and assign
         *
         * The computation is done in-place. @f[
         *      q \cdot a = [\boldsymbol q_V \cdot a, q_S \cdot a]
         * @f]
         */
        inline Quaternion<T>& operator*=(T scalar) {
            _vector *= scalar;
            _scalar *= scalar;
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
         * @brief Divide with scalar and assign
         *
         * The computation is done in-place. @f[
         *      \frac q a = [\frac {\boldsymbol q_V} a, \frac {q_S} a]
         * @f]
         */
        inline Quaternion<T>& operator/=(T scalar) {
            _vector /= scalar;
            _scalar /= scalar;
            return *this;
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
         * @f[
         *      p q = [p_S \boldsymbol q_V + q_S \boldsymbol p_V + \boldsymbol p_V \times \boldsymbol q_V,
         *             p_S q_S - \boldsymbol p_V \cdot \boldsymbol q_V]
         * @f]
         */
        inline Quaternion<T> operator*(const Quaternion<T>& other) const {
            return {_scalar*other._vector + other._scalar*_vector + Vector3<T>::cross(_vector, other._vector),
                    _scalar*other._scalar - Vector3<T>::dot(_vector, other._vector)};
        }

        /**
         * @brief Dot product of the quaternion
         *
         * Should be used instead of length() for comparing quaternion length
         * with other values, because it doesn't compute the square root. @f[
         *      q \cdot q = \boldsymbol q_V \cdot \boldsymbol q_V + q_S^2
         * @f]
         * @see dot(const Quaternion<T>&, const Quaternion<T>&)
         */
        inline T dot() const {
            return dot(*this, *this);
        }

        /**
         * @brief %Quaternion length
         *
         * @f[
         *      |q| = \sqrt{q \cdot q}
         * @f]
         * @see dot() const
         */
        inline T length() const {
            return std::sqrt(dot());
        }

        /** @brief Normalized quaternion (of length 1) */
        inline Quaternion<T> normalized() const {
            return (*this)/length();
        }

        /**
         * @brief Conjugated quaternion
         *
         * @f[
         *      q^* = [-\boldsymbol q_V, q_S]
         * @f]
         */
        inline Quaternion<T> conjugated() const {
            return {-_vector, _scalar};
        }

        /**
         * @brief Inverted quaternion
         *
         * See invertedNormalized() which is faster for normalized
         * quaternions. @f[
         *      q^{-1} = \frac{q^*}{|q|^2} = \frac{[-\boldsymbol q_V, q_S]}{q \cdot q}
         * @f]
         */
        inline Quaternion<T> inverted() const {
            return conjugated()/dot();
        }

        /**
         * @brief Inverted normalized quaternion
         *
         * Equivalent to conjugated(). Expects that the quaternion is
         * normalized. @f[
         *      q^{-1} = q^* = [-\boldsymbol q_V, q_S] ~~~~~ |q| = 1
         * @f]
         */
        inline Quaternion<T> invertedNormalized() const {
            CORRADE_ASSERT(MathTypeTraits<T>::equals(dot(), T(1)),
                           "Math::Quaternion::invertedNormalized(): quaternion must be normalized",
                           Quaternion<T>({}, std::numeric_limits<T>::quiet_NaN()));
            return conjugated();
        }

    private:
        Vector3<T> _vector;
        T _scalar;
};

/** @relates Quaternion
@brief Multiply scalar with quaternion

Same as Quaternion::operator*(T) const.
*/
template<class T> inline Quaternion<T> operator*(T scalar, const Quaternion<T>& quaternion) {
    return quaternion*scalar;
}

/** @relates Quaternion
@brief Divide quaternion with number and invert

@f[
    \frac a q = [\frac a {\boldsymbol q_V}, \frac a {q_S}]
@f]
@see Quaternion::operator/()
*/
template<class T> inline Quaternion<T> operator/(T scalar, const Quaternion<T>& quaternion) {
    return {scalar/quaternion.vector(), scalar/quaternion.scalar()};
}

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
