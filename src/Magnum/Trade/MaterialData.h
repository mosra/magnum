#ifndef Magnum_Trade_MaterialData_h
#define Magnum_Trade_MaterialData_h
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
 * @brief Class @ref Magnum::Trade::MaterialData, @ref Magnum::Trade::MaterialAttributeData, enum @ref Magnum::Trade::MaterialLayer, @ref Magnum::Trade::MaterialAttribute, @ref Magnum::Trade::MaterialTextureSwizzle, @ref Magnum::Trade::MaterialAttributeType
 * @m_since_latest
 */

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/EnumSet.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/StringView.h>
#include <Corrade/Utility/Endianness.h>

#include "Magnum/Magnum.h"
#include "Magnum/Math/RectangularMatrix.h"
#include "Magnum/Trade/Data.h"
#include "Magnum/Trade/Trade.h"
#include "Magnum/Trade/visibility.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Utility/Macros.h>
#endif

namespace Magnum { namespace Trade {

/**
@brief Material layer name
@m_since_latest

Convenience aliases to actual layer name strings. The alias is in the same form
and capitalization --- so for example @ref MaterialLayer::ClearCoat is an alias
for @cpp "ClearCoat" @ce. Each layer is expected to contain (a subset of) the
@ref MaterialAttribute::LayerName, @ref MaterialAttribute::LayerFactor,
@ref MaterialAttribute::LayerFactorTexture,
@ref MaterialAttribute::LayerFactorTextureSwizzle,
@ref MaterialAttribute::LayerFactorTextureMatrix,
@ref MaterialAttribute::LayerFactorTextureCoordinates attributes in addition to
what's specified for a particular named layer.
@see @ref MaterialData, @ref MaterialData::layerName(), @ref MaterialLayerData,
    @ref materialLayerName()
*/
enum class MaterialLayer: UnsignedInt {
    /* Zero used for an invalid value */

    /**
     * Clear coat material layer.
     *
     * Expected to contain (a subset of) the
     * @ref MaterialAttribute::Roughness,
     * @ref MaterialAttribute::RoughnessTexture,
     * @ref MaterialAttribute::RoughnessTextureSwizzle,
     * @ref MaterialAttribute::RoughnessTextureMatrix,
     * @ref MaterialAttribute::RoughnessTextureCoordinates,
     * @ref MaterialAttribute::NormalTexture,
     * @ref MaterialAttribute::NormalTextureSwizzle,
     * @ref MaterialAttribute::NormalTextureMatrix and
     * @ref MaterialAttribute::NormalTextureCoordinates attributes.
     * @see @ref PbrClearCoatMaterialData
     */
    ClearCoat = 1,
};

namespace Implementation {
    /* Compared to materialLayerName() below returns an empty string for
       invalid layers, used internally to provide better assertion messages */
    MAGNUM_TRADE_EXPORT Containers::StringView materialLayerNameInternal(MaterialLayer layer);
}

/**
@brief Material layer name as a string

Expects that @p layer is a valid @ref MaterialLayer value. The returned view
has both @relativeref{Corrade,Containers::StringViewFlag::Global} and
@relativeref{Corrade::Containers::StringViewFlag,NullTerminated} set.
*/
MAGNUM_TRADE_EXPORT Containers::StringView materialLayerName(MaterialLayer layer);

/**
@debugoperatorenum{MaterialLayer}
@m_since_latest
*/
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, MaterialLayer value);

/**
@brief Material attribute name
@m_since_latest

Convenience aliases to actual attribute name strings. In most cases the alias
is in the same form and capitalization --- so for example
@ref MaterialAttribute::DoubleSided is an alias for @cpp "DoubleSided" @ce, the
only exception is @ref MaterialAttribute::LayerName which is
@cpp " LayerName" @ce (with a space at the front).

When this enum is used in @ref MaterialAttributeData constructors, the data are
additionally checked for type compatibility. Other than that, there is no
difference to the string variants.

Each attribute value documents the default value that should be used if the
attribute isn't present. Some attributes, such as `*Texture`, have no defaults
--- in that case it means the material doesn't use given feature.
@see @ref MaterialAttributeData, @ref MaterialData,
    @ref materialAttributeName()
*/
enum class MaterialAttribute: UnsignedInt {
    /* Zero used for an invalid value */

    /**
     * Layer name, @ref MaterialAttributeType::String.
     *
     * Unlike other attributes where string name matches the enum name, in this
     * case the corresponding string is @cpp " LayerName" @ce (with a space at
     * the front), done in order to have the layer name attribute appear first
     * in each layer and thus simplify layer implementation.
     *
     * Default value is an empty string.
     * @see @ref MaterialData::layerName()
     */
    LayerName = 1,

    /**
     * Alpha mask, @ref MaterialAttributeType::Float.
     *
     * If set together with @ref MaterialAttribute::AlphaBlend, blending is
     * preferred, however renderers can fall back to alpha-masked rendering.
     * Alpha values below this value are meant to be rendered as fully
     * transparent and alpha values above this value as fully opaque.
     *
     * Default value is @cpp 0.5f @ce.
     * @see @ref MaterialAlphaMode, @ref MaterialData::alphaMode(),
     *      @ref MaterialData::alphaMask()
     */
    AlphaMask,

    /**
     * Alpha blending, @ref MaterialAttributeType::Bool.
     *
     * If @cpp true @ce, the material is expected to be rendered with blending
     * enabled and in correct depth order. If @cpp false @ce or not present,
     * the material should be treated as opaque. If set together with
     * @ref MaterialAttribute::AlphaMask, blending is preferred, however
     * renderers can fall back to alpha-masked rendering.
     *
     * Default value is @cpp false @ce.
     * @see @ref MaterialAlphaMode, @ref MaterialData::alphaMode()
     */
    AlphaBlend,

    /**
     * Double sided, @ref MaterialAttributeType::Bool.
     *
     * Default value is @cpp false @ce.
     * @see @ref MaterialData::isDoubleSided()
     */
    DoubleSided,

    /**
     * Ambient color for Phong materials, @ref MaterialAttributeType::Vector4.
     *
     * If @ref MaterialAttribute::AmbientTexture is present as well, these two
     * are multiplied together.
     *
     * Default value is @cpp 0x000000ff_srgbaf @ce if there's no
     * @ref MaterialAttribute::AmbientTexture and @cpp 0xffffffff_srgbaf @ce if
     * there is.
     * @see @ref PhongMaterialData::ambientColor()
     */
    AmbientColor,

    /**
     * Ambient texture index for Phong materials,
     * @ref MaterialAttributeType::UnsignedInt.
     *
     * If @ref MaterialAttribute::AmbientColor is present as well, these two
     * are multiplied together.
     * @see @ref PhongMaterialData::ambientTexture()
     */
    AmbientTexture,

    /**
     * Ambient texture transformation matrix for Phong materials,
     * @ref MaterialAttributeType::Matrix3x3.
     *
     * Has a precedence over @ref MaterialAttribute::TextureMatrix if both are
     * present.
     *
     * Default value is an identity matrix.
     * @see @ref PhongMaterialData::ambientTextureMatrix()
     */
    AmbientTextureMatrix,

    /**
     * Ambient texture coordinate set index for Phong materials,
     * @ref MaterialAttributeType::UnsignedInt.
     *
     * Has a precedence over @ref MaterialAttribute::TextureCoordinates if both
     * are present.
     *
     * Default value is @cpp 0u @ce.
     * @see @ref PhongMaterialData::ambientTextureCoordinates()
     */
    AmbientTextureCoordinates,

    /**
     * Ambient texture array layer for Phong materials,
     * @ref MaterialAttributeType::UnsignedInt.
     *
     * Has a precedence over @ref MaterialAttribute::TextureLayer if both are
     * present.
     *
     * Default value is @cpp 0u @ce.
     * @see @ref PhongMaterialData::ambientTextureLayer()
     */
    AmbientTextureLayer,

    /**
     * Diffuse color for Phong or PBR specular/glossiness materials,
     * @ref MaterialAttributeType::Vector4.
     *
     * If @ref MaterialAttribute::DiffuseTexture is present as well, these two
     * are multiplied together.
     *
     * Default value is @cpp 0xffffffff_srgbaf @ce.
     * @see @ref FlatMaterialData::color(),
     *      @ref PhongMaterialData::diffuseColor(),
     *      @ref PbrSpecularGlossinessMaterialData::diffuseColor()
     */
    DiffuseColor,

    /**
     * Diffuse texture index for Phong or PBR specular/glossiness materials,
     * @ref MaterialAttributeType::UnsignedInt.
     *
     * If @ref MaterialAttribute::DiffuseColor is present as well, these two
     * are multiplied together.
     * @see @ref FlatMaterialData::texture(),
     *      @ref PhongMaterialData::diffuseTexture(),
     *      @ref PbrSpecularGlossinessMaterialData::diffuseTexture()
     */
    DiffuseTexture,

    /**
     * Diffuse texture transformation matrix for Phong or PBR
     * specular/glossiness materials, @ref MaterialAttributeType::Matrix3x3.
     *
     * Has a precedence over @ref MaterialAttribute::TextureMatrix if both are
     * present.
     *
     * Default value is an identity matrix.
     * @see @ref FlatMaterialData::textureMatrix(),
     *      @ref PhongMaterialData::diffuseTextureMatrix(),
     *      @ref PbrSpecularGlossinessMaterialData::diffuseTextureMatrix()
     */
    DiffuseTextureMatrix,

    /**
     * Diffuse texture coordinate set index for Phong or PBR
     * specular/glossiness materials, @ref MaterialAttributeType::UnsignedInt.
     *
     * Has a precedence over @ref MaterialAttribute::TextureCoordinates if both
     * are present.
     *
     * Default value is @cpp 0u @ce.
     * @see @ref FlatMaterialData::textureCoordinates(),
     *      @ref PhongMaterialData::diffuseTextureCoordinates(),
     *      @ref PbrSpecularGlossinessMaterialData::diffuseTextureCoordinates()
     */
    DiffuseTextureCoordinates,

    /**
     * Diffuse texture array layer for Phong materials,
     * @ref MaterialAttributeType::UnsignedInt.
     *
     * Has a precedence over @ref MaterialAttribute::TextureLayer if both are
     * present.
     *
     * Default value is @cpp 0u @ce.
     * @see @ref PhongMaterialData::diffuseTextureLayer()
     */
    DiffuseTextureLayer,

    /**
     * Specular color for Phong or PBR specular/glossiness materials,
     * @ref MaterialAttributeType::Vector4. Alpha is commonly zero to not
     * interfere with alpha-masked objects, non-zero alpha can be for example
     * used to render transparent material which are still expected to have
     * specular highlights such as glass or soap bubbles.
     *
     * If @ref MaterialAttribute::SpecularTexture or
     * @ref MaterialAttribute::SpecularGlossinessTexture is present as well,
     * these two are multiplied together.
     *
     * Default value is @cpp 0xffffff00_srgbaf @ce.
     * @see @ref PhongMaterialData::specularColor(),
     *      @ref PbrSpecularGlossinessMaterialData::specularColor()
     */
    SpecularColor,

    /**
     * Specular texture index for Phong or PBR specular/glossiness materials,
     * @ref MaterialAttributeType::UnsignedInt.
     *
     * If @ref MaterialAttribute::SpecularColor is present as well, these two
     * are multiplied together. Can be alternatively supplied as a packed
     * @ref MaterialAttribute::SpecularGlossinessTexture.
     *
     * Default value is @cpp 0u @ce.
     * @see @ref PhongMaterialData::hasSpecularTexture(),
     *      @ref PhongMaterialData::specularTexture(),
     *      @ref PbrSpecularGlossinessMaterialData::hasSpecularTexture(),
     *      @ref PbrSpecularGlossinessMaterialData::hasSpecularGlossinessTexture(),
     *      @ref PbrSpecularGlossinessMaterialData::specularTexture()
     */
    SpecularTexture,

    /**
     * Specular texture swizzle for Phong or PBR specular/glossiness materials,
     * @ref MaterialAttributeType::TextureSwizzle.
     *
     * Can be used to describe whether the alpha channel of a
     * @ref MaterialAttribute::SpecularTexture is used or not. Either
     * @ref MaterialTextureSwizzle::RGBA or @ref MaterialTextureSwizzle::RGB
     * is expected. Does not apply to
     * @ref MaterialAttribute::SpecularGlossinessTexture --- in that case,
     * the specular texture is always three-channel, regardless of this
     * attribute.
     *
     * Default value is @ref MaterialTextureSwizzle::RGB.
     * @see @ref PbrSpecularGlossinessMaterialData::specularTextureSwizzle()
     */
    SpecularTextureSwizzle,

    /**
     * Specular texture transformation matrix for Phong or PBR
     * specular/glossiness materials, @ref MaterialAttributeType::Matrix3x3.
     *
     * Has a precedence over @ref MaterialAttribute::TextureMatrix if both are
     * present.
     *
     * Default value is an identity matrix.
     * @see @ref PhongMaterialData::specularTextureMatrix(),
     *      @ref PbrSpecularGlossinessMaterialData::glossinessTextureMatrix()
     */
    SpecularTextureMatrix,

    /**
     * Specular texture coordinate set index for Phong or PBR
     * specular/glossiness materials, @ref MaterialAttributeType::UnsignedInt.
     *
     * Has a precedence over @ref MaterialAttribute::TextureCoordinates if both
     * are present.
     *
     * Default value is @cpp 0u @ce.
     * @see @ref PhongMaterialData::specularTextureCoordinates(),
     *      @ref PbrSpecularGlossinessMaterialData::specularTextureCoordinates()
     */
    SpecularTextureCoordinates,

    /**
     * Specular texture array layer for Phong materials,
     * @ref MaterialAttributeType::UnsignedInt.
     *
     * Has a precedence over @ref MaterialAttribute::TextureLayer if both are
     * present.
     *
     * Default value is @cpp 0u @ce.
     * @see @ref PhongMaterialData::specularTextureLayer(),
     *      @ref PbrSpecularGlossinessMaterialData::specularTextureLayer()
     */
    SpecularTextureLayer,

    /**
     * Shininess value for Phong materials, @ref MaterialAttributeType::Float.
     *
     * No default value is specified for this attribute.
     * @see @ref PhongMaterialData::shininess()
     */
    Shininess,

    /**
     * Base color for PBR metallic/roughness materials,
     * @ref MaterialAttributeType::Vector4.
     *
     * If @ref MaterialAttribute::BaseColorTexture is present as well, these
     * two are multiplied together.
     *
     * Default value is @cpp 0xffffffff_srgbaf @ce.
     * @see @ref FlatMaterialData::color(),
     *      @ref PbrMetallicRoughnessMaterialData::baseColor()
     */
    BaseColor,

    /**
     * Base color texture index for PBR metallic/roughness materials,
     * @ref MaterialAttributeType::UnsignedInt.
     *
     * If @ref MaterialAttribute::BaseColor is present as well, these two are
     * multiplied together.
     * @see @ref FlatMaterialData::texture(),
     *      @ref PbrMetallicRoughnessMaterialData::baseColorTexture()
     */
    BaseColorTexture,

    /**
     * Base color texture transformation matrix for PBR metallic/roughness
     * materials, @ref MaterialAttributeType::Matrix3x3.
     *
     * Has a precedence over @ref MaterialAttribute::TextureMatrix if both are
     * present.
     *
     * Default value is an identity matrix.
     * @see @ref FlatMaterialData::textureMatrix(),
     *      @ref PbrMetallicRoughnessMaterialData::baseColorTextureMatrix()
     */
    BaseColorTextureMatrix,

    /**
     * Base color texture coordinate set index for PBR metallic/roughness
     * materials, @ref MaterialAttributeType::UnsignedInt.
     *
     * Has a precedence over @ref MaterialAttribute::TextureCoordinates if both
     * are present.
     *
     * Default value is @cpp 0u @ce.
     * @see @ref FlatMaterialData::textureCoordinates(),
     *      @ref PbrMetallicRoughnessMaterialData::baseColorTextureCoordinates()
     */
    BaseColorTextureCoordinates,

    /**
     * Base color texture array layer for PBR metallic/roughness materials,
     * @ref MaterialAttributeType::UnsignedInt.
     *
     * Has a precedence over @ref MaterialAttribute::TextureLayer if both are
     * present.
     *
     * Default value is @cpp 0u @ce.
     * @see @ref FlatMaterialData::textureLayer(),
     *      @ref PbrMetallicRoughnessMaterialData::baseColorTextureLayer()
     */
    BaseColorTextureLayer,

    /**
     * Metalness for PBR metallic/roughness materials,
     * @ref MaterialAttributeType::Float.
     *
     * If @ref MaterialAttribute::MetalnessTexture or
     * @ref MaterialAttribute::NoneRoughnessMetallicTexture is present as well,
     * these two are multiplied together.
     *
     * Default value is @cpp 1.0f @ce.
     * @see @ref PbrMetallicRoughnessMaterialData::metalness()
     */
    Metalness,

    /**
     * Metalness texture index for PBR metallic/roughness materials,
     * @ref MaterialAttributeType::UnsignedInt.
     *
     * If @ref MaterialAttribute::Metalness is present as well, these two are
     * multiplied together. Can be alternatively supplied as a packed
     * @ref MaterialAttribute::NoneRoughnessMetallicTexture.
     * @see @ref PbrMetallicRoughnessMaterialData::hasMetalnessTexture(),
     *      @ref PbrMetallicRoughnessMaterialData::hasNoneRoughnessMetallicTexture(),
     *      @ref PbrMetallicRoughnessMaterialData::hasOcclusionRoughnessMetallicTexture(),
     *      @ref PbrMetallicRoughnessMaterialData::hasNormalRoughnessMetallicTexture()
     *      @ref PbrMetallicRoughnessMaterialData::metalnessTexture()
     */
    MetalnessTexture,

    /**
     * Metalness texture swizzle for PBR metallic/roughness materials,
     * @ref MaterialAttributeType::TextureSwizzle.
     *
     * Can be used to express arbitrary packing of
     * @ref MaterialAttribute::MetalnessTexture together with other maps in a
     * single texture. A single-channel swizzle value is expected. Does not
     * apply to @ref MaterialAttribute::NoneRoughnessMetallicTexture --- in
     * that case, the metalness is implicitly in the red channel regardless of
     * this attribute.
     *
     * Default value is @ref MaterialTextureSwizzle::R.
     * @see @ref PbrMetallicRoughnessMaterialData::hasNoneRoughnessMetallicTexture(),
     *      @ref PbrMetallicRoughnessMaterialData::hasOcclusionRoughnessMetallicTexture(),
     *      @ref PbrMetallicRoughnessMaterialData::hasNormalRoughnessMetallicTexture()
     *      @ref PbrMetallicRoughnessMaterialData::metalnessTextureSwizzle()
     */
    MetalnessTextureSwizzle,

    /**
     * Metalness texture transformation matrix for PBR metallic/roughness
     * materials, @ref MaterialAttributeType::Matrix3x3.
     *
     * Has a precedence over @ref MaterialAttribute::TextureMatrix if both are
     * present.
     *
     * Default value is an identity matrix.
     * @see @ref PbrMetallicRoughnessMaterialData::metalnessTextureMatrix()
     */
    MetalnessTextureMatrix,

    /**
     * Metalness texture coordinate set index for PBR metallic/roughness
     * materials, @ref MaterialAttributeType::UnsignedInt.
     *
     * Has a precedence over @ref MaterialAttribute::TextureCoordinates if both
     * are present.
     *
     * Default value is @cpp 0u @ce.
     * @see @ref PbrMetallicRoughnessMaterialData::metalnessTextureCoordinates()
     */
    MetalnessTextureCoordinates,

    /**
     * Metalness texture array layer for PBR metallic/roughness materials,
     * @ref MaterialAttributeType::UnsignedInt.
     *
     * Has a precedence over @ref MaterialAttribute::TextureLayer if both are
     * present.
     *
     * Default value is @cpp 0u @ce.
     * @see @ref PbrMetallicRoughnessMaterialData::metalnessTextureLayer()
     */
    MetalnessTextureLayer,

    /**
     * Roughness for PBR metallic/roughness materials,
     * @ref MaterialAttributeType::Float.
     *
     * If @ref MaterialAttribute::RoughnessTexture or
     * @ref MaterialAttribute::NoneRoughnessMetallicTexture is present as well,
     * these two are multiplied together.
     *
     * Default value is @cpp 1.0f @ce.
     * @see @ref PbrMetallicRoughnessMaterialData::roughness()
     */
    Roughness,

    /**
     * Roughness texture index for PBR metallic/roughness materials,
     * @ref MaterialAttributeType::UnsignedInt.
     *
     * If @ref MaterialAttribute::Roughness is present as well, these two are
     * multiplied together. Can be alternatively supplied as a packed
     * @ref MaterialAttribute::NoneRoughnessMetallicTexture.
     * @see @ref PbrMetallicRoughnessMaterialData::hasRoughnessTexture(),
     *      @ref PbrMetallicRoughnessMaterialData::hasNoneRoughnessMetallicTexture(),
     *      @ref PbrMetallicRoughnessMaterialData::hasOcclusionRoughnessMetallicTexture(),
     *      @ref PbrMetallicRoughnessMaterialData::hasNormalRoughnessMetallicTexture()
     *      @ref PbrMetallicRoughnessMaterialData::roughnessTexture()
     */
    RoughnessTexture,

    /**
     * Roughness texture swizzle for PBR metallic/roughness materials,
     * @ref MaterialAttributeType::TextureSwizzle.
     *
     * Can be used to express arbitrary packing of
     * @ref MaterialAttribute::RoughnessTexture together with other maps in a
     * single texture. A single-channel swizzle value is expected. Does not
     * apply to @ref MaterialAttribute::NoneRoughnessMetallicTexture --- in
     * that case, the metalness is implicitly in the green channel regardless
     * of this attribute.
     *
     * Default value is @ref MaterialTextureSwizzle::R.
     * @see @ref PbrMetallicRoughnessMaterialData::hasNoneRoughnessMetallicTexture(),
     *      @ref PbrMetallicRoughnessMaterialData::hasOcclusionRoughnessMetallicTexture(),
     *      @ref PbrMetallicRoughnessMaterialData::hasNormalRoughnessMetallicTexture()
     *      @ref PbrMetallicRoughnessMaterialData::roughnessTextureSwizzle()
     */
    RoughnessTextureSwizzle,

    /**
     * Roughness texture transformation matrix for PBR metallic/roughness
     * materials, @ref MaterialAttributeType::Matrix3x3.
     *
     * Has a precedence over @ref MaterialAttribute::TextureMatrix if both are
     * present.
     *
     * Default value is an identity matrix.
     * @see @ref PbrMetallicRoughnessMaterialData::roughnessTextureMatrix()
     */
    RoughnessTextureMatrix,

    /**
     * Roughness texture coordinate set index for PBR metallic/roughness
     * materials, @ref MaterialAttributeType::UnsignedInt.
     *
     * Has a precedence over @ref MaterialAttribute::TextureCoordinates if both
     * are present.
     *
     * Default value is @cpp 0u @ce.
     * @see @ref PbrMetallicRoughnessMaterialData::roughnessTextureCoordinates()
     */
    RoughnessTextureCoordinates,

    /**
     * Roughness texture array layer for PBR metallic/roughness materials,
     * @ref MaterialAttributeType::UnsignedInt.
     *
     * Has a precedence over @ref MaterialAttribute::TextureLayer if both are
     * present.
     *
     * Default value is @cpp 0u @ce.
     * @see @ref PbrMetallicRoughnessMaterialData::roughnessTextureLayer()
     */
    RoughnessTextureLayer,

    /**
     * Combined roughness/metallic texture index for PBR metallic/roughness
     * materials with metalness in the blue channel and roughness in the green
     * channel, @ref MaterialAttributeType::UnsignedInt.
     *
     * If @ref MaterialAttribute::Metalness / @ref MaterialAttribute::Roughness
     * is present as well, these two are multiplied together.
     *
     * This is a convenience alias to simplify representation of glTF and UE4
     * materials, which is where this packing is used ([rationale](https://github.com/KhronosGroup/glTF/issues/857)).
     * This packing (and other variants) can be alternatively specified as a
     * pair of @ref MaterialAttribute::RoughnessTexture /
     * @ref MaterialAttribute::MetalnessTexture attributes together with
     * @ref MaterialAttribute::RoughnessTextureSwizzle set to
     * @ref MaterialTextureSwizzle::G
     * and @ref MaterialAttribute::MetalnessTextureSwizzle set to
     * @ref MaterialTextureSwizzle::B. Texture transformation and coordinate
     * set, if needed, have to be specified either using the global
     * @ref MaterialAttribute::TextureMatrix and
     * @ref MaterialAttribute::TextureCoordinates attributes or the per-texture
     * @ref MaterialAttribute::RoughnessTextureMatrix,
     * @ref MaterialAttribute::MetalnessTextureMatrix,
     * @ref MaterialAttribute::RoughnessTextureCoordinates and
     * @ref MaterialAttribute::MetalnessTextureCoordinates variants.
     * @see @ref PbrMetallicRoughnessMaterialData::hasNoneRoughnessMetallicTexture(),
     *      @ref PbrMetallicRoughnessMaterialData::hasOcclusionRoughnessMetallicTexture(),
     *      @ref PbrMetallicRoughnessMaterialData::hasNormalRoughnessMetallicTexture()
     *      @ref PbrMetallicRoughnessMaterialData::metalnessTexture(),
     *      @ref PbrMetallicRoughnessMaterialData::roughnessTexture()
     */
    NoneRoughnessMetallicTexture,

    /* DiffuseColor, DiffuseTexture, DiffuseTextureMatrix,
       DiffuseTextureCoordinates, DiffuseTextureLayer, SpecularColor,
       SpecularTexture, SpecularTextureSwizzle, SpecularTextureMatrix,
       SpecularTextureCoordinates, SpecularTextureLayer specified above for
       Phong already */

    /**
     * Glossiness for PBR specular/glossiness materials,
     * @ref MaterialAttributeType::Float.
     *
     * If @ref MaterialAttribute::GlossinessTexture or
     * @ref MaterialAttribute::SpecularGlossinessTexture is present as well,
     * these two are multiplied together.
     *
     * Default value is @cpp 1.0f @ce.
     * @see @ref PbrSpecularGlossinessMaterialData::glossiness()
     */
    Glossiness,

    /**
     * Glossiness texture index for PBR specular/glossiness materials,
     * @ref MaterialAttributeType::UnsignedInt.
     *
     * If @ref MaterialAttribute::Glossiness is present as well, these two are
     * multiplied together. Can be alternatively supplied as a packed
     * @ref MaterialAttribute::SpecularGlossinessTexture.
     * @see @ref PbrSpecularGlossinessMaterialData::hasGlossinessTexture(),
     *      @ref PbrSpecularGlossinessMaterialData::hasSpecularGlossinessTexture(),
     *      @ref PbrSpecularGlossinessMaterialData::glossinessTexture()
     */
    GlossinessTexture,

    /**
     * Glossiness texture swizzle for PBR specular/glossiness materials,
     * @ref MaterialAttributeType::TextureSwizzle.
     *
     * Can be used to express arbitrary packing of
     * @ref MaterialAttribute::GlossinessTexture together with other maps in a
     * single texture. A single-channel swizzle value is expected. Does not
     * apply to @ref MaterialAttribute::SpecularGlossinessTexture --- in that
     * case, the glossiness is implicitly in the alpha channel regardless of
     * this attribute.
     *
     * Default value is @ref MaterialTextureSwizzle::R.
     * @see @ref PbrSpecularGlossinessMaterialData::hasSpecularGlossinessTexture(),
     *      @ref PbrSpecularGlossinessMaterialData::glossinessTextureSwizzle()
     */
    GlossinessTextureSwizzle,

    /**
     * Glossiness texture transformation matrix for PBR specular/glossiness
     * materials, @ref MaterialAttributeType::Matrix3x3.
     *
     * Has a precedence over @ref MaterialAttribute::TextureMatrix if both are
     * present.
     *
     * Default value is an identity matrix.
     * @see @ref PbrSpecularGlossinessMaterialData::glossinessTextureMatrix()
     */
    GlossinessTextureMatrix,

    /**
     * Glossiness texture coordinate set index for PBR specular/glossiness
     * materials, @ref MaterialAttributeType::UnsignedInt.
     *
     * Has a precedence over @ref MaterialAttribute::TextureCoordinates if both
     * are present.
     *
     * Default value is @cpp 0u @ce.
     * @see @ref PbrSpecularGlossinessMaterialData::glossinessTextureCoordinates()
     */
    GlossinessTextureCoordinates,

    /**
     * Metalness texture array layer for PBR specular/glossiness materials,
     * @ref MaterialAttributeType::UnsignedInt.
     *
     * Has a precedence over @ref MaterialAttribute::TextureLayer if both are
     * present.
     *
     * Default value is @cpp 0u @ce.
     * @see @ref PbrSpecularGlossinessMaterialData::glossinessTextureLayer()
     */
    GlossinessTextureLayer,

    /**
     * Combined specular/glossiness texture index for PBR specular/glossiness
     * materials with specular color in the RGB channels and glossiness in
     * alpha, @ref MaterialAttributeType::UnsignedInt.
     *
     * If @ref MaterialAttribute::SpecularColor / @ref MaterialAttribute::Glossiness
     * is present as well, these two are multiplied together. Can be
     * alternatively specified as a pair of @ref MaterialAttribute::SpecularTexture
     * / @ref MaterialAttribute::GlossinessTexture attributes together with
     * @ref MaterialAttribute::GlossinessTextureSwizzle set to
     * @ref MaterialTextureSwizzle::A. Texture transformation and coordinate
     * set, if needed, have to be specified either using the global
     * @ref MaterialAttribute::TextureMatrix and
     * @ref MaterialAttribute::TextureCoordinates attributes or the per-texture
     * @ref MaterialAttribute::SpecularTextureMatrix,
     * @ref MaterialAttribute::GlossinessTextureMatrix,
     * @ref MaterialAttribute::SpecularTextureCoordinates and
     * @ref MaterialAttribute::GlossinessTextureCoordinates variants.
     * @see @ref PbrSpecularGlossinessMaterialData::hasSpecularGlossinessTexture(),
     *      @ref PbrSpecularGlossinessMaterialData::specularTexture(),
     *      @ref PbrSpecularGlossinessMaterialData::glossinessTexture()
     */
    SpecularGlossinessTexture,

    /**
     * Tangent-space normal map texture index,
     * @ref MaterialAttributeType::UnsignedInt.
     *
     * If @ref MaterialAttribute::NormalTextureScale is present as well, these
     * two are multiplied together, affecting strength of the effect. The scale
     * @f$ s @f$ multiplies the XY channels of a normal @f$ \boldsymbol{n}' @f$
     * fetched from (usually) an unsigned normal texture
     * @f$ \boldsymbol{t}_n @f$. The result is renormalized again after to form
     * the final normal @f$ \boldsymbol{n} @f$: @f[
     *      \begin{array}{rcl}
     *          \boldsymbol{n}' & = & (2 \boldsymbol{t}_n - \boldsymbol{1}) (s, s, 1)^T \\
     *          \boldsymbol{n}\phantom{'} & = & \frac{\boldsymbol{n}'}{|\boldsymbol{n}'|}
     *      \end{array}
     * @f]
     *
     * @see @ref PhongMaterialData::normalTexture(),
     *      @ref PbrMetallicRoughnessMaterialData::hasNormalRoughnessMetallicTexture(),
     *      @ref PbrMetallicRoughnessMaterialData::normalTexture(),
     *      @ref PbrSpecularGlossinessMaterialData::normalTexture()
     */
    NormalTexture,

    /**
     * Normal texture scale, @ref MaterialAttributeType::Float.
     *
     * Scales the texture defined by @ref MaterialAttribute::NormalTexture, see
     * above for details.
     *
     * Default value is @cpp 1.0f @ce.
     * @see @ref PhongMaterialData::normalTextureScale(),
     *      @ref PbrMetallicRoughnessMaterialData::normalTextureScale(),
     *      @ref PbrSpecularGlossinessMaterialData::normalTextureScale(),
     *      @ref PbrClearCoatMaterialData::normalTextureScale()
     */
    NormalTextureScale,

    /**
     * Normal texture swizzle, @ref MaterialAttributeType::TextureSwizzle.
     *
     * Can be used to express arbitrary packing together with other maps in a
     * single texture. A two- or three-channel swizzle value is expected.
     *
     * If the texture is just two-component, the remaining component is
     * implicit and calculated as @f$ z = \sqrt{1 - x^2 - y^2} @f$. In order to
     * account for numeric issues and avoid negative values under the square
     * root, it's commonly done as @f$ z = \sqrt{\max(0, 1 - x^2 - y^2)} @f$.
     * Additionally, to mitigate artifacts when storing normal texture in a
     * compressed format, @ref MaterialTextureSwizzle::GA may get used instead
     * of @ref MaterialTextureSwizzle::RG.
     *
     * Shader code that is able to reconstruct a XYZ normal from both RG and GA
     * variants assuming constant values in other channels ([source](https://github.com/KhronosGroup/glTF/issues/1682#issuecomment-557880407)):
     *
     * @snippet Trade.glsl unpackTwoChannelNormal
     *
     * Default value is @ref MaterialTextureSwizzle::RGB.
     * @see @ref PbrMetallicRoughnessMaterialData::hasNormalRoughnessMetallicTexture(),
     *      @ref PbrMetallicRoughnessMaterialData::normalTextureSwizzle(),
     *      @ref PbrSpecularGlossinessMaterialData::normalTextureSwizzle(),
     *      @ref materialTextureSwizzleComponentCount()
     */
    NormalTextureSwizzle,

    /**
     * Normal texture transformation matrix,
     * @ref MaterialAttributeType::Matrix3x3.
     *
     * Has a precedence over @ref MaterialAttribute::TextureMatrix if both are
     * present.
     *
     * Default value is an identity matrix.
     * @see @ref PhongMaterialData::normalTextureMatrix(),
     *      @ref PbrMetallicRoughnessMaterialData::normalTextureMatrix(),
     *      @ref PbrSpecularGlossinessMaterialData::normalTextureMatrix()
     */
    NormalTextureMatrix,

    /**
     * Normal texture coordinate set index,
     * @ref MaterialAttributeType::UnsignedInt.
     *
     * Has a precedence over @ref MaterialAttribute::TextureCoordinates if both
     * are present.
     *
     * Default value is @cpp 0u @ce.
     * @see @ref PhongMaterialData::normalTextureCoordinates(),
     *      @ref PbrMetallicRoughnessMaterialData::normalTextureCoordinates(),
     *      @ref PbrSpecularGlossinessMaterialData::normalTextureCoordinates()
     */
    NormalTextureCoordinates,

    /**
     * Normal texture array layer, @ref MaterialAttributeType::UnsignedInt.
     *
     * Has a precedence over @ref MaterialAttribute::TextureLayer if both are
     * present.
     *
     * Default value is @cpp 0u @ce.
     * @see @ref PhongMaterialData::normalTextureLayer(),
     *      @ref PbrMetallicRoughnessMaterialData::normalTextureLayer(),
     *      @ref PbrSpecularGlossinessMaterialData::normalTextureLayer()
     */
    NormalTextureLayer,

    /**
     * Occlusion texture index,
     * @ref MaterialAttributeType::UnsignedInt.
     *
     * Single-channel texture that multiplies the resulting material color
     * @f$ \boldsymbol{c} @f$: @f[
     *      \boldsymbol{c}_o = o \boldsymbol{c}
     * @f]
     *
     * If @ref MaterialAttribute::OcclusionTextureStrength is present as well,
     * it's used as an interpolation factor @f$ \color{m-success} s @f$ between
     * material color @f$ \boldsymbol{c} @f$ and color with occlusion applied
     * @f$ o \boldsymbol{c} @f$: @f[
     *      \boldsymbol{c}_o = \operatorname{lerp}(\boldsymbol{c}, o \boldsymbol{c}, {\color{m-success} s}) =
     *      \boldsymbol{c} (1 - {\color{m-success} s}) + o \boldsymbol{c} {\color{m-success} s}
     * @f]
     * @see @ref PbrMetallicRoughnessMaterialData::hasOcclusionRoughnessMetallicTexture(),
     *      @ref PbrMetallicRoughnessMaterialData::occlusionTexture(),
     *      @ref PbrSpecularGlossinessMaterialData::occlusionTexture(),
     *      @ref Math::lerp()
     */
    OcclusionTexture,

    /**
     * Occlusion texture strength, @ref MaterialAttributeType::Float.
     *
     * Affects the texture defined by @ref MaterialAttribute::OcclusionTexture,
     * see above for details.
     *
     * Default value is @cpp 1.0f @ce.
     * @see @ref PbrMetallicRoughnessMaterialData::occlusionTextureStrength(),
     *      @ref PbrSpecularGlossinessMaterialData::occlusionTextureStrength()
     */
    OcclusionTextureStrength,

    /**
     * Occlusion texture swizzle, @ref MaterialAttributeType::TextureSwizzle.
     *
     * Can be used to express arbitrary packing together with other maps in a
     * single texture. A single-channel swizzle value is expected.
     *
     * Default value is @ref MaterialTextureSwizzle::R.
     * @see @ref PbrMetallicRoughnessMaterialData::hasOcclusionRoughnessMetallicTexture(),
     *      @ref PbrMetallicRoughnessMaterialData::occlusionTextureSwizzle(),
     *      @ref PbrSpecularGlossinessMaterialData::occlusionTextureSwizzle()
     */
    OcclusionTextureSwizzle,

    /**
     * Occlusion texture transformation matrix,
     * @ref MaterialAttributeType::Matrix3x3.
     *
     * Has a precedence over @ref MaterialAttribute::TextureMatrix if both are
     * present.
     *
     * Default value is an identity matrix.
     * @see @ref PbrMetallicRoughnessMaterialData::occlusionTextureMatrix(),
     *      @ref PbrSpecularGlossinessMaterialData::occlusionTextureSwizzle()
     */
    OcclusionTextureMatrix,

    /**
     * Occlusion texture coordinate set index,
     * @ref MaterialAttributeType::UnsignedInt.
     *
     * Has a precedence over @ref MaterialAttribute::TextureCoordinates if both
     * are present.
     *
     * Default value is @cpp 0u @ce.
     * @see @ref PbrMetallicRoughnessMaterialData::occlusionTextureCoordinates(),
     *      @ref PbrSpecularGlossinessMaterialData::occlusionTextureCoordinates()
     */
    OcclusionTextureCoordinates,

    /**
     * Occlusion texture array layer, @ref MaterialAttributeType::UnsignedInt.
     *
     * Has a precedence over @ref MaterialAttribute::TextureLayer if both are
     * present.
     *
     * Default value is @cpp 0u @ce.
     * @see @ref PbrMetallicRoughnessMaterialData::occlusionTextureLayer(),
     *      @ref PbrSpecularGlossinessMaterialData::occlusionTextureLayer()
     */
    OcclusionTextureLayer,

    /**
     * Emissive color,
     * @ref MaterialAttributeType::Vector3.
     *
     * If @ref MaterialAttribute::EmissiveTexture is present as well, these two
     * are multiplied together.
     *
     * Default value is @cpp 0x000000_srgbf @ce.
     * @see @ref PbrMetallicRoughnessMaterialData::emissiveColor(),
     *      @ref PbrSpecularGlossinessMaterialData::emissiveColor()
     */
    EmissiveColor,

    /**
     * Emissive texture index,
     * @ref MaterialAttributeType::UnsignedInt.
     *
     * If @ref MaterialAttribute::EmissiveColor is present as well, these two
     * are multiplied together.
     * @see @ref PbrMetallicRoughnessMaterialData::emissiveTexture(),
     *      @ref PbrSpecularGlossinessMaterialData::emissiveTexture()
     */
    EmissiveTexture,

    /** @todo EmissiveTextureSwizzle? It's a color and I'm not aware of any
        existing packing schemes, so probably safe to assume it's always RGB */

    /**
     * Emissive texture transformation matrix,
     * @ref MaterialAttributeType::Matrix3x3.
     *
     * Has a precedence over @ref MaterialAttribute::TextureMatrix if both are
     * present.
     *
     * Default value is an identity matrix.
     * @see @ref PbrMetallicRoughnessMaterialData::emissiveTextureMatrix(),
     *      @ref PbrSpecularGlossinessMaterialData::emissiveTextureMatrix(),
     */
    EmissiveTextureMatrix,

    /**
     * Emissive texture coordinate set index,
     * @ref MaterialAttributeType::UnsignedInt.
     *
     * Has a precedence over @ref MaterialAttribute::TextureCoordinates if both
     * are present.
     *
     * Default value is @cpp 0u @ce.
     * @see @ref PbrMetallicRoughnessMaterialData::emissiveTextureCoordinates(),
     *      @ref PbrSpecularGlossinessMaterialData::emissiveTextureCoordinates()
     */
    EmissiveTextureCoordinates,

    /**
     * Emissive texture array layer, @ref MaterialAttributeType::UnsignedInt.
     *
     * Has a precedence over @ref MaterialAttribute::TextureLayer if both are
     * present.
     *
     * Default value is @cpp 0u @ce.
     * @see @ref PbrMetallicRoughnessMaterialData::emissiveTextureLayer(),
     *      @ref PbrSpecularGlossinessMaterialData::emissiveTextureLayer()
     */
    EmissiveTextureLayer,

    /**
     * Layer intensity. @ref MaterialAttributeType::Float.
     *
     * Expected to be contained in additional layers, not the base material.
     * The exact semantic of this attribute is layer-specific. If
     * @ref MaterialAttribute::LayerFactorTexture is present as well, these two
     * are multiplied together.
     *
     * Default value is @cpp 1.0f @ce.
     * @see @ref MaterialData::layerFactor()
     */
    LayerFactor,

    /**
     * Layer intensity texture, @ref MaterialAttributeType::UnsignedInt.
     *
     * Expected to be contained in additional layers, not the base material.
     * The exact semantic of this attribute is layer-specific. If
     * @ref MaterialAttribute::LayerFactor is present as well, these two are
     * multiplied together.
     * @see @ref MaterialData::layerFactorTexture()
     */
    LayerFactorTexture,

    /**
     * Layer intensity texture swizzle, @ref MaterialAttributeType::TextureSwizzle.
     *
     * Can be used to express arbitrary packing together with other maps in a
     * single texture. A single-channel swizzle value is expected.
     *
     * Default value is @ref MaterialTextureSwizzle::R.
     * @see @ref MaterialData::layerFactorTextureSwizzle()
     */
    LayerFactorTextureSwizzle,

    /**
     * Layer intensity texture transformation matrix,
     * @ref MaterialAttributeType::Matrix3x3.
     *
     * Expected to be contained in additional layers, not the base material.
     * Has a precedence over @ref MaterialAttribute::TextureMatrix if both are
     * present.
     *
     * Default value is an identity matrix.
     * @see @ref MaterialData::layerFactorTextureMatrix()
     */
    LayerFactorTextureMatrix,

    /**
     * Layer intensity texture coordinate set index,
     * @ref MaterialAttributeType::UnsignedInt.
     *
     * Expected to be contained in additional layers, not the base material.
     * Has a precedence over @ref MaterialAttribute::TextureCoordinates if both
     * are present.
     *
     * Default value is @cpp 0u @ce.
     * @see @ref MaterialData::layerFactorTextureCoordinates()
     */
    LayerFactorTextureCoordinates,

    /**
     * Layer intensity texture array layer,
     * @ref MaterialAttributeType::UnsignedInt.
     *
     * Expected to be contained in additional layers, not the base material.
     * Has a precedence over @ref MaterialAttribute::TextureLayer if both are
     * present.
     *
     * Default value is @cpp 0u @ce.
     * @see @ref MaterialData::layerFactorTextureLayer()
     */
    LayerFactorTextureLayer,

    /**
     * Common texture transformation matrix for all textures,
     * @ref MaterialAttributeType::Matrix3x3.
     *
     * @ref MaterialAttribute::AmbientTextureMatrix /
     * @ref MaterialAttribute::DiffuseTextureMatrix /
     * @ref MaterialAttribute::SpecularTextureMatrix /
     * @ref MaterialAttribute::MetalnessTextureMatrix /
     * @ref MaterialAttribute::RoughnessTextureMatrix /
     * @ref MaterialAttribute::GlossinessTextureMatrix /
     * @ref MaterialAttribute::NormalTextureMatrix /
     * @ref MaterialAttribute::OcclusionTextureMatrix /
     * @ref MaterialAttribute::EmissiveTextureMatrix /
     * @ref MaterialAttribute::LayerFactorTextureMatrix have a precedence over
     * this attribute for given texture, if present.
     *
     * Default value is an identity matrix.
     * @see @ref PhongMaterialData::hasCommonTextureTransformation(),
     *      @ref PbrMetallicRoughnessMaterialData::hasCommonTextureTransformation(),
     *      @ref PbrSpecularGlossinessMaterialData::hasCommonTextureTransformation(),
     *      @ref PbrClearCoatMaterialData::hasCommonTextureTransformation(),
     *      @ref PhongMaterialData::commonTextureMatrix(),
     *      @ref PbrMetallicRoughnessMaterialData::commonTextureMatrix(),
     *      @ref PbrSpecularGlossinessMaterialData::commonTextureMatrix(),
     *      @ref PbrClearCoatMaterialData::commonTextureMatrix(),
     *      @ref FlatMaterialData::textureMatrix()
     */
    TextureMatrix,

    /**
     * Common texture coordinate set index for all textures,
     * @ref MaterialAttributeType::UnsignedInt.
     *
     * @ref MaterialAttribute::AmbientTextureCoordinates /
     * @ref MaterialAttribute::DiffuseTextureCoordinates /
     * @ref MaterialAttribute::SpecularTextureCoordinates /
     * @ref MaterialAttribute::MetalnessTextureCoordinates /
     * @ref MaterialAttribute::RoughnessTextureCoordinates /
     * @ref MaterialAttribute::GlossinessTextureCoordinates /
     * @ref MaterialAttribute::NormalTextureCoordinates /
     * @ref MaterialAttribute::OcclusionTextureCoordinates /
     * @ref MaterialAttribute::EmissiveTextureCoordinates /
     * @ref MaterialAttribute::LayerFactorTextureCoordinates have a precedence
     * over this attribute for given texture, if present.
     *
     * Default value is @cpp 0u @ce.
     * @see @ref PhongMaterialData::hasCommonTextureCoordinates(),
     *      @ref PbrMetallicRoughnessMaterialData::hasCommonTextureCoordinates(),
     *      @ref PbrSpecularGlossinessMaterialData::hasCommonTextureCoordinates(),
     *      @ref PbrClearCoatMaterialData::hasCommonTextureCoordinates(),
     *      @ref PhongMaterialData::commonTextureCoordinates(),
     *      @ref PbrMetallicRoughnessMaterialData::commonTextureCoordinates(),
     *      @ref PbrSpecularGlossinessMaterialData::commonTextureCoordinates(),
     *      @ref PbrClearCoatMaterialData::commonTextureCoordinates(),
     *      @ref FlatMaterialData::textureCoordinates()
     */
    TextureCoordinates,

    /**
     * Common texture array layer for all textures,
     * @ref MaterialAttributeType::UnsignedInt.
     *
     * @ref MaterialAttribute::AmbientTextureLayer /
     * @ref MaterialAttribute::DiffuseTextureLayer /
     * @ref MaterialAttribute::SpecularTextureLayer /
     * @ref MaterialAttribute::MetalnessTextureLayer /
     * @ref MaterialAttribute::RoughnessTextureLayer /
     * @ref MaterialAttribute::GlossinessTextureLayer /
     * @ref MaterialAttribute::NormalTextureLayer /
     * @ref MaterialAttribute::OcclusionTextureLayer /
     * @ref MaterialAttribute::EmissiveTextureLayer /
     * @ref MaterialAttribute::LayerFactorTextureLayer have a precedence over
     * this attribute for given texture, if present.
     *
     * Default value is @cpp 0u @ce.
     * @see @ref PhongMaterialData::hasCommonTextureLayer(),
     *      @ref PbrMetallicRoughnessMaterialData::hasCommonTextureLayer(),
     *      @ref PbrSpecularGlossinessMaterialData::hasCommonTextureLayer(),
     *      @ref PbrClearCoatMaterialData::hasCommonTextureLayer(),
     *      @ref PhongMaterialData::commonTextureLayer(),
     *      @ref PbrMetallicRoughnessMaterialData::commonTextureLayer(),
     *      @ref PbrSpecularGlossinessMaterialData::commonTextureLayer(),
     *      @ref PbrClearCoatMaterialData::commonTextureLayer(),
     *      @ref FlatMaterialData::textureLayer()
     */
    TextureLayer,
};

namespace Implementation {
    /* Compared to materialLayerName() below returns an empty string for
       invalid layers, used internally to provide better assertion messages */
    MAGNUM_TRADE_EXPORT Containers::StringView materialAttributeNameInternal(MaterialAttribute attribute);
}

/**
@brief Material layer name as a string
@m_since_latest

Expects that @p attribute is a valid @ref MaterialAttribute value. The returned
view has both @relativeref{Corrade,Containers::StringViewFlag::Global} and
@relativeref{Corrade::Containers::StringViewFlag,NullTerminated} set.
*/
MAGNUM_TRADE_EXPORT Containers::StringView materialAttributeName(MaterialAttribute attribute);

/**
@debugoperatorenum{MaterialAttribute}
@m_since_latest
*/
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, MaterialAttribute value);

/**
@brief Material texture swizzle
@m_since_latest

See @ref MaterialData for more information.
@see @ref materialTextureSwizzleComponentCount()
*/
enum class MaterialTextureSwizzle: UnsignedInt {
    /** Red component */
    R = Utility::Endianness::fourCC('R', '\0', '\0', '\0'),

    /** Green component */
    G = Utility::Endianness::fourCC('G', '\0', '\0', '\0'),

    /** Blue component */
    B = Utility::Endianness::fourCC('B', '\0', '\0', '\0'),

    /** Alpha component */
    A = Utility::Endianness::fourCC('A', '\0', '\0', '\0'),

    /** Red and green component */
    RG = Utility::Endianness::fourCC('R', 'G', '\0', '\0'),

    /** Green and blue component */
    GB = Utility::Endianness::fourCC('G', 'B', '\0', '\0'),

    /**
     * Green and alpha component. May get used to mitigate artifacts when
     * storing two independent channels (such as two-channel normal maps) in
     * compressed texture formats --- these commonly have separately compressed
     * RGB and alpha, with green channel having the most precision of the RGB
     * triplet.
     * @see @ref MaterialAttribute::NormalTextureSwizzle
     */
    GA = Utility::Endianness::fourCC('G', 'A', '\0', '\0'),

    /** Blue and alpha component */
    BA = Utility::Endianness::fourCC('B', 'A', '\0', '\0'),

    /** RGB components */
    RGB = Utility::Endianness::fourCC('R', 'G', 'B', '\0'),

    /** GBA components */
    GBA = Utility::Endianness::fourCC('G', 'B', 'A', '\0'),

    /** RGBA components */
    RGBA = Utility::Endianness::fourCC('R', 'G', 'B', 'A'),
};

/**
@brief Component count in a material texture swizzle
@m_since_latest

Returns for example @cpp 2 @ce for @ref MaterialTextureSwizzle::GA.
*/
MAGNUM_TRADE_EXPORT UnsignedInt materialTextureSwizzleComponentCount(MaterialTextureSwizzle swizzle);

/**
@debugoperatorenum{MaterialTextureSwizzle}
@m_since_latest
*/
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, MaterialTextureSwizzle value);

/**
@brief Material attribute type
@m_since_latest

See @ref MaterialData for more information.
@see @ref MaterialAttribute, @ref MaterialAttributeData,
    @ref materialAttributeTypeSize()
*/
enum class MaterialAttributeType: UnsignedByte {
    /* Zero used for an invalid value */

    Bool = 1,       /**< @cpp bool @ce <b></b> */

    Float,          /**< @ref Magnum::Float "Float" */
    Deg,            /**< @ref Magnum::Deg "Deg" */
    Rad,            /**< @ref Magnum::Rad "Rad" */
    UnsignedInt,    /**< @ref Magnum::UnsignedInt "UnsignedInt" */
    Int,            /**< @ref Magnum::Int "Int" */
    UnsignedLong,   /**< @ref Magnum::UnsignedLong "UnsignedLong" */
    Long,           /**< @ref Magnum::Long "Long" */

    Vector2,        /**< @ref Magnum::Vector2 "Vector2" */
    Vector2ui,      /**< @ref Magnum::Vector2ui "Vector2ui" */
    Vector2i,       /**< @ref Magnum::Vector2i "Vector2i" */

    Vector3,        /**< @ref Magnum::Vector3 "Vector3" */
    Vector3ui,      /**< @ref Magnum::Vector3ui "Vector3ui" */
    Vector3i,       /**< @ref Magnum::Vector3i "Vector3i" */

    Vector4,        /**< @ref Magnum::Vector4 "Vector4" */
    Vector4ui,      /**< @ref Magnum::Vector4ui "Vector4ui" */
    Vector4i,       /**< @ref Magnum::Vector4i "Vector4i" */

    Matrix2x2,      /**< @ref Magnum::Matrix2x2 "Matrix2x2" */
    Matrix2x3,      /**< @ref Magnum::Matrix2x3 "Matrix2x3" */
    Matrix2x4,      /**< @ref Magnum::Matrix2x4 "Matrix2x4" */

    Matrix3x2,      /**< @ref Magnum::Matrix3x2 "Matrix3x2" */
    Matrix3x3,      /**< @ref Magnum::Matrix3x3 "Matrix3x3" */
    Matrix3x4,      /**< @ref Magnum::Matrix3x4 "Matrix3x4" */

    Matrix4x2,      /**< @ref Magnum::Matrix4x2 "Matrix4x2" */
    Matrix4x3,      /**< @ref Magnum::Matrix4x3 "Matrix4x3" */

    /* Matrix4x4 not present as it won't fit (a deliberate decision), see
       MaterialData docs for more information */

    /**
     * @cpp const void* @ce, type is not preserved. For convenience it's
     * possible to retrieve the value by calling @cpp attribute<const T*>() @ce
     * with an arbitrary `T` but the user has to ensure the type is correct.
     */
    Pointer,

    /**
     * @cpp void* @ce, type is not preserved. For convenience it's possible to
     * retrieve the value by calling @cpp attribute<T*>() @ce with an arbitrary
     * `T` but the user has to ensure the type is correct.
     */
    MutablePointer,

    /**
     * Null-terminated string. Can be stored using any type convertible to
     * @relativeref{Corrade,Containers::StringView}, retrieval has to be done
     * using @relativeref{Corrade,Containers::StringView}.
     */
    String,

    /**
     * Opaque data. Can be stored using any type convertible to
     * @relativeref{Corrade,Containers::ArrayView}, retrieval has to be done
     * using @ref Corrade::Containers::ArrayView "Containers::ArrayView<const void>".
     */
    Buffer,

    /** One of the values from @ref MaterialTextureSwizzle */
    TextureSwizzle
};

/**
@brief Byte size of a material attribute type
@m_since_latest

Can't be used with @ref MaterialAttributeType::String, as the size varies
depending on the value.
*/
MAGNUM_TRADE_EXPORT std::size_t materialAttributeTypeSize(MaterialAttributeType type);

/**
@debugoperatorenum{MaterialAttributeType}
@m_since_latest
*/
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, MaterialAttributeType value);

namespace Implementation {
    template<class> struct MaterialAttributeTypeFor;
    enum: std::size_t { MaterialAttributeDataSize = 64 };
}

/**
@brief Material attribute data
@m_since_latest

See @ref MaterialData for more information.
*/
class MAGNUM_TRADE_EXPORT MaterialAttributeData {
    public:
        /**
         * @brief Default constructor
         *
         * Leaves contents at unspecified values. Provided as a convenience for
         * initialization of the attribute array for @ref MaterialData,
         * expected to be replaced with concrete values later.
         */
        constexpr explicit MaterialAttributeData() noexcept: _data{} {}

        /**
         * @brief Construct with a string name
         * @param name      Attribute name
         * @param value     Attribute value
         *
         * The @p name is expected to be non-empty and together with @p value
         * is expected to fit into 62 bytes. @ref MaterialAttributeType is
         * inferred from the type passed.
         *
         * This function is useful in @cpp constexpr @ce contexts and for
         * creating custom material attributes. For known attributes prefer to
         * use @ref MaterialAttributeData(MaterialAttribute, const T&) if you
         * don't need @cpp constexpr @ce, as it additionally checks that given
         * attribute has the expected type.
         */
        template<class T
            #ifndef DOXYGEN_GENERATING_OUTPUT
            , typename std::enable_if<!std::is_convertible<const T&, Containers::StringView>::value && !std::is_convertible<const T&, Containers::ArrayView<const void>>::value, int>::type = 0
            #endif
        > constexpr /*implicit*/ MaterialAttributeData(Containers::StringView name, const T& value) noexcept;

        /**
         * @brief Construct with a string name and a string value
         * @param name      Attribute name
         * @param value     Attribute value
         *
         * The @p name is expected to be non-empty and the combined length of
         * @p name and @p value is expected to fit into 60 bytes. Type is set
         * to @ref MaterialAttributeType::String.
         *
         * This function is useful in @cpp constexpr @ce contexts and for
         * creating custom material attributes. For known attributes prefer to
         * use @ref MaterialAttributeData(MaterialAttribute, Containers::StringView)
         * if you don't need @cpp constexpr @ce, as it additionally checks that
         * given attribute has the expected type.
         */
        constexpr /*implicit*/ MaterialAttributeData(Containers::StringView name, Containers::StringView value) noexcept;

        /**
         * @brief Construct with a string name and a buffer value
         * @param name      Attribute name
         * @param value     Attribute value
         *
         * The @p name is expected to be non-empty and the combined length of
         * @p name and @p value is expected to fit into 61 bytes. Type is set
         * to @ref MaterialAttributeType::Buffer.
         *
         * This function is useful for creating custom material attributes.
         * Currently there isn't any builtin @ref MaterialAttribute with a
         * buffer data type.
         */
        /*implicit*/ MaterialAttributeData(Containers::StringView name, Containers::ArrayView<const void> value) noexcept: MaterialAttributeData{name, MaterialAttributeType::Buffer, 0, &value} {}

        #ifndef DOXYGEN_GENERATING_OUTPUT
        /* "Sure can't be constexpr" overloads to avoid going through the
           *insane* overload puzzle when not needed */
        template<class T, typename std::enable_if<!std::is_convertible<const T&, Containers::StringView>::value && !std::is_convertible<const T&, Containers::ArrayView<const void>>::value, int>::type = 0> /*implicit*/ MaterialAttributeData(const char* name, const T& value) noexcept: MaterialAttributeData{name, Implementation::MaterialAttributeTypeFor<T>::type(), sizeof(T), &value} {}
        /*implicit*/ MaterialAttributeData(const char* name, Containers::StringView value) noexcept: MaterialAttributeData{name, MaterialAttributeType::String, 0, &value} {}
        #endif

        /**
         * @brief Construct with a predefined name
         * @param name      Attribute name
         * @param value     Attribute value
         *
         * Compared to @ref MaterialAttributeData(Containers::StringView, const T&)
         * checks that the attribute is in expected type. The
         * @ref MaterialAttribute gets converted to a corresponding string
         * name. Apart from the type check, the following two instances are
         * equivalent:
         *
         * @snippet Trade.cpp MaterialAttributeData-name
         */
        template<class T
            #ifndef DOXYGEN_GENERATING_OUTPUT
            , typename std::enable_if<!std::is_convertible<const T&, Containers::StringView>::value, int>::type = 0
            #endif
        > /*implicit*/ MaterialAttributeData(MaterialAttribute name, const T& value) noexcept: MaterialAttributeData{name, Implementation::MaterialAttributeTypeFor<T>::type(), &value} {}

        /**
         * @brief Construct with a predefined name and a string value
         * @param name      Attribute name
         * @param value     Attribute value
         *
         * Compared to @ref MaterialAttributeData(Containers::StringView, Containers::StringView)
         * checks that the attribute is in expected type. The
         * @ref MaterialAttribute gets converted to a corresponding string
         * name.
         */
        /*implicit*/ MaterialAttributeData(MaterialAttribute name, Containers::StringView value) noexcept: MaterialAttributeData{name, MaterialAttributeType::String, &value} {}

        /* No MaterialAttributeData(MaterialAttribute, Containers::ArrayView<const void>)
           variant as there's no builtin MaterialAttributeType::Buffer
           attribute yet */

        /**
         * @brief Construct from a type-erased value
         * @param name      Attribute name
         * @param type      Attribute type
         * @param value     Type-erased value
         *
         * The @p name is expected to be non-empty.
         *
         * In case @p type is neither @ref MaterialAttributeType::String nor
         * @ref MaterialAttributeType::Buffer, copies a number of bytes
         * according to @ref materialAttributeTypeSize() from @p value. The
         * @p name together with @p value is expected to fit into 62 bytes.
         * Note that in case of a @ref MaterialAttributeType::Pointer or a
         * @ref MaterialAttributeType::MutablePointer, takes a
         * *pointer to a pointer*, not the pointer value itself.
         *
         * In case @p type is @ref MaterialAttributeType::String, @p value is
         * expected to point to a @relativeref{Corrade,Containers::StringView}.
         * The combined length of @p name and @p value strings is expected to
         * fit into 60 bytes. In case @p type is
         * @ref MaterialAttributeType::Buffer, @p value is expected to point to
         * a @relativeref{Corrade,Containers::ArrayView}. The combined length
         * of @p name and @p value views is expected to fit into 61 bytes.
         */
        /*implicit*/ MaterialAttributeData(Containers::StringView name, MaterialAttributeType type, const void* value) noexcept;

        /**
         * @brief Construct with a predefined name and a type-erased value
         * @param name      Attribute name
         * @param type      Attribute type
         * @param value     Attribute value
         *
         * Compared to @ref MaterialAttributeData(Containers::StringView, MaterialAttributeType, const void*)
         * checks that the attribute is in expected type. The
         * @ref MaterialAttribute gets converted to a corresponding string
         * name.
         */
        /*implicit*/ MaterialAttributeData(MaterialAttribute name, MaterialAttributeType type, const void* value) noexcept;

        /**
         * @brief Construct a layer name attribute
         * @param layerName Material layer name
         *
         * Equivalent to calling @ref MaterialAttributeData(MaterialAttribute, Containers::StringView)
         * with @ref MaterialAttribute::LayerName and a string corresponding to
         * @p layerName.
         */
        /*implicit*/ MaterialAttributeData(MaterialLayer layerName) noexcept;

        /** @brief Attribute type */
        MaterialAttributeType type() const { return _data.type; }

        /**
         * @brief Attribute name
         *
         * The returned view always has
         * @relativeref{Corrade,Containers::StringViewFlag::NullTerminated} set.
         */
        Containers::StringView name() const { return _data.data + 1; }

        /**
         * @brief Type-erased attribute value
         *
         * Cast the pointer to a concrete type based on @ref type(). Note that
         * in case of a @ref MaterialAttributeType::Pointer or a
         * @ref MaterialAttributeType::MutablePointer, returns a
         * *pointer to a pointer*, not the pointer value itself.
         *
         * In case of a @ref MaterialAttributeType::String, returns a
         * null-terminated @cpp const char* @ce (not a pointer to
         * @relativeref{Corrade,Containers::StringView}). This doesn't preserve
         * the actual string size in case the string data contain @cpp '\0' @ce
         * bytes, thus prefer to use typed access in that case.
         *
         * In case of a @ref MaterialAttributeType::Buffer, returns a
         * pointer to the data with no size information. Prefer to use typed
         * access in that case.
         */
        const void* value() const;

        /**
         * @brief Attribute value
         *
         * Expects that @p T corresponds to @ref type().
         */
        template<class T> T value() const;

    private:
        friend MaterialData;

        explicit MaterialAttributeData(Containers::StringView name, const MaterialAttributeType type, std::size_t typeSize, const void* value) noexcept;

        /* Most of this is needed only for the constexpr constructor (yay C++),
           the actual data layout is

            |------------------------- x B -----------------------|

            +--------+------- .. -----+-------- .. ---------------+
            |  type  | name   ..   \0 |           data            |
            |   1 B  |  (x - n - 2) B |            n B            |
            +--------+------- .. -----+-------- .. --------+------+
            | String | name   ..   \0 |     data .. \0     | size |
            |   1 B  |  (x - n - 4) B |         n B        |  1 B |
            +--------+------- .. -----+-------- .. -------++------+
            | Buffer | name \0 | size |       .. \0       |  data |
            |   1 B  | m + 1 B |  1 B | (x - m - n - 3) B |  n B  |
            +--------+---------+------+-------- .. -------+-------+

          where

           - `x` is Implementation::MaterialAttributeDataSize,
           - `type` is an 8-bit MaterialAttributeType,
           - `data` is of size matching `type`, at the offset of
             `(x - materialAttributeTypeSize(type))` B, or in case of strings
             at offset `(x - string.size() - 2)` B, with one byte for storing
             size and one null terminator, or in case of buffers at offset
             `(x - buffer.size())` B,
           - `name` is a null-terminated string filling the rest

          This way the name is always at the same offset to make binary search
          lookup fast and efficient, and data being at the end (instead of
          right after the null-terminated string) makes them accessible in O(1)
          as well. In case of string values, to achieve O(1) access, the size
          is stored as the last byte and the string data is right before.

          The only exception is arbitrary data buffers. There, similarly to
          plain values, it's important that the data are aligned, which means
          we can't store the 1-byte size at the end. Instead, it's put right
          after the null-terminated name, which means it takes O(m) to
          retrieve. But since names have a constant upper bound on their length
          and buffers are not so common, it shouldn't be too problematic. */
        struct StringData {
            template<std::size_t ...sequence> constexpr explicit StringData(MaterialAttributeType type, Containers::StringView name, Containers::StringView value, Containers::Implementation::Sequence<sequence...>): type{type}, nameValue{(sequence < name.size() ? name[sequence] : (sequence - (Implementation::MaterialAttributeDataSize - value.size() - 3) < value.size() ? value[sequence - (Implementation::MaterialAttributeDataSize - value.size() - 3)] : '\0'))...}, size{UnsignedByte(value.size())} {}
            constexpr explicit StringData(MaterialAttributeType type, Containers::StringView name, Containers::StringView value): StringData{type, name, value, typename Containers::Implementation::GenerateSequence<Implementation::MaterialAttributeDataSize - 2>::Type{}} {}

            MaterialAttributeType type;
            char nameValue[Implementation::MaterialAttributeDataSize - 2];
            UnsignedByte size;
        };
        union ErasedScalar {
            constexpr explicit ErasedScalar(Float value): f{value} {}
            constexpr explicit ErasedScalar(Deg value): f{Float(value)} {}
            constexpr explicit ErasedScalar(Rad value): f{Float(value)} {}
            constexpr explicit ErasedScalar(UnsignedInt value): u{value} {}
            constexpr explicit ErasedScalar(Int value): i{value} {}
            constexpr explicit ErasedScalar(MaterialTextureSwizzle value):
                /* Interestingly enough, on GCC 4.8, using u{} will spam with
                    warning: parameter ‘value’ set but not used [-Wunused-but-set-parameter]
                   even though everything works as intended. Using () instead. */
                u(UnsignedInt(value)) {}

            Float f;
            UnsignedInt u;
            Int i;
        };
        union ErasedLongScalar {
            constexpr explicit ErasedLongScalar(UnsignedLong value): u{value} {}
            constexpr explicit ErasedLongScalar(Long value): i{value} {}

            UnsignedLong u;
            Long i;
        };
        template<std::size_t size> union ErasedVector {
            constexpr explicit ErasedVector(const Math::Vector<size, Float>& value): f{value} {}
            constexpr explicit ErasedVector(const Math::Vector<size, UnsignedInt>& value): u{value} {}
            constexpr explicit ErasedVector(const Math::Vector<size, Int>& value): i{value} {}

            Math::Vector<size, Float> f;
            Math::Vector<size, UnsignedInt> u;
            Math::Vector<size, Int> i;
        };
        template<std::size_t cols, std::size_t rows> union ErasedMatrix {
            constexpr explicit ErasedMatrix(const Math::RectangularMatrix<cols, rows, Float>& value): a{value} {}
            constexpr explicit ErasedMatrix(const Math::RectangularMatrix<rows, cols, Float>& value): b{value} {}

            Math::RectangularMatrix<cols, rows, Float> a;
            Math::RectangularMatrix<rows, cols, Float> b;
        };
        template<class T> struct Data {
            template<class U, std::size_t ...sequence> constexpr explicit Data(MaterialAttributeType type, Containers::StringView name, const U& value, Containers::Implementation::Sequence<sequence...>): type{type}, name{(sequence < name.size() ? name[sequence] : '\0')...}, value{value} {}
            template<class U> constexpr explicit Data(MaterialAttributeType type, Containers::StringView name, const U& value): Data{type, name, value, typename Containers::Implementation::GenerateSequence<63 - sizeof(T)>::Type{}} {}

            MaterialAttributeType type;
            char name[Implementation::MaterialAttributeDataSize - sizeof(MaterialAttributeType) - sizeof(T)];
            T value;
        };
        union alignas(8) Storage {
            constexpr explicit Storage() noexcept: data{} {}

            constexpr explicit Storage(Containers::StringView name, Containers::StringView value) noexcept: s{MaterialAttributeType::String, name, value} {}

            template<class T, typename std::enable_if<sizeof(T) == 1, int>::type = 0> constexpr explicit Storage(MaterialAttributeType type, Containers::StringView name, const T& value) noexcept: _1{type, name, value} {}
            template<class T, typename std::enable_if<sizeof(T) == 4 && !std::is_pointer<T>::value, int>::type = 0> constexpr explicit Storage(MaterialAttributeType type, Containers::StringView name, const T& value) noexcept: _4{type, name, value} {}
            template<class T, typename std::enable_if<sizeof(T) == 8 && !Math::IsVector<T>::value && !std::is_pointer<T>::value, int>::type = 0> constexpr explicit Storage(MaterialAttributeType type, Containers::StringView name, const T& value) noexcept: _8{type, name, value} {}
            template<class T, typename std::enable_if<sizeof(T) == 8 && Math::IsVector<T>::value && !std::is_pointer<T>::value, int>::type = 0> constexpr explicit Storage(MaterialAttributeType type, Containers::StringView name, const T& value) noexcept: _8v{type, name, value} {}
            constexpr explicit Storage(MaterialAttributeType type, Containers::StringView name, const void* value) noexcept: p{type, name, value} {}
            template<class T, typename std::enable_if<sizeof(T) == 12, int>::type = 0> constexpr explicit Storage(MaterialAttributeType type, Containers::StringView name, const T& value) noexcept: _12{type, name, value} {}
            template<class T, typename std::enable_if<sizeof(T) == 16 && Math::IsVector<T>::value, int>::type = 0> constexpr explicit Storage(MaterialAttributeType type, Containers::StringView name, const T& value) noexcept: _16{type, name, value} {}
            template<class T, typename std::enable_if<sizeof(T) == 16 && !Math::IsVector<T>::value, int>::type = 0> constexpr explicit Storage(MaterialAttributeType type, Containers::StringView name, const T& value) noexcept: _16m{type, name, value} {}
            template<class T, typename std::enable_if<sizeof(T) == 24, int>::type = 0> constexpr explicit Storage(MaterialAttributeType type, Containers::StringView name, const T& value) noexcept: _24{type, name, value} {}
            template<class T, typename std::enable_if<sizeof(T) == 32, int>::type = 0> constexpr explicit Storage(MaterialAttributeType type, Containers::StringView name, const T& value) noexcept: _32{type, name, value} {}
            template<class T, typename std::enable_if<sizeof(T) == 36, int>::type = 0> constexpr explicit Storage(MaterialAttributeType type, Containers::StringView name, const T& value) noexcept: _36{type, name, value} {}
            template<class T, typename std::enable_if<sizeof(T) == 48, int>::type = 0> constexpr explicit Storage(MaterialAttributeType type, Containers::StringView name, const T& value) noexcept: _48{type, name, value} {}

            MaterialAttributeType type;
            char data[Implementation::MaterialAttributeDataSize];
            StringData s;
            /* Buffer values can't be filled in a constexpr way so they don't
               have a dedicated union type. The filling is done in
               MaterialAttributeData(Containers::StringView, MaterialAttributeType, std::size_t, const void*)
               manually instead. */
            Data<bool> _1;
            Data<const void*> p;
            Data<ErasedScalar> _4;
            Data<ErasedLongScalar> _8;
            Data<ErasedVector<2>> _8v;
            Data<ErasedVector<3>> _12;
            Data<ErasedVector<4>> _16;
            Data<Math::RectangularMatrix<2, 2, Float>> _16m;
            Data<ErasedMatrix<2, 3>> _24;
            Data<ErasedMatrix<2, 4>> _32;
            Data<Math::RectangularMatrix<3, 3, Float>> _36;
            Data<ErasedMatrix<3, 4>> _48;
        } _data;

        static_assert(sizeof(Storage) == Implementation::MaterialAttributeDataSize, "something is off, huh");
};

/**
@brief Material type

@see @ref MaterialTypes, @ref MaterialData::types()
*/
enum class MaterialType: UnsignedInt {
    /**
     * Flat. Use @ref FlatMaterialData for convenience attribute access.
     * Materials of this type are generally not combined with any other types.
     */
    Flat = 1 << 0,

    /**
     * Phong. Use @ref PhongMaterialData for convenience attribute access.
     */
    Phong = 1 << 1,

    /**
     * PBR metallic/roughness. Use @ref PbrMetallicRoughnessMaterialData for
     * convenience attribute access.
     */
    PbrMetallicRoughness = 1 << 2,

    /**
     * PBR specular/glossiness. Use @ref PbrSpecularGlossinessMaterialData for
     * convenience attribute access.
     */
    PbrSpecularGlossiness = 1 << 3,

    /**
     * PBR clear coat layer. Use @ref PbrClearCoatMaterialData for convenience
     * attribute access.
     */
    PbrClearCoat = 1 << 4
};

/** @debugoperatorenum{MaterialType} */
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, MaterialType value);

/**
@brief Material types
@m_since_latest

@see @ref MaterialData::types()
*/
typedef Containers::EnumSet<MaterialType> MaterialTypes;

CORRADE_ENUMSET_OPERATORS(MaterialTypes)

/** @debugoperatorenum{MaterialTypes} */
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, MaterialTypes value);

/**
@brief Material alpha mode

Convenience access to @ref MaterialAttribute::AlphaBlend and
@ref MaterialAttribute::AlphaMask attributes.
@see @ref MaterialData::alphaMode(), @ref MaterialData::alphaMask()
*/
enum class MaterialAlphaMode: UnsignedByte {
    /** Alpha value is ignored and the rendered output is fully opaque. */
    Opaque,

    /**
     * The rendered output is either fully transparent or fully opaque,
     * depending on the alpha value and specified
     * @ref MaterialData::alphaMask() value.
     */
    Mask,

    /**
     * The alpha value is used to combine source and destination colors using
     * additive blending.
     */
    Blend
};

/** @debugoperatorenum{MaterialAlphaMode} */
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, MaterialAlphaMode value);

/**
@brief Material data
@m_since_latest

Key-value store for builtin as well as custom material attributes, with an
ability to define additional layers further affecting the base material.
Populated instances of this class are returned from
@ref AbstractImporter::material(), can be passed to
@ref AbstractSceneConverter::add(const MaterialData&, Containers::StringView)
as well as used in various
@ref MaterialTools algorithms
. Like with other @ref Trade types, the internal
representation is fixed upon construction and allows only optional in-place
modification of the attribute values itself, but not of the overall structure.

@section Trade-MaterialData-usage Usage

The simplest usage is through templated @ref attribute() functions, which take
a string attribute name or one of the pre-defined @ref MaterialAttribute
values. You're expected to check for attribute presence first with
@ref hasAttribute(), and the requested type has to match @ref attributeType().
To make things easier, each of the attributes defined in @ref MaterialAttribute
has a strictly defined type, so you can safely assume the type when requesting
those. In addition there's @ref findAttribute() and @ref attributeOr() that
return a @relativeref{Corrade,Containers::NullOpt} or a default value in case
given attribute is not found.

@snippet Trade.cpp MaterialData-usage

It's also possible to iterate through all attributes using @ref attributeName(),
@ref attributeType() and @ref attribute() taking indices instead of names, with
@ref attributeCount() returning the total attribute count.

@subsection Trade-MaterialData-usage-types Material types and convenience accessors

A material usually consists of a set of attributes for a particular rendering
workflow --- PBR metallic/roughness, Phong or for example flat-shaded
materials. To hint what the material contains, @ref types() returns a set of
@ref MaterialType values. It's not just a single value as the data can define
attributes for more than one material type (for example both metallic/roughness
and specular/glossiness PBR workflow), allowing the application to pick the
best type for a particular use case.

Because retrieving everything through the @ref attribute() APIs can get verbose
and complex, the @ref Trade library provides a set of accessor APIs for common
material types such as @ref FlatMaterialData, @ref PhongMaterialData,
@ref PbrMetallicRoughnessMaterialData, @ref PbrSpecularGlossinessMaterialData
as well as material layers like @ref PbrClearCoatMaterialData. Using @ref as()
you can convert any @ref MaterialData instance to a reference to one of those.
These convenience APIs then take care of default values when an attribute isn't
present or handle fallbacks when an attribute can be defined in multiple ways:

@snippet Trade.cpp MaterialData-usage-types

Each @ref MaterialAttribute is exposed through one or more of those convenience
APIs, see the documentation of of a particular enum value for more information.

@subsection Trade-MaterialData-usage-texture-complexity Texture packing, coordinate transformation and coordinate sets

The material APIs allow for a lot of flexibility --- texture maps may be
arbitrarily packed together to efficiently use all four channels, each texture
can use a different set of texture coordinates and there can be a different
coordinate transformation for each texture.

In most cases, however, real-world textures fit into a few well-known packing
schemes and usually have a common transformation and coordinate sets for all.
Checking for all corner cases on the application side would be a headache, so
there are queries like @ref PbrSpecularGlossinessMaterialData::hasSpecularGlossinessTexture() or
@ref PbrSpecularGlossinessMaterialData::hasCommonTextureTransformation() to
help narrowing the options down:

@snippet Trade.cpp MaterialData-usage-texture-complexity

@subsection Trade-MaterialData-usage-layers Material layers

In addition to the base material, there can be material layers. While a
material attribute can be specified only once for a particular layer, multiple
layers can use the same attribute name for different purpose. Layers are
commonly used in PBR workflows to describe lacquered wood, metallic paint or
for example a thin film on leather surfaces. You can enumerate and query layers
using @ref layerCount(), @ref layerName() and @ref hasLayer(), layer-specific
attributes are retrieved by passing layer ID or name as the first parameter to
the @ref attribute() family of APIs.

For each layer there can be predefined @ref layerFactor(),
@ref layerFactorTexture() and other texture-related attributes which define how
much the layer affects the underlying material, but the exact semantics of how
the factor is applied is left to the layer implementation.

Here's an example showing retrieval of a clear coat layer parameters, if
present:

@snippet Trade.cpp MaterialData-usage-layers

Like with base material attributes, builtin layers also have convenience
accessor APIs. The above can be written in a more compact way using
@link PbrClearCoatMaterialData @endlink:

@snippet Trade.cpp MaterialData-usage-layers-types

@subsection Trade-MaterialData-usage-mutable Mutable data access

The interfaces implicitly return attribute values by copy or through
@cpp const @ce views on the contained data through the @ref attributeData(),
@ref layerData() and @ref attribute() accessors. This is done because in
general case the data can also refer to a memory-mapped file or constant
memory. In cases when it's desirable to modify the attribute values in-place,
there's a set of @ref mutableAttribute() functions. To use these, you need to
check that the data are mutable using @ref attributeDataFlags() first. The
following snippet desaturates the base color of a PBR material:

@snippet Trade.cpp MaterialData-usage-mutable

Because the class internally expects the attribute data to be sorted and
partitioned into layers, it's not possible to modify attribute names,
add/remove attributes or change layer offsets --- only to edit values of
existing attributes.

@section Trade-MaterialData-populating Populating an instance

A @ref MaterialData instance by default takes over ownership of an
@relativeref{Corrade,Containers::Array} containing @ref MaterialAttributeData
instances, together with @ref MaterialTypes suggesting available material types
(or an empty set, in case of a fully custom material). Attribute values can be
in one of the types from @ref MaterialAttributeType, and the type is in most
cases inferred implicitly. The class internally uses strings for attribute
names, but you're encouraged to use the predefined names from
@ref MaterialAttribute --- with those, the attribute gets checked additionally
that it's in an expected type. Attribute order doesn't matter, the array gets
internally sorted by name to allow a @f$ \mathcal{O}(\log n) @f$ lookup.

@snippet Trade.cpp MaterialData-populating

@subsection Trade-MaterialData-populating-non-owned Non-owned instances

In some cases you may want the @ref MaterialData instance to only refer to
external data without taking ownership, for example with a memory-mapped file,
global data etc. For that, instead of moving in an
@relativeref{Corrade,Containers::Array} of @ref MaterialAttributeData or
allocating it implicitly from an initializer list in the constructor, pass
@ref DataFlags describing data mutability and ownership together with an
@relativeref{Corrade,Containers::ArrayView}. Note that in this case, since the
attribute data is treated as immutable, you *have to* ensure the list is
already sorted by name.

@snippet Trade.cpp MaterialData-populating-non-owned

<b></b>

@m_class{m-note m-info}

@par
    Additionally, as shown above, in order to create a @cpp constexpr @ce
    @ref MaterialAttributeData array, you need to use
    @relativeref{Corrade,Containers::StringView} literals instead of plain C
    strings or the @ref MaterialAttribute enum, and be sure to call only
    @cpp constexpr @ce-enabled constructors of stored data types.

@subsection Trade-MaterialData-populating-custom Custom material attributes

While attribute names beginning with uppercase letters and whitespace are
reserved for builtin Magnum attributes, anything beginning with a lowercase
letter or a printable non-letter character can be a custom attribute. For
greater flexibility, custom attributes can be also strings, untyped buffers
or pointers, allowing you to store arbitrary properties such as image
filenames or direct texture pointers instead of IDs:

@snippet Trade.cpp MaterialData-populating-custom

@subsection Trade-MaterialData-populating-layers Adding material layers

Material layers are internally represented as ranges of the attribute array and
by default the whole attribute array is treated as a base material. The actual
split into layers can be described using an additional offset array passed to
@ref MaterialData(MaterialTypes, Containers::Array<MaterialAttributeData>&&, Containers::Array<UnsignedInt>&&, const void*),
where entry *i* specifies the end offset of layer *i* --- in the following
snippet we have two layers (one base material and one clear coat layer), the
base material being the first two attributes and the clear coat layer being
attributes in range @cpp 2 @ce to @cpp 6 @ce (thus four attributes):

@snippet Trade.cpp MaterialData-populating-layers

Like with just a base material, the attributes get sorted for a
@f$ \mathcal{O}(\log n) @f$ lookup --- but not as a whole, each layer
separately. In contrary, because layer order matters, those are not reordered
(and thus their lookup is @f$ \mathcal{O}(n) @f$, however it isn't expected to
have that many layers for this to matter). The layers can be named by supplying
a @ref MaterialAttribute::LayerName attribute (or, like shown above, by using
the convenience @ref MaterialAttributeData::MaterialAttributeData(MaterialLayer)
constructor) but don't have to --- if a layer doesn't have a name, it can be
only looked up by its index, not by a name.

Apart from builtin layers, there's no limit in what the layers can be used for
--- the data can for example describe a completely custom landscape from a set
of authored `rockTile`, `sandTile`, `grassTile` textures and procedurally
generated blend factors `a`, `b`, `c`, `d`:

@snippet Trade.cpp MaterialData-populating-layers-custom

@section Trade-MaterialData-representation Internal representation

The attributes are stored sorted by the key in a contiguous array, with each
@ref MaterialAttributeData item occupying 64 bytes. The item contains a 1-byte
type identifier, the actual value and the rest is occupied with null-terminated
name. This means the name length can vary from 14 bytes for
@ref Magnum::Matrix3x4 "Matrix3x4" / @ref Magnum::Matrix4x3 "Matrix4x3" to 61
bytes for @cpp bool @ce (excluding null terminator). As each item has a fixed
size anyway, there's no value in supporting space-efficient 8-, 16- or
half-float types. Conversely, @ref Magnum::Double "Double" types are currently
not supported either as there isn't currently seen any need for extended
precision.

@m_class{m-block m-warning}

@par Max representable data size
    With the current design, @ref MaterialAttributeData is 64 bytes and in
    order to fit a type identifier and a string attribute name of a reasonable
    length, the maximum data size is capped to 48 bytes. This means
    @ref Magnum::Matrix4x4 "Matrix4x4" isn't listed among supported types, but
    it shouldn't be a problem in practice --- ever an arbitrary color
    correction matrix is just 3x4 values with the bottom row being always
    @f$ \begin{pmatrix} 0 & 0 & 0 & 1 \end{pmatrix} @f$. This restriction might
    get lifted eventually.
*/
class MAGNUM_TRADE_EXPORT MaterialData {
    public:
        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Material flag
         * @m_deprecated_since_latest The flags are no longer stored directly
         *      but generated on-the-fly from attribute data, which makes them
         *      less efficient than calling @ref hasAttribute(),
         *      @ref isDoubleSided() etc.
         *
         * This enum is further extended in subclasses.
         * @see @ref Flags, @ref flags()
         */
        enum class CORRADE_DEPRECATED_ENUM("use hasAttribute() etc. instead") Flag: UnsignedInt {
            /**
             * The material is double-sided. Back faces should not be culled
             * away but rendered as well, with normals flipped for correct
             * lighting.
             */
            DoubleSided = 1 << 0
        };

        /**
         * @brief Material flags
         * @m_deprecated_since_latest The flags are no longer stored directly
         *      but generated on-the-fly from attribute data, which makes them
         *      less efficient than calling @ref hasAttribute(),
         *      @ref isDoubleSided() etc.
         *
         * This enum is extended in subclasses.
         * @see @ref flags()
         */
        CORRADE_IGNORE_DEPRECATED_PUSH /* GCC warns about Flag, ugh */
        typedef CORRADE_DEPRECATED("use hasAttribute() etc. instead") Containers::EnumSet<Flag> Flags;
        CORRADE_IGNORE_DEPRECATED_POP
        #endif

        /**
         * @brief Construct
         * @param types             Which material types are described by this
         *      data. Can be an empty set.
         * @param attributeData     Attribute data
         * @param importerState     Importer-specific state
         *
         * The @p attributeData gets sorted by name internally, expecting no
         * duplicates.
         *
         * The @ref attributeDataFlags() / @ref layerDataFlags() are implicitly
         * set to a combination of @ref DataFlag::Owned and
         * @ref DataFlag::Mutable. For non-owned data use the
         * @ref MaterialData(MaterialTypes, DataFlags, Containers::ArrayView<const MaterialAttributeData>, const void*)
         * constructor instead.
         */
        explicit MaterialData(MaterialTypes types, Containers::Array<MaterialAttributeData>&& attributeData, const void* importerState = nullptr) noexcept: MaterialData{types, Utility::move(attributeData), nullptr, importerState} {}

        /** @overload */
        /* Not noexcept because allocation happens inside */
        explicit MaterialData(MaterialTypes types, std::initializer_list<MaterialAttributeData> attributeData, const void* importerState = nullptr): MaterialData{types, attributeData, {}, importerState} {}

        /**
         * @brief Construct a non-owned material data
         * @param types                 Which material types are described by
         *      this data. Can be an empty set.
         * @param attributeDataFlags    Attribute data flags
         * @param attributeData         Attribute data
         * @param importerState         Importer-specific state
         *
         * Compared to @ref MaterialData(MaterialTypes, Containers::Array<MaterialAttributeData>&&, const void*)
         * creates an instance that doesn't own the passed attribute data. The
         * @p attributeData is expected to be already sorted by name, without
         * duplicates. The @p attributeDataFlags can contain
         * @ref DataFlag::Mutable to indicate the external data can be
         * modified, and is expected to *not* have @ref DataFlag::Owned set.
         * The @ref layerDataFlags() are implicitly set to empty @ref DataFlags.
         */
        explicit MaterialData(MaterialTypes types, DataFlags attributeDataFlags, Containers::ArrayView<const MaterialAttributeData> attributeData, const void* importerState = nullptr) noexcept: MaterialData{types, attributeDataFlags, attributeData, {}, nullptr, importerState} {}

        /**
         * @brief Construct with layers
         * @param types             Which material types are described by this
         *      data. Can be an empty set.
         * @param attributeData     Attribute data
         * @param layerData         Layer offset data
         * @param importerState     Importer-specific state
         *
         * The @p attributeData gets sorted by name internally, expecting no
         * duplicates inside each layer. The @p layerData is expected to be
         * either empty or a monotonically non-decreasing sequence of offsets
         * counting up to @p attributeData size, with *i*-th item specifying
         * end offset of *i*-th layer.
         *
         * The @ref attributeDataFlags() / @ref layerDataFlags() are implicitly
         * set to a combination of @ref DataFlag::Owned and
         * @ref DataFlag::Mutable. For non-owned data use the
         * @ref MaterialData(MaterialTypes, DataFlags, Containers::ArrayView<const MaterialAttributeData>, DataFlags, Containers::ArrayView<const UnsignedInt>, const void*)
         * constructor instead.
         */
        explicit MaterialData(MaterialTypes types, Containers::Array<MaterialAttributeData>&& attributeData, Containers::Array<UnsignedInt>&& layerData, const void* importerState = nullptr) noexcept;

        /** @overload */
        /* Not noexcept because allocation happens inside */
        explicit MaterialData(MaterialTypes types, std::initializer_list<MaterialAttributeData> attributeData, std::initializer_list<UnsignedInt> layerData, const void* importerState = nullptr);

        /**
         * @brief Construct a non-owned material data with layers
         * @param types                 Which material types are described by
         *      this data. Can be an empty set.
         * @param attributeDataFlags    Attribute data flags
         * @param attributeData         Attribute data
         * @param layerDataFlags        Layer offset data flags
         * @param layerData             Layer offset data
         * @param importerState         Importer-specific state
         *
         * The @p data is expected to be already sorted by name, without
         * duplicates inside each layer. The @p layerData is expected to be
         * either empty or a monotonically non-decreasing sequence of offsets
         * counting up to @p attributeData size, with *i*-th item specifying
         * end offset of *i*-th layer.
         *
         * The @p attributeDataFlags / @p layerDataFlags parameters can contain
         * @ref DataFlag::Mutable to indicate the external data can be
         * modified, and are expected to *not* have @ref DataFlag::Owned set.
         */
        /* The second (ignored) DataFlags is present in order to make it ready
           for a possible extension where only one of the data is non-owned.
           But so far I didn't see a need. */
        explicit MaterialData(MaterialTypes types, DataFlags attributeDataFlags, Containers::ArrayView<const MaterialAttributeData> attributeData, DataFlags layerDataFlags, Containers::ArrayView<const UnsignedInt> layerData, const void* importerState = nullptr) noexcept;

        ~MaterialData();

        /** @brief Copying is not allowed */
        MaterialData(const MaterialData&) = delete;

        /** @brief Move constructor */
        MaterialData(MaterialData&&) noexcept;

        /** @brief Copying is not allowed */
        MaterialData& operator=(const MaterialData&) = delete;

        /** @brief Move assignment */
        MaterialData& operator=(MaterialData&&) noexcept;

        /**
         * @brief Attribute data flags
         *¨
         * Since the attribute list is always assumed to be sorted and
         * partitioned into layers, only attribute values can be edited when
         * the @ref DataFlag::Mutable flag is present.
         * @see @ref releaseAttributeData(), @ref mutableAttribute()
         */
        DataFlags attributeDataFlags() const { return _attributeDataFlags; }

        /**
         * @brief Layer data flags
         *
         * Since the attribute list is always assumed to be sorted and
         * partitioned into layers, the @ref DataFlag::Mutable flag has no
         * effect here --- only attribute values can be edited when
         * @ref DataFlag::Mutable is present in @ref attributeDataFlags().
         * @see @ref releaseLayerData()
         */
        DataFlags layerDataFlags() const { return _layerDataFlags; }

        /**
         * @brief Material types
         *
         * Each type indicates that the material data can be interpreted as
         * given type. For custom materials the set can also be empty.
         */
        MaterialTypes types() const { return _types; }

        /**
         * @brief Interpret as a material data of concrete type
         *
         * Returns a reference to @cpp *this @ce cast to given type. @p T is
         * expected to be a subclass of the same size such as
         * @ref PhongMaterialData or @ref PbrMetallicRoughnessMaterialData, as
         * well as layers like @ref PbrClearCoatMaterialData.
         */
        /* MSVC needs the & here, otherwise it complains that "cannot overload
           a member function with ref-qualifier with a member function without
           ref-qualifier". Clang or GCC doesn't. */
        template<class T> const T& as() const & {
            static_assert(std::is_base_of<MaterialData, T>::value && sizeof(T) == sizeof(MaterialData), "expected a trivial subclass of MaterialData");
            return static_cast<const T&>(*this);
        }

        /**
         * @brief Interpret a rvalue as a material data of concrete type
         *
         * Compared to the above, returns a value and not a reference. The
         * original instance then behaves the same as a moved-from instance.
         */
        template<class T> T as() && {
            return T{Utility::move(const_cast<T&>(as<T>()))};
        }

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Material type
         * @m_deprecated_since_latest Use @ref types() instead.
         */
        CORRADE_DEPRECATED("use types() instead") MaterialType type() const {
            return MaterialType(UnsignedInt(_types & MaterialType::Phong));
        }
        #endif

        /**
         * @brief Raw layer offset data
         *
         * May return @cpp nullptr @ce if the material doesn't have any extra
         * layers.
         * @see @ref releaseLayerData()
         */
        Containers::ArrayView<const UnsignedInt> layerData() const { return _layerOffsets; }

        /**
         * @brief Raw attribute data
         *
         * Returns @cpp nullptr @ce if the material has no attributes.
         * @see @ref releaseAttributeData(), @ref attributeDataOffset(),
         *      @ref attributeData(UnsignedInt, UnsignedInt) const,
         *      @ref attributeData(UnsignedInt) const
         */
        Containers::ArrayView<const MaterialAttributeData> attributeData() const { return _data; }

        /**
         * @brief Layer count
         *
         * There's always at least the base material, so this function returns
         * at least @cpp 1 @ce.
         */
        UnsignedInt layerCount() const {
            return _layerOffsets.isEmpty() ? 1 : _layerOffsets.size();
        }

        /**
         * @brief Offset of a layer inside attribute data
         *
         * Returns the offset where attributes for @p layer start in the array
         * returned by @ref attributeData() const. The @p layer is expected to
         * be less *or equal to* @ref layerCount(), i.e. it's always possible
         * to call this function with @cpp layer @ce and @cpp layer + 1 @ce to
         * get the attribute range for given layer, or with @ref layerCount()
         * to get the total attribute count in all layers.
         */
        UnsignedInt attributeDataOffset(UnsignedInt layer) const;

        /**
         * @brief Whether a material has given named layer
         *
         * Layers with no name assigned are skipped. The base material (layer
         * @cpp 0 @ce is skipped as well) to avoid confusing base material with
         * a layer. If you want to create a material consisting of just a
         * layer, use @cpp 0 @ce for the first layer offset in the constructor.
         * @see @ref hasAttribute(), @ref findLayerId()
         */
        bool hasLayer(Containers::StringView layer) const;
        bool hasLayer(MaterialLayer layer) const; /**< @overload */

        /**
         * @brief Find ID of a named layer
         *
         * The @p layer doesn't exist, returns
         * @relativeref{Corrade,Containers::NullOpt}. The lookup is done in an
         * @f$ \mathcal{O}(n) @f$ complexity with @f$ n @f$ being the layer
         * count.
         * @see @ref hasLayer()
         */
        Containers::Optional<UnsignedInt> findLayerId(Containers::StringView layer) const;
        Containers::Optional<UnsignedInt> findLayerId(MaterialLayer layer) const; /**< @overload */

        /**
         * @brief ID of a named layer
         *
         * Like @ref findLayerId(), but the @p layer is expected to exist.
         * @see @ref hasLayer()
         */
        UnsignedInt layerId(Containers::StringView layer) const;
        UnsignedInt layerId(MaterialLayer layer) const; /**< @overload */

        /**
         * @brief Layer name
         *
         * Retrieves a @ref MaterialAttribute::LayerName attribute from given
         * layer, if present. Returns a @cpp nullptr @ce view if the layer
         * has no name, and an empty non-null view if the layer name is empty.
         * The @p layer is expected to be smaller than @ref layerCount() const.
         *
         * The name, if present, is ignored for the base material (layer
         * @cpp 0 @ce) to avoid confsing base material with a layer. If you
         * want to create a material consisting of just a layer, use @cpp 0 @ce
         * for the first layer offset in the constructor.
         * @see @ref materialLayerName()
         */
        Containers::StringView layerName(UnsignedInt layer) const;

        /**
         * @brief Factor of given layer
         *
         * Convenience access to the @ref MaterialAttribute::LayerFactor
         * attribute. If not present, the default is @cpp 1.0f @ce. The
         * @p layer is expected to be smaller than @ref layerCount() const.
         *
         * If the layer has @ref MaterialAttribute::LayerFactorTexture, the
         * factor and texture is meant to be multiplied together.
         */
        Float layerFactor(UnsignedInt layer) const;

        /**
         * @brief Factor of a named layer
         *
         * Convenience access to the @ref MaterialAttribute::LayerFactor
         * attribute. If not present, the default is @cpp 1.0f @ce. The
         * @p layer is expected to exist.
         *
         * If the layer has @ref MaterialAttribute::LayerFactorTexture, the
         * factor and texture is meant to be multiplied together.
         * @see @ref hasLayer()
         */
        Float layerFactor(Containers::StringView layer) const;
        Float layerFactor(MaterialLayer layer) const; /**< @overload */

        /**
         * @brief Factor texture ID for given layer
         *
         * Available only if the @ref MaterialAttribute::LayerFactorTexture
         * attribute is present. Meant to be multiplied with @ref layerFactor().
         * The @p layer is expected to be smaller than @ref layerCount().
         * @see @ref hasAttribute()
         */
        UnsignedInt layerFactorTexture(UnsignedInt layer) const;

        /**
         * @brief Factor texture ID for a named layer
         *
         * Available only if the @ref MaterialAttribute::LayerFactorTexture
         * attribute is present. Meant to be multiplied with @ref layerFactor().
         * The @p layer is expected to exist.
         * @see @ref hasLayer(), @ref hasAttribute()
         */
        UnsignedInt layerFactorTexture(Containers::StringView layer) const;
        UnsignedInt layerFactorTexture(MaterialLayer layer) const; /**< @overload */

        /**
         * @brief Factor texture swizzle for given layer
         *
         * Convenience access to the @ref MaterialAttribute::LayerFactorTextureSwizzle
         * attribute. If not present, the default is @ref MaterialTextureSwizzle::R.
         * Available only if the @ref MaterialAttribute::LayerFactorTexture
         * attribute is present. The @p layer is expected to be smaller than
         * @ref layerCount().
         * @see @ref hasAttribute()
         */
        MaterialTextureSwizzle layerFactorTextureSwizzle(UnsignedInt layer) const;

        /**
         * @brief Factor texture swizzle for a named layer
         *
         * Convenience access to the @ref MaterialAttribute::LayerFactorTextureSwizzle
         * attribute. If not present, the default is @ref MaterialTextureSwizzle::R.
         * Available only if the @ref MaterialAttribute::LayerFactorTexture
         * attribute is present. The @p layer is expected to exist.
         * @see @ref hasLayer(), @ref hasAttribute()
         */
        MaterialTextureSwizzle layerFactorTextureSwizzle(Containers::StringView layer) const;
        MaterialTextureSwizzle layerFactorTextureSwizzle(MaterialLayer layer) const; /**< @overload */

        /**
         * @brief Factor texture coordinate transformation matrix for given layer
         *
         * Convenience access to the @ref MaterialAttribute::LayerFactorTextureMatrix
         * / @ref MaterialAttribute::TextureMatrix attributes in given layer or
         * a @ref MaterialAttribute::TextureMatrix attribute in the base
         * material. If neither is present, the default is an identity matrix.
         * Available only if the @ref MaterialAttribute::LayerFactorTexture
         * attribute is present. The @p layer is expected to be smaller than
         * @ref layerCount().
         * @see @ref hasAttribute()
         */
        Matrix3 layerFactorTextureMatrix(UnsignedInt layer) const;

        /**
         * @brief Factor texture coordinate transformation matrix for a named layer
         *
         * Convenience access to the @ref MaterialAttribute::LayerFactorTextureMatrix
         * / @ref MaterialAttribute::TextureMatrix attributes in given layer or
         * a @ref MaterialAttribute::TextureMatrix attribute in the base
         * material. If neither is present, the default is an identity matrix.
         * Available only if the @ref MaterialAttribute::LayerFactorTexture
         * attribute is present. The @p layer is expected to exist.
         * @see @ref hasLayer(), @ref hasAttribute()
         */
        Matrix3 layerFactorTextureMatrix(Containers::StringView layer) const;
        Matrix3 layerFactorTextureMatrix(MaterialLayer layer) const; /**< @overload */

        /**
         * @brief Factor texture coordinate set for given layer
         *
         * Convenience access to the @ref MaterialAttribute::LayerFactorTextureCoordinates
         * / @ref MaterialAttribute::TextureCoordinates attributes in given
         * layer or a @ref MaterialAttribute::TextureCoordinates attribute in
         * the base material. If neither is present, the default is @cpp 0 @ce.
         * Available only if the @ref MaterialAttribute::LayerFactorTexture
         * attribute is present. The @p layer is expected to be smaller than
         * @ref layerCount().
         * @see @ref hasAttribute()
         */
        UnsignedInt layerFactorTextureCoordinates(UnsignedInt layer) const;

        /**
         * @brief Factor texture coordinate set for a named layer
         *
         * Convenience access to the @ref MaterialAttribute::LayerFactorTextureCoordinates
         * / @ref MaterialAttribute::TextureCoordinates attributes in given
         * layer or a @ref MaterialAttribute::TextureCoordinates attribute in
         * the base material. If neither is present, the default is @cpp 0 @ce.
         * Available only if the @ref MaterialAttribute::LayerFactorTexture
         * attribute is present. The @p layer is expected to exist.
         * @see @ref hasLayer(), @ref hasAttribute()
         */
        UnsignedInt layerFactorTextureCoordinates(Containers::StringView layer) const;
        UnsignedInt layerFactorTextureCoordinates(MaterialLayer layer) const; /**< @overload */

        /**
         * @brief Factor array texture layer for given layer
         *
         * Convenience access to the @ref MaterialAttribute::LayerFactorTextureLayer
         * / @ref MaterialAttribute::TextureLayer attributes in given layer or
         * a @ref MaterialAttribute::TextureLayer attribute in the base
         * material. If neither is present, the default is @cpp 0 @ce.
         * Available only if the @ref MaterialAttribute::LayerFactorTexture
         * attribute is present. The @p layer is expected to be smaller than
         * @ref layerCount().
         * @see @ref hasAttribute()
         */
        UnsignedInt layerFactorTextureLayer(UnsignedInt layer) const;

        /**
         * @brief Factor array texture layer for a named layer
         *
         * Convenience access to the @ref MaterialAttribute::LayerFactorTextureLayer
         * / @ref MaterialAttribute::TextureLayer attributes in given layer or
         * a @ref MaterialAttribute::TextureLayer attribute in the base
         * material. If neither is present, the default is @cpp 0 @ce.
         * Available only if the @ref MaterialAttribute::LayerFactorTexture
         * attribute is present. The @p layer is expected to exist.
         * @see @ref hasLayer(), @ref hasAttribute()
         */
        UnsignedInt layerFactorTextureLayer(Containers::StringView layer) const;
        UnsignedInt layerFactorTextureLayer(MaterialLayer layer) const; /**< @overload */

        /**
         * @brief Attribute count in given layer
         *
         * The @p layer is expected to be smaller than @ref layerCount() const.
         */
        UnsignedInt attributeCount(UnsignedInt layer) const;

        /**
         * @brief Attribute count in a named layer
         *
         * The @p layer is expected to exist.
         * @see @ref hasLayer()
         */
        UnsignedInt attributeCount(Containers::StringView layer) const;
        UnsignedInt attributeCount(MaterialLayer layer) const; /**< @overload */

        /**
         * @brief Attribute count in the base material
         *
         * Equivalent to calling @ref attributeCount(UnsignedInt) const with
         * @p layer set to @cpp 0 @ce.
         */
        UnsignedInt attributeCount() const { return attributeCount(0); }

        /**
         * @brief Whether a material layer has given attribute
         *
         * The @p layer is expected to be smaller than @ref layerCount() const.
         * @see @ref findAttribute(), @ref attributeOr(), @ref hasLayer(),
         *      @ref findAttributeId()
         */
        bool hasAttribute(UnsignedInt layer, Containers::StringView name) const;
        bool hasAttribute(UnsignedInt layer, MaterialAttribute name) const; /**< @overload */

        /**
         * @brief Whether a named material layer has given attribute
         *
         * The @p layer is expected to exist.
         * @see @ref findAttribute(), @ref attributeOr(), @ref hasLayer(),
         *      @ref findAttributeId()
         */
        bool hasAttribute(Containers::StringView layer, Containers::StringView name) const;
        bool hasAttribute(Containers::StringView layer, MaterialAttribute name) const; /**< @overload */
        bool hasAttribute(MaterialLayer layer, Containers::StringView name) const; /**< @overload */
        bool hasAttribute(MaterialLayer layer, MaterialAttribute name) const; /**< @overload */

        /**
         * @brief Whether the base material has given attribute
         *
         * Equivalent to calling @ref hasAttribute(UnsignedInt, Containers::StringView) const
         * with @p layer set to @cpp 0 @ce.
         * @see @ref findAttribute(), @ref attributeOr(),
         *      @ref findAttributeId()
         */
        bool hasAttribute(Containers::StringView name) const {
            return hasAttribute(0, name);
        }
        bool hasAttribute(MaterialAttribute name) const {
            return hasAttribute(0, name);
        } /**< @overload */

        /**
         * @brief Find ID of a named attribute in given material layer
         *
         * If @p name doesn't exist, returns
         * @relativeref{Corrade,Containers::NullOpt}. The @p layer is expected
         * to be smaller than @ref layerCount() const. The lookup is done in an
         * @f$ \mathcal{O}(\log n) @f$ complexity with @f$ n @f$ being
         * attribute count in given @p layer.
         * @see @ref hasAttribute(), @ref attributeId()
         */
        Containers::Optional<UnsignedInt> findAttributeId(UnsignedInt layer, Containers::StringView name) const;
        Containers::Optional<UnsignedInt> findAttributeId(UnsignedInt layer, MaterialAttribute name) const; /**< @overload */

        /**
         * @brief Find ID of a named attribute in a named material layer
         *
         * If @p name doesn't exist, returns
         * @relativeref{Corrade,Containers::NullOpt}. The @p layer is expected
         * to exist. The lookup is done in an @f$ \mathcal{O}(m + \log n) @f$
         * complexity with @f$ m @f$ being layer count and @f$ n @f$ being
         * attribute count in given @p layer.
         * @see @ref hasLayer(), @ref hasAttribute(), @ref attributeId(),
         *      @ref findLayerId()
         */
        Containers::Optional<UnsignedInt> findAttributeId(Containers::StringView layer, Containers::StringView name) const;
        Containers::Optional<UnsignedInt> findAttributeId(Containers::StringView layer, MaterialAttribute name) const; /**< @overload */
        Containers::Optional<UnsignedInt> findAttributeId(MaterialLayer layer, Containers::StringView name) const; /**< @overload */
        Containers::Optional<UnsignedInt> findAttributeId(MaterialLayer layer, MaterialAttribute name) const; /**< @overload */

        /**
         * @brief Find ID of a named attribute in the base material
         *
         * Equivalent to calling @ref findAttributeId(UnsignedInt, Containers::StringView) const
         * with @p layer set to @cpp 0 @ce.
         */
        Containers::Optional<UnsignedInt> findAttributeId(Containers::StringView name) const;
        Containers::Optional<UnsignedInt> findAttributeId(MaterialAttribute name) const; /**< @overload */

        /**
         * @brief ID of a named attribute in given material layer
         *
         * Like @ref findAttributeId(UnsignedInt, Containers::StringView) const,
         * but the @p name is expected to exist.
         * @see @ref hasAttribute(),
         *      @ref attributeName(UnsignedInt, UnsignedInt) const
         */
        UnsignedInt attributeId(UnsignedInt layer, Containers::StringView name) const;
        UnsignedInt attributeId(UnsignedInt layer, MaterialAttribute name) const; /**< @overload */

        /**
         * @brief ID of a named attribute in a named material layer
         *
         * Like @ref findAttributeId(Containers::StringView, Containers::StringView) const,
         * but the @p name is expected to exist.
         * @see @ref hasLayer(), @ref hasAttribute()
         */
        UnsignedInt attributeId(Containers::StringView layer, Containers::StringView name) const;
        UnsignedInt attributeId(Containers::StringView layer, MaterialAttribute name) const; /**< @overload */
        UnsignedInt attributeId(MaterialLayer layer, Containers::StringView name) const; /**< @overload */
        UnsignedInt attributeId(MaterialLayer layer, MaterialAttribute name) const; /**< @overload */

        /**
         * @brief ID of a named attribute in the base material
         *
         * Equivalent to calling @ref attributeId(UnsignedInt, Containers::StringView) const
         * with @p layer set to @cpp 0 @ce.
         */
        UnsignedInt attributeId(Containers::StringView name) const {
            return attributeId(0, name);
        }
        UnsignedInt attributeId(MaterialAttribute name) const {
            return attributeId(0, name);
        } /**< @overload */

        /**
         * @brief Raw attribute data
         *
         * The @p layer is expected to be smaller than @ref layerCount() const,
         * @p id is expected to be smaller than @ref attributeCount(UnsignedInt) const.
         * @see @ref attributeDataOffset()
         */
        const MaterialAttributeData& attributeData(UnsignedInt layer, UnsignedInt id) const;

        /**
         * @brief Raw attribute data in the base material
         *
         * The @p @p id is expected to be smaller than @ref attributeCount() const.
         * @see @ref attributeDataOffset()
         */
        const MaterialAttributeData& attributeData(UnsignedInt id) const {
            return attributeData(0, id);
        }

        /**
         * @brief Name of an attribute in given material layer
         *
         * The @p layer is expected to be smaller than @ref layerCount() const
         * and the @p id is expected to be smaller than @ref attributeCount(UnsignedInt) const
         * in that layer. The returned view always has
         * @relativeref{Corrade,Containers::StringViewFlag::NullTerminated} set.
         * @see @ref attributeType(), @ref materialAttributeName()
         */
        Containers::StringView attributeName(UnsignedInt layer, UnsignedInt id) const;

        /**
         * @brief Name of an attribute in a named material layer
         *
         * The @p layer is expected to exist and the @p id is expected to be smaller than @ref attributeCount(UnsignedInt) const
         * in that layer.
         * @see @ref hasLayer(), @ref materialAttributeName()
         */
        Containers::StringView attributeName(Containers::StringView layer, UnsignedInt id) const;
        Containers::StringView attributeName(MaterialLayer layer, UnsignedInt id) const; /**< @overload */

        /**
         * @brief Name of an attribute in the base material
         *
         * Equivalent to calling @ref attributeName(UnsignedInt, UnsignedInt) const
         * with @p layer set to @cpp 0 @ce.
         * @see @ref materialAttributeName()
         */
        Containers::StringView attributeName(UnsignedInt id) const {
            return attributeName(0, id);
        }

        /**
         * @brief Type of an attribute in given material layer
         *
         * The @p layer is expected to be smaller than @ref layerCount() const
         * and @p id is expected to be smaller than @ref attributeCount(UnsignedInt) const
         * in that layer.
         * @see @ref attributeName()
         */
        MaterialAttributeType attributeType(UnsignedInt layer, UnsignedInt id) const;

        /**
         * @brief Type of a named attribute in given material layer
         *
         * The @p layer is expected to be smaller than @ref layerCount() const
         * and @p name is expected to exist in that layer.
         * @see @ref hasAttribute()
         */
        MaterialAttributeType attributeType(UnsignedInt layer, Containers::StringView name) const;
        /** @overload */
        MaterialAttributeType attributeType(UnsignedInt layer, MaterialAttribute name) const;

        /**
         * @brief Type of an attribute in a named material layer
         *
         * The @p layer is expected to exist and the @p id is expected to be smaller than @ref attributeCount(UnsignedInt) const
         * in that layer.
         * @see @ref hasLayer()
         */
        MaterialAttributeType attributeType(Containers::StringView layer, UnsignedInt id) const;
        MaterialAttributeType attributeType(MaterialLayer layer, UnsignedInt id) const; /**< @overload */

        /**
         * @brief Type of a named attribute in a named material layer
         *
         * The @p layer is expected to exist and @p name is expected to exist
         * in that layer.
         * @see @ref hasLayer(), @ref hasAttribute()
         */
        MaterialAttributeType attributeType(Containers::StringView layer, Containers::StringView name) const;
        MaterialAttributeType attributeType(Containers::StringView layer, MaterialAttribute name) const; /**< @overload */
        MaterialAttributeType attributeType(MaterialLayer layer, Containers::StringView name) const; /**< @overload */
        MaterialAttributeType attributeType(MaterialLayer layer, MaterialAttribute name) const; /**< @overload */

        /**
         * @brief Type of an attribute in the base material
         *
         * Equivalent to calling @ref attributeType(UnsignedInt, UnsignedInt) const
         * with @p layer set to @cpp 0 @ce.
         */
        MaterialAttributeType attributeType(UnsignedInt id) const {
            return attributeType(0, id);
        }

        /**
         * @brief Type of a named attribute in the base material
         *
         * Equivalent to calling @ref attributeType(UnsignedInt, Containers::StringView) const
         * with @p layer set to @cpp 0 @ce.
         */
        MaterialAttributeType attributeType(Containers::StringView name) const {
            return attributeType(0, name);
        }
        MaterialAttributeType attributeType(MaterialAttribute name) const {
            return attributeType(0, name);
        } /**< @overload */

        /**
         * @brief Type-erased value of an attribute in given material layer
         *
         * The @p layer is expected to be smaller than @ref layerCount() const
         * and @p id is expected to be smaller than @ref attributeCount(UnsignedInt) const
         * in that layer. Cast the pointer to a concrete type based on
         * @ref type().
         *
         * -    In case of a @ref MaterialAttributeType::Pointer or a
         *      @ref MaterialAttributeType::MutablePointer, returns a
         *      *pointer to a pointer*, not the pointer value itself.
         * -    In case of a @ref MaterialAttributeType::String returns a
         *      null-terminated @cpp const char* @ce (not a pointer to
         *      @relativeref{Corrade,Containers::StringView}). This doesn't
         *      preserve the actual string size in case the string data contain
         *      zero bytes, thus prefer to use typed access in that case.
         * -    In case of a @ref MaterialAttributeType::Buffer returns a
         *      pointer to the data with no size information, Prefer to use
         *      typed access in that case.
         */
        const void* attribute(UnsignedInt layer, UnsignedInt id) const;

        /**
         * @brief Type-erased mutable value of an attribute in given material layer
         *
         * Like @ref attribute(UnsignedInt, UnsignedInt) const but returns a
         * mutable pointer. Expects that the material is mutable.
         * @see @ref attributeDataFlags()
         */
        void* mutableAttribute(UnsignedInt layer, UnsignedInt id);

        /**
         * @brief Type-erased value of a named attribute in given material layer
         *
         * The @p layer is expected to be smaller than @ref layerCount() const
         * and @p name is expected to exist in that layer. Cast the pointer to
         * a concrete type based on @ref attributeType().
         *
         * -    In case of a @ref MaterialAttributeType::Pointer or a
         *      @ref MaterialAttributeType::MutablePointer, returns a
         *      *pointer to a pointer*, not the pointer value itself.
         * -    In case of a @ref MaterialAttributeType::String returns a
         *      null-terminated @cpp const char* @ce (not a pointer to
         *      @relativeref{Corrade,Containers::StringView}). This doesn't
         *      preserve the actual string size in case the string data contain
         *      zero bytes, thus prefer to use typed access in that case.
         * -    In case of a @ref MaterialAttributeType::Buffer returns a
         *      pointer to the data with no size information, Prefer to use
         *      typed access in that case.
         *
         * @see @ref hasAttribute(), @ref findAttribute(), @ref attributeOr()
         */
        const void* attribute(UnsignedInt layer, Containers::StringView name) const;
        const void* attribute(UnsignedInt layer, MaterialAttribute name) const; /**< @overload */

        /**
         * @brief Type-erased value of a named attribute in given material layer
         *
         * Like @ref attribute(UnsignedInt, Containers::StringView) const, but
         * returns a mutable pointer. Expects that the material is mutable.
         * @see @ref attributeDataFlags()
         */
        void* mutableAttribute(UnsignedInt layer, Containers::StringView name);
        void* mutableAttribute(UnsignedInt layer, MaterialAttribute name); /**< @overload */

        /**
         * @brief Type-erased value of an attribute in a named material layer
         *
         * The @p layer is expected to exist and the @p id is expected to be smaller than @ref attributeCount(UnsignedInt) const
         * in that layer. Cast the pointer to a concrete type based on
         * @ref attributeType().
         *
         * -    In case of a @ref MaterialAttributeType::Pointer or a
         *      @ref MaterialAttributeType::MutablePointer, returns a
         *      *pointer to a pointer*, not the pointer value itself.
         * -    In case of a @ref MaterialAttributeType::String returns a
         *      null-terminated @cpp const char* @ce (not a pointer to
         *      @relativeref{Corrade,Containers::StringView}). This doesn't
         *      preserve the actual string size in case the string data contain
         *      zero bytes, thus prefer to use typed access in that case.
         * -    In case of a @ref MaterialAttributeType::Buffer returns a
         *      pointer to the data with no size information, Prefer to use
         *      typed access in that case.
         *
         * @see @ref hasLayer()
         */
        const void* attribute(Containers::StringView layer, UnsignedInt id) const;
        const void* attribute(MaterialLayer layer, UnsignedInt id) const; /**< @overload */

        /**
         * @brief Type-erased mutable value of an attribute in a named material layer
         *
         * Like @ref attribute(Containers::StringView, UnsignedInt) const but
         * returns a mutable pointer. Expects that the material is mutable.
         * @see @ref attributeDataFlags()
         */
        void* mutableAttribute(Containers::StringView layer, UnsignedInt id);
        void* mutableAttribute(MaterialLayer layer, UnsignedInt id); /**< @overload */

        /**
         * @brief Type-erased value of a named attribute in a named material layer
         *
         * The @p layer is expected to exist and @p name is expected to exist
         * in that layer. Cast the pointer to a concrete type based on
         * @ref attributeType().
         *
         * -    In case of a @ref MaterialAttributeType::Pointer or a
         *      @ref MaterialAttributeType::MutablePointer, returns a
         *      *pointer to a pointer*, not the pointer value itself.
         * -    In case of a @ref MaterialAttributeType::String returns a
         *      null-terminated @cpp const char* @ce (not a pointer to
         *      @relativeref{Corrade,Containers::StringView}). This doesn't
         *      preserve the actual string size in case the string data contain
         *      zero bytes, thus prefer to use typed access in that case.
         * -    In case of a @ref MaterialAttributeType::Buffer returns a
         *      pointer to the data with no size information, Prefer to use
         *      typed access in that case.
         *
         * @see @ref hasLayer(), @ref hasAttribute()
         */
        const void* attribute(Containers::StringView layer, Containers::StringView name) const;
        const void* attribute(Containers::StringView layer, MaterialAttribute name) const; /**< @overload */
        const void* attribute(MaterialLayer layer, Containers::StringView name) const; /**< @overload */
        const void* attribute(MaterialLayer layer, MaterialAttribute name) const; /**< @overload */

        /**
         * @brief Type-erased mutable value of a named attribute in a named material layer
         *
         * Like @ref attribute(Containers::StringView, Containers::StringView) const
         * but returns a mutable pointer. Expects that the material is mutable.
         * @see @ref attributeDataFlags()
         */
        void* mutableAttribute(Containers::StringView layer, Containers::StringView name);
        void* mutableAttribute(Containers::StringView layer, MaterialAttribute name); /**< @overload */
        void* mutableAttribute(MaterialLayer layer, Containers::StringView name); /**< @overload */
        void* mutableAttribute(MaterialLayer layer, MaterialAttribute name); /**< @overload */

        /**
         * @brief Type-erased value of an attribute in the base material
         *
         * Equivalent to calling @ref attribute(UnsignedInt, UnsignedInt) const
         * with @p layer set to @cpp 0 @ce.
         */
        const void* attribute(UnsignedInt id) const {
            return attribute(0, id);
        }

        /**
         * @brief Type-erased mutable value of an attribute in the base material
         *
         * Like @ref attribute(UnsignedInt) const but returns a mutable
         * pointer. Expects that the material is mutable.
         * @see @ref attributeDataFlags()
         */
        void* mutableAttribute(UnsignedInt id) {
            return mutableAttribute(0, id);
        }

        /**
         * @brief Type-erased value of a named attribute in the base material
         *
         * Equivalent to calling @ref attribute(UnsignedInt, Containers::StringView) const
         * with @p layer set to @cpp 0 @ce.
         */
        const void* attribute(Containers::StringView name) const {
            return attribute(0, name);
        }
        const void* attribute(MaterialAttribute name) const {
            return attribute(0, name);
        } /**< @overload */

        /**
         * @brief Type-erased mutable value of a named attribute in the base material
         *
         * Like @ref attribute(Containers::StringView) const but returns a
         * mutable pointer. Expects that the material is mutable.
         * @see @ref attributeDataFlags()
         */
        void* mutableAttribute(Containers::StringView name) {
            return mutableAttribute(0, name);
        }
        void* mutableAttribute(MaterialAttribute name) {
            return mutableAttribute(0, name);
        } /**< @overload */

        /**
         * @brief Value of an attribute in given material layer
         *
         * The @p layer is expected to be smaller than @ref layerCount() const
         * and @p id is expected to be smaller than
         * @ref attributeCount(UnsignedInt) const in that layer. Expects that
         * @p T corresponds to @ref attributeType(UnsignedInt, UnsignedInt) const
         * for given @p layer and @p id. In case of a string, the returned view
         * always has @relativeref{Corrade,Containers::StringViewFlag::NullTerminated}
         * set.
         */
        template<class T> T attribute(UnsignedInt layer, UnsignedInt id) const;

        /**
         * @brief Mutable value of an attribute in given material layer
         *
         * Like @ref attribute(UnsignedInt, UnsignedInt) const but returns a
         * mutable reference. Expects that the material is mutable. In case of
         * a string / buffer, you're expected to use
         * @relativeref{Corrade,Containers::MutableStringView} /
         * @relativeref{Corrade,Containers::ArrayView<void>} instead of
         * @relativeref{Corrade,Containers::StringView} /
         * @relativeref{Corrade,Containers::ArrayView<const void>} for @p T and
         * you get a @relativeref{Corrade,Containers::MutableStringView} /
         * @relativeref{Corrade,Containers::ArrayView<void>} back by-value, not
         * by-reference. Changing the string / buffer size is not possible.
         * @see @ref attributeDataFlags()
         */
        template<class T> typename std::conditional<std::is_same<T, Containers::MutableStringView>::value || std::is_same<T, Containers::ArrayView<void>>::value, T, T&>::type mutableAttribute(UnsignedInt layer, UnsignedInt id);

        /**
         * @brief Value of a named attribute in given material layer
         *
         * The @p layer is expected to be smaller than @ref layerCount() const
         * and @p name is expected to exist in that layer. Expects that @p T
         * corresponds to @ref attributeType(UnsignedInt, Containers::StringView) const
         * for given @p layer and @p name. In case of a string, the returned
         * view always has @relativeref{Corrade,Containers::StringViewFlag::NullTerminated}
         * set.
         * @see @ref hasLayer(), @ref hasAttribute()
         */
        template<class T> T attribute(UnsignedInt layer, Containers::StringView name) const;
        template<class T> T attribute(UnsignedInt layer, MaterialAttribute name) const; /**< @overload */

        /**
         * @brief Mutable value of a named attribute in given material layer
         *
         * Like @ref attribute(UnsignedInt, Containers::StringView) const but
         * returns a mutable reference. Expects that the material is mutable.
         * In case of a string / buffer, you're expected to use
         * @relativeref{Corrade,Containers::MutableStringView} /
         * @relativeref{Corrade,Containers::ArrayView<void>} instead of
         * @relativeref{Corrade,Containers::StringView} /
         * @relativeref{Corrade,Containers::ArrayView<const void>} for @p T and
         * you get a @relativeref{Corrade,Containers::MutableStringView} /
         * @relativeref{Corrade,Containers::ArrayView<void>} back by-value, not
         * by-reference. Changing the string / buffer size is not possible.
         * @see @ref attributeDataFlags()
         */
        template<class T> typename std::conditional<std::is_same<T, Containers::MutableStringView>::value || std::is_same<T, Containers::ArrayView<void>>::value, T, T&>::type mutableAttribute(UnsignedInt layer, Containers::StringView name);
        template<class T> typename std::conditional<std::is_same<T, Containers::MutableStringView>::value || std::is_same<T, Containers::ArrayView<void>>::value, T, T&>::type mutableAttribute(UnsignedInt layer, MaterialAttribute name); /**< @overload */

        /**
         * @brief Value of an attribute in a named material layer
         *
         * The @p layer is expected to exist and @p id is expected to be
         * smaller than @ref attributeCount(UnsignedInt) const in that layer.
         * Expects that @p T corresponds to
         * @ref attributeType(Containers::StringView, UnsignedInt) const
         * for given @p layer and @p id. In case of a string, the returned view
         * always has @relativeref{Corrade,Containers::StringViewFlag::NullTerminated}
         * set.
         * @see @ref hasLayer()
         */
        template<class T> T attribute(Containers::StringView layer, UnsignedInt id) const;
        template<class T> T attribute(MaterialLayer layer, UnsignedInt id) const; /**< @overload */

        /**
         * @brief Mutable value of an attribute in a named material layer
         *
         * Like @ref attribute(Containers::StringView, UnsignedInt) const but
         * returns a mutable reference. Expects that the material is mutable.
         * In case of a string / buffer, you're expected to use
         * @relativeref{Corrade,Containers::MutableStringView} /
         * @relativeref{Corrade,Containers::ArrayView<void>} instead of
         * @relativeref{Corrade,Containers::StringView} /
         * @relativeref{Corrade,Containers::ArrayView<const void>} for @p T and
         * you get a @relativeref{Corrade,Containers::MutableStringView} /
         * @relativeref{Corrade,Containers::ArrayView<void>} back by-value, not
         * by-reference. Changing the string / buffer size is not possible.
         * @see @ref attributeDataFlags()
         */
        template<class T> typename std::conditional<std::is_same<T, Containers::MutableStringView>::value || std::is_same<T, Containers::ArrayView<void>>::value, T, T&>::type mutableAttribute(Containers::StringView layer, UnsignedInt id);
        template<class T> typename std::conditional<std::is_same<T, Containers::MutableStringView>::value || std::is_same<T, Containers::ArrayView<void>>::value, T, T&>::type mutableAttribute(MaterialLayer layer, UnsignedInt id); /**< @overload */

        /**
         * @brief Value of a named attribute in a named material layer
         *
         * The @p layer is expected to exist and @p name is expected to exist
         * in that layer. Expects that @p T corresponds to
         * @ref attributeType(Containers::StringView, Containers::StringView) const
         * for given @p layer and @p name. In case of a string, the returned
         * view always has @relativeref{Corrade,Containers::StringViewFlag::NullTerminated}
         * set.
         * @see @ref hasLayer(), @ref hasAttribute()
         */
        template<class T> T attribute(Containers::StringView layer, Containers::StringView name) const;
        template<class T> T attribute(Containers::StringView layer, MaterialAttribute name) const; /**< @overload */
        template<class T> T attribute(MaterialLayer layer, Containers::StringView name) const; /**< @overload */
        template<class T> T attribute(MaterialLayer layer, MaterialAttribute name) const; /**< @overload */

        /**
         * @brief Mutable value of a named attribute in a named material layer
         *
         * Like @ref attribute(Containers::StringView, Containers::StringView) const
         * but returns a mutable reference. Expects that the material is
         * mutable. In case of a string / buffer, you're expected to use
         * @relativeref{Corrade,Containers::MutableStringView} /
         * @relativeref{Corrade,Containers::ArrayView<void>} instead of
         * @relativeref{Corrade,Containers::StringView} /
         * @relativeref{Corrade,Containers::ArrayView<const void>} for @p T and
         * you get a @relativeref{Corrade,Containers::MutableStringView} /
         * @relativeref{Corrade,Containers::ArrayView<void>} back by-value, not
         * by-reference. Changing the string / buffer size is not possible.
         * @see @ref attributeDataFlags()
         */
        template<class T> typename std::conditional<std::is_same<T, Containers::MutableStringView>::value || std::is_same<T, Containers::ArrayView<void>>::value, T, T&>::type mutableAttribute(Containers::StringView layer, Containers::StringView name);
        template<class T> typename std::conditional<std::is_same<T, Containers::MutableStringView>::value || std::is_same<T, Containers::ArrayView<void>>::value, T, T&>::type mutableAttribute(Containers::StringView layer, MaterialAttribute name); /**< @overload */
        template<class T> typename std::conditional<std::is_same<T, Containers::MutableStringView>::value || std::is_same<T, Containers::ArrayView<void>>::value, T, T&>::type mutableAttribute(MaterialLayer layer, Containers::StringView name); /**< @overload */
        template<class T> typename std::conditional<std::is_same<T, Containers::MutableStringView>::value || std::is_same<T, Containers::ArrayView<void>>::value, T, T&>::type mutableAttribute(MaterialLayer layer, MaterialAttribute name); /**< @overload */

        /**
         * @brief Value of an attribute in the base material
         *
         * Equivalent to calling @ref attribute(UnsignedInt, UnsignedInt) const
         * with @p layer set to @cpp 0 @ce.
         */
        template<class T> T attribute(UnsignedInt id) const {
            return attribute<T>(0, id);
        }

        /**
         * @brief Mutable value of an attribute in the base material
         *
         * Equivalent to calling @ref mutableAttribute(UnsignedInt, UnsignedInt)
         * with @p layer set to @cpp 0 @ce.
         */
        template<class T> typename std::conditional<std::is_same<T, Containers::MutableStringView>::value || std::is_same<T, Containers::ArrayView<void>>::value, T, T&>::type mutableAttribute(UnsignedInt id) {
            return mutableAttribute<T>(0, id);
        }

        /**
         * @brief Value of a named attribute in the base material
         *
         * Equivalent to calling @ref attribute(UnsignedInt, Containers::StringView) const
         * with @p layer set to @cpp 0 @ce.
         */
        template<class T> T attribute(Containers::StringView name) const {
            return attribute<T>(0, name);
        }
        template<class T> T attribute(MaterialAttribute name) const {
            return attribute<T>(0, name);
        } /**< @overload */

        /**
         * @brief Mutable value of a named attribute in the base material
         *
         * Equivalent to calling @ref mutableAttribute(UnsignedInt, Containers::StringView)
         * with @p layer set to @cpp 0 @ce.
         */
        template<class T> typename std::conditional<std::is_same<T, Containers::MutableStringView>::value || std::is_same<T, Containers::ArrayView<void>>::value, T, T&>::type mutableAttribute(Containers::StringView name) {
            return mutableAttribute<T>(0, name);
        }
        template<class T> typename std::conditional<std::is_same<T, Containers::MutableStringView>::value || std::is_same<T, Containers::ArrayView<void>>::value, T, T&>::type mutableAttribute(MaterialAttribute name) {
            return mutableAttribute<T>(0, name);
        } /**< @overload */

        /**
         * @brief Type-erased attribute value in given material layer, if exists
         *
         * Compared to @ref attribute(UnsignedInt, Containers::StringView name) const,
         * if @p name doesn't exist, returns @cpp nullptr @ce instead of
         * asserting. Expects that @p layer is smaller than @ref layerCount() const.
         * Cast the pointer to a concrete type based on @ref attributeType().
         * @see @ref hasAttribute(), @ref attributeOr()
         */
        const void* findAttribute(UnsignedInt layer, Containers::StringView name) const;
        const void* findAttribute(UnsignedInt layer, MaterialAttribute name) const; /**< @overload */

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief @copybrief findAttribute(UnsignedInt, Containers::StringView) const
         * @m_deprecated_since_latest Use @ref findAttribute(UnsignedInt, Containers::StringView) const
         *      instead.
         */
        CORRADE_DEPRECATED("use findAttribute() instead") const void* tryAttribute(UnsignedInt layer, Containers::StringView name) const {
            return findAttribute(layer, name);
        }

        /**
         * @brief @copybrief findAttribute(UnsignedInt, MaterialAttribute) const
         * @m_deprecated_since_latest Use @ref findAttribute(UnsignedInt, MaterialAttribute) const
         *      instead.
         */
        CORRADE_DEPRECATED("use findAttribute() instead") const void* tryAttribute(UnsignedInt layer, MaterialAttribute name) const {
            return findAttribute(layer, name);
        }
        #endif

        /**
         * @brief Type-erased attribute value in a named material layer, if exists
         *
         * Compared to @ref attribute(Containers::StringView, Containers::StringView name) const,
         * if @p name doesn't exist, returns @cpp nullptr @ce instead of
         * asserting. Expects that @p layer exists. Cast the pointer to a
         * concrete type based on @ref attributeType().
         * @see @ref hasLayer(), @ref hasAttribute(), @ref attributeOr()
         */
        const void* findAttribute(Containers::StringView layer, Containers::StringView name) const;
        const void* findAttribute(Containers::StringView layer, MaterialAttribute name) const; /**< @overload */
        const void* findAttribute(MaterialLayer layer, Containers::StringView name) const; /**< @overload */
        const void* findAttribute(MaterialLayer layer, MaterialAttribute name) const; /**< @overload */

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief @copybrief findAttribute(Containers::StringView, Containers::StringView) const
         * @m_deprecated_since_latest Use @ref findAttribute(Containers::StringView, Containers::StringView) const
         *      instead.
         */
        CORRADE_DEPRECATED("use findAttribute() instead") const void* tryAttribute(Containers::StringView layer, Containers::StringView name) const {
            return findAttribute(layer, name);
        }

        /**
         * @brief @copybrief findAttribute(Containers::StringView, MaterialAttribute) const
         * @m_deprecated_since_latest Use @ref findAttribute(Containers::StringView, MaterialAttribute) const
         *      instead.
         */
        CORRADE_DEPRECATED("use findAttribute() instead") const void* tryAttribute(Containers::StringView layer, MaterialAttribute name) const {
            return findAttribute(layer, name);
        }

        /**
         * @brief @copybrief findAttribute(MaterialLayer, Containers::StringView) const
         * @m_deprecated_since_latest Use @ref findAttribute(MaterialLayer, Containers::StringView) const
         *      instead.
         */
        CORRADE_DEPRECATED("use findAttribute() instead") const void* tryAttribute(MaterialLayer layer, Containers::StringView name) const {
            return findAttribute(layer, name);
        }

        /**
         * @brief @copybrief findAttribute(MaterialLayer, MaterialAttribute) const
         * @m_deprecated_since_latest Use @ref findAttribute(MaterialLayer, MaterialAttribute) const
         *      instead.
         */
        CORRADE_DEPRECATED("use findAttribute() instead") const void* tryAttribute(MaterialLayer layer, MaterialAttribute name) const {
            return findAttribute(layer, name);
        }
        #endif

        /**
         * @brief Value of a named attribute in given material layer, if exists
         *
         * Compared to @ref attribute(UnsignedInt, Containers::StringView name) const,
         * if @p name doesn't exist, returns
         * @relativeref{Corrade,Containers::NullOpt} instead of asserting.
         * Expects that @p layer is smaller than @ref layerCount() const and
         * that @p T corresponds to
         * @ref attributeType(UnsignedInt, Containers::StringView) const for
         * given @p layer and @p name.
         */
        template<class T> Containers::Optional<T> findAttribute(UnsignedInt layer, Containers::StringView name) const;
        template<class T> Containers::Optional<T> findAttribute(UnsignedInt layer, MaterialAttribute name) const; /**< @overload */

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief @copybrief findAttribute(UnsignedInt, Containers::StringView) const
         * @m_deprecated_since_latest Use @ref findAttribute(UnsignedInt, Containers::StringView) const
         *      instead.
         */
        template<class T> CORRADE_DEPRECATED("use findAttribute() instead") Containers::Optional<T> tryAttribute(UnsignedInt layer, Containers::StringView name) const {
            return findAttribute<T>(layer, name);
        }

        /**
         * @brief @copybrief findAttribute(UnsignedInt, MaterialAttribute) const
         * @m_deprecated_since_latest Use @ref findAttribute(UnsignedInt, MaterialAttribute) const
         *      instead.
         */
        template<class T> CORRADE_DEPRECATED("use findAttribute() instead") Containers::Optional<T> tryAttribute(UnsignedInt layer, MaterialAttribute name) const {
            return findAttribute<T>(layer, name);
        }
        #endif

        /**
         * @brief Value of a named attribute in a named material layer, if exists
         *
         * Compared to @ref attribute(Containers::StringView, Containers::StringView name) const,
         * if @p name doesn't exist, returns
         * @relativeref{Corrade,Containers::NullOpt} instead of asserting.
         * Expects that @p layer exists and that @p T corresponds to
         * @ref attributeType(Containers::StringView, Containers::StringView) const
         * for given @p layer and @p name.
         * @see @ref hasLayer()
         */
        template<class T> Containers::Optional<T> findAttribute(Containers::StringView layer, Containers::StringView name) const;
        template<class T> Containers::Optional<T> findAttribute(Containers::StringView layer, MaterialAttribute name) const; /**< @overload */
        template<class T> Containers::Optional<T> findAttribute(MaterialLayer layer, Containers::StringView name) const; /**< @overload */
        template<class T> Containers::Optional<T> findAttribute(MaterialLayer layer, MaterialAttribute name) const; /**< @overload */

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief @copybrief findAttribute(Containers::StringView, Containers::StringView) const
         * @m_deprecated_since_latest Use @ref findAttribute(Containers::StringView, Containers::StringView) const
         *      instead.
         */
        template<class T> CORRADE_DEPRECATED("use findAttribute() instead") Containers::Optional<T> tryAttribute(Containers::StringView layer, Containers::StringView name) const {
            return findAttribute<T>(layer, name);
        }

        /**
         * @brief @copybrief findAttribute(Containers::StringView, MaterialAttribute) const
         * @m_deprecated_since_latest Use @ref findAttribute(Containers::StringView, MaterialAttribute) const
         *      instead.
         */
        template<class T> CORRADE_DEPRECATED("use findAttribute() instead") Containers::Optional<T> tryAttribute(Containers::StringView layer, MaterialAttribute name) const {
            return findAttribute<T>(layer, name);
        }

        /**
         * @brief @copybrief findAttribute(UnsignedInt, Containers::StringView) const
         * @m_deprecated_since_latest Use @ref findAttribute(UnsignedInt, Containers::StringView) const
         *      instead.
         */
        template<class T> CORRADE_DEPRECATED("use findAttribute() instead") Containers::Optional<T> tryAttribute(MaterialLayer layer, Containers::StringView name) const {
            return findAttribute<T>(layer, name);
        }

        /**
         * @brief @copybrief findAttribute(UnsignedInt, MaterialAttribute) const
         * @m_deprecated_since_latest Use @ref findAttribute(UnsignedInt, MaterialAttribute) const
         *      instead.
         */
        template<class T> CORRADE_DEPRECATED("use findAttribute() instead") Containers::Optional<T> tryAttribute(MaterialLayer layer, MaterialAttribute name) const {
            return findAttribute<T>(layer, name);
        }
        #endif

        /**
         * @brief Type-erased attribute value in the base material, if exists
         *
         * Equivalent to calling @ref findAttribute(UnsignedInt, Containers::StringView) const
         * with @p layer set to @cpp 0 @ce.
         */
        const void* findAttribute(Containers::StringView name) const {
            return findAttribute(0, name);
        }
        const void* findAttribute(MaterialAttribute name) const {
            return findAttribute(0, name);
        } /**< @overload */

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief @copybrief findAttribute(Containers::StringView) const
         * @m_deprecated_since_latest Use @ref findAttribute(Containers::StringView) const
         *      instead.
         */
        CORRADE_DEPRECATED("use findAttribute() instead") const void* tryAttribute(Containers::StringView name) const {
            return findAttribute(name);
        }

        /**
         * @brief @copybrief findAttribute(MaterialAttribute) const
         * @m_deprecated_since_latest Use @ref findAttribute(MaterialAttribute) const
         *      instead.
         */
        CORRADE_DEPRECATED("use findAttribute() instead") const void* tryAttribute(MaterialAttribute name) const {
            return findAttribute(name);
        }
        #endif

        /**
         * @brief Value of a named attribute in the base material, if exists
         *
         * Equivalent to calling @ref findAttribute(UnsignedInt, Containers::StringView) const
         * with @p layer set to @cpp 0 @ce.
         */
        template<class T> Containers::Optional<T> findAttribute(Containers::StringView name) const {
            return findAttribute<T>(0, name);
        }
        template<class T> Containers::Optional<T> findAttribute(MaterialAttribute name) const {
            return findAttribute<T>(0, name);
        } /**< @overload */

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief @copybrief findAttribute(Containers::StringView) const
         * @m_deprecated_since_latest Use @ref findAttribute(Containers::StringView) const
         *      instead.
         */
        template<class T> CORRADE_DEPRECATED("use findAttribute() instead") Containers::Optional<T> tryAttribute(Containers::StringView name) const {
            return findAttribute<T>(name);
        }

        /**
         * @brief @copybrief findAttribute(MaterialAttribute) const
         * @m_deprecated_since_latest Use @ref findAttribute(MaterialAttribute) const
         *      instead.
         */
        template<class T> CORRADE_DEPRECATED("use findAttribute() instead") Containers::Optional<T> tryAttribute(MaterialAttribute name) const {
            return findAttribute<T>(name);
        }
        #endif

        /**
         * @brief Value of a named attribute in given layer or a default
         *
         * Compared to @ref attribute(UnsignedInt, Containers::StringView name) const,
         * if @p name doesn't exist, returns @p defaultValue instead of
         * asserting. Expects that @p layer is smaller than @ref layerCount()
         * const
         * and that @p T corresponds to @ref attributeType(UnsignedInt, Containers::StringView) const
         * for given @p layer and @p name.
         */
        template<class T> T attributeOr(UnsignedInt layer, Containers::StringView name, const T& defaultValue) const;
        template<class T> T attributeOr(UnsignedInt layer, MaterialAttribute name, const T& defaultValue) const; /**< @overload */

        /**
         * @brief Value of a named attribute in a named layer or a default
         *
         * Compared to @ref attribute(Containers::StringView, Containers::StringView name) const,
         * if @p name doesn't exist, returns @p defaultValue instead of
         * asserting. Expects that @p layer exists and that @p T corresponds to
         * @ref attributeType(Containers::StringView, Containers::StringView) const
         * for given @p layer and @p name.
         * @see @ref hasLayer()
         */
        template<class T> T attributeOr(Containers::StringView layer, Containers::StringView name, const T& defaultValue) const;
        template<class T> T attributeOr(Containers::StringView layer, MaterialAttribute name, const T& defaultValue) const; /**< @overload */
        template<class T> T attributeOr(MaterialLayer layer, Containers::StringView name, const T& defaultValue) const; /**< @overload */
        template<class T> T attributeOr(MaterialLayer layer, MaterialAttribute name, const T& defaultValue) const; /**< @overload */

        /**
         * @brief Value of a named attribute in the base material or a default
         *
         * Equivalent to calling @ref attributeOr(UnsignedInt, Containers::StringView, const T&) const
         * with @p layer set to @cpp 0 @ce.
         */
        template<class T> T attributeOr(Containers::StringView name, const T& defaultValue) const {
            return attributeOr<T>(0, name, defaultValue);
        }
        template<class T> T attributeOr(MaterialAttribute name, const T& defaultValue) const {
            return attributeOr<T>(0, name, defaultValue);
        }/**< @overload */

        /**
         * @brief Whether a material is double-sided
         *
         * Convenience access to the @ref MaterialAttribute::DoubleSided
         * attribute. If not present, the default is @cpp false @ce.
         */
        bool isDoubleSided() const;

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Material flags
         * @m_deprecated_since_latest The flags are no longer stored directly
         *      but generated on-the-fly from attribute data, which makes them
         *      less efficient than calling @ref hasAttribute(),
         *      @ref isDoubleSided() etc.
         *
         * Not all bits returned might be defined by @ref Flag, subclasses may
         * define extra values.
         */
        CORRADE_IGNORE_DEPRECATED_PUSH /* GCC warns about Flags, ugh */
        CORRADE_DEPRECATED("use hasAttribute() instead") Flags flags() const;
        CORRADE_IGNORE_DEPRECATED_POP
        #endif

        /**
         * @brief Alpha mode
         *
         * Convenience access to @ref MaterialAttribute::AlphaBlend and
         * @ref MaterialAttribute::AlphaMask attributes. If neither is present,
         * the default is @ref MaterialAlphaMode::Opaque.
         */
        MaterialAlphaMode alphaMode() const;

        /**
         * @brief Alpha mask
         *
         * Convenience access to the @ref MaterialAttribute::AlphaMask
         * attribute. If not present, the default is @cpp 0.5f @ce.
         */
        Float alphaMask() const;

        /**
         * @brief Release layer data storage
         *
         * Releases the ownership of the layer offset array and resets internal
         * layer-related state to default. The material then behaves like if it
         * has no layers. Note that the returned array has a custom no-op
         * deleter when the data are not owned by the material, and while the
         * returned array type is mutable, the actual memory might be not.
         *
         * @attention Querying attributes after calling @ref releaseLayerData()
         *      has undefined behavior and might lead to crashes. This is done
         *      intentionally in order to simplify the interaction between this
         *      function and @ref releaseAttributeData().
         * @see @ref layerData(), @ref layerDataFlags()
         */
        Containers::Array<UnsignedInt> releaseLayerData();

        /**
         * @brief Release attribute data storage
         *
         * Releases the ownership of the attribute array and resets internal
         * attribute-related state to default. The material then behaves like
         * if it has no attributes. Note that the returned array has a custom
         * no-op deleter when the data are not owned by the material, and while
         * the returned array type is mutable, the actual memory might be not.
         *
         * @attention Querying layers after calling @ref releaseAttributeData()
         *      has undefined behavior and might lead to crashes. This is done
         *      intentionally in order to simplify the interaction between this
         *      function and @ref releaseLayerData().
         * @see @ref attributeData(), @ref attributeDataFlags()
         */
        Containers::Array<MaterialAttributeData> releaseAttributeData();

        /**
         * @brief Importer-specific state
         *
         * See @ref AbstractImporter::importerState() for more information.
         */
        const void* importerState() const { return _importerState; }

    private:
        /* For custom deleter checks. Not done in the constructors here because
           the restriction is pointless when used outside of plugin
           implementations. */
        friend AbstractImporter;

        /* Internal helpers that don't assert, unlike layerId() / attributeId() */
        UnsignedInt findLayerIdInternal(Containers::StringView layer) const;
        UnsignedInt layerOffset(UnsignedInt layer) const {
            return layer && _layerOffsets ? _layerOffsets[layer - 1] : 0;
        }
        UnsignedInt findAttributeIdInternal(UnsignedInt layer, Containers::StringView name) const;

        Containers::Array<MaterialAttributeData> _data;
        Containers::Array<UnsignedInt> _layerOffsets;
        MaterialTypes _types;
        DataFlags _attributeDataFlags, _layerDataFlags;
        /* 2 bytes free */
        const void* _importerState;
};

#ifdef MAGNUM_BUILD_DEPRECATED
CORRADE_IGNORE_DEPRECATED_PUSH
CORRADE_ENUMSET_OPERATORS(MaterialData::Flags)

/**
@debugoperatorclassenum{MaterialData,MaterialData::Flag}
@m_deprecated_since_latest The flags are no longer stored directly but
    generated on-the-fly from attribute data, which makes them less efficient
    than calling @ref MaterialData::hasAttribute(),
    @ref MaterialData::isDoubleSided() etc.
*/
/* Not marked with CORRADE_DEPRECATED() as there's enough warnings already */
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, MaterialData::Flag value);

/**
@debugoperatorclassenum{MaterialData,MaterialData::Flags}
@m_deprecated_since_latest The flags are no longer stored directly but
    generated on-the-fly from attribute data, which makes them less efficient
    than calling @ref MaterialData::hasAttribute(),
    @ref MaterialData::isDoubleSided() etc.
*/
/* Not marked with CORRADE_DEPRECATED() as there's enough warnings already */
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, MaterialData::Flags value);
CORRADE_IGNORE_DEPRECATED_POP
#endif

namespace Implementation {
    /* LCOV_EXCL_START */
    /* Has to be a struct because there can't be partial specializations for a
       function (which we need for pointers) */
    template<class T> struct MaterialAttributeTypeFor {
        /* C++ why there isn't an obvious way to do such a thing?! */
        static_assert(sizeof(T) == 0, "unsupported attribute type");
    };
    template<> struct MaterialAttributeTypeFor<bool> {
        constexpr static MaterialAttributeType type() {
            return MaterialAttributeType::Bool;
        }
    };
    template<class T> struct MaterialAttributeTypeFor<const T*> {
        constexpr static MaterialAttributeType type() {
            return MaterialAttributeType::Pointer;
        }
    };
    template<class T> struct MaterialAttributeTypeFor<T*> {
        constexpr static MaterialAttributeType type() {
            return MaterialAttributeType::MutablePointer;
        }
    };
    /* No specialization for StringView as this type trait should not be used
       in that case */
    template<> struct MaterialAttributeTypeFor<MaterialTextureSwizzle> {
        constexpr static MaterialAttributeType type() {
            return MaterialAttributeType::TextureSwizzle;
        }
    };
    #ifndef DOXYGEN_GENERATING_OUTPUT
    #define _c(type_) template<> struct MaterialAttributeTypeFor<type_> {   \
        constexpr static MaterialAttributeType type() {                     \
            return MaterialAttributeType::type_;                            \
        }                                                                   \
    };
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
    #undef _c
    #endif
    template<> struct MaterialAttributeTypeFor<Color3>: MaterialAttributeTypeFor<Vector3> {};
    template<> struct MaterialAttributeTypeFor<Color4>: MaterialAttributeTypeFor<Vector4> {};
    template<> struct MaterialAttributeTypeFor<Matrix3>: MaterialAttributeTypeFor<Matrix3x3> {};
    /* LCOV_EXCL_STOP */
}

/* The 2 extra bytes are for a null byte after the name and a type */
template<class T
    #ifndef DOXYGEN_GENERATING_OUTPUT
    , typename std::enable_if<!std::is_convertible<const T&, Containers::StringView>::value && !std::is_convertible<const T&, Containers::ArrayView<const void>>::value, int>::type
    #endif
> constexpr MaterialAttributeData::MaterialAttributeData(const Containers::StringView name, const T& value) noexcept:
    _data{Implementation::MaterialAttributeTypeFor<T>::type(), (
        /* It would sort before " LayerName" and that's not desirable */
        CORRADE_CONSTEXPR_ASSERT(!name.isEmpty(), "Trade::MaterialAttributeData: name is not allowed to be empty"),
        /* MSVC 2015 complains about "error C2065: 'T': undeclared identifier"
           in the lambda inside this macro. Sorry, the assert will be less
           useful on that stupid thing. */
        #ifndef CORRADE_MSVC2015_COMPATIBILITY
        CORRADE_CONSTEXPR_ASSERT(name.size() + sizeof(T) + 2 <= Implementation::MaterialAttributeDataSize, "Trade::MaterialAttributeData: name" << name << "too long, expected at most" << Implementation::MaterialAttributeDataSize - sizeof(T) - 2 << "bytes for" << Implementation::MaterialAttributeTypeFor<T>::type() << "but got" << name.size()),
        #else
        CORRADE_CONSTEXPR_ASSERT(name.size() + sizeof(T) + 2 <= Implementation::MaterialAttributeDataSize, "Trade::MaterialAttributeData: name" << name << "too long, got" << name.size() << "bytes"),
        #endif
    name), value} {}

/* The 4 extra bytes are for a null byte after both the name and value, a type
   and a string size */
constexpr MaterialAttributeData::MaterialAttributeData(const Containers::StringView name, const Containers::StringView value) noexcept: _data{(
        /* It would sort before " LayerName" and that's not desirable */
        CORRADE_CONSTEXPR_ASSERT(!name.isEmpty(), "Trade::MaterialAttributeData: name is not allowed to be empty"),
        CORRADE_CONSTEXPR_ASSERT(name.size() + value.size() + 4 <= Implementation::MaterialAttributeDataSize, "Trade::MaterialAttributeData: name" << name << "and value" << value << "too long, expected at most" << Implementation::MaterialAttributeDataSize - 4 << "bytes in total but got" << name.size() + value.size()),
    name), value} {}

template<class T> T MaterialAttributeData::value() const {
    CORRADE_ASSERT(Implementation::MaterialAttributeTypeFor<T>::type() == _data.type,
        "Trade::MaterialAttributeData::value():" << (_data.data + 1) << "is" << _data.type << "but requested a type equivalent to" << Implementation::MaterialAttributeTypeFor<T>::type(), {});
    return *reinterpret_cast<const T*>(value());
}

#ifndef DOXYGEN_GENERATING_OUTPUT
template<> Containers::StringView MaterialAttributeData::value<Containers::StringView>() const;
template<> Containers::ArrayView<const void> MaterialAttributeData::value<Containers::ArrayView<const void>>() const;
#endif

template<class T> T MaterialData::attribute(const UnsignedInt layer, const UnsignedInt id) const {
    const void* const value = attribute(layer, id);
    #ifdef CORRADE_GRACEFUL_ASSERT
    if(!value) return {};
    #endif
    #ifndef CORRADE_NO_ASSERT
    const Trade::MaterialAttributeData& data = _data[layerOffset(layer) + id];
    #endif
    CORRADE_ASSERT(Implementation::MaterialAttributeTypeFor<T>::type() == data._data.type,
        "Trade::MaterialData::attribute():" << (data._data.data + 1) << "is" << data._data.type << "but requested a type equivalent to" << Implementation::MaterialAttributeTypeFor<T>::type(), {});
    return *reinterpret_cast<const T*>(value);
}

template<class T> typename std::conditional<std::is_same<T, Containers::MutableStringView>::value || std::is_same<T, Containers::ArrayView<void>>::value, T, T&>::type MaterialData::mutableAttribute(const UnsignedInt layer, const UnsignedInt id) {
    void* const value = mutableAttribute(layer, id);
    #ifdef CORRADE_GRACEFUL_ASSERT
    if(!value) return *reinterpret_cast<T*>(this);
    #endif
    #ifndef CORRADE_NO_ASSERT
    const Trade::MaterialAttributeData& data = _data[layerOffset(layer) + id];
    #endif
    CORRADE_ASSERT(Implementation::MaterialAttributeTypeFor<T>::type() == data._data.type,
        "Trade::MaterialData::mutableAttribute():" << (data._data.data + 1) << "is" << data._data.type << "but requested a type equivalent to" << Implementation::MaterialAttributeTypeFor<T>::type(), *reinterpret_cast<T*>(this));
    return *reinterpret_cast<T*>(value);
}

#ifndef DOXYGEN_GENERATING_OUTPUT
template<> Containers::StringView MaterialData::attribute<Containers::StringView>(UnsignedInt, UnsignedInt) const;
template<> Containers::MutableStringView MaterialData::mutableAttribute<Containers::MutableStringView>(UnsignedInt, UnsignedInt);
template<> Containers::ArrayView<const void> MaterialData::attribute<Containers::ArrayView<const void>>(UnsignedInt, UnsignedInt) const;
template<> Containers::ArrayView<void> MaterialData::mutableAttribute<Containers::ArrayView<void>>(UnsignedInt, UnsignedInt);
#endif

template<class T> T MaterialData::attribute(const UnsignedInt layer, const Containers::StringView name) const {
    CORRADE_ASSERT(layer < layerCount(),
        "Trade::MaterialData::attribute(): index" << layer << "out of range for" << layerCount() << "layers", {});
    const UnsignedInt id = findAttributeIdInternal(layer, name);
    CORRADE_ASSERT(id != ~UnsignedInt{},
        "Trade::MaterialData::attribute(): attribute" << name << "not found in layer" << layer, {});
    return attribute<T>(layer, id);
}

template<class T> typename std::conditional<std::is_same<T, Containers::MutableStringView>::value || std::is_same<T, Containers::ArrayView<void>>::value, T, T&>::type MaterialData::mutableAttribute(const UnsignedInt layer, const Containers::StringView name) {
    CORRADE_ASSERT(layer < layerCount(),
        "Trade::MaterialData::mutableAttribute(): index" << layer << "out of range for" << layerCount() << "layers", *reinterpret_cast<T*>(this));
    const UnsignedInt id = findAttributeIdInternal(layer, name);
    CORRADE_ASSERT(id != ~UnsignedInt{},
        "Trade::MaterialData::mutableAttribute(): attribute" << name << "not found in layer" << layer, *reinterpret_cast<T*>(this));
    return mutableAttribute<T>(layer, id);
}

template<class T> T MaterialData::attribute(const UnsignedInt layer, const MaterialAttribute name) const {
    const Containers::StringView string = Implementation::materialAttributeNameInternal(name);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::attribute(): invalid name" << name, {});
    return attribute<T>(layer, string);
}

template<class T> typename std::conditional<std::is_same<T, Containers::MutableStringView>::value || std::is_same<T, Containers::ArrayView<void>>::value, T, T&>::type MaterialData::mutableAttribute(const UnsignedInt layer, const MaterialAttribute name) {
    const Containers::StringView string = Implementation::materialAttributeNameInternal(name);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::mutableAttribute(): invalid name" << name, *reinterpret_cast<T*>(this));
    return mutableAttribute<T>(layer, string);
}

template<class T> T MaterialData::attribute(const Containers::StringView layer, const UnsignedInt id) const {
    const UnsignedInt layerId = findLayerIdInternal(layer);
    CORRADE_ASSERT(layerId != ~UnsignedInt{},
        "Trade::MaterialData::attribute(): layer" << layer << "not found", {});
    CORRADE_ASSERT(id < attributeCount(layer),
        "Trade::MaterialData::attribute(): index" << id << "out of range for" << attributeCount(layer) << "attributes in layer" << layer, {});
    return attribute<T>(layerId, id);
}

template<class T> typename std::conditional<std::is_same<T, Containers::MutableStringView>::value || std::is_same<T, Containers::ArrayView<void>>::value, T, T&>::type MaterialData::mutableAttribute(const Containers::StringView layer, const UnsignedInt id) {
    const UnsignedInt layerId = findLayerIdInternal(layer);
    CORRADE_ASSERT(layerId != ~UnsignedInt{},
        "Trade::MaterialData::mutableAttribute(): layer" << layer << "not found", *reinterpret_cast<T*>(this));
    CORRADE_ASSERT(id < attributeCount(layer),
        "Trade::MaterialData::mutableAttribute(): index" << id << "out of range for" << attributeCount(layer) << "attributes in layer" << layer, *reinterpret_cast<T*>(this));
    return mutableAttribute<T>(layerId, id);
}

template<class T> T MaterialData::attribute(const Containers::StringView layer, const Containers::StringView name) const {
    const UnsignedInt layerId = findLayerIdInternal(layer);
    CORRADE_ASSERT(layerId != ~UnsignedInt{},
        "Trade::MaterialData::attribute(): layer" << layer << "not found", {});
    const UnsignedInt id = findAttributeIdInternal(layerId, name);
    CORRADE_ASSERT(id != ~UnsignedInt{},
        "Trade::MaterialData::attribute(): attribute" << name << "not found in layer" << layer, {});
    return attribute<T>(layerId, id);
}

template<class T> typename std::conditional<std::is_same<T, Containers::MutableStringView>::value || std::is_same<T, Containers::ArrayView<void>>::value, T, T&>::type MaterialData::mutableAttribute(const Containers::StringView layer, const Containers::StringView name) {
    const UnsignedInt layerId = findLayerIdInternal(layer);
    CORRADE_ASSERT(layerId != ~UnsignedInt{},
        "Trade::MaterialData::mutableAttribute(): layer" << layer << "not found", *reinterpret_cast<T*>(this));
    const UnsignedInt id = findAttributeIdInternal(layerId, name);
    CORRADE_ASSERT(id != ~UnsignedInt{},
        "Trade::MaterialData::mutableAttribute(): attribute" << name << "not found in layer" << layer, *reinterpret_cast<T*>(this));
    return mutableAttribute<T>(layerId, id);
}

template<class T> T MaterialData::attribute(const Containers::StringView layer, const MaterialAttribute name) const {
    const Containers::StringView string = Implementation::materialAttributeNameInternal(name);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::attribute(): invalid name" << name, {});
    return attribute<T>(layer, string);
}

template<class T> typename std::conditional<std::is_same<T, Containers::MutableStringView>::value || std::is_same<T, Containers::ArrayView<void>>::value, T, T&>::type MaterialData::mutableAttribute(const Containers::StringView layer, const MaterialAttribute name) {
    const Containers::StringView string = Implementation::materialAttributeNameInternal(name);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::mutableAttribute(): invalid name" << name, *reinterpret_cast<T*>(this));
    return mutableAttribute<T>(layer, string);
}

template<class T> T MaterialData::attribute(const MaterialLayer layer, const UnsignedInt id) const {
    const Containers::StringView string = Implementation::materialLayerNameInternal(layer);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::attribute(): invalid name" << layer, {});
    return attribute<T>(string, id);
}

template<class T> typename std::conditional<std::is_same<T, Containers::MutableStringView>::value || std::is_same<T, Containers::ArrayView<void>>::value, T, T&>::type MaterialData::mutableAttribute(const MaterialLayer layer, const UnsignedInt id) {
    const Containers::StringView string = Implementation::materialLayerNameInternal(layer);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::mutableAttribute(): invalid name" << layer, *reinterpret_cast<T*>(this));
    return mutableAttribute<T>(string, id);
}

template<class T> T MaterialData::attribute(const MaterialLayer layer, const Containers::StringView name) const {
    const Containers::StringView string = Implementation::materialLayerNameInternal(layer);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::attribute(): invalid name" << layer, {});
    return attribute<T>(string, name);
}

template<class T> typename std::conditional<std::is_same<T, Containers::MutableStringView>::value || std::is_same<T, Containers::ArrayView<void>>::value, T, T&>::type MaterialData::mutableAttribute(const MaterialLayer layer, const Containers::StringView name) {
    const Containers::StringView string = Implementation::materialLayerNameInternal(layer);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::mutableAttribute(): invalid name" << layer, *reinterpret_cast<T*>(this));
    return mutableAttribute<T>(string, name);
}

template<class T> T MaterialData::attribute(const MaterialLayer layer, const MaterialAttribute name) const {
    const Containers::StringView string = Implementation::materialLayerNameInternal(layer);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::attribute(): invalid name" << layer, {});
    return attribute<T>(string, name);
}

template<class T> typename std::conditional<std::is_same<T, Containers::MutableStringView>::value || std::is_same<T, Containers::ArrayView<void>>::value, T, T&>::type MaterialData::mutableAttribute(const MaterialLayer layer, const MaterialAttribute name) {
    const Containers::StringView string = Implementation::materialLayerNameInternal(layer);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::mutableAttribute(): invalid name" << layer, *reinterpret_cast<T*>(this));
    return mutableAttribute<T>(string, name);
}

template<class T> Containers::Optional<T> MaterialData::findAttribute(const UnsignedInt layer, const Containers::StringView name) const {
    CORRADE_ASSERT(layer < layerCount(),
        "Trade::MaterialData::findAttribute(): index" << layer << "out of range for" << layerCount() << "layers", {});
    const UnsignedInt id = findAttributeIdInternal(layer, name);
    if(id == ~UnsignedInt{}) return {};
    return attribute<T>(layer, id);
}

template<class T> Containers::Optional<T> MaterialData::findAttribute(const UnsignedInt layer, const MaterialAttribute name) const {
    const Containers::StringView string = Implementation::materialAttributeNameInternal(name);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::findAttribute(): invalid name" << name, {});
    return findAttribute<T>(layer, string);
}

template<class T> Containers::Optional<T> MaterialData::findAttribute(const Containers::StringView layer, const Containers::StringView name) const {
    const UnsignedInt layerId = findLayerIdInternal(layer);
    CORRADE_ASSERT(layerId != ~UnsignedInt{},
        "Trade::MaterialData::findAttribute(): layer" << layer << "not found", {});
    return findAttribute<T>(layerId, name);
}

template<class T> Containers::Optional<T> MaterialData::findAttribute(const Containers::StringView layer, const MaterialAttribute name) const {
    const Containers::StringView string = Implementation::materialAttributeNameInternal(name);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::findAttribute(): invalid name" << name, {});
    return findAttribute<T>(layer, string);
}

template<class T> Containers::Optional<T> MaterialData::findAttribute(const MaterialLayer layer, const Containers::StringView name) const {
    const Containers::StringView string = Implementation::materialLayerNameInternal(layer);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::findAttribute(): invalid name" << layer, {});
    return findAttribute<T>(string, name);
}

template<class T> Containers::Optional<T> MaterialData::findAttribute(const MaterialLayer layer, const MaterialAttribute name) const {
    const Containers::StringView string = Implementation::materialLayerNameInternal(layer);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::findAttribute(): invalid name" << layer, {});
    return findAttribute<T>(string, name);
}

template<class T> T MaterialData::attributeOr(const UnsignedInt layer, const Containers::StringView name, const T& defaultValue) const {
    CORRADE_ASSERT(layer < layerCount(),
        "Trade::MaterialData::attributeOr(): index" << layer << "out of range for" << layerCount() << "layers", {});
    const UnsignedInt id = findAttributeIdInternal(layer, name);
    if(id == ~UnsignedInt{}) return defaultValue;
    return attribute<T>(layer, id);
}

template<class T> T MaterialData::attributeOr(const UnsignedInt layer, const MaterialAttribute name, const T& defaultValue) const {
    const Containers::StringView string = Implementation::materialAttributeNameInternal(name);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::attributeOr(): invalid name" << name, {});
    return attributeOr<T>(layer, string, defaultValue);
}

template<class T> T MaterialData::attributeOr(const Containers::StringView layer, const Containers::StringView name, const T& defaultValue) const {
    const UnsignedInt layerId = findLayerIdInternal(layer);
    CORRADE_ASSERT(layerId != ~UnsignedInt{},
        "Trade::MaterialData::attributeOr(): layer" << layer << "not found", {});
    return attributeOr<T>(layerId, name, defaultValue);
}

template<class T> T MaterialData::attributeOr(const Containers::StringView layer, const MaterialAttribute name, const T& defaultValue) const {
    const Containers::StringView string = Implementation::materialAttributeNameInternal(name);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::attributeOr(): invalid name" << name, {});
    return attributeOr<T>(layer, string, defaultValue);
}

template<class T> T MaterialData::attributeOr(const MaterialLayer layer, const Containers::StringView name, const T& defaultValue) const {
    const Containers::StringView string = Implementation::materialLayerNameInternal(layer);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::attributeOr(): invalid name" << layer, {});
    return attributeOr<T>(string, name, defaultValue);
}

template<class T> T MaterialData::attributeOr(const MaterialLayer layer, const MaterialAttribute name, const T& defaultValue) const {
    const Containers::StringView string = Implementation::materialLayerNameInternal(layer);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::attributeOr(): invalid name" << layer, {});
    return attributeOr<T>(string, name, defaultValue);
}

}}

#endif
