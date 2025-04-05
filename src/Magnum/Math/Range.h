#ifndef Magnum_Math_Range_h
#define Magnum_Math_Range_h
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
 * @brief Class @ref Magnum::Math::Range, @ref Magnum::Math::Range2D, @ref Magnum::Math::Range3D, alias @ref Magnum::Math::Range1D
 */

/* std::declval() is said to be in <utility> but libstdc++, libc++ and MSVC STL
   all have it directly in <type_traits> because it just makes sense */
#include <type_traits>

#include "Magnum/Math/Functions.h"
#include "Magnum/Math/Vector3.h"

#ifdef MAGNUM_BUILD_DEPRECATED
/* Some APIs were taking a std::pair before */
#include <Corrade/Containers/PairStl.h>
#endif

namespace Magnum { namespace Math {

namespace Implementation {
    template<UnsignedInt, class> struct RangeTraits;

    template<class T> struct RangeTraits<1, T> {
        typedef T Type;
        constexpr static Type fromVector(const Vector<1, T>& value) {
            return value[0];
        }
    };
    template<class T> struct RangeTraits<2, T> {
        typedef Vector2<T> Type;
        constexpr static Type fromVector(const Vector<2, T>& value) {
            return value;
        }
    };
    template<class T> struct RangeTraits<3, T> {
        typedef Vector3<T> Type;
        constexpr static Type fromVector(const Vector<3, T>& value) {
            return value;
        }
    };

    template<UnsignedInt, class, class> struct RangeConverter;
}

/**
@brief N-dimensional range

@m_keywords{AABB}

Axis-aligned line (in 1D), rectangle (in 2D) or box (in 3D). The minimal
coordinate is inclusive, maximal exclusive. See @ref Range1D, @ref Range2D and
@ref Range3D specializations for given dimension count.

@section Math-Range-generic Use in generic code

While @ref Range2D and @ref Range3D have a vector underlying type, @ref Range1D
is just a scalar. This makes common usage simpler, but may break in generic
code that expects a vector type for any dimension. Solution is to
unconditionally cast the value to a vector type or explicitly specify template
parameters to choose a vector function overload. For example:

@snippet Math.cpp Range-generic
*/
template<UnsignedInt dimensions, class T> class Range {
    template<UnsignedInt, class> friend class Range;

    public:
        /**
         * @brief Underlying vector type
         *
         * @cpp T @ce in 1D, @ref Math::Vector2 "Vector2<T>" in 2D,
         * @ref Math::Vector3 "Vector3<T>" in 3D.
         */
        typedef typename Implementation::RangeTraits<dimensions, T>::Type VectorType;

        /**
         * @brief Create a range from minimal coordinates and a size
         * @param min   Minimal coordinates
         * @param size  Range size
         */
        static Range<dimensions, T> fromSize(const VectorType& min, const VectorType& size) {
            return {min, min+size};
        }

        /**
         * @brief Create a range from a center and a half size
         * @param center    Range center
         * @param halfSize  Half size
         *
         * For creating integer center ranges you can use @ref fromSize()
         * together with @ref padded(), for example:
         *
         * @snippet Math.cpp Range-fromCenter-integer
         */
        static Range<dimensions, T> fromCenter(const VectorType& center, const VectorType& halfSize) {
            return {center - halfSize, center + halfSize};
        }

        /**
         * @brief Default constructor
         *
         * Equivalent to @ref Range(ZeroInitT).
         */
        constexpr /*implicit*/ Range() noexcept: Range<dimensions, T>{ZeroInit, typename std::conditional<dimensions == 1, void*, ZeroInitT*>::type{}} {}

        /**
         * @brief Construct a zero range
         *
         * Construct a zero-size range positioned at origin.
         */
        constexpr explicit Range(ZeroInitT) noexcept: Range<dimensions, T>{ZeroInit, typename std::conditional<dimensions == 1, void*, ZeroInitT*>::type{}} {}

        /** @brief Construct without initializing the contents */
        explicit Range(Magnum::NoInitT) noexcept: Range<dimensions, T>{Magnum::NoInit, typename std::conditional<dimensions == 1, void*, Magnum::NoInitT*>::type{}} {}

        /** @brief Construct a range from minimal and maximal coordinates */
        constexpr /*implicit*/ Range(const VectorType& min, const VectorType& max) noexcept: _min{min}, _max{max} {}
        /** @overload */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<UnsignedInt d = dimensions, typename std::enable_if<d == 1, int>::type = 0>
        #endif
        constexpr /*implicit*/ Range(const Vector<dimensions, T>& min, const Vector<dimensions, T>& max) noexcept: _min{Implementation::RangeTraits<dimensions, T>::fromVector(min)}, _max{Implementation::RangeTraits<dimensions, T>::fromVector(max)} {}

        /**
         * @brief Construct a range from a pair of minimal and maximal coordinates
         *
         * Useful in combination with e.g. @ref minmax(), here for example to
         * calculate bounds of a triangle:
         *
         * @snippet Math.cpp Range-construct-minmax3D
         */
        constexpr /*implicit*/ Range(const Containers::Pair<VectorType, VectorType>& minmax) noexcept:
            _min{minmax.first()}, _max{minmax.second()} {}

        /** @overload */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<UnsignedInt d = dimensions, typename std::enable_if<d != 1, int>::type = 0>
        #endif
        constexpr /*implicit*/ Range(const Containers::Pair<Vector<dimensions, T>, Vector<dimensions, T>>& minmax) noexcept: _min{minmax.first()}, _max{minmax.second()} {}

        /**
         * @brief Construct a range from another of different type
         *
         * Performs only default casting on the values, no rounding or
         * anything else. Example usage:
         *
         * @snippet Math.cpp Range-conversion
         */
        template<class U> constexpr explicit Range(const Range<dimensions, U>& other) noexcept: _min(other._min), _max(other._max) {}

        /** @brief Construct a range from external representation */
        template<class U, class = decltype(Implementation::RangeConverter<dimensions, T, U>::from(std::declval<U>()))> constexpr explicit Range(const U& other): Range{Implementation::RangeConverter<dimensions, T, U>::from(other)} {}

        /** @brief Convert the range to external representation */
        template<class U, class = decltype(Implementation::RangeConverter<dimensions, T, U>::to(std::declval<Range<dimensions, T>>()))> constexpr explicit operator U() const {
            return Implementation::RangeConverter<dimensions, T, U>::to(*this);
        }

        /**
         * @brief Equality comparison
         *
         * Done by comparing the underlying vectors, which internally uses
         * @ref TypeTraits::equals(), i.e. a fuzzy compare for floating-point
         * types.
         */
        bool operator==(const Range<dimensions, T>& other) const;

        /**
         * @brief Non-equality comparison
         *
         * Done by comparing the underlying vectors, which internally uses
         * @ref TypeTraits::equals(), i.e. a fuzzy compare for floating-point
         * types.
         */
        bool operator!=(const Range<dimensions, T>& other) const {
            return !operator==(other);
        }

        /**
         * @brief Raw data
         *
         * Contrary to what Doxygen shows, returns reference to an
         * one-dimensional fixed-size array of @cpp dimensions*2 @ce elements,
         * i.e. @cpp T(&)[dimensions*2] @ce.
         * @see @ref min(), @ref max()
         * @todoc Fix once there's a possibility to patch the signature in a
         *      post-processing step (https://github.com/mosra/m.css/issues/56)
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        T* data();
        const T* data() const; /**< @overload */
        #else
        auto data() -> T(&)[dimensions*2] {
            return reinterpret_cast<T(&)[dimensions*2]>(_min);
        }
        /* Can't really be constexpr anymore, the only other solution is having
           an union with `T _rawData[24]` and return that, but in a constexpr
           context it'd mean we'd have to initialize it instead of `_data` in
           the constructor ... and then operator[]() could not be constexpr
           anymore. I can't think of a practical use case where constexpr
           data() would be needed and operator[]() could not be used instead.
           Similarly to RectangularMatrix::data(), having a statically sized
           array returned is a far more useful property than constexpr, so that
           wins. */
        auto data() const -> const T(&)[dimensions*2] {
            return reinterpret_cast<const T(&)[dimensions*2]>(_min);
        }
        #endif

        /**
         * @brief Minimal coordinates (inclusive)
         *
         * Denoted as @f$ \operatorname{min}(A) @f$ in related math equations.
         * @see @ref size(), @ref Range2D::bottomLeft(),
         *      @ref Range3D::backBottomLeft()
         */
        VectorType& min() { return _min; }
        constexpr const VectorType min() const { return _min; } /**< @overload */

        /**
         * @brief Maximal coordinates (exclusive)
         *
         * Denoted as @f$ \operatorname{max}(A) @f$ in related math equations.
         * @see @ref size(), @ref Range2D::topRight(),
         *      @ref Range3D::frontTopRight()
         */
        VectorType& max() { return _max; }
        constexpr const VectorType max() const { return _max; } /**< @overload */

        /**
         * @brief Range size
         *
         * @see @ref min(), @ref max(), @ref Range2D::sizeX(),
         *      @ref Range2D::sizeY(), @ref Range3D::sizeX(),
         *      @ref Range3D::sizeY(), @ref Range3D::sizeZ(), @ref center()
         */
        VectorType size() const { return _max - _min; }

        /**
         * @brief Range center
         *
         * @see @ref Range2D::centerX(), @ref Range2D::centerY(),
         *      @ref Range3D::centerX(), @ref Range3D::centerY(),
         *      @ref Range3D::centerZ(), @ref size()
         */
        VectorType center() const { return (_min + _max)/T(2); }

        /**
         * @brief Translated range
         *
         * Translates the minimal and maximal coordinates by given amount. Size
         * remains the same.
         * @see @ref padded()
         */
        Range<dimensions, T> translated(const VectorType& vector) const {
            return {_min + vector, _max + vector};
        }

        /**
         * @brief Padded range
         *
         * Translates the minimal and maximal coordinates by given amount.
         * Center remains the same.
         * @see @ref translated(), @ref fromCenter()
         */
        Range<dimensions, T> padded(const VectorType& padding) const {
            return {_min - padding, _max + padding};
        }

        /**
         * @brief Scaled range
         *
         * Multiplies the minimal and maximal coordinates by given amount.
         * Center *doesn't* remain the same, use @ref scaledFromCenter() for
         * that operation.
         * @see @ref padded(), @ref scaled(T) const
         */
        Range<dimensions, T> scaled(const VectorType& scaling) const {
            return {_min*scaling, _max*scaling};
        }

        /**
         * @overload
         * @m_since_latest
         */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<UnsignedInt d = dimensions, typename std::enable_if<d != 1, int>::type = 0>
        #endif
        Range<dimensions, T> scaled(T scaling) const {
            return {_min*scaling, _max*scaling};
        }

        /**
         * @brief Range scaled from the center
         *
         * Scales the size, while center remains the same.
         * @see @ref scaled(), @ref padded(), @ref fromCenter(),
         *      @ref scaledFromCenter(T) const
         */
        Range<dimensions, T> scaledFromCenter(const VectorType& scaling) const {
            /* Can't use *T(0.5) because that won't work for integers */
            return fromCenter(center(), size()*scaling/T(2));
        }

        /**
         * @overload
         * @m_since_latest
         */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<UnsignedInt d = dimensions, typename std::enable_if<d != 1, int>::type = 0>
        #endif
        Range<dimensions, T> scaledFromCenter(T scaling) const {
            /* Can't use *T(0.5) because that won't work for integers */
            return fromCenter(center(), size()*scaling/T(2));
        }

        /**
         * @brief Whether given point is contained inside the range
         *
         * Returns @cpp true @ce if the following holds for all dimensions
         * @f$ i @f$, @cpp false @ce otherwise. @f[
         *      \bigwedge_i
         *      (b_i \ge \operatorname{min}(A)_i) \land
         *      (b_i < \operatorname{max}(A)_i)
         * @f]
         *
         * The range minimum is interpreted as inclusive, maximum as exclusive.
         * Results are undefined if the range has negative size.
         * @see @ref intersects(), @ref contains(const Range<dimensions, T>&) const,
         *      @ref min(), @ref max()
         */
        bool contains(const VectorType& b) const {
            return (Vector<dimensions, T>{b} >= _min).all() &&
                   (Vector<dimensions, T>{b} < _max).all();
        }

        /**
         * @brief Whether another range is fully contained inside this range
         *
         * Returns @cpp true @ce if the following holds for all dimensions
         * @f$ i @f$, @cpp false @ce otherwise. @f[
         *      \bigwedge_i
         *      (\operatorname{min}(B)_i \ge \operatorname{min}(A)_i) \land
         *      (\operatorname{max}(B)_i \le \operatorname{max}(A)_i)
         * @f]
         *
         * Results are undefined if the range has negative size.
         * @see @ref intersects(), @ref contains(const VectorType&) const,
         *      @ref min(), @ref max()
         */
        bool contains(const Range<dimensions, T>& b) const {
            return (Vector<dimensions, T>{b._min} >= _min).all() &&
                   (Vector<dimensions, T>{b._max} <= _max).all();
        }

    private:
        /* Called from Range(ZeroInit), either using the ZeroInit constructor
           (if available) or passing zero directly (for scalar types) */
        constexpr explicit Range(ZeroInitT, ZeroInitT*) noexcept: _min{ZeroInit}, _max{ZeroInit} {}
        constexpr explicit Range(ZeroInitT, void*) noexcept: _min{T(0)}, _max{T(0)} {}

        /* Called from Range(NoInit), either using the NoInit constructor (if
           available) or not doing anything */
        explicit Range(Magnum::NoInitT, Magnum::NoInitT*) noexcept: _min{Magnum::NoInit}, _max{Magnum::NoInit} {}
        explicit Range(Magnum::NoInitT, void*) noexcept {}

        VectorType _min, _max;
};

#ifndef DOXYGEN_GENERATING_OUTPUT
#define MAGNUM_RANGE_SUBCLASS_IMPLEMENTATION(dimensions, Type, VectorType)  \
    static Type<T> fromSize(const VectorType<T>& min, const VectorType<T>& size) { \
        return Range<dimensions, T>::fromSize(min, size);                   \
    }                                                                       \
    static Type<T> fromCenter(const VectorType<T>& center, const VectorType<T>& halfSize) { \
        return Range<dimensions, T>::fromCenter(center, halfSize);          \
    }                                                                       \
                                                                            \
    Type<T> translated(const VectorType<T>& vector) const {                 \
        return Range<dimensions, T>::translated(vector);                    \
    }                                                                       \
    Type<T> padded(const VectorType<T>& padding) const {                    \
        return Range<dimensions, T>::padded(padding);                       \
    }                                                                       \
    Type<T> scaled(const VectorType<T>& scaling) const {                    \
        return Range<dimensions, T>::scaled(scaling);                       \
    }                                                                       \
    Type<T> scaled(T scaling) const {                                       \
        return Range<dimensions, T>::scaled(scaling);                       \
    }                                                                       \
    Type<T> scaledFromCenter(const VectorType<T>& scaling) const {          \
        return Range<dimensions, T>::scaledFromCenter(scaling);             \
    }                                                                       \
    Type<T> scaledFromCenter(T scaling) const {                             \
        return Range<dimensions, T>::scaledFromCenter(scaling);             \
    }
#endif

/**
@brief One-dimensional range

Convenience alternative to @cpp Range<1, T> @ce. See @ref Range for more
information.
@see @ref Range2D, @ref Range3D, @ref Magnum::Range1D, @ref Magnum::Range1Dh,
    @ref Magnum::Range1Dd, @ref Magnum::Range1Dui, @ref Magnum::Range1Di
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<class T> using Range1D = Range<1, T>;
#endif

/**
@brief Two-dimensional range

See @ref Range for more information.
@see @ref Range1D, @ref Range3D, @ref Magnum::Range2D, @ref Magnum::Range2Dh,
    @ref Magnum::Range2Dd, @ref Magnum::Range2Dui, @ref Magnum::Range2Di
*/
template<class T> class Range2D: public Range<2, T> {
    public:
        /**
         * @brief Default constructor
         *
         * Equivalent to @ref Range2D(ZeroInitT).
         */
        constexpr /*implicit*/ Range2D() noexcept: Range<2, T>{ZeroInit} {}

        /** @copydoc Range(ZeroInitT) */
        constexpr explicit Range2D(ZeroInitT) noexcept: Range<2, T>{ZeroInit} {}

        /** @copydoc Range(NoInitT) */
        explicit Range2D(Magnum::NoInitT) noexcept: Range<2, T>{Magnum::NoInit} {}

        /** @copydoc Range(const VectorType&, const VectorType&) */
        constexpr /*implicit*/ Range2D(const Vector2<T>& min, const Vector2<T>& max) noexcept: Range<2, T>(min, max) {}

        /**
         * @brief Construct a range from a pair of minimal and maximal coordinates
         * @m_since{2020,06}
         *
         * Useful in combination with e.g. @ref minmax(), here for example to
         * calculate texture bounds:
         *
         * @snippet Math.cpp Range-construct-minmax2D
         */
        constexpr /*implicit*/ Range2D(const Containers::Pair<Vector2<T>, Vector2<T>>& minmax) noexcept: Range<2, T>{minmax.first(), minmax.second()} {}

        /**
         * @overload
         * @m_since{2020,06}
         */
        constexpr /*implicit*/ Range2D(const Containers::Pair<Vector<2, T>, Vector<2, T>>& minmax) noexcept: Range<2, T>{minmax.first(), minmax.second()} {}

        /** @copydoc Range(const Range<dimensions, U>&) */
        template<class U> constexpr explicit Range2D(const Range2D<U>& other) noexcept: Range<2, T>(other) {}

        /** @brief Construct a range from external representation */
        template<class U, class =
            #ifndef CORRADE_MSVC2015_COMPATIBILITY /* Causes ICE */
            decltype(Implementation::RangeConverter<2, T, U>::from(std::declval<U>()))
            #else
            decltype(Implementation::RangeConverter<2, T, U>())
            #endif
            >
        constexpr explicit Range2D(const U& other): Range<2, T>{Implementation::RangeConverter<2, T, U>::from(other)} {}

        /** @brief Copy constructor */
        constexpr /*implicit*/ Range2D(const Range<2, T>& other) noexcept: Range<2, T>(other) {}

        /**
         * @brief Bottom left corner
         *
         * Equivalent to @ref min().
         */
        Vector2<T>& bottomLeft() { return Range<2, T>::min(); }
        constexpr Vector2<T> bottomLeft() const { return Range<2, T>::min(); } /**< @overload */

        /** @brief Bottom right corner */
        constexpr Vector2<T> bottomRight() const {
            return {Range<2, T>::max().x(), Range<2, T>::min().y()};
        }

        /** @brief Top left corner */
        constexpr Vector2<T> topLeft() const {
            return {Range<2, T>::min().x(), Range<2, T>::max().y()};
        }

        /**
         * @brief Top right corner
         *
         * Equivalent to @ref max().
         */
        Vector2<T>& topRight() { return Range<2, T>::max(); }
        constexpr Vector2<T> topRight() const { return Range<2, T>::max(); } /**< @overload */

        /** @brief Left edge */
        T& left() { return Range<2, T>::min().x(); }
        constexpr T left() const { return Range<2, T>::min().x(); } /**< @overload */

        /** @brief Right edge */
        T& right() { return Range<2, T>::max().x(); }
        constexpr T right() const { return Range<2, T>::max().x(); } /**< @overload */

        /** @brief Bottom edge */
        T& bottom() { return Range<2, T>::min().y(); }
        constexpr T bottom() const { return Range<2, T>::min().y(); } /**< @overload */

        /** @brief Top edge */
        T& top() { return Range<2, T>::max().y(); }
        constexpr T top() const { return Range<2, T>::max().y(); } /**< @overload */

        /** @brief Range in the X axis */
        constexpr Range<1, T> x() const {
            return {Range<2, T>::min().x(), Range<2, T>::max().x()};
        }

        /** @brief Range in the Y axis */
        constexpr Range<1, T> y() const {
            return {Range<2, T>::min().y(), Range<2, T>::max().y()};
        }

        /**
         * @brief Range width
         *
         * @see @ref size()
         */
        T sizeX() const {
            return Range<2, T>::max().x() - Range<2, T>::min().x();
        }

        /**
         * @brief Range height
         *
         * @see @ref size()
         */
        T sizeY() const {
            return Range<2, T>::max().y() - Range<2, T>::min().y();
        }

        /**
         * @brief Range center on X axis
         *
         * @see @ref center()
         */
        T centerX() const {
            return (Range<2, T>::min().x() + Range<2, T>::max().x())/T(2);
        }

        /**
         * @brief Range center on Y axis
         *
         * @see @ref center()
         */
        T centerY() const {
            return (Range<2, T>::min().y() + Range<2, T>::max().y())/T(2);
        }

        MAGNUM_RANGE_SUBCLASS_IMPLEMENTATION(2, Range2D, Vector2)
};

/**
@brief Three-dimensional range

See @ref Range for more information.
@see @ref Range1D, @ref Range2D, @ref Magnum::Range3D, @ref Magnum::Range3Dh,
    @ref Magnum::Range3Dd, @ref Magnum::Range3Dui, @ref Magnum::Range3Di
*/
template<class T> class Range3D: public Range<3, T> {
    public:
        /**
         * @brief Default constructor
         *
         * Equivalent to @ref Range3D(ZeroInitT).
         */
        constexpr /*implicit*/ Range3D() noexcept: Range<3, T>{ZeroInit} {}

        /** @copydoc Range(ZeroInitT) */
        constexpr explicit Range3D(ZeroInitT) noexcept: Range<3, T>{ZeroInit} {}

        /** @brief @copybrief Range(Magnum::NoInitT) */
        explicit Range3D(Magnum::NoInitT) noexcept: Range<3, T>{Magnum::NoInit} {}

        /** @copydoc Range(const VectorType&, const VectorType&) */
        constexpr /*implicit*/ Range3D(const Vector3<T>& min, const Vector3<T>& max) noexcept: Range<3, T>(min, max) {}

        /**
         * @brief Construct a range from a pair of minimal and maximal coordinates
         * @m_since{2020,06}
         *
         * Useful in combination with e.g. @ref minmax(), here for example to
         * calculate bounds of a triangle:
         *
         * @snippet Math.cpp Range-construct-minmax3D
         */
        constexpr /*implicit*/ Range3D(const Containers::Pair<Vector3<T>, Vector3<T>>& minmax) noexcept: Range<3, T>{minmax.first(), minmax.second()} {}

        /**
         * @overload
         * @m_since{2020,06}
         */
        constexpr /*implicit*/ Range3D(const Containers::Pair<Vector<3, T>, Vector<3, T>>& minmax) noexcept: Range<3, T>{minmax.first(), minmax.second()} {}

        /** @copydoc Range(const Range<dimensions, U>&) */
        template<class U> constexpr explicit Range3D(const Range3D<U>& other) noexcept: Range<3, T>(other) {}

        /** @brief Construct a range from external representation */
        template<class U, class = decltype(Implementation::RangeConverter<3, T, U>::from(std::declval<U>()))> constexpr explicit Range3D(const U& other) noexcept: Range<3, T>{Implementation::RangeConverter<3, T, U>::from(other)} {}

        /** @brief Copy constructor */
        constexpr /*implicit*/ Range3D(const Range<3, T>& other) noexcept: Range<3, T>(other) {}

        /**
         * @brief Back bottom left corner
         *
         * Equivalent to @ref min().
         */
        Vector3<T>& backBottomLeft() { return Range<3, T>::min(); }
        constexpr Vector3<T> backBottomLeft() const { return Range<3, T>::min(); } /**< @overload */

        /** @brief Back bottom right corner */
        constexpr Vector3<T> backBottomRight() const {
            return {Range<3, T>::max().x(), Range<3, T>::min().y(), Range<3, T>::min().z()};
        }

        /** @brief Back top right corner */
        constexpr Vector3<T> backTopLeft() const {
            return {Range<3, T>::min().x(), Range<3, T>::max().y(), Range<3, T>::min().z()};
        }

        /** @brief Back top right corner */
        constexpr Vector3<T> backTopRight() const {
            return {Range<3, T>::max().x(), Range<3, T>::max().y(), Range<3, T>::min().z()};
        }

        /**
         * @brief Front top right corner
         *
         * Equivalent to @ref max().
         */
        Vector3<T>& frontTopRight() { return Range<3, T>::max(); }
        constexpr Vector3<T> frontTopRight() const { return Range<3, T>::max(); } /**< @overload */

        /** @brief Front top left corner */
        constexpr Vector3<T> frontTopLeft() const {
            return {Range<3, T>::min().x(), Range<3, T>::max().y(), Range<3, T>::max().z()};
        }

        /** @brief Front bottom right corner */
        constexpr Vector3<T> frontBottomRight() const {
            return {Range<3, T>::max().x(), Range<3, T>::min().y(), Range<3, T>::max().z()};
        }

        /** @brief Front bottom left corner */
        constexpr Vector3<T> frontBottomLeft() const {
            return {Range<3, T>::min().x(), Range<3, T>::min().y(), Range<3, T>::max().z()};
        }

        /** @brief Left edge */
        T& left() { return Range<3, T>::min().x(); }
        constexpr T left() const { return Range<3, T>::min().x(); } /**< @overload */

        /** @brief Right edge */
        T& right() { return Range<3, T>::max().x(); }
        constexpr T right() const { return Range<3, T>::max().x(); } /**< @overload */

        /** @brief Bottom edge */
        T& bottom() { return Range<3, T>::min().y(); }
        constexpr T bottom() const { return Range<3, T>::min().y(); } /**< @overload */

        /** @brief Top edge */
        T& top() { return Range<3, T>::max().y(); }
        constexpr T top() const { return Range<3, T>::max().y(); } /**< @overload */

        /** @brief Back edge */
        T& back() { return Range<3, T>::min().z(); }
        constexpr T back() const { return Range<3, T>::min().z(); } /**< @overload */

        /** @brief Front edge */
        T& front() { return Range<3, T>::max().z(); }
        constexpr T front() const { return Range<3, T>::max().z(); } /**< @overload */

        /** @brief Range in the X axis */
        constexpr Range<1, T> x() const {
            return {Range<3, T>::min().x(), Range<3, T>::max().x()};
        }

        /** @brief Range in the Y axis */
        constexpr Range<1, T> y() const {
            return {Range<3, T>::min().y(), Range<3, T>::max().y()};
        }

        /** @brief Range in the Z axis */
        constexpr Range<1, T> z() const {
            return {Range<3, T>::min().z(), Range<3, T>::max().z()};
        }

        /** @brief Range in the XY plane */
        constexpr Range2D<T> xy() const {
            return {Range<3, T>::min().xy(), Range<3, T>::max().xy()};
        }

        /**
         * @brief Range width
         *
         * @see @ref size()
         */
        T sizeX() const {
            return Range<3, T>::max().x() - Range<3, T>::min().x();
        }

        /**
         * @brief Range height
         *
         * @see @ref size()
         */
        T sizeY() const {
            return Range<3, T>::max().y() - Range<3, T>::min().y();
        }

        /**
         * @brief Range depth
         *
         * @see @ref size()
         */
        T sizeZ() const {
            return Range<3, T>::max().z() - Range<3, T>::min().z();
        }

        /**
         *
         * @brief Range center on X axis
         *
         * @see @ref center()
         */
        T centerX() const {
            return (Range<3, T>::min().x() + Range<3, T>::max().x())/T(2);
        }

        /**
         * @brief Range center on Y axis
         *
         * @see @ref center()
         */
        T centerY() const {
            return (Range<3, T>::min().y() + Range<3, T>::max().y())/T(2);
        }

        /**
         * @brief Range center on Z axis
         *
         * @see @ref center()
         */
        T centerZ() const {
            return (Range<3, T>::min().z() + Range<3, T>::max().z())/T(2);
        }

        MAGNUM_RANGE_SUBCLASS_IMPLEMENTATION(3, Range3D, Vector3)
};

/** @relatesalso Range
@brief Join two ranges

Returns a range that contains both input ranges. If one of the ranges is empty,
only the other is returned. Results are undefined if any range has a negative
size.
@see @ref join(const Range<dimensions, T>&, const Vector<dimensions, T>&)
*/
template<UnsignedInt dimensions, class T> inline Range<dimensions, T> join(const Range<dimensions, T>& a, const Range<dimensions, T>& b) {
    if(a.min() == a.max()) return b;
    if(b.min() == b.max()) return a;
    return {min(a.min(), b.min()), max(a.max(), b.max())};
}

/** @relatesalso Range
@brief Join a range and a point
@m_since_latest

Returns a range that contains both the input range and the point. Compared to
@ref join(const Range<dimensions, T>&, const Range<dimensions, T>&) there's no
special casing for an empty range. Results are undefined if the range has a
negative size.
*/
template<UnsignedInt dimensions, class T> inline Range<dimensions, T> join(const Range<dimensions, T>& a,
    /* std::common_type has to be used so it's possible to pass Vector2 and
       other subclasses, FFS */
    #ifdef DOXYGEN_GENERATING_OUTPUT
    const Vector<dimensions, T>& b
    #else
    const typename std::common_type<Vector<dimensions, T>>::type& b
    #endif
) {
    /* Range::min() / max() return scalars for 1D, have to explicitly choose a
       vector overload of Math::min() / max() to make it work with 1D vectors */
    return {min<dimensions, T>(a.min(), b), max<dimensions, T>(a.max(), b)};
}

/** @relatesalso Range
@brief Intersect two ranges

Returns a range that covers the intersection of both ranges. If the
intersection is empty, a default-constructed range is returned. The range
minimum is interpreted as inclusive, maximum as exclusive. Results are
undefined if any range has a negative size.
@see @ref intersects()
*/
template<UnsignedInt dimensions, class T> inline Range<dimensions, T> intersect(const Range<dimensions, T>& a, const Range<dimensions, T>& b) {
    if(!intersects(a, b)) return {};
    return {max(a.min(), b.min()), min(a.max(), b.max())};
}

/** @relatesalso Range
@brief Whether two ranges intersect

Returns @cpp true @ce if the following holds for all dimensions @f$ i @f$,
@cpp false @ce otherwise. @f[
    \bigwedge_i
    (\operatorname{max}(A)_i > \operatorname{min}(B)_i) \land
    (\operatorname{min}(A)_i < \operatorname{max}(B)_i)
@f]
The range minimum is interpreted as inclusive, maximum as exclusive. Results
are undefined if any range has a negative size.
@see @ref Range::contains(), @ref intersect(), @ref join(), @ref Range::min(),
    @ref Range::max()
*/
template<UnsignedInt dimensions, class T> inline bool intersects(const Range<dimensions, T>& a, const Range<dimensions, T>& b) {
    return (Vector<dimensions, T>{a.max()} > b.min()).all() &&
           (Vector<dimensions, T>{a.min()} < b.max()).all();
}

#ifndef CORRADE_SINGLES_NO_DEBUG
/** @debugoperator{Range} */
template<UnsignedInt dimensions, class T> Debug& operator<<(Debug& debug, const Range<dimensions, T>& value) {
    /** @todo might make sense to propagate the flags also, for hex value
        printing etc */
    const bool packed = debug.immediateFlags() >= Debug::Flag::Packed;
    debug << (packed ? "{{" : "Range({") << Debug::nospace << Vector<dimensions, T>{value.min()}[0];
    for(UnsignedInt i = 1; i != dimensions; ++i)
        debug << Debug::nospace << "," << Vector<dimensions, T>{value.min()}[i];
    debug << Debug::nospace << "}, {"
          << Debug::nospace << Vector<dimensions, T>{value.max()}[0];
    for(UnsignedInt i = 1; i != dimensions; ++i)
        debug << Debug::nospace << "," << Vector<dimensions, T>{value.max()}[i];
    return debug << Debug::nospace << (packed ? "}}" : "})");
}

/* Explicit instantiation for commonly used types */
#ifndef DOXYGEN_GENERATING_OUTPUT
extern template MAGNUM_EXPORT Debug& operator<<(Debug&, const Range<1, Float>&);
extern template MAGNUM_EXPORT Debug& operator<<(Debug&, const Range<2, Float>&);
extern template MAGNUM_EXPORT Debug& operator<<(Debug&, const Range<3, Float>&);
extern template MAGNUM_EXPORT Debug& operator<<(Debug&, const Range<1, Int>&);
extern template MAGNUM_EXPORT Debug& operator<<(Debug&, const Range<2, Int>&);
extern template MAGNUM_EXPORT Debug& operator<<(Debug&, const Range<3, Int>&);
extern template MAGNUM_EXPORT Debug& operator<<(Debug&, const Range<1, Double>&);
extern template MAGNUM_EXPORT Debug& operator<<(Debug&, const Range<2, Double>&);
extern template MAGNUM_EXPORT Debug& operator<<(Debug&, const Range<3, Double>&);
#endif
#endif

template<UnsignedInt dimensions, class T> inline bool Range<dimensions, T>::operator==(const Range<dimensions, T>& other) const {
    /* For non-scalar types default implementation of TypeTraits would be used,
       which is just operator== */
    return TypeTraits<VectorType>::equals(_min, other._min) &&
        TypeTraits<VectorType>::equals(_max, other._max);
}

#ifndef MAGNUM_NO_MATH_STRICT_WEAK_ORDERING
namespace Implementation {

template<UnsignedInt dimensions, class T> struct StrictWeakOrdering<Range<dimensions, T>> {
    bool operator()(const Range<dimensions, T>& a, const Range<dimensions, T>& b) const {
        StrictWeakOrdering<typename Range<dimensions, T>::VectorType> o;
        if(o(a.min(), b.min()))
            return true;
        if(o(b.min(), a.min()))
            return false;
        return o(a.max(), b.max());
    }
};

}
#endif

}}

#endif
