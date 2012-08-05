#ifndef Magnum_Math_Swizzle_h
#define Magnum_Math_Swizzle_h
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
 * @brief Function Magnum::Math::swizzle()
 */

#include "Vector4.h"

namespace Magnum { namespace Math {

#ifndef DOXYGEN_GENERATING_OUTPUT
namespace Implementation {
    template<size_t size, size_t position> struct GetPosition {
        static_assert(size > position, "Swizzle parameter out of range of base vector");

        inline constexpr static size_t value() { return position; }
    };

    template<size_t size, char component> struct GetComponent {};
    template<size_t size> struct GetComponent<size, 'x'>: public GetPosition<size, 0> {};
    template<size_t size> struct GetComponent<size, 'y'>: public GetPosition<size, 1> {};
    template<size_t size> struct GetComponent<size, 'z'>: public GetPosition<size, 2> {};
    template<size_t size> struct GetComponent<size, 'w'>: public GetPosition<size, 3> {};
    template<size_t size> struct GetComponent<size, 'r'>: public GetPosition<size, 0> {};
    template<size_t size> struct GetComponent<size, 'g'>: public GetPosition<size, 1> {};
    template<size_t size> struct GetComponent<size, 'b'>: public GetPosition<size, 2> {};
    template<size_t size> struct GetComponent<size, 'a'>: public GetPosition<size, 3> {};

    template<size_t size, class T> struct TypeForSize {
        typedef Vector<size, T> Type;
    };

    template<class T> struct TypeForSize<2, T> { typedef Vector2<T> Type; };
    template<class T> struct TypeForSize<3, T> { typedef Vector3<T> Type; };
    template<class T> struct TypeForSize<4, T> { typedef Vector4<T> Type; };

    inline constexpr size_t getPosition(size_t size, size_t position) {
        return size > position ? position : throw;
    }

    template<size_t size> inline constexpr size_t getComponent(char component) {
        return component == 'x' ? getPosition(size, 0) :
               component == 'y' ? getPosition(size, 1) :
               component == 'z' ? getPosition(size, 2) :
               component == 'w' ? getPosition(size, 3) :
               component == 'r' ? getPosition(size, 0) :
               component == 'g' ? getPosition(size, 1) :
               component == 'b' ? getPosition(size, 2) :
               component == 'a' ? getPosition(size, 3) :
               throw;
    }

    template<size_t size, class T, size_t ...sequence> inline constexpr Vector<sizeof...(sequence), T> swizzleFrom(Sequence<sequence...>, const Vector<size, T>& vector, const char(&components)[sizeof...(sequence)+1]) {
        return {vector[getComponent<size>(components[sequence])]...};
    }
}
#endif

/**
@brief Swizzle Vector components

Creates new vector from given components. Example:
@code
Vector4<int> original(1, 2, 3, 4);

auto vec = swizzle<'a', 'b', 'b', 'g', 'r', 'r'>(original);
// vec == { 4, 3, 3, 2, 1, 1 }
@endcode
You can use letters `x`, `y`, `z`, `w` and `r`, `g`, `b`, `a`. Count of
elements is unlimited, but must be at least one. If the resulting vector is
two, three or four-component, corresponding Vector2, Vector3 or Vector4
specialization is returned.

@attention This function is less convenient to write than
swizzle(const Vector<size, T>&, const char(&)[newSize]), but the evaluation of
the swizzling operation is guaranteed to be always done at compile time
instead of at runtime.

@see Vector4::xyz(), Vector4::rgb(), Vector4::xy(), Vector3::xy()
*/
template<char ...components, size_t size, class T> inline constexpr typename Implementation::TypeForSize<sizeof...(components), T>::Type swizzle(const Vector<size, T>& vector) {
    return {vector[Implementation::GetComponent<size, components>::value()]...};
}

/**
@brief Swizzle Vector components

Creates new vector from given components. Example:
@code
Vector4<int> original(1, 2, 3, 4);

auto vec = swizzle(original, "abbgrr");
// vec == { 4, 3, 3, 2, 1, 1 }
@endcode
You can use letters `x`, `y`, `z`, `w` and `r`, `g`, `b`, `a`. Count of
elements is unlimited, but must be at least one. If the resulting vector is
two, three or four-component, corresponding Vector2, Vector3 or Vector4
specialization is returned.

@attention This function is more convenient to write than
swizzle(const Vector<size, T>&), but unless the result is marked with
`constexpr`, the evaluation of the swizzling operation probably won't be
evaluated at compile time, but at runtime.

@see Vector4::xyz(), Vector4::rgb(), Vector4::xy(), Vector3::xy()
*/
template<size_t size, class T, size_t newSize> inline constexpr typename Implementation::TypeForSize<newSize-1, T>::Type swizzle(const Vector<size, T>& vector, const char(&components)[newSize]) {
    return Implementation::swizzleFrom(typename Implementation::GenerateSequence<newSize-1>::Type(), vector, components);
}

}}

#endif
