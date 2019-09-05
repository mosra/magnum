#ifndef Magnum_Math_Swizzle_h
#define Magnum_Math_Swizzle_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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
 * @brief Function @ref Magnum::Math::gather(), @ref Magnum::Math::scatter()
 */

#include "Magnum/Math/Vector.h"

namespace Magnum { namespace Math {

namespace Implementation {
    template<std::size_t size, std::size_t position> struct ComponentAtPosition {
        static_assert(size > position, "swizzle parameter out of range of gather vector");

        template<class T> constexpr static T value(const Math::Vector<size, T>& vector) {
            return vector[position];
        }
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

    template<std::size_t size, char component, std::size_t i> struct ComponentOr {
        static_assert(component == 'x' || component == 'r' ||
                    ((component == 'y' || component == 'g') && size > 1) ||
                    ((component == 'z' || component == 'b') && size > 2) ||
                    ((component == 'w' || component == 'a') && size > 3),
            "swizzle parameter out of range of scatter vector");

        template<class T> constexpr static T value(const Math::Vector<size, T>& vector, const T&) {
            return vector[i];
        }
    };
    template<std::size_t size, std::size_t position> struct Value {
        template<class T> constexpr static T value(const Math::Vector<size, T>&, const T& value) {
            return value;
        }
    };
    template<std::size_t size> struct ComponentOr<size, 'x', 0>: Value<size, 0> {};
    template<std::size_t size> struct ComponentOr<size, 'y', 1>: Value<size, 1> {};
    template<std::size_t size> struct ComponentOr<size, 'z', 2>: Value<size, 2> {};
    template<std::size_t size> struct ComponentOr<size, 'w', 3>: Value<size, 3> {};
    template<std::size_t size> struct ComponentOr<size, 'r', 0>: Value<size, 0> {};
    template<std::size_t size> struct ComponentOr<size, 'g', 1>: Value<size, 1> {};
    template<std::size_t size> struct ComponentOr<size, 'b', 2>: Value<size, 2> {};
    template<std::size_t size> struct ComponentOr<size, 'a', 3>: Value<size, 3> {};

    template<class T, char component, std::size_t ...sequence> constexpr T componentOr(const T& vector, const typename T::Type& value, Sequence<sequence...>) {
        return {ComponentOr<T::Size, component, sequence>::value(vector, value)...};
    }
    template<class T, std::size_t valueSize> constexpr T scatterRecursive(const T& vector, const Math::Vector<valueSize, typename T::Type>&, std::size_t) {
        return vector;
    }
    template<class T, std::size_t valueSize, char component, char ...next> constexpr T scatterRecursive(const T& vector, const Math::Vector<valueSize, typename T::Type>& values, std::size_t valueIndex) {
        return scatterRecursive<T, valueSize, next...>(
            componentOr<T, component>(vector, values[valueIndex], typename GenerateSequence<T::Size>::Type{}),
            values, valueIndex + 1);
    }
}

/**
@brief Gather @ref Vector components

Creates a new vector from given components. Example:

@snippet MagnumMath.cpp gather

You can use letters @cpp 'x' @ce, @cpp 'y' @ce, @cpp 'z' @ce, @cpp 'w' @ce and
@cpp 'r' @ce, @cpp 'g' @ce, @cpp 'b' @ce, @cpp 'a' @ce for addressing
components or letters @cpp '0' @ce and @cpp '1' @ce for zero and one. Count of
elements is unlimited, but must be at least one. If the resulting vector is
two, three or four-component, corresponding @ref Vector2, @ref Vector3,
@ref Vector4, @ref Color3 or @ref Color4 specialization is returned.

@see @ref scatter(), @ref matrix-vector-component-access, @ref Vector4::xyz(),
    @ref Vector4::rgb(), @ref Vector4::xy(), @ref Vector3::xy()
*/
template<char ...components, class T> constexpr typename Implementation::TypeForSize<sizeof...(components), T>::Type gather(const T& vector) {
    return {Implementation::Component<T::Size, components>::value(vector)...};
}

/**
@brief Scatter @ref Vector components
@param vector   Vector to update
@param values   Values to update it with
@return Updated vector

Returns a copy of @p vector with particular components updated from @p values.
Inverse to @ref gather(), supporting the same component addressing except for
@cpp '0' @ce and @cpp '1' @ce. Example:

@snippet MagnumMath.cpp scatter

@see @ref matrix-vector-component-access, @ref Vector4::xyz(),
    @ref Vector4::rgb(), @ref Vector4::xy(), @ref Vector3::xy()
*/
template<char ...components, class T> constexpr T scatter(const T& vector, const Vector<sizeof...(components), typename T::Type>& values) {
    return Implementation::scatterRecursive<T, sizeof...(components), components...>(vector, values, 0);
}

#ifdef MAGNUM_BUILD_DEPRECATED
/** @brief @copybrief gather()
 * @deprecated Use @ref gather instead.
 */
template<char ...components, class T> CORRADE_DEPRECATED("use gather() instead") constexpr typename Implementation::TypeForSize<sizeof...(components), T>::Type swizzle(const T& vector) {
    return gather<components...>(vector);
}
#endif

}}

#endif
