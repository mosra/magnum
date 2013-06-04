#ifndef Magnum_Math_Geometry_Rectangle_h
#define Magnum_Math_Geometry_Rectangle_h
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
 * @brief Class Magnum::Math::Geometry::Rectangle
 */

#include "Math/Vector2.h"

namespace Magnum { namespace Math { namespace Geometry {

/**
@brief %Rectangle

Helper class for storing axis-aligned rectangles consisting of bottom left and
top right corner positions with origin in bottom left. Bottom/left positions
are inclusive, while top/right positions are exclusive.
@see Magnum::Rectangle, Magnum::Rectanglei, Magnum::Rectangled
@todo rename to Range, make it generic for one, two and three dimensions, add translated(), padded()...
@todo move outside Math?
*/
template<class T> class Rectangle {
    template<class> friend class Rectangle;

    public:
        /**
         * Create rectangle from position and size
         * @param bottomLeft    Bottom left rectangle corner
         * @param size          %Rectangle size
         */
        static Rectangle<T> fromSize(const Vector2<T>& bottomLeft, const Vector2<T>& size) {
            return {bottomLeft, bottomLeft+size};
        }

        /**
         * @brief Construct zero rectangle
         *
         * Construct zero-area rectangle positioned at origin.
         */
        constexpr Rectangle() {}

        /** @brief Construct rectangle from two corners */
        constexpr Rectangle(const Vector2<T>& bottomLeft, const Vector2<T>& topRight): _bottomLeft(bottomLeft), _topRight(topRight) {}

        /**
         * @brief Construct rectangle from another of different type
         *
         * Performs only default casting on the values, no rounding or
         * anything else. Example usage:
         * @code
         * Rectangle<Float> floatingPoint({1.3f, 2.7f}, {-15.0f, 7.0f});
         * Rectangle<Byte> integral(floatingPoint); // {{1, 2}, {-15, 7}}
         * @endcode
         */
        template<class U> constexpr explicit Rectangle(const Rectangle<U>& other): _bottomLeft(other._bottomLeft), _topRight(other._topRight) {}

        /** @brief Copy constructor */
        constexpr Rectangle(const Rectangle<T>&) = default;

        /** @brief Assignment operator */
        Rectangle<T>& operator=(const Rectangle<T>&) = default;

        /** @brief Equality operator */
        constexpr bool operator==(const Rectangle<T>& other) const {
            return _bottomLeft == other._bottomLeft && _topRight == other._topRight;
        }

        /** @brief Non-equality operator */
        constexpr bool operator!=(const Rectangle<T>& other) const {
            return !operator==(other);
        }

        /** @brief Bottom left corner */
        Vector2<T>& bottomLeft() { return _bottomLeft; }
        constexpr Vector2<T> bottomLeft() const { return _bottomLeft; } /**< @overload */

        /** @brief Bottom right corner */
        constexpr Vector2<T> bottomRight() const { return {_topRight.x(), _bottomLeft.y()}; } /**< @overload */

        /** @brief Top left corner */
        constexpr Vector2<T> topLeft() const { return {_bottomLeft.x(), _topRight.y()}; } /**< @overload */

        /** @brief Top right corner */
        Vector2<T>& topRight() { return _topRight; }
        constexpr Vector2<T> topRight() const { return _topRight; } /**< @overload */

        /** @brief Bottom edge */
        T& bottom() { return _bottomLeft.y(); }
        constexpr T bottom() const { return _bottomLeft.y(); } /**< @overload */

        /** @brief Top edge */
        T& top() { return _topRight.y(); }
        constexpr T top() const { return _topRight.y(); } /**< @overload */

        /** @brief Left edge */
        T& left() { return _bottomLeft.x(); }
        constexpr T left() const { return _bottomLeft.x(); } /**< @overload */

        /** @brief Right edge */
        T& right() { return _topRight.x(); }
        constexpr T right() const { return _topRight.x(); } /**< @overload */

        /** @brief %Rectangle size */
        constexpr Vector2<T> size() const { return _topRight-_bottomLeft; }

        /** @brief %Rectangle width */
        constexpr T width() const { return _topRight.x() - _bottomLeft.x(); }

        /** @brief %Rectangle height */
        constexpr T height() const { return _topRight.y() - _bottomLeft.y(); }

    private:
        Vector2<T> _bottomLeft;
        Vector2<T> _topRight;
};

/** @debugoperator{Magnum::Math::Geometry::Rectangle} */
template<class T> Corrade::Utility::Debug operator<<(Corrade::Utility::Debug debug, const Rectangle<T>& value) {
    debug << "Rectangle({";
    debug.setFlag(Corrade::Utility::Debug::SpaceAfterEachValue, false);
    debug << value.left() << ", " << value.bottom() << "}, {" << value.right() << ", " << value.top() << "})";
    debug.setFlag(Corrade::Utility::Debug::SpaceAfterEachValue, true);
    return debug;
}

}}}

#endif
