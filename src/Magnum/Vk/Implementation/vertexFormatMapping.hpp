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

/* See Magnum/Vk/Enums.cpp and Magnum/Vk/Test/EnumsTest.cpp */
#ifdef _c
_c(Float, R32_SFLOAT)
_c(Half, R16_SFLOAT)
_c(Double, R64_SFLOAT)
_c(UnsignedByte, R8_UINT)
_c(UnsignedByteNormalized, R8_UNORM)
_c(Byte, R8_SINT)
_c(ByteNormalized, R8_SNORM)
_c(UnsignedShort, R16_UINT)
_c(UnsignedShortNormalized, R16_UNORM)
_c(Short, R16_SINT)
_c(ShortNormalized, R16_SNORM)
_c(UnsignedInt, R32_UINT)
_c(Int, R32_SINT)
_c(Vector2, R32G32_SFLOAT)
_c(Vector2h, R16G16_SFLOAT)
_c(Vector2d, R64G64_SFLOAT)
_c(Vector2ub, R8G8_UINT)
_c(Vector2ubNormalized, R8G8_UNORM)
_c(Vector2b, R8G8_SINT)
_c(Vector2bNormalized, R8G8_SNORM)
_c(Vector2us, R16G16_UINT)
_c(Vector2usNormalized, R16G16_UNORM)
_c(Vector2s, R16G16_SINT)
_c(Vector2sNormalized, R16G16_SNORM)
_c(Vector2ui, R32G32_UINT)
_c(Vector2i, R32G32_SINT)
_c(Vector3, R32G32B32_SFLOAT)
_c(Vector3h, R16G16B16_SFLOAT)
_c(Vector3d, R64G64B64_SFLOAT)
_c(Vector3ub, R8G8B8_UINT)
_c(Vector3ubNormalized, R8G8B8_UNORM)
_c(Vector3b, R8G8B8_SINT)
_c(Vector3bNormalized, R8G8B8_SNORM)
_c(Vector3us, R16G16B16_UINT)
_c(Vector3usNormalized, R16G16B16_UNORM)
_c(Vector3s, R16G16B16_SINT)
_c(Vector3sNormalized, R16G16B16_SNORM)
_c(Vector3ui, R32G32B32_UINT)
_c(Vector3i, R32G32B32_SINT)
_c(Vector4, R32G32B32A32_SFLOAT)
_c(Vector4h, R16G16B16A16_SFLOAT)
_c(Vector4d, R64G64B64A64_SFLOAT)
_c(Vector4ub, R8G8B8A8_UINT)
_c(Vector4ubNormalized, R8G8B8A8_UNORM)
_c(Vector4b, R8G8B8A8_SINT)
_c(Vector4bNormalized, R8G8B8A8_SNORM)
_c(Vector4us, R16G16B16A16_UINT)
_c(Vector4usNormalized, R16G16B16A16_UNORM)
_c(Vector4s, R16G16B16A16_SINT)
_c(Vector4sNormalized, R16G16B16A16_SNORM)
_c(Vector4ui, R32G32B32A32_UINT)
_c(Vector4i, R32G32B32A32_SINT)
_c(Matrix2x2, R32G32_SFLOAT)
_c(Matrix2x2h, R16G16_SFLOAT)
_c(Matrix2x2d, R64G64_SFLOAT)
_c(Matrix2x2bNormalized, R8G8_SNORM)
_c(Matrix2x2sNormalized, R16G16_SNORM)
_c(Matrix2x3, R32G32B32_SFLOAT)
_c(Matrix2x3h, R16G16B16_SFLOAT)
_c(Matrix2x3d, R64G64B64_SFLOAT)
_c(Matrix2x3bNormalized, R8G8B8_SNORM)
_c(Matrix2x3sNormalized, R16G16B16_SNORM)
_c(Matrix2x4, R32G32B32A32_SFLOAT)
_c(Matrix2x4h, R16G16B16A16_SFLOAT)
_c(Matrix2x4d, R64G64B64A64_SFLOAT)
_c(Matrix2x4bNormalized, R8G8B8A8_SNORM)
_c(Matrix2x4sNormalized, R16G16B16A16_SNORM)
_c(Matrix2x2bNormalizedAligned, R8G8_SNORM)
_c(Matrix2x3hAligned, R16G16B16_SFLOAT)
_c(Matrix2x3bNormalizedAligned, R8G8B8_SNORM)
_c(Matrix2x3sNormalizedAligned, R16G16B16_SNORM)
_c(Matrix3x2, R32G32_SFLOAT)
_c(Matrix3x2h, R16G16_SFLOAT)
_c(Matrix3x2d, R64G64_SFLOAT)
_c(Matrix3x2bNormalized, R8G8_SNORM)
_c(Matrix3x2sNormalized, R16G16_SNORM)
_c(Matrix3x3, R32G32B32_SFLOAT)
_c(Matrix3x3h, R16G16B16_SFLOAT)
_c(Matrix3x3d, R64G64B64_SFLOAT)
_c(Matrix3x3bNormalized, R8G8B8_SNORM)
_c(Matrix3x3sNormalized, R16G16B16_SNORM)
_c(Matrix3x4, R32G32B32A32_SFLOAT)
_c(Matrix3x4h, R16G16B16A16_SFLOAT)
_c(Matrix3x4d, R64G64B64A64_SFLOAT)
_c(Matrix3x4bNormalized, R8G8B8A8_SNORM)
_c(Matrix3x4sNormalized, R16G16B16A16_SNORM)
_c(Matrix3x2bNormalizedAligned, R8G8_SNORM)
_c(Matrix3x3hAligned, R16G16B16_SFLOAT)
_c(Matrix3x3bNormalizedAligned, R8G8B8_SNORM)
_c(Matrix3x3sNormalizedAligned, R16G16B16_SNORM)
_c(Matrix4x2, R32G32_SFLOAT)
_c(Matrix4x2h, R16G16_SFLOAT)
_c(Matrix4x2d, R64G64_SFLOAT)
_c(Matrix4x2bNormalized, R8G8_SNORM)
_c(Matrix4x2sNormalized, R16G16_SNORM)
_c(Matrix4x3, R32G32B32_SFLOAT)
_c(Matrix4x3h, R16G16B16_SFLOAT)
_c(Matrix4x3d, R64G64B64_SFLOAT)
_c(Matrix4x3bNormalized, R8G8B8_SNORM)
_c(Matrix4x3sNormalized, R16G16B16_SNORM)
_c(Matrix4x4, R32G32B32A32_SFLOAT)
_c(Matrix4x4h, R16G16B16A16_SFLOAT)
_c(Matrix4x4d, R64G64B64A64_SFLOAT)
_c(Matrix4x4bNormalized, R8G8B8A8_SNORM)
_c(Matrix4x4sNormalized, R16G16B16A16_SNORM)
_c(Matrix4x2bNormalizedAligned, R8G8_SNORM)
_c(Matrix4x3hAligned, R16G16B16_SFLOAT)
_c(Matrix4x3bNormalizedAligned, R8G8B8_SNORM)
_c(Matrix4x3sNormalizedAligned, R16G16B16_SNORM)
#endif
