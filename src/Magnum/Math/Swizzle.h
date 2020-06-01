#ifndef Magnum_Math_Swizzle_h
#define Magnum_Math_Swizzle_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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
    template<std::size_t size, std::size_t position> struct GatherComponentAt {
        static_assert(size > position, "numeric swizzle parameter out of range of gather vector, use either xyzw/rgba/0/1 letters or small enough numbers");

        template<class T> constexpr static T value(const Math::Vector<size, T>& vector) {
            return vector._data[position];
        }
    };

    template<std::size_t size, char component> struct GatherComponent: GatherComponentAt<size, component> {};
    template<std::size_t size> struct GatherComponent<size, 'x'>: public GatherComponentAt<size, 0> {};
    template<std::size_t size> struct GatherComponent<size, 'y'>: public GatherComponentAt<size, 1> {};
    template<std::size_t size> struct GatherComponent<size, 'z'>: public GatherComponentAt<size, 2> {};
    template<std::size_t size> struct GatherComponent<size, 'w'>: public GatherComponentAt<size, 3> {};
    template<std::size_t size> struct GatherComponent<size, 'r'>: public GatherComponentAt<size, 0> {};
    template<std::size_t size> struct GatherComponent<size, 'g'>: public GatherComponentAt<size, 1> {};
    template<std::size_t size> struct GatherComponent<size, 'b'>: public GatherComponentAt<size, 2> {};
    template<std::size_t size> struct GatherComponent<size, 'a'>: public GatherComponentAt<size, 3> {};
    template<std::size_t size> struct GatherComponent<size, '0'> {
        template<class T> constexpr static T value(const Math::Vector<size, T>&) { return T(0); }
    };
    template<std::size_t size> struct GatherComponent<size, '1'> {
        template<class T> constexpr static T value(const Math::Vector<size, T>&) { return T(1); }
    };

    template<std::size_t size, class T> struct TypeForSize {
        typedef Math::Vector<size, typename T::Type> Type;
    };

    template<std::size_t size, std::size_t i, bool = true> struct ScatterComponentOr {
        template<class T> constexpr static T value(const Math::Vector<size, T>&, const T& value) {
            return value;
        }
    };
    template<std::size_t size, std::size_t i> struct ScatterComponentOr<size, i, false> {
        template<class T> constexpr static T value(const Math::Vector<size, T>& vector, const T&) {
            return vector._data[i];
        }
    };
    /* MSVC complains about C4389 signed/unsigned mismatch without the cast */
    template<std::size_t size, char component, std::size_t i> struct ScatterComponent: ScatterComponentOr<size, i, std::size_t(component) == i> {
        static_assert(component == 'x' || component == 'r' ||
                    ((component == 'y' || component == 'g') && size > 1) ||
                    ((component == 'z' || component == 'b') && size > 2) ||
                    ((component == 'w' || component == 'a') && size > 3) ||
                     std::size_t(component) < size,
            "swizzle parameter out of range of scatter vector, use either xyzw/rgba letters or small enough numbers");
    };
    template<std::size_t size> struct ScatterComponent<size, 'x', 0>: ScatterComponentOr<size, 0> {};
    template<std::size_t size> struct ScatterComponent<size, 'y', 1>: ScatterComponentOr<size, 1> {};
    template<std::size_t size> struct ScatterComponent<size, 'z', 2>: ScatterComponentOr<size, 2> {};
    template<std::size_t size> struct ScatterComponent<size, 'w', 3>: ScatterComponentOr<size, 3> {};
    template<std::size_t size> struct ScatterComponent<size, 'r', 0>: ScatterComponentOr<size, 0> {};
    template<std::size_t size> struct ScatterComponent<size, 'g', 1>: ScatterComponentOr<size, 1> {};
    template<std::size_t size> struct ScatterComponent<size, 'b', 2>: ScatterComponentOr<size, 2> {};
    template<std::size_t size> struct ScatterComponent<size, 'a', 3>: ScatterComponentOr<size, 3> {};

    template<class T, char component, std::size_t ...sequence> constexpr T scatterComponentOr(const T& vector, const typename T::Type& value, Sequence<sequence...>) {
        return {ScatterComponent<T::Size, component, sequence>::value(vector, value)...};
    }
    template<class T, std::size_t valueSize> constexpr T scatterRecursive(const T& vector, const Vector<valueSize, typename T::Type>&, std::size_t) {
        return vector;
    }
    template<class T, std::size_t valueSize, char component, char ...next> constexpr T scatterRecursive(const T& vector, const Vector<valueSize, typename T::Type>& values, std::size_t valueIndex) {
        return scatterRecursive<T, valueSize, next...>(
            scatterComponentOr<T, component>(vector, values._data[valueIndex], typename GenerateSequence<T::Size>::Type{}),
            values, valueIndex + 1);
    }
}

/**
@brief Gather @ref Vector components
@m_since{2019,10}

Creates a new vector from given components. Example:

@snippet MagnumMath.cpp gather

You can use letters @cpp 'x' @ce, @cpp 'y' @ce, @cpp 'z' @ce, @cpp 'w' @ce and
@cpp 'r' @ce, @cpp 'g' @ce, @cpp 'b' @ce, @cpp 'a' @ce for addressing
components or letters @cpp '0' @ce and @cpp '1' @ce for zero and one.
Alternatively you can also address components using their numeric index ---
which is especially useful when your input has more than four components. Count
of elements is unlimited, but must be at least one. If the resulting vector is
two, three or four-component, corresponding @ref Vector2, @ref Vector3,
@ref Vector4, @ref Color3 or @ref Color4 specialization is returned.

@see @ref scatter(), @ref matrix-vector-component-access, @ref Vector4::xyz(),
    @ref Vector4::rgb(), @ref Vector4::xy(), @ref Vector3::xy()
*/
template<char ...components, class T> constexpr typename Implementation::TypeForSize<sizeof...(components), T>::Type gather(const T& vector) {
    return {Implementation::GatherComponent<T::Size, components>::value(vector)...};
}

/**
@brief Scatter @ref Vector components
@param vector   Vector to update
@param values   Values to update it with
@return Updated vector
@m_since{2019,10}

Returns a copy of @p vector with particular components updated from @p values.
Inverse to @ref gather(), supporting the same component addressing except for
@cpp '0' @ce and @cpp '1' @ce. Example:

@snippet MagnumMath.cpp scatter

@see @ref matrix-vector-component-access, @ref Vector4::xyz(),
    @ref Vector4::rgb(), @ref Vector4::xy(), @ref Vector3::xy()
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
template<char ...components, class T> constexpr T scatter(const T& vector, const Vector<sizeof...(components), typename T::Type>& values)
#else
/* Using std::common_type otherwise GCC 4.8 fails to match the arguments
   in SwizzleTest::scatterOneComponent() */
template<char ...components, class T> constexpr T scatter(const T& vector, const typename std::common_type<Vector<sizeof...(components), typename T::Type>>::type& values)
#endif
{
    return Implementation::scatterRecursive<T, sizeof...(components), components...>(vector, values, 0);
}

#ifdef MAGNUM_BUILD_DEPRECATED
/** @brief @copybrief gather()
 * @m_deprecated_since{2019,10} Use @ref gather() instead.
 */
template<char ...components, class T> CORRADE_DEPRECATED("use gather() instead") constexpr typename Implementation::TypeForSize<sizeof...(components), T>::Type swizzle(const T& vector) {
    return gather<components...>(vector);
}
#endif

}}

#endif
