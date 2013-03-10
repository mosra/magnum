#ifndef Magnum_Math_Swizzle_h
#define Magnum_Math_Swizzle_h
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
 * @brief Function Magnum::Math::swizzle()
 */

#include "Vector.h"

namespace Magnum { namespace Math {

#ifndef DOXYGEN_GENERATING_OUTPUT
namespace Implementation {
    template<std::size_t size, std::size_t position> struct ComponentAtPosition {
        static_assert(size > position, "Swizzle parameter out of range of base vector");

        template<class T> inline constexpr static T value(const Math::Vector<size, T>& vector) { return vector[position]; }
    };

    template<std::size_t size, char component> struct Component {};
    template<std::size_t size> struct Component<size, 'x'>: public ComponentAtPosition<size, 0> {};
    template<std::size_t size> struct Component<size, 'y'>: public ComponentAtPosition<size, 1> {};
    template<std::size_t size> struct Component<size, 'z'>: public ComponentAtPosition<size, 2> {};
    template<std::size_t size> struct Component<size, 'w'>: public ComponentAtPosition<size, 3> {};
    template<std::size_t size> struct Component<size, '0'> {
        template<class T> inline constexpr static T value(const Math::Vector<size, T>&) { return T(0); }
    };
    template<std::size_t size> struct Component<size, '1'> {
        template<class T> inline constexpr static T value(const Math::Vector<size, T>&) { return T(1); }
    };
}
#endif

/**
@brief Swizzle Vector components

Creates new vector from given components. Example:
@code
Vector4<Int> original(-1, 2, 3, 4);

auto vec = swizzle<'w', '1', '0', 'x', 'y', 'z'>(original);
// vec == { 4, 1, 0, -1, 2, 3 }
@endcode
You can use letters `x`, `y`, `z`, `w` for addressing components or letters
`0` and `1` for zero and one. Count of elements is unlimited, but must be at
least one.

See also Magnum::swizzle() which has some added convenience features not
present in this lightweight implementation for Math namespace.

@see @ref matrix-vector-component-access, Vector4::xyz(),
    Vector4::xy(), Vector3::xy()
*/
template<char ...components, std::size_t size, class T> inline constexpr Vector<sizeof...(components), T> swizzle(const Vector<size, T>& vector) {
    return {Implementation::Component<size, components>::value(vector)...};
}

}}

#endif
