#ifndef Magnum_TextureTools_Sample_h
#define Magnum_TextureTools_Sample_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025, 2026
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
 * @brief Function @ref Magnum::TextureTools::sampleLinear(), @ref Magnum::TextureTools::sampleSrgb(), @ref Magnum::TextureTools::sampleSrgbAlpha()
 * @m_since_latest
 */

#include "Magnum/Magnum.h"
#include "Magnum/TextureTools/visibility.h"

namespace Magnum { namespace TextureTools {

/**
@brief Sample a 1D RGB texture with linear interpolation
@m_since_latest

Expects that @p texture has at least one element and @p factor is in a
@f$ [0, 1] @f$ range. For a factor of @cpp 0.0f @ce returns the first element
of @p texture, for @cpp 1.0f @ce returns the last, values in between are a
linear interpolation of two nearest elements that are first unpacked to a
floating-point type.

Note that the @p texture is interpreted as having linear colors. Use
@ref sampleSrgb() if you want to perform conversion from sRGB values instead.
@see @ref Math::lerp(const T&, const T&, U), @ref Math::unpack(const Integral&)
*/
MAGNUM_TEXTURETOOLS_EXPORT Color3 sampleLinear(const Containers::StridedArrayView1D<const Vector3ub>& texture, Float factor);

/**
@brief Sample a 1D RGBA texture with linear interpolation
@m_since_latest

Like @ref sampleLinear(const Containers::StridedArrayView1D<const Vector3ub>&, Float)
but with a four-component input.
*/
MAGNUM_TEXTURETOOLS_EXPORT Color4 sampleLinear(const Containers::StridedArrayView1D<const Vector4ub>& texture, Float factor);

/**
@brief Sample a 1D RGB texture with sRGB interpolation
@m_since_latest

Compared to @ref sampleLinear(const Containers::StridedArrayView1D<const Vector3ub>&, Float)
treats the input values as sRGB and applies @ref Color3::fromSrgb() instead of
@ref Math::unpack(const Integral&).
*/
MAGNUM_TEXTURETOOLS_EXPORT Color3 sampleSrgb(const Containers::StridedArrayView1D<const Vector3ub>& texture, Float factor);

/**
@brief Sample a 1D RGBA texture with sRGB interpolation
@m_since_latest

Compared to @ref sampleLinear(const Containers::StridedArrayView1D<const Vector3ub>&, Float)
treats the input RGB channels as sRGB and applies @ref Color4::fromSrgbAlpha()
instead of @ref Math::unpack(const Integral&). The alpha channel is treated by
that function as linear.
*/
MAGNUM_TEXTURETOOLS_EXPORT Color4 sampleSrgbAlpha(const Containers::StridedArrayView1D<const Vector4ub>& texture, Float factor);

}}

#endif
