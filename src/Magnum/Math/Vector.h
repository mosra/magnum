#ifndef Magnum_Math_Vector_h
#define Magnum_Math_Vector_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
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
 * @brief Class @ref Magnum::Math::Vector, function @ref Magnum::Math::dot(), @ref Magnum::Math::angle()
 */

/* std::declval() is said to be in <utility> but libstdc++, libc++ and MSVC STL
   all have it directly in <type_traits> because it just makes sense */
#include <type_traits>
#include <Corrade/Containers/Pair.h>
#ifndef CORRADE_SINGLES_NO_DEBUG
#include <Corrade/Utility/Debug.h>
#endif
#include <Corrade/Utility/DebugAssert.h>
#include <Corrade/Utility/StlMath.h>

#include "Magnum/Magnum.h"
#include "Magnum/visibility.h"
#include "Magnum/Math/Angle.h"
#include "Magnum/Math/BitVector.h"
#include "Magnum/Math/TypeTraits.h"

#ifdef MAGNUM_BUILD_DEPRECATED
/* Some APIs returned std::pair before */
#include <Corrade/Containers/PairStl.h>
#endif

namespace Magnum { namespace Math {

#ifndef DOXYGEN_GENERATING_OUTPUT
/* Documented in Functions.h, defined here because Vector needs them */
template<class T, typename std::enable_if<IsScalar<T>::value, int>::type = 0> inline bool isNan(T value) {
    return std::isnan(UnderlyingTypeOf<T>(value));
}
/* Keeping the same parameter names as in Functions.h so the note about
   NaN propagation works here too */
template<class T, typename std::enable_if<IsScalar<T>::value, int>::type = 0> constexpr T min(T value, T min) {
    return min < value ? min : value;
}
template<class T, typename std::enable_if<IsScalar<T>::value, int>::type = 0> constexpr T max(T value, T max) {
    return value < max ? max : value;
}
template<class T, typename std::enable_if<IsScalar<T>::value, int>::type = 0> constexpr T clamp(T value, T min, T max) {
    return Math::min(Math::max(value, min), max);
}
#endif

namespace Implementation {
    template<std::size_t, class, class> struct VectorConverter;
    /* Needed by DualQuaternion and Functions.h (to avoid dependency between them) */
    template<class T, class U> T lerp(const T& a, const T& b, U t) {
        /* While `t*(b - a) + a` is one ALU op less, the following is
           guaranteed to correctly preserves exact boundary values with t being
           0 or 1. See FunctionsTest::lerpLimits() for details. */
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
@see @ref Vector::dot() const, @ref Vector::operator-(),
    @ref Vector2::perpendicular(),
    @ref cross(const Vector3<T>&, const Vector3<T>&),
    @ref cross(const Vector2<T>&, const Vector2<T>&)
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
template<std::size_t size, class FloatingPoint
    #ifndef DOXYGEN_GENERATING_OUTPUT
    , typename std::enable_if<std::is_floating_point<FloatingPoint>::value, int>::type = 0
    #endif
> inline Rad<FloatingPoint> angle(const Vector<size, FloatingPoint>& normalizedA, const Vector<size, FloatingPoint>& normalizedB) {
    CORRADE_DEBUG_ASSERT(normalizedA.isNormalized() && normalizedB.isNormalized(),
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
         * Use with caution, the function doesn't check whether the array is
         * long enough. If possible, prefer to use the
         * @ref Vector(const T(&)[size_]) constructor.
         */
        static Vector<size, T>& from(T* data) {
            return *reinterpret_cast<Vector<size, T>*>(data);
        }
        /** @overload */
        static const Vector<size, T>& from(const T* data) {
            return *reinterpret_cast<const Vector<size, T>*>(data);
        }

        /**
         * @brief Pad a vector
         *
         * If size of @p a is smaller than @ref Size, it is padded from right
         * with @p value, otherwise it's cut.
         * @see @ref Vector4::pad(const Vector<otherSize, T>&, T, T)
         */
        template<std::size_t otherSize> constexpr static Vector<size, T> pad(const Vector<otherSize, T>& a, T value = T()) {
            return padInternal<otherSize>(typename Containers::Implementation::GenerateSequence<size>::Type{}, a, value);
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
        template<class ...U
            #ifndef DOXYGEN_GENERATING_OUTPUT
            , typename std::enable_if<sizeof...(U)+1 == size, int>::type = 0
            #endif
        > constexpr /*implicit*/ Vector(T first, U... next) noexcept: _data{first, next...} {}

        /**
         * @brief Construct a vector from a fixed-size array
         * @m_since_latest
         *
         * Use @ref Vector::from(T*) "from(const T*)" to reinterpret an
         * arbitrary pointer to a vector.
         */
        #if !defined(CORRADE_TARGET_GCC) || defined(CORRADE_TARGET_CLANG) || __GNUC__ >= 5
        template<std::size_t size_> constexpr explicit Vector(const T(&data)[size_]) noexcept: Vector{typename Containers::Implementation::GenerateSequence<size_>::Type{}, data} {
            static_assert(size_ == size, "wrong number of initializers");
        }
        #else
        /* GCC 4.8 isn't able to figure out the size on its own. Which means
           there we use the type-provided size and lose the check for element
           count, but at least it compiles. */
        constexpr explicit Vector(const T(&data)[size]) noexcept: Vector{typename Containers::Implementation::GenerateSequence<size>::Type{}, data} {}
        #endif

        /** @brief Construct a vector with one value for all components */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        constexpr explicit Vector(T value) noexcept; /* showing T instead of U */
        #else
        template<class U, typename std::enable_if<std::is_same<T, U>::value && size != 1, int>::type = 0> constexpr explicit Vector(U value) noexcept: Vector(typename Containers::Implementation::GenerateSequence<size>::Type{}, value) {}
        #endif

        /**
         * @brief Construct a vector from another of different type
         *
         * Performs only default casting on the values, no rounding or
         * anything else. Example usage:
         *
         * @snippet Math.cpp Vector-conversion
         */
        template<class U> constexpr explicit Vector(const Vector<size, U>& other) noexcept: Vector(typename Containers::Implementation::GenerateSequence<size>::Type{}, other) {}

        /**
         * @brief Construct a vector from a BitVector
         * @m_since_latest
         *
         * Bits that are unset are converted to @cpp 0 @ce, set bits to
         * @cpp 1 @ce. If you need a different behavior, for example converting
         * a bit mask to @cpp 0 @ce or @cpp 255 @ce for a color representation,
         * use @ref lerp(const Vector<size, T>&, const Vector<size, T>&, const BitVector<size>&)
         * instead, for example:
         *
         * @snippet Math.cpp Vector-conversion-bit
         */
        constexpr explicit Vector(const BitVector<size>& other) noexcept: Vector{typename Containers::Implementation::GenerateSequence<size>::Type{}, other} {}

        /** @brief Construct a vector from external representation */
        template<class U, class = decltype(Implementation::VectorConverter<size, T, U>::from(std::declval<U>()))> constexpr explicit Vector(const U& other) noexcept: Vector(Implementation::VectorConverter<size, T, U>::from(other)) {}

        /** @brief Convert the vector to external representation */
        template<class U, class = decltype(Implementation::VectorConverter<size, T, U>::to(std::declval<Vector<size, T>>()))> constexpr explicit operator U() const {
            return Implementation::VectorConverter<size, T, U>::to(*this);
        }

        /**
         * @brief Raw data
         *
         * Contrary to what Doxygen shows, returns reference to an
         * one-dimensional fixed-size array of `size` elements, i.e.
         * @cpp T(&)[size] @ce.
         * @see @ref operator[]()
         * @todoc Fix once there's a possibility to patch the signature in a
         *      post-processing step (https://github.com/mosra/m.css/issues/56)
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        T* data();
        constexpr const T* data() const; /**< @overload */
        #else
        auto data() -> T(&)[size] { return _data; }
        constexpr auto data() const -> const T(&)[size] { return _data; }
        #endif

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
         * Done using @ref TypeTraits::equals(), i.e. with fuzzy compare for
         * floating-point types.
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
         * Done using @ref TypeTraits::equals(), i.e. with fuzzy compare for
         * floating-point types.
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
        BitVector<size> operator<(const Vector<size, T>& other) const;

        /**
         * @brief Component-wise less than or equal comparison
         *
         * @m_keyword{lessThanEqual(),GLSL lessThanEqual(),}
         */
        BitVector<size> operator<=(const Vector<size, T>& other) const;

        /**
         * @brief Component-wise greater than or equal comparison
         *
         * @m_keyword{greaterThanEqual(),GLSL greaterThanEqual(),}
         */
        BitVector<size> operator>=(const Vector<size, T>& other) const;

        /**
         * @brief Component-wise greater than comparison
         *
         * @m_keyword{greaterThan(),GLSL greaterThan(),}
         */
        BitVector<size> operator>(const Vector<size, T>& other) const;

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
         * @brief Promotion
         * @m_since_latest
         *
         * Returns the value as-is.
         */
        constexpr Vector<size, T> operator+() const { return *this; }

        /**
         * @brief Negated vector
         *
         * Enabled only for signed types. @f[
         *      \boldsymbol b_i = -\boldsymbol a_i
         * @f]
         * @see @ref flipped(), @ref Vector2::perpendicular()
         */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<class U = T, typename std::enable_if<std::is_signed<U>::value, int>::type = 0>
        #endif
        constexpr Vector<size, T> operator-() const {
            return negateInternal(typename Containers::Implementation::GenerateSequence<size>::Type{});
        }

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
        constexpr Vector<size, T> operator+(const Vector<size, T>& other) const {
            return addInternal(other, typename Containers::Implementation::GenerateSequence<size>::Type{});
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
        constexpr Vector<size, T> operator-(const Vector<size, T>& other) const {
            return subtractInternal(other, typename Containers::Implementation::GenerateSequence<size>::Type{});
        }

        /**
         * @brief Multiply with a scalar and assign
         *
         * The computation is done in-place. @f[
         *      \boldsymbol a_i = b \boldsymbol a_i
         * @f]
         * @see @ref operator*=(const Vector<size, T>&),
         *      @ref operator*=(FloatingPoint)
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
         *      @ref operator*(FloatingPoint) const
         */
        constexpr Vector<size, T> operator*(T scalar) const {
            return multiplyInternal(scalar, typename Containers::Implementation::GenerateSequence<size>::Type{});
        }

        /**
         * @brief Multiply a scalar with a vector
         *
         * Same as @ref operator*(T) const.
         */
        constexpr friend Vector<size, T> operator*(
            #ifdef DOXYGEN_GENERATING_OUTPUT
            T
            #else
            typename std::common_type<T>::type
            #endif
            scalar, const Vector<size, T>& vector)
        {
            return vector*scalar;
        }

        /**
         * @brief Multiply an integral vector with a floating-point scalar and assign
         *
         * Similar to @ref operator*=(T), except that the multiplication is
         * done in floating-point. The computation is done in-place.
         */
        template<class FloatingPoint
            #ifndef DOXYGEN_GENERATING_OUTPUT
            , class Integral = T, typename std::enable_if<std::is_integral<Integral>::value && std::is_floating_point<FloatingPoint>::value, int>::type = 0
            #endif
        > Vector<size, T>& operator*=(FloatingPoint scalar) {
            for(std::size_t i = 0; i != size; ++i)
                _data[i] = T(_data[i]*scalar);

            return *this;
        }

        /**
         * @brief Multiply an integral vector with a floating-point scalar
         *
         * Similar to @ref operator*(T) const, except that the multiplication
         * is done in floating-point.
        */
        template<class FloatingPoint
            #ifndef DOXYGEN_GENERATING_OUTPUT
            , class Integral = T, typename std::enable_if<std::is_integral<Integral>::value && std::is_floating_point<FloatingPoint>::value, int>::type = 0
            #endif
        > constexpr Vector<size, T> operator*(FloatingPoint scalar) const {
            return multiplyIntegerInternal(scalar, typename Containers::Implementation::GenerateSequence<size>::Type{});
        }

        /**
         * @brief Multiply a floating-point scalar with an integral vector
         *
         * Same as @ref operator*(FloatingPoint) const.
         */
        /* Note that this one isn't correctly picked up on MSVC 2015, there's
           an out-of-class overload wrapped in CORRADE_MSVC2015_COMPATIBILITY
           which is (and the two don't conflict, apparently, so both are
           present) */
        template<class FloatingPoint
            #ifndef DOXYGEN_GENERATING_OUTPUT
            , class Integral = T, typename std::enable_if<std::is_integral<Integral>::value && std::is_floating_point<FloatingPoint>::value, int>::type = 0
            #endif
        > friend constexpr Vector<size, T>
        operator*(FloatingPoint scalar, const Vector<size, T>& vector) {
            return vector*scalar;
        }

        /**
         * @brief Divide with a scalar and assign
         *
         * The computation is done in-place. @f[
         *      \boldsymbol a_i = \frac{\boldsymbol a_i} b
         * @f]
         * @see @ref operator/=(const Vector<size, T>&),
         *      @ref operator/=(FloatingPoint)
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
         *      @ref operator/(FloatingPoint) const
         */
        constexpr Vector<size, T> operator/(T scalar) const {
            return divideInternal(scalar, typename Containers::Implementation::GenerateSequence<size>::Type{});
        }

        /**
         * @brief Divide a vector with a scalar and invert
         *
         * @f[
         *      \boldsymbol c_i = \frac b {\boldsymbol a_i}
         * @f]
         * @see @ref operator/(T) const
        */
        friend constexpr Vector<size, T> operator/(
            #ifdef DOXYGEN_GENERATING_OUTPUT
            T
            #else
            typename std::common_type<T>::type
            #endif
            scalar, const Vector<size, T>& vector)
        {
            return divideInternal(scalar, vector, typename Containers::Implementation::GenerateSequence<size>::Type{});
        }

        /**
         * @brief Divide an integral vector with a floating-point scalar and assign
         *
         * Similar to @ref operator/=(T), except that the division is done in
         * floating-point. The computation is done in-place.
        */
        template<class FloatingPoint
            #ifndef DOXYGEN_GENERATING_OUTPUT
            , class Integral = T, typename std::enable_if<std::is_integral<Integral>::value && std::is_floating_point<FloatingPoint>::value, int>::type = 0
            #endif
        > Vector<size, T>& operator/=(FloatingPoint scalar) {
            for(std::size_t i = 0; i != size; ++i)
                _data[i] = T(_data[i]/scalar);

            return *this;
        }

        /**
         * @brief Divide an integral vector with a floating-point scalar
         *
         * Similar to @ref operator/(T) const, except that the division is done
         * in floating-point.
         */
        template<class FloatingPoint
            #ifndef DOXYGEN_GENERATING_OUTPUT
            , class Integral = T, typename std::enable_if<std::is_integral<Integral>::value && std::is_floating_point<FloatingPoint>::value, int>::type = 0
            #endif
        > constexpr Vector<size, T> operator/(FloatingPoint scalar) const {
            return divideIntegerInternal(scalar, typename Containers::Implementation::GenerateSequence<size>::Type{});
        }

        /**
         * @brief Multiply a vector component-wise and assign
         *
         * The computation is done in-place. @f[
         *      \boldsymbol a_i = \boldsymbol a_i \boldsymbol b_i
         * @f]
         * @see @ref operator*=(T),
         *      @ref operator*=(const Vector<size, FloatingPoint>&)
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
         *      @ref operator*(const Vector<size, FloatingPoint>&) const,
         *      @ref product()
         */
        constexpr Vector<size, T> operator*(const Vector<size, T>& other) const {
            return multiplyInternal(other, typename Containers::Implementation::GenerateSequence<size>::Type{});
        }

        /**
         * @brief Multiply an integral vector with a floating-point vector component-wise and assign
         *
         * Similar to @ref operator*=(const Vector<size, T>&), except that the
         * multiplication is done in floating-point. The computation is done
         * in-place.
         */
        template<class FloatingPoint
            #ifndef DOXYGEN_GENERATING_OUTPUT
            , class Integral = T, typename std::enable_if<std::is_integral<Integral>::value && std::is_floating_point<FloatingPoint>::value, int>::type = 0
            #endif
        > Vector<size, T>& operator*=(const Vector<size, FloatingPoint>& other) {
            for(std::size_t i = 0; i != size; ++i)
                _data[i] = T(_data[i]*other._data[i]);

            return *this;
        }

        /**
         * @brief Multiply an integral vector with a floating-point vector component-wise
         *
         * Similar to @ref operator*(const Vector<size, T>&) const, except that
         * the multiplication is done in floating-point. The result is always
         * an integral vector, convert both arguments to the same
         * floating-point type to have a floating-point result.
         */
        template<class FloatingPoint
            #ifndef DOXYGEN_GENERATING_OUTPUT
            , class Integral = T, typename std::enable_if<std::is_integral<Integral>::value && std::is_floating_point<FloatingPoint>::value, int>::type = 0
            #endif
        > constexpr Vector<size, T> operator*(const Vector<size, FloatingPoint>& other) const {
            return multiplyIntegerInternal(other, typename Containers::Implementation::GenerateSequence<size>::Type{});
        }

        /**
         * @brief Multiply a floating-point vector with an integral vector component-wise
         *
         * Same as @ref operator*(const Vector<size, FloatingPoint>&) const.
         */
        /* This was originally friend operator*(const Vector<size, FloatingPoint>&, const Vector<size, T>&),
           but that made it not found on MSVC 2015 and 2017 (and possibly
           newer?) for some reason. Making it a member operator makes it work,
           but it additionally has to prevent a conflict with the
           Integral*FloatingPoint variant above */
        template<class Integral
            #ifndef DOXYGEN_GENERATING_OUTPUT
            , class FloatingPoint = T, typename std::enable_if<std::is_integral<Integral>::value && std::is_floating_point<FloatingPoint>::value, int>::type = 0
            #endif
        > constexpr Vector<size, Integral> operator*(const Vector<size, Integral>& other) const {
            return other**this;
        }

        /**
         * @brief Divide a vector component-wise and assign
         *
         * The computation is done in-place. @f[
         *      \boldsymbol a_i = \frac{\boldsymbol a_i}{\boldsymbol b_i}
         * @f]
         * @see @ref operator/=(T),
         *      @ref operator/=(const Vector<size, FloatingPoint>&)
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
         *      @ref operator/(const Vector<size, FloatingPoint>&) const
         */
        constexpr Vector<size, T> operator/(const Vector<size, T>& other) const {
            return divideInternal(other, typename Containers::Implementation::GenerateSequence<size>::Type{});
        }

        /**
         * @brief Divide an integral vector with a floating-point vector component-wise and assign
         *
         * Similar to @ref operator/=(const Vector<size, T>&), except that the
         * division is done in floating-point. The computation is done
         * in-place.
         */
        template<class FloatingPoint
            #ifndef DOXYGEN_GENERATING_OUTPUT
            , class Integral = T, typename std::enable_if<std::is_integral<Integral>::value && std::is_floating_point<FloatingPoint>::value, int>::type = 0
            #endif
        > Vector<size, T>& operator/=(const Vector<size, FloatingPoint>& other) {
            for(std::size_t i = 0; i != size; ++i)
                _data[i] = T(_data[i]/other._data[i]);

            return *this;
        }

        /**
         * @brief Divide an integral vector with a floating-point vector component-wise
         *
         * Similar to @ref Vector::operator/(const Vector<size, T>&) const,
         * except that the division is done in floating-point. The result is
         * always an integral vector, convert both arguments to the same
         * floating-point type to have a floating-point result.
         */
        template<class FloatingPoint
            #ifndef DOXYGEN_GENERATING_OUTPUT
            , class Integral = T, typename std::enable_if<std::is_integral<Integral>::value && std::is_floating_point<FloatingPoint>::value, int>::type = 0
            #endif
        > constexpr Vector<size, T> operator/(const Vector<size, FloatingPoint>& other) const {
            return divideIntegerInternal(other, typename Containers::Implementation::GenerateSequence<size>::Type{});
        }

        /**
         * @brief Do modulo of a vector and assign
         *
         * Enabled only for integral types. The computation is done in-place.
         */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<class Integral = T, typename std::enable_if<std::is_integral<Integral>::value, int>::type = 0>
        #endif
        Vector<size, T>& operator%=(T scalar) {
            for(std::size_t i = 0; i != size; ++i)
                _data[i] %= scalar;

            return *this;
        }

        /**
         * @brief Modulo of a vector
         *
         * Enabled only for integral types.
         */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<class Integral = T, typename std::enable_if<std::is_integral<Integral>::value, int>::type = 0>
        #endif
        constexpr Vector<size, T> operator%(T scalar) const {
            return moduloInternal(scalar, typename Containers::Implementation::GenerateSequence<size>::Type{});
        }

        /**
         * @brief Do modulo of two vectors and assign
         *
         * Enabled only for integral types. The computation is done in-place.
         */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<class Integral = T, typename std::enable_if<std::is_integral<Integral>::value, int>::type = 0>
        #endif
        Vector<size, T>&  operator%=(const Vector<size, T>& other) {
            for(std::size_t i = 0; i != size; ++i)
                _data[i] %= other._data[i];

            return *this;
        }

        /**
         * @brief Modulo of two vectors
         *
         * Enabled only for integral types.
         */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<class Integral = T, typename std::enable_if<std::is_integral<Integral>::value, int>::type = 0>
        #endif
        constexpr Vector<size, T> operator%(const Vector<size, T>& other) const {
            return moduloInternal(other, typename Containers::Implementation::GenerateSequence<size>::Type{});
        }

        /**
         * @brief Bitwise NOT of a vector
         *
         * Enabled only for integral types.
         */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<class Integral = T, typename std::enable_if<std::is_integral<Integral>::value, int>::type = 0>
        #endif
        constexpr Vector<size, T> operator~() const {
            return invertInternal(typename Containers::Implementation::GenerateSequence<size>::Type{});
        }

        /**
         * @brief Do bitwise AND of two vectors and assign
         *
         * Enabled only for integral types. The computation is done in-place.
         */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<class Integral = T, typename std::enable_if<std::is_integral<Integral>::value, int>::type = 0>
        #endif
        Vector<size, T>& operator&=(const Vector<size, T>& other) {
            for(std::size_t i = 0; i != size; ++i)
                _data[i] &= other._data[i];

            return *this;
        }

        /**
         * @brief Bitwise AND of two vectors
         *
         * Enabled only for integral types.
         */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<class Integral = T, typename std::enable_if<std::is_integral<Integral>::value, int>::type = 0>
        #endif
        constexpr Vector<size, T> operator&(const Vector<size, T>& other) const {
            return andInternal(other, typename Containers::Implementation::GenerateSequence<size>::Type{});
        }

        /**
         * @brief Do bitwise OR of two vectors and assign
         *
         * Enabled only for integral types. The computation is done in-place.
         */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<class Integral = T, typename std::enable_if<std::is_integral<Integral>::value, int>::type = 0>
        #endif
        Vector<size, T>& operator|=(const Vector<size, T>& other) {
            for(std::size_t i = 0; i != size; ++i)
                _data[i] |= other._data[i];

            return *this;
        }

        /**
         * @brief Bitwise OR of two vectors
         *
         * Enabled only for integral types.
         */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<class Integral = T, typename std::enable_if<std::is_integral<Integral>::value, int>::type = 0>
        #endif
        constexpr Vector<size, T> operator|(const Vector<size, T>& other) const {
            return orInternal(other, typename Containers::Implementation::GenerateSequence<size>::Type{});
        }

        /**
         * @brief Do bitwise XOR of two vectors and assign
         *
         * Enabled only for integral types. The computation is done in-place.
         */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<class Integral = T, typename std::enable_if<std::is_integral<Integral>::value, int>::type = 0>
        #endif
        Vector<size, T>& operator^=(const Vector<size, T>& other) {
            for(std::size_t i = 0; i != size; ++i)
                _data[i] ^= other._data[i];

            return *this;
        }

        /**
         * @brief Bitwise XOR of two vectors
         *
         * Enabled only for integral types.
         */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<class Integral = T, typename std::enable_if<std::is_integral<Integral>::value, int>::type = 0>
        #endif
        constexpr Vector<size, T> operator^(const Vector<size, T>& other) const {
            return xorInternal(other, typename Containers::Implementation::GenerateSequence<size>::Type{});
        }

        /**
         * @brief Do bitwise left shift of a vector and assign
         *
         * Enabled only for integral types. The computation is done in-place.
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        Vector<size, T>& operator<<=(T shift)
        #else
        template<class Integral = T, typename std::enable_if<std::is_integral<Integral>::value, int>::type = 0> Vector<size, T>& operator<<=(typename std::common_type<T>::type shift)
        #endif
        {
            for(std::size_t i = 0; i != size; ++i)
                _data[i] <<= shift;

            return *this;
        }

        /**
         * @brief Bitwise left shift of a vector
         *
         * Enabled only for integral types.
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        constexpr Vector<size, T> operator<<(T shift) const
        #else
        template<class Integral = T, typename std::enable_if<std::is_integral<Integral>::value, int>::type = 0> constexpr Vector<size, T> operator<<(typename std::common_type<T>::type shift) const
        #endif
        {
            return shiftLeftInternal(shift, typename Containers::Implementation::GenerateSequence<size>::Type{});
        }

        /**
         * @brief Do bitwise right shift of a vector and assign
         *
         * Enabled only for integral types. The computation is done in-place.
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        Vector<size, T>& operator>>=(T shift)
        #else
        template<class Integral = T, typename std::enable_if<std::is_integral<Integral>::value, int>::type = 0> Vector<size, T>& operator>>=(typename std::common_type<T>::type shift)
        #endif
        {
            for(std::size_t i = 0; i != size; ++i)
                _data[i] >>= shift;

            return *this;
        }

        /**
         * @brief Bitwise left shift of a vector
         *
         * Enabled only for integral types.
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        constexpr Vector<size, T> operator>>(T shift) const
        #else
        template<class Integral = T, typename std::enable_if<std::is_integral<Integral>::value, int>::type = 0>
        constexpr Vector<size, T> operator>>(typename std::common_type<T>::type shift) const
        #endif
        {
            return shiftRightInternal(shift, typename Containers::Implementation::GenerateSequence<size>::Type{});
        }

        /**
         * @brief Dot product of the vector
         *
         * Should be used instead of @ref length() for comparing vector length
         * with other values, because it doesn't compute the square root. @f[
         *      \boldsymbol a \cdot \boldsymbol a = \sum_{i=0}^{n-1} \boldsymbol a_i^2
         * @f]
         * @see @ref dot(const Vector<size, T>&, const Vector<size, T>&),
         *      @ref isNormalized(), @ref Distance::pointPointSquared(),
         *      @ref Intersection::pointSphere()
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
         * @snippet Math.cpp Vector-length-integer
         *
         * A [Manhattan length](https://en.wikipedia.org/wiki/Taxicab_geometry)
         * might be more suitable than @ref length() in certain cases where the
         * square root is undesirable --- it's a sum of absolute values:
         *
         * @snippet Math.cpp Vector-length-manhattan
         *
         * @see @ref lengthInverted(), @ref Math::sqrt(), @ref normalized(),
         *      @ref resized(), @ref Distance::pointPoint(),
         *      @ref Intersection::pointSphere()
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
        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<class U = T, typename std::enable_if<std::is_floating_point<U>::value, int>::type = 0>
        #endif
        T lengthInverted() const { return T(1)/length(); }

        /**
         * @brief Normalized vector (of unit length)
         *
         * Enabled only for floating-point types.
         * @see @ref isNormalized(), @ref lengthInverted(), @ref resized()
         * @m_keyword{normalize(),GLSL normalize(),}
         */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<class U = T, typename std::enable_if<std::is_floating_point<U>::value, int>::type = 0>
        #endif
        Vector<size, T> normalized() const { return *this*lengthInverted(); }

        /**
         * @brief Resized vector
         *
         * Convenience equivalent to the following code. Due to operation order
         * this function is faster than the obvious way of sizing
         * a @ref normalized() vector. Enabled only for floating-point types.
         *
         * @snippet Math.cpp Vector-resized
         *
         * @see @ref normalized()
         */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<class U = T, typename std::enable_if<std::is_floating_point<U>::value, int>::type = 0>
        #endif
        Vector<size, T> resized(T length) const {
            return *this*(lengthInverted()*length);
        }

        /**
         * @brief Vector projected onto a line
         *
         * Returns a vector [projected](https://en.wikipedia.org/wiki/Vector_projection)
         * onto @p line. Enabled only for floating-point types. @f[
         *      \operatorname{proj}_{\boldsymbol{b}}\,(\boldsymbol{a}) = \frac{\boldsymbol a \cdot \boldsymbol b}{\boldsymbol b \cdot \boldsymbol b} \boldsymbol b
         * @f]
         * @see @ref Math::dot(), @ref projectedOntoNormalized()
         */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<class U = T, typename std::enable_if<std::is_floating_point<U>::value, int>::type = 0>
        #endif
        Vector<size, T> projected(const Vector<size, T>& line) const {
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
        #else
        template<class U = T, typename std::enable_if<std::is_floating_point<U>::value, int>::type = 0>
        #endif
        Vector<size, T> projectedOntoNormalized(const Vector<size, T>& line) const;

        /**
         * @brief Flipped vector
         *
         * Returns the vector with components in reverse order. If you want to
         * flip the vector *direction* instead, negate it.
         * @see @ref operator-() const,
         *      @ref RectangularMatrix::flippedCols(),
         *      @ref RectangularMatrix::flippedRows()
         */
        constexpr Vector<size, T> flipped() const {
            return flippedInternal(typename Containers::Implementation::GenerateSequence<size>::Type{});
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
        Containers::Pair<T, T> minmax() const;

    #ifndef DOXYGEN_GENERATING_OUTPUT
    protected:
    #else
    private:
    #endif
        /* So derived classes can avoid the overhead of operator[] in debug
           builds */
        T _data[size];

        /* Implementation for constexpr operators. Not SFINAE-restricted for
           integers or integers + floats, not marked as inline friends, no
           std::common_type<T> workarounds for scalars as the callers do all
           that already. Protected as they're used directly by subclasses
           through the MAGNUM_VECTOR_SUBCLASS_IMPLEMENTATION() macro to speed
           up debug builds. */
        template<std::size_t ...sequence> constexpr Vector<size, T> negateInternal(Containers::Implementation::Sequence<sequence...>) const {
            /* All these explicitly cast to T because with e.g. Vector2s it
               would otherwise cause narrowing warnings because stupid C
               promotion rules make e.g. `short + short` result in an int */
            return {T(-_data[sequence])...};
        }
        template<std::size_t ...sequence> constexpr Vector<size, T> addInternal(const Vector<size, T>& other, Containers::Implementation::Sequence<sequence...>) const {
            return {T(_data[sequence] + other._data[sequence])...};
        }
        template<std::size_t ...sequence> constexpr Vector<size, T> subtractInternal(const Vector<size, T>& other, Containers::Implementation::Sequence<sequence...>) const {
            return {T(_data[sequence] - other._data[sequence])...};
        }
        template<std::size_t ...sequence> constexpr Vector<size, T> multiplyInternal(T scalar, Containers::Implementation::Sequence<sequence...>) const {
            return {T(_data[sequence]*scalar)...};
        }
        template<std::size_t ...sequence, class FloatingPoint> constexpr Vector<size, T> multiplyIntegerInternal(FloatingPoint scalar, Containers::Implementation::Sequence<sequence...>) const {
            /* This has to cast even without C promotion rules in effect, to
               convert a floating-point result back to an integer */
            return {T(_data[sequence]*scalar)...};
        }
        template<std::size_t ...sequence> constexpr Vector<size, T> divideInternal(T scalar, Containers::Implementation::Sequence<sequence...>) const {
            return {T(_data[sequence]/scalar)...};
        }
        template<std::size_t ...sequence, class FloatingPoint> constexpr Vector<size, T> divideIntegerInternal(FloatingPoint scalar, Containers::Implementation::Sequence<sequence...>) const {
            /* This has to cast even without C promotion rules in effect, to
               convert a floating-point result back to an integer */
            return {T(_data[sequence]/scalar)...};
        }
        template<std::size_t ...sequence> constexpr static Vector<size, T> divideInternal(T scalar, const Vector<size, T>& vector, Containers::Implementation::Sequence<sequence...>) {
            return {T(scalar/vector._data[sequence])...};
        }
        template<std::size_t ...sequence> constexpr Vector<size, T> multiplyInternal(const Vector<size, T>& other, Containers::Implementation::Sequence<sequence...>) const {
            return {T(_data[sequence]*other._data[sequence])...};
        }
        template<std::size_t ...sequence, class FloatingPoint> constexpr Vector<size, T> multiplyIntegerInternal(const Vector<size, FloatingPoint>& other, Containers::Implementation::Sequence<sequence...>) const {
            /* This has to cast even without C promotion rules in effect, to
               convert a floating-point result back to an integer */
            return {T(_data[sequence]*other._data[sequence])...};
        }
        template<std::size_t ...sequence> constexpr Vector<size, T> divideInternal(const Vector<size, T>& other, Containers::Implementation::Sequence<sequence...>) const {
            return {T(_data[sequence]/other._data[sequence])...};
        }
        template<std::size_t ...sequence, class FloatingPoint> constexpr Vector<size, T> divideIntegerInternal(const Vector<size, FloatingPoint>& other, Containers::Implementation::Sequence<sequence...>) const {
            /* This has to cast even without C promotion rules in effect, to
               convert a floating-point result back to an integer */
            return {T(_data[sequence]/other._data[sequence])...};
        }

        template<std::size_t ...sequence> constexpr Vector<size, T> moduloInternal(T scalar, Containers::Implementation::Sequence<sequence...>) const {
            return {T(_data[sequence] % scalar)...};
        }
        template<std::size_t ...sequence> constexpr Vector<size, T> moduloInternal(const Math::Vector<size, T>& other, Containers::Implementation::Sequence<sequence...>) const {
            return {T(_data[sequence] % other._data[sequence])...};
        }

        template<std::size_t ...sequence> constexpr Vector<size, T> invertInternal(Containers::Implementation::Sequence<sequence...>) const {
            return {T(~_data[sequence])...};
        }
        template<std::size_t ...sequence> constexpr Vector<size, T> andInternal(const Math::Vector<size, T>& other, Containers::Implementation::Sequence<sequence...>) const {
            return {T(_data[sequence] & other._data[sequence])...};
        }
        template<std::size_t ...sequence> constexpr Vector<size, T> orInternal(const Math::Vector<size, T>& other, Containers::Implementation::Sequence<sequence...>) const {
            return {T(_data[sequence] | other._data[sequence])...};
        }
        template<std::size_t ...sequence> constexpr Vector<size, T> xorInternal(const Math::Vector<size, T>& other, Containers::Implementation::Sequence<sequence...>) const {
            return {T(_data[sequence] ^ other._data[sequence])...};
        }
        template<std::size_t ...sequence> constexpr Vector<size, T> shiftLeftInternal(typename std::common_type<T>::type shift, Containers::Implementation::Sequence<sequence...>) const {
            return {T(_data[sequence] << shift)...};
        }
        template<std::size_t ...sequence> constexpr Vector<size, T> shiftRightInternal(typename std::common_type<T>::type shift, Containers::Implementation::Sequence<sequence...>) const {
            return {T(_data[sequence] >> shift)...};
        }

    private:
        #ifndef DOXYGEN_GENERATING_OUTPUT
        /* Since I added deprecated aliases to Shaders::VectorGL, this FUCKING
           DUMPSTER FIRE DOXYGEN CRAP thinks this refers to Shaders::Vector or
           whatever fucking insane thing. WHAT THE FUCK. */
        template<std::size_t, class> friend class Vector;
        #endif
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
        template<std::size_t size_, class T_> friend BitVector<size_> equal(const Vector<size_, T_>&, const Vector<size_, T_>&);
        template<std::size_t size_, class T_> friend BitVector<size_> notEqual(const Vector<size_, T_>&, const Vector<size_, T_>&);

        template<std::size_t size_, class U> friend U dot(const Vector<size_, U>&, const Vector<size_, U>&);

        /* Implementation for Vector<size, T>::Vector(const T(&data)[size_]) */
        template<std::size_t ...sequence> constexpr explicit Vector(Containers::Implementation::Sequence<sequence...>, const T(&data)[sizeof...(sequence)]) noexcept: _data{data[sequence]...} {}

        /* Implementation for Vector<size, T>::Vector(const Vector<size, U>&) */
        template<class U, std::size_t ...sequence> constexpr explicit Vector(Containers::Implementation::Sequence<sequence...>, const Vector<size, U>& vector) noexcept: _data{T(vector._data[sequence])...} {}

        /* Implementation for Vector<size, T>::Vector(const BitVector<size>&) */
        template<std::size_t ...sequence> constexpr explicit Vector(Containers::Implementation::Sequence<sequence...>, const BitVector<size>& bitVector) noexcept: _data{T(bitVector[sequence])...} {}

        /* Implementation for Vector<size, T>::Vector(U) */
        template<std::size_t ...sequence> constexpr explicit Vector(Containers::Implementation::Sequence<sequence...>, T value) noexcept: _data{Implementation::repeat(value, sequence)...} {}

        template<std::size_t otherSize, std::size_t ...sequence> constexpr static Vector<size, T> padInternal(Containers::Implementation::Sequence<sequence...>, const Vector<otherSize, T>& a, T value) {
            return {sequence < otherSize ? a[sequence] : value...};
        }

        template<std::size_t ...sequence> constexpr Vector<size, T> flippedInternal(Containers::Implementation::Sequence<sequence...>) const {
            return {_data[size - 1 - sequence]...};
        }
};

/** @relatesalso Vector
@brief Component-wise equality comparison
@m_since{2019,10}

Unlike @ref Vector::operator==() returns a @ref BitVector instead of a single
value. Vector complement to @ref equal(T, T).
*/
template<std::size_t size, class T> inline BitVector<size> equal(const Vector<size, T>& a, const Vector<size, T>& b) {
    BitVector<size> out;

    for(std::size_t i = 0; i != size; ++i)
        out.set(i, TypeTraits<T>::equals(a._data[i], b._data[i]));

    return out;
}

/** @relatesalso Vector
@brief Component-wise non-equality comparison
@m_since{2019,10}

Unlike @ref Vector::operator!=() returns a @ref BitVector instead of a single
value. Vector complement to @ref notEqual(T, T).
*/
template<std::size_t size, class T> inline BitVector<size> notEqual(const Vector<size, T>& a, const Vector<size, T>& b) {
    BitVector<size> out;

    for(std::size_t i = 0; i != size; ++i)
        out.set(i, !TypeTraits<T>::equals(a._data[i], b._data[i]));

    return out;
}

#ifndef CORRADE_SINGLES_NO_DEBUG
/** @debugoperator{Vector} */
template<std::size_t size, class T> Debug& operator<<(Debug& debug, const Vector<size, T>& value) {
    /* Nested values should get printed with the same flags, so make all
       immediate flags temporarily global -- except NoSpace, unless it's also
       set globally */
    const Utility::Debug::Flags prevFlags = debug.flags();
    debug.setFlags(prevFlags | (debug.immediateFlags() & ~Utility::Debug::Flag::NoSpace));

    const bool packed = debug.immediateFlags() >= Debug::Flag::Packed;
    debug << (packed ? "{" : "Vector(") << Debug::nospace;
    for(std::size_t i = 0; i != size; ++i) {
        if(i != 0) debug << Debug::nospace << ",";
        debug << value[i];
    }
    debug << Debug::nospace << (packed ? "}" : ")");

    /* Reset the original flags back */
    debug.setFlags(prevFlags);

    return debug;
}

/* Explicit instantiation for commonly used types */
#ifndef DOXYGEN_GENERATING_OUTPUT
extern template MAGNUM_EXPORT Debug& operator<<(Debug&, const Vector<2, Float>&);
extern template MAGNUM_EXPORT Debug& operator<<(Debug&, const Vector<3, Float>&);
extern template MAGNUM_EXPORT Debug& operator<<(Debug&, const Vector<4, Float>&);
extern template MAGNUM_EXPORT Debug& operator<<(Debug&, const Vector<2, Int>&);
extern template MAGNUM_EXPORT Debug& operator<<(Debug&, const Vector<3, Int>&);
extern template MAGNUM_EXPORT Debug& operator<<(Debug&, const Vector<4, Int>&);
extern template MAGNUM_EXPORT Debug& operator<<(Debug&, const Vector<2, UnsignedInt>&);
extern template MAGNUM_EXPORT Debug& operator<<(Debug&, const Vector<3, UnsignedInt>&);
extern template MAGNUM_EXPORT Debug& operator<<(Debug&, const Vector<4, UnsignedInt>&);
extern template MAGNUM_EXPORT Debug& operator<<(Debug&, const Vector<2, Double>&);
extern template MAGNUM_EXPORT Debug& operator<<(Debug&, const Vector<3, Double>&);
extern template MAGNUM_EXPORT Debug& operator<<(Debug&, const Vector<4, Double>&);
#endif
#endif

#ifndef DOXYGEN_GENERATING_OUTPUT
#define MAGNUM_VECTOR_SUBCLASS_IMPLEMENTATION(size, Type_)                  \
    static Type_<T>& from(T* data) {                                        \
        return *reinterpret_cast<Type_<T>*>(data);                          \
    }                                                                       \
    static const Type_<T>& from(const T* data) {                            \
        return *reinterpret_cast<const Type_<T>*>(data);                    \
    }                                                                       \
    template<std::size_t otherSize> constexpr static Type_<T> pad(const Math::Vector<otherSize, T>& a, T value = T()) { \
        return Math::Vector<size, T>::pad(a, value);                        \
    }                                                                       \
                                                                            \
    constexpr Type_<T> operator+() const {                                  \
        return Math::Vector<size, T>::operator+();                          \
    }                                                                       \
    template<class U = T, typename std::enable_if<std::is_signed<U>::value, int>::type = 0> constexpr Type_<T> operator-() const { \
        return Math::Vector<size, T>::negateInternal(typename Containers::Implementation::GenerateSequence<size>::Type{}); \
    }                                                                       \
    Type_<T>& operator+=(const Math::Vector<size, T>& other) {              \
        Math::Vector<size, T>::operator+=(other);                           \
        return *this;                                                       \
    }                                                                       \
    constexpr Type_<T> operator+(const Math::Vector<size, T>& other) const { \
        return Math::Vector<size, T>::addInternal(other, typename Containers::Implementation::GenerateSequence<size>::Type{}); \
    }                                                                       \
    Type_<T>& operator-=(const Math::Vector<size, T>& other) {              \
        Math::Vector<size, T>::operator-=(other);                           \
        return *this;                                                       \
    }                                                                       \
    constexpr Type_<T> operator-(const Math::Vector<size, T>& other) const { \
        return Math::Vector<size, T>::subtractInternal(other, typename Containers::Implementation::GenerateSequence<size>::Type{}); \
    }                                                                       \
                                                                            \
    Type_<T>& operator*=(T scalar) {                                        \
        Math::Vector<size, T>::operator*=(scalar);                          \
        return *this;                                                       \
    }                                                                       \
    constexpr Type_<T> operator*(T scalar) const {                          \
        return Math::Vector<size, T>::multiplyInternal(scalar, typename Containers::Implementation::GenerateSequence<size>::Type{}); \
    }                                                                       \
    friend constexpr Type_<T> operator*(typename std::common_type<T>::type scalar, const Type_<T>& vector) { \
        return scalar*static_cast<const Math::Vector<size, T>&>(vector);    \
    }                                                                       \
    template<class FloatingPoint, class Integral = T, typename std::enable_if<std::is_integral<Integral>::value && std::is_floating_point<FloatingPoint>::value, int>::type = 0> Type_<T>& operator*=(FloatingPoint scalar) { \
        Math::Vector<size, T>::operator*=(scalar);                          \
        return *this;                                                       \
    }                                                                       \
    template<class FloatingPoint, class Integral = T, typename std::enable_if<std::is_integral<Integral>::value && std::is_floating_point<FloatingPoint>::value, int>::type = 0> constexpr Type_<T> operator*(FloatingPoint scalar) const { \
        return Math::Vector<size, T>::multiplyIntegerInternal(scalar, typename Containers::Implementation::GenerateSequence<size>::Type{}); \
    }                                                                       \
    template<class FloatingPoint, class Integral = T, typename std::enable_if<std::is_integral<Integral>::value && std::is_floating_point<FloatingPoint>::value, int>::type = 0> friend constexpr Type_<T> operator*(FloatingPoint scalar, const Type_<T>& vector) { \
        return scalar*static_cast<const Math::Vector<size, T>&>(vector);    \
    }                                                                       \
                                                                            \
    Type_<T>& operator/=(T scalar) {                                        \
        Math::Vector<size, T>::operator/=(scalar);                          \
        return *this;                                                       \
    }                                                                       \
    constexpr Type_<T> operator/(T scalar) const {                          \
        return Math::Vector<size, T>::divideInternal(scalar, typename Containers::Implementation::GenerateSequence<size>::Type{}); \
    }                                                                       \
    friend constexpr Type_<T> operator/(typename std::common_type<T>::type scalar, const Type_<T>& vector) { \
        return scalar/static_cast<const Math::Vector<size, T>&>(vector);    \
    }                                                                       \
    template<class FloatingPoint, class Integral = T, typename std::enable_if<std::is_integral<Integral>::value && std::is_floating_point<FloatingPoint>::value, int>::type = 0> Type_<T>& operator/=(FloatingPoint scalar) { \
        Math::Vector<size, T>::operator/=(scalar);                          \
        return *this;                                                       \
    }                                                                       \
    template<class FloatingPoint, class Integral = T, typename std::enable_if<std::is_integral<Integral>::value && std::is_floating_point<FloatingPoint>::value, int>::type = 0> constexpr Type_<T> operator/(FloatingPoint scalar) const { \
        return Math::Vector<size, T>::divideIntegerInternal(scalar, typename Containers::Implementation::GenerateSequence<size>::Type{}); \
    }                                                                       \
                                                                            \
    Type_<T>& operator*=(const Math::Vector<size, T>& other) {              \
        Math::Vector<size, T>::operator*=(other);                           \
        return *this;                                                       \
    }                                                                       \
    constexpr Type_<T> operator*(const Math::Vector<size, T>& other) const { \
        return Math::Vector<size, T>::multiplyInternal(other, typename Containers::Implementation::GenerateSequence<size>::Type{}); \
    }                                                                       \
    template<class FloatingPoint, class Integral = T, typename std::enable_if<std::is_integral<Integral>::value && std::is_floating_point<FloatingPoint>::value, int>::type = 0> Type_<T>& operator*=(const Math::Vector<size, FloatingPoint>& other) { \
        Math::Vector<size, T>::operator*=(other);                           \
        return *this;                                                       \
    }                                                                       \
    template<class FloatingPoint, class Integral = T, typename std::enable_if<std::is_integral<Integral>::value && std::is_floating_point<FloatingPoint>::value, int>::type = 0> constexpr Type_<T> operator*(const Math::Vector<size, FloatingPoint>& other) const { \
        return Math::Vector<size, T>::multiplyIntegerInternal(other, typename Containers::Implementation::GenerateSequence<size>::Type{}); \
    }                                                                       \
    template<class Integral, class FloatingPoint = T, typename std::enable_if<std::is_integral<Integral>::value && std::is_floating_point<FloatingPoint>::value, int>::type = 0> constexpr Type_<Integral> operator*(const Math::Vector<size, Integral>& other) const { \
        return other**this;                                                 \
    }                                                                       \
                                                                            \
    Type_<T>& operator/=(const Math::Vector<size, T>& other) {              \
        Math::Vector<size, T>::operator/=(other);                           \
        return *this;                                                       \
    }                                                                       \
    constexpr Type_<T> operator/(const Math::Vector<size, T>& other) const { \
        return Math::Vector<size, T>::divideInternal(other, typename Containers::Implementation::GenerateSequence<size>::Type{}); \
    }                                                                       \
    template<class FloatingPoint, class Integral = T, typename std::enable_if<std::is_integral<Integral>::value && std::is_floating_point<FloatingPoint>::value, int>::type = 0> Type_<T>& operator/=(const Math::Vector<size, FloatingPoint>& other) { \
        Math::Vector<size, T>::operator/=(other);                           \
        return *this;                                                       \
    }                                                                       \
    template<class FloatingPoint, class Integral = T, typename std::enable_if<std::is_integral<Integral>::value && std::is_floating_point<FloatingPoint>::value, int>::type = 0> constexpr Type_<T> operator/(const Math::Vector<size, FloatingPoint>& other) const { \
        return Math::Vector<size, T>::divideIntegerInternal(other, typename Containers::Implementation::GenerateSequence<size>::Type{}); \
    }                                                                       \
                                                                            \
    template<class Integral = T, typename std::enable_if<std::is_integral<Integral>::value, int>::type = 0> Type_<T>& operator%=(T scalar) { \
        Math::Vector<size, T>::operator%=(scalar);                          \
        return *this;                                                       \
    }                                                                       \
    template<class Integral = T, typename std::enable_if<std::is_integral<Integral>::value, int>::type = 0> constexpr Type_<T> operator%(T scalar) const { \
        return Math::Vector<size, T>::moduloInternal(scalar, typename Containers::Implementation::GenerateSequence<size>::Type{}); \
    }                                                                       \
    template<class Integral = T, typename std::enable_if<std::is_integral<Integral>::value, int>::type = 0> Type_<T>& operator%=(const Math::Vector<size, T>& other) { \
        Math::Vector<size, T>::operator%=(other);                           \
        return *this;                                                       \
    }                                                                       \
    template<class Integral = T, typename std::enable_if<std::is_integral<Integral>::value, int>::type = 0> constexpr Type_<T> operator%(const Math::Vector<size, T>& other) const { \
        return Math::Vector<size, T>::moduloInternal(other, typename Containers::Implementation::GenerateSequence<size>::Type{}); \
    }                                                                       \
                                                                            \
    template<class Integral = T, typename std::enable_if<std::is_integral<Integral>::value, int>::type = 0> constexpr Type_<T> operator~() const { \
        return Math::Vector<size, T>::invertInternal(typename Containers::Implementation::GenerateSequence<size>::Type{}); \
    }                                                                       \
    template<class Integral = T, typename std::enable_if<std::is_integral<Integral>::value, int>::type = 0> Type_<T>& operator&=(const Math::Vector<size, T>& other) { \
        Math::Vector<size, T>::operator&=(other);                           \
        return *this;                                                       \
    }                                                                       \
    template<class Integral = T, typename std::enable_if<std::is_integral<Integral>::value, int>::type = 0> constexpr Type_<T> operator&(const Math::Vector<size, T>& other) const { \
        return Math::Vector<size, T>::andInternal(other, typename Containers::Implementation::GenerateSequence<size>::Type{}); \
    }                                                                       \
    template<class Integral = T, typename std::enable_if<std::is_integral<Integral>::value, int>::type = 0> Type_<T>& operator|=(const Math::Vector<size, T>& other) { \
        Math::Vector<size, T>::operator|=(other);                           \
        return *this;                                                       \
    }                                                                       \
    template<class Integral = T, typename std::enable_if<std::is_integral<Integral>::value, int>::type = 0> constexpr Type_<T> operator|(const Math::Vector<size, T>& other) const { \
        return Math::Vector<size, T>::orInternal(other, typename Containers::Implementation::GenerateSequence<size>::Type{}); \
    }                                                                       \
    template<class Integral = T, typename std::enable_if<std::is_integral<Integral>::value, int>::type = 0> Type_<T>& operator^=(const Math::Vector<size, T>& other) { \
        Math::Vector<size, T>::operator^=(other);                           \
        return *this;                                                       \
    }                                                                       \
    template<class Integral = T, typename std::enable_if<std::is_integral<Integral>::value, int>::type = 0> constexpr Type_<T> operator^(const Math::Vector<size, T>& other) const { \
        return Math::Vector<size, T>::xorInternal(other, typename Containers::Implementation::GenerateSequence<size>::Type{}); \
    }                                                                       \
    template<class Integral = T, typename std::enable_if<std::is_integral<Integral>::value, int>::type = 0> Type_<T>& operator<<=(typename std::common_type<T>::type shift) { \
        Math::Vector<size, T>::operator<<=(shift);                          \
        return *this;                                                       \
    }                                                                       \
    template<class Integral = T, typename std::enable_if<std::is_integral<Integral>::value, int>::type = 0> constexpr Type_<T> operator<<(typename std::common_type<T>::type shift) const { \
        return Math::Vector<size, T>::shiftLeftInternal(shift, typename Containers::Implementation::GenerateSequence<size>::Type{}); \
    }                                                                       \
    template<class Integral = T, typename std::enable_if<std::is_integral<Integral>::value, int>::type = 0> Type_<T>& operator>>=(typename std::common_type<T>::type shift) { \
        Math::Vector<size, T>::operator>>=(shift);                          \
        return *this;                                                       \
    }                                                                       \
    template<class Integral = T, typename std::enable_if<std::is_integral<Integral>::value, int>::type = 0> constexpr Type_<T> operator>>(typename std::common_type<T>::type shift) const { \
        return Math::Vector<size, T>::shiftRightInternal(shift, typename Containers::Implementation::GenerateSequence<size>::Type{}); \
    }                                                                       \
                                                                            \
    template<class U = T, typename std::enable_if<std::is_floating_point<U>::value, int>::type = 0> Type_<T> normalized() const { \
        return Math::Vector<size, T>::normalized();                         \
    }                                                                       \
    template<class U = T, typename std::enable_if<std::is_floating_point<U>::value, int>::type = 0> Type_<T> resized(T length) const { \
        return Math::Vector<size, T>::resized(length);                      \
    }                                                                       \
    template<class U = T, typename std::enable_if<std::is_floating_point<U>::value, int>::type = 0> Type_<T> projected(const Math::Vector<size, T>& other) const { \
        return Math::Vector<size, T>::projected(other);                     \
    }                                                                       \
    template<class U = T, typename std::enable_if<std::is_floating_point<U>::value, int>::type = 0> Type_<T> projectedOntoNormalized(const Math::Vector<size, T>& other) const { \
        return Math::Vector<size, T>::projectedOntoNormalized(other);       \
    }                                                                       \
    constexpr Type_<T> flipped() const {                                    \
        return Math::Vector<size, T>::flipped();                            \
    }
#endif

#ifdef CORRADE_MSVC2015_COMPATIBILITY
/* MSVC 2015 doesn't correctly pick up the in-class inline friend that does
   this, resulting in float*VectorNi expressions being wrongly executed as
   int*VectorNi due to an implicit conversion fallback. This overload is picked
   up correctly (and doesn't conflict with the in-class one), subclasses then
   need to use the MAGNUM_VECTORn_OPERATOR_IMPLEMENTATION() overloads as well
   to return a correct subtype. See VectorTest::multiplyDivideIntegral(),
   VectorTest::subclass() and corresponding cases in Vector2Test, Vector3Test,
   Vector4Test and ColorTest for regression tests. The same issue and a
   matching workaround is done in Unit as well. */
template<std::size_t size, class FloatingPoint, class Integral, typename std::enable_if<std::is_integral<Integral>::value && std::is_floating_point<FloatingPoint>::value, int>::type = 0> constexpr Vector<size, Integral> operator*(FloatingPoint scalar, const Vector<size, Integral>& vector) {
    return vector*scalar;
}

#define MAGNUM_VECTORn_OPERATOR_IMPLEMENTATION(size, Type)                   \
    template<class FloatingPoint, class Integral, typename std::enable_if<std::is_integral<Integral>::value && std::is_floating_point<FloatingPoint>::value, int>::type = 0> constexpr Type<Integral> operator*(FloatingPoint scalar, const Type<Integral>& vector) { \
        return vector*scalar;                                               \
    }
#endif

template<std::size_t size, class T> inline BitVector<size> Vector<size, T>::operator<(const Vector<size, T>& other) const {
    BitVector<size> out;

    for(std::size_t i = 0; i != size; ++i)
        out.set(i, _data[i] < other._data[i]);

    return out;
}

template<std::size_t size, class T> inline BitVector<size> Vector<size, T>::operator<=(const Vector<size, T>& other) const {
    BitVector<size> out;

    for(std::size_t i = 0; i != size; ++i)
        out.set(i, _data[i] <= other._data[i]);

    return out;
}

template<std::size_t size, class T> inline BitVector<size> Vector<size, T>::operator>=(const Vector<size, T>& other) const {
    BitVector<size> out;

    for(std::size_t i = 0; i != size; ++i)
        out.set(i, _data[i] >= other._data[i]);

    return out;
}

template<std::size_t size, class T> inline BitVector<size> Vector<size, T>::operator>(const Vector<size, T>& other) const {
    BitVector<size> out;

    for(std::size_t i = 0; i != size; ++i)
        out.set(i, _data[i] > other._data[i]);

    return out;
}

template<std::size_t size, class T>
#ifndef DOXYGEN_GENERATING_OUTPUT
template<class U, typename std::enable_if<std::is_floating_point<U>::value, int>::type>
#endif
inline Vector<size, T> Vector<size, T>::projectedOntoNormalized(const Vector<size, T>& line) const {
    CORRADE_DEBUG_ASSERT(line.isNormalized(),
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

template<std::size_t size, class T> inline Containers::Pair<T, T> Vector<size, T>::minmax() const {
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

#ifndef MAGNUM_NO_MATH_STRICT_WEAK_ORDERING
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
#endif

}}

#endif
