#ifndef Magnum_Shaders_Phong_h
#define Magnum_Shaders_Phong_h
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
 * @brief Struct @ref Magnum::Shaders::PhongDrawUniform, @ref Magnum::Shaders::PhongMaterialUniform, @ref Magnum::Shaders::PhongLightUniform
 */

#include "Magnum/Magnum.h"
#include "Magnum/Tags.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Matrix.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Utility/Macros.h>

#include "Magnum/Shaders/PhongGL.h"
#endif

namespace Magnum { namespace Shaders {

/**
@brief Per-draw uniform for Phong shaders
@m_since_latest

Together with the generic @ref TransformationUniform3D contains parameters that
are specific to each draw call. Texture transformation, if needed, is supplied
separately in a @ref TextureTransformationUniform; material-related properties
are expected to be shared among multiple draw calls and thus are provided in a
separate @ref PhongMaterialUniform structure, referenced by @ref materialId.
@see @ref PhongGL::bindDrawBuffer()
*/
struct PhongDrawUniform {
    /** @brief Construct with default parameters */
    constexpr explicit PhongDrawUniform(DefaultInitT = DefaultInit) noexcept: normalMatrix{Math::IdentityInit},
        #if ((defined(CORRADE_TARGET_CLANG) && __clang_major__ < 4) || (defined(CORRADE_TARGET_APPLE_CLANG) && __clang_major__ < 8)) && defined(CORRADE_TARGET_BIG_ENDIAN)
        _pad0{}, /* Otherwise it refuses to constexpr, on 3.8 at least */
        #endif
        materialId{0},
        #if (defined(CORRADE_TARGET_CLANG) && __clang_major__ < 4) || (defined(CORRADE_TARGET_APPLE_CLANG) && __clang_major__ < 8) && !defined(CORRADE_TARGET_BIG_ENDIAN)
        _pad0{},
        #endif
        objectId{0}, lightOffset{0}, lightCount{0xffffffffu} {}

    /** @brief Construct without initializing the contents */
    explicit PhongDrawUniform(NoInitT) noexcept: normalMatrix{NoInit} {}

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
    PhongDrawUniform& setNormalMatrix(const Matrix3x3& matrix) {
        normalMatrix = Matrix3x4{matrix};
        return *this;
    }

    /**
     * @brief Set the @ref materialId field
     * @return Reference to self (for method chaining)
     */
    PhongDrawUniform& setMaterialId(UnsignedInt id) {
        materialId = id;
        return *this;
    }

    /**
     * @brief Set the @ref objectId field
     * @return Reference to self (for method chaining)
     */
    PhongDrawUniform& setObjectId(UnsignedInt id) {
        objectId = id;
        return *this;
    }

    /**
     * @brief Set the @ref lightOffset and @ref lightCount fields
     * @return Reference to self (for method chaining)
     */
    PhongDrawUniform& setLightOffsetCount(UnsignedInt offset, UnsignedInt count) {
        lightOffset = offset;
        lightCount = count;
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
     *
     * If @ref PhongGL::Flag::InstancedTransformation is enabled, the
     * per-instance normal matrix coming from the @ref PhongGL::NormalMatrix
     * attribute is applied first, before this one.
     * @see @ref PhongGL::setNormalMatrix()
     */
    Matrix3x4 normalMatrix;

    /** @var materialId
     * @brief Material ID
     *
     * References a particular material from a @ref PhongMaterialUniform array.
     * Useful when a UBO with more than one material is supplied or in a
     * multi-draw scenario. Should be less than the material count passed to
     * the @ref PhongGL::PhongGL(Flags, UnsignedInt, UnsignedInt, UnsignedInt)
     * constructor, if material count is @cpp 1 @ce, this field is assumed to
     * be @cpp 0 @ce and isn't even read by the shader. Default value is
     * @cpp 0 @ce, meaning the first material gets used.
     */

    /* This field is an UnsignedInt in the shader and materialId is extracted
       as (value & 0xffff), so the order has to be different on BE */
    #ifndef CORRADE_TARGET_BIG_ENDIAN
    UnsignedShort materialId;
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
    UnsignedShort
        #if (defined(CORRADE_TARGET_CLANG) && __clang_major__ < 4) || (defined(CORRADE_TARGET_APPLE_CLANG) && __clang_major__ < 8)
        _pad0 /* Otherwise it refuses to constexpr, on 3.8 at least */
        #endif
        :16; /* reserved for skinOffset */
    UnsignedShort materialId;
    #endif

    /**
     * @brief Object ID
     *
     * Unlike @ref materialId, this index is used only for the object ID
     * framebuffer output, not to access any other uniform data. Default value
     * is @cpp 0 @ce.
     *
     * Used only if @ref PhongGL::Flag::ObjectId is enabled, ignored otherwise.
     * If @ref PhongGL::Flag::InstancedObjectId is enabled as well, this value
     * is added to the ID coming from the @ref PhongGL::ObjectId attribute.
     * @see @ref PhongGL::setObjectId()
     */
    UnsignedInt objectId;

    /**
     * @brief Light offset
     *
     * References the first light in the @ref PhongLightUniform array. Should
     * be less than the light count passed to @ref PhongGL constructor. Default
     * value is @cpp 0 @ce.
     *
     * Used only if @ref PhongGL::Flag::LightCulling is enabled, otherwise
     * light offset is implicitly @cpp 0 @ce.
     */
    UnsignedInt lightOffset;

    /**
     * @brief Light count
     *
     * Specifies how many lights after the @p lightOffset are used from the
     * @ref PhongLightUniform array. Gets clamped by the shader so it's
     * together with @ref lightOffset not larger than the light count passed to
     * @ref PhongGL constructor. Default value is @cpp 0xffffffffu @ce.
     *
     * Used only if @ref PhongGL::Flag::LightCulling is enabled, otherwise
     * light count is implicitly @ref PhongGL::lightCount().
     */
    UnsignedInt lightCount;
};

/**
@brief Material uniform for Phong shaders
@m_since_latest

Describes material properties referenced from
@ref PhongDrawUniform::materialId.
@see @ref PhongGL::bindMaterialBuffer()
*/
struct PhongMaterialUniform {
    /** @brief Construct with default parameters */
    constexpr explicit PhongMaterialUniform(DefaultInitT = DefaultInit) noexcept: ambientColor{0.0f, 0.0f, 0.0f, 0.0f}, diffuseColor{1.0f, 1.0f, 1.0f, 1.0f}, specularColor{1.0f, 1.0f, 1.0f, 0.0f}, normalTextureScale{1.0f}, shininess{80.0f}, alphaMask{0.5f}
        #if (defined(CORRADE_TARGET_CLANG) && __clang_major__ < 4) || (defined(CORRADE_TARGET_APPLE_CLANG) && __clang_major__ < 8)
        , _pad0{} /* Otherwise it refuses to constexpr, on 3.8 at least */
        #endif
        {}

    /** @brief Construct without initializing the contents */
    explicit PhongMaterialUniform(NoInitT) noexcept: ambientColor{NoInit}, diffuseColor{NoInit}, specularColor{NoInit} {}

    /** @{
     * @name Convenience setters
     *
     * Provided to allow the use of method chaining for populating a structure
     * in a single expression, otherwise equivalent to accessing the fields
     * directly. Also guaranteed to provide backwards compatibility when
     * packing of the actual fields changes.
     */

    /**
     * @brief Set the @ref ambientColor field
     * @return Reference to self (for method chaining)
     */
    PhongMaterialUniform& setAmbientColor(const Color4& color) {
        ambientColor = color;
        return *this;
    }

    /**
     * @brief Set the @ref diffuseColor field
     * @return Reference to self (for method chaining)
     */
    PhongMaterialUniform& setDiffuseColor(const Color4& color) {
        diffuseColor = color;
        return *this;
    }

    /**
     * @brief Set the @ref specularColor field
     * @return Reference to self (for method chaining)
     */
    PhongMaterialUniform& setSpecularColor(const Color4& color) {
        specularColor = color;
        return *this;
    }

    /**
     * @brief Set the @ref normalTextureScale field
     * @return Reference to self (for method chaining)
     */
    PhongMaterialUniform& setNormalTextureScale(Float scale) {
        normalTextureScale = scale;
        return *this;
    }

    /**
     * @brief Set the @ref shininess field
     * @return Reference to self (for method chaining)
     */
    PhongMaterialUniform& setShininess(Float shininess) {
        this->shininess = shininess;
        return *this;
    }

    /**
     * @brief Set the @ref alphaMask field
     * @return Reference to self (for method chaining)
     */
    PhongMaterialUniform& setAlphaMask(Float alphaMask) {
        this->alphaMask = alphaMask;
        return *this;
    }

    /**
     * @}
     */

    /**
     * @brief Ambient color
     *
     * Default value is @cpp 0x00000000_rgbaf @ce. If
     * @ref PhongGL::Flag::AmbientTexture is enabled, be sure to set this field
     * to @cpp 0xffffffff_rgbaf @ce, otherwise the texture will be ignored.
     *
     * If @ref PhongGL::Flag::VertexColor is enabled, the color is multiplied
     * with a color coming from the @ref PhongGL::Color3 / @ref PhongGL::Color4
     * attribute.
     * @see @ref PhongGL::setAmbientColor()
     */
    Color4 ambientColor;

    /**
     * @brief Diffuse color
     *
     * Default value is @cpp 0xffffffff_rgbaf @ce.
     *
     * Used only if the effective light count for given draw is not zero,
     * ignored otherwise. If @ref PhongGL::Flag::VertexColor is enabled, the
     * color is multiplied with a color coming from the @ref PhongGL::Color3 /
     * @ref PhongGL::Color4 attribute.
     * @see @ref PhongGL::setDiffuseColor()
     */
    Color4 diffuseColor;

    /**
     * @brief Specular color
     *
     * Default value is @cpp 0xffffff00_rgbaf @ce.
     *
     * Used only if the effective light count for given draw is not zero and
     * @ref PhongGL::Flag::NoSpecular is not set, ignored otherwise.
     * @see @ref PhongGL::setSpecularColor()
     */
    Color4 specularColor;

    /**
     * @brief Normal texture scale
     *
     * Affects strength of the normal mapping. Default value is @cpp 1.0f @ce,
     * meaning the normal texture is not changed in any way; a value of
     * @cpp 0.0f @ce disables the normal texture effect altogether.
     *
     * Used only if @ref PhongGL::Flag::NormalTexture is enabled and the
     * effective light count for given draw is not zero, ignored otherwise.
     * @see @ref PhongGL::setNormalTextureScale()
     */
    Float normalTextureScale;

    /**
     * @brief Shininess
     *
     * The larger value, the harder surface (smaller specular highlight).
     * Default value is @cpp 80.0f @ce.
     *
     * Used only if the effective light count for given draw is not zero and
     * @ref PhongGL::Flag::NoSpecular is not set, ignored otherwise.
     * @see @ref PhongGL::setShininess()
     */
    Float shininess;

    /**
     * @brief Alpha mask value
     *
     * Fragments with alpha values smaller than the mask value will be
     * discarded. Default value is @cpp 0.5f @ce.
     *
     * Used only if @ref PhongGL::Flag::AlphaMask is enabled, ignored
     * otherwise.
     * @see @ref PhongGL::setAlphaMask()
     */
    Float alphaMask;

    /* warning: Member __pad0__ is not documented. FFS DOXYGEN WHY DO YOU THINK
       I MADE THOSE UNNAMED, YOU DUMB FOOL */
    #ifndef DOXYGEN_GENERATING_OUTPUT
    Int
        #if (defined(CORRADE_TARGET_CLANG) && __clang_major__ < 4) || (defined(CORRADE_TARGET_APPLE_CLANG) && __clang_major__ < 8)
        _pad0 /* Otherwise it refuses to constexpr, on 3.8 at least */
        #endif
        :32;
    #endif
};

/**
@brief Light parameters for Phong shaders
@m_since_latest

Describes light properties for each light used by the shader, either all
@ref PhongGL::lightCount() or the subrange referenced by the
@ref PhongDrawUniform::lightOffset and @ref PhongDrawUniform::lightCount range
if @ref PhongGL::Flag::LightCulling is enabled.
@see @ref PhongGL::bindLightBuffer()
*/
struct PhongLightUniform {
    /** @brief Construct with default parameters */
    constexpr explicit PhongLightUniform(DefaultInitT = DefaultInit) noexcept: position{0.0f, 0.0f, 1.0f, 0.0f}, color{1.0f, 1.0f, 1.0f},
        #if (defined(CORRADE_TARGET_CLANG) && __clang_major__ < 4) || (defined(CORRADE_TARGET_APPLE_CLANG) && __clang_major__ < 8)
        _pad0{}, /* Otherwise it refuses to constexpr, on 3.8 at least */
        #endif
        specularColor{1.0f, 1.0f, 1.0f},
        #if (defined(CORRADE_TARGET_CLANG) && __clang_major__ < 4) || (defined(CORRADE_TARGET_APPLE_CLANG) && __clang_major__ < 8)
        _pad1{},
        #endif
        range{Constants::inf()}
        #if (defined(CORRADE_TARGET_CLANG) && __clang_major__ < 4) || (defined(CORRADE_TARGET_APPLE_CLANG) && __clang_major__ < 8)
        , _pad2{}, _pad3{}, _pad4{}
        #endif
        {}
    /** @brief Construct without initializing the contents */
    explicit PhongLightUniform(NoInitT) noexcept: position{NoInit}, color{NoInit}, specularColor{NoInit} {}

    /** @{
     * @name Convenience setters
     *
     * Provided to allow the use of method chaining for populating a structure
     * in a single expression, otherwise equivalent to accessing the fields
     * directly. Also guaranteed to provide backwards compatibility when
     * packing of the actual fields changes.
     */

    /**
     * @brief Set the @ref position field
     * @return Reference to self (for method chaining)
     */
    PhongLightUniform& setPosition(const Vector4& position) {
        this->position = position;
        return *this;
    }

    /**
     * @brief Set the @ref color field
     * @return Reference to self (for method chaining)
     */
    PhongLightUniform& setColor(const Color3& color) {
        this->color = color;
        return *this;
    }

    /**
     * @brief Set the @ref specularColor field
     * @return Reference to self (for method chaining)
     */
    PhongLightUniform& setSpecularColor(const Color3& specularColor) {
        this->specularColor = specularColor;
        return *this;
    }

    /**
     * @brief Set the @ref range field
     * @return Reference to self (for method chaining)
     */
    PhongLightUniform& setRange(Float range) {
        this->range = range;
        return *this;
    }

    /**
     * @}
     */

    /**
     * @brief Position
     *
     * Depending on the fourth component, the value is treated as either a
     * camera-relative position of a point light, if the fourth component is
     * @cpp 1.0f @ce; or a direction *to* a directional light, if the fourth
     * component is @cpp 0.0f @ce. Default value is
     * @cpp {0.0f, 0.0f, 1.0f, 0.0f} @ce --- a directional "fill" light coming
     * from the camera.
     * @see @ref PhongGL::setLightPositions()
     */
    Vector4 position;

    /**
     * @brief Color
     *
     * Default value is @cpp 0xffffff_rgbf @ce.
     * @see @ref PhongGL::setLightColors()
     */
    Color3 color;

    /* warning: Member __pad0__ is not documented. FFS DOXYGEN WHY DO YOU THINK
       I MADE THOSE UNNAMED, YOU DUMB FOOL */
    #ifndef DOXYGEN_GENERATING_OUTPUT
    Int
        #if (defined(CORRADE_TARGET_CLANG) && __clang_major__ < 4) || (defined(CORRADE_TARGET_APPLE_CLANG) && __clang_major__ < 8)
        _pad0 /* Otherwise it refuses to constexpr, on 3.8 at least */
        #endif
        :32; /* reserved for cone inner angle */
    #endif

    /**
     * @brief Specular color
     *
     * Usually you'd set this value to the same as @ref color, but it allows
     * for greater flexibility such as disabling specular highlights on certain
     * lights. Default value is @cpp 0xffffff_rgbf @ce.
     * @see @ref PhongGL::setLightColors()
     */
    Color3 specularColor;

    /* warning: Member __pad1__ is not documented. FFS DOXYGEN WHY DO YOU THINK
       I MADE THOSE UNNAMED, YOU DUMB FOOL */
    #ifndef DOXYGEN_GENERATING_OUTPUT
    Int
        #if (defined(CORRADE_TARGET_CLANG) && __clang_major__ < 4) || (defined(CORRADE_TARGET_APPLE_CLANG) && __clang_major__ < 8)
        _pad1 /* Otherwise it refuses to constexpr, on 3.8 at least */
        #endif
        :32; /* reserved for cone outer angle */
    #endif

    /**
     * @brief Attenuation range
     *
     * Default value is @ref Constants::inf().
     * @see @ref PhongGL::setLightRanges()
     */
    Float range;

    /* warning: Member __pad2__ is not documented. FFS DOXYGEN WHY DO YOU THINK
       I MADE THOSE UNNAMED, YOU DUMB FOOL */
    #ifndef DOXYGEN_GENERATING_OUTPUT
    Int
        #if (defined(CORRADE_TARGET_CLANG) && __clang_major__ < 4) || (defined(CORRADE_TARGET_APPLE_CLANG) && __clang_major__ < 8)
        _pad2 /* Otherwise it refuses to constexpr, on 3.8 at least */
        #endif
        :32; /* reserved for cone direction */
    Int
        #if (defined(CORRADE_TARGET_CLANG) && __clang_major__ < 4) || (defined(CORRADE_TARGET_APPLE_CLANG) && __clang_major__ < 8)
        _pad3 /* Otherwise it refuses to constexpr, on 3.8 at least */
        #endif
        :32;
    Int
        #if (defined(CORRADE_TARGET_CLANG) && __clang_major__ < 4) || (defined(CORRADE_TARGET_APPLE_CLANG) && __clang_major__ < 8)
        _pad4 /* Otherwise it refuses to constexpr, on 3.8 at least */
        #endif
        :32;
    #endif
};

#ifdef MAGNUM_BUILD_DEPRECATED
/** @brief @copybrief PhongGL
 * @m_deprecated_since_latest Use @ref PhongGL instead.
 */
typedef CORRADE_DEPRECATED("use PhongGL instead") PhongGL Phong;
#endif

}}

#endif
