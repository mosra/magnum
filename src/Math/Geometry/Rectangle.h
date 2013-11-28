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
 * @brief Class @ref Magnum::Math::Geometry::Rectangle
 * @deprecated Use @ref Math/Range.h instead.
 */

#include "Math/Range.h"

#ifdef MAGNUM_BUILD_DEPRECATED
namespace Magnum { namespace Math { namespace Geometry {

/**
@copybrief Math::Range2D
@deprecated Use @ref Math::Range2D instead.
*/
template<class T> class Rectangle: public Range2D<T> {
    public:
        /** @copydoc Range2D() */
        constexpr Rectangle() = default;

        /** @copydoc Range2D(const VectorType&, const VectorType&) */
        constexpr Rectangle(const Vector2<T>& min, const Vector2<T>& max): Range2D<T>(min, max) {}

        /** @copydoc Range2D(const Range&) */
        constexpr Rectangle(const Range<2, T>& other): Range2D<T>(other) {}

        /** @copydoc Range2D(const Range<dimensions, U>&) */
        template<class U> constexpr explicit Rectangle(const Range2D<U>& other): Range2D<T>(other) {}

        /** @copydoc Range2D::sizeX() */
        T width() const { return Range2D<T>::sizeX(); }

        /** @copydoc Range2D::sizeY() */
        T height() const { return Range2D<T>::sizeY(); }
};

}}}

namespace Corrade { namespace Utility {
    /** @configurationvalue{Magnum::Math::Geometry::Rectangle} */
    template<class T> struct ConfigurationValue<Magnum::Math::Geometry::Rectangle<T>>: public ConfigurationValue<Magnum::Math::Range2D<T>> {};
}}
#endif

#endif
