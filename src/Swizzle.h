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
        typedef Math::Vector<size, typename T::Type> Type;
    };
    template<class T> struct TypeForSize<2, T> { typedef Math::Vector2<typename T::Type> Type; };
    template<class T> struct TypeForSize<3, T> { typedef Math::Vector3<typename T::Type> Type; };
    template<class T> struct TypeForSize<4, T> { typedef Math::Vector4<typename T::Type> Type; };
    template<class T> struct TypeForSize<3, Color3<T>> { typedef Color3<T> Type; };
    template<class T> struct TypeForSize<3, Color4<T>> { typedef Color3<T> Type; };
    template<class T> struct TypeForSize<4, Color3<T>> { typedef Color4<T> Type; };
    template<class T> struct TypeForSize<4, Color4<T>> { typedef Color4<T> Type; };

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

    template<size_t size, class T, size_t ...sequence> inline constexpr Math::Vector<sizeof...(sequence), T> swizzleFrom(Sequence<sequence...>, const Math::Vector<size, T>& vector, const char(&components)[sizeof...(sequence)+1]) {
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
swizzle(const T&, const char(&)[newSize]), but the evaluation of
the swizzling operation is guaranteed to be always done at compile time
instead of at runtime.

@see Vector4::xyz(), Vector4::rgb(), Vector4::xy(), Vector3::xy()
*/
template<char ...components, class T> inline constexpr typename Implementation::TypeForSize<sizeof...(components), T>::Type swizzle(const T& vector) {
    return {vector[Implementation::GetComponent<T::size, components>::value()]...};
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
swizzle(const T&), but unless the result is marked with
`constexpr`, the evaluation of the swizzling operation probably won't be
evaluated at compile time, but at runtime.

@see Vector4::xyz(), Vector4::rgb(), Vector4::xy(), Vector3::xy()
*/
template<class T, size_t newSize> inline constexpr typename Implementation::TypeForSize<newSize-1, T>::Type swizzle(const T& vector, const char(&components)[newSize]) {
    return Implementation::swizzleFrom(typename Implementation::GenerateSequence<newSize-1>::Type(), vector, components);
}

}

#endif
