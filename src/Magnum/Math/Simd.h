#ifndef Magnum_Math_Simd_h
#define Magnum_Math_Simd_h
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
 * @brief Namespace @ref Magnum::Math::Simd
 */

/** @namespace Magnum::Math::Simd
@brief SIMD dispatch tags

Tags for dispatching to particular SIMD-optimized versions of batch math
algorithms.

This library is built as part of Magnum by default. To use this library with
CMake, you need to find the `Magnum` package and link to the `Magnum::Magnum`
target:

@code{.cmake}
find_package(Magnum REQUIRED)

# ...
target_link_libraries(your-app Magnum::Magnum)
@endcode

See @ref building and @ref cmake for more information.
*/
namespace Magnum { namespace Math { namespace Simd {

/**
@brief No SIMD acceleration tag type

Used to distinguish algorithms that have no explicit SIMD optimizations, apart
from compiler magic.
@see @ref None
*/
/* Explicit constructor to avoid ambiguous calls when using {} */
struct NoneT {
    #ifndef DOXYGEN_GENERATING_OUTPUT
    struct Init{};
    constexpr explicit NoneT(Init) {}
    #endif
};

/**
@brief SSE2 SIMD acceleration tag type

Used to distinguish algorithms that use at most the
[SSE2](https://en.wikipedia.org/wiki/SSE2) instruction set.
@see @ref Sse2
*/
/* Explicit constructor to avoid ambiguous calls when using {} */
struct Sse2T {
    #ifndef DOXYGEN_GENERATING_OUTPUT
    struct Init{};
    constexpr explicit Sse2T(Init) {}
    #endif
};

/**
@brief SSE4.1 SIMD acceleration tag type

Used to distinguish algorithms that use at most the
[SSE4.1](https://en.wikipedia.org/wiki/SSE4#SSE4.1) instruction set.
@see @ref Sse41
*/
/* Explicit constructor to avoid ambiguous calls when using {} */
struct Sse41T {
    #ifndef DOXYGEN_GENERATING_OUTPUT
    struct Init{};
    constexpr explicit Sse41T(Init) {}
    #endif
};

/**
@brief AVX2 SIMD acceleration tag type

Used to distinguish algorithms that use at most the
[AVX2](https://en.wikipedia.org/wiki/Advanced_Vector_Extensions#AVX2)
instruction set.
@see @ref Avx2
*/
/* Explicit constructor to avoid ambiguous calls when using {} */
struct Avx2T {
    #ifndef DOXYGEN_GENERATING_OUTPUT
    struct Init{};
    constexpr explicit Avx2T(Init) {}
    #endif
};

/**
@brief No SIMD acceleration tag

Use for selecting algorithms with no explicit SIMD optimizations.
*/
constexpr NoneT None{NoneT::Init{}};

/**
@brief SSE2 SIMD acceleration tag

Use for selecting algorithms that use at most the
[SSE2](https://en.wikipedia.org/wiki/SSE2) instruction set.
*/
constexpr Sse2T Sse2{Sse2T::Init{}};

/**
@brief SSE4.1 SIMD acceleration tag

Use for selecting algorithms that use at most the
[SSE4.1](https://en.wikipedia.org/wiki/SSE4#SSE4.1) instruction set.
*/
constexpr Sse41T Sse41{Sse41T::Init{}};

/**
@brief AVX2 SIMD acceleration tag type

Use for selecting algorithms that use at most the
[AVX2](https://en.wikipedia.org/wiki/Advanced_Vector_Extensions#AVX2)
instruction set.
*/
constexpr Avx2T Avx2{Avx2T::Init{}};

}}}

#endif
