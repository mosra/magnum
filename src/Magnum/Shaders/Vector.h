#ifndef Magnum_Shaders_Vector_h
#define Magnum_Shaders_Vector_h
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

/** @file
 * @brief Struct @ref Magnum::Shaders::VectorDrawUniform, @ref Magnum::Shaders::VectorMaterialUniform
 */

#include "Magnum/Magnum.h"
#include "Magnum/Math/Color.h"

#if defined(MAGNUM_TARGET_GL) && defined(MAGNUM_BUILD_DEPRECATED)
#include <Corrade/Utility/Macros.h>

#include "Magnum/Shaders/VectorGL.h"
#endif

namespace Magnum { namespace Shaders {

/**
@brief Per-draw uniform for vector shaders
@m_since_latest

Together with the generic @ref TransformationProjectionUniform2D /
@ref TransformationProjectionUniform3D contains parameters that are specific to
each draw call. Texture transformation, if needed, is supplied separately in a
@ref TextureTransformationUniform; material-related properties are expected to
be shared among multiple draw calls and thus are provided in a separate
@ref VectorMaterialUniform structure, referenced by @ref materialId.
@see @ref VectorGL::bindDrawBuffer()
*/
struct VectorDrawUniform {
    /** @brief Construct with default parameters */
    constexpr explicit VectorDrawUniform(DefaultInitT = DefaultInit) noexcept: materialId{0} {}

    /** @brief Construct without initializing the contents */
    explicit VectorDrawUniform(NoInitT) noexcept {}

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
    VectorDrawUniform& setMaterialId(UnsignedInt id) {
        materialId = id;
        return *this;
    }

    /**
     * @}
     */

    /** @var materialId
     * @brief Material ID
     *
     * References a particular material from a @ref VectorMaterialUniform
     * array. Useful when an UBO with more than one material is supplied or in
     * a multi-draw scenario. Should be less than the material count passed to
     * @ref VectorGL::Configuration::setMaterialCount(), if material count is
     * @cpp 1 @ce, this field is assumed to be @cpp 0 @ce and isn't even read
     * by the shader. Default value is @cpp 0 @ce, meaning the first material
     * gets used.
     */

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

    /* warning: Member __pad1__ is not documented. FFS DOXYGEN WHY DO YOU THINK
       I MADE THOSE UNNAMED, YOU DUMB FOOL */
    #ifndef DOXYGEN_GENERATING_OUTPUT
    Int:32; /* reserved for objectId */
    Int:32;
    Int:32;
    #endif
};

/**
@brief Material uniform for vector shaders
@m_since_latest

Describes material properties referenced from
@ref VectorDrawUniform::materialId.
@see @ref VectorGL::bindMaterialBuffer()
*/
struct VectorMaterialUniform {
    /** @brief Construct with default parameters */
    constexpr explicit VectorMaterialUniform(DefaultInitT = DefaultInit) noexcept: color{1.0f, 1.0f, 1.0f, 1.0f}, backgroundColor{0.0f, 0.0f, 0.0f, 0.0f} {}

    /** @brief Construct without initializing the contents */
    explicit VectorMaterialUniform(NoInitT) noexcept: color{NoInit}, backgroundColor{NoInit} {}

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
    VectorMaterialUniform& setColor(const Color4& color) {
        this->color = color;
        return *this;
    }

    /**
     * @brief Set the @ref backgroundColor field
     * @return Reference to self (for method chaining)
     */
    VectorMaterialUniform& setBackgroundColor(const Color4& color) {
        backgroundColor = color;
        return *this;
    }

    /**
     * @}
     */

    /**
     * @brief Fill color
     *
     * Default is @cpp 0xffffffff_rgbaf @ce.
     * @see @ref VectorGL::setColor()
     */
    Color4 color;

    /**
     * @brief Background color
     *
     * Default is @cpp 0x00000000_rgbaf @ce.
     * @see @ref VectorGL::setBackgroundColor()
     */
    Color4 backgroundColor;

    /* warning: Member __pad0__ is not documented. FFS DOXYGEN WHY DO YOU THINK
       I MADE THOSE UNNAMED, YOU DUMB FOOL */
    #ifndef DOXYGEN_GENERATING_OUTPUT
    Int:32; /* reserved for alpha mask */
    Int:32;
    Int:32;
    Int:32;
    #endif
};

#if defined(MAGNUM_TARGET_GL) && defined(MAGNUM_BUILD_DEPRECATED)
/** @brief @copybrief Shaders::VectorGL
 * @m_deprecated_since_latest Use @ref Shaders::VectorGL "VectorGL" instead.
 */
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<UnsignedInt dimensions> using Vector CORRADE_DEPRECATED_ALIAS("use VectorGL instead") = VectorGL<dimensions>;
#endif

/** @brief @copybrief VectorGL2D
 * @m_deprecated_since_latest Use @ref VectorGL2D instead.
 */
typedef CORRADE_DEPRECATED("use VectorGL2D instead") VectorGL2D Vector2D;

/** @brief @copybrief VectorGL3D
 * @m_deprecated_since_latest Use @ref VectorGL3D instead.
 */
typedef CORRADE_DEPRECATED("use VectorGL3D instead") VectorGL3D Vector3D;
#endif

}}

#endif
