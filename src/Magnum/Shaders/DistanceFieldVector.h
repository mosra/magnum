#ifndef Magnum_Shaders_DistanceFieldVector_h
#define Magnum_Shaders_DistanceFieldVector_h
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
 * @brief Struct @ref Magnum::Shaders::DistanceFieldVectorDrawUniform, @ref Magnum::Shaders::DistanceFieldVectorMaterialUniform
 */

#include "Magnum/Magnum.h"
#include "Magnum/Math/Color.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Utility/Macros.h>

#include "Magnum/Shaders/DistanceFieldVectorGL.h"
#endif

namespace Magnum { namespace Shaders {

/**
@brief Per-draw uniform for distance field vector shaders
@m_since_latest

Together with the generic @ref TransformationProjectionUniform2D /
@ref TransformationProjectionUniform3D contains parameters that are specific to
each draw call. Texture transformation, if needed, is supplied separately in a
@ref TextureTransformationUniform; material-related properties are expected to
be shared among multiple draw calls and thus are provided in a separate
@ref DistanceFieldVectorMaterialUniform structure, referenced by
@ref materialId.
@see @ref DistanceFieldVectorGL::bindDrawBuffer()
*/
struct DistanceFieldVectorDrawUniform {
    /** @brief Construct with default parameters */
    constexpr explicit DistanceFieldVectorDrawUniform(DefaultInitT = DefaultInit) noexcept:
        #if ((defined(CORRADE_TARGET_CLANG) && __clang_major__ < 4) || (defined(CORRADE_TARGET_APPLE_CLANG) && __clang_major__ < 8)) && defined(CORRADE_TARGET_BIG_ENDIAN)
        _pad0{}, /* Otherwise it refuses to constexpr, on 3.8 at least */
        #endif
        materialId{0}
        #if (defined(CORRADE_TARGET_CLANG) && __clang_major__ < 4) || (defined(CORRADE_TARGET_APPLE_CLANG) && __clang_major__ < 8)
        #ifndef CORRADE_TARGET_BIG_ENDIAN
        , _pad0{}
        #endif
        , _pad1{}, _pad2{}, _pad3{}
        #endif
        {}

    /** @brief Construct without initializing the contents */
    explicit DistanceFieldVectorDrawUniform(NoInitT) noexcept {}

    /** @{
     * @name Convenience setters
     *
     * Provided to allow the use of method chaining for populating a structure
     * in a single expression, otherwise equivalent to accessing the fields
     * directly. Also guaranteed to provide backwards compatibility when
     * packing of the actual fields changes.
     */

    /**
     * @brief Set the @ref materialId field
     * @return Reference to self (for method chaining)
     */
    DistanceFieldVectorDrawUniform& setMaterialId(UnsignedInt id) {
        materialId = id;
        return *this;
    }

    /**
     * @}
     */

    /** @var materialId
     * @brief Material ID
     *
     * References a particular material from a
     * @ref DistanceFieldVectorMaterialUniform array. Useful when an UBO with
     * more than one material is supplied or in a multi-draw scenario. Should
     * be less than the material count passed to the
     * @ref DistanceFieldVectorGL::DistanceFieldVectorGL(Flags, UnsignedInt, UnsignedInt)
     * constructor, if material count is @cpp 1 @ce, this field is assumed to
     * be @cpp 0 @ce and isn't even read by the shader. Default value is
     * @cpp 0 @ce, meaning the first material gets used.
     */

    /* This field is an UnsignedInt in the shader and materialId is extracted
       as (value & 0xffff), so the order has to be different on BE */
    #ifndef CORRADE_TARGET_BIG_ENDIAN
    alignas(4) UnsignedShort materialId;
    /* warning: Member __pad0__ is not documented. FFS DOXYGEN WHY DO YOU THINK
       I MADE THOSE UNNAMED, YOU DUMB FOOL */
    #ifndef DOXYGEN_GENERATING_OUTPUT
    UnsignedShort
        #if (defined(CORRADE_TARGET_CLANG) && __clang_major__ < 4) || (defined(CORRADE_TARGET_APPLE_CLANG) && __clang_major__ < 8)
        _pad0 /* Otherwise it refuses to constexpr, on 3.8 at least */
        #endif
        :16; /* reserved for skinOffset */
    #endif
    #else
    alignas(4) UnsignedShort
        #if (defined(CORRADE_TARGET_CLANG) && __clang_major__ < 4) || (defined(CORRADE_TARGET_APPLE_CLANG) && __clang_major__ < 8)
        _pad0 /* Otherwise it refuses to constexpr, on 3.8 at least */
        #endif
        :16; /* reserved for skinOffset */
    UnsignedShort materialId;
    #endif

    /* warning: Member __pad1__ is not documented. FFS DOXYGEN WHY DO YOU THINK
       I MADE THOSE UNNAMED, YOU DUMB FOOL */
    #ifndef DOXYGEN_GENERATING_OUTPUT
    Int
        #if (defined(CORRADE_TARGET_CLANG) && __clang_major__ < 4) || (defined(CORRADE_TARGET_APPLE_CLANG) && __clang_major__ < 8)
        _pad1 /* Otherwise it refuses to constexpr, on 3.8 at least */
        #endif
        :32; /* reserved for objectId */
    Int
        #if (defined(CORRADE_TARGET_CLANG) && __clang_major__ < 4) || (defined(CORRADE_TARGET_APPLE_CLANG) && __clang_major__ < 8)
        _pad2 /* Otherwise it refuses to constexpr, on 3.8 at least */
        #endif
        :32;
    Int
        #if (defined(CORRADE_TARGET_CLANG) && __clang_major__ < 4) || (defined(CORRADE_TARGET_APPLE_CLANG) && __clang_major__ < 8)
        _pad3 /* Otherwise it refuses to constexpr, on 3.8 at least */
        #endif
        :32;
    #endif
};

/**
@brief Material uniform for distance field vector shaders
@m_since_latest

Describes material properties referenced from
@ref DistanceFieldVectorDrawUniform::materialId.
@see @ref DistanceFieldVectorGL::bindMaterialBuffer()
*/
struct DistanceFieldVectorMaterialUniform {
    /** @brief Construct with default parameters */
    constexpr explicit DistanceFieldVectorMaterialUniform(DefaultInitT = DefaultInit) noexcept: color{1.0f, 1.0f, 1.0f, 1.0f},
        #if (defined(CORRADE_TARGET_CLANG) && __clang_major__ < 4) || (defined(CORRADE_TARGET_APPLE_CLANG) && __clang_major__ < 8)
        /* Otherwise it refuses to constexpr, on 3.8 at least */
        _pad0{}, _pad1{}, _pad2{}, _pad3{},
        #endif
        outlineColor{0.0f, 0.0f, 0.0f, 0.0f}, outlineStart{0.5f}, outlineEnd{1.0f}, smoothness{0.04f}
        #if (defined(CORRADE_TARGET_CLANG) && __clang_major__ < 4) || (defined(CORRADE_TARGET_APPLE_CLANG) && __clang_major__ < 8)
        , _pad4{}
        #endif
        {}

    /** @brief Construct without initializing the contents */
    explicit DistanceFieldVectorMaterialUniform(NoInitT) noexcept: color{NoInit}, outlineColor{NoInit} {}

    /** @{
     * @name Convenience setters
     *
     * Provided to allow the use of method chaining for populating a structure
     * in a single expression, otherwise equivalent to accessing the fields
     * directly. Also guaranteed to provide backwards compatibility when
     * packing of the actual fields changes.
     */

    /**
     * @brief Set the @ref color field
     * @return Reference to self (for method chaining)
     */
    DistanceFieldVectorMaterialUniform& setColor(const Color4& color) {
        this->color = color;
        return *this;
    }

    /**
     * @brief Set the @ref outlineColor field
     * @return Reference to self (for method chaining)
     */
    DistanceFieldVectorMaterialUniform& setOutlineColor(const Color4& color) {
        outlineColor = color;
        return *this;
    }

    /**
     * @brief Set the @ref outlineStart and @ref outlineEnd fields
     * @return Reference to self (for method chaining)
     */
    DistanceFieldVectorMaterialUniform& setOutlineRange(Float start, Float end) {
        outlineStart = start;
        outlineEnd = end;
        return *this;
    }

    /**
     * @brief Set the @ref smoothness field
     * @return Reference to self (for method chaining)
     */
    DistanceFieldVectorMaterialUniform& setSmoothness(Float smoothness) {
        this->smoothness = smoothness;
        return *this;
    }

    /**
     * @}
     */

    /**
     * @brief Fill color
     *
     * Default value is @cpp 0xffffffff_rgbaf @ce.
     * @see @ref DistanceFieldVectorGL::setColor()
     */
    Color4 color;

    /* warning: Member __pad0__ is not documented. FFS DOXYGEN WHY DO YOU THINK
       I MADE THOSE UNNAMED, YOU DUMB FOOL */
    #ifndef DOXYGEN_GENERATING_OUTPUT
    Int
        #if (defined(CORRADE_TARGET_CLANG) && __clang_major__ < 4) || (defined(CORRADE_TARGET_APPLE_CLANG) && __clang_major__ < 8)
        _pad0 /* Otherwise it refuses to constexpr, on 3.8 at least */
        #endif
        :32; /* reserved for backgroundColor */
    Int
        #if (defined(CORRADE_TARGET_CLANG) && __clang_major__ < 4) || (defined(CORRADE_TARGET_APPLE_CLANG) && __clang_major__ < 8)
        _pad1 /* Otherwise it refuses to constexpr, on 3.8 at least */
        #endif
        :32;
    Int
        #if (defined(CORRADE_TARGET_CLANG) && __clang_major__ < 4) || (defined(CORRADE_TARGET_APPLE_CLANG) && __clang_major__ < 8)
        _pad2 /* Otherwise it refuses to constexpr, on 3.8 at least */
        #endif
        :32;
    Int
        #if (defined(CORRADE_TARGET_CLANG) && __clang_major__ < 4) || (defined(CORRADE_TARGET_APPLE_CLANG) && __clang_major__ < 8)
        _pad3 /* Otherwise it refuses to constexpr, on 3.8 at least */
        #endif
        :32;
    #endif

    /**
     * @brief Outline color
     *
     * Default value is @cpp 0x00000000_rgbaf @ce and the outline is not drawn
     * --- see @ref outlineStart and @ref outlineEnd for more information.
     * @see @ref DistanceFieldVectorGL::setColor()
     */
    Color4 outlineColor;

    /**
     * @brief Outline start
     *
     * Describe where fill ends and possible outline starts. Default value is
     * @cpp 0.5f @ce, larger values will make the vector art look thinner,
     * smaller will make it look thicker.
     * @see @ref DistanceFieldVectorGL::setOutlineRange()
     */
    Float outlineStart;

    /**
     * @brief Outline end
     *
     * Describe where outline ends. If set to a value larger than
     * @ref outlineStart, the outline is not drawn. Initial value is
     * @cpp 1.0f @ce.
     * @see @ref DistanceFieldVectorGL::setOutlineRange()
     */
    Float outlineEnd;

    /**
     * @brief Smoothness radius
     *
     * Larger values will make edges look less aliased (but blurry), smaller
     * values will make them look more crisp (but possibly aliased). Initial
     * value is @cpp 0.04f @ce.
     * @see @ref DistanceFieldVectorGL::setSmoothness()
     */
    Float smoothness;

    /* warning: Member __pad4__ is not documented. FFS DOXYGEN WHY DO YOU THINK
       I MADE THOSE UNNAMED, YOU DUMB FOOL */
    #ifndef DOXYGEN_GENERATING_OUTPUT
    Int
        #if (defined(CORRADE_TARGET_CLANG) && __clang_major__ < 4) || (defined(CORRADE_TARGET_APPLE_CLANG) && __clang_major__ < 8)
        _pad4 /* Otherwise it refuses to constexpr, on 3.8 at least */
        #endif
        :32;
    #endif
};

#ifdef MAGNUM_BUILD_DEPRECATED
/** @brief @copybrief DistanceFieldVectorGL
 * @m_deprecated_since_latest Use @ref DistanceFieldVectorGL instead.
 */
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<UnsignedInt dimensions> using DistanceFieldVector CORRADE_DEPRECATED_ALIAS("use DistanceFieldVectorGL instead") = DistanceFieldVectorGL<dimensions>;
#endif

/** @brief @copybrief DistanceFieldVectorGL2D
 * @m_deprecated_since_latest Use @ref DistanceFieldVectorGL2D instead.
 */
typedef CORRADE_DEPRECATED("use DistanceFieldVectorGL2D instead") DistanceFieldVectorGL2D DistanceFieldVector2D;

/** @brief @copybrief DistanceFieldVectorGL3D
 * @m_deprecated_since_latest Use @ref DistanceFieldVectorGL3D instead.
 */
typedef CORRADE_DEPRECATED("use DistanceFieldVectorGL3D instead") DistanceFieldVectorGL3D DistanceFieldVector3D;
#endif

}}

#endif
