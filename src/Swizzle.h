#ifndef Magnum_Swizzle_h
#define Magnum_Swizzle_h
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
 * @brief Function Magnum::swizzle()
 */

#include "Color.h"

namespace Magnum {

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
    template<std::size_t size> struct Component<size, 'r'>: public ComponentAtPosition<size, 0> {};
    template<std::size_t size> struct Component<size, 'g'>: public ComponentAtPosition<size, 1> {};
    template<std::size_t size> struct Component<size, 'b'>: public ComponentAtPosition<size, 2> {};
    template<std::size_t size> struct Component<size, 'a'>: public ComponentAtPosition<size, 3> {};
    template<std::size_t size> struct Component<size, '0'> {
        template<class T> inline constexpr static T value(const Math::Vector<size, T>&) { return T(0); }
    };
    template<std::size_t size> struct Component<size, '1'> {
        template<class T> inline constexpr static T value(const Math::Vector<size, T>&) { return T(1); }
    };

    template<std::size_t size, class T> struct TypeForSize {
        typedef Math::Vector<size, typename T::Type> Type;
    };
    template<class T> struct TypeForSize<2, T> { typedef Math::Vector2<typename T::Type> Type; };
    template<class T> struct TypeForSize<3, T> { typedef Math::Vector3<typename T::Type> Type; };
    template<class T> struct TypeForSize<4, T> { typedef Math::Vector4<typename T::Type> Type; };
    template<class T> struct TypeForSize<3, Color3<T>> { typedef Color3<T> Type; };
    template<class T> struct TypeForSize<3, Color4<T>> { typedef Color3<T> Type; };
    template<class T> struct TypeForSize<4, Color3<T>> { typedef Color4<T> Type; };
    template<class T> struct TypeForSize<4, Color4<T>> { typedef Color4<T> Type; };
}
#endif

/**
@brief Swizzle Vector components

Creates new vector from given components. Example:
@code
Vector4i original(-1, 2, 3, 4);

auto vec = swizzle<'a', '1', '0', 'r', 'g', 'b'>(original);
// vec == { 4, 1, 0, -1, 2, 3 }
@endcode
You can use letters `x`, `y`, `z`, `w` and `r`, `g`, `b`, `a` for addressing
components or letters `0` and `1` for zero and one. Count of elements is
unlimited, but must be at least one. If the resulting vector is two, three or
four-component, corresponding Math::Vector2, Math::Vector3, Math::Vector4,
Color3 or Color4 specialization is returned.

@see @ref matrix-vector-component-access, Vector4::xyz(), Color4::rgb(),
    Vector4::xy(), Vector3::xy()
*/
template<char ...components, class T> inline constexpr typename Implementation::TypeForSize<sizeof...(components), T>::Type swizzle(const T& vector) {
    return {Implementation::Component<T::Size, components>::value(vector)...};
}

}

#endif
