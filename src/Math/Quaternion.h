#ifndef Magnum_Math_Quaternion_h
#define Magnum_Math_Quaternion_h
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
 * @brief Class Magnum::Math::Quaternion
 */

#include <cmath>
#include <Utility/Assert.h>
#include <Utility/Debug.h>

#include "Math/TypeTraits.h"
#include "Math/Matrix.h"
#include "Math/Vector3.h"

namespace Magnum { namespace Math {

namespace Implementation {

/* No assertions fired, for internal use. Not private member because used from
   outside the class. */
template<class T> inline Quaternion<T> quaternionFromMatrix(const Matrix<3, T>& m) {
    const Vector<3, T> diagonal = m.diagonal();
    const T trace = diagonal.sum();

    /* Diagonal is positive */
    if(trace > T(0)) {
        const T s = std::sqrt(trace + T(1));
        const T t = T(0.5)/s;
        return {Vector3<T>(m[1][2] - m[2][1],
                           m[2][0] - m[0][2],
                           m[0][1] - m[1][0])*t, s*T(0.5)};
    }

    /* Diagonal is negative */
    std::size_t i = 0;
    if(diagonal[1] > diagonal[0]) i = 1;
    if(diagonal[2] > diagonal[i]) i = 2;

    const std::size_t j = (i + 1) % 3;
    const std::size_t k = (i + 2) % 3;

    const T s = std::sqrt(diagonal[i] - diagonal[j] - diagonal[k] + T(1));
    const T t = (s == T(0) ? T(0) : T(0.5)/s);

    Vector3<T> vec;
    vec[i] = s*T(0.5);
    vec[j] = (m[i][j] + m[j][i])*t;
    vec[k] = (m[i][k] + m[k][i])*t;

    return {vec, (m[j][k] - m[k][j])*t};
}

}

/**
@brief %Quaternion
@tparam T   Underlying data type

Represents 3D rotation. See @ref transformations for brief introduction.
@see Magnum::Quaternion, Magnum::Quaterniond, DualQuaternion, Matrix4
*/
template<class T> class Quaternion {
    public:
        typedef T Type;                         /**< @brief Underlying data type */

        /**
         * @brief Dot product
         *
         * @f[
         *      p \cdot q = \boldsymbol p_V \cdot \boldsymbol q_V + p_S q_S
         * @f]
         * @see dot() const
         */
        inline static T dot(const Quaternion<T>& a, const Quaternion<T>& b) {
            /** @todo Use four-component SIMD implementation when available */
            return Vector3<T>::dot(a.vector(), b.vector()) + a.scalar()*b.scalar();
        }

        /**
         * @brief Angle between normalized quaternions
         *
         * Expects that both quaternions are normalized. @f[
         *      \theta = acos \left( \frac{p \cdot q}{|p| |q|} \right) = acos(p \cdot q)
         * @f]
         * @see isNormalized(), Complex::angle(), Vector::angle()
         */
        inline static Rad<T> angle(const Quaternion<T>& normalizedA, const Quaternion<T>& normalizedB) {
            CORRADE_ASSERT(normalizedA.isNormalized() && normalizedB.isNormalized(),
                           "Math::Quaternion::angle(): quaternions must be normalized", Rad<T>(std::numeric_limits<T>::quiet_NaN()));
            return Rad<T>(angleInternal(normalizedA, normalizedB));
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
         * @see isNormalized(), slerp(), Math::lerp()
         */
        inline static Quaternion<T> lerp(const Quaternion<T>& normalizedA, const Quaternion<T>& normalizedB, T t) {
            CORRADE_ASSERT(normalizedA.isNormalized() && normalizedB.isNormalized(),
                           "Math::Quaternion::lerp(): quaternions must be normalized",
                           Quaternion<T>({}, std::numeric_limits<T>::quiet_NaN()));
            return ((T(1) - t)*normalizedA + t*normalizedB).normalized();
        }

        /**
         * @brief Spherical linear interpolation of two quaternions
         * @param normalizedA   First quaternion
         * @param normalizedB   Second quaternion
         * @param t             Interpolation phase (from range @f$ [0; 1] @f$)
         *
         * Expects that both quaternions are normalized. @f[
         *      q_{SLERP} = \frac{sin((1 - t) \theta) q_A + sin(t \theta) q_B}{sin \theta}
         *      ~~~~~~~~~~
         *      \theta = acos \left( \frac{q_A \cdot q_B}{|q_A| \cdot |q_B|} \right) = acos(q_A \cdot q_B)
         * @f]
         * @see isNormalized(), lerp()
         */
        inline static Quaternion<T> slerp(const Quaternion<T>& normalizedA, const Quaternion<T>& normalizedB, T t) {
            CORRADE_ASSERT(normalizedA.isNormalized() && normalizedB.isNormalized(),
                           "Math::Quaternion::slerp(): quaternions must be normalized",
                           Quaternion<T>({}, std::numeric_limits<T>::quiet_NaN()));
            T a = angleInternal(normalizedA, normalizedB);
            return (std::sin((T(1) - t)*a)*normalizedA + std::sin(t*a)*normalizedB)/std::sin(a);
        }

        /**
         * @brief Rotation quaternion
         * @param angle             Rotation angle (counterclockwise)
         * @param normalizedAxis    Normalized rotation axis
         *
         * Expects that the rotation axis is normalized. @f[
         *      q = [\boldsymbol a \cdot sin \frac \theta 2, cos \frac \theta 2]
         * @f]
         * @see angle(), axis(), DualQuaternion::rotation(),
         *      Matrix4::rotation(), Complex::rotation(), Vector3::xAxis(),
         *      Vector3::yAxis(), Vector3::zAxis(), Vector::isNormalized()
         */
        inline static Quaternion<T> rotation(Rad<T> angle, const Vector3<T>& normalizedAxis) {
            CORRADE_ASSERT(normalizedAxis.isNormalized(),
                           "Math::Quaternion::rotation(): axis must be normalized", {});

            return {normalizedAxis*std::sin(T(angle)/2), std::cos(T(angle)/2)};
        }

        /**
         * @brief Create quaternion from rotation matrix
         *
         * Expects that the matrix is orthogonal (i.e. pure rotation).
         * @see toMatrix(), DualComplex::fromMatrix(), Matrix::isOrthogonal()
         */
        inline static Quaternion<T> fromMatrix(const Matrix<3, T>& matrix) {
            CORRADE_ASSERT(matrix.isOrthogonal(),
                "Math::Quaternion::fromMatrix(): the matrix is not orthogonal", {});
            return Implementation::quaternionFromMatrix(matrix);
        }

        /**
         * @brief Default constructor
         *
         * Creates unit quaternion. @f[
         *      q = [\boldsymbol 0, 1]
         * @f]
         */
        inline constexpr /*implicit*/ Quaternion(): _scalar(T(1)) {}

        /**
         * @brief Construct quaternion from vector and scalar
         *
         * @f[
         *      q = [\boldsymbol v, s]
         * @f]
         */
        inline constexpr /*implicit*/ Quaternion(const Vector3<T>& vector, T scalar): _vector(vector), _scalar(scalar) {}

        /**
         * @brief Construct quaternion from vector
         *
         * To be used in transformations later. @f[
         *      q = [\boldsymbol v, 0]
         * @f]
         * @see transformVector(), transformVectorNormalized()
         */
        inline constexpr explicit Quaternion(const Vector3<T>& vector): _vector(vector), _scalar(T(0)) {}

        /** @brief Equality comparison */
        inline bool operator==(const Quaternion<T>& other) const {
            return _vector == other._vector && TypeTraits<T>::equals(_scalar, other._scalar);
        }

        /** @brief Non-equality comparison */
        inline bool operator!=(const Quaternion<T>& other) const {
            return !operator==(other);
        }

        /**
         * @brief Whether the quaternion is normalized
         *
         * Quaternion is normalized if it has unit length: @f[
         *      |q|^2 = |q| = 1
         * @f]
         * @see dot(), normalized()
         */
        inline bool isNormalized() const {
            return Implementation::isNormalizedSquared(dot());
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
         * @see isNormalized(), axis(), rotation()
         */
        inline Rad<T> angle() const {
            CORRADE_ASSERT(isNormalized(),
                           "Math::Quaternion::angle(): quaternion must be normalized",
                           Rad<T>(std::numeric_limits<T>::quiet_NaN()));
            return Rad<T>(T(2)*std::acos(_scalar));
        }

        /**
         * @brief Rotation axis of unit quaternion
         *
         * Expects that the quaternion is normalized. Returns either unit-length
         * vector for valid rotation quaternion or NaN vector for
         * default-constructed quaternion. @f[
         *      \boldsymbol a = \frac{\boldsymbol q_V}{\sqrt{1 - q_S^2}}
         * @f]
         * @see isNormalized(), angle(), rotation()
         */
        inline Vector3<T> axis() const {
            CORRADE_ASSERT(isNormalized(),
                           "Math::Quaternion::axis(): quaternion must be normalized",
                           {});
            return _vector/std::sqrt(1-pow2(_scalar));
        }

        /**
         * @brief Convert quaternion to rotation matrix
         *
         * @see fromMatrix(), DualQuaternion::toMatrix(),
         *      Matrix4::from(const Matrix<3, T>&, const Vector3<T>&)
         */
        Matrix<3, T> toMatrix() const {
            return {
                Vector<3, T>(T(1) - 2*pow2(_vector.y()) - 2*pow2(_vector.z()),
                    2*_vector.x()*_vector.y() + 2*_vector.z()*_scalar,
                        2*_vector.x()*_vector.z() - 2*_vector.y()*_scalar),
                Vector<3, T>(2*_vector.x()*_vector.y() - 2*_vector.z()*_scalar,
                    T(1) - 2*pow2(_vector.x()) - 2*pow2(_vector.z()),
                        2*_vector.y()*_vector.z() + 2*_vector.x()*_scalar),
                Vector<3, T>(2*_vector.x()*_vector.z() + 2*_vector.y()*_scalar,
                    2*_vector.y()*_vector.z() - 2*_vector.x()*_scalar,
                        T(1) - 2*pow2(_vector.x()) - 2*pow2(_vector.y()))
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
            return Quaternion<T>(*this) += other;
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
            return Quaternion<T>(*this) -= other;
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
            return Quaternion<T>(*this) *= scalar;
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
            return Quaternion<T>(*this) /= scalar;
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
         * @see isNormalized(), dot(const Quaternion&, const Quaternion&)
         */
        inline T dot() const {
            return dot(*this, *this);
        }

        /**
         * @brief %Quaternion length
         *
         * See also dot() const which is faster for comparing length with other
         * values. @f[
         *      |q| = \sqrt{q \cdot q}
         * @f]
         * @see isNormalized()
         */
        inline T length() const {
            return std::sqrt(dot());
        }

        /**
         * @brief Normalized quaternion (of unit length)
         *
         * @see isNormalized()
         */
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
         *      q^{-1} = \frac{q^*}{|q|^2} = \frac{q^*}{q \cdot q}
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
         *      q^{-1} = \frac{q^*}{|q|^2} = q^*
         * @f]
         * @see isNormalized(), inverted()
         */
        inline Quaternion<T> invertedNormalized() const {
            CORRADE_ASSERT(isNormalized(),
                           "Math::Quaternion::invertedNormalized(): quaternion must be normalized",
                           Quaternion<T>({}, std::numeric_limits<T>::quiet_NaN()));
            return conjugated();
        }

        /**
         * @brief Rotate vector with quaternion
         *
         * See transformVectorNormalized(), which is faster for normalized
         * quaternions. @f[
         *      v' = qvq^{-1} = q [\boldsymbol v, 0] q^{-1}
         * @f]
         * @see Quaternion(const Vector3&), vector(), Matrix4::transformVector(),
         *      DualQuaternion::transformPoint(), Complex::transformVector()
         */
        inline Vector3<T> transformVector(const Vector3<T>& vector) const {
            return ((*this)*Quaternion<T>(vector)*inverted()).vector();
        }

        /**
         * @brief Rotate vector with normalized quaternion
         *
         * Faster alternative to transformVector(), expects that the quaternion
         * is normalized. @f[
         *      v' = qvq^{-1} = qvq^* = q [\boldsymbol v, 0] q^*
         * @f]
         * @see isNormalized(), Quaternion(const Vector3&), vector(), Matrix4::transformVector(),
         *      DualQuaternion::transformPointNormalized(), Complex::transformVector()
         */
        inline Vector3<T> transformVectorNormalized(const Vector3<T>& vector) const {
            CORRADE_ASSERT(isNormalized(),
                           "Math::Quaternion::transformVectorNormalized(): quaternion must be normalized",
                           Vector3<T>(std::numeric_limits<T>::quiet_NaN()));
            return ((*this)*Quaternion<T>(vector)*conjugated()).vector();
        }

    private:
        /* Used to avoid including Functions.h */
        inline constexpr static T pow2(T value) {
            return value*value;
        }

        /* Used in angle() and slerp() (no assertions) */
        inline static T angleInternal(const Quaternion<T>& normalizedA, const Quaternion<T>& normalizedB) {
            return std::acos(dot(normalizedA, normalizedB));
        }

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

/** @debugoperator{Magnum::Math::Quaternion} */
template<class T> Corrade::Utility::Debug operator<<(Corrade::Utility::Debug debug, const Quaternion<T>& value) {
    debug << "Quaternion({";
    debug.setFlag(Corrade::Utility::Debug::SpaceAfterEachValue, false);
    debug << value.vector().x() << ", " << value.vector().y() << ", " << value.vector().z() << "}, " << value.scalar() << ")";
    debug.setFlag(Corrade::Utility::Debug::SpaceAfterEachValue, true);
    return debug;
}

/* Explicit instantiation for commonly used types */
#ifndef DOXYGEN_GENERATING_OUTPUT
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const Quaternion<Float>&);
#ifndef MAGNUM_TARGET_GLES
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const Quaternion<Double>&);
#endif
#endif

}}

#endif
