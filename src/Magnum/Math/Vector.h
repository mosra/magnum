#ifndef Magnum_Math_Vector_h
#define Magnum_Math_Vector_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
              Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class @ref Magnum::Math::Vector
 */

#include <cmath>
#ifdef _MSC_VER
#include <algorithm> /* std::max() */
#endif
#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/ConfigurationValue.h>
#include <Corrade/Utility/Debug.h>

#include "Magnum/visibility.h"
#include "Magnum/Math/Angle.h"
#include "Magnum/Math/BoolVector.h"
#include "Magnum/Math/TypeTraits.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Utility/Macros.h>
#endif

namespace Magnum { namespace Math {

namespace Implementation {
    template<std::size_t, class, class> struct VectorConverter;
    /* Needed by DualQuaternion and Functions.h (to avoid dependency between them) */
    template<class T, class U> T lerp(const T& a, const T& b, U t) {
        return T((U(1) - t)*a + t*b);
    }

    template<bool integral> struct IsZero;
    template<> struct IsZero<false> {
        template<std::size_t size, class T> bool operator()(const Vector<size, T>& vec) const {
            /* Proper comparison should be with epsilon^2, but the value is not
               representable in given precision. Comparing to epsilon instead. */
            return std::abs(vec.dot()) < TypeTraits<T>::epsilon();
        }
    };
    template<> struct IsZero<true> {
        template<std::size_t size, class T> bool operator()(const Vector<size, T>& vec) const {
            return vec == Vector<size, T>{};
        }
    };
}

/** @relatesalso Vector
@brief Dot product of two vectors

Returns `0` when two vectors are perpendicular, `> 0` when two vectors are in
the same general direction, `1` when two *normalized* vectors are parallel,
`< 0` when two vectors are in opposite general direction and `-1` when two
*normalized* vectors are antiparallel. @f[
    \boldsymbol a \cdot \boldsymbol b = \sum_{i=0}^{n-1} \boldsymbol a_i \boldsymbol b_i
@f]
@see @ref Vector::dot() const, @ref Vector::operator-(), @ref Vector2::perpendicular()
*/
template<std::size_t size, class T> inline T dot(const Vector<size, T>& a, const Vector<size, T>& b) {
    return (a*b).sum();
}

/** @relatesalso Vector
@brief Angle between normalized vectors

Expects that both vectors are normalized. @f[
    \theta = acos \left( \frac{\boldsymbol a \cdot \boldsymbol b}{|\boldsymbol a| |\boldsymbol b|} \right) = acos (\boldsymbol a \cdot \boldsymbol b)
@f]
@see @ref Vector::isNormalized(),
    @ref angle(const Complex<T>&, const Complex<T>&),
    @ref angle(const Quaternion<T>&, const Quaternion<T>&)
*/
template<std::size_t size, class T> inline Rad<T> angle(const Vector<size, T>& normalizedA, const Vector<size, T>& normalizedB) {
    CORRADE_ASSERT(normalizedA.isNormalized() && normalizedB.isNormalized(),
        "Math::angle(): vectors must be normalized", {});
    return Rad<T>(std::acos(dot(normalizedA, normalizedB)));
}

/**
@brief Vector
@tparam size    Vector size
@tparam T       Underlying data type

See @ref matrix-vector for brief introduction.
@configurationvalueref{Magnum::Math::Vector}
*/
template<std::size_t size, class T> class Vector {
    static_assert(size != 0, "Vector cannot have zero elements");

    template<std::size_t, class> friend class Vector;

    public:
        typedef T Type;         /**< @brief Underlying data type */

        enum: std::size_t {
            Size = size         /**< Vector size */
        };

        /**
         * @brief Vector from array
         * @return Reference to the data as if it was Vector, thus doesn't
         *      perform any copying.
         *
         * @attention Use with caution, the function doesn't check whether the
         *      array is long enough.
         */
        static Vector<size, T>& from(T* data) {
            return *reinterpret_cast<Vector<size, T>*>(data);
        }
        /** @overload */
        static const Vector<size, T>& from(const T* data) {
            return *reinterpret_cast<const Vector<size, T>*>(data);
        }

        /**
         * @brief Pad vector
         *
         * If size of @p a is smaller than @ref Size, it is padded from right
         * with @p value, otherwise it's cut.
         * @see @ref Vector4::pad(const Vector<otherSize, T>&, T, T)
         */
        template<std::size_t otherSize> constexpr static Vector<size, T> pad(const Vector<otherSize, T>& a, T value = T(0)) {
            return padInternal<otherSize>(typename Implementation::GenerateSequence<size>::Type(), a, value);
        }

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @copybrief Math::dot(const Vector<size, T>&, const Vector<size, T>&)
         * @deprecated Use @ref Math::dot(const Vector<size, T>&, const Vector<size, T>&)
         *      instead.
         */
        CORRADE_DEPRECATED("use Math::dot() instead") static T dot(const Vector<size, T>& a, const Vector<size, T>& b) {
            return Math::dot(a, b);
        }

        /**
         * @copybrief Math::angle(const Vector<size, T>&, const Vector<size, T>&)
         * @deprecated Use @ref Math::angle(const Vector<size, T>&, const Vector<size, T>&)
         *      instead.
         */
        CORRADE_DEPRECATED("use Math::angle() instead") static Rad<T> angle(const Vector<size, T>& normalizedA, const Vector<size, T>& normalizedB) {
            return Math::angle(normalizedA, normalizedB);
        }
        #endif

        /**
         * @brief Default constructor
         *
         * @f[
         *      \boldsymbol v = \boldsymbol 0
         * @f]
         */
        constexpr /*implicit*/ Vector(ZeroInitT = ZeroInit) noexcept: _data{} {}

        /** @brief Construct vector without initializing the contents */
        explicit Vector(NoInitT) noexcept {}

        /** @brief Construct vector from components */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        template<class ...U> constexpr /*implicit*/ Vector(T first, U... next) noexcept;
        #else
        template<class ...U, class V = typename std::enable_if<sizeof...(U)+1 == size, T>::type> constexpr /*implicit*/ Vector(T first, U... next) noexcept: _data{first, next...} {}
        #endif

        /** @brief Construct vector with one value for all components */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        constexpr explicit Vector(T value) noexcept;
        #else
        template<class U, class V = typename std::enable_if<std::is_same<T, U>::value && size != 1, T>::type> constexpr explicit Vector(U value) noexcept: Vector(typename Implementation::GenerateSequence<size>::Type(), value) {}
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
        template<class U> constexpr explicit Vector(const Vector<size, U>& other) noexcept: Vector(typename Implementation::GenerateSequence<size>::Type(), other) {}

        /** @brief Construct vector from external representation */
        template<class U, class V = decltype(Implementation::VectorConverter<size, T, U>::from(std::declval<U>()))> constexpr explicit Vector(const U& other) noexcept: Vector(Implementation::VectorConverter<size, T, U>::from(other)) {}

        /** @brief Copy constructor */
        constexpr /*implicit*/ Vector(const Vector<size, T>&) noexcept = default;

        /** @brief Convert vector to external representation */
        template<class U, class V = decltype(Implementation::VectorConverter<size, T, U>::to(std::declval<Vector<size, T>>()))> constexpr explicit operator U() const {
            return Implementation::VectorConverter<size, T, U>::to(*this);
        }

        /**
         * @brief Raw data
         * @return One-dimensional array of `size` length.
         *
         * @see @ref operator[]()
         */
        T* data() { return _data; }
        constexpr const T* data() const { return _data; } /**< @overload */

        /**
         * @brief Value at given position
         *
         * @see @ref data()
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
         * @see @ref dot(), @ref normalized()
         */
        bool isZero() const {
            return Implementation::IsZero<std::is_integral<T>::value>{}(*this);
        }

        /**
         * @brief Whether the vector is normalized
         *
         * The vector is normalized if it has unit length: @f[
         *      |\boldsymbol a \cdot \boldsymbol a - 1| < 2 \epsilon + \epsilon^2 \cong 2 \epsilon
         * @f]
         * @see @ref dot(), @ref normalized()
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
         * @see @ref Vector2::perpendicular()
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
         * @see @ref operator+=(), @ref sum()
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
         * @see @ref operator-=()
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
         * @see @ref operator*=(const Vector<size, T>&),
         *      @ref operator*=(Vector<size, Integral>&, FloatingPoint)
         */
        Vector<size, T>& operator*=(T number) {
            for(std::size_t i = 0; i != size; ++i)
                _data[i] *= number;

            return *this;
        }

        /**
         * @brief Multiply vector with number
         *
         * @see @ref operator*(const Vector<size, T>&) const,
         *      @ref operator*=(T), @ref operator*(T, const Vector<size, T>&),
         *      @ref operator*(const Vector<size, Integral>&, FloatingPoint)
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
         * @see @ref operator/=(const Vector<size, T>&),
         *      @ref operator/=(Vector<size, Integral>&, FloatingPoint)
         */
        Vector<size, T>& operator/=(T number) {
            for(std::size_t i = 0; i != size; ++i)
                _data[i] /= number;

            return *this;
        }

        /**
         * @brief Divide vector with number
         *
         * @see @ref operator/(const Vector<size, T>&) const,
         *      @ref operator/=(T), @ref operator/(T, const Vector<size, T>&),
         *      @ref operator/(const Vector<size, Integral>&, FloatingPoint)
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
         * @see @ref operator*=(T),
         *      @ref operator*=(Vector<size, Integral>&, const Vector<size, FloatingPoint>&)
         */
        Vector<size, T>& operator*=(const Vector<size, T>& other) {
            for(std::size_t i = 0; i != size; ++i)
                _data[i] *= other._data[i];

            return *this;
        }

        /**
         * @brief Multiply vector component-wise
         *
         * @see @ref operator*(T) const, @ref operator*=(const Vector<size, T>&),
         *      @ref operator*(const Vector<size, Integral>&, const Vector<size, FloatingPoint>&),
         *      @ref product()
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
         * @see @ref operator/=(T),
         *      @ref operator/=(Vector<size, Integral>&, const Vector<size, FloatingPoint>&)
         */
        Vector<size, T>& operator/=(const Vector<size, T>& other) {
            for(std::size_t i = 0; i != size; ++i)
                _data[i] /= other._data[i];

            return *this;
        }

        /**
         * @brief Divide vector component-wise
         *
         * @see @ref operator/(T) const, @ref operator/=(const Vector<size, T>&),
         *      @ref operator/(const Vector<size, Integral>&, const Vector<size, FloatingPoint>&)
         */
        Vector<size, T> operator/(const Vector<size, T>& other) const {
            return Vector<size, T>(*this) /= other;
        }

        /**
         * @brief Dot product of the vector
         *
         * Should be used instead of @ref length() for comparing vector length
         * with other values, because it doesn't compute the square root. @f[
         *      \boldsymbol a \cdot \boldsymbol a = \sum_{i=0}^{n-1} \boldsymbol a_i^2
         * @f]
         * @see @ref dot(const Vector<size, T>&, const Vector<size, T>&),
         *      @ref isNormalized()
         */
        T dot() const { return Math::dot(*this, *this); }

        /**
         * @brief Vector length
         *
         * See also @ref dot() const which is faster for comparing length with
         * other values. @f[
         *      |\boldsymbol a| = \sqrt{\boldsymbol a \cdot \boldsymbol a}
         * @f]
         * @see @ref lengthInverted(), @ref Math::sqrt(), @ref normalized(),
         *      @ref resized()
         * @todo something like std::hypot() for possibly better precision?
         */
        T length() const { return std::sqrt(dot()); }

        /**
         * @brief Inverse vector length
         *
         * @f[
         *      \frac{1}{|\boldsymbol a|} = \frac{1}{\sqrt{\boldsymbol a \cdot \boldsymbol a}}
         * @f]
         * @see @ref length(), @ref Math::sqrtInverted(), @ref normalized(),
         *      @ref resized()
         */
        T lengthInverted() const { return T(1)/length(); }

        /**
         * @brief Normalized vector (of unit length)
         *
         * @see @ref isNormalized(), @ref lengthInverted(), @ref resized()
         */
        Vector<size, T> normalized() const { return *this*lengthInverted(); }

        /**
         * @brief Resized vector
         *
         * Convenience equivalent to the following code. Due to operation order
         * this function is faster than the obvious way of sizing
         * @ref normalized() vector.
         * @code
         * vec*(vec.lengthInverted()*length) // the brackets are important
         * @endcode
         * @see @ref normalized()
         */
        Vector<size, T> resized(T length) const {
            return *this*(lengthInverted()*length);
        }

        /**
         * @brief Vector projected onto line
         *
         * Returns vector projected onto @p line. @f[
         *      \boldsymbol a_1 = \frac{\boldsymbol a \cdot \boldsymbol b}{\boldsymbol b \cdot \boldsymbol b} \boldsymbol b
         * @f]
         * @see @ref dot(), @ref projectedOntoNormalized()
         */
        Vector<size, T> projected(const Vector<size, T>& line) const {
            return line*Math::dot(*this, line)/line.dot();
        }

        /**
         * @brief Vector projected onto normalized line
         *
         * Slightly faster alternative to @ref projected(), expects @p line to
         * be normalized. @f[
         *      \boldsymbol a_1 = \frac{\boldsymbol a \cdot \boldsymbol b}{\boldsymbol b \cdot \boldsymbol b} \boldsymbol b =
         *          (\boldsymbol a \cdot \boldsymbol b) \boldsymbol b
         * @f]
         * @see @ref dot() const
         */
        Vector<size, T> projectedOntoNormalized(const Vector<size, T>& line) const;

        /**
         * @brief Flipped vector
         *
         * Returns the vector with components in reverse order.
         * @see @ref RectangularMatrix::flippedCols(),
         *      @ref RectangularMatrix::flippedRows()
         */
        constexpr Vector<size, T> flipped() const {
            return flippedInternal(typename Implementation::GenerateSequence<size>::Type{});
        }

        /**
         * @brief Sum of values in the vector
         *
         * @see @ref operator+()
         */
        T sum() const;

        /**
         * @brief Product of values in the vector
         *
         * @see @ref operator*(const Vector<size, T>&) const
         */
        T product() const;

        /**
         * @brief Minimal value in the vector
         *
         * @see @ref Math::min(), @ref minmax()
         */
        T min() const;

        /**
         * @brief Maximal value in the vector
         *
         * @see @ref Math::max(), @ref minmax()
         */
        T max() const;

        /**
         * @brief Minimal and maximal value in the vector
         *
         * @see @ref min(), @ref max(), @ref Math::minmax()
         */
        std::pair<T, T> minmax() const;

    private:
        /* Implementation for Vector<size, T>::Vector(const Vector<size, U>&) */
        template<class U, std::size_t ...sequence> constexpr explicit Vector(Implementation::Sequence<sequence...>, const Vector<size, U>& vector) noexcept: _data{T(vector._data[sequence])...} {}

        /* Implementation for Vector<size, T>::Vector(U) */
        template<std::size_t ...sequence> constexpr explicit Vector(Implementation::Sequence<sequence...>, T value) noexcept: _data{Implementation::repeat(value, sequence)...} {}

        template<std::size_t otherSize, std::size_t ...sequence> constexpr static Vector<size, T> padInternal(Implementation::Sequence<sequence...>, const Vector<otherSize, T>& a, T value) {
            return {sequence < otherSize ? a[sequence] : value...};
        }

        template<std::size_t ...sequence> constexpr Vector<size, T> flippedInternal(Implementation::Sequence<sequence...>) const {
            return {(*this)[size - 1 - sequence]...};
        }

        T _data[size];
};

/** @relates Vector
@brief Multiply number with vector

Same as @ref Vector::operator*(T) const.
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
@see @ref Vector::operator/(T) const
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

Similar to @ref Vector::operator*=(T), except that the multiplication is done
in floating-point. The computation is done in-place.
*/
template<std::size_t size, class Integral, class FloatingPoint> inline
#ifdef DOXYGEN_GENERATING_OUTPUT
Vector<size, Integral>&
#else
typename std::enable_if<std::is_integral<Integral>::value && std::is_floating_point<FloatingPoint>::value, Vector<size, Integral>&>::type
#endif
operator*=(Vector<size, Integral>& vector, FloatingPoint number) {
    for(std::size_t i = 0; i != size; ++i)
        vector[i] = Integral(vector[i]*number);

    return vector;
}

/** @relates Vector
@brief Multiply integral vector with floating-point number

Similar to @ref Vector::operator*(T) const, except that the multiplication is
done in floating-point.
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

Same as @ref operator*(const Vector<size, Integral>&, FloatingPoint).
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

Similar to @ref Vector::operator/=(T), except that the division is done in
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
        vector[i] = Integral(vector[i]/number);

    return vector;
}

/** @relates Vector
@brief Divide integral vector with floating-point number

Similar to @ref Vector::operator/(T) const, except that the division is done in
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

Similar to @ref Vector::operator*=(const Vector<size, T>&), except that the
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
        a[i] = Integral(a[i]*b[i]);

    return a;
}

/** @relates Vector
@brief Multiply integral vector with floating-point vector component-wise

Similar to @ref Vector::operator*(const Vector<size, T>&) const, except that
the multiplication is done in floating-point. The result is always integral
vector, convert both arguments to the same floating-point type to have
floating-point result.
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

Same as @ref operator*(const Vector<size, Integral>&, const Vector<size, FloatingPoint>&).
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

Similar to @ref Vector::operator/=(const Vector<size, T>&), except that the
division is done in floating-point. The computation is done in-place.
*/
template<std::size_t size, class Integral, class FloatingPoint> inline
#ifdef DOXYGEN_GENERATING_OUTPUT
Vector<size, Integral>&
#else
typename std::enable_if<std::is_integral<Integral>::value && std::is_floating_point<FloatingPoint>::value, Vector<size, Integral>&>::type
#endif
operator/=(Vector<size, Integral>& a, const Vector<size, FloatingPoint>& b) {
    for(std::size_t i = 0; i != size; ++i)
        a[i] = Integral(a[i]/b[i]);

    return a;
}

/** @relates Vector
@brief Divide integral vector with floating-point vector component-wise

Similar to @ref Vector::operator/(const Vector<size, T>&) const, except that
the division is done in floating-point. The result is always integral vector,
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
template<std::size_t size, class T> Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug& debug, const Vector<size, T>& value) {
    debug << "Vector(" << Corrade::Utility::Debug::nospace;
    for(std::size_t i = 0; i != size; ++i) {
        if(i != 0) debug << Corrade::Utility::Debug::nospace << ",";
        debug << value[i];
    }
    return debug << Corrade::Utility::Debug::nospace << ")";
}

/* Explicit instantiation for commonly used types */
#ifndef DOXYGEN_GENERATING_OUTPUT
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const Vector<2, Float>&);
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const Vector<3, Float>&);
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const Vector<4, Float>&);
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const Vector<2, Int>&);
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const Vector<3, Int>&);
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const Vector<4, Int>&);
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const Vector<2, UnsignedInt>&);
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const Vector<3, UnsignedInt>&);
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const Vector<4, UnsignedInt>&);
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const Vector<2, Double>&);
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const Vector<3, Double>&);
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const Vector<4, Double>&);
#endif

#ifndef DOXYGEN_GENERATING_OUTPUT
#define MAGNUM_VECTOR_SUBCLASS_IMPLEMENTATION(size, Type)                   \
    static Type<T>& from(T* data) {                                         \
        return *reinterpret_cast<Type<T>*>(data);                           \
    }                                                                       \
    static const Type<T>& from(const T* data) {                             \
        return *reinterpret_cast<const Type<T>*>(data);                     \
    }                                                                       \
    template<std::size_t otherSize> constexpr static Type<T> pad(const Math::Vector<otherSize, T>& a, T value = T(0)) { \
        return Math::Vector<size, T>::pad(a, value);                        \
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
    }                                                                       \
    constexpr Type<T> flipped() const {                                     \
        return Math::Vector<size, T>::flipped();                            \
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
    CORRADE_ASSERT(line.isNormalized(), "Math::Vector::projectedOntoNormalized(): line must be normalized", {});
    return line*Math::dot(*this, line);
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

template<std::size_t size, class T> inline std::pair<T, T> Vector<size, T>::minmax() const {
    T min{_data[0]}, max{_data[0]};

    for(std::size_t i = 1; i != size; ++i) {
        if(_data[i] < min)
            min = _data[i];
        else if(_data[i] > max)
            max = _data[i];
    }

    return {min, max};
}

}}

namespace Corrade { namespace Utility {

/** @configurationvalue{Magnum::Math::Vector} */
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
        } while(pos != std::string::npos && i != size);

        return result;
    }
};

/* Explicit instantiation for commonly used types */
#if !defined(DOXYGEN_GENERATING_OUTPUT) && !defined(__MINGW32__)
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Vector<2, Magnum::Float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Vector<3, Magnum::Float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Vector<4, Magnum::Float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Vector<2, Magnum::Int>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Vector<3, Magnum::Int>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Vector<4, Magnum::Int>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Vector<2, Magnum::UnsignedInt>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Vector<3, Magnum::UnsignedInt>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Vector<4, Magnum::UnsignedInt>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Vector<2, Magnum::Double>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Vector<3, Magnum::Double>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Vector<4, Magnum::Double>>;
#endif

}}

#endif
