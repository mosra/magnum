#ifndef Magnum_Shaders_MeshVisualizer_h
#define Magnum_Shaders_MeshVisualizer_h
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
 * @brief Struct @ref Magnum::Shaders::MeshVisualizerDrawUniform2D, @ref Magnum::Shaders::MeshVisualizerDrawUniform3D, @ref Magnum::Shaders::MeshVisualizerMaterialUniform
 */

#include "Magnum/Magnum.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Matrix.h"

#if defined(MAGNUM_TARGET_GL) && defined(MAGNUM_BUILD_DEPRECATED)
#include <Corrade/Utility/Macros.h>

#include "Magnum/Shaders/MeshVisualizerGL.h"
#endif

namespace Magnum { namespace Shaders {

/**
@brief Per-draw uniform for 2D mesh visualizer shaders
@m_since_latest

Together with the generic @ref TransformationProjectionUniform2D contains
parameters that are specific to each draw call. Material-related properties are
expected to be shared among multiple draw calls and thus are provided in a
separate @ref MeshVisualizerMaterialUniform structure, referenced by
@ref materialId.
@see @ref MeshVisualizerGL2D::bindDrawBuffer()
*/
struct MeshVisualizerDrawUniform2D {
    /** @brief Construct with default parameters */
    constexpr explicit MeshVisualizerDrawUniform2D(DefaultInitT = DefaultInit) noexcept: materialId{0}, objectId{0},
        #ifndef CORRADE_TARGET_BIG_ENDIAN
        jointOffset{0}, perInstanceJointCount{0}
        #else
        perInstanceJointCount{0}, jointOffset{0}
        #endif
        {}

    /** @brief Construct without initializing the contents */
    explicit MeshVisualizerDrawUniform2D(NoInitT) noexcept {}

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
    MeshVisualizerDrawUniform2D& setMaterialId(UnsignedInt id) {
        materialId = id;
        return *this;
    }

    /**
     * @brief Set the @ref objectId field
     * @return Reference to self (for method chaining)
     */
    MeshVisualizerDrawUniform2D& setObjectId(UnsignedInt id) {
        objectId = id;
        return *this;
    }

    /**
     * @brief Set the @ref jointOffset field
     * @return Reference to self (for method chaining)
     */
    MeshVisualizerDrawUniform2D& setJointOffset(UnsignedInt offset) {
        jointOffset = offset;
        return *this;
    }

    /**
     * @brief Set the @ref perInstanceJointCount field
     * @return Reference to self (for method chaining)
     */
    MeshVisualizerDrawUniform2D& setPerInstanceJointCount(UnsignedInt count) {
        perInstanceJointCount = count;
        return *this;
    }

    /**
     * @}
     */

    /** @var materialId
     * @brief Material ID
     *
     * References a particular material from a
     * @ref MeshVisualizerMaterialUniform array. Useful when an UBO with
     * more than one material is supplied or in a multi-draw scenario. Should
     * be less than the material count passed to
     * @ref MeshVisualizerGL2D::Configuration::setMaterialCount() /
     * @ref MeshVisualizerGL3D::Configuration::setMaterialCount(), if material
     * count is @cpp 1 @ce, this field is assumed to be @cpp 0 @ce and isn't
     * even read by the shader. Default value is @cpp 0 @ce, meaning the first
     * material gets used.
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
     * Unlike @ref materialId, this index is used only for the object ID
     * visualization, not to access any other uniform data. Default value
     * is @cpp 0 @ce.
     *
     * Used only if @ref MeshVisualizerGL2D::Flag::ObjectId is enabled, ignored
     * otherwise. If @ref MeshVisualizerGL2D::Flag::InstancedObjectId is
     * enabled as well, this value is added to the ID coming from the
     * @ref MeshVisualizerGL2D::ObjectId attribute.
     * @see @ref MeshVisualizerGL2D::setObjectId()
     */
    UnsignedInt objectId;

    /** @var jointOffset
     * @brief Joint offset
     *
     * Offset added to joint IDs in the @ref MeshVisualizerGL2D::JointIds and
     * @ref MeshVisualizerGL2D::SecondaryJointIds attributes. Useful when a UBO
     * with joint matrices for more than one skin is supplied or in a
     * multi-draw scenario. Should be less than the joint count passed to
     * @ref MeshVisualizerGL2D::Configuration::setJointCount(). Default value
     * is @cpp 0 @ce, meaning no offset is added to joint IDs.
     */

    /** @var perInstanceJointCount
     * @brief Per-instance joint count
     *
     * Offset added to joint IDs in the @ref MeshVisualizerGL2D::JointIds and
     * @ref MeshVisualizerGL2D::SecondaryJointIds atttributes in instanced
     * draws. Should be less than the joint count passed to
     * @ref MeshVisualizerGL2D::Configuration::setJointCount(). Default value
     * is @cpp 0 @ce, meaning every instance will use the same joint matrices,
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
@brief Per-draw uniform for 3D mesh visualizer shaders
@m_since_latest

Together with the generic @ref TransformationUniform3D contains parameters that
are specific to each draw call. Material-related properties are expected to be
shared among multiple draw calls and thus are provided in a separate
@ref MeshVisualizerMaterialUniform structure, referenced by @ref materialId.
@see @ref MeshVisualizerGL3D::bindDrawBuffer()
*/
struct MeshVisualizerDrawUniform3D {
    /** @brief Construct with default parameters */
    constexpr explicit MeshVisualizerDrawUniform3D(DefaultInitT = DefaultInit) noexcept: normalMatrix{Math::IdentityInit}, materialId{0}, objectId{0},
        #ifndef CORRADE_TARGET_BIG_ENDIAN
        jointOffset{0}, perInstanceJointCount{0}
        #else
        perInstanceJointCount{0}, jointOffset{0}
        #endif
        {}

    /** @brief Construct without initializing the contents */
    explicit MeshVisualizerDrawUniform3D(NoInitT) noexcept: normalMatrix{NoInit} {}

    /** @{
     * @name Convenience setters
     *
     * Provided to allow the use of method chaining for populating a structure
     * in a single expression, otherwise equivalent to accessing the fields
     * directly. Also guaranteed to provide backwards compatibility when
     * packing of the actual fields changes.
     */

    /**
     * @brief Set the @ref normalMatrix field
     * @return Reference to self (for method chaining)
     *
     * The matrix is expanded to @relativeref{Magnum,Matrix3x4}, with the
     * bottom row being zeros.
     */
    MeshVisualizerDrawUniform3D& setNormalMatrix(const Matrix3x3& matrix) {
        normalMatrix = Matrix3x4{matrix};
        return *this;
    }

    /**
     * @brief Set the @ref materialId field
     * @return Reference to self (for method chaining)
     */
    MeshVisualizerDrawUniform3D& setMaterialId(UnsignedInt id) {
        materialId = id;
        return *this;
    }

    /**
     * @brief Set the @ref objectId field
     * @return Reference to self (for method chaining)
     */
    MeshVisualizerDrawUniform3D& setObjectId(UnsignedInt id) {
        objectId = id;
        return *this;
    }

    /**
     * @brief Set the @ref jointOffset field
     * @return Reference to self (for method chaining)
     */
    MeshVisualizerDrawUniform3D& setJointOffset(UnsignedInt offset) {
        jointOffset = offset;
        return *this;
    }

    /**
     * @brief Set the @ref perInstanceJointCount field
     * @return Reference to self (for method chaining)
     */
    MeshVisualizerDrawUniform3D& setPerInstanceJointCount(UnsignedInt count) {
        perInstanceJointCount = count;
        return *this;
    }

    /**
     * @}
     */

    /**
     * @brief Normal matrix
     *
     * Default value is an identity matrix. The bottom row is unused and acts
     * only as a padding to match uniform buffer packing rules.
     * @see @ref MeshVisualizerGL3D::setNormalMatrix()
     */
    Matrix3x4 normalMatrix;

    /** @var materialId
     * @brief Material ID
     *
     * References a particular material from a
     * @ref MeshVisualizerMaterialUniform array. Useful when an UBO with
     * more than one material is supplied or in a multi-draw scenario. Should
     * be less than the material count passed to @ref MeshVisualizerGL2D /
     * @ref MeshVisualizerGL3D constructor. Default value is @cpp 0 @ce,
     * meaning the first material gets used.
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
     * Unlike @ref materialId, this index is used only for the object ID
     * visualization, not to access any other uniform data. Default value
     * is @cpp 0 @ce.
     *
     * Used only if @ref MeshVisualizerGL3D::Flag::ObjectId is enabled, ignored
     * otherwise. If @ref MeshVisualizerGL3D::Flag::InstancedObjectId is
     * enabled as well, this value is added to the ID coming from the
     * @ref MeshVisualizerGL3D::ObjectId attribute.
     * @see @ref MeshVisualizerGL3D::setObjectId()
     */
    UnsignedInt objectId;

    /** @var jointOffset
     * @brief Joint offset
     *
     * Offset added to joint IDs in the @ref MeshVisualizerGL2D::JointIds and
     * @ref MeshVisualizerGL2D::SecondaryJointIds attributes. Useful when a UBO
     * with joint matrices for more than one skin is supplied or in a
     * multi-draw scenario. Should be less than the joint count passed to
     * @ref MeshVisualizerGL2D::Configuration::setJointCount(). Default value
     * is @cpp 0 @ce, meaning no offset is added to joint IDs.
     */

    /** @var perInstanceJointCount
     * @brief Per-instance joint count
     *
     * Offset added to joint IDs in the @ref MeshVisualizerGL2D::JointIds and
     * @ref MeshVisualizerGL2D::SecondaryJointIds atttributes in instanced
     * draws. Should be less than the joint count passed to
     * @ref MeshVisualizerGL2D::Configuration::setJointCount(). Default value
     * is @cpp 0 @ce, meaning every instance will use the same joint matrices,
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
@brief Material uniform for mesh visualizer shaders
@m_since_latest

Describes material properties referenced from
@ref MeshVisualizerDrawUniform2D::materialId and
@ref MeshVisualizerDrawUniform3D::materialId.
*/
struct MeshVisualizerMaterialUniform {
    /** @brief Construct with default parameters */
    constexpr explicit MeshVisualizerMaterialUniform(DefaultInitT = DefaultInit) noexcept: color{1.0f, 1.0f, 1.0f, 1.0f}, wireframeColor{0.0f, 0.0f, 0.0f, 1.0f}, wireframeWidth{1.0f}, colorMapOffset{1.0f/512.0f}, colorMapScale{1.0f/256.0f}, lineWidth{1.0f}, lineLength{1.0f}, smoothness{2.0f} {}

    /** @brief Construct without initializing the contents */
    explicit MeshVisualizerMaterialUniform(NoInitT) noexcept: color{NoInit}, wireframeColor{NoInit} {}

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
    MeshVisualizerMaterialUniform& setColor(const Color4& color) {
        this->color = color;
        return *this;
    }

    /**
     * @brief Set the @ref wireframeColor field
     * @return Reference to self (for method chaining)
     */
    MeshVisualizerMaterialUniform& setWireframeColor(const Color4& color) {
        wireframeColor = color;
        return *this;
    }

    /**
     * @brief Set the @ref wireframeWidth field
     * @return Reference to self (for method chaining)
     */
    MeshVisualizerMaterialUniform& setWireframeWidth(Float width) {
        wireframeWidth = width;
        return *this;
    }

    /**
     * @brief Set the @ref colorMapOffset and @ref colorMapScale fields
     * @return Reference to self (for method chaining)
     */
    MeshVisualizerMaterialUniform& setColorMapTransformation(Float offset, Float scale) {
        colorMapOffset = offset;
        colorMapScale = scale;
        return *this;
    }

    /**
     * @brief Set the @ref lineWidth field
     * @return Reference to self (for method chaining)
     */
    MeshVisualizerMaterialUniform& setLineWidth(Float width) {
        lineWidth = width;
        return *this;
    }

    /**
     * @brief Set the @ref lineLength field
     * @return Reference to self (for method chaining)
     */
    MeshVisualizerMaterialUniform& setLineLength(Float length) {
        lineLength = length;
        return *this;
    }

    /**
     * @brief Set the @ref smoothness field
     * @return Reference to self (for method chaining)
     */
    MeshVisualizerMaterialUniform& setSmoothness(Float smoothness) {
        this->smoothness = smoothness;
        return *this;
    }

    /**
     * @}
     */

    /**
     * @brief Base object color
     *
     * Default value is @cpp 0xffffffff_rgbaf @ce.
     *
     * Used only if @ref MeshVisualizerGL3D::Flag::Wireframe "MeshVisualizerGL*D::Flag::Wireframe"
     * or @relativeref{MeshVisualizerGL3D,Flag::InstancedObjectId} /
     * @relativeref{MeshVisualizerGL3D,Flag::PrimitiveId} /
     * @relativeref{MeshVisualizerGL3D,Flag::PrimitiveIdFromVertexId} is
     * enabled. In case of the latter, the color is multiplied with the color
     * map coming from @ref MeshVisualizerGL3D::bindColorMapTexture() "MeshVisualizerGL*D::bindColorMapTexture()".
     * @see @ref MeshVisualizerGL2D::setColor(),
     *      @ref MeshVisualizerGL3D::setColor()
     */
    Color4 color;

    /**
     * @brief Wireframe color
     *
     * Default value is @cpp 0x000000ff_rgbaf @ce.
     *
     * Used only if @ref MeshVisualizerGL3D::Flag::Wireframe "MeshVisualizerGL*D::Flag::Wireframe"
     * is enabled.
     * @see @ref MeshVisualizerGL2D::setWireframeColor(),
     *      @ref MeshVisualizerGL3D::setWireframeColor()
     */
    Color4 wireframeColor;

    /**
     * @brief Wireframe width
     *
     * The value is in screen space (depending on
     * @ref MeshVisualizerGL3D::setViewportSize() "MeshVisualizerGL*D::setViewportSize()"),
     * default value is @cpp 1.0f @ce.
     *
     * Used only if @ref MeshVisualizerGL3D::Flag::Wireframe "MeshVisualizerGL*D::Flag::Wireframe"
     * is enabled.
     * @see @ref MeshVisualizerGL2D::setWireframeWidth(),
     *      @ref MeshVisualizerGL3D::setWireframeWidth()
     */
    Float wireframeWidth;

    /**
     * @brief Color map offset
     *
     * Together with @ref colorMapScale forms an offset and scale applied to
     * the input value coming either from the
     * @ref MeshVisualizerGL3D::ObjectId "MeshVisualizerGL*D::ObjectId"
     * attribute or @glsl gl_PrimitiveID @ce, resulting value is then used to
     * fetch a color from a color map bound with @ref MeshVisualizerGL3D::bindColorMapTexture() "MeshVisualizerGL*D::bindColorMapTexture()".
     * Default offset and scale values are @cpp 1.0f/512.0f @ce and
     * @cpp 1.0/256.0f @ce, meaning that for a 256-entry colormap the first 256
     * values get an exact color from it and the next values will be either
     * clamped to last color or repeated depending on the color map texture
     * wrapping mode.
     *
     * Used only if @ref MeshVisualizerGL3D::Flag::InstancedObjectId "MeshVisualizerGL*D::Flag::InstancedObjectId" or
     * @relativeref{MeshVisualizerGL3D,Flag::PrimitiveId} /
     * @relativeref{MeshVisualizerGL3D,Flag::PrimitiveIdFromVertexId} is
     * enabled.
     * @see @ref MeshVisualizerGL2D::setColorMapTransformation(),
     *      @ref MeshVisualizerGL3D::setColorMapTransformation()
     */
    Float colorMapOffset;

    /**
     * @brief Color map offset
     *
     * See @ref colorMapOffset for more information.
     *
     * Used only by @ref MeshVisualizerGL3D and only if
     * @ref MeshVisualizerGL3D::Flag::InstancedObjectId "MeshVisualizerGL*D::Flag::InstancedObjectId" or
     * @relativeref{MeshVisualizerGL3D,Flag::PrimitiveId} /
     * @relativeref{MeshVisualizerGL3D,Flag::PrimitiveIdFromVertexId} is
     * enabled.
     * @see @ref MeshVisualizerGL2D::setColorMapTransformation(),
     *      @ref MeshVisualizerGL3D::setColorMapTransformation()
     */
    Float colorMapScale;

    /**
     * @brief Line width
     *
     * The value is in screen space (depending on
     * @ref MeshVisualizerGL3D::setViewportSize() "MeshVisualizerGL*D::setViewportSize()"),
     * default value is @cpp 1.0f @ce.
     *
     * Used only by @ref MeshVisualizerGL3D and only if
     * @ref MeshVisualizerGL3D::Flag::TangentDirection,
     * @relativeref{MeshVisualizerGL3D,Flag::BitangentFromTangentDirection},
     * @relativeref{MeshVisualizerGL3D,Flag::BitangentDirection} or
     * @relativeref{MeshVisualizerGL3D,Flag::NormalDirection} is enabled.
     * @see @ref MeshVisualizerGL3D::setLineWidth()
     */
    Float lineWidth;

    /**
     * @brief Line length
     *
     * The value is in object space, default value is @cpp 1.0f @ce.
     *
     * Used only by @ref MeshVisualizerGL3D and only if
     * @ref MeshVisualizerGL3D::Flag::TangentDirection,
     * @relativeref{MeshVisualizerGL3D,Flag::BitangentFromTangentDirection},
     * @relativeref{MeshVisualizerGL3D,Flag::BitangentDirection} or
     * @relativeref{MeshVisualizerGL3D,Flag::NormalDirection} is enabled.
     * @see @ref MeshVisualizerGL3D::setLineLength()
     */
    Float lineLength;

    /**
     * @brief Line smoothness
     *
     * The value is in screen space (depending on
     * @ref MeshVisualizerGL3D::setViewportSize() "MeshVisualizerGL*D::setViewportSize()"),
     * initial value is @cpp 2.0f @ce.
     *
     * Used only if @ref MeshVisualizerGL3D::Flag::Wireframe "MeshVisualizerGL*D::Flag::Wireframe",
     * @ref MeshVisualizerGL3D::Flag::TangentDirection,
     * @relativeref{MeshVisualizerGL3D,Flag::BitangentFromTangentDirection},
     * @relativeref{MeshVisualizerGL3D,Flag::BitangentDirection} or
     * @relativeref{MeshVisualizerGL3D,Flag::NormalDirection} is enabled.
     * @see @ref MeshVisualizerGL2D::setSmoothness(),
     *      @ref MeshVisualizerGL3D::setSmoothness()
     */
    Float smoothness;

    /* warning: Member __pad0__ is not documented. FFS DOXYGEN WHY DO YOU THINK
       I MADE THOSE UNNAMED, YOU DUMB FOOL */
    #ifndef DOXYGEN_GENERATING_OUTPUT
    Int:32;
    Int:32;
    #endif
};

#if defined(MAGNUM_TARGET_GL) && defined(MAGNUM_BUILD_DEPRECATED)
/** @brief @copybrief MeshVisualizerGL2D
 * @m_deprecated_since_latest Use @ref MeshVisualizerGL2D instead.
 */
typedef CORRADE_DEPRECATED("use MeshVisualizerGL2D instead") MeshVisualizerGL2D MeshVisualizer2D;

/** @brief @copybrief MeshVisualizerGL3D
 * @m_deprecated_since_latest Use @ref MeshVisualizerGL3D instead.
 */
typedef CORRADE_DEPRECATED("use MeshVisualizerGL3D instead") MeshVisualizerGL3D MeshVisualizer3D;

/** @brief @copybrief MeshVisualizerGL3D
 * @m_deprecated_since{2020,06} Use @ref MeshVisualizerGL3D instead.
 */
typedef CORRADE_DEPRECATED("use MeshVisualizerGL3D instead") MeshVisualizerGL3D MeshVisualizer;
#endif

}}

#endif
