/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021 Vladimír Vondruš <mosra@centrum.cz>

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

#include "SceneData.h"

namespace Magnum { namespace Trade {

Debug& operator<<(Debug& debug, const SceneField value) {
    debug << "Trade::SceneField" << Debug::nospace;

    if(UnsignedShort(value) >= UnsignedShort(SceneField::Custom))
        return debug << "::Custom(" << Debug::nospace << (UnsignedShort(value) - UnsignedShort(SceneField::Custom)) << Debug::nospace << ")";

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case SceneField::value: return debug << "::" #value;
        _c(Parent)
        _c(Transformation)
        _c(Translation)
        _c(Rotation)
        _c(Scaling)
        _c(MeshId)
        _c(MeshMaterialId)
        _c(LightId)
        _c(CameraId)
        _c(AnimationId)
        _c(SkinId)
        #undef _c
        /* LCOV_EXCL_STOP */

        /* To silence compiler warning about unhandled values */
        case SceneField::Custom: CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedShort(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const SceneFieldType value) {
    debug << "Trade::SceneFieldType" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case SceneFieldType::value: return debug << "::" #value;
        _c(Float)
        _c(Half)
        _c(Double)
        _c(UnsignedByte)
        _c(Byte)
        _c(UnsignedShort)
        _c(Short)
        _c(UnsignedInt)
        _c(Int)
        _c(UnsignedLong)
        _c(Long)
        _c(Vector2)
        _c(Vector2h)
        _c(Vector2d)
        _c(Vector2ub)
        _c(Vector2b)
        _c(Vector2us)
        _c(Vector2s)
        _c(Vector2ui)
        _c(Vector2i)
        _c(Vector3)
        _c(Vector3h)
        _c(Vector3d)
        _c(Vector3ub)
        _c(Vector3b)
        _c(Vector3us)
        _c(Vector3s)
        _c(Vector3ui)
        _c(Vector3i)
        _c(Vector4)
        _c(Vector4h)
        _c(Vector4d)
        _c(Vector4ub)
        _c(Vector4b)
        _c(Vector4us)
        _c(Vector4s)
        _c(Vector4ui)
        _c(Vector4i)
        _c(Matrix2x2)
        _c(Matrix2x2h)
        _c(Matrix2x2d)
        _c(Matrix2x3)
        _c(Matrix2x3h)
        _c(Matrix2x3d)
        _c(Matrix2x4)
        _c(Matrix2x4h)
        _c(Matrix2x4d)
        _c(Matrix3x2)
        _c(Matrix3x2h)
        _c(Matrix3x2d)
        _c(Matrix3x3)
        _c(Matrix3x3h)
        _c(Matrix3x3d)
        _c(Matrix3x4)
        _c(Matrix3x4h)
        _c(Matrix3x4d)
        _c(Matrix4x2)
        _c(Matrix4x2h)
        _c(Matrix4x2d)
        _c(Matrix4x3)
        _c(Matrix4x3h)
        _c(Matrix4x3d)
        _c(Matrix4x4)
        _c(Matrix4x4h)
        _c(Matrix4x4d)
        _c(Range1D)
        _c(Range1Dh)
        _c(Range1Dd)
        _c(Range1Di)
        _c(Range2D)
        _c(Range2Dh)
        _c(Range2Dd)
        _c(Range2Di)
        _c(Range3D)
        _c(Range3Dh)
        _c(Range3Dd)
        _c(Range3Di)
        _c(Complex)
        _c(Complexd)
        _c(DualComplex)
        _c(DualComplexd)
        _c(Quaternion)
        _c(Quaterniond)
        _c(DualQuaternion)
        _c(DualQuaterniond)
        _c(Deg)
        _c(Degh)
        _c(Degd)
        _c(Rad)
        _c(Radh)
        _c(Radd)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedShort(value)) << Debug::nospace << ")";
}

UnsignedInt sceneFieldTypeSize(const SceneFieldType type) {
    switch(type) {
        case SceneFieldType::UnsignedByte:
        case SceneFieldType::Byte:
            return 1;
        case SceneFieldType::UnsignedShort:
        case SceneFieldType::Short:
        case SceneFieldType::Half:
        case SceneFieldType::Vector2ub:
        case SceneFieldType::Vector2b:
        case SceneFieldType::Degh:
        case SceneFieldType::Radh:
            return 2;
        case SceneFieldType::Vector3ub:
        case SceneFieldType::Vector3b:
            return 3;
        case SceneFieldType::UnsignedInt:
        case SceneFieldType::Int:
        case SceneFieldType::Float:
        case SceneFieldType::Vector2us:
        case SceneFieldType::Vector2s:
        case SceneFieldType::Vector2h:
        case SceneFieldType::Vector4ub:
        case SceneFieldType::Vector4b:
        case SceneFieldType::Range1Dh:
        case SceneFieldType::Deg:
        case SceneFieldType::Rad:
            return 4;
        case SceneFieldType::Vector3us:
        case SceneFieldType::Vector3s:
        case SceneFieldType::Vector3h:
            return 6;
        case SceneFieldType::UnsignedLong:
        case SceneFieldType::Long:
        case SceneFieldType::Double:
        case SceneFieldType::Vector2:
        case SceneFieldType::Vector2ui:
        case SceneFieldType::Vector2i:
        case SceneFieldType::Vector4us:
        case SceneFieldType::Vector4s:
        case SceneFieldType::Vector4h:
        case SceneFieldType::Matrix2x2h:
        case SceneFieldType::Range1D:
        case SceneFieldType::Range1Di:
        case SceneFieldType::Range2Dh:
        case SceneFieldType::Complex:
        case SceneFieldType::Degd:
        case SceneFieldType::Radd:
            return 8;
        case SceneFieldType::Vector3:
        case SceneFieldType::Vector3ui:
        case SceneFieldType::Vector3i:
        case SceneFieldType::Matrix2x3h:
        case SceneFieldType::Matrix3x2h:
        case SceneFieldType::Range3Dh:
            return 12;
        case SceneFieldType::Vector2d:
        case SceneFieldType::Vector4:
        case SceneFieldType::Vector4ui:
        case SceneFieldType::Vector4i:
        case SceneFieldType::Matrix2x2:
        case SceneFieldType::Matrix2x4h:
        case SceneFieldType::Matrix4x2h:
        case SceneFieldType::Range1Dd:
        case SceneFieldType::Range2D:
        case SceneFieldType::Range2Di:
        case SceneFieldType::Complexd:
        case SceneFieldType::DualComplex:
        case SceneFieldType::Quaternion:
            return 16;
        case SceneFieldType::Matrix3x3h:
            return 18;
        case SceneFieldType::Vector3d:
        case SceneFieldType::Matrix2x3:
        case SceneFieldType::Matrix3x4h:
        case SceneFieldType::Matrix3x2:
        case SceneFieldType::Matrix4x3h:
        case SceneFieldType::Range3D:
        case SceneFieldType::Range3Di:
            return 24;
        case SceneFieldType::Vector4d:
        case SceneFieldType::Matrix2x2d:
        case SceneFieldType::Matrix2x4:
        case SceneFieldType::Matrix4x2:
        case SceneFieldType::Matrix4x4h:
        case SceneFieldType::Range2Dd:
        case SceneFieldType::DualComplexd:
        case SceneFieldType::Quaterniond:
        case SceneFieldType::DualQuaternion:
            return 32;
        case SceneFieldType::Matrix3x3:
            return 36;
        case SceneFieldType::Matrix2x3d:
        case SceneFieldType::Matrix3x4:
        case SceneFieldType::Matrix3x2d:
        case SceneFieldType::Matrix4x3:
        case SceneFieldType::Range3Dd:
            return 48;
        case SceneFieldType::Matrix2x4d:
        case SceneFieldType::Matrix4x2d:
        case SceneFieldType::Matrix4x4:
        case SceneFieldType::DualQuaterniond:
            return 64;
        case SceneFieldType::Matrix3x3d:
            return 72;
        case SceneFieldType::Matrix3x4d:
        case SceneFieldType::Matrix4x3d:
            return 96;
        case SceneFieldType::Matrix4x4d:
            return 128;
    }

    CORRADE_ASSERT_UNREACHABLE("Trade::sceneFieldTypeSize(): invalid type" << type, {});
}

Debug& operator<<(Debug& debug, const SceneIndexType value) {
    debug << "Trade::SceneIndexType" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case SceneIndexType::value: return debug << "::" #value;
        _c(UnsignedByte)
        _c(UnsignedInt)
        _c(UnsignedShort)
        _c(UnsignedLong)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedByte(value)) << Debug::nospace << ")";
}

UnsignedInt sceneIndexTypeSize(const SceneIndexType type) {
    switch(type) {
        case SceneIndexType::UnsignedByte: return 1;
        case SceneIndexType::UnsignedShort: return 2;
        case SceneIndexType::UnsignedInt: return 4;
        case SceneIndexType::UnsignedLong: return 8;
    }

    CORRADE_ASSERT_UNREACHABLE("Trade::sceneIndexTypeSize(): invalid type" << type, {});
}

}}
