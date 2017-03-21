#ifndef Magnum_Math_Swizzle_h
#define Magnum_Math_Swizzle_h
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
 * @brief Function @ref Magnum::Math::swizzle()
 */

#include "Magnum/Math/Vector.h"

namespace Magnum { namespace Math {

namespace Implementation {
    template<std::size_t size, std::size_t position> struct ComponentAtPosition {
        static_assert(size > position, "Swizzle parameter out of range of base vector");

        template<class T> constexpr static T value(const Math::Vector<size, T>& vector) { return vector[position]; }
    };

    template<std::size_t size, char component> struct Component {};
    template<std::size_t size> struct Component<size, 'x'>: public ComponentAtPosition<size, 0> {};
    template<std::size_t size> struct Component<size, 'y'>: public ComponentAtPosition<size, 1> {};
    template<std::size_t size> struct Component<size, 'z'>: public ComponentAtPosition<size, 2> {};
    template<std::size_t size> struct Component<size, 'w'>: public ComponentAtPosition<size, 3> {};
    template<std::size_t size> struct Component<size, 'r'>: public ComponentAtPosition<size, 0> {};
    template<std::size_t size> struct Component<size, 'g'>: public ComponentAtPosition<size, 1> {};
    template<std::size_t size> struct Component<size, 'b'>: public ComponentAtPosition<size, 2> {};
    template<std::size_t size> struct Component<size, 'a'>: public ComponentAtPosition<size, 3> {};
    template<std::size_t size> struct Component<size, '0'> {
        template<class T> constexpr static T value(const Math::Vector<size, T>&) { return T(0); }
    };
    template<std::size_t size> struct Component<size, '1'> {
        template<class T> constexpr static T value(const Math::Vector<size, T>&) { return T(1); }
    };

    template<std::size_t size, class T> struct TypeForSize {
        typedef Math::Vector<size, typename T::Type> Type;
    };
}

/**
@brief Swizzle Vector components

Creates new vector from given components. Example:
@code
Vector4i original(-1, 2, 3, 4);

auto vec = swizzle<'w', '1', '0', 'x', 'y', 'z'>(original);
// vec == { 4, 1, 0, -1, 2, 3 }
@endcode
You can use letters `x`, `y`, `z`, `w` and `r`, `g`, `b`, `a` for addressing
components or letters `0` and `1` for zero and one. Count of elements is
unlimited, but must be at least one. If the resulting vector is two, three or
four-component, corresponding @ref Math::Vector2, @ref Math::Vector3,
@ref Math::Vector4, @ref Color3 or @ref Color4 specialization is returned.

@see @ref matrix-vector-component-access, @ref Vector4::xyz(),
    @ref Vector4::rgb(), @ref Vector4::xy(), @ref Vector3::xy()
*/
template<char ...components, class T> constexpr typename Implementation::TypeForSize<sizeof...(components), T>::Type swizzle(const T& vector) {
    return {Implementation::Component<T::Size, components>::value(vector)...};
}

}}

#endif
