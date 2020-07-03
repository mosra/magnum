#ifndef Magnum_Math_Quaternion_h
#define Magnum_Math_Quaternion_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2019 Marco Melorio <m.melorio@icloud.com>

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
 * @brief Class @ref Magnum::Math::Quaternion, function @ref Magnum::Math::dot(), @ref Magnum::Math::angle(), @ref Magnum::Math::lerp(), @ref Magnum::Math::slerp()
 */

#include <Corrade/Utility/Assert.h>
#ifndef CORRADE_NO_DEBUG
#include <Corrade/Utility/Debug.h>
#endif
#include <Corrade/Utility/StlMath.h>

#include "Magnum/Math/Matrix.h"
#include "Magnum/Math/TypeTraits.h"
#include "Magnum/Math/Vector3.h"

namespace Magnum { namespace Math {

namespace Implementation {
    template<class, class> struct QuaternionConverter;
}

/** @relatesalso Quaternion
@brief Dot product between two quaternions

@f[
     p \cdot q = \boldsymbol p_V \cdot \boldsymbol q_V + p_S q_S
@f]
@see @ref Quaternion::dot() const
*/
template<class T> inline T dot(const Quaternion<T>& a, const Quaternion<T>& b) {
    return dot(a.vector(), b.vector()) + a.scalar()*b.scalar();
}

/** @relatesalso Quaternion
@brief Angle between normalized quaternions

Expects that both quaternions are normalized. @f[
    \theta = \arccos \left( \frac{p \cdot q}{|p| |q|} \right) = \arccos(p \cdot q)
@f]

To avoid numerical issues when two complex numbers are very close to each
other, the dot product is clamped to the @f$ [-1, +1] @f$ range before being
passed to @f$ \arccos @f$.
@see @ref Quaternion::isNormalized(),
    @ref angle(const Complex<T>&, const Complex<T>&),
    @ref angle(const Vector<size, FloatingPoint>&, const Vector<size, FloatingPoint>&)
 */
template<class T> inline Rad<T> angle(const Quaternion<T>& normalizedA, const Quaternion<T>& normalizedB) {
    CORRADE_ASSERT(normalizedA.isNormalized() && normalizedB.isNormalized(),
        "Math::angle(): quaternions" << normalizedA << "and" << normalizedB << "are not normalized", {});
    return Rad<T>{std::acos(clamp(dot(normalizedA, normalizedB), T(-1), T(1)))};
}

/** @relatesalso Quaternion
@brief Linear interpolation of two quaternions
@param normalizedA  First quaternion
@param normalizedB  Second quaternion
@param t            Interpolation phase (from range @f$ [0; 1] @f$)

Expects that both quaternions are normalized. @f[
    q_{LERP} = \frac{(1 - t) q_A + t q_B}{|(1 - t) q_A + t q_B|}
@f]

Note that this function does not check for shortest path interpolation, see
@ref lerpShortestPath(const Quaternion<T>&, const Quaternion<T>&, T) for an
alternative.
@see @ref Quaternion::isNormalized(),
    @ref slerp(const Quaternion<T>&, const Quaternion<T>&, T), @ref sclerp(),
    @ref lerp(const T&, const T&, U),
    @ref lerp(const Complex<T>&, const Complex<T>&, T),
    @ref lerp(const CubicHermite<T>&, const CubicHermite<T>&, U),
    @ref lerp(const CubicHermiteComplex<T>&, const CubicHermiteComplex<T>&, T),
    @ref lerp(const CubicHermiteQuaternion<T>&, const CubicHermiteQuaternion<T>&, T)
*/
template<class T> inline Quaternion<T> lerp(const Quaternion<T>& normalizedA, const Quaternion<T>& normalizedB, T t) {
    CORRADE_ASSERT(normalizedA.isNormalized() && normalizedB.isNormalized(),
        "Math::lerp(): quaternions" << normalizedA << "and" << normalizedB << "are not normalized", {});
    return ((T(1) - t)*normalizedA + t*normalizedB).normalized();
}

/** @relatesalso Quaternion
@brief Linear shortest-path interpolation of two quaternions
@param normalizedA  First quaternion
@param normalizedB  Second quaternion
@param t            Interpolation phase (from range @f$ [0; 1] @f$)

Unlike @ref lerp(const Quaternion<T>&, const Quaternion<T>&, T), this
interpolates on the shortest path at some performance expense. Expects that
both quaternions are normalized. @f[
    \begin{array}{rcl}
        d & = & q_A \cdot q_B \\[5pt]
        q'_A & = & \begin{cases}
                \phantom{-}q_A, & d \ge 0 \\
                -q_A, & d < 0
            \end{cases} \\[15pt]
        q_{LERP} & = & \cfrac{(1 - t) q'_A + t q_B}{|(1 - t) q'_A + t q_B|}
    \end{array}
@f]
@see @ref Quaternion::isNormalized(),
    @ref slerpShortestPath(const Quaternion<T>&, const Quaternion<T>&, T),
    @ref lerpShortestPath(const CubicHermiteQuaternion<T>&, const CubicHermiteQuaternion<T>&, T)
    @ref sclerpShortestPath()
*/
template<class T> inline Quaternion<T> lerpShortestPath(const Quaternion<T>& normalizedA, const Quaternion<T>& normalizedB, T t) {
    return lerp(dot(normalizedA, normalizedB) < T(0) ? -normalizedA : normalizedA, normalizedB, t);
}

/** @relatesalso Quaternion
@brief Spherical linear interpolation of two quaternions
@param normalizedA  First quaternion
@param normalizedB  Second quaternion
@param t            Interpolation phase (from range @f$ [0; 1] @f$)

Expects that both quaternions are normalized. If the quaternions are nearly the
same or one is a negation of the other, it falls back to a linear interpolation
(shortest-path to avoid a degenerate case of returning a zero quaternion for
@f$ t = 0.5 @f$), but without post-normalization as the interpolation result
can still be considered sufficiently normalized: @f[
    \begin{array}{rcl}
        d & = & q_A \cdot q_B \\[5pt]
        q_{SLERP} & = & (1 - t) \left\{ \begin{array}{lr}
            \phantom{-}q_A, & d \ge 0 \\
            -q_A, & d < 0
        \end{array} \right\} + t q_B, ~ {\color{m-primary} \text{if} ~ |d| \ge 1 - \frac{\epsilon}{2}}
    \end{array}
@f]

@m_class{m-noindent}

Otherwise, the interpolation is performed as: @f[
    \begin{array}{rcl}
        \theta & = & \arccos \left( \frac{q_A \cdot q_B}{|q_A| |q_B|} \right) = \arccos(q_A \cdot q_B) = \arccos(d) \\[5pt]
        q_{SLERP} & = & \cfrac{\sin((1 - t) \theta) q_A + \sin(t \theta) q_B}{\sin(\theta)}
    \end{array}
@f]

Note that this function does not check for shortest path interpolation, see
@ref slerpShortestPath(const Quaternion<T>&, const Quaternion<T>&, T) for an
alternative.
@see @ref Quaternion::isNormalized(), @ref lerp(const Quaternion<T>&, const Quaternion<T>&, T),
    @ref slerp(const Complex<T>&, const Complex<T>&, T), @ref sclerp(),
    @ref slerp(const CubicHermiteQuaternion<T>&, const CubicHermiteQuaternion<T>&, T)
*/
template<class T> inline Quaternion<T> slerp(const Quaternion<T>& normalizedA, const Quaternion<T>& normalizedB, T t) {
    CORRADE_ASSERT(normalizedA.isNormalized() && normalizedB.isNormalized(),
        "Math::slerp(): quaternions" << normalizedA << "and" << normalizedB << "are not normalized", {});
    const T cosHalfAngle = dot(normalizedA, normalizedB);

    /* Avoid division by zero if the quats are very close and instead fall back
       to a linear interpolation. This is intentionally not doing any
       normalization as that's not needed. For a maximum angle α satisfying the
       condition below, the two quaternions form two sides of an isosceles
       triangle and its altitude x is length of the "shortest" possible
       interpolated quaternion:

               +
              /|\           cos(α)  > 1 - ε/2
             /α|α\               α  < arccos(1 - ε/2)
            /-_|_-\
         1 /   |   \ 1          x/1 < cos(α)
          /    |x   \           x/1 < cos(arccos(1 - ε/2))
         /     |     \            x < 1 - ε/2
        +------+------+

       Magnum's isNormalized() check treats all lengths in (1 - ε, 1 + ε) as
       normalized, thus for an safety headroom this stops only at 1 - ε/2.
       Additionally this needs to account for the case of the quaternions being
       mutual negatives, in which case a simple lerp() would return a zero
       quaternion for t = 0.5. */
    if(std::abs(cosHalfAngle) > T(1) - T(0.5)*TypeTraits<T>::epsilon()) {
        const Quaternion<T> shortestNormalizedA = cosHalfAngle < 0 ? -normalizedA : normalizedA;
        return (T(1) - t)*shortestNormalizedA + t*normalizedB;
    }

    const T a = std::acos(cosHalfAngle);
    return (std::sin((T(1) - t)*a)*normalizedA + std::sin(t*a)*normalizedB)/std::sin(a);
}

/** @relatesalso Quaternion
@brief Spherical linear shortest-path interpolation of two quaternions
@param normalizedA  First quaternion
@param normalizedB  Second quaternion
@param t            Interpolation phase (from range @f$ [0; 1] @f$)

Unlike @ref slerp(const Quaternion<T>&, const Quaternion<T>&, T) this function
interpolates on the shortest path. Expects that both quaternions are
normalized. If the quaternions are nearly the same or one is a negation of the
other, it falls back to a linear interpolation (shortest-path to avoid a
degenerate case of returning a zero quaternion for @f$ t = 0.5 @f$) but without
post-normalization as the interpolation result can still be considered
sufficiently normalized: @f[
    \begin{array}{rcl}
        d & = & q_A \cdot q_B \\[15pt]
        q'_A & = & \begin{cases}
                \phantom{-}q_A, & d \ge 0 \\
                -q_A, & d < 0
            \end{cases} \\[15pt]
        q_{SLERP} & = & (1 - t) q'_A + t q_B, ~ {\color{m-primary} \text{if} ~ |d| \ge 1 - \frac{\epsilon}{2}}
    \end{array}
@f]

@m_class{m-noindent}

Otherwise, the interpolation is performed as: @f[
    \begin{array}{rcl}
        \theta & = & \arccos \left( \frac{|q'_A \cdot q_B|}{|q'_A| |q_B|} \right) = \arccos(|q'_A \cdot q_B|) = \arccos(|d|) \\[5pt]
        q_{SLERP} & = & \cfrac{\sin((1 - t) \theta) q'_A + \sin(t \theta) q_B}{\sin(\theta)}
    \end{array}
@f]
@see @ref Quaternion::isNormalized(),
    @ref lerpShortestPath(const Quaternion<T>&, const Quaternion<T>&, T),
    @ref slerpShortestPath(const CubicHermiteQuaternion<T>&, const CubicHermiteQuaternion<T>&, T),
    @ref sclerpShortestPath()
*/
template<class T> inline Quaternion<T> slerpShortestPath(const Quaternion<T>& normalizedA, const Quaternion<T>& normalizedB, T t) {
    CORRADE_ASSERT(normalizedA.isNormalized() && normalizedB.isNormalized(),
        "Math::slerpShortestPath(): quaternions" << normalizedA << "and" << normalizedB << "are not normalized", {});
    const T cosHalfAngle = dot(normalizedA, normalizedB);

    const Quaternion<T> shortestNormalizedA = cosHalfAngle < 0 ? -normalizedA : normalizedA;

    /* Avoid division by zero if the quats are very close and instead fall back
       to a linear interpolation. This is intentionally not doing any
       normalization, see slerp() above for more information. */
    if(std::abs(cosHalfAngle) >= T(1) - TypeTraits<T>::epsilon()) {
        return (T(1) - t)*shortestNormalizedA + t*normalizedB;
    }

    const T a = std::acos(std::abs(cosHalfAngle));
    return (std::sin((T(1) - t)*a)*shortestNormalizedA + std::sin(t*a)*normalizedB)/std::sin(a);
}

/**
@brief Quaternion
@tparam T   Underlying data type

Represents 3D rotation. Usually denoted as the following in equations, with
@f$ \boldsymbol{q}_V @f$ being the @ref vector() part and @f$ q_S @f$ being the
@ref scalar() part: @f[
    q = [\boldsymbol{q}_V, q_S]
@f]

See @ref transformations for a brief introduction.
@see @ref Magnum::Quaternion, @ref Magnum::Quaterniond, @ref DualQuaternion,
    @ref Matrix4
*/
template<class T> class Quaternion {
    public:
        typedef T Type; /**< @brief Underlying data type */

        /**
         * @brief Rotation quaternion
         * @param angle             Rotation angle (counterclockwise)
         * @param normalizedAxis    Normalized rotation axis
         *
         * Expects that the rotation axis is normalized. @f[
         *      q = [\boldsymbol a \cdot \sin(\frac{\theta}{2}), \cos(\frac{\theta}{2})]
         * @f]
         * @see @ref angle(), @ref axis(), @ref DualQuaternion::rotation(),
         *      @ref Matrix4::rotation(), @ref Complex::rotation(),
         *      @ref Vector3::xAxis(), @ref Vector3::yAxis(),
         *      @ref Vector3::zAxis(), @ref Vector::isNormalized()
         */
        static Quaternion<T> rotation(Rad<T> angle, const Vector3<T>& normalizedAxis);

        /**
         * @brief Create a quaternion from a rotation matrix
         *
         * Expects that the matrix is orthogonal (i.e. pure rotation).
         * @see @ref toMatrix(), @ref DualComplex::fromMatrix(),
         *      @ref Matrix::isOrthogonal()
         */
        static Quaternion<T> fromMatrix(const Matrix3x3<T>& matrix);

        /**
         * @brief Default constructor
         *
         * Equivalent to @ref Quaternion(IdentityInitT).
         */
        constexpr /*implicit*/ Quaternion() noexcept: _scalar{T(1)} {}

        /**
         * @brief Construct an identity quaternion
         *
         * Creates unit quaternion. @f[
         *      q = [\boldsymbol 0, 1]
         * @f]
         */
        constexpr explicit Quaternion(IdentityInitT) noexcept: _scalar{T(1)} {}

        /** @brief Construct a zero-initialized quaternion */
        constexpr explicit Quaternion(ZeroInitT) noexcept: _vector{ZeroInit}, _scalar{T{0}} {}

        /** @brief Construct without initializing the contents */
        explicit Quaternion(Magnum::NoInitT) noexcept: _vector{Magnum::NoInit} {}

        /**
         * @brief Construct from a vector and a scalar
         *
         * @f[
         *      q = [\boldsymbol v, s]
         * @f]
         */
        constexpr /*implicit*/ Quaternion(const Vector3<T>& vector, T scalar) noexcept: _vector(vector), _scalar(scalar) {}

        /**
         * @brief Construct from a vector
         *
         * To be used in transformations later. @f[
         *      q = [\boldsymbol v, 0]
         * @f]
         * @see @ref transformVector(), @ref transformVectorNormalized()
         */
        constexpr explicit Quaternion(const Vector3<T>& vector) noexcept: _vector(vector), _scalar(T(0)) {}

        /**
         * @brief Construct from a quaternion of different type
         *
         * Performs only default casting on the values, no rounding or anything
         * else.
         */
        template<class U> constexpr explicit Quaternion(const Quaternion<U>& other) noexcept: _vector{other._vector}, _scalar{T(other._scalar)} {}

        /** @brief Construct quaternion from external representation */
        template<class U, class V = decltype(Implementation::QuaternionConverter<T, U>::from(std::declval<U>()))> constexpr explicit Quaternion(const U& other): Quaternion{Implementation::QuaternionConverter<T, U>::from(other)} {}

        /** @brief Copy constructor */
        constexpr /*implicit*/ Quaternion(const Quaternion<T>&) noexcept = default;

        /** @brief Convert quaternion to external representation */
        template<class U, class V = decltype(Implementation::QuaternionConverter<T, U>::to(std::declval<Quaternion<T>>()))> constexpr explicit operator U() const {
            return Implementation::QuaternionConverter<T, U>::to(*this);
        }

        /**
         * @brief Raw data
         *
         * Returns one-dimensional array of four elements, vector part first,
         * scalar after.
         * @see @ref vector(), @ref scalar()
         */
        T* data() { return _vector.data(); }
        constexpr const T* data() const { return _vector.data(); } /**< @overload */

        /** @brief Equality comparison */
        bool operator==(const Quaternion<T>& other) const {
            return _vector == other._vector && TypeTraits<T>::equals(_scalar, other._scalar);
        }

        /** @brief Non-equality comparison */
        bool operator!=(const Quaternion<T>& other) const {
            return !operator==(other);
        }

        /**
         * @brief Whether the quaternion is normalized
         *
         * Quaternion is normalized if it has unit length: @f[
         *      |q \cdot q - 1| < 2 \epsilon + \epsilon^2 \cong 2 \epsilon
         * @f]
         * @see @ref dot(), @ref normalized()
         */
        bool isNormalized() const {
            return Implementation::isNormalizedSquared(dot());
        }

        /** @brief Vector part (@f$ \boldsymbol{q}_V @f$) */
        Vector3<T>& vector() { return _vector; }
        /* Returning const so it's possible to call constexpr functions on the
           result. WTF, C++?! */
        constexpr const Vector3<T> vector() const { return _vector; } /**< @overload */

        /** @brief Scalar part (@f$ q_S @f$) */
        T& scalar() { return _scalar; }
        constexpr T scalar() const { return _scalar; } /**< @overload */

        /**
         * @brief Rotation angle of a unit quaternion
         *
         * Expects that the quaternion is normalized. @f[
         *      \theta = 2 \cdot \arccos(q_S)
         * @f]
         * @see @ref isNormalized(), @ref axis(), @ref rotation()
         */
        Rad<T> angle() const;

        /**
         * @brief Rotation axis of a unit quaternion
         *
         * Expects that the quaternion is normalized. Returns either unit-length
         * vector for valid rotation quaternion or NaN vector for
         * default-constructed quaternion. @f[
         *      \boldsymbol a = \frac{\boldsymbol q_V}{\sqrt{1 - q_S^2}}
         * @f]
         * @see @ref isNormalized(), @ref angle(), @ref rotation()
         */
        Vector3<T> axis() const;

        /**
         * @brief Convert to a rotation matrix
         *
         * @see @ref fromMatrix(), @ref DualQuaternion::toMatrix(),
         *      @ref Matrix4::from(const Matrix3x3<T>&, const Vector3<T>&)
         */
        Matrix3x3<T> toMatrix() const;

        /**
         * @brief Convert to an euler vector
         * @m_since{2020,06}
         *
         * Expects that the quaternion is normalized. Returns the angles in an
         * XYZ order, you can combine them back to a quaternion like this:
         *
         * @snippet MagnumMath.cpp Quaternion-fromEuler
         *
         * @see @ref rotation(), @ref DualQuaternion::rotation()
         */
        Vector3<Rad<T>> toEuler() const;

        /**
         * @brief Negated quaternion
         *
         * @f[
         *      -q = [-\boldsymbol q_V, -q_S]
         * @f]
         */
        Quaternion<T> operator-() const { return {-_vector, -_scalar}; }

        /**
         * @brief Add and assign a quaternion
         *
         * The computation is done in-place. @f[
         *      p + q = [\boldsymbol p_V + \boldsymbol q_V, p_S + q_S]
         * @f]
         */
        Quaternion<T>& operator+=(const Quaternion<T>& other) {
            _vector += other._vector;
            _scalar += other._scalar;
            return *this;
        }

        /**
         * @brief Add a quaternion
         *
         * @see @ref operator+=()
         */
        Quaternion<T> operator+(const Quaternion<T>& other) const {
            return Quaternion<T>(*this) += other;
        }

        /**
         * @brief Subtract and assign a quaternion
         *
         * The computation is done in-place. @f[
         *      p - q = [\boldsymbol p_V - \boldsymbol q_V, p_S - q_S]
         * @f]
         */
        Quaternion<T>& operator-=(const Quaternion<T>& other) {
            _vector -= other._vector;
            _scalar -= other._scalar;
            return *this;
        }

        /**
         * @brief Subtract a quaternion
         *
         * @see @ref operator-=()
         */
        Quaternion<T> operator-(const Quaternion<T>& other) const {
            return Quaternion<T>(*this) -= other;
        }

        /**
         * @brief Multiply with a scalar and assign
         *
         * The computation is done in-place. @f[
         *      q \cdot a = [\boldsymbol q_V \cdot a, q_S \cdot a]
         * @f]
         */
        Quaternion<T>& operator*=(T scalar) {
            _vector *= scalar;
            _scalar *= scalar;
            return *this;
        }

        /**
         * @brief Multiply with a scalar
         *
         * @see @ref operator*=(T)
         */
        Quaternion<T> operator*(T scalar) const {
            return Quaternion<T>(*this) *= scalar;
        }

        /**
         * @brief Divide with a scalar and assign
         *
         * The computation is done in-place. @f[
         *      \frac q a = [\frac {\boldsymbol q_V} a, \frac {q_S} a]
         * @f]
         */
        Quaternion<T>& operator/=(T scalar) {
            _vector /= scalar;
            _scalar /= scalar;
            return *this;
        }

        /**
         * @brief Divide with a scalar
         *
         * @see @ref operator/=(T)
         */
        Quaternion<T> operator/(T scalar) const {
            return Quaternion<T>(*this) /= scalar;
        }

        /**
         * @brief Multiply with a quaternion
         *
         * @f[
         *      p q = [p_S \boldsymbol q_V + q_S \boldsymbol p_V + \boldsymbol p_V \times \boldsymbol q_V,
         *             p_S q_S - \boldsymbol p_V \cdot \boldsymbol q_V]
         * @f]
         */
        Quaternion<T> operator*(const Quaternion<T>& other) const;

        /**
         * @brief Dot product of the quaternion
         *
         * Should be used instead of @ref length() for comparing quaternion
         * length with other values, because it doesn't compute the square
         * root. @f[
         *      q \cdot q = \boldsymbol q_V \cdot \boldsymbol q_V + q_S^2
         * @f]
         * @see @ref isNormalized(),
         *      @ref dot(const Quaternion<T>&, const Quaternion<T>&)
         */
        T dot() const { return Math::dot(*this, *this); }

        /**
         * @brief Quaternion length
         *
         * See also @ref dot() const which is faster for comparing length with
         * other values. @f[
         *      |q| = \sqrt{q \cdot q}
         * @f]
         * @see @ref isNormalized()
         */
        T length() const { return std::sqrt(dot()); }

        /**
         * @brief Normalized quaternion (of unit length)
         *
         * @see @ref isNormalized()
         */
        Quaternion<T> normalized() const { return (*this)/length(); }

        /**
         * @brief Conjugated quaternion
         *
         * @f[
         *      q^* = [-\boldsymbol q_V, q_S]
         * @f]
         */
        Quaternion<T> conjugated() const { return {-_vector, _scalar}; }

        /**
         * @brief Inverted quaternion
         *
         * See @ref invertedNormalized() which is faster for normalized
         * quaternions. @f[
         *      q^{-1} = \frac{q^*}{|q|^2} = \frac{q^*}{q \cdot q}
         * @f]
         */
        Quaternion<T> inverted() const { return conjugated()/dot(); }

        /**
         * @brief Inverted normalized quaternion
         *
         * Equivalent to @ref conjugated(). Expects that the quaternion is
         * normalized. @f[
         *      q^{-1} = \frac{q^*}{|q|^2} = q^*
         * @f]
         * @see @ref isNormalized(), @ref inverted()
         */
        Quaternion<T> invertedNormalized() const;

        /**
         * @brief Rotate a vector with a quaternion
         *
         * See @ref transformVectorNormalized(), which is faster for normalized
         * quaternions. @f[
         *      v' = qvq^{-1} = q [\boldsymbol v, 0] q^{-1}
         * @f]
         * @see @ref Quaternion(const Vector3<T>&), @ref vector(),
         *      @ref Matrix4::transformVector(),
         *      @ref DualQuaternion::transformPoint(),
         *      @ref Complex::transformVector()
         */
        Vector3<T> transformVector(const Vector3<T>& vector) const {
            return ((*this)*Quaternion<T>(vector)*inverted()).vector();
        }

        /**
         * @brief Rotate a vector with a normalized quaternion
         *
         * Faster alternative to @ref transformVector(), expects that the
         * quaternion is normalized. Done using the following equation: @f[
         *      \begin{array}{rcl}
         *          \boldsymbol t & = & 2 (\boldsymbol q_V \times \boldsymbol v) \\
         *          \boldsymbol v' & = & \boldsymbol v + q_S \boldsymbol t + \boldsymbol q_V \times \boldsymbol t
         *      \end{array}
         * @f]
         * Which is equivalent to the common equation (source:
         * https://molecularmusings.wordpress.com/2013/05/24/a-faster-quaternion-vector-multiplication/): @f[
         *      v' = qvq^{-1} = qvq^* = q [\boldsymbol v, 0] q^*
         * @f]
         * @see @ref isNormalized(), @ref Quaternion(const Vector3<T>&),
         *      @ref vector(), @ref Matrix4::transformVector(),
         *      @ref DualQuaternion::transformPointNormalized(),
         *      @ref Complex::transformVector()
         */
        Vector3<T> transformVectorNormalized(const Vector3<T>& vector) const;

    private:
        #ifndef DOXYGEN_GENERATING_OUTPUT
        /* Doxygen copies the description from Magnum::Quaternion here. Ugh. */
        template<class> friend class Quaternion;
        #endif

        /* Used to avoid including Functions.h */
        constexpr static T pow2(T value) {
            return value*value;
        }

        Vector3<T> _vector;
        T _scalar;
};

/** @relates Quaternion
@brief Multiply a scalar with a quaternion

Same as @ref Quaternion::operator*(T) const.
*/
template<class T> inline Quaternion<T> operator*(T scalar, const Quaternion<T>& quaternion) {
    return quaternion*scalar;
}

/** @relates Quaternion
@brief Divide a quaternion with a scalar and invert

@f[
    \frac a q = [\frac a {\boldsymbol q_V}, \frac a {q_S}]
@f]
@see @ref Quaternion::operator/()
*/
template<class T> inline Quaternion<T> operator/(T scalar, const Quaternion<T>& quaternion) {
    return {scalar/quaternion.vector(), scalar/quaternion.scalar()};
}

#ifndef CORRADE_NO_DEBUG
/** @debugoperator{Quaternion} */
template<class T> Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug& debug, const Quaternion<T>& value) {
    return debug << "Quaternion({" << Corrade::Utility::Debug::nospace
        << value.vector().x() << Corrade::Utility::Debug::nospace << ","
        << value.vector().y() << Corrade::Utility::Debug::nospace << ","
        << value.vector().z() << Corrade::Utility::Debug::nospace << "},"
        << value.scalar() << Corrade::Utility::Debug::nospace << ")";
}

/* Explicit instantiation for commonly used types */
#ifndef DOXYGEN_GENERATING_OUTPUT
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const Quaternion<Float>&);
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const Quaternion<Double>&);
#endif
#endif

namespace Implementation {

/* No assertions fired, for internal use. Not private member because used from
   outside the class. */
template<class T> Quaternion<T> quaternionFromMatrix(const Matrix3x3<T>& m) {
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

template<class T> inline Quaternion<T> Quaternion<T>::rotation(const Rad<T> angle, const Vector3<T>& normalizedAxis) {
    CORRADE_ASSERT(normalizedAxis.isNormalized(),
        "Math::Quaternion::rotation(): axis" << normalizedAxis << "is not normalized", {});
    return {normalizedAxis*std::sin(T(angle)/2), std::cos(T(angle)/2)};
}

template<class T> inline Quaternion<T> Quaternion<T>::fromMatrix(const Matrix3x3<T>& matrix) {
    CORRADE_ASSERT(matrix.isOrthogonal(),
        "Math::Quaternion::fromMatrix(): the matrix is not orthogonal:" << Corrade::Utility::Debug::newline << matrix, {});
    return Implementation::quaternionFromMatrix(matrix);
}

template<class T> inline Rad<T> Quaternion<T>::angle() const {
    CORRADE_ASSERT(isNormalized(),
        "Math::Quaternion::angle():" << *this << "is not normalized", {});
    return Rad<T>(T(2)*std::acos(_scalar));
}

template<class T> inline Vector3<T> Quaternion<T>::axis() const {
    CORRADE_ASSERT(isNormalized(),
        "Math::Quaternion::axis():" << *this << "is not normalized", {});
    return _vector/std::sqrt(1-pow2(_scalar));
}

template<class T> Matrix3x3<T> Quaternion<T>::toMatrix() const {
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

/* Algorithm from:
   https://github.com/mrdoob/three.js/blob/6892dd0aba1411d35c5e2b44dc6ff280b24d6aa2/src/math/Euler.js#L197 */
template<class T> Vector3<Rad<T>> Quaternion<T>::toEuler() const {
    CORRADE_ASSERT(isNormalized(),
        "Math::Quaternion::toEuler():" << *this << "is not normalized", {});

    Vector3<Rad<T>> euler{Magnum::NoInit};

    Matrix3x3<T> rotMatrix = toMatrix();

    T m11 = rotMatrix[0][0];
    T m12 = rotMatrix[0][1];
    T m13 = rotMatrix[0][2];
    T m21 = rotMatrix[1][0];
    T m22 = rotMatrix[1][1];
    T m23 = rotMatrix[1][2];
    T m33 = rotMatrix[2][2];

    euler.y() = Rad<T>(std::asin(-Math::min(Math::max(m13, T(-1.0)), T(1.0))));

    if(!TypeTraits<T>::equalsZero(m13 - T(1.0), T(1.0))) {
        euler.x() = Rad<T>(std::atan2(m23, m33));
        euler.z() = Rad<T>(std::atan2(m12, m11));
    } else {
        euler.x() = Rad<T>(0.0);
        euler.z() = Rad<T>(std::atan2(-m21, m22));
    }

    return euler;
}

template<class T> inline Quaternion<T> Quaternion<T>::operator*(const Quaternion<T>& other) const {
    return {_scalar*other._vector + other._scalar*_vector + Math::cross(_vector, other._vector),
            _scalar*other._scalar - Math::dot(_vector, other._vector)};
}

template<class T> inline Quaternion<T> Quaternion<T>::invertedNormalized() const {
    CORRADE_ASSERT(isNormalized(),
        "Math::Quaternion::invertedNormalized():" << *this << "is not normalized", {});
    return conjugated();
}

template<class T> inline Vector3<T> Quaternion<T>::transformVectorNormalized(const Vector3<T>& vector) const {
    CORRADE_ASSERT(isNormalized(),
        "Math::Quaternion::transformVectorNormalized():" << *this << "is not normalized", {});
    const Vector3<T> t = T(2)*Math::cross(_vector, vector);
    return vector + _scalar*t + Math::cross(_vector, t);
}

namespace Implementation {

template<class T> struct StrictWeakOrdering<Quaternion<T>> {
    bool operator()(const Quaternion<T>& a, const Quaternion<T>& b) const {
        StrictWeakOrdering<Vector3<T>> o;
        if(o(a.vector(), b.vector()))
            return true;
        if(o(b.vector(), a.vector()))
            return false;

        return a.scalar() < b.scalar();
    }
};

}

}}

#endif
