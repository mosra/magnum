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

#include "VertexFormat.h"

#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Utility/Debug.h>

#include "Magnum/VertexFormat.h"

namespace Magnum { namespace Vk {

namespace {

#ifndef DOXYGEN_GENERATING_OUTPUT /* It gets *really* confused */
constexpr VertexFormat VertexFormatMapping[] {
    /* GCC 4.8 doesn't like just a {} for default enum values */
    #define _c(input, output) VertexFormat::output,
    #define _s(input) VertexFormat{},
    #include "Magnum/Vk/Implementation/vertexFormatMapping.hpp"
    #undef _s
    #undef _c
};
#endif

}

Debug& operator<<(Debug& debug, const VertexFormat value) {
    debug << "Vk::VertexFormat" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case Vk::VertexFormat::value: return debug << "::" << Debug::nospace << #value;
        _c(Float)
        _c(Half)
        _c(Double)
        _c(UnsignedByte)
        _c(UnsignedByteNormalized)
        _c(Byte)
        _c(ByteNormalized)
        _c(UnsignedShort)
        _c(UnsignedShortNormalized)
        _c(Short)
        _c(ShortNormalized)
        _c(UnsignedInt)
        _c(Int)
        _c(Vector2)
        _c(Vector2h)
        _c(Vector2d)
        _c(Vector2ub)
        _c(Vector2ubNormalized)
        _c(Vector2b)
        _c(Vector2bNormalized)
        _c(Vector2us)
        _c(Vector2usNormalized)
        _c(Vector2s)
        _c(Vector2sNormalized)
        _c(Vector2ui)
        _c(Vector2i)
        _c(Vector3)
        _c(Vector3h)
        _c(Vector3d)
        _c(Vector3ub)
        _c(Vector3ubNormalized)
        _c(Vector3b)
        _c(Vector3bNormalized)
        _c(Vector3us)
        _c(Vector3usNormalized)
        _c(Vector3s)
        _c(Vector3sNormalized)
        _c(Vector3ui)
        _c(Vector3i)
        _c(Vector4)
        _c(Vector4h)
        _c(Vector4d)
        _c(Vector4ub)
        _c(Vector4ubNormalized)
        _c(Vector4b)
        _c(Vector4bNormalized)
        _c(Vector4us)
        _c(Vector4usNormalized)
        _c(Vector4s)
        _c(Vector4sNormalized)
        _c(Vector4ui)
        _c(Vector4i)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    /* Vulkan docs have the values in decimal, so not converting to hex */
    return debug << "(" << Debug::nospace << Int(value) << Debug::nospace << ")";
}

bool hasVertexFormat(const Magnum::VertexFormat format) {
    if(isVertexFormatImplementationSpecific(format))
        return true;

    CORRADE_ASSERT(UnsignedInt(format) - 1 < Containers::arraySize(VertexFormatMapping),
        "Vk::hasVertexFormat(): invalid format" << format, {});
    return UnsignedInt(VertexFormatMapping[UnsignedInt(format) - 1]);
}

VertexFormat vertexFormat(const Magnum::VertexFormat format) {
    if(isVertexFormatImplementationSpecific(format))
        return vertexFormatUnwrap<VertexFormat>(format);

    CORRADE_ASSERT(UnsignedInt(format) - 1 < Containers::arraySize(VertexFormatMapping),
        "Vk::vertexFormat(): invalid format" << format, {});
    const VertexFormat out = VertexFormatMapping[UnsignedInt(format) - 1];
    CORRADE_ASSERT(UnsignedInt(out),
        "Vk::vertexFormat(): unsupported format" << format, {});
    return out;
}

}}
