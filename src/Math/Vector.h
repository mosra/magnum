#ifndef Magnum_Math_Vector_h
#define Magnum_Math_Vector_h
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
 * @brief Class Magnum::Math::Vector
 */

#include <cmath>
#include <limits>
#include <Utility/Assert.h>
#include <Utility/Debug.h>
#include <Utility/ConfigurationValue.h>

#include "Math/Angle.h"
#include "Math/BoolVector.h"
#include "Math/TypeTraits.h"

#include "magnumVisibility.h"

namespace Magnum { namespace Math {

namespace Implementation {
    template<std::size_t, class, class> struct VectorConverter;
}

/**
@brief %Vector
@tparam size    %Vector size
@tparam T       Underlying data type

See @ref matrix-vector for brief introduction.
@configurationvalueref{Magnum::Math::Vector}
*/
template<std::size_t size, class T> class Vector {
    static_assert(size != 0, "Vector cannot have zero elements");

    template<std::size_t, class> friend class Vector;

    public:
        typedef T Type;                         /**< @brief Underlying data type */
        const static std::size_t Size = size;   /**< @brief %Vector size */

        /**
         * @brief %Vector from array
         * @return Reference to the data as if it was Vector, thus doesn't
         *      perform any copying.
         *
         * @attention Use with caution, the function doesn't check whether the
         *      array is long enough.
         */
        constexpr static Vector<size, T>& from(T* data) {
            return *reinterpret_cast<Vector<size, T>*>(data);
        }
        /** @overload */
        constexpr static const Vector<size, T>& from(const T* data) {
            return *reinterpret_cast<const Vector<size, T>*>(data);
        }

        /**
         * @brief Dot product
         *
         * Returns `0` if two vectors are orthogonal, `1` if two *normalized*
         * vectors are parallel and `-1` if two *normalized* vectors are
         * antiparallel. @f[
         *      \boldsymbol a \cdot \boldsymbol b = \sum_{i=0}^{n-1} \boldsymbol a_i \boldsymbol b_i
         * @f]
         * @see dot() const, operator-(), Vector2::perpendicular()
         */
        static T dot(const Vector<size, T>& a, const Vector<size, T>& b) {
            return (a*b).sum();
        }

        /**
         * @brief Angle between normalized vectors
         *
         * Expects that both vectors are normalized. @f[
         *      \theta = acos \left( \frac{\boldsymbol a \cdot \boldsymbol b}{|\boldsymbol a| |\boldsymbol b|} \right) = acos (\boldsymbol a \cdot \boldsymbol b)
         * @f]
         * @see isNormalized(), Quaternion::angle(), Complex::angle()
         */
        static Rad<T> angle(const Vector<size, T>& normalizedA, const Vector<size, T>& normalizedB);

        /**
         * @brief Default constructor
         *
         * @f[
         *      \boldsymbol v = \boldsymbol 0
         * @f]
         */
        constexpr /*implicit*/ Vector(): _data() {}

        /** @todo Creating Vector from combination of vector and scalar types */

        /**
         * @brief Construct vector from values
         * @param first First value
         * @param next  Next values
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        template<class ...U> constexpr /*implicit*/ Vector(T first, U... next);
        #else
        template<class ...U, class V = typename std::enable_if<sizeof...(U)+1 == size, T>::type> constexpr /*implicit*/ Vector(T first, U... next): _data{first, next...} {}
        #endif

        /** @brief Construct vector with one value for all fields */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        constexpr explicit Vector(T value);
        #else
        #ifndef CORRADE_GCC46_COMPATIBILITY
        template<class U, class V = typename std::enable_if<std::is_same<T, U>::value && size != 1, T>::type> constexpr explicit Vector(U value): Vector(typename Implementation::GenerateSequence<size>::Type(), value) {}
        #else
        template<class U, class V = typename std::enable_if<std::is_same<T, U>::value && size != 1, T>::type> explicit Vector(U value) {
            *this = Vector(typename Implementation::GenerateSequence<size>::Type(), value);
        }
        #endif
        #endif

        /**
         * @brief Construct vector from another of different type
         *
         * Performs only default casting on the values, no rounding or
         * anything else. Example usage:
         * @code
         * Vector<4, Float> floatingPoint(1.3f, 2.7f, -15.0f, 7.0f);
         * Vector<4, Byte> integral(floatingPoint);
         * // integral == {1, 2, -15, 7}
         * @endcode
         */
        #ifndef CORRADE_GCC46_COMPATIBILITY
        template<class U> constexpr explicit Vector(const Vector<size, U>& other): Vector(typename Implementation::GenerateSequence<size>::Type(), other) {}
        #else
        template<class U> explicit Vector(const Vector<size, U>& other) {
            *this = Vector(typename Implementation::GenerateSequence<size>::Type(), other);
        }
        #endif

        /** @brief Construct vector from external representation */
        #ifndef CORRADE_GCC46_COMPATIBILITY
        template<class U, class V = decltype(Implementation::VectorConverter<size, T, U>::from(std::declval<U>()))> constexpr explicit Vector(const U& other): Vector(Implementation::VectorConverter<size, T, U>::from(other)) {}
        #else
        template<class U, class V = decltype(Implementation::VectorConverter<size, T, U>::from(std::declval<U>()))> explicit Vector(const U& other) {
            *this = Implementation::VectorConverter<size, T, U>::from(other);
        }
        #endif

        /** @brief Copy constructor */
        constexpr Vector(const Vector<size, T>&) = default;

        /** @brief Assignment operator */
        Vector<size, T>& operator=(const Vector<size, T>&) = default;

        /** @brief Convert vector to external representation */
        template<class U, class V = decltype(Implementation::VectorConverter<size, T, U>::to(std::declval<Vector<size, T>>()))> constexpr explicit operator U() const {
            /** @bug Why this is not constexpr under GCC 4.6? */
            return Implementation::VectorConverter<size, T, U>::to(*this);
        }

        /**
         * @brief Raw data
         * @return One-dimensional array of `size` length.
         *
         * @see operator[]()
         */
        T* data() { return _data; }
        constexpr const T* data() const { return _data; } /**< @overload */

        /**
         * @brief Value at given position
         *
         * @see data()
         */
        T& operator[](std::size_t pos) { return _data[pos]; }
        constexpr T operator[](std::size_t pos) const { return _data[pos]; } /**< @overload */

        /** @brief Equality comparison */
        bool operator==(const Vector<size, T>& other) const {
            for(std::size_t i = 0; i != size; ++i)
                if(!TypeTraits<T>::equals(_data[i], other._data[i])) return false;

            return true;
        }

        /** @brief Non-equality comparison */
        bool operator!=(const Vector<size, T>& other) const {
            return !operator==(other);
        }

        /** @brief Component-wise less than */
        BoolVector<size> operator<(const Vector<size, T>& other) const;

        /** @brief Component-wise less than or equal */
        BoolVector<size> operator<=(const Vector<size, T>& other) const;

        /** @brief Component-wise greater than or equal */
        BoolVector<size> operator>=(const Vector<size, T>& other) const;

        /** @brief Component-wise greater than */
        BoolVector<size> operator>(const Vector<size, T>& other) const;

        /**
         * @brief Whether the vector is zero
         *
         * @f[
         *      |\boldsymbol a \cdot \boldsymbol a - 0| < \epsilon^2 \cong \epsilon
         * @f]
         * @see dot(), normalized()
         */
        bool isZero() const {
            return Implementation::isZeroSquared(dot());
        }

        /**
         * @brief Whether the vector is normalized
         *
         * The vector is normalized if it has unit length: @f[
         *      |\boldsymbol a \cdot \boldsymbol a - 1| < 2 \epsilon + \epsilon^2 \cong 2 \epsilon
         * @f]
         * @see dot(), normalized()
         */
        bool isNormalized() const {
            return Implementation::isNormalizedSquared(dot());
        }

        /**
         * @brief Negated vector
         *
         * @f[
         *      \boldsymbol b_i = -\boldsymbol a_i
         * @f]
         * @see Vector2::perpendicular()
         */
        Vector<size, T> operator-() const;

        /**
         * @brief Add and assign vector
         *
         * The computation is done in-place. @f[
         *      \boldsymbol a_i = \boldsymbol a_i + \boldsymbol b_i
         * @f]
         */
        Vector<size, T>& operator+=(const Vector<size, T>& other) {
            for(std::size_t i = 0; i != size; ++i)
                _data[i] += other._data[i];

            return *this;
        }

        /**
         * @brief Add vector
         *
         * @see operator+=(), sum()
         */
        Vector<size, T> operator+(const Vector<size, T>& other) const {
            return Vector<size, T>(*this) += other;
        }

        /**
         * @brief Subtract and assign vector
         *
         * The computation is done in-place. @f[
         *      \boldsymbol a_i = \boldsymbol a_i - \boldsymbol b_i
         * @f]
         */
        Vector<size, T>& operator-=(const Vector<size, T>& other) {
            for(std::size_t i = 0; i != size; ++i)
                _data[i] -= other._data[i];

            return *this;
        }

        /**
         * @brief Subtract vector
         *
         * @see operator-=()
         */
        Vector<size, T> operator-(const Vector<size, T>& other) const {
            return Vector<size, T>(*this) -= other;
        }

        /**
         * @brief Multiply vector with number and assign
         *
         * The computation is done in-place. @f[
         *      \boldsymbol a_i = b \boldsymbol a_i
         * @f]
         * @see operator*=(const Vector<size, T>&),
         *      operator*=(Vector<size, Integral>&, FloatingPoint)
         */
        Vector<size, T>& operator*=(T number) {
            for(std::size_t i = 0; i != size; ++i)
                _data[i] *= number;

            return *this;
        }

        /**
         * @brief Multiply vector with number
         *
         * @see operator*(const Vector<size, T>&) const,
         *      operator*=(T), operator*(T, const Vector<size, T>&),
         *      operator*(const Vector<size, Integral>&, FloatingPoint)
         */
        Vector<size, T> operator*(T number) const {
            return Vector<size, T>(*this) *= number;
        }

        /**
         * @brief Divide vector with number and assign
         *
         * The computation is done in-place. @f[
         *      \boldsymbol a_i = \frac{\boldsymbol a_i} b
         * @f]
         * @see operator/=(const Vector<size, T>&),
         *      operator/=(Vector<size, Integral>&, FloatingPoint)
         */
        Vector<size, T>& operator/=(T number) {
            for(std::size_t i = 0; i != size; ++i)
                _data[i] /= number;

            return *this;
        }

        /**
         * @brief Divide vector with number
         *
         * @see operator/(const Vector<size, T>&) const,
         *      operator/=(T), operator/(T, const Vector<size, T>&),
         *      operator/(const Vector<size, Integral>&, FloatingPoint)
         */
        Vector<size, T> operator/(T number) const {
            return Vector<size, T>(*this) /= number;
        }

        /**
         * @brief Multiply vector component-wise and assign
         *
         * The computation is done in-place. @f[
         *      \boldsymbol a_i = \boldsymbol a_i \boldsymbol b_i
         * @f]
         * @see operator*=(T),
         *      operator*=(Vector<size, Integral>&, const Vector<size, FloatingPoint>&)
         */
        Vector<size, T>& operator*=(const Vector<size, T>& other) {
            for(std::size_t i = 0; i != size; ++i)
                _data[i] *= other._data[i];

            return *this;
        }

        /**
         * @brief Multiply vector component-wise
         *
         * @see operator*(T) const, operator*=(const Vector<size, T>&),
         *      operator*(const Vector<size, Integral>&, const Vector<size, FloatingPoint>&)
         */
        Vector<size, T> operator*(const Vector<size, T>& other) const {
            return Vector<size, T>(*this) *= other;
        }

        /**
         * @brief Divide vector component-wise and assign
         *
         * The computation is done in-place. @f[
         *      \boldsymbol a_i = \frac{\boldsymbol a_i}{\boldsymbol b_i}
         * @f]
         * @see operator/=(T),
         *      operator/=(Vector<size, Integral>&, const Vector<size, FloatingPoint>&)
         */
        Vector<size, T>& operator/=(const Vector<size, T>& other) {
            for(std::size_t i = 0; i != size; ++i)
                _data[i] /= other._data[i];

            return *this;
        }

        /**
         * @brief Divide vector component-wise
         *
         * @see operator/(T) const, operator/=(const Vector<size, T>&),
         *      operator/(const Vector<size, Integral>&, const Vector<size, FloatingPoint>&)
         */
        Vector<size, T> operator/(const Vector<size, T>& other) const {
            return Vector<size, T>(*this) /= other;
        }

        /**
         * @brief Dot product of the vector
         *
         * Should be used instead of length() for comparing vector length with
         * other values, because it doesn't compute the square root. @f[
         *      \boldsymbol a \cdot \boldsymbol a = \sum_{i=0}^{n-1} \boldsymbol a_i^2
         * @f]
         * @see dot(const Vector<size, T>&, const Vector<size, T>&), isNormalized()
         */
        T dot() const { return dot(*this, *this); }

        /**
         * @brief %Vector length
         *
         * See also dot() const which is faster for comparing length with other
         * values. @f[
         *      |\boldsymbol a| = \sqrt{\boldsymbol a \cdot \boldsymbol a}
         * @f]
         * @see lengthInverted(), Math::sqrt(), normalized(), resized()
         * @todo something like std::hypot() for possibly better precision?
         */
        T length() const { return std::sqrt(dot()); }

        /**
         * @brief Inverse vector length
         *
         * @f[
         *      \frac{1}{|\boldsymbol a|} = \frac{1}{\sqrt{\boldsymbol a \cdot \boldsymbol a}}
         * @f]
         * @see length(), Math::sqrtInverted(), normalized(), resized()
         */
        T lengthInverted() const { return T(1)/length(); }

        /**
         * @brief Normalized vector (of unit length)
         *
         * @see isNormalized(), lengthInverted(), resized()
         */
        Vector<size, T> normalized() const { return *this*lengthInverted(); }

        /**
         * @brief Resized vector
         *
         * Convenience equivalent to the following code. Due to operation order
         * this function is faster than the obvious way of sizing normalized()
         * vector.
         * @code
         * vec*(vec.lengthInverted()*length) // the brackets are important
         * @endcode
         * @see normalized()
         */
        Vector<size, T> resized(T length) const {
            return *this*(lengthInverted()*length);
        }

        /**
         * @brief %Vector projected onto line
         *
         * Returns vector projected onto @p line. @f[
         *      \boldsymbol a_1 = \frac{\boldsymbol a \cdot \boldsymbol b}{\boldsymbol b \cdot \boldsymbol b} \boldsymbol b
         * @f]
         * @see dot(), projectedOntoNormalized()
         */
        Vector<size, T> projected(const Vector<size, T>& line) const {
            return line*dot(*this, line)/line.dot();
        }

        /**
         * @brief %Vector projected onto normalized line
         *
         * Slightly faster alternative to projected(), expects @p line to be
         * normalized. @f[
         *      \boldsymbol a_1 = \frac{\boldsymbol a \cdot \boldsymbol b}{\boldsymbol b \cdot \boldsymbol b} \boldsymbol b =
         *          (\boldsymbol a \cdot \boldsymbol b) \boldsymbol b
         * @f]
         * @see dot()
         */
        Vector<size, T> projectedOntoNormalized(const Vector<size, T>& line) const;

        /**
         * @brief Sum of values in the vector
         *
         * @see operator+()
         */
        T sum() const;

        /**
         * @brief Product of values in the vector
         *
         * @see operator*(const Vector<size, T>&) const
         */
        T product() const;

        /**
         * @brief Minimal value in the vector
         *
         * @see Math::min()
         */
        T min() const;

        /**
         * @brief Maximal value in the vector
         *
         * @see Math::max()
         */
        T max() const;

    private:
        /* Implementation for Vector<size, T>::Vector(const Vector<size, U>&) */
        template<class U, std::size_t ...sequence> constexpr explicit Vector(Implementation::Sequence<sequence...>, const Vector<sizeof...(sequence), U>& vector): _data{T(vector._data[sequence])...} {}

        /* Implementation for Vector<size, T>::Vector(U) */
        template<std::size_t ...sequence> constexpr explicit Vector(Implementation::Sequence<sequence...>, T value): _data{Implementation::repeat(value, sequence)...} {}

        T _data[size];
};

/** @relates Vector
@brief Multiply number with vector

Same as Vector::operator*(T) const.
*/
template<std::size_t size, class T> inline Vector<size, T> operator*(
    #ifdef DOXYGEN_GENERATING_OUTPUT
    T
    #else
    typename std::common_type<T>::type
    #endif
    number, const Vector<size, T>& vector)
{
    return vector*number;
}

/** @relates Vector
@brief Divide vector with number and invert

@f[
    \boldsymbol c_i = \frac b {\boldsymbol a_i}
@f]
@see Vector::operator/(T) const
*/
template<std::size_t size, class T> inline Vector<size, T> operator/(
    #ifdef DOXYGEN_GENERATING_OUTPUT
    T
    #else
    typename std::common_type<T>::type
    #endif
    number, const Vector<size, T>& vector)
{
    Vector<size, T> out;

    for(std::size_t i = 0; i != size; ++i)
        out[i] = number/vector[i];

    return out;
}

/** @relates Vector
@brief Do modulo of integral vector and assign

The computation is done in-place.
*/
template<std::size_t size, class Integral> inline
#ifdef DOXYGEN_GENERATING_OUTPUT
Vector<size, Integral>&
#else
typename std::enable_if<std::is_integral<Integral>::value, Vector<size, Integral>&>::type
#endif
operator%=(Vector<size, Integral>& a, Integral b) {
    for(std::size_t i = 0; i != size; ++i)
        a[i] %= b;

    return a;
}

/** @relates Vector
@brief Modulo of integral vector
*/
template<std::size_t size, class Integral> inline
#ifdef DOXYGEN_GENERATING_OUTPUT
Vector<size, Integral>
#else
typename std::enable_if<std::is_integral<Integral>::value, Vector<size, Integral>>::type
#endif
operator%(const Vector<size, Integral>& a, Integral b) {
    Vector<size, Integral> copy(a);
    return copy %= b;
}

/** @relates Vector
@brief Do modulo of two integral vectors and assign

The computation is done in-place.
*/
template<std::size_t size, class Integral> inline
#ifdef DOXYGEN_GENERATING_OUTPUT
Vector<size, Integral>&
#else
typename std::enable_if<std::is_integral<Integral>::value, Vector<size, Integral>&>::type
#endif
operator%=(Vector<size, Integral>& a, const Vector<size, Integral>& b) {
    for(std::size_t i = 0; i != size; ++i)
        a[i] %= b[i];

    return a;
}

/** @relates Vector
@brief Modulo of two integral vectors
*/
template<std::size_t size, class Integral> inline
#ifdef DOXYGEN_GENERATING_OUTPUT
Vector<size, Integral>
#else
typename std::enable_if<std::is_integral<Integral>::value, Vector<size, Integral>>::type
#endif
operator%(const Vector<size, Integral>& a, const Vector<size, Integral>& b) {
    Vector<size, Integral> copy(a);
    return copy %= b;
}

/** @relates Vector
@brief Bitwise NOT of integral vector
*/
template<std::size_t size, class Integral> inline
#ifdef DOXYGEN_GENERATING_OUTPUT
Vector<size, Integral>
#else
typename std::enable_if<std::is_integral<Integral>::value, Vector<size, Integral>>::type
#endif
operator~(const Vector<size, Integral>& vector) {
    Vector<size, Integral> out;

    for(std::size_t i = 0; i != size; ++i)
        out[i] = ~vector[i];

    return out;
}

/** @relates Vector
@brief Do bitwise AND of two integral vectors and assign

The computation is done in-place.
*/
template<std::size_t size, class Integral> inline
#ifdef DOXYGEN_GENERATING_OUTPUT
Vector<size, Integral>&
#else
typename std::enable_if<std::is_integral<Integral>::value, Vector<size, Integral>&>::type
#endif
operator&=(Vector<size, Integral>& a, const Vector<size, Integral>& b) {
    for(std::size_t i = 0; i != size; ++i)
        a[i] &= b[i];

    return a;
}

/** @relates Vector
@brief Bitwise AND of two integral vectors
*/
template<std::size_t size, class Integral> inline
#ifdef DOXYGEN_GENERATING_OUTPUT
Vector<size, Integral>
#else
typename std::enable_if<std::is_integral<Integral>::value, Vector<size, Integral>>::type
#endif
operator&(const Vector<size, Integral>& a, const Vector<size, Integral>& b) {
    Vector<size, Integral> copy(a);
    return copy &= b;
}

/** @relates Vector
@brief Do bitwise OR of two integral vectors and assign

The computation is done in-place.
*/
template<std::size_t size, class Integral> inline
#ifdef DOXYGEN_GENERATING_OUTPUT
Vector<size, Integral>&
#else
typename std::enable_if<std::is_integral<Integral>::value, Vector<size, Integral>&>::type
#endif
operator|=(Vector<size, Integral>& a, const Vector<size, Integral>& b) {
    for(std::size_t i = 0; i != size; ++i)
        a[i] |= b[i];

    return a;
}

/** @relates Vector
@brief Bitwise OR of two integral vectors
*/
template<std::size_t size, class Integral> inline
#ifdef DOXYGEN_GENERATING_OUTPUT
Vector<size, Integral>
#else
typename std::enable_if<std::is_integral<Integral>::value, Vector<size, Integral>>::type
#endif
operator|(const Vector<size, Integral>& a, const Vector<size, Integral>& b) {
    Vector<size, Integral> copy(a);
    return copy |= b;
}

/** @relates Vector
@brief Do bitwise XOR of two integral vectors and assign

The computation is done in-place.
*/
template<std::size_t size, class Integral> inline
#ifdef DOXYGEN_GENERATING_OUTPUT
Vector<size, Integral>&
#else
typename std::enable_if<std::is_integral<Integral>::value, Vector<size, Integral>&>::type
#endif
operator^=(Vector<size, Integral>& a, const Vector<size, Integral>& b) {
    for(std::size_t i = 0; i != size; ++i)
        a[i] ^= b[i];

    return a;
}

/** @relates Vector
@brief Bitwise XOR of two integral vectors
*/
template<std::size_t size, class Integral> inline
#ifdef DOXYGEN_GENERATING_OUTPUT
Vector<size, Integral>
#else
typename std::enable_if<std::is_integral<Integral>::value, Vector<size, Integral>>::type
#endif
operator^(const Vector<size, Integral>& a, const Vector<size, Integral>& b) {
    Vector<size, Integral> copy(a);
    return copy ^= b;
}

/** @relates Vector
@brief Do bitwise left shift of integral vector and assign

The computation is done in-place.
*/
template<std::size_t size, class Integral> inline
#ifdef DOXYGEN_GENERATING_OUTPUT
Vector<size, Integral>&
#else
typename std::enable_if<std::is_integral<Integral>::value, Vector<size, Integral>&>::type
#endif
operator<<=(Vector<size, Integral>& vector,
    #ifdef DOXYGEN_GENERATING_OUTPUT
    Integral
    #else
    typename std::common_type<Integral>::type
    #endif
    shift)
{
    for(std::size_t i = 0; i != size; ++i)
        vector[i] <<= shift;

    return vector;
}

/** @relates Vector
@brief Bitwise left shift of integral vector
*/
template<std::size_t size, class Integral> inline
#ifdef DOXYGEN_GENERATING_OUTPUT
Vector<size, Integral>
#else
typename std::enable_if<std::is_integral<Integral>::value, Vector<size, Integral>>::type
#endif
operator<<(const Vector<size, Integral>& vector,
    #ifdef DOXYGEN_GENERATING_OUTPUT
    Integral
    #else
    typename std::common_type<Integral>::type
    #endif
    shift)
{
    Vector<size, Integral> copy(vector);
    return copy <<= shift;
}

/** @relates Vector
@brief Do bitwise right shift of integral vector and assign

The computation is done in-place.
*/
template<std::size_t size, class Integral> inline
#ifdef DOXYGEN_GENERATING_OUTPUT
Vector<size, Integral>&
#else
typename std::enable_if<std::is_integral<Integral>::value, Vector<size, Integral>&>::type
#endif
operator>>=(Vector<size, Integral>& vector,
    #ifdef DOXYGEN_GENERATING_OUTPUT
    Integral
    #else
    typename std::common_type<Integral>::type
    #endif
    shift) {
    for(std::size_t i = 0; i != size; ++i)
        vector[i] >>= shift;

    return vector;
}

/** @relates Vector
@brief Bitwise left shift of integral vector
*/
template<std::size_t size, class Integral> inline
#ifdef DOXYGEN_GENERATING_OUTPUT
Vector<size, Integral>
#else
typename std::enable_if<std::is_integral<Integral>::value, Vector<size, Integral>>::type
#endif
operator>>(const Vector<size, Integral>& vector,
    #ifdef DOXYGEN_GENERATING_OUTPUT
    Integral
    #else
    typename std::common_type<Integral>::type
    #endif
    shift) {
    Vector<size, Integral> copy(vector);
    return copy >>= shift;
}

/** @relates Vector
@brief Multiply integral vector with floating-point number and assign

Similar to Vector::operator*=(T), except that the multiplication is done in
floating-point. The computation is done in-place.
*/
template<std::size_t size, class Integral, class FloatingPoint> inline
#ifdef DOXYGEN_GENERATING_OUTPUT
Vector<size, Integral>&
#else
typename std::enable_if<std::is_integral<Integral>::value && std::is_floating_point<FloatingPoint>::value, Vector<size, Integral>&>::type
#endif
operator*=(Vector<size, Integral>& vector, FloatingPoint number) {
    for(std::size_t i = 0; i != size; ++i)
        vector[i] *= number;

    return vector;
}

/** @relates Vector
@brief Multiply integral vector with floating-point number

Similar to Vector::operator*(T) const, except that the multiplication is done
in floating-point.
*/
template<std::size_t size, class Integral, class FloatingPoint> inline
#ifdef DOXYGEN_GENERATING_OUTPUT
Vector<size, Integral>
#else
typename std::enable_if<std::is_integral<Integral>::value && std::is_floating_point<FloatingPoint>::value, Vector<size, Integral>>::type
#endif
operator*(const Vector<size, Integral>& vector, FloatingPoint number) {
    Vector<size, Integral> copy(vector);
    return copy *= number;
}

/** @relates Vector
@brief Multiply floating-point number with integral vector

Same as operator*(const Vector<size, Integral>&, FloatingPoint).
*/
template<std::size_t size, class FloatingPoint, class Integral> inline
#ifdef DOXYGEN_GENERATING_OUTPUT
Vector<size, Integral>
#else
typename std::enable_if<std::is_integral<Integral>::value && std::is_floating_point<FloatingPoint>::value, Vector<size, Integral>>::type
#endif
operator*(FloatingPoint number, const Vector<size, Integral>& vector) {
    return vector*number;
}

/** @relates Vector
@brief Divide integral vector with floating-point number and assign

Similar to Vector::operator/=(T), except that the division is done in
floating-point. The computation is done in-place.
*/
template<std::size_t size, class Integral, class FloatingPoint> inline
#ifdef DOXYGEN_GENERATING_OUTPUT
Vector<size, Integral>&
#else
typename std::enable_if<std::is_integral<Integral>::value && std::is_floating_point<FloatingPoint>::value, Vector<size, Integral>&>::type
#endif
operator/=(Vector<size, Integral>& vector, FloatingPoint number) {
    for(std::size_t i = 0; i != size; ++i)
        vector[i] /= number;

    return vector;
}

/** @relates Vector
@brief Divide integral vector with floating-point number

Similar to Vector::operator/(T) const, except that the division is done in
floating-point.
*/
template<std::size_t size, class Integral, class FloatingPoint> inline
#ifdef DOXYGEN_GENERATING_OUTPUT
Vector<size, Integral>
#else
typename std::enable_if<std::is_integral<Integral>::value && std::is_floating_point<FloatingPoint>::value, Vector<size, Integral>>::type
#endif
operator/(const Vector<size, Integral>& vector, FloatingPoint number) {
    Vector<size, Integral> copy(vector);
    return copy /= number;
}

/** @relates Vector
@brief Multiply integral vector with floating-point vector component-wise and assign

Similar to Vector::operator*=(const Vector<size, T>&), except that the
multiplication is done in floating-point. The computation is done in-place.
*/
template<std::size_t size, class Integral, class FloatingPoint> inline
#ifdef DOXYGEN_GENERATING_OUTPUT
Vector<size, Integral>&
#else
typename std::enable_if<std::is_integral<Integral>::value && std::is_floating_point<FloatingPoint>::value, Vector<size, Integral>&>::type
#endif
operator*=(Vector<size, Integral>& a, const Vector<size, FloatingPoint>& b) {
    for(std::size_t i = 0; i != size; ++i)
        a[i] *= b[i];

    return a;
}

/** @relates Vector
@brief Multiply integral vector with floating-point vector component-wise

Similar to Vector::operator*(const Vector<size, T>&) const, except that the
multiplication is done in floating-point. The result is always integral vector,
convert both arguments to the same floating-point type to have floating-point
result.
*/
template<std::size_t size, class Integral, class FloatingPoint> inline
#ifdef DOXYGEN_GENERATING_OUTPUT
Vector<size, Integral>
#else
typename std::enable_if<std::is_integral<Integral>::value && std::is_floating_point<FloatingPoint>::value, Vector<size, Integral>>::type
#endif
operator*(const Vector<size, Integral>& a, const Vector<size, FloatingPoint>& b) {
    Vector<size, Integral> copy(a);
    return copy *= b;
}

/** @relates Vector
@brief Multiply floating-point vector with integral vector component-wise

Same as operator*(const Vector<size, Integral>&, const Vector<size, FloatingPoint>&).
*/
template<std::size_t size, class FloatingPoint, class Integral> inline
#ifdef DOXYGEN_GENERATING_OUTPUT
Vector<size, Integral>
#else
typename std::enable_if<std::is_integral<Integral>::value && std::is_floating_point<FloatingPoint>::value, Vector<size, Integral>>::type
#endif
operator*(const Vector<size, FloatingPoint>& a, const Vector<size, Integral>& b) {
    return b*a;
}

/** @relates Vector
@brief Divide integral vector with floating-point vector component-wise and assign

Similar to Vector::operator/=(const Vector<size, T>&), except that the division
is done in floating-point. The computation is done in-place.
*/
template<std::size_t size, class Integral, class FloatingPoint> inline
#ifdef DOXYGEN_GENERATING_OUTPUT
Vector<size, Integral>&
#else
typename std::enable_if<std::is_integral<Integral>::value && std::is_floating_point<FloatingPoint>::value, Vector<size, Integral>&>::type
#endif
operator/=(Vector<size, Integral>& a, const Vector<size, FloatingPoint>& b) {
    for(std::size_t i = 0; i != size; ++i)
        a[i] /= b[i];

    return a;
}

/** @relates Vector
@brief Divide integral vector with floating-point vector component-wise

Similar to Vector::operator/(const Vector<size, T>&) const, except that the
division is done in floating-point. The result is always integral vector,
convert both arguments to the same floating-point type to have floating-point
result.
*/
template<std::size_t size, class Integral, class FloatingPoint> inline
#ifdef DOXYGEN_GENERATING_OUTPUT
Vector<size, Integral>
#else
typename std::enable_if<std::is_integral<Integral>::value && std::is_floating_point<FloatingPoint>::value, Vector<size, Integral>>::type
#endif
operator/(const Vector<size, Integral>& a, const Vector<size, FloatingPoint>& b) {
    Vector<size, Integral> copy(a);
    return copy /= b;
}

/** @debugoperator{Magnum::Math::Vector} */
template<std::size_t size, class T> Corrade::Utility::Debug operator<<(Corrade::Utility::Debug debug, const Vector<size, T>& value) {
    debug << "Vector(";
    debug.setFlag(Corrade::Utility::Debug::SpaceAfterEachValue, false);
    for(std::size_t i = 0; i != size; ++i) {
        if(i != 0) debug << ", ";
        debug << value[i];
    }
    debug << ")";
    debug.setFlag(Corrade::Utility::Debug::SpaceAfterEachValue, true);
    return debug;
}

/* Explicit instantiation for types used in OpenGL */
#ifndef DOXYGEN_GENERATING_OUTPUT
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const Vector<2, Float>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const Vector<3, Float>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const Vector<4, Float>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const Vector<2, Int>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const Vector<3, Int>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const Vector<4, Int>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const Vector<2, UnsignedInt>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const Vector<3, UnsignedInt>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const Vector<4, UnsignedInt>&);
#ifndef MAGNUM_TARGET_GLES
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const Vector<2, Double>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const Vector<3, Double>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const Vector<4, Double>&);
#endif
#endif

#ifndef DOXYGEN_GENERATING_OUTPUT
#define MAGNUM_VECTOR_SUBCLASS_IMPLEMENTATION(size, Type)                   \
    constexpr static Type<T>& from(T* data) {                               \
        return *reinterpret_cast<Type<T>*>(data);                           \
    }                                                                       \
    constexpr static const Type<T>& from(const T* data) {                   \
        return *reinterpret_cast<const Type<T>*>(data);                     \
    }                                                                       \
                                                                            \
    Type<T>& operator=(const Type<T>& other) {                              \
        Math::Vector<size, T>::operator=(other);                            \
        return *this;                                                       \
    }                                                                       \
                                                                            \
    Type<T> operator-() const {                                             \
        return Math::Vector<size, T>::operator-();                          \
    }                                                                       \
    Type<T>& operator+=(const Math::Vector<size, T>& other) {               \
        Math::Vector<size, T>::operator+=(other);                           \
        return *this;                                                       \
    }                                                                       \
    Type<T> operator+(const Math::Vector<size, T>& other) const {           \
        return Math::Vector<size, T>::operator+(other);                     \
    }                                                                       \
    Type<T>& operator-=(const Math::Vector<size, T>& other) {               \
        Math::Vector<size, T>::operator-=(other);                           \
        return *this;                                                       \
    }                                                                       \
    Type<T> operator-(const Math::Vector<size, T>& other) const {           \
        return Math::Vector<size, T>::operator-(other);                     \
    }                                                                       \
    Type<T>& operator*=(T number) {                                         \
        Math::Vector<size, T>::operator*=(number);                          \
        return *this;                                                       \
    }                                                                       \
    Type<T> operator*(T number) const {                                     \
        return Math::Vector<size, T>::operator*(number);                    \
    }                                                                       \
    Type<T>& operator/=(T number) {                                         \
        Math::Vector<size, T>::operator/=(number);                          \
        return *this;                                                       \
    }                                                                       \
    Type<T> operator/(T number) const {                                     \
        return Math::Vector<size, T>::operator/(number);                    \
    }                                                                       \
    Type<T>& operator*=(const Math::Vector<size, T>& other) {               \
        Math::Vector<size, T>::operator*=(other);                           \
        return *this;                                                       \
    }                                                                       \
    Type<T> operator*(const Math::Vector<size, T>& other) const {           \
        return Math::Vector<size, T>::operator*(other);                     \
    }                                                                       \
    Type<T>& operator/=(const Math::Vector<size, T>& other) {               \
        Math::Vector<size, T>::operator/=(other);                           \
        return *this;                                                       \
    }                                                                       \
    Type<T> operator/(const Math::Vector<size, T>& other) const {           \
        return Math::Vector<size, T>::operator/(other);                     \
    }                                                                       \
                                                                            \
    Type<T> normalized() const {                                            \
        return Math::Vector<size, T>::normalized();                         \
    }                                                                       \
    Type<T> resized(T length) const {                                       \
        return Math::Vector<size, T>::resized(length);                      \
    }                                                                       \
    Type<T> projected(const Math::Vector<size, T>& other) const {           \
        return Math::Vector<size, T>::projected(other);                     \
    }                                                                       \
    Type<T> projectedOntoNormalized(const Math::Vector<size, T>& other) const { \
        return Math::Vector<size, T>::projectedOntoNormalized(other);       \
    }

#define MAGNUM_VECTORn_OPERATOR_IMPLEMENTATION(size, Type)                  \
    template<class T> inline Type<T> operator*(typename std::common_type<T>::type number, const Type<T>& vector) { \
        return number*static_cast<const Math::Vector<size, T>&>(vector);    \
    }                                                                       \
    template<class T> inline Type<T> operator/(typename std::common_type<T>::type number, const Type<T>& vector) { \
        return number/static_cast<const Math::Vector<size, T>&>(vector);    \
    }                                                                       \
                                                                            \
    template<class Integral> inline typename std::enable_if<std::is_integral<Integral>::value, Type<Integral>&>::type operator%=(Type<Integral>& a, Integral b) { \
        static_cast<Math::Vector<size, Integral>&>(a) %= b;                 \
        return a;                                                           \
    }                                                                       \
    template<class Integral> inline typename std::enable_if<std::is_integral<Integral>::value, Type<Integral>>::type operator%(const Type<Integral>& a, Integral b) { \
        return static_cast<const Math::Vector<size, Integral>&>(a) % b;     \
    }                                                                       \
    template<class Integral> inline typename std::enable_if<std::is_integral<Integral>::value, Type<Integral>&>::type operator%=(Type<Integral>& a, const Math::Vector<size, Integral>& b) { \
        static_cast<Math::Vector<size, Integral>&>(a) %= b;                 \
        return a;                                                           \
    }                                                                       \
    template<class Integral> inline typename std::enable_if<std::is_integral<Integral>::value, Type<Integral>>::type operator%(const Type<Integral>& a, const Math::Vector<size, Integral>& b) { \
        return static_cast<const Math::Vector<size, Integral>&>(a) % b;     \
    }                                                                       \
                                                                            \
    template<class Integral> inline typename std::enable_if<std::is_integral<Integral>::value, Type<Integral>>::type operator~(const Type<Integral>& vector) { \
        return ~static_cast<const Math::Vector<size, Integral>&>(vector);   \
    }                                                                       \
    template<class Integral> inline typename std::enable_if<std::is_integral<Integral>::value, Type<Integral>&>::type operator&=(Type<Integral>& a, const Math::Vector<size, Integral>& b) { \
        static_cast<Math::Vector<size, Integral>&>(a) &= b;                 \
        return a;                                                           \
    }                                                                       \
    template<class Integral> inline typename std::enable_if<std::is_integral<Integral>::value, Type<Integral>>::type operator&(const Type<Integral>& a, const Math::Vector<size, Integral>& b) { \
        return static_cast<const Math::Vector<size, Integral>&>(a) & b;     \
    }                                                                       \
    template<class Integral> inline typename std::enable_if<std::is_integral<Integral>::value, Type<Integral>&>::type operator|=(Type<Integral>& a, const Math::Vector<size, Integral>& b) { \
        static_cast<Math::Vector<size, Integral>&>(a) |= b;                 \
        return a;                                                           \
    }                                                                       \
    template<class Integral> inline typename std::enable_if<std::is_integral<Integral>::value, Type<Integral>>::type operator|(const Type<Integral>& a, const Math::Vector<size, Integral>& b) { \
        return static_cast<const Math::Vector<size, Integral>&>(a) | b;     \
    }                                                                       \
    template<class Integral> inline typename std::enable_if<std::is_integral<Integral>::value, Type<Integral>&>::type operator^=(Type<Integral>& a, const Math::Vector<size, Integral>& b) { \
        static_cast<Math::Vector<size, Integral>&>(a) ^= b;                 \
        return a;                                                           \
    }                                                                       \
    template<class Integral> inline typename std::enable_if<std::is_integral<Integral>::value, Type<Integral>>::type operator^(const Type<Integral>& a, const Math::Vector<size, Integral>& b) { \
        return static_cast<const Math::Vector<size, Integral>&>(a) ^ b;     \
    }                                                                       \
    template<class Integral> inline typename std::enable_if<std::is_integral<Integral>::value, Type<Integral>&>::type operator<<=(Type<Integral>& vector, typename std::common_type<Integral>::type shift) { \
        static_cast<Math::Vector<size, Integral>&>(vector) <<= shift;       \
        return vector;                                                      \
    }                                                                       \
    template<class Integral> inline typename std::enable_if<std::is_integral<Integral>::value, Type<Integral>>::type operator<<(const Type<Integral>& vector, typename std::common_type<Integral>::type shift) { \
        return static_cast<const Math::Vector<size, Integral>&>(vector) << shift; \
    }                                                                       \
    template<class Integral> inline typename std::enable_if<std::is_integral<Integral>::value, Type<Integral>&>::type operator>>=(Type<Integral>& vector, typename std::common_type<Integral>::type shift) { \
        static_cast<Math::Vector<size, Integral>&>(vector) >>= shift;       \
        return vector;                                                      \
    }                                                                       \
    template<class Integral> inline typename std::enable_if<std::is_integral<Integral>::value, Type<Integral>>::type operator>>(const Type<Integral>& vector, typename std::common_type<Integral>::type shift) { \
        return static_cast<const Math::Vector<size, Integral>&>(vector) >> shift; \
    }                                                                       \
    template<class Integral, class FloatingPoint> inline typename std::enable_if<std::is_integral<Integral>::value && std::is_floating_point<FloatingPoint>::value, Type<Integral>&>::type operator*=(Type<Integral>& vector, FloatingPoint number) { \
        static_cast<Math::Vector<size, Integral>&>(vector) *= number;       \
        return vector;                                                      \
    }                                                                       \
    template<class Integral, class FloatingPoint> inline typename std::enable_if<std::is_integral<Integral>::value && std::is_floating_point<FloatingPoint>::value, Type<Integral>>::type operator*(const Type<Integral>& vector, FloatingPoint number) { \
        return static_cast<const Math::Vector<size, Integral>&>(vector)*number; \
    }                                                                       \
    template<class FloatingPoint, class Integral> inline typename std::enable_if<std::is_integral<Integral>::value && std::is_floating_point<FloatingPoint>::value, Type<Integral>>::type operator*(FloatingPoint number, const Type<Integral>& vector) { \
        return number*static_cast<const Math::Vector<size, Integral>&>(vector); \
    }                                                                       \
    template<class Integral, class FloatingPoint> inline typename std::enable_if<std::is_integral<Integral>::value && std::is_floating_point<FloatingPoint>::value, Type<Integral>&>::type operator/=(Type<Integral>& vector, FloatingPoint number) { \
        static_cast<Math::Vector<size, Integral>&>(vector) /= number;       \
        return vector;                                                      \
    }                                                                       \
    template<class Integral, class FloatingPoint> inline typename std::enable_if<std::is_integral<Integral>::value && std::is_floating_point<FloatingPoint>::value, Type<Integral>>::type operator/(const Type<Integral>& vector, FloatingPoint number) { \
        return static_cast<const Math::Vector<size, Integral>&>(vector)/number; \
    }                                                                       \
                                                                            \
    template<class Integral, class FloatingPoint> inline typename std::enable_if<std::is_integral<Integral>::value && std::is_floating_point<FloatingPoint>::value, Type<Integral>&>::type operator*=(Type<Integral>& a, const Math::Vector<size, FloatingPoint>& b) { \
        static_cast<Math::Vector<size, Integral>&>(a) *= b;                 \
        return a;                                                           \
    }                                                                       \
    template<class Integral, class FloatingPoint> inline typename std::enable_if<std::is_integral<Integral>::value && std::is_floating_point<FloatingPoint>::value, Type<Integral>>::type operator*(const Type<Integral>& a, const Math::Vector<size, FloatingPoint>& b) { \
        return static_cast<const Math::Vector<size, Integral>&>(a)*b;       \
    }                                                                       \
    template<class FloatingPoint, class Integral> inline typename std::enable_if<std::is_integral<Integral>::value && std::is_floating_point<FloatingPoint>::value, Type<Integral>>::type operator*(const Math::Vector<size, FloatingPoint>& a, const Type<Integral>& b) { \
        return a*static_cast<const Math::Vector<size, Integral>&>(b);       \
    }                                                                       \
    template<class Integral, class FloatingPoint> inline typename std::enable_if<std::is_integral<Integral>::value && std::is_floating_point<FloatingPoint>::value, Type<Integral>&>::type operator/=(Type<Integral>& a, const Math::Vector<size, FloatingPoint>& b) { \
        static_cast<Math::Vector<size, Integral>&>(a) /= b;                 \
        return a;                                                           \
    }                                                                       \
    template<class Integral, class FloatingPoint> inline typename std::enable_if<std::is_integral<Integral>::value && std::is_floating_point<FloatingPoint>::value, Type<Integral>>::type operator/(const Type<Integral>& a, const Math::Vector<size, FloatingPoint>& b) { \
        return static_cast<const Math::Vector<size, Integral>&>(a)/b;       \
    }
#endif

template<std::size_t size, class T> inline Rad<T> Vector<size, T>::angle(const Vector<size, T>& normalizedA, const Vector<size, T>& normalizedB) {
    CORRADE_ASSERT(normalizedA.isNormalized() && normalizedB.isNormalized(),
        "Math::Vector::angle(): vectors must be normalized", Rad<T>(std::numeric_limits<T>::quiet_NaN()));
    return Rad<T>(std::acos(dot(normalizedA, normalizedB)));
}

template<std::size_t size, class T> inline BoolVector<size> Vector<size, T>::operator<(const Vector<size, T>& other) const {
    BoolVector<size> out;

    for(std::size_t i = 0; i != size; ++i)
        out.set(i, _data[i] < other._data[i]);

    return out;
}

template<std::size_t size, class T> inline BoolVector<size> Vector<size, T>::operator<=(const Vector<size, T>& other) const {
    BoolVector<size> out;

    for(std::size_t i = 0; i != size; ++i)
        out.set(i, _data[i] <= other._data[i]);

    return out;
}

template<std::size_t size, class T> inline BoolVector<size> Vector<size, T>::operator>=(const Vector<size, T>& other) const {
    BoolVector<size> out;

    for(std::size_t i = 0; i != size; ++i)
        out.set(i, _data[i] >= other._data[i]);

    return out;
}

template<std::size_t size, class T> inline BoolVector<size> Vector<size, T>::operator>(const Vector<size, T>& other) const {
    BoolVector<size> out;

    for(std::size_t i = 0; i != size; ++i)
        out.set(i, _data[i] > other._data[i]);

    return out;
}

template<std::size_t size, class T> inline Vector<size, T> Vector<size, T>::operator-() const {
    Vector<size, T> out;

    for(std::size_t i = 0; i != size; ++i)
        out._data[i] = -_data[i];

    return out;
}

template<std::size_t size, class T> inline Vector<size, T> Vector<size, T>::projectedOntoNormalized(const Vector<size, T>& line) const {
    CORRADE_ASSERT(line.isNormalized(), "Math::Vector::projectedOntoNormalized(): line must be normalized",
        (Vector<size, T>(std::numeric_limits<T>::quiet_NaN())));
    return line*dot(*this, line);
}

template<std::size_t size, class T> inline T Vector<size, T>::sum() const {
    T out(_data[0]);

    for(std::size_t i = 1; i != size; ++i)
        out += _data[i];

    return out;
}

template<std::size_t size, class T> inline T Vector<size, T>::product() const {
    T out(_data[0]);

    for(std::size_t i = 1; i != size; ++i)
        out *= _data[i];

    return out;
}

template<std::size_t size, class T> inline T Vector<size, T>::min() const {
    T out(_data[0]);

    for(std::size_t i = 1; i != size; ++i)
        out = std::min(out, _data[i]);

    return out;
}

template<std::size_t size, class T> inline T Vector<size, T>::max() const {
    T out(_data[0]);

    for(std::size_t i = 1; i != size; ++i)
        out = std::max(out, _data[i]);

    return out;
}

}}

namespace Corrade { namespace Utility {

/** @configurationvalue{Magnum::Math::RectangularMatrix} */
template<std::size_t size, class T> struct ConfigurationValue<Magnum::Math::Vector<size, T>> {
    ConfigurationValue() = delete;

    /** @brief Writes elements separated with spaces */
    static std::string toString(const Magnum::Math::Vector<size, T>& value, ConfigurationValueFlags flags) {
        std::string output;

        for(std::size_t i = 0; i != size; ++i) {
            if(!output.empty()) output += ' ';
            output += ConfigurationValue<T>::toString(value[i], flags);
        }

        return output;
    }

    /** @brief Reads elements separated with whitespace */
    static Magnum::Math::Vector<size, T> fromString(const std::string& stringValue, ConfigurationValueFlags flags) {
        Magnum::Math::Vector<size, T> result;

        std::size_t oldpos = 0, pos = std::string::npos, i = 0;
        do {
            pos = stringValue.find(' ', oldpos);
            std::string part = stringValue.substr(oldpos, pos-oldpos);

            if(!part.empty()) {
                result[i] = ConfigurationValue<T>::fromString(part, flags);
                ++i;
            }

            oldpos = pos+1;
        } while(pos != std::string::npos);

        return result;
    }
};

#ifndef DOXYGEN_GENERATING_OUTPUT
/* Vectors */
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Vector<2, Magnum::Float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Vector<3, Magnum::Float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Vector<4, Magnum::Float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Vector<2, Magnum::Int>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Vector<3, Magnum::Int>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Vector<4, Magnum::Int>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Vector<2, Magnum::UnsignedInt>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Vector<3, Magnum::UnsignedInt>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Vector<4, Magnum::UnsignedInt>>;
#ifndef MAGNUM_TARGET_GLES
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Vector<2, Magnum::Double>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Vector<3, Magnum::Double>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Vector<4, Magnum::Double>>;
#endif
#endif

}}

#endif
