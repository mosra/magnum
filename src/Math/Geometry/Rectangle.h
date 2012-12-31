#ifndef Magnum_Math_Geometry_Rectangle_h
#define Magnum_Math_Geometry_Rectangle_h
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
 * @brief Class Magnum::Math::Geometry::Rectangle
 */

#include "Math/Vector2.h"

namespace Magnum { namespace Math { namespace Geometry {

/**
@brief %Rectangle

Helper class for storing axis-aligned rectangles consisting of bottom left and
top right corner positions. Bottom left position is inclusive, while top right
position is exclusive.
@see Magnum::Rectanglei
*/
template<class T> class Rectangle {
    public:
        /**
         * Create rectangle from position and size
         * @param bottomLeft    Bottom left rectangle corner
         * @param size          %Rectangle size
         */
        inline static Rectangle<T> fromSize(const Vector2<T>& bottomLeft, const Vector2<T>& size) {
            return {bottomLeft, bottomLeft+size};
        }

        /**
         * @brief %Rectangle from another of different type
         *
         * Performs only default casting on the values, no rounding or
         * anything else. Example usage:
         * @code
         * Rectangle<float> floatingPoint({1.3f, 2.7f}, {-15.0f, 7.0f});
         * auto integral = Rectangle<std::int8_t>::from(floatingPoint);
         * // integral == {{1, 2}, {-15, 7}}
         * @endcode
         */
        template<class U> inline constexpr static Rectangle<T> from(const Rectangle<U>& other) {
            return {Vector2<T>::from(other.bottomLeft()), Vector2<T>::from(other.topRight())};
        }

        /**
         * @brief Default constructor
         *
         * Construct zero-area rectangle positioned at origin.
         */
        inline constexpr Rectangle() = default;

        /**
         * @brief Constructor
         * @param bottomLeft    Bottom left rectangle corner
         * @param topRight      Top right rectangle corner
         */
        inline constexpr Rectangle(const Vector2<T>& bottomLeft, const Vector2<T>& topRight): _bottomLeft(bottomLeft), _topRight(topRight) {}

        /** @brief Equality operator */
        inline constexpr bool operator==(const Rectangle<T>& other) const {
            return _bottomLeft == other._bottomLeft && _topRight == other._topRight;
        }

        /** @brief Non-equality operator */
        inline constexpr bool operator!=(const Rectangle<T>& other) const {
            return !operator==(other);
        }

        /** @brief Bottom left corner */
        inline Vector2<T>& bottomLeft() { return _bottomLeft; }
        inline constexpr Vector2<T> bottomLeft() const { return _bottomLeft; } /**< @overload */

        /** @brief Top right corner */
        inline Vector2<T>& topRight() { return _topRight; }
        inline constexpr Vector2<T> topRight() const { return _topRight; } /**< @overload */

        /** @brief Bottom edge */
        inline T& bottom() { return _bottomLeft.y(); }
        inline constexpr T bottom() const { return _bottomLeft.y(); } /**< @overload */

        /** @brief Top edge */
        inline T& top() { return _topRight.y(); }
        inline constexpr T top() const { return _topRight.y(); } /**< @overload */

        /** @brief Left edge */
        inline T& left() { return _bottomLeft.x(); }
        inline constexpr T left() const { return _bottomLeft.x(); } /**< @overload */

        /** @brief Right edge */
        inline T& right() { return _topRight.x(); }
        inline constexpr T right() const { return _topRight.x(); } /**< @overload */

        /** @brief %Rectangle size */
        inline constexpr Vector2<T> size() const {
            return _topRight-_bottomLeft;
        }

        /** @brief %Rectangle width */
        inline constexpr T width() const { return _topRight.x() - _bottomLeft.x(); }

        /** @brief %Rectangle height */
        inline constexpr T height() const { return _topRight.y() - _bottomLeft.y(); }

    private:
        Vector2<T> _bottomLeft;
        Vector2<T> _topRight;
};

/** @debugoperator{Magnum::Math::Geometry::Rectangle} */
template<class T> Corrade::Utility::Debug operator<<(Corrade::Utility::Debug debug, const Rectangle<T>& value) {
    debug << "Rectangle({";
    debug.setFlag(Corrade::Utility::Debug::SpaceAfterEachValue, false);
    debug << value.left() << ", " << value.bottom() << "}, {" << value.right() << ", " << value.top() <<"})";
    debug.setFlag(Corrade::Utility::Debug::SpaceAfterEachValue, true);
    return debug;
}

}}}

#endif
