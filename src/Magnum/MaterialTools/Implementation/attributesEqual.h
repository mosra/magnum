#ifndef Magnum_MaterialTools_Implementation_attributesEqual_h
#define Magnum_MaterialTools_Implementation_attributesEqual_h
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

#include "Magnum/Trade/MaterialData.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Vector4.h"

/* Used by DebugTools::CompareMaterial as well, but depending on this library
   isn't desirable at the moment so it's just a header and an anonymous
   namespace to avoid clashes */
namespace Magnum { namespace MaterialTools { namespace Implementation { namespace {

bool attributesEqual(const Trade::MaterialAttributeData& a, const Trade::MaterialAttributeData& b) {
    #ifdef CORRADE_TARGET_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic error "-Wswitch"
    #endif
    CORRADE_INTERNAL_ASSERT(a.type() == b.type());
    switch(a.type()) {
        #define _c(type) case Trade::MaterialAttributeType::type:           \
            return Math::TypeTraits<type>::equals(a.value<type>(), b.value<type>());
        #define _ct(name, type) case Trade::MaterialAttributeType::name:    \
            return Math::TypeTraits<type>::equals(a.value<type>(), b.value<type>());
        _ct(Bool, bool)
        /* LCOV_EXCL_START */
        _c(Float)
        _c(Deg)
        _c(Rad)
        _c(UnsignedInt)
        _c(Int)
        _c(UnsignedLong)
        _c(Long)
        _c(Vector2)
        _c(Vector2ui)
        _c(Vector2i)
        _c(Vector3)
        _c(Vector3ui)
        _c(Vector3i)
        _c(Vector4)
        _c(Vector4ui)
        _c(Vector4i)
        _c(Matrix2x2)
        _c(Matrix2x3)
        _c(Matrix2x4)
        _c(Matrix3x2)
        _c(Matrix3x3)
        _c(Matrix3x4)
        _c(Matrix4x2)
        _c(Matrix4x3)
        /* LCOV_EXCL_STOP */
        _ct(Pointer, const void*)
        _ct(MutablePointer, void*)
        _ct(String, Containers::StringView)
        _ct(TextureSwizzle, Trade::MaterialTextureSwizzle)
        #undef _c
        #undef _ct
        case Trade::MaterialAttributeType::Buffer:
            return Containers::StringView{Containers::arrayCast<const char>(a.value<Containers::ArrayView<const void>>())} == Containers::StringView{Containers::arrayCast<const char>(b.value<Containers::ArrayView<const void>>())};
    }
    #ifdef CORRADE_TARGET_GCC
    #pragma GCC diagnostic pop
    #endif

    CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

}}}}

#endif
