#ifndef Magnum_Math_Vector_h
#define Magnum_Math_Vector_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class @ref Magnum::Math::Vector, function @ref Magnum::Math::dot(), @ref Magnum::Math::angle()
 */

#include <utility>
#include <Corrade/Utility/Assert.h>
#ifndef CORRADE_NO_DEBUG
#include <Corrade/Utility/Debug.h>
#endif
#include <Corrade/Utility/StlMath.h>

#include "Magnum/visibility.h"
#include "Magnum/Math/Angle.h"
#include "Magnum/Math/BoolVector.h"
#include "Magnum/Math/TypeTraits.h"

namespace Magnum { namespace Math {

#ifndef DOXYGEN_GENERATING_OUTPUT
/* Documented in Functions.h, defined here because Vector needs them */
template<class T> inline typename std::enable_if<IsScalar<T>::value, bool>::type isNan(T value) {
    return std::isnan(UnderlyingTypeOf<T>(value));
}
/* Keeping the same parameter names as in Functions.h so the note about
   NaN propagation works here too */
template<class T> constexpr typename std::enable_if<IsScalar<T>::value, T>::type min(T value, T min) {
    return min < value ? min : value;
}
template<class T> constexpr typename std::enable_if<IsScalar<T>::value, T>::type max(T value, T max) {
    return value < max ? max : value;
}
template<class T> constexpr typename std::enable_if<IsScalar<T>::value, T>::type clamp(T value, T min, T max) {
    return Math::min(Math::max(value, min), max);
}
#endif

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

    /* Used to make friends to speed up debug builds */
    template<std::size_t, class> struct MatrixDeterminant;
    /* To make gather() / scatter() faster */
    template<std::size_t, std::size_t> struct GatherComponentAt;
    template<std::size_t, std::size_t, bool> struct ScatterComponentOr;
    template<class T, std::size_t valueSize, char, char...> constexpr T scatterRecursive(const T&, const Vector<valueSize, typename T::Type>&, std::size_t);
}

/** @relatesalso Vector
@brief Dot product of two vectors

Returns `0` when two vectors are perpendicular, `> 0` when two vectors are in
the same general direction, `1` when two *normalized* vectors are parallel,
`< 0` when two vectors are in opposite general direction and `-1` when two
* *normalized* vectors are antiparallel. @f[
    \boldsymbol a \cdot \boldsymbol b = \sum_{i=0}^{n-1} \boldsymbol a_i \boldsymbol b_i
@f]
@see @ref Vector::dot() const, @ref Vector::operator-(), @ref Vector2::perpendicular()
*/
template<std::size_t size, class T> inline T dot(const Vector<size, T>& a, const Vector<size, T>& b) {
    T out{};
    for(std::size_t i = 0; i != size; ++i)
        out += a._data[i]*b._data[i];
    return out;
}

/** @relatesalso Vector
@brief Angle between normalized vectors

Expects that both vectors are normalized. Enabled only for floating-point
types. @f[
    \theta = \arccos \left( \frac{\boldsymbol a \cdot \boldsymbol b}{|\boldsymbol a| |\boldsymbol b|} \right) = \arccos (\boldsymbol a \cdot \boldsymbol b)
@f]

To avoid numerical issues when two vectors are very close to each other, the
dot product is clamped to the @f$ [-1, +1] @f$ range before being passed to
@f$ \arccos @f$.
@see @ref Vector::isNormalized(),
    @ref angle(const Complex<T>&, const Complex<T>&),
    @ref angle(const Quaternion<T>&, const Quaternion<T>&)
*/
template<std::size_t size, class FloatingPoint> inline
#ifdef DOXYGEN_GENERATING_OUTPUT
Rad<FloatingPoint>
#else
typename std::enable_if<std::is_floating_point<FloatingPoint>::value, Rad<FloatingPoint>>::type
#endif
angle(const Vector<size, FloatingPoint>& normalizedA, const Vector<size, FloatingPoint>& normalizedB) {
    CORRADE_ASSERT(normalizedA.isNormalized() && normalizedB.isNormalized(),
        "Math::angle(): vectors" << normalizedA << "and" << normalizedB << "are not normalized", {});
    return Rad<FloatingPoint>(std::acos(clamp(dot(normalizedA, normalizedB), FloatingPoint(-1), FloatingPoint(1))));
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

    public:
        typedef T Type;         /**< @brief Underlying data type */

        enum: std::size_t {
            Size = size         /**< Vector size */
        };

        /**
         * @brief Vector from an array
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
        template<std::size_t otherSize> constexpr static Vector<size, T> pad(const Vector<otherSize, T>& a, T value = T()) {
            return padInternal<otherSize>(typename Implementation::GenerateSequence<size>::Type(), a, value);
        }

        /**
         * @brief Default constructor
         *
         * Equivalent to @ref Vector(ZeroInitT).
         */
        constexpr /*implicit*/ Vector() noexcept: _data{} {}

        /**
         * @brief Construct a zero vector
         *
         * @f[
         *      \boldsymbol v = \boldsymbol 0
         * @f]
         */
        constexpr explicit Vector(ZeroInitT) noexcept: _data{} {}

        /** @brief Construct a vector without initializing the contents */
        explicit Vector(Magnum::NoInitT) noexcept {}

        /** @brief Construct a vector from components */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        template<class ...U> constexpr /*implicit*/ Vector(T first, U... next) noexcept;
        #else
        template<class ...U, class V = typename std::enable_if<sizeof...(U)+1 == size, T>::type> constexpr /*implicit*/ Vector(T first, U... next) noexcept: _data{first, next...} {}
        #endif

        /** @brief Construct a vector with one value for all components */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        constexpr explicit Vector(T value) noexcept;
        #else
        template<class U, class V = typename std::enable_if<std::is_same<T, U>::value && size != 1, T>::type> constexpr explicit Vector(U value) noexcept: Vector(typename Implementation::GenerateSequence<size>::Type(), value) {}
        #endif

        /**
         * @brief Construct a vector from another of different type
         *
         * Performs only default casting on the values, no rounding or
         * anything else. Example usage:
         *
         * @snippet MagnumMath.cpp Vector-conversion
         */
        template<class U> constexpr explicit Vector(const Vector<size, U>& other) noexcept: Vector(typename Implementation::GenerateSequence<size>::Type(), other) {}

        /** @brief Construct a vector from external representation */
        template<class U, class V = decltype(Implementation::VectorConverter<size, T, U>::from(std::declval<U>()))> constexpr explicit Vector(const U& other) noexcept: Vector(Implementation::VectorConverter<size, T, U>::from(other)) {}

        /** @brief Copy constructor */
        constexpr /*implicit*/ Vector(const Vector<size, T>&) noexcept = default;

        /** @brief Convert a vector to external representation */
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

        /**
         * @brief Equality comparison
         *
         * @see @ref Math::equal()
         */
        bool operator==(const Vector<size, T>& other) const {
            for(std::size_t i = 0; i != size; ++i)
                if(!TypeTraits<T>::equals(_data[i], other._data[i])) return false;

            return true;
        }

        /**
         * @brief Non-equality comparison
         *
         * @see @ref Math::notEqual()
         */
        bool operator!=(const Vector<size, T>& other) const {
            return !operator==(other);
        }

        /**
         * @brief Component-wise less than comparison
         *
         * @m_keyword{lessThan(),GLSL lessThan(),}
         */
        BoolVector<size> operator<(const Vector<size, T>& other) const;

        /**
         * @brief Component-wise less than or equal comparison
         *
         * @m_keyword{lessThanEqual(),GLSL lessThanEqual(),}
         */
        BoolVector<size> operator<=(const Vector<size, T>& other) const;

        /**
         * @brief Component-wise greater than or equal comparison
         *
         * @m_keyword{greaterThanEqual(),GLSL greaterThanEqual(),}
         */
        BoolVector<size> operator>=(const Vector<size, T>& other) const;

        /**
         * @brief Component-wise greater than comparison
         *
         * @m_keyword{greaterThan(),GLSL greaterThan(),}
         */
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
         * Enabled only for signed types. @f[
         *      \boldsymbol b_i = -\boldsymbol a_i
         * @f]
         * @see @ref Vector2::perpendicular()
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        Vector<size, T>
        #else
        template<class U = T> typename std::enable_if<std::is_signed<U>::value, Vector<size, T>>::type
        #endif
        operator-() const;

        /**
         * @brief Add and assign a vector
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
         * @brief Add a vector
         *
         * @see @ref operator+=(), @ref sum()
         */
        Vector<size, T> operator+(const Vector<size, T>& other) const {
            return Vector<size, T>(*this) += other;
        }

        /**
         * @brief Subtract and assign a vector
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
         * @brief Subtract a vector
         *
         * @see @ref operator-=()
         */
        Vector<size, T> operator-(const Vector<size, T>& other) const {
            return Vector<size, T>(*this) -= other;
        }

        /**
         * @brief Multiply with a scalar and assign
         *
         * The computation is done in-place. @f[
         *      \boldsymbol a_i = b \boldsymbol a_i
         * @f]
         * @see @ref operator*=(const Vector<size, T>&),
         *      @ref operator*=(Vector<size, Integral>&, FloatingPoint)
         */
        Vector<size, T>& operator*=(T scalar) {
            for(std::size_t i = 0; i != size; ++i)
                _data[i] *= scalar;

            return *this;
        }

        /**
         * @brief Multiply with a scalar
         *
         * @see @ref operator*(const Vector<size, T>&) const,
         *      @ref operator*=(T), @ref operator*(T, const Vector<size, T>&),
         *      @ref operator*(const Vector<size, Integral>&, FloatingPoint)
         */
        Vector<size, T> operator*(T scalar) const {
            return Vector<size, T>(*this) *= scalar;
        }

        /**
         * @brief Divide with a scalar and assign
         *
         * The computation is done in-place. @f[
         *      \boldsymbol a_i = \frac{\boldsymbol a_i} b
         * @f]
         * @see @ref operator/=(const Vector<size, T>&),
         *      @ref operator/=(Vector<size, Integral>&, FloatingPoint)
         */
        Vector<size, T>& operator/=(T scalar) {
            for(std::size_t i = 0; i != size; ++i)
                _data[i] /= scalar;

            return *this;
        }

        /**
         * @brief Divide with a scalar
         *
         * @see @ref operator/(const Vector<size, T>&) const,
         *      @ref operator/=(T), @ref operator/(T, const Vector<size, T>&),
         *      @ref operator/(const Vector<size, Integral>&, FloatingPoint)
         */
        Vector<size, T> operator/(T scalar) const {
            return Vector<size, T>(*this) /= scalar;
        }

        /**
         * @brief Multiply a vector component-wise and assign
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
         * @brief Multiply a vector component-wise
         *
         * @see @ref operator*(T) const, @ref operator*=(const Vector<size, T>&),
         *      @ref operator*(const Vector<size, Integral>&, const Vector<size, FloatingPoint>&),
         *      @ref product()
         */
        Vector<size, T> operator*(const Vector<size, T>& other) const {
            return Vector<size, T>(*this) *= other;
        }

        /**
         * @brief Divide a vector component-wise and assign
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
         * @brief Divide a vector component-wise
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
         *
         * For integral types the result may be imprecise, to get a
         * floating-point value of desired precision, cast to a floating-point
         * vector first:
         *
         * @snippet MagnumMath.cpp Vector-length-integer
         *
         * A [Manhattan length](https://en.wikipedia.org/wiki/Taxicab_geometry)
         * might be more suitable than @ref length() in certain cases where the
         * square root is undesirable --- it's a sum of absolute values:
         *
         * @snippet MagnumMath.cpp Vector-length-manhattan
         *
         * @see @ref lengthInverted(), @ref Math::sqrt(), @ref normalized(),
         *      @ref resized()
         * @todo something like std::hypot() for possibly better precision?
         */
        T length() const { return T(std::sqrt(dot())); }

        /**
         * @brief Inverse vector length
         *
         * Enabled only for floating-point types. @f[
         *      \frac{1}{|\boldsymbol a|} = \frac{1}{\sqrt{\boldsymbol a \cdot \boldsymbol a}}
         * @f]
         * @see @ref length(), @ref Math::sqrtInverted(), @ref normalized(),
         *      @ref resized()
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        T
        #else
        template<class U = T> typename std::enable_if<std::is_floating_point<U>::value, T>::type
        #endif
        lengthInverted() const { return T(1)/length(); }

        /**
         * @brief Normalized vector (of unit length)
         *
         * Enabled only for floating-point types.
         * @see @ref isNormalized(), @ref lengthInverted(), @ref resized()
         * @m_keyword{normalize(),GLSL normalize(),}
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        Vector<size, T>
        #else
        template<class U = T> typename std::enable_if<std::is_floating_point<U>::value, Vector<size, T>>::type
        #endif
        normalized() const { return *this*lengthInverted(); }

        /**
         * @brief Resized vector
         *
         * Convenience equivalent to the following code. Due to operation order
         * this function is faster than the obvious way of sizing
         * a @ref normalized() vector. Enabled only for floating-point types.
         *
         * @snippet MagnumMath.cpp Vector-resized
         *
         * @see @ref normalized()
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        Vector<size, T>
        #else
        template<class U = T> typename std::enable_if<std::is_floating_point<U>::value, Vector<size, T>>::type
        #endif
        resized(T length) const {
            return *this*(lengthInverted()*length);
        }

        /**
         * @brief Vector projected onto a line
         *
         * Returns a vector projected onto @p line. Enabled only for
         * floating-point types. @f[
         *      \operatorname{proj}_{\boldsymbol{b}}\,(\boldsymbol{a}) = \frac{\boldsymbol a \cdot \boldsymbol b}{\boldsymbol b \cdot \boldsymbol b} \boldsymbol b
         * @f]
         * @see @ref Math::dot(), @ref projectedOntoNormalized()
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        Vector<size, T>
        #else
        template<class U = T> typename std::enable_if<std::is_floating_point<U>::value, Vector<size, T>>::type
        #endif
        projected(const Vector<size, T>& line) const {
            return line*Math::dot(*this, line)/line.dot();
        }

        /**
         * @brief Vector projected onto a normalized line
         *
         * Slightly faster alternative to @ref projected(), expects @p line to
         * be normalized. Enabled only for floating-point types. @f[
         *      \operatorname{proj}_{\boldsymbol{b}}\,(\boldsymbol{a}) = \frac{\boldsymbol a \cdot \boldsymbol b}{\boldsymbol b \cdot \boldsymbol b} \boldsymbol b =
         *          (\boldsymbol a \cdot \boldsymbol b) \boldsymbol b
         * @f]
         * @see @ref Math::dot()
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        Vector<size, T>
        #else
        template<class U = T> typename std::enable_if<std::is_floating_point<U>::value, Vector<size, T>>::type
        #endif
        projectedOntoNormalized(const Vector<size, T>& line) const;

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
         * @see @ref operator+(), @ref length()
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
         * <em>NaN</em>s are ignored, unless the vector is all <em>NaN</em>s.
         * @see @ref Math::min(), @ref minmax(), @ref Math::isNan()
         */
        T min() const;

        /**
         * @brief Maximal value in the vector
         *
         * <em>NaN</em>s are ignored, unless the vector is all <em>NaN</em>s.
         * @see @ref Math::max(), @ref minmax(), @ref Math::isNan()
         */
        T max() const;

        /**
         * @brief Minimal and maximal value in the vector
         *
         * <em>NaN</em>s are ignored, unless the vector is all <em>NaN</em>s.
         * @see @ref min(), @ref max(), @ref Math::minmax(), @ref Math::isNan()
         */
        std::pair<T, T> minmax() const;

    #ifndef DOXYGEN_GENERATING_OUTPUT
    protected:
    #else
    private:
    #endif
        /* So derived classes can avoid the overhead of operator[] in debug
           builds */
        T _data[size];

    private:
        template<std::size_t, class> friend class Vector;
        /* These three needed to access _data to speed up debug builds */
        template<std::size_t, std::size_t, class> friend class RectangularMatrix;
        template<std::size_t, class> friend class Matrix;
        template<std::size_t, class> friend struct Implementation::MatrixDeterminant;
        /* To make gather() / scatter() faster */
        template<std::size_t, std::size_t> friend struct Implementation::GatherComponentAt;
        template<std::size_t, std::size_t, bool> friend struct Implementation::ScatterComponentOr;
        template<class T_, std::size_t valueSize, char, char...> friend constexpr T_ Implementation::scatterRecursive(const T_&, const Vector<valueSize, typename T_::Type>&, std::size_t);

        /* So the out-of-class comparators can access data directly to avoid
           function call overhead */
        template<std::size_t size_, class T_> friend BoolVector<size_> equal(const Vector<size_, T_>&, const Vector<size_, T_>&);
        template<std::size_t size_, class T_> friend BoolVector<size_> notEqual(const Vector<size_, T_>&, const Vector<size_, T_>&);

        template<std::size_t size_, class U> friend U dot(const Vector<size_, U>&, const Vector<size_, U>&);

        /* Implementation for Vector<size, T>::Vector(const Vector<size, U>&) */
        template<class U, std::size_t ...sequence> constexpr explicit Vector(Implementation::Sequence<sequence...>, const Vector<size, U>& vector) noexcept: _data{T(vector._data[sequence])...} {}

        /* Implementation for Vector<size, T>::Vector(U) */
        template<std::size_t ...sequence> constexpr explicit Vector(Implementation::Sequence<sequence...>, T value) noexcept: _data{Implementation::repeat(value, sequence)...} {}

        template<std::size_t otherSize, std::size_t ...sequence> constexpr static Vector<size, T> padInternal(Implementation::Sequence<sequence...>, const Vector<otherSize, T>& a, T value) {
            return {sequence < otherSize ? a[sequence] : value...};
        }

        template<std::size_t ...sequence> constexpr Vector<size, T> flippedInternal(Implementation::Sequence<sequence...>) const {
            return {_data[size - 1 - sequence]...};
        }
};

/** @relatesalso Vector
@brief Component-wise equality comparison
@m_since{2019,10}

Unlike @ref Vector::operator==() returns a @ref BoolVector instead of a single
value. Vector complement to @ref equal(T, T).
*/
template<std::size_t size, class T> inline BoolVector<size> equal(const Vector<size, T>& a, const Vector<size, T>& b) {
    BoolVector<size> out;

    for(std::size_t i = 0; i != size; ++i)
        out.set(i, TypeTraits<T>::equals(a._data[i], b._data[i]));

    return out;
}

/** @relatesalso Vector
@brief Component-wise non-equality comparison
@m_since{2019,10}

Unlike @ref Vector::operator!=() returns a @ref BoolVector instead of a single
value. Vector complement to @ref notEqual(T, T).
*/
template<std::size_t size, class T> inline BoolVector<size> notEqual(const Vector<size, T>& a, const Vector<size, T>& b) {
    BoolVector<size> out;

    for(std::size_t i = 0; i != size; ++i)
        out.set(i, !TypeTraits<T>::equals(a._data[i], b._data[i]));

    return out;
}

/** @relates Vector
@brief Multiply a scalar with a vector

Same as @ref Vector::operator*(T) const.
*/
template<std::size_t size, class T> inline Vector<size, T> operator*(
    #ifdef DOXYGEN_GENERATING_OUTPUT
    T
    #else
    typename std::common_type<T>::type
    #endif
    scalar, const Vector<size, T>& vector)
{
    return vector*scalar;
}

/** @relates Vector
@brief Divide a vector with a scalar and invert

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
    scalar, const Vector<size, T>& vector)
{
    Vector<size, T> out;

    for(std::size_t i = 0; i != size; ++i)
        out[i] = scalar/vector[i];

    return out;
}

/** @relates Vector
@brief Do modulo of an integral vector and assign

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
@brief Modulo of an integral vector
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
@brief Bitwise NOT of an integral vector
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
@brief Do bitwise left shift of an integral vector and assign

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
@brief Bitwise left shift of an integral vector
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
@brief Do bitwise right shift of an integral vector and assign

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
@brief Bitwise left shift of an integral vector
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
@brief Multiply an integral vector with a floating-point scalar and assign

Similar to @ref Vector::operator*=(T), except that the multiplication is done
in floating-point. The computation is done in-place.
*/
template<std::size_t size, class Integral, class FloatingPoint> inline
#ifdef DOXYGEN_GENERATING_OUTPUT
Vector<size, Integral>&
#else
typename std::enable_if<std::is_integral<Integral>::value && std::is_floating_point<FloatingPoint>::value, Vector<size, Integral>&>::type
#endif
operator*=(Vector<size, Integral>& vector, FloatingPoint scalar) {
    for(std::size_t i = 0; i != size; ++i)
        vector[i] = Integral(vector[i]*scalar);

    return vector;
}

/** @relates Vector
@brief Multiply an integral vector with a floating-point scalar

Similar to @ref Vector::operator*(T) const, except that the multiplication is
done in floating-point.
*/
template<std::size_t size, class Integral, class FloatingPoint> inline
#ifdef DOXYGEN_GENERATING_OUTPUT
Vector<size, Integral>
#else
typename std::enable_if<std::is_integral<Integral>::value && std::is_floating_point<FloatingPoint>::value, Vector<size, Integral>>::type
#endif
operator*(const Vector<size, Integral>& vector, FloatingPoint scalar) {
    Vector<size, Integral> copy(vector);
    return copy *= scalar;
}

/** @relates Vector
@brief Multiply a floating-point scalar with an integral vector

Same as @ref operator*(const Vector<size, Integral>&, FloatingPoint).
*/
template<std::size_t size, class FloatingPoint, class Integral> inline
#ifdef DOXYGEN_GENERATING_OUTPUT
Vector<size, Integral>
#else
typename std::enable_if<std::is_integral<Integral>::value && std::is_floating_point<FloatingPoint>::value, Vector<size, Integral>>::type
#endif
operator*(FloatingPoint scalar, const Vector<size, Integral>& vector) {
    return vector*scalar;
}

/** @relates Vector
@brief Divide an integral vector with a floating-point scalar and assign

Similar to @ref Vector::operator/=(T), except that the division is done in
floating-point. The computation is done in-place.
*/
template<std::size_t size, class Integral, class FloatingPoint> inline
#ifdef DOXYGEN_GENERATING_OUTPUT
Vector<size, Integral>&
#else
typename std::enable_if<std::is_integral<Integral>::value && std::is_floating_point<FloatingPoint>::value, Vector<size, Integral>&>::type
#endif
operator/=(Vector<size, Integral>& vector, FloatingPoint scalar) {
    for(std::size_t i = 0; i != size; ++i)
        vector[i] = Integral(vector[i]/scalar);

    return vector;
}

/** @relates Vector
@brief Divide an integral vector with a floating-point scalar

Similar to @ref Vector::operator/(T) const, except that the division is done in
floating-point.
*/
template<std::size_t size, class Integral, class FloatingPoint> inline
#ifdef DOXYGEN_GENERATING_OUTPUT
Vector<size, Integral>
#else
typename std::enable_if<std::is_integral<Integral>::value && std::is_floating_point<FloatingPoint>::value, Vector<size, Integral>>::type
#endif
operator/(const Vector<size, Integral>& vector, FloatingPoint scalar) {
    Vector<size, Integral> copy(vector);
    return copy /= scalar;
}

/** @relates Vector
@brief Multiply an integral vector with a floating-point vector component-wise and assign

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
@brief Multiply an integral vector with a floating-point vector component-wise

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
@brief Multiply a floating-point vector with an integral vector component-wise

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
@brief Divide an integral vector with a floating-point vector component-wise and assign

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
@brief Divide an integral vector with a floating-point vector component-wise

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

#ifndef CORRADE_NO_DEBUG
/** @debugoperator{Vector} */
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
#endif

#ifndef DOXYGEN_GENERATING_OUTPUT
#define MAGNUM_VECTOR_SUBCLASS_IMPLEMENTATION(size, Type)                   \
    static Type<T>& from(T* data) {                                         \
        return *reinterpret_cast<Type<T>*>(data);                           \
    }                                                                       \
    static const Type<T>& from(const T* data) {                             \
        return *reinterpret_cast<const Type<T>*>(data);                     \
    }                                                                       \
    template<std::size_t otherSize> constexpr static Type<T> pad(const Math::Vector<otherSize, T>& a, T value = T()) { \
        return Math::Vector<size, T>::pad(a, value);                        \
    }                                                                       \
                                                                            \
    template<class U = T> typename std::enable_if<std::is_signed<U>::value, Type<T>>::type \
    operator-() const {                                                     \
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
    template<class U = T> typename std::enable_if<std::is_floating_point<U>::value, Type<T>>::type normalized() const { \
        return Math::Vector<size, T>::normalized();                         \
    }                                                                       \
    template<class U = T> typename std::enable_if<std::is_floating_point<U>::value, Type<T>>::type resized(T length) const { \
        return Math::Vector<size, T>::resized(length);                      \
    }                                                                       \
    template<class U = T> typename std::enable_if<std::is_floating_point<U>::value, Type<T>>::type projected(const Math::Vector<size, T>& other) const {           \
        return Math::Vector<size, T>::projected(other);                     \
    }                                                                       \
    template<class U = T> typename std::enable_if<std::is_floating_point<U>::value, Type<T>>::type projectedOntoNormalized(const Math::Vector<size, T>& other) const { \
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

template<std::size_t size, class T>
#ifdef DOXYGEN_GENERATING_OUTPUT
inline Vector<size, T>
#else
template<class U> inline typename std::enable_if<std::is_signed<U>::value, Vector<size, T>>::type
#endif
Vector<size, T>::operator-() const {
    Vector<size, T> out;

    for(std::size_t i = 0; i != size; ++i)
        out._data[i] = -_data[i];

    return out;
}

template<std::size_t size, class T>
#ifdef DOXYGEN_GENERATING_OUTPUT
inline Vector<size, T>
#else
template<class U> inline typename std::enable_if<std::is_floating_point<U>::value, Vector<size, T>>::type
#endif
Vector<size, T>::projectedOntoNormalized(const Vector<size, T>& line) const {
    CORRADE_ASSERT(line.isNormalized(),
        "Math::Vector::projectedOntoNormalized(): line" << line << "is not normalized", {});
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

namespace Implementation {
    /* Non-floating-point types, the first is a non-NaN for sure */
    template<std::size_t size, class T> constexpr std::size_t firstNonNan(const T(&)[size], std::false_type) {
        return 0;
    }
    /* Floating-point types, return the first that's not NaN */
    template<std::size_t size, class T> inline std::size_t firstNonNan(const T(&data)[size], std::true_type) {
        /* Find the first non-NaN value to compare against. If all are NaN,
           return the last value so the following loop in min/max/minmax()
           doesn't even execute. */
        for(std::size_t i = 0; i != size; ++i)
            if(!isNan(data[i])) return i;
        return size - 1;
    }
}

template<std::size_t size, class T> inline T Vector<size, T>::min() const {
    std::size_t i = Implementation::firstNonNan(_data, IsFloatingPoint<T>{});
    T out(_data[i]);

    for(++i; i != size; ++i)
        out = Math::min(out, _data[i]);

    return out;
}

template<std::size_t size, class T> inline T Vector<size, T>::max() const {
    std::size_t i = Implementation::firstNonNan(_data, IsFloatingPoint<T>{});
    T out(_data[i]);

    for(++i; i != size; ++i)
        out = Math::max(out, _data[i]);

    return out;
}

template<std::size_t size, class T> inline std::pair<T, T> Vector<size, T>::minmax() const {
    std::size_t i = Implementation::firstNonNan(_data, IsFloatingPoint<T>{});
    T min{_data[i]}, max{_data[i]};

    for(++i; i != size; ++i) {
        if(_data[i] < min)
            min = _data[i];
        else if(_data[i] > max)
            max = _data[i];
    }

    return {min, max};
}

namespace Implementation {

template<std::size_t size, class T> struct StrictWeakOrdering<Vector<size, T>> {
    bool operator()(const Vector<size, T>& a, const Vector<size, T>& b) const {
        for(std::size_t i = 0; i < size; ++i) {
            if(a[i] < b[i])
                return true;
            if(a[i] > b[i])
                return false;
        }

        return false; /* a and b are equivalent */
    }
};

}

}}

#endif
