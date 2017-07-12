#ifndef Magnum_Math_Range_h
#define Magnum_Math_Range_h
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
 * @brief Class @ref Magnum::Math::Range, @ref Magnum::Math::Range2D, @ref Magnum::Math::Range3D, alias @ref Magnum::Math::Range1D
 */

#include "Magnum/Math/Functions.h"
#include "Magnum/Math/Vector3.h"

namespace Magnum { namespace Math {

namespace Implementation {
    template<UnsignedInt, class> struct RangeTraits;

    template<class T> struct RangeTraits<1, T> { typedef Vector<1, T> Type; };
    template<class T> struct RangeTraits<2, T> { typedef Vector2<T> Type; };
    template<class T> struct RangeTraits<3, T> { typedef Vector3<T> Type; };

    template<UnsignedInt, class, class> struct RangeConverter;
}

/**
@brief N-dimensional range

Axis-aligned line (in 1D), rectangle (in 2D) or cube (in 3D). Minimal
coordinate is inclusive, maximal exclusive. See @ref Range1D, @ref Range2D and
@ref Range3D specializations for given dimension count.
*/
template<UnsignedInt dimensions, class T> class Range {
    template<UnsignedInt, class> friend class Range;

    public:
        /**
         * @brief Underlying vector type
         *
         * `T` in 1D, @ref Vector2<T> in 2D, @ref Vector3<T> in 3D.
         */
        typedef typename Implementation::RangeTraits<dimensions, T>::Type VectorType;

        /**
         * @brief Create range from minimal coordinates and size
         * @param min   Minimal coordinates
         * @param size  Range size
         */
        static Range<dimensions, T> fromSize(const VectorType& min, const VectorType& size) {
            return {min, min+size};
        }

        /**
         * @brief Construct zero range
         *
         * Construct zero-size range positioned at origin.
         */
        constexpr /*implicit*/ Range(ZeroInitT = ZeroInit) noexcept: _min{ZeroInit}, _max{ZeroInit} {}

        /** @brief Construct without initializing the contents */
        explicit Range(NoInitT) noexcept: _min{NoInit}, _max{NoInit} {}

        /** @brief Construct range from minimal and maximal coordinates */
        constexpr /*implicit*/ Range(const VectorType& min, const VectorType& max) noexcept: _min{min}, _max{max} {}

        /**
         * @brief Construct range from another of different type
         *
         * Performs only default casting on the values, no rounding or
         * anything else. Example usage:
         * @code
         * Range2D<Float> floatingPoint({1.3f, 2.7f}, {-15.0f, 7.0f});
         * Range2D<Byte> integral(floatingPoint); // {{1, 2}, {-15, 7}}
         * @endcode
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
        constexpr bool operator==(const Range<dimensions, T>& other) const {
            return _min == other._min && _max == other._max;
        }

        /** @brief Non-equality comparison */
        constexpr bool operator!=(const Range<dimensions, T>& other) const {
            return !operator==(other);
        }

        /**
         * @brief Raw data
         * @return One-dimensional array of `dimensions`*2 length.
         */
        T* data() { return _min.data(); }
        constexpr const T* data() const { return _min.data(); } /**< @overload */

        /**
         * @brief Minimal coordinates (inclusive)
         *
         * @see @ref size(), @ref Range2D::bottomLeft(),
         *      @ref Range3D::backBottomLeft()
         */
        VectorType& min() { return _min; }
        constexpr const VectorType min() const { return _min; } /**< @overload */

        /**
         * @brief Maximal coordinates (exclusive)
         *
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
        Range<dimensions, T> translated(const VectorType& vector) const;

        /**
         * @brief Padded rage
         *
         * Translates the minimal and maximal coordinates by given amount.
         * Center remains the same.
         * @see @ref translated()
         */
        Range<dimensions, T> padded(const VectorType& padding) const;

        /**
         * @brief Scaled range
         *
         * Multiplies the minimal and maximal coordinates by given amount.
         * @see @ref padded()
         */
        Range<dimensions, T> scaled(const VectorType& scaling) const;

        /** @brief Whether given point is contained inside the range */
        constexpr bool contains(const VectorType& a) const {
            return (a >= _min).all() && (a < _max).all();
        }

    private:
        VectorType _min, _max;
};

#ifndef DOXYGEN_GENERATING_OUTPUT
#define MAGNUM_RANGE_SUBCLASS_IMPLEMENTATION(dimensions, Type, VectorType)  \
    static Type<T> fromSize(const VectorType<T>& min, const VectorType<T>& size) { \
        return Range<dimensions, T>::fromSize(min, size);                   \
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
    }
#endif

/**
@brief One-dimensional range

Convenience alternative to `Range<1, T>`. See @ref Range for more
information.
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<class T> using Range1D = Range<1, T>;
#endif

/**
@brief Two-dimensional range

See @ref Range for more information.
@see @ref Range1D, @ref Range3D
*/
template<class T> class Range2D: public Range<2, T> {
    public:
        /** @copydoc Range(ZeroInitT) */
        constexpr /*implicit*/ Range2D(ZeroInitT = ZeroInit) noexcept
            /** @todoc remove workaround when doxygen is sane */
            #ifndef DOXYGEN_GENERATING_OUTPUT
            : Range<2, T>{ZeroInit}
            #endif
            {}

        /** @copydoc Range(NoInitT) */
        explicit Range2D(NoInitT) noexcept
            /** @todoc remove workaround when doxygen is sane */
            #ifndef DOXYGEN_GENERATING_OUTPUT
            : Range<2, T>{NoInit}
            #endif
            {}

        /** @copydoc Range(const VectorType&, const VectorType&) */
        constexpr /*implicit*/ Range2D(const Vector2<T>& min, const Vector2<T>& max) noexcept: Range<2, T>(min, max) {}

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
        constexpr explicit Range2D(const U& other)
            /** @todoc remove workaround when doxygen is sane */
            #ifndef DOXYGEN_GENERATING_OUTPUT
            : Range<2, T>{Implementation::RangeConverter<2, T, U>::from(other)}
            #endif
            {}

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
@brief Two-dimensional range

See @ref Range for more information.
@see @ref Range1D, @ref Range2D
*/
template<class T> class Range3D: public Range<3, T> {
    public:
        /** @copydoc Range(ZeroInitT) */
        constexpr /*implicit*/ Range3D(ZeroInitT = ZeroInit) noexcept
            /** @todoc remove workaround when doxygen is sane */
            #ifndef DOXYGEN_GENERATING_OUTPUT
            : Range<3, T>{ZeroInit}
            #endif
            {}

        /** @copybrief Range(NoInitT) */
        explicit Range3D(NoInitT) noexcept
            /** @todoc remove workaround when doxygen is sane */
            #ifndef DOXYGEN_GENERATING_OUTPUT
            : Range<3, T>{NoInit}
            #endif
            {}

        /** @copydoc Range(const VectorType&, const VectorType&) */
        constexpr /*implicit*/ Range3D(const Vector3<T>& min, const Vector3<T>& max) noexcept: Range<3, T>(min, max) {}

        /** @copydoc Range(const Range<dimensions, U>&) */
        template<class U> constexpr explicit Range3D(const Range3D<U>& other) noexcept: Range<3, T>(other) {}

        /**
         * @brief Construct range from external representation
         * @todoc Remove workaround when Doxygen no longer chokes on that line
         */
        template<class U, class V = decltype(Implementation::RangeConverter<3, T, U>::from(std::declval<U>()))> constexpr explicit Range3D(const U& other) noexcept
            /** @todoc remove workaround when doxygen is sane */
            #ifndef DOXYGEN_GENERATING_OUTPUT
            : Range<3, T>{Implementation::RangeConverter<3, T, U>::from(other)}
            #endif
            {}

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
only the other is returned. Results are undefined if any range has negative
size.
*/
template<UnsignedInt dimensions, class T> inline Range<dimensions, T> join(const Range<dimensions, T>& a, const Range<dimensions, T>& b) {
    if(a.min() == a.max()) return b;
    if(b.min() == b.max()) return a;
    return {min(a.min(), b.min()), max(a.max(), b.max())};
}

/** @debugoperator{Magnum::Math::Range} */
template<UnsignedInt dimensions, class T> Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug& debug, const Range<dimensions, T>& value) {
    debug << "Range({" << Corrade::Utility::Debug::nospace << value.min()[0];
    for(UnsignedInt i = 1; i != dimensions; ++i)
        debug << Corrade::Utility::Debug::nospace << "," << value.min()[i];
    debug << Corrade::Utility::Debug::nospace << "}, {"
          << Corrade::Utility::Debug::nospace << value.max()[0];
    for(UnsignedInt i = 1; i != dimensions; ++i)
        debug << Corrade::Utility::Debug::nospace << "," << value.max()[i];
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

template<UnsignedInt dimensions, class T> Range<dimensions, T> Range<dimensions, T>::translated(const VectorType& vector) const {
    return {_min + vector, _max + vector};
}

template<UnsignedInt dimensions, class T> Range<dimensions, T> Range<dimensions, T>::padded(const VectorType& padding) const {
    return {_min - padding, _max + padding};
}

template<UnsignedInt dimensions, class T> Range<dimensions, T> Range<dimensions, T>::scaled(const VectorType& scaling) const {
    return {_min*scaling, _max*scaling};
}

}}

namespace Corrade { namespace Utility {

/** @configurationvalue{Magnum::Math::Range} */
template<Magnum::UnsignedInt dimensions, class T> struct ConfigurationValue<Magnum::Math::Range<dimensions, T>> {
    ConfigurationValue() = delete;

    /** @brief Writes elements separated with spaces */
    static std::string toString(const Magnum::Math::Range<dimensions, T>& value, const ConfigurationValueFlags flags) {
        return ConfigurationValue<Magnum::Math::Vector<dimensions*2, T>>::toString(
            reinterpret_cast<const Magnum::Math::Vector<dimensions*2, T>&>(value), flags);
    }

    /** @brief Reads elements separated with whitespace */
    static Magnum::Math::Range<dimensions, T> fromString(const std::string& stringValue, const ConfigurationValueFlags flags) {
        const auto vec = ConfigurationValue<Magnum::Math::Vector<dimensions*2, T>>::fromString(stringValue, flags);
        return *reinterpret_cast<const Magnum::Math::Range<dimensions, T>*>(vec.data());
    }
};

/** @configurationvalue{Magnum::Math::Range2D} */
template<class T> struct ConfigurationValue<Magnum::Math::Range2D<T>>: public ConfigurationValue<Magnum::Math::Range<2, T>> {};

/** @configurationvalue{Magnum::Math::Range3D} */
template<class T> struct ConfigurationValue<Magnum::Math::Range3D<T>>: public ConfigurationValue<Magnum::Math::Range<3, T>> {};

/* Explicit instantiation for commonly used types */
#if !defined(DOXYGEN_GENERATING_OUTPUT) && !defined(__MINGW32__)
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Range<2, Magnum::Float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Range<2, Magnum::Int>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Range<3, Magnum::Float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Range<3, Magnum::Int>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Range<2, Magnum::Double>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Range<3, Magnum::Double>>;
#endif

}}

#endif
