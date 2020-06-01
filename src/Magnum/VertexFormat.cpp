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

#include <string>
#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/Debug.h>

namespace Magnum {

UnsignedInt vertexFormatSize(const VertexFormat format) {
    CORRADE_ASSERT(!isVertexFormatImplementationSpecific(format),
        "vertexFormatSize(): can't determine size of an implementation-specific format" << reinterpret_cast<void*>(vertexFormatUnwrap(format)), {});

    switch(format) {
        case VertexFormat::UnsignedByte:
        case VertexFormat::UnsignedByteNormalized:
        case VertexFormat::Byte:
        case VertexFormat::ByteNormalized:
            return 1;
        case VertexFormat::Half:
        case VertexFormat::UnsignedShort:
        case VertexFormat::UnsignedShortNormalized:
        case VertexFormat::Short:
        case VertexFormat::ShortNormalized:
        case VertexFormat::Vector2ub:
        case VertexFormat::Vector2ubNormalized:
        case VertexFormat::Vector2b:
        case VertexFormat::Vector2bNormalized:
            return 2;
        case VertexFormat::Vector3ub:
        case VertexFormat::Vector3ubNormalized:
        case VertexFormat::Vector3b:
        case VertexFormat::Vector3bNormalized:
            return 3;
        case VertexFormat::Float:
        case VertexFormat::UnsignedInt:
        case VertexFormat::Int:
        case VertexFormat::Vector2h:
        case VertexFormat::Vector2us:
        case VertexFormat::Vector2usNormalized:
        case VertexFormat::Vector2s:
        case VertexFormat::Vector2sNormalized:
        case VertexFormat::Vector4ub:
        case VertexFormat::Vector4ubNormalized:
        case VertexFormat::Vector4b:
        case VertexFormat::Vector4bNormalized:
        case VertexFormat::Matrix2x2bNormalized:
            return 4;
        case VertexFormat::Vector3h:
        case VertexFormat::Vector3us:
        case VertexFormat::Vector3usNormalized:
        case VertexFormat::Vector3s:
        case VertexFormat::Vector3sNormalized:
        case VertexFormat::Matrix2x3bNormalized:
        case VertexFormat::Matrix3x2bNormalized:
            return 6;
        case VertexFormat::Double:
        case VertexFormat::Vector2:
        case VertexFormat::Vector2ui:
        case VertexFormat::Vector2i:
        case VertexFormat::Vector4h:
        case VertexFormat::Vector4us:
        case VertexFormat::Vector4usNormalized:
        case VertexFormat::Vector4s:
        case VertexFormat::Vector4sNormalized:
        case VertexFormat::Matrix2x2h:
        case VertexFormat::Matrix2x2sNormalized:
        case VertexFormat::Matrix2x4bNormalized:
        case VertexFormat::Matrix2x2bNormalizedAligned:
        case VertexFormat::Matrix2x3bNormalizedAligned:
        case VertexFormat::Matrix4x2bNormalized:
            return 8;
        case VertexFormat::Matrix3x3bNormalized:
            return 9;
        case VertexFormat::Vector3:
        case VertexFormat::Vector3ui:
        case VertexFormat::Vector3i:
        case VertexFormat::Matrix2x3h:
        case VertexFormat::Matrix2x3sNormalized:
        case VertexFormat::Matrix3x2h:
        case VertexFormat::Matrix3x2sNormalized:
        case VertexFormat::Matrix3x4bNormalized:
        case VertexFormat::Matrix3x2bNormalizedAligned:
        case VertexFormat::Matrix3x3bNormalizedAligned:
        case VertexFormat::Matrix4x3bNormalized:
            return 12;
        case VertexFormat::Vector2d:
        case VertexFormat::Vector4:
        case VertexFormat::Vector4ui:
        case VertexFormat::Vector4i:
        case VertexFormat::Matrix2x2:
        case VertexFormat::Matrix2x4h:
        case VertexFormat::Matrix2x4sNormalized:
        case VertexFormat::Matrix2x3hAligned:
        case VertexFormat::Matrix2x3sNormalizedAligned:
        case VertexFormat::Matrix4x2h:
        case VertexFormat::Matrix4x2sNormalized:
        case VertexFormat::Matrix4x4bNormalized:
        case VertexFormat::Matrix4x2bNormalizedAligned:
        case VertexFormat::Matrix4x3bNormalizedAligned:
            return 16;
        case VertexFormat::Matrix3x3h:
        case VertexFormat::Matrix3x3sNormalized:
            return 18;
        case VertexFormat::Vector3d:
        case VertexFormat::Matrix2x3:
        case VertexFormat::Matrix3x2:
        case VertexFormat::Matrix3x4h:
        case VertexFormat::Matrix3x4sNormalized:
        case VertexFormat::Matrix3x3hAligned:
        case VertexFormat::Matrix3x3sNormalizedAligned:
        case VertexFormat::Matrix4x3h:
        case VertexFormat::Matrix4x3sNormalized:
            return 24;
        case VertexFormat::Vector4d:
        case VertexFormat::Matrix2x2d:
        case VertexFormat::Matrix2x4:
        case VertexFormat::Matrix4x2:
        case VertexFormat::Matrix4x4h:
        case VertexFormat::Matrix4x4sNormalized:
        case VertexFormat::Matrix4x3hAligned:
        case VertexFormat::Matrix4x3sNormalizedAligned:
            return 32;
        case VertexFormat::Matrix3x3:
            return 36;
        case VertexFormat::Matrix2x3d:
        case VertexFormat::Matrix3x2d:
        case VertexFormat::Matrix3x4:
        case VertexFormat::Matrix4x3:
            return 48;
        case VertexFormat::Matrix2x4d:
        case VertexFormat::Matrix4x2d:
        case VertexFormat::Matrix4x4:
            return 64;
        case VertexFormat::Matrix3x3d:
            return 72;
        case VertexFormat::Matrix3x4d:
        case VertexFormat::Matrix4x3d:
            return 96;
        case VertexFormat::Matrix4x4d:
            return 128;
    }

    CORRADE_ASSERT_UNREACHABLE("vertexFormatSize(): invalid format" << format, {});
}

UnsignedInt vertexFormatComponentCount(const VertexFormat format) {
    CORRADE_ASSERT(!isVertexFormatImplementationSpecific(format),
        "vertexFormatComponentCount(): can't determine component count of an implementation-specific format" << reinterpret_cast<void*>(vertexFormatUnwrap(format)), {});

    switch(format) {
        case VertexFormat::Float:
        case VertexFormat::Half:
        case VertexFormat::Double:
        case VertexFormat::UnsignedByte:
        case VertexFormat::UnsignedByteNormalized:
        case VertexFormat::Byte:
        case VertexFormat::ByteNormalized:
        case VertexFormat::UnsignedShort:
        case VertexFormat::UnsignedShortNormalized:
        case VertexFormat::Short:
        case VertexFormat::ShortNormalized:
        case VertexFormat::UnsignedInt:
        case VertexFormat::Int:
            return 1;

        case VertexFormat::Vector2:
        case VertexFormat::Vector2h:
        case VertexFormat::Vector2d:
        case VertexFormat::Vector2ub:
        case VertexFormat::Vector2ubNormalized:
        case VertexFormat::Vector2b:
        case VertexFormat::Vector2bNormalized:
        case VertexFormat::Vector2us:
        case VertexFormat::Vector2usNormalized:
        case VertexFormat::Vector2s:
        case VertexFormat::Vector2sNormalized:
        case VertexFormat::Vector2ui:
        case VertexFormat::Vector2i:
        case VertexFormat::Matrix2x2:
        case VertexFormat::Matrix2x2h:
        case VertexFormat::Matrix2x2d:
        case VertexFormat::Matrix2x2bNormalized:
        case VertexFormat::Matrix2x2sNormalized:
        case VertexFormat::Matrix2x2bNormalizedAligned:
        case VertexFormat::Matrix3x2:
        case VertexFormat::Matrix3x2h:
        case VertexFormat::Matrix3x2d:
        case VertexFormat::Matrix3x2bNormalized:
        case VertexFormat::Matrix3x2sNormalized:
        case VertexFormat::Matrix3x2bNormalizedAligned:
        case VertexFormat::Matrix4x2:
        case VertexFormat::Matrix4x2h:
        case VertexFormat::Matrix4x2d:
        case VertexFormat::Matrix4x2bNormalized:
        case VertexFormat::Matrix4x2sNormalized:
        case VertexFormat::Matrix4x2bNormalizedAligned:
            return 2;

        case VertexFormat::Vector3:
        case VertexFormat::Vector3h:
        case VertexFormat::Vector3d:
        case VertexFormat::Vector3ub:
        case VertexFormat::Vector3ubNormalized:
        case VertexFormat::Vector3b:
        case VertexFormat::Vector3bNormalized:
        case VertexFormat::Vector3us:
        case VertexFormat::Vector3usNormalized:
        case VertexFormat::Vector3s:
        case VertexFormat::Vector3sNormalized:
        case VertexFormat::Vector3ui:
        case VertexFormat::Vector3i:
        case VertexFormat::Matrix2x3:
        case VertexFormat::Matrix2x3h:
        case VertexFormat::Matrix2x3d:
        case VertexFormat::Matrix2x3bNormalized:
        case VertexFormat::Matrix2x3sNormalized:
        case VertexFormat::Matrix2x3bNormalizedAligned:
        case VertexFormat::Matrix2x3hAligned:
        case VertexFormat::Matrix2x3sNormalizedAligned:
        case VertexFormat::Matrix3x3:
        case VertexFormat::Matrix3x3h:
        case VertexFormat::Matrix3x3d:
        case VertexFormat::Matrix3x3bNormalized:
        case VertexFormat::Matrix3x3sNormalized:
        case VertexFormat::Matrix3x3bNormalizedAligned:
        case VertexFormat::Matrix3x3hAligned:
        case VertexFormat::Matrix3x3sNormalizedAligned:
        case VertexFormat::Matrix4x3:
        case VertexFormat::Matrix4x3h:
        case VertexFormat::Matrix4x3d:
        case VertexFormat::Matrix4x3bNormalized:
        case VertexFormat::Matrix4x3sNormalized:
        case VertexFormat::Matrix4x3bNormalizedAligned:
        case VertexFormat::Matrix4x3hAligned:
        case VertexFormat::Matrix4x3sNormalizedAligned:
            return 3;

        case VertexFormat::Vector4:
        case VertexFormat::Vector4h:
        case VertexFormat::Vector4d:
        case VertexFormat::Vector4ub:
        case VertexFormat::Vector4ubNormalized:
        case VertexFormat::Vector4b:
        case VertexFormat::Vector4bNormalized:
        case VertexFormat::Vector4us:
        case VertexFormat::Vector4usNormalized:
        case VertexFormat::Vector4s:
        case VertexFormat::Vector4sNormalized:
        case VertexFormat::Vector4ui:
        case VertexFormat::Vector4i:
        case VertexFormat::Matrix2x4:
        case VertexFormat::Matrix2x4h:
        case VertexFormat::Matrix2x4d:
        case VertexFormat::Matrix2x4bNormalized:
        case VertexFormat::Matrix2x4sNormalized:
        case VertexFormat::Matrix3x4:
        case VertexFormat::Matrix3x4h:
        case VertexFormat::Matrix3x4d:
        case VertexFormat::Matrix3x4bNormalized:
        case VertexFormat::Matrix3x4sNormalized:
        case VertexFormat::Matrix4x4:
        case VertexFormat::Matrix4x4h:
        case VertexFormat::Matrix4x4d:
        case VertexFormat::Matrix4x4bNormalized:
        case VertexFormat::Matrix4x4sNormalized:
            return 4;
    }

    CORRADE_ASSERT_UNREACHABLE("vertexFormatComponentCount(): invalid format" << format, {});
}

VertexFormat vertexFormatComponentFormat(const VertexFormat format) {
    CORRADE_ASSERT(!isVertexFormatImplementationSpecific(format),
        "vertexFormatComponentFormat(): can't determine component format of an implementation-specific format" << reinterpret_cast<void*>(vertexFormatUnwrap(format)), {});

    switch(format) {
        case VertexFormat::Float:
        case VertexFormat::Vector2:
        case VertexFormat::Vector3:
        case VertexFormat::Vector4:
        case VertexFormat::Matrix2x2:
        case VertexFormat::Matrix2x3:
        case VertexFormat::Matrix2x4:
        case VertexFormat::Matrix3x2:
        case VertexFormat::Matrix3x3:
        case VertexFormat::Matrix3x4:
        case VertexFormat::Matrix4x2:
        case VertexFormat::Matrix4x3:
        case VertexFormat::Matrix4x4:
            return VertexFormat::Float;

        case VertexFormat::Half:
        case VertexFormat::Vector2h:
        case VertexFormat::Vector3h:
        case VertexFormat::Vector4h:
        case VertexFormat::Matrix2x2h:
        case VertexFormat::Matrix2x3h:
        case VertexFormat::Matrix2x4h:
        case VertexFormat::Matrix2x3hAligned:
        case VertexFormat::Matrix3x2h:
        case VertexFormat::Matrix3x3h:
        case VertexFormat::Matrix3x4h:
        case VertexFormat::Matrix3x3hAligned:
        case VertexFormat::Matrix4x2h:
        case VertexFormat::Matrix4x3h:
        case VertexFormat::Matrix4x4h:
        case VertexFormat::Matrix4x3hAligned:
            return VertexFormat::Half;

        case VertexFormat::Double:
        case VertexFormat::Vector2d:
        case VertexFormat::Vector3d:
        case VertexFormat::Vector4d:
        case VertexFormat::Matrix2x2d:
        case VertexFormat::Matrix2x3d:
        case VertexFormat::Matrix2x4d:
        case VertexFormat::Matrix3x2d:
        case VertexFormat::Matrix3x3d:
        case VertexFormat::Matrix3x4d:
        case VertexFormat::Matrix4x2d:
        case VertexFormat::Matrix4x3d:
        case VertexFormat::Matrix4x4d:
            return VertexFormat::Double;

        case VertexFormat::UnsignedByte:
        case VertexFormat::UnsignedByteNormalized:
        case VertexFormat::Vector2ub:
        case VertexFormat::Vector2ubNormalized:
        case VertexFormat::Vector3ub:
        case VertexFormat::Vector3ubNormalized:
        case VertexFormat::Vector4ub:
        case VertexFormat::Vector4ubNormalized:
            return VertexFormat::UnsignedByte;

        case VertexFormat::Byte:
        case VertexFormat::ByteNormalized:
        case VertexFormat::Vector2b:
        case VertexFormat::Vector2bNormalized:
        case VertexFormat::Vector3b:
        case VertexFormat::Vector3bNormalized:
        case VertexFormat::Vector4b:
        case VertexFormat::Vector4bNormalized:
        case VertexFormat::Matrix2x2bNormalized:
        case VertexFormat::Matrix2x3bNormalized:
        case VertexFormat::Matrix2x4bNormalized:
        case VertexFormat::Matrix2x2bNormalizedAligned:
        case VertexFormat::Matrix2x3bNormalizedAligned:
        case VertexFormat::Matrix3x2bNormalized:
        case VertexFormat::Matrix3x3bNormalized:
        case VertexFormat::Matrix3x4bNormalized:
        case VertexFormat::Matrix3x2bNormalizedAligned:
        case VertexFormat::Matrix3x3bNormalizedAligned:
        case VertexFormat::Matrix4x2bNormalized:
        case VertexFormat::Matrix4x3bNormalized:
        case VertexFormat::Matrix4x4bNormalized:
        case VertexFormat::Matrix4x2bNormalizedAligned:
        case VertexFormat::Matrix4x3bNormalizedAligned:
            return VertexFormat::Byte;

        case VertexFormat::UnsignedShort:
        case VertexFormat::UnsignedShortNormalized:
        case VertexFormat::Vector2us:
        case VertexFormat::Vector2usNormalized:
        case VertexFormat::Vector3us:
        case VertexFormat::Vector3usNormalized:
        case VertexFormat::Vector4us:
        case VertexFormat::Vector4usNormalized:
            return VertexFormat::UnsignedShort;

        case VertexFormat::Short:
        case VertexFormat::ShortNormalized:
        case VertexFormat::Vector2s:
        case VertexFormat::Vector2sNormalized:
        case VertexFormat::Vector3s:
        case VertexFormat::Vector3sNormalized:
        case VertexFormat::Vector4s:
        case VertexFormat::Vector4sNormalized:
        case VertexFormat::Matrix2x2sNormalized:
        case VertexFormat::Matrix2x3sNormalized:
        case VertexFormat::Matrix2x4sNormalized:
        case VertexFormat::Matrix2x3sNormalizedAligned:
        case VertexFormat::Matrix3x2sNormalized:
        case VertexFormat::Matrix3x3sNormalized:
        case VertexFormat::Matrix3x4sNormalized:
        case VertexFormat::Matrix3x3sNormalizedAligned:
        case VertexFormat::Matrix4x2sNormalized:
        case VertexFormat::Matrix4x3sNormalized:
        case VertexFormat::Matrix4x4sNormalized:
        case VertexFormat::Matrix4x3sNormalizedAligned:
            return VertexFormat::Short;

        case VertexFormat::UnsignedInt:
        case VertexFormat::Vector2ui:
        case VertexFormat::Vector3ui:
        case VertexFormat::Vector4ui:
            return VertexFormat::UnsignedInt;

        case VertexFormat::Int:
        case VertexFormat::Vector2i:
        case VertexFormat::Vector3i:
        case VertexFormat::Vector4i:
            return VertexFormat::Int;
    }

    CORRADE_ASSERT_UNREACHABLE("vertexFormatComponentType(): invalid format" << format, {});
}

UnsignedInt vertexFormatVectorCount(const VertexFormat format) {
    CORRADE_ASSERT(!isVertexFormatImplementationSpecific(format),
        "vertexFormatVectorCount(): can't determine vector count of an implementation-specific format" << reinterpret_cast<void*>(vertexFormatUnwrap(format)), {});

    switch(format) {
        case VertexFormat::Float:
        case VertexFormat::Half:
        case VertexFormat::Double:
        case VertexFormat::UnsignedByte:
        case VertexFormat::UnsignedByteNormalized:
        case VertexFormat::Byte:
        case VertexFormat::ByteNormalized:
        case VertexFormat::UnsignedShort:
        case VertexFormat::UnsignedShortNormalized:
        case VertexFormat::Short:
        case VertexFormat::ShortNormalized:
        case VertexFormat::UnsignedInt:
        case VertexFormat::Int:
        case VertexFormat::Vector2:
        case VertexFormat::Vector2h:
        case VertexFormat::Vector2d:
        case VertexFormat::Vector2ub:
        case VertexFormat::Vector2ubNormalized:
        case VertexFormat::Vector2b:
        case VertexFormat::Vector2bNormalized:
        case VertexFormat::Vector2us:
        case VertexFormat::Vector2usNormalized:
        case VertexFormat::Vector2s:
        case VertexFormat::Vector2sNormalized:
        case VertexFormat::Vector2ui:
        case VertexFormat::Vector2i:
        case VertexFormat::Vector3:
        case VertexFormat::Vector3h:
        case VertexFormat::Vector3d:
        case VertexFormat::Vector3ub:
        case VertexFormat::Vector3ubNormalized:
        case VertexFormat::Vector3b:
        case VertexFormat::Vector3bNormalized:
        case VertexFormat::Vector3us:
        case VertexFormat::Vector3usNormalized:
        case VertexFormat::Vector3s:
        case VertexFormat::Vector3sNormalized:
        case VertexFormat::Vector3ui:
        case VertexFormat::Vector3i:
        case VertexFormat::Vector4:
        case VertexFormat::Vector4h:
        case VertexFormat::Vector4d:
        case VertexFormat::Vector4ub:
        case VertexFormat::Vector4ubNormalized:
        case VertexFormat::Vector4b:
        case VertexFormat::Vector4bNormalized:
        case VertexFormat::Vector4us:
        case VertexFormat::Vector4usNormalized:
        case VertexFormat::Vector4s:
        case VertexFormat::Vector4sNormalized:
        case VertexFormat::Vector4ui:
        case VertexFormat::Vector4i:
            return 1;

        case VertexFormat::Matrix2x2:
        case VertexFormat::Matrix2x2h:
        case VertexFormat::Matrix2x2d:
        case VertexFormat::Matrix2x2bNormalized:
        case VertexFormat::Matrix2x2sNormalized:
        case VertexFormat::Matrix2x2bNormalizedAligned:
        case VertexFormat::Matrix2x3:
        case VertexFormat::Matrix2x3h:
        case VertexFormat::Matrix2x3d:
        case VertexFormat::Matrix2x3bNormalized:
        case VertexFormat::Matrix2x3sNormalized:
        case VertexFormat::Matrix2x3bNormalizedAligned:
        case VertexFormat::Matrix2x3hAligned:
        case VertexFormat::Matrix2x3sNormalizedAligned:
        case VertexFormat::Matrix2x4:
        case VertexFormat::Matrix2x4h:
        case VertexFormat::Matrix2x4d:
        case VertexFormat::Matrix2x4bNormalized:
        case VertexFormat::Matrix2x4sNormalized:
            return 2;

        case VertexFormat::Matrix3x2:
        case VertexFormat::Matrix3x2h:
        case VertexFormat::Matrix3x2d:
        case VertexFormat::Matrix3x2bNormalized:
        case VertexFormat::Matrix3x2sNormalized:
        case VertexFormat::Matrix3x2bNormalizedAligned:
        case VertexFormat::Matrix3x3:
        case VertexFormat::Matrix3x3h:
        case VertexFormat::Matrix3x3d:
        case VertexFormat::Matrix3x3bNormalized:
        case VertexFormat::Matrix3x3sNormalized:
        case VertexFormat::Matrix3x3bNormalizedAligned:
        case VertexFormat::Matrix3x3hAligned:
        case VertexFormat::Matrix3x3sNormalizedAligned:
        case VertexFormat::Matrix3x4:
        case VertexFormat::Matrix3x4h:
        case VertexFormat::Matrix3x4d:
        case VertexFormat::Matrix3x4bNormalized:
        case VertexFormat::Matrix3x4sNormalized:
            return 3;

        case VertexFormat::Matrix4x2:
        case VertexFormat::Matrix4x2h:
        case VertexFormat::Matrix4x2d:
        case VertexFormat::Matrix4x2bNormalized:
        case VertexFormat::Matrix4x2sNormalized:
        case VertexFormat::Matrix4x2bNormalizedAligned:
        case VertexFormat::Matrix4x3:
        case VertexFormat::Matrix4x3h:
        case VertexFormat::Matrix4x3d:
        case VertexFormat::Matrix4x3bNormalized:
        case VertexFormat::Matrix4x3sNormalized:
        case VertexFormat::Matrix4x3bNormalizedAligned:
        case VertexFormat::Matrix4x3hAligned:
        case VertexFormat::Matrix4x3sNormalizedAligned:
        case VertexFormat::Matrix4x4:
        case VertexFormat::Matrix4x4h:
        case VertexFormat::Matrix4x4d:
        case VertexFormat::Matrix4x4bNormalized:
        case VertexFormat::Matrix4x4sNormalized:
            return 4;
    }

    CORRADE_ASSERT_UNREACHABLE("vertexFormatVectorCount(): invalid format" << format, {});
}

UnsignedInt vertexFormatVectorStride(const VertexFormat format) {
    CORRADE_ASSERT(!isVertexFormatImplementationSpecific(format),
        "vertexFormatVectorStride(): can't determine vector count of an implementation-specific format" << reinterpret_cast<void*>(vertexFormatUnwrap(format)), {});

    switch(format) {
        case VertexFormat::UnsignedByte:
        case VertexFormat::UnsignedByteNormalized:
        case VertexFormat::Byte:
        case VertexFormat::ByteNormalized:
            return 1;
        case VertexFormat::Half:
        case VertexFormat::UnsignedShort:
        case VertexFormat::UnsignedShortNormalized:
        case VertexFormat::Short:
        case VertexFormat::ShortNormalized:
        case VertexFormat::Vector2ub:
        case VertexFormat::Vector2ubNormalized:
        case VertexFormat::Vector2b:
        case VertexFormat::Vector2bNormalized:
        case VertexFormat::Matrix2x2bNormalized:
        case VertexFormat::Matrix3x2bNormalized:
        case VertexFormat::Matrix4x2bNormalized:
            return 2;
        case VertexFormat::Vector3ub:
        case VertexFormat::Vector3ubNormalized:
        case VertexFormat::Vector3b:
        case VertexFormat::Vector3bNormalized:
        case VertexFormat::Matrix2x3bNormalized:
        case VertexFormat::Matrix3x3bNormalized:
        case VertexFormat::Matrix4x3bNormalized:
            return 3;
        case VertexFormat::Float:
        case VertexFormat::UnsignedInt:
        case VertexFormat::Int:
        case VertexFormat::Vector2h:
        case VertexFormat::Vector2us:
        case VertexFormat::Vector2usNormalized:
        case VertexFormat::Vector2s:
        case VertexFormat::Vector2sNormalized:
        case VertexFormat::Vector4ub:
        case VertexFormat::Vector4ubNormalized:
        case VertexFormat::Vector4b:
        case VertexFormat::Vector4bNormalized:
        case VertexFormat::Matrix2x2bNormalizedAligned:
        case VertexFormat::Matrix3x2bNormalizedAligned:
        case VertexFormat::Matrix4x2bNormalizedAligned:
        case VertexFormat::Matrix2x3bNormalizedAligned:
        case VertexFormat::Matrix3x3bNormalizedAligned:
        case VertexFormat::Matrix4x3bNormalizedAligned:
        case VertexFormat::Matrix2x4bNormalized:
        case VertexFormat::Matrix3x4bNormalized:
        case VertexFormat::Matrix4x4bNormalized:
        case VertexFormat::Matrix2x2h:
        case VertexFormat::Matrix3x2h:
        case VertexFormat::Matrix4x2h:
        case VertexFormat::Matrix2x2sNormalized:
        case VertexFormat::Matrix3x2sNormalized:
        case VertexFormat::Matrix4x2sNormalized:
            return 4;
        case VertexFormat::Vector3h:
        case VertexFormat::Vector3us:
        case VertexFormat::Vector3usNormalized:
        case VertexFormat::Vector3s:
        case VertexFormat::Vector3sNormalized:
        case VertexFormat::Matrix2x3h:
        case VertexFormat::Matrix3x3h:
        case VertexFormat::Matrix4x3h:
        case VertexFormat::Matrix2x3sNormalized:
        case VertexFormat::Matrix3x3sNormalized:
        case VertexFormat::Matrix4x3sNormalized:
            return 6;
        case VertexFormat::Double:
        case VertexFormat::Vector2:
        case VertexFormat::Vector2ui:
        case VertexFormat::Vector2i:
        case VertexFormat::Vector4h:
        case VertexFormat::Vector4us:
        case VertexFormat::Vector4usNormalized:
        case VertexFormat::Vector4s:
        case VertexFormat::Vector4sNormalized:
        case VertexFormat::Matrix2x2:
        case VertexFormat::Matrix3x2:
        case VertexFormat::Matrix4x2:
        case VertexFormat::Matrix2x4h:
        case VertexFormat::Matrix3x4h:
        case VertexFormat::Matrix4x4h:
        case VertexFormat::Matrix2x4sNormalized:
        case VertexFormat::Matrix3x4sNormalized:
        case VertexFormat::Matrix4x4sNormalized:
        case VertexFormat::Matrix2x3hAligned:
        case VertexFormat::Matrix3x3hAligned:
        case VertexFormat::Matrix4x3hAligned:
        case VertexFormat::Matrix2x3sNormalizedAligned:
        case VertexFormat::Matrix3x3sNormalizedAligned:
        case VertexFormat::Matrix4x3sNormalizedAligned:
            return 8;
        case VertexFormat::Vector3:
        case VertexFormat::Vector3ui:
        case VertexFormat::Vector3i:
        case VertexFormat::Matrix2x3:
        case VertexFormat::Matrix3x3:
        case VertexFormat::Matrix4x3:
            return 12;
        case VertexFormat::Vector2d:
        case VertexFormat::Vector4:
        case VertexFormat::Vector4ui:
        case VertexFormat::Vector4i:
        case VertexFormat::Matrix2x4:
        case VertexFormat::Matrix3x4:
        case VertexFormat::Matrix4x4:
        case VertexFormat::Matrix2x2d:
        case VertexFormat::Matrix3x2d:
        case VertexFormat::Matrix4x2d:
            return 16;
        case VertexFormat::Vector3d:
        case VertexFormat::Matrix2x3d:
        case VertexFormat::Matrix3x3d:
        case VertexFormat::Matrix4x3d:
            return 24;
        case VertexFormat::Vector4d:
        case VertexFormat::Matrix2x4d:
        case VertexFormat::Matrix3x4d:
        case VertexFormat::Matrix4x4d:
            return 32;
    }

    CORRADE_ASSERT_UNREACHABLE("vertexFormatVectorStride(): invalid format" << format, {});
}

bool isVertexFormatNormalized(const VertexFormat format) {
    CORRADE_ASSERT(!isVertexFormatImplementationSpecific(format),
        "isVertexFormatNormalized(): can't determine normalization of an implementation-specific format" << reinterpret_cast<void*>(vertexFormatUnwrap(format)), {});

    switch(format) {
        case VertexFormat::Float:
        case VertexFormat::Half:
        case VertexFormat::Double:
        case VertexFormat::UnsignedByte:
        case VertexFormat::Byte:
        case VertexFormat::UnsignedShort:
        case VertexFormat::Short:
        case VertexFormat::UnsignedInt:
        case VertexFormat::Int:
        case VertexFormat::Vector2:
        case VertexFormat::Vector2h:
        case VertexFormat::Vector2d:
        case VertexFormat::Vector2ub:
        case VertexFormat::Vector2b:
        case VertexFormat::Vector2us:
        case VertexFormat::Vector2s:
        case VertexFormat::Vector2ui:
        case VertexFormat::Vector2i:
        case VertexFormat::Vector3:
        case VertexFormat::Vector3h:
        case VertexFormat::Vector3d:
        case VertexFormat::Vector3ub:
        case VertexFormat::Vector3b:
        case VertexFormat::Vector3us:
        case VertexFormat::Vector3s:
        case VertexFormat::Vector3ui:
        case VertexFormat::Vector3i:
        case VertexFormat::Vector4:
        case VertexFormat::Vector4h:
        case VertexFormat::Vector4d:
        case VertexFormat::Vector4ub:
        case VertexFormat::Vector4b:
        case VertexFormat::Vector4us:
        case VertexFormat::Vector4s:
        case VertexFormat::Vector4ui:
        case VertexFormat::Vector4i:
        case VertexFormat::Matrix2x2:
        case VertexFormat::Matrix2x2h:
        case VertexFormat::Matrix2x2d:
        case VertexFormat::Matrix2x3:
        case VertexFormat::Matrix2x3h:
        case VertexFormat::Matrix2x3d:
        case VertexFormat::Matrix2x4:
        case VertexFormat::Matrix2x4h:
        case VertexFormat::Matrix2x4d:
        case VertexFormat::Matrix2x3hAligned:
        case VertexFormat::Matrix3x2:
        case VertexFormat::Matrix3x2h:
        case VertexFormat::Matrix3x2d:
        case VertexFormat::Matrix3x3:
        case VertexFormat::Matrix3x3h:
        case VertexFormat::Matrix3x3d:
        case VertexFormat::Matrix3x4:
        case VertexFormat::Matrix3x4h:
        case VertexFormat::Matrix3x4d:
        case VertexFormat::Matrix3x3hAligned:
        case VertexFormat::Matrix4x2:
        case VertexFormat::Matrix4x2h:
        case VertexFormat::Matrix4x2d:
        case VertexFormat::Matrix4x3:
        case VertexFormat::Matrix4x3h:
        case VertexFormat::Matrix4x3d:
        case VertexFormat::Matrix4x4:
        case VertexFormat::Matrix4x4h:
        case VertexFormat::Matrix4x4d:
        case VertexFormat::Matrix4x3hAligned:
            return false;

        case VertexFormat::UnsignedByteNormalized:
        case VertexFormat::ByteNormalized:
        case VertexFormat::UnsignedShortNormalized:
        case VertexFormat::ShortNormalized:
        case VertexFormat::Vector2ubNormalized:
        case VertexFormat::Vector2bNormalized:
        case VertexFormat::Vector2usNormalized:
        case VertexFormat::Vector2sNormalized:
        case VertexFormat::Vector3ubNormalized:
        case VertexFormat::Vector3bNormalized:
        case VertexFormat::Vector3usNormalized:
        case VertexFormat::Vector3sNormalized:
        case VertexFormat::Vector4ubNormalized:
        case VertexFormat::Vector4bNormalized:
        case VertexFormat::Vector4usNormalized:
        case VertexFormat::Vector4sNormalized:
        case VertexFormat::Matrix2x2bNormalized:
        case VertexFormat::Matrix2x2sNormalized:
        case VertexFormat::Matrix2x3bNormalized:
        case VertexFormat::Matrix2x3sNormalized:
        case VertexFormat::Matrix2x4bNormalized:
        case VertexFormat::Matrix2x4sNormalized:
        case VertexFormat::Matrix2x2bNormalizedAligned:
        case VertexFormat::Matrix2x3bNormalizedAligned:
        case VertexFormat::Matrix2x3sNormalizedAligned:
        case VertexFormat::Matrix3x2bNormalized:
        case VertexFormat::Matrix3x2sNormalized:
        case VertexFormat::Matrix3x3bNormalized:
        case VertexFormat::Matrix3x3sNormalized:
        case VertexFormat::Matrix3x4bNormalized:
        case VertexFormat::Matrix3x4sNormalized:
        case VertexFormat::Matrix3x2bNormalizedAligned:
        case VertexFormat::Matrix3x3bNormalizedAligned:
        case VertexFormat::Matrix3x3sNormalizedAligned:
        case VertexFormat::Matrix4x2bNormalized:
        case VertexFormat::Matrix4x2sNormalized:
        case VertexFormat::Matrix4x3bNormalized:
        case VertexFormat::Matrix4x3sNormalized:
        case VertexFormat::Matrix4x4bNormalized:
        case VertexFormat::Matrix4x4sNormalized:
        case VertexFormat::Matrix4x2bNormalizedAligned:
        case VertexFormat::Matrix4x3bNormalizedAligned:
        case VertexFormat::Matrix4x3sNormalizedAligned:
            return true;
    }

    CORRADE_ASSERT_UNREACHABLE("isVertexFormatNormalized(): invalid format" << format, {});
}

VertexFormat vertexFormat(const VertexFormat format, const UnsignedInt componentCount, const bool normalized) {
    CORRADE_ASSERT(!isVertexFormatImplementationSpecific(format),
        "vertexFormat(): can't assemble a format out of an implementation-specific format" << reinterpret_cast<void*>(vertexFormatUnwrap(format)), {});

    VertexFormat componentFormat = vertexFormatComponentFormat(format);

    /* First turn the format into a normalized one, if requested */
    if(normalized) {
        switch(componentFormat) {
            case VertexFormat::UnsignedByte:
                componentFormat = VertexFormat::UnsignedByteNormalized;
                break;
            case VertexFormat::Byte:
                componentFormat = VertexFormat::ByteNormalized;
                break;
            case VertexFormat::UnsignedShort:
                componentFormat = VertexFormat::UnsignedShortNormalized;
                break;
            case VertexFormat::Short:
                componentFormat = VertexFormat::ShortNormalized;
                break;
            default: CORRADE_ASSERT_UNREACHABLE("vertexFormat():" << format << "can't be made normalized", {});
        }
    }

    /* Then turn them into desired component count, assuming the initial order
       is the same in all cases */
    if(componentCount == 1)
        return componentFormat;
    else if(componentCount == 2)
        return VertexFormat(UnsignedInt(VertexFormat::Vector2) +
            UnsignedInt(componentFormat) - UnsignedInt(VertexFormat::Float));
    else if(componentCount == 3)
        return VertexFormat(UnsignedInt(VertexFormat::Vector3) +
            UnsignedInt(componentFormat) - UnsignedInt(VertexFormat::Float));
    else if(componentCount == 4)
        return VertexFormat(UnsignedInt(VertexFormat::Vector4) +
            UnsignedInt(componentFormat) - UnsignedInt(VertexFormat::Float));
    else CORRADE_ASSERT_UNREACHABLE("vertexFormat(): invalid component count" << componentCount, {});
}

VertexFormat vertexFormat(const VertexFormat format, const UnsignedInt vectorCount, UnsignedInt componentCount, const bool aligned) {
    CORRADE_ASSERT(!isVertexFormatImplementationSpecific(format),
        "vertexFormat(): can't assemble a format out of an implementation-specific format" << reinterpret_cast<void*>(vertexFormatUnwrap(format)), {});
    CORRADE_ASSERT(vectorCount >= 2 && vectorCount <= 4,
        "vertexFormat(): invalid vector count" << vectorCount, {});
    CORRADE_ASSERT(componentCount >= 2 && componentCount <= 4,
        "vertexFormat(): invalid component count" << componentCount, {});

    const VertexFormat componentFormat = vertexFormatComponentFormat(format);
    const UnsignedInt vectorDistance = (vectorCount - 2)*(
        UnsignedInt(VertexFormat::Matrix3x2) -
        UnsignedInt(VertexFormat::Matrix2x2));
    const UnsignedInt componentDistance = (componentCount - 2)*(
        UnsignedInt(VertexFormat::Matrix2x3) -
        UnsignedInt(VertexFormat::Matrix2x2));

    UnsignedInt out;
    switch(componentFormat) {
        case VertexFormat::Float:
            out = UnsignedInt(VertexFormat::Matrix2x2) + componentDistance;
            break;
        case VertexFormat::Half:
            if(aligned && componentCount == 3)
                out = UnsignedInt(VertexFormat::Matrix2x3hAligned);
            else
                out = UnsignedInt(VertexFormat::Matrix2x2h) + componentDistance;
            break;
        case VertexFormat::Double:
            out = UnsignedInt(VertexFormat::Matrix2x2d) + componentDistance;
            break;
        case VertexFormat::Byte:
            if(aligned && componentCount == 2)
                out = UnsignedInt(VertexFormat::Matrix2x2bNormalizedAligned);
            else if(aligned && componentCount == 3)
                out = UnsignedInt(VertexFormat::Matrix2x3bNormalizedAligned);
            else
                out = UnsignedInt(VertexFormat::Matrix2x2bNormalized) + componentDistance;
            break;
        case VertexFormat::Short:
            if(aligned && componentCount == 3)
                out = UnsignedInt(VertexFormat::Matrix2x3sNormalizedAligned);
            else
                out = UnsignedInt(VertexFormat::Matrix2x2sNormalized) + componentDistance;
            break;
        default: CORRADE_ASSERT_UNREACHABLE("vertexFormat(): invalid matrix component type" << componentFormat << Debug::nospace << ", only floating-point or 8-/16-bit signed integer types are supported", {});
    }

    return VertexFormat(out + vectorDistance);
}

namespace {

constexpr const char* VertexFormatNames[] {
    #define _c(format) #format,
    #include "Magnum/Implementation/vertexFormatMapping.hpp"
    #undef _c
};

}

Debug& operator<<(Debug& debug, const VertexFormat value) {
    debug << "VertexFormat" << Debug::nospace;

    if(isVertexFormatImplementationSpecific(value)) {
        return debug << "::ImplementationSpecific(" << Debug::nospace << reinterpret_cast<void*>(vertexFormatUnwrap(value)) << Debug::nospace << ")";
    }

    if(UnsignedInt(value) - 1 < Containers::arraySize(VertexFormatNames)) {
        return debug << "::" << Debug::nospace << VertexFormatNames[UnsignedInt(value) - 1];
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedInt(value)) << Debug::nospace << ")";
}

}

namespace Corrade { namespace Utility {

std::string ConfigurationValue<Magnum::VertexFormat>::toString(Magnum::VertexFormat value, ConfigurationValueFlags) {
    if(Magnum::UnsignedInt(value) - 1 < Containers::arraySize(Magnum::VertexFormatNames))
        return Magnum::VertexFormatNames[Magnum::UnsignedInt(value) - 1];

    return {};
}

Magnum::VertexFormat ConfigurationValue<Magnum::VertexFormat>::fromString(const std::string& stringValue, ConfigurationValueFlags) {
    for(std::size_t i = 0; i != Containers::arraySize(Magnum::VertexFormatNames); ++i)
        if(stringValue == Magnum::VertexFormatNames[i]) return Magnum::VertexFormat(i + 1);

    return {};
}

}}
