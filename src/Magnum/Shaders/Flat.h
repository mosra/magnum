#ifndef Magnum_Shaders_Flat_h
#define Magnum_Shaders_Flat_h
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

/** @file
 * @brief Struct @ref Magnum::Shaders::FlatDrawUniform
 */

#include "Magnum/Magnum.h"
#include "Magnum/Math/Color.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Utility/Macros.h>

#include "Magnum/Shaders/FlatGL.h"
#endif

namespace Magnum { namespace Shaders {

/**
@brief Per-draw uniform for flat shaders
@m_since_latest

Together with the generic @ref TransformationProjectionUniform2D /
@ref TransformationProjectionUniform3D contains parameters that are specific to
each draw call. Texture transformation, if needed, is supplied separately in a
@ref TextureTransformationUniform.
@see @ref FlatGL::bindDrawBuffer()
*/
struct FlatDrawUniform {
    /** @brief Construct with default parameters */
    constexpr explicit FlatDrawUniform(DefaultInitT = DefaultInit) noexcept: color{1.0f, 1.0f, 1.0f, 1.0f}, objectId{0},
        #if (defined(CORRADE_TARGET_CLANG) && __clang_major__ < 4) || (defined(CORRADE_TARGET_APPLE_CLANG) && __clang_major__ < 8)
        /* Otherwise it refuses to constexpr, on 3.8 at least */
        _pad0{}, _pad1{},
        #endif
        alphaMask{0.5f}
        #if (defined(CORRADE_TARGET_CLANG) && __clang_major__ < 4) || (defined(CORRADE_TARGET_APPLE_CLANG) && __clang_major__ < 8)
        , _pad2{}
        #endif
        {}

    /** @brief Construct without initializing the contents */
    explicit FlatDrawUniform(NoInitT) noexcept: color{NoInit} {}

    /** @{
     * @name Convenience setters
     *
     * Provided to allow the use of method chaining for populating a structure
     * in a single expression, otherwise equivalent to accessing the fields
     * directly. Also guaranteed to provide backwards compatibility when
     * packing of the actual fields changes.
     */

    /**
     * @brief Set the @ref objectId field
     * @return Reference to self (for method chaining)
     */
    FlatDrawUniform& setObjectId(UnsignedInt id) {
        objectId = id;
        return *this;
    }

    /**
     * @brief Set the @ref alphaMask field
     * @return Reference to self (for method chaining)
     */
    FlatDrawUniform& setAlphaMask(Float alphaMask) {
        this->alphaMask = alphaMask;
        return *this;
    }

    /**
     * @brief Set the @ref color field
     * @return Reference to self (for method chaining)
     */
    FlatDrawUniform& setColor(const Color4& color) {
        this->color = color;
        return *this;
    }

    /**
     * @}
     */

    /**
     * @brief Color
     *
     * Default value is @cpp 0xffffffff_rgbaf @ce.
     *
     * If @ref FlatGL::Flag::VertexColor is enabled, the color is multiplied
     * with a color coming from the @ref FlatGL::Color3 / @ref FlatGL::Color4
     * attribute.
     * @see @ref FlatGL::setColor()
     */
    Color4 color;

    /**
     * @brief Object ID
     *
     * Used only for the object ID framebuffer output, not to access any other
     * uniform data. Default value is @cpp 0 @ce.
     *
     * Used only if @ref FlatGL::Flag::ObjectId is enabled, ignored otherwise.
     * If @ref FlatGL::Flag::InstancedObjectId is enabled as well, this value
     * is added to the ID coming from the @ref FlatGL::ObjectId attribute.
     * @see @ref FlatGL::setObjectId()
     */
    UnsignedInt objectId;

    /* warning: Member __pad0__ is not documented. FFS DOXYGEN WHY DO YOU THINK
       I MADE THOSE UNNAMED, YOU DUMB FOOL */
    #ifndef DOXYGEN_GENERATING_OUTPUT
    /* This field is an UnsignedInt in the shader and skinOffset is extracted
       as (value >> 16), so the order has to be different on BE */
    #ifndef CORRADE_TARGET_BIG_ENDIAN
    UnsignedShort
        #if (defined(CORRADE_TARGET_CLANG) && __clang_major__ < 4) || (defined(CORRADE_TARGET_APPLE_CLANG) && __clang_major__ < 8)
        _pad0 /* Otherwise it refuses to constexpr, on 3.8 at least */
        #endif
        :16;
    UnsignedShort
        #if (defined(CORRADE_TARGET_CLANG) && __clang_major__ < 4) || (defined(CORRADE_TARGET_APPLE_CLANG) && __clang_major__ < 8)
        _pad1 /* Otherwise it refuses to constexpr, on 3.8 at least */
        #endif
        :16; /* reserved for skinOffset */
    #else
    UnsignedShort
        #if (defined(CORRADE_TARGET_CLANG) && __clang_major__ < 4) || (defined(CORRADE_TARGET_APPLE_CLANG) && __clang_major__ < 8)
        _pad0 /* Otherwise it refuses to constexpr, on 3.8 at least */
        #endif
        :16; /* reserved for skinOffset */
    UnsignedShort
        #if (defined(CORRADE_TARGET_CLANG) && __clang_major__ < 4) || (defined(CORRADE_TARGET_APPLE_CLANG) && __clang_major__ < 8)
        _pad1 /* Otherwise it refuses to constexpr, on 3.8 at least */
        #endif
        :16;
    #endif
    #endif

    /**
     * @brief Alpha mask value
     *
     * Fragments with alpha values smaller than the mask value will be
     * discarded. Default value is @cpp 0.5f @ce.
     *
     * Used only if @ref FlatGL::Flag::AlphaMask is enabled, ignored otherwise.
     * @see @ref FlatGL::setAlphaMask()
     */
    Float alphaMask;

    /* warning: Member __pad2__ is not documented. FFS DOXYGEN WHY DO YOU THINK
       I MADE THOSE UNNAMED, YOU DUMB FOOL */
    #ifndef DOXYGEN_GENERATING_OUTPUT
    Int
        #if (defined(CORRADE_TARGET_CLANG) && __clang_major__ < 4) || (defined(CORRADE_TARGET_APPLE_CLANG) && __clang_major__ < 8)
        _pad2 /* Otherwise it refuses to constexpr, on 3.8 at least */
        #endif
        :32;
    #endif
};

#ifdef MAGNUM_BUILD_DEPRECATED
/** @brief @copybrief FlatGL
 * @m_deprecated_since_latest Use @ref FlatGL instead.
 */
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<UnsignedInt dimensions> using Flat CORRADE_DEPRECATED_ALIAS("use FlatGL instead") = FlatGL<dimensions>;
#endif

/** @brief @copybrief FlatGL2D
 * @m_deprecated_since_latest Use @ref FlatGL2D instead.
 */
typedef CORRADE_DEPRECATED("use FlatGL2D instead") FlatGL2D Flat2D;

/** @brief @copybrief FlatGL3D
 * @m_deprecated_since_latest Use @ref FlatGL3D instead.
 */
typedef CORRADE_DEPRECATED("use FlatGL3D instead") FlatGL3D Flat3D;
#endif

}}

#endif
