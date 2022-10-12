#ifndef Magnum_Shaders_Line_h
#define Magnum_Shaders_Line_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Struct @ref Magnum::Shaders::LineDrawUniform, @ref Magnum::Shaders::LineMaterialUniform
 */

#include "Magnum/Magnum.h"
#include "Magnum/Math/Color.h"

namespace Magnum { namespace Shaders {

struct LineDrawUniform {
    /** @brief Construct with default parameters */
    constexpr explicit LineDrawUniform(DefaultInitT = DefaultInit) noexcept: materialId{0}, objectId{0} {}

    /** @brief Construct without initializing the contents */
    explicit LineDrawUniform(NoInitT) noexcept {}

    LineDrawUniform& setMaterialId(UnsignedInt id) {
        materialId = id;
        return *this;
    }

    LineDrawUniform& setObjectId(UnsignedInt id) {
        objectId = id;
        return *this;
    }

    /* This field is an UnsignedInt in the shader and materialId is extracted
       as (value & 0xffff), so the order has to be different on BE */
    #ifndef CORRADE_TARGET_BIG_ENDIAN
    alignas(4) UnsignedShort materialId;
    /* warning: Member __pad0__ is not documented. FFS DOXYGEN WHY DO YOU THINK
       I MADE THOSE UNNAMED, YOU DUMB FOOL */
    #ifndef DOXYGEN_GENERATING_OUTPUT
    UnsignedShort:16; /* reserved for skinOffset */
    #endif
    #else
    alignas(4) UnsignedShort:16; /* reserved for skinOffset */
    UnsignedShort materialId;
    #endif

    UnsignedInt objectId;

    /* warning: Member __pad1__ is not documented. FFS DOXYGEN WHY DO YOU THINK
       I MADE THOSE UNNAMED, YOU DUMB FOOL */
    #ifndef DOXYGEN_GENERATING_OUTPUT
    Int:32;
    Int:32;
    #endif
};

struct LineMaterialUniform {
    /** @brief Construct with default parameters */
    constexpr explicit LineMaterialUniform(DefaultInitT = DefaultInit) noexcept: color{1.0f, 1.0f, 1.0f, 1.0f} {}

    /** @brief Construct without initializing the contents */
    explicit LineMaterialUniform(NoInitT) noexcept: color{NoInit} {}

    LineMaterialUniform& setColor(const Color4& color) {
        this->color = color;
        return *this;
    }

    Color4 color;
};

}}

#endif
