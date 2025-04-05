#ifndef Magnum_Shaders_Flat_h
#define Magnum_Shaders_Flat_h
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
 * @brief Struct @ref Magnum::Shaders::FlatDrawUniform, @ref Magnum::Shaders::FlatMaterialUniform
 */

#include "Magnum/Magnum.h"
#include "Magnum/Math/Color.h"

#if defined(MAGNUM_TARGET_GL) && defined(MAGNUM_BUILD_DEPRECATED)
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
@ref TextureTransformationUniform; material-related properties are expected to
be shared among multiple draw calls and thus are provided in a separate
@ref FlatMaterialUniform structure, referenced by @ref materialId.
@see @ref FlatGL::bindDrawBuffer()
*/
struct FlatDrawUniform {
    /** @brief Construct with default parameters */
    constexpr explicit FlatDrawUniform(DefaultInitT = DefaultInit) noexcept: materialId{0}, objectId{0},
        #ifndef CORRADE_TARGET_BIG_ENDIAN
        jointOffset{0}, perInstanceJointCount{0}
        #else
        perInstanceJointCount{0}, jointOffset{0}
        #endif
        {}

    /** @brief Construct without initializing the contents */
    explicit FlatDrawUniform(NoInitT) noexcept {}

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
    FlatDrawUniform& setMaterialId(UnsignedInt id) {
        materialId = id;
        return *this;
    }

    /**
     * @brief Set the @ref objectId field
     * @return Reference to self (for method chaining)
     */
    FlatDrawUniform& setObjectId(UnsignedInt id) {
        objectId = id;
        return *this;
    }

    /**
     * @brief Set the @ref jointOffset field
     * @return Reference to self (for method chaining)
     */
    FlatDrawUniform& setJointOffset(UnsignedInt offset) {
        jointOffset = offset;
        return *this;
    }

    /**
     * @brief Set the @ref perInstanceJointCount field
     * @return Reference to self (for method chaining)
     */
    FlatDrawUniform& setPerInstanceJointCount(UnsignedInt count) {
        perInstanceJointCount = count;
        return *this;
    }

    /**
     * @}
     */

    /** @var materialId
     * @brief Material ID
     *
     * References a particular material from a @ref FlatMaterialUniform array.
     * Useful when an UBO with more than one material is supplied or in a
     * multi-draw scenario. Should be less than the material count passed to
     * @ref FlatGL::Configuration::setMaterialCount(), if material count is
     * @cpp 1 @ce, this field is assumed to be @cpp 0 @ce and isn't even read
     * by the shader. Default value is @cpp 0 @ce, meaning the first material
     * gets used.
     */

    /* This field is an UnsignedInt in the shader and materialId is extracted
       as (value & 0xffff), so the order has to be different on BE */
    #ifndef CORRADE_TARGET_BIG_ENDIAN
    UnsignedShort materialId;
    /* warning: Member __pad0__ is not documented. FFS DOXYGEN WHY DO YOU THINK
       I MADE THOSE UNNAMED, YOU DUMB FOOL */
    #ifndef DOXYGEN_GENERATING_OUTPUT
    UnsignedShort:16; /* reserved */
    #endif
    #else
    UnsignedShort:16; /* reserved */
    UnsignedShort materialId;
    #endif

    /**
     * @brief Object ID
     *
     * Used only for the object ID framebuffer output, not to access any other
     * uniform data. Default value is @cpp 0 @ce.
     *
     * Used only if @ref FlatGL::Flag::ObjectId is enabled, ignored otherwise.
     * If @ref FlatGL::Flag::InstancedObjectId and/or
     * @ref FlatGL::Flag::ObjectIdTexture is enabled as well, this value is
     * added to the ID coming from the @ref FlatGL::ObjectId attribute and/or
     * the texture.
     * @see @ref FlatGL::setObjectId()
     */
    UnsignedInt objectId;

    /** @var jointOffset
     * @brief Joint offset
     *
     * Offset added to joint IDs in the @ref FlatGL::JointIds and
     * @ref FlatGL::SecondaryJointIds attributes. Useful when a UBO with joint
     * matrices for more than one skin is supplied or in a multi-draw scenario.
     * Should be less than the joint count passed to
     * @ref FlatGL::Configuration::setJointCount(). Default value is
     * @cpp 0 @ce, meaning no offset is added to joint IDs.
     */

    /** @var perInstanceJointCount
     * @brief Per-instance joint count
     *
     * Offset added to joint IDs in the @ref FlatGL::JointIds and
     * @ref FlatGL::SecondaryJointIds atttributes in instanced draws. Should
     * be less than the joint count passed to
     * @ref FlatGL::Configuration::setJointCount(). Default value is
     * @cpp 0 @ce, meaning every instance will use the same joint matrices,
     * setting it to a non-zero value causes the joint IDs to be interpreted as
     * @glsl gl_InstanceID*count + jointId @ce.
     */

    /* This field is an UnsignedInt in the shader and jointOffset is extracted
       as (value & 0xffff), so the order has to be different on BE */
    #ifndef CORRADE_TARGET_BIG_ENDIAN
    UnsignedShort jointOffset;
    UnsignedShort perInstanceJointCount;
    #else
    UnsignedShort perInstanceJointCount;
    UnsignedShort jointOffset;
    #endif

    /* warning: Member __pad1__ is not documented. FFS DOXYGEN WHY DO YOU THINK
       I MADE THOSE UNNAMED, YOU DUMB FOOL */
    #ifndef DOXYGEN_GENERATING_OUTPUT
    Int:32;
    #endif
};

/**
@brief Material uniform for flat shaders
@m_since_latest

Describes material properties referenced from
@ref FlatDrawUniform::materialId.
@see @ref FlatGL::bindMaterialBuffer()
*/
struct FlatMaterialUniform {
    /** @brief Construct with default parameters */
    constexpr explicit FlatMaterialUniform(DefaultInitT = DefaultInit) noexcept: color{1.0f, 1.0f, 1.0f, 1.0f}, alphaMask{0.5f} {}

    /** @brief Construct without initializing the contents */
    explicit FlatMaterialUniform(NoInitT) noexcept: color{NoInit} {}

    /** @{
     * @name Convenience setters
     *
     * Provided to allow the use of method chaining for populating a structure
     * in a single expression, otherwise equivalent to accessing the fields
     * directly. Also guaranteed to provide backwards compatibility when
     * packing of the actual fields changes.
     */

    /**
     * @brief Set the @ref alphaMask field
     * @return Reference to self (for method chaining)
     */
    FlatMaterialUniform& setAlphaMask(Float alphaMask) {
        this->alphaMask = alphaMask;
        return *this;
    }

    /**
     * @brief Set the @ref color field
     * @return Reference to self (for method chaining)
     */
    FlatMaterialUniform& setColor(const Color4& color) {
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
     * @brief Alpha mask value
     *
     * Fragments with alpha values smaller than the mask value will be
     * discarded. Default value is @cpp 0.5f @ce.
     *
     * Used only if @ref FlatGL::Flag::AlphaMask is enabled, ignored otherwise.
     * @see @ref FlatGL::setAlphaMask()
     */
    Float alphaMask;

    /* warning: Member __pad0__ is not documented. FFS DOXYGEN WHY DO YOU THINK
       I MADE THOSE UNNAMED, YOU DUMB FOOL */
    #ifndef DOXYGEN_GENERATING_OUTPUT
    Int:32;
    Int:32;
    Int:32;
    #endif
};

#if defined(MAGNUM_TARGET_GL) && defined(MAGNUM_BUILD_DEPRECATED)
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
