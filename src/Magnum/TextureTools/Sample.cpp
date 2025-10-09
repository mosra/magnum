/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
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

#include "Sample.h"

#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Utility/Assert.h>

#include "Magnum/Math/Color.h"

namespace Magnum { namespace TextureTools {

namespace {

template<class In, class Out, class In2, Out(*unpacker)(const In2&)> Out sampleInternal(
    #ifndef CORRADE_NO_ASSERT
    const char* messagePrefix,
    #endif
    const Containers::StridedArrayView1D<const In>& texture, Float factor)
{
    CORRADE_ASSERT(!texture.isEmpty(),
        messagePrefix << "expected texture to have at least one element", {});
    CORRADE_ASSERT(factor >= 0.0f && factor <= 1.0f,
        messagePrefix << "expected factor to be within the [0, 1] range but got" << factor, {});

    /* If we're exactly at the end or the texture has just a single element,
       return the last element */
    if(factor == 1.0f || texture.size() == 1)
        return unpacker(texture.back());

    /* Otherwise it's an interpolation of two values */
    const Float sample = factor*(texture.size() - 1);
    const UnsignedInt index = sample;
    return Math::lerp(
        unpacker(texture[index]),
        unpacker(texture[index + 1]),
        sample - index);
}

}

Color3 sampleLinear(const Containers::StridedArrayView1D<const Vector3ub>& texture, Float factor) {
    return sampleInternal<Vector3ub, Color3, Math::Vector<3, UnsignedByte>, Math::unpack>(
        #ifndef CORRADE_NO_ASSERT
        "TextureTools::sampleLinear():",
        #endif
        texture, factor);
}

Color4 sampleLinear(const Containers::StridedArrayView1D<const Vector4ub>& texture, Float factor) {
    return sampleInternal<Vector4ub, Color4, Math::Vector<4, UnsignedByte>, Math::unpack>(
        #ifndef CORRADE_NO_ASSERT
        "TextureTools::sampleLinear():",
        #endif
        texture, factor);
}

Color3 sampleSrgb(const Containers::StridedArrayView1D<const Vector3ub>& texture, Float factor) {
    return sampleInternal<Vector3ub, Color3, Math::Vector3<UnsignedByte>, Color3::fromSrgb>(
        #ifndef CORRADE_NO_ASSERT
        "TextureTools::sampleSrgb():",
        #endif
        texture, factor);
}

Color4 sampleSrgbAlpha(const Containers::StridedArrayView1D<const Vector4ub>& texture, Float factor) {
    return sampleInternal<Vector4ub, Color4, Math::Vector4<UnsignedByte>, Color4::fromSrgbAlpha>(
        #ifndef CORRADE_NO_ASSERT
        "TextureTools::sampleSrgbAlpha():",
        #endif
        texture, factor);
}

}}
