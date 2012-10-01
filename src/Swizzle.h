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
 * @brief Functions Magnum::swizzle(const T&), Magnum::swizzle(const T&, const char(&)[])
 */

#include "Color.h"

namespace Magnum {

#ifndef DOXYGEN_GENERATING_OUTPUT
namespace Implementation {
    using Math::Implementation::Sequence;
    using Math::Implementation::GenerateSequence;

    template<size_t size, size_t position> struct ComponentAtPosition {
        static_assert(size > position, "Swizzle parameter out of range of base vector");

        template<class T> inline constexpr static T value(const Math::Vector<size, T>& vector) { return vector[position]; }
    };

    template<size_t size, char component> struct Component {};
    template<size_t size> struct Component<size, 'x'>: public ComponentAtPosition<size, 0> {};
    template<size_t size> struct Component<size, 'y'>: public ComponentAtPosition<size, 1> {};
    template<size_t size> struct Component<size, 'z'>: public ComponentAtPosition<size, 2> {};
    template<size_t size> struct Component<size, 'w'>: public ComponentAtPosition<size, 3> {};
    template<size_t size> struct Component<size, 'r'>: public ComponentAtPosition<size, 0> {};
    template<size_t size> struct Component<size, 'g'>: public ComponentAtPosition<size, 1> {};
    template<size_t size> struct Component<size, 'b'>: public ComponentAtPosition<size, 2> {};
    template<size_t size> struct Component<size, 'a'>: public ComponentAtPosition<size, 3> {};
    template<size_t size> struct Component<size, '0'> {
        template<class T> inline constexpr static T value(const Math::Vector<size, T>&) { return T(0); }
    };
    template<size_t size> struct Component<size, '1'> {
        template<class T> inline constexpr static T value(const Math::Vector<size, T>&) { return T(1); }
    };

    template<size_t size, class T> struct TypeForSize {
        typedef Math::Vector<size, typename T::Type> Type;
    };
    template<class T> struct TypeForSize<2, T> { typedef Math::Vector2<typename T::Type> Type; };
    template<class T> struct TypeForSize<3, T> { typedef Math::Vector3<typename T::Type> Type; };
    template<class T> struct TypeForSize<4, T> { typedef Math::Vector4<typename T::Type> Type; };
    template<class T> struct TypeForSize<3, Color3<T>> { typedef Color3<T> Type; };
    template<class T> struct TypeForSize<3, Color4<T>> { typedef Color3<T> Type; };
    template<class T> struct TypeForSize<4, Color3<T>> { typedef Color4<T> Type; };
    template<class T> struct TypeForSize<4, Color4<T>> { typedef Color4<T> Type; };

    template<size_t size, class T> inline constexpr T componentAtPosition(const Math::Vector<size, T>& vector, size_t position) {
        return size > position ? vector[position] : throw;
    }

    template<size_t size, class T> inline constexpr T component(const Math::Vector<size, T>& vector, char component) {
        return component == 'x' ? componentAtPosition(vector, 0) :
               component == 'y' ? componentAtPosition(vector, 1) :
               component == 'z' ? componentAtPosition(vector, 2) :
               component == 'w' ? componentAtPosition(vector, 3) :
               component == 'r' ? componentAtPosition(vector, 0) :
               component == 'g' ? componentAtPosition(vector, 1) :
               component == 'b' ? componentAtPosition(vector, 2) :
               component == 'a' ? componentAtPosition(vector, 3) :
               component == '0' ? T(0) :
               component == '1' ? T(1) :
               throw;
    }

    template<size_t size, class T, size_t ...sequence> inline constexpr Math::Vector<sizeof...(sequence), T> swizzleFrom(Sequence<sequence...>, const Math::Vector<size, T>& vector, const char(&components)[sizeof...(sequence)+1]) {
        return {component<size>(vector, components[sequence])...};
    }
}
#endif

/**
@brief Swizzle Vector components

Creates new vector from given components. Example:
@code
Vector4<int> original(-1, 2, 3, 4);

auto vec = swizzle<'a', '1', '0', 'r', 'g', 'b'>(original);
// vec == { 4, 1, 0, -1, 2, 3 }
@endcode
You can use letters `x`, `y`, `z`, `w` and `r`, `g`, `b`, `a` for addressing
components or letters `0` and `1` for zero and one. Count of elements is
unlimited, but must be at least one. If the resulting vector is two, three or
four-component, corresponding Vector2, Vector3, Vector4, Color3 or Color4
specialization is returned.

@attention This function is less convenient to write than
swizzle(const T&, const char(&)[newSize]), but the evaluation of
the swizzling operation is guaranteed to be always done at compile time
instead of at runtime.

@see @ref matrix-vector-component-access, Vector4::xyz(), Color4::rgb(),
    Vector4::xy(), Vector3::xy()
*/
template<char ...components, class T> inline constexpr typename Implementation::TypeForSize<sizeof...(components), T>::Type swizzle(const T& vector) {
    return {Implementation::Component<T::Size, components>::value(vector)...};
}

/**
@brief Swizzle Vector components

Creates new vector from given components. Example:
@code
Vector4<int> original(-1, 2, 3, 4);

auto vec = swizzle(original, "a10rgb");
// vec == { 4, 1, 0, -1, 2, 3 }
@endcode
You can use letters `x`, `y`, `z`, `w` and `r`, `g`, `b`, `a` for addressing
components or letters `0` and `1` for zero and one. Count of elements is
unlimited, but must be at least one. If the resulting vector is two, three or
four-component, corresponding Vector2, Vector3 or Vector4 specialization is
returned.

@attention This function is more convenient to write than
swizzle(const T&), but unless the result is marked with
`constexpr`, the evaluation of the swizzling operation probably won't be
evaluated at compile time, but at runtime.

@see @ref matrix-vector-component-access, Vector4::xyz(), Color4::rgb(),
    Vector4::xy(), Vector3::xy()
*/
template<class T, size_t newSize> inline constexpr typename Implementation::TypeForSize<newSize-1, T>::Type swizzle(const T& vector, const char(&components)[newSize]) {
    return Implementation::swizzleFrom(typename Implementation::GenerateSequence<newSize-1>::Type(), vector, components);
}

}

#endif
