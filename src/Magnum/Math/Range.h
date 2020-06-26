#ifndef Magnum_Math_Range_h
#define Magnum_Math_Range_h
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
 * @brief Class @ref Magnum::Math::Range, @ref Magnum::Math::Range2D, @ref Magnum::Math::Range3D, alias @ref Magnum::Math::Range1D
 */

#include "Magnum/Math/Functions.h"
#include "Magnum/Math/Vector3.h"

namespace Magnum { namespace Math {

namespace Implementation {
    template<UnsignedInt, class> struct RangeTraits;

    template<class T> struct RangeTraits<1, T> { typedef T Type; };
    template<class T> struct RangeTraits<2, T> { typedef Vector2<T> Type; };
    template<class T> struct RangeTraits<3, T> { typedef Vector3<T> Type; };

    template<UnsignedInt, class, class> struct RangeConverter;
}

/**
@brief N-dimensional range

Axis-aligned line (in 1D), rectangle (in 2D) or cube (in 3D). The minimal
coordinate is inclusive, maximal exclusive. See @ref Range1D, @ref Range2D and
@ref Range3D specializations for given dimension count.

@section Math-Range-generic Use in generic code

While @ref Range2D and @ref Range3D have a vector underlying type, @ref Range1D
is just a scalar. This makes common usage simpler, but may break in generic
code that expects a vector type for any dimension. Solution is to
unconditionally cast the value to a vector type or explicitly specify template
parameters to choose a vector function overload. For example:

@snippet MagnumMath.cpp Range-generic
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
         * @brief Create a range from minimal coordinates and size
         * @param min   Minimal coordinates
         * @param size  Range size
         */
        static Range<dimensions, T> fromSize(const VectorType& min, const VectorType& size) {
            return {min, min+size};
        }

        /**
         * @brief Create a range from center and half size
         * @param center    Range center
         * @param halfSize  Half size
         *
         * For creating integer center ranges you can use @ref fromSize()
         * together with @ref padded(), for example:
         *
         * @snippet MagnumMath.cpp Range-fromCenter-integer
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

        /**
         * @brief Construct a range from a pair of minimal and maximal coordinates
         *
         * Useful in combination with e.g. @ref minmax(), here for example to
         * calculate bounds of a triangle:
         *
         * @snippet MagnumMath.cpp Range-construct-minmax3D
         *
         * @todo std::pair constructors are not constexpr in C++11, make it so in C++14
         */
        /*implicit*/ Range(const std::pair<VectorType, VectorType>& minmax) noexcept:
            _min{minmax.first}, _max{minmax.second} {}

        /** @overload */
        /** @todo std::pair constructors are not constexpr in C++11, make it so in C++14 */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<UnsignedInt d = dimensions, class = typename std::enable_if<d != 1>::type>
        #endif
        /*implicit*/ Range(const std::pair<Vector<dimensions, T>, Vector<dimensions, T>>& minmax) noexcept: _min{minmax.first}, _max{minmax.second} {}

        /**
         * @brief Construct range from another of different type
         *
         * Performs only default casting on the values, no rounding or
         * anything else. Example usage:
         *
         * @snippet MagnumMath.cpp Range-conversion
         */
        template<class U> constexpr explicit Range(const Range<dimensions, U>& other) noexcept: _min(other._min), _max(other._max) {}

        /** @brief Construct range from external representation */
        template<class U, class V = decltype(Implementation::RangeConverter<dimensions, T, U>::from(std::declval<U>()))> constexpr explicit Range(const U& other): Range{Implementation::RangeConverter<dimensions, T, U>::from(other)} {}

        /** @brief Copy constructor */
        constexpr /*implicit*/ Range(const Range<dimensions, T>&) noexcept = default;

        /** @brief Convert range to external representation */
        template<class U, class V = decltype(Implementation::RangeConverter<dimensions, T, U>::to(std::declval<Range<dimensions, T>>()))> constexpr explicit operator U() const {
            return Implementation::RangeConverter<dimensions, T, U>::to(*this);
        }

        /** @brief Equality comparison */
        bool operator==(const Range<dimensions, T>& other) const;

        /** @brief Non-equality comparison */
        bool operator!=(const Range<dimensions, T>& other) const {
            return !operator==(other);
        }

        /**
         * @brief Raw data
         * @return One-dimensional array of `dimensions`*2 length.
         */
        T* data() {
            return dataInternal(typename std::conditional<dimensions == 1, void*, T*>::type{});
        }
        constexpr const T* data() const {
            return dataInternal(typename std::conditional<dimensions == 1, void*, T*>::type{});
        } /**< @overload */

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
         * @see @ref padded()
         */
        Range<dimensions, T> scaled(const VectorType& scaling) const {
            return {_min*scaling, _max*scaling};
        }

        /**
         * @brief Range scaled from the center
         *
         * Scales the size, while center remains the same.
         * @see @ref scaled(), @ref padded(), @ref fromCenter()
         */
        Range<dimensions, T> scaledFromCenter(const VectorType& scaling) const {
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

        /* Called from data(), always returning a T* */
        constexpr const VectorType* dataInternal(void*) const { return &_min; }
        VectorType* dataInternal(void*) { return &_min; }
        constexpr const T* dataInternal(T*) const { return _min.data(); }
        T* dataInternal(T*) { return _min.data(); }

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
    Type<T> scaledFromCenter(const VectorType<T>& scaling) const {          \
        return Range<dimensions, T>::scaledFromCenter(scaling);             \
    }
#endif

/**
@brief One-dimensional range

Convenience alternative to @cpp Range<1, T> @ce. See @ref Range for more
information.
@see @ref Range2D, @ref Range3D, @ref Magnum::Range1D, @ref Magnum::Range1Di,
    @ref Magnum::Range1Dd
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<class T> using Range1D = Range<1, T>;
#endif

/**
@brief Two-dimensional range

See @ref Range for more information.
@see @ref Range1D, @ref Range3D, @ref Magnum::Range2D, @ref Magnum::Range2Di,
    @ref Magnum::Range2Dd
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
         * @snippet MagnumMath.cpp Range-construct-minmax2D
         *
         * @todo std::pair constructors are not constexpr in C++11, make it so in C++14
         */
        /*implicit*/ Range2D(const std::pair<Vector2<T>, Vector2<T>>& minmax) noexcept: Range<2, T>{minmax.first, minmax.second} {}

        /**
         * @overload
         * @m_since{2020,06}
         */
        /*implicit*/ Range2D(const std::pair<Vector<2, T>, Vector<2, T>>& minmax) noexcept: Range<2, T>{minmax.first, minmax.second} {}

        /** @copydoc Range(const Range<dimensions, U>&) */
        template<class U> constexpr explicit Range2D(const Range2D<U>& other) noexcept: Range<2, T>(other) {}

        /**
         * @brief Construct range from external representation
         * @todoc Remove workaround when Doxygen no longer chokes on that line
         */
        template<class U, class V =
            #ifndef CORRADE_MSVC2015_COMPATIBILITY /* Causes ICE */
            decltype(Implementation::RangeConverter<2, T, U>::from(std::declval<U>()))
            #else
            decltype(Implementation::RangeConverter<2, T, U>())
            #endif
            >
        constexpr explicit Range2D(const U& other): Range<2, T>{Implementation::RangeConverter<2, T, U>::from(other)} {}

        /** @copydoc Range(const Range&) */
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
@see @ref Range1D, @ref Range2D, @ref Magnum::Range3D, @ref Magnum::Range3Di,
    @ref Magnum::Range3Dd
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
         * @snippet MagnumMath.cpp Range-construct-minmax3D
         *
         * @todo std::pair constructors are not constexpr in C++11, make it so in C++14
         */
        /*implicit*/ Range3D(const std::pair<Vector3<T>, Vector3<T>>& minmax) noexcept: Range<3, T>{minmax.first, minmax.second} {}

        /**
         * @overload
         * @m_since{2020,06}
         */
        /*implicit*/ Range3D(const std::pair<Vector<3, T>, Vector<3, T>>& minmax) noexcept: Range<3, T>{minmax.first, minmax.second} {}

        /** @copydoc Range(const Range<dimensions, U>&) */
        template<class U> constexpr explicit Range3D(const Range3D<U>& other) noexcept: Range<3, T>(other) {}

        /**
         * @brief Construct range from external representation
         * @todoc Remove workaround when Doxygen no longer chokes on that line
         */
        template<class U, class V = decltype(Implementation::RangeConverter<3, T, U>::from(std::declval<U>()))> constexpr explicit Range3D(const U& other) noexcept: Range<3, T>{Implementation::RangeConverter<3, T, U>::from(other)} {}

        /** @copydoc Range(const Range&) */
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
*/
template<UnsignedInt dimensions, class T> inline Range<dimensions, T> join(const Range<dimensions, T>& a, const Range<dimensions, T>& b) {
    if(a.min() == a.max()) return b;
    if(b.min() == b.max()) return a;
    return {min(a.min(), b.min()), max(a.max(), b.max())};
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

#ifndef CORRADE_NO_DEBUG
/** @debugoperator{Range} */
template<UnsignedInt dimensions, class T> Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug& debug, const Range<dimensions, T>& value) {
    debug << "Range({" << Corrade::Utility::Debug::nospace << Vector<dimensions, T>{value.min()}[0];
    for(UnsignedInt i = 1; i != dimensions; ++i)
        debug << Corrade::Utility::Debug::nospace << "," << Vector<dimensions, T>{value.min()}[i];
    debug << Corrade::Utility::Debug::nospace << "}, {"
          << Corrade::Utility::Debug::nospace << Vector<dimensions, T>{value.max()}[0];
    for(UnsignedInt i = 1; i != dimensions; ++i)
        debug << Corrade::Utility::Debug::nospace << "," << Vector<dimensions, T>{value.max()}[i];
    return debug << Corrade::Utility::Debug::nospace << "})";
}

/* Explicit instantiation for commonly used types */
#ifndef DOXYGEN_GENERATING_OUTPUT
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const Range<1, Float>&);
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const Range<2, Float>&);
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const Range<3, Float>&);
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const Range<1, Int>&);
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const Range<2, Int>&);
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const Range<3, Int>&);
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const Range<1, Double>&);
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const Range<2, Double>&);
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const Range<3, Double>&);
#endif
#endif

template<UnsignedInt dimensions, class T> inline bool Range<dimensions, T>::operator==(const Range<dimensions, T>& other) const {
    /* For non-scalar types default implementation of TypeTraits would be used,
       which is just operator== */
    return TypeTraits<VectorType>::equals(_min, other._min) &&
        TypeTraits<VectorType>::equals(_max, other._max);
}

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

}}

#endif
