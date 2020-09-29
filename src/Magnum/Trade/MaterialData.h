#ifndef Magnum_Trade_MaterialData_h
#define Magnum_Trade_MaterialData_h
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
#include "Magnum/Trade/Trade.h"
#include "Magnum/Trade/visibility.h"

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
@see @ref MaterialData, @ref MaterialData::layerName(), @ref MaterialLayerData
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
only exception is @ref MaterialAttribute::LayerName which is @cpp "$LayerName" @ce.

When this enum si used in
@ref MaterialAttributeData constructors, the data are additionally checked for
type compatibility. Other than that, there is no difference to the string
variants.
@see @ref MaterialAttributeData, @ref MaterialData
*/
enum class MaterialAttribute: UnsignedInt {
    /* Zero used for an invalid value */

    /**
     * Layer name, @ref MaterialAttributeType::String.
     *
     * Unlike other attributes where string name matches the enum name, in this
     * case the corresponding string is @cpp "$LayerName" @ce, done in order to
     * have the layer name attribute appear first in each layer and thus
     * simplify layer implementation.
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
     * @see @ref MaterialAlphaMode, @ref MaterialData::alphaMode()
     */
    AlphaBlend,

    /**
     * Double sided, @ref MaterialAttributeType::Bool.
     *
     * If not present, the default value is @cpp false @ce.
     * @see @ref MaterialData::isDoubleSided()
     */
    DoubleSided,

    /**
     * Ambient color for Phong materials, @ref MaterialAttributeType::Vector4.
     *
     * If @ref MaterialAttribute::AmbientTexture is present as well, these two
     * are multiplied together.
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
     * @see @ref PhongMaterialData::ambientTextureMatrix()
     */
    AmbientTextureMatrix,

    /**
     * Ambient texture coordinate set index for Phong materials,
     * @ref MaterialAttributeType::UnsignedInt.
     *
     * Has a precedence over @ref MaterialAttribute::TextureCoordinates if both
     * are present.
     * @see @ref PhongMaterialData::ambientTextureCoordinates()
     */
    AmbientTextureCoordinates,

    /**
     * Diffuse color for Phong or PBR specular/glossiness materials,
     * @ref MaterialAttributeType::Vector4.
     *
     * If @ref MaterialAttribute::DiffuseTexture is present as well, these two
     * are multiplied together.
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
     * @see @ref FlatMaterialData::textureCoordinates(),
     *      @ref PhongMaterialData::diffuseTextureCoordinates(),
     *      @ref PbrSpecularGlossinessMaterialData::diffuseTextureCoordinates()
     */
    DiffuseTextureCoordinates,

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
     * (which is the default) is expected. Does not apply to
     * @ref MaterialAttribute::SpecularGlossinessTexture --- in that case,
     * the specular texture is always three-channel, regardless of this
     * attribute.
     * @see @ref PbrSpecularGlossinessMaterialData::specularTextureSwizzle()
     */
    SpecularTextureSwizzle,

    /**
     * Specular texture transformation matrix for Phong or PBR
     * specular/glossiness materials, @ref MaterialAttributeType::Matrix3x3.
     *
     * Has a precedence over @ref MaterialAttribute::TextureMatrix if both are
     * present.
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
     * @see @ref PhongMaterialData::specularTextureCoordinates(),
     *      @ref PbrSpecularGlossinessMaterialData::specularTextureCoordinates()
     */
    SpecularTextureCoordinates,

    /**
     * Shininess value for Phong materials, @ref MaterialAttributeType::Float.
     *
     * @see @ref PhongMaterialData::shininess()
     */
    Shininess,

    /**
     * Base color for PBR metallic/roughness materials,
     * @ref MaterialAttributeType::Vector4.
     *
     * If @ref MaterialAttribute::BaseColorTexture is present as well, these
     * two are multiplied together.
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
     * @see @ref FlatMaterialData::textureCoordinates(),
     *      @ref PbrMetallicRoughnessMaterialData::baseColorTextureCoordinates()
     */
    BaseColorTextureCoordinates,

    /**
     * Metalness for PBR metallic/roughness materials,
     * @ref MaterialAttributeType::Float.
     *
     * If @ref MaterialAttribute::MetalnessTexture or
     * @ref MaterialAttribute::NoneRoughnessMetallicTexture is present as well,
     * these two are multiplied together.
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
     * single texture. A single-channel swizzle value is expected. If not
     * present, @ref MaterialTextureSwizzle::R is assumed. Does not apply to
     * @ref MaterialAttribute::NoneRoughnessMetallicTexture --- in that case,
     * the metalness is implicitly in the red channel regardless of this
     * attribute.
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
     * @see @ref PbrMetallicRoughnessMaterialData::metalnessTextureMatrix()
     */
    MetalnessTextureMatrix,

    /**
     * Metalness texture coordinate set index for PBR metallic/roughness
     * materials, @ref MaterialAttributeType::UnsignedInt.
     *
     * Has a precedence over @ref MaterialAttribute::TextureCoordinates if both
     * are present.
     * @see @ref PbrMetallicRoughnessMaterialData::metalnessTextureCoordinates()
     */
    MetalnessTextureCoordinates,

    /**
     * Roughness for PBR metallic/roughness materials,
     * @ref MaterialAttributeType::Float.
     *
     * If @ref MaterialAttribute::RoughnessTexture or
     * @ref MaterialAttribute::NoneRoughnessMetallicTexture is present as well,
     * these two are multiplied together.
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
     * single texture. A single-channel swizzle value is expected. If not
     * present, @ref MaterialTextureSwizzle::R is assumed. Does not apply to
     * @ref MaterialAttribute::NoneRoughnessMetallicTexture --- in that case,
     * the metalness is implicitly in the green channel regardless of this
     * attribute.
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
     * @see @ref PbrMetallicRoughnessMaterialData::roughnessTextureMatrix()
     */
    RoughnessTextureMatrix,

    /**
     * Roughness texture coordinate set index for PBR metallic/roughness
     * materials, @ref MaterialAttributeType::UnsignedInt.
     *
     * Has a precedence over @ref MaterialAttribute::TextureCoordinates if both
     * are present.
     * @see @ref PbrMetallicRoughnessMaterialData::roughnessTextureCoordinates()
     */
    RoughnessTextureCoordinates,

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
       DiffuseTextureCoordinates, SpecularColor, SpecularTexture,
       SpecularTextureSwizzle, SpecularTextureMatrix,
       SpecularTextureCoordinates specified above for Phong already */

    /**
     * Glossiness for PBR specular/glossiness materials,
     * @ref MaterialAttributeType::Float.
     *
     * If @ref MaterialAttribute::GlossinessTexture or
     * @ref MaterialAttribute::SpecularGlossinessTexture is present as well,
     * these two are multiplied together.
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
     * single texture. A single-channel swizzle value is expected. If not
     * present, @ref MaterialTextureSwizzle::R is assumed. Does not apply to
     * @ref MaterialAttribute::SpecularGlossinessTexture --- in that case,
     * the glossiness is implicitly in the alpha channel regardless of this
     * attribute.
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
     * @see @ref PbrSpecularGlossinessMaterialData::glossinessTextureMatrix()
     */
    GlossinessTextureMatrix,

    /**
     * Glossiness texture coordinate set index for PBR specular/glossiness
     * materials, @ref MaterialAttributeType::UnsignedInt.
     *
     * Has a precedence over @ref MaterialAttribute::TextureCoordinates if both
     * are present.
     * @see @ref PbrSpecularGlossinessMaterialData::glossinessTextureCoordinates()
     */
    GlossinessTextureCoordinates,

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
     * single texture. A two- or three-channel swizzle value is expected. If
     * not present, @ref MaterialTextureSwizzle::RGB is assumed.
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
     * @snippet MagnumTrade.glsl unpackTwoChannelNormal
     *
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
     * @see @ref PhongMaterialData::normalTextureCoordinates(),
     *      @ref PbrMetallicRoughnessMaterialData::normalTextureCoordinates(),
     *      @ref PbrSpecularGlossinessMaterialData::normalTextureCoordinates()
     */
    NormalTextureCoordinates,

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
     * @see @ref PbrMetallicRoughnessMaterialData::occlusionTextureStrength(),
     *      @ref PbrSpecularGlossinessMaterialData::occlusionTextureStrength()
     */
    OcclusionTextureStrength,

    /**
     * Occlusion texture swizzle, @ref MaterialAttributeType::TextureSwizzle.
     *
     * Can be used to express arbitrary packing together with other maps in a
     * single texture. A single-channel swizzle value is expected. If
     * not present, @ref MaterialTextureSwizzle::R is assumed.
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
     * @see @ref PbrMetallicRoughnessMaterialData::occlusionTextureCoordinates(),
     *      @ref PbrSpecularGlossinessMaterialData::occlusionTextureCoordinates()
     */
    OcclusionTextureCoordinates,

    /**
     * Emissive color,
     * @ref MaterialAttributeType::Vector3.
     *
     * If @ref MaterialAttribute::EmissiveTexture is present as well, these two
     * are multiplied together.
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
     * @see @ref PbrMetallicRoughnessMaterialData::emissiveTextureCoordinates(),
     *      @ref PbrSpecularGlossinessMaterialData::emissiveTextureCoordinates()
     */
    EmissiveTextureCoordinates,

    /**
     * Layer intensity. @ref MaterialAttributeType::Float.
     *
     * Expected to be contained in additional layers, not the base material.
     * The exact semantic of this attribute is layer-specific. If
     * @ref MaterialAttribute::LayerFactorTexture is present as well, these two
     * are multiplied together.
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
     * single texture. A single-channel swizzle value is expected. If not
     * present, @ref MaterialTextureSwizzle::R is assumed.
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
     * @see @ref MaterialData::layerFactorTextureCoordinates()
     */
    LayerFactorTextureCoordinates,

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
};

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

    /* Matrix4x4 not present */

    /**
     * @cpp const void* @ce, type is not preserved. For convenience it's
     * possible to retrieve the value by calling @cpp attribute<const T>() @ce
     * with an arbitrary `T` but the user has to ensure the type is correct.
     */
    Pointer,

    /**
     * @cpp void* @ce, type is not preserved. For convenience it's possible to
     * retrieve the value by calling @cpp attribute<T>() @ce with an arbitrary
     * `T` but the user has to ensure the type is correct.
     */
    MutablePointer,

    /**
     * Null-terminated string. Can be stored using any type convertible to
     * @ref Corrade::Containers::StringView, retrieval has to be done using
     * @ref Corrade::Containers::StringView.
     */
    String,

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
         * The @p name together with @p value is expected to fit into 62 bytes.
         * @ref MaterialAttributeType is inferred from the type passed.
         *
         * This function is useful in @cpp constexpr @ce contexts and for
         * creating custom material attributes. For known attributes prefer to
         * use @ref MaterialAttributeData(MaterialAttribute, const T&) if you
         * don't need @cpp constexpr @ce, as it additionally checks that given
         * attribute has the expected type.
         */
        template<class T
            #ifndef DOXYGEN_GENERATING_OUTPUT
            , class = typename std::enable_if<!std::is_convertible<const T&, Containers::StringView>::value>::type
            #endif
        > constexpr /*implicit*/ MaterialAttributeData(Containers::StringView name, const T& value) noexcept;

        /**
         * @brief Construct with a string name and string value
         * @param name      Attribute name
         * @param value     Attribute value
         *
         * The combined length of @p name and @p value is expected to fit into
         * 61 bytes. Type is set to @ref MaterialAttributeType::String.
         *
         * This function is useful in @cpp constexpr @ce contexts and for
         * creating custom material attributes. For known attributes prefer to
         * use @ref MaterialAttributeData(MaterialAttribute, const T&) if you
         * don't need @cpp constexpr @ce, as it additionally checks that given
         * attribute has the expected type.
         */
        constexpr /*implicit*/ MaterialAttributeData(Containers::StringView name, Containers::StringView value) noexcept;

        #ifndef DOXYGEN_GENERATING_OUTPUT
        /* "Sure can't be constexpr" overloads to avoid going through the
           *insane* overload puzzle when not needed */
        template<class T, class = typename std::enable_if<!std::is_convertible<const T&, Containers::StringView>::value>::type> /*implicit*/ MaterialAttributeData(const char* name, const T& value) noexcept: MaterialAttributeData{name, Implementation::MaterialAttributeTypeFor<T>::type(), sizeof(T), &value} {}
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
         * @snippet MagnumTrade.cpp MaterialAttributeData-name
         */
        template<class T
            #ifndef DOXYGEN_GENERATING_OUTPUT
            , class = typename std::enable_if<!std::is_convertible<const T&, Containers::StringView>::value>::type
            #endif
        > /*implicit*/ MaterialAttributeData(MaterialAttribute name, const T& value) noexcept: MaterialAttributeData{name, Implementation::MaterialAttributeTypeFor<T>::type(), &value} {}

        /**
         * @brief Construct with a predefined name and string value
         * @param name      Attribute name
         * @param value     Attribute value
         *
         * Compared to @ref MaterialAttributeData(Containers::StringView, Containers::StringView)
         * checks that the attribute is in expected type. The
         * @ref MaterialAttribute gets converted to a corresponding string
         * name. Apart from the type check, the following two instances are
         * equivalent:
         *
         * @snippet MagnumTrade.cpp MaterialAttributeData-name
         */
        /*implicit*/ MaterialAttributeData(MaterialAttribute name, Containers::StringView value) noexcept: MaterialAttributeData{name, MaterialAttributeType::String, &value} {}

        /**
         * @brief Construct from a type-erased value
         * @param name      Attribute name
         * @param type      Attribute type
         * @param value     Type-erased value
         *
         * In case @p type is not @ref MaterialAttributeType::String, copies a
         * number of bytes according to @ref materialAttributeTypeSize() from
         * @p value. The @p name together with @p value is expected to fit into
         * 62 bytes.
         *
         * In case @p type is @ref MaterialAttributeType::String, @p value is
         * expected to point to a @ref Containers::StringView. The combined
         * length of @p name and @p value strings is expected to fit into 61
         * bytes.
         */
        /*implicit*/ MaterialAttributeData(Containers::StringView name, MaterialAttributeType type, const void* value) noexcept;

        /**
         * @brief Construct with a predefined name
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
         * @ref Corrade::Containers::StringViewFlag::NullTerminated set.
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
         * @ref Containers::StringView). This doesn't preserve the actual
         * string size in case the string data contain zero bytes, thus prefer
         * to use typed access in that case.
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

            |------------- x B ------------|

            +------+------- .. -----+------+
            | type | name   ..   \0 | data |
            | 1 B  |  (x - n - 2) B | n B  |
            +------+------- .. -----+------+

          where

           - `x` is Implementation::MaterialAttributeDataSize,
           - `type` is an 8-bit MaterialAttributeType,
           - `data` is of size matching `type`, at the offset of
             `(x - materialAttributeTypeSize(type))` B,
           - `name` is a null-terminated string filling the rest.

          This way the name is always at the same offset to make binary search
          lookup fast and efficient, and data being at the end (instead of
          right after the null-terminated string) makes them accessible in O(1)
          as well. */
        struct StringData {
            template<std::size_t ...sequence> constexpr explicit StringData(MaterialAttributeType type, Containers::StringView name, Containers::StringView value, Math::Implementation::Sequence<sequence...>): type{type}, nameValue{(sequence < name.size() ? name[sequence] : (sequence - (Implementation::MaterialAttributeDataSize - value.size() - 3) < value.size() ? value[sequence - (Implementation::MaterialAttributeDataSize - value.size() - 3)] : '\0'))...}, size{UnsignedByte(value.size())} {}
            constexpr explicit StringData(MaterialAttributeType type, Containers::StringView name, Containers::StringView value): StringData{type, name, value, typename Math::Implementation::GenerateSequence<Implementation::MaterialAttributeDataSize - 2>::Type{}} {}

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
            constexpr explicit ErasedScalar(MaterialTextureSwizzle value): u{UnsignedInt(value)} {}

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
            template<class U, std::size_t ...sequence> constexpr explicit Data(MaterialAttributeType type, Containers::StringView name, const U& value, Math::Implementation::Sequence<sequence...>): type{type}, name{(sequence < name.size() ? name[sequence] : '\0')...}, value{value} {}
            template<class U> constexpr explicit Data(MaterialAttributeType type, Containers::StringView name, const U& value): Data{type, name, value, typename Math::Implementation::GenerateSequence<63 - sizeof(T)>::Type{}} {}

            MaterialAttributeType type;
            char name[Implementation::MaterialAttributeDataSize - sizeof(MaterialAttributeType) - sizeof(T)];
            T value;
        };
        union CORRADE_ALIGNAS(8) Storage {
            constexpr explicit Storage() noexcept: data{} {}

            constexpr explicit Storage(Containers::StringView name, Containers::StringView value) noexcept: s{MaterialAttributeType::String, name, value} {}

            template<class T> constexpr explicit Storage(typename std::enable_if<sizeof(T) == 1, MaterialAttributeType>::type type, Containers::StringView name, const T& value) noexcept: _1{type, name, value} {}
            template<class T> constexpr explicit Storage(typename std::enable_if<sizeof(T) == 4 && !std::is_pointer<T>::value, MaterialAttributeType>::type type, Containers::StringView name, const T& value) noexcept: _4{type, name, value} {}
            template<class T> constexpr explicit Storage(typename std::enable_if<sizeof(T) == 8 && !Math::IsVector<T>::value && !std::is_pointer<T>::value, MaterialAttributeType>::type type, Containers::StringView name, const T& value) noexcept: _8{type, name, value} {}
            template<class T> constexpr explicit Storage(typename std::enable_if<sizeof(T) == 8 && Math::IsVector<T>::value && !std::is_pointer<T>::value, MaterialAttributeType>::type type, Containers::StringView name, const T& value) noexcept: _8v{type, name, value} {}
            constexpr explicit Storage(MaterialAttributeType type, Containers::StringView name, const void* value) noexcept: p{type, name, value} {}
            template<class T> constexpr explicit Storage(typename std::enable_if<sizeof(T) == 12, MaterialAttributeType>::type type, Containers::StringView name, const T& value) noexcept: _12{type, name, value} {}
            template<class T> constexpr explicit Storage(typename std::enable_if<sizeof(T) == 16 && Math::IsVector<T>::value, MaterialAttributeType>::type type, Containers::StringView name, const T& value) noexcept: _16{type, name, value} {}
            template<class T> constexpr explicit Storage(typename std::enable_if<sizeof(T) == 16 && !Math::IsVector<T>::value, MaterialAttributeType>::type type, Containers::StringView name, const T& value) noexcept: _16m{type, name, value} {}
            template<class T> constexpr explicit Storage(typename std::enable_if<sizeof(T) == 24, MaterialAttributeType>::type type, Containers::StringView name, const T& value) noexcept: _24{type, name, value} {}
            template<class T> constexpr explicit Storage(typename std::enable_if<sizeof(T) == 32, MaterialAttributeType>::type type, Containers::StringView name, const T& value) noexcept: _32{type, name, value} {}
            template<class T> constexpr explicit Storage(typename std::enable_if<sizeof(T) == 36, MaterialAttributeType>::type type, Containers::StringView name, const T& value) noexcept: _36{type, name, value} {}
            template<class T> constexpr explicit Storage(typename std::enable_if<sizeof(T) == 48, MaterialAttributeType>::type type, Containers::StringView name, const T& value) noexcept: _48{type, name, value} {}

            MaterialAttributeType type;
            char data[Implementation::MaterialAttributeDataSize];
            StringData s;
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
@ref AbstractImporter::material().

@section Trade-MaterialData-usage Usage

The simplest usage is through templated @ref attribute() functions, which take
a string attribute name or one of the pre-defined @ref MaterialAttribute
values. You're expected to check for attribute presence first with
@ref hasAttribute(), and the requested type has to match @ref attributeType().
To make things easier, each of the attributes defined in @ref MaterialAttribute
has a strictly defined type, so you can safely assume the type when requesting
those. In addition there's @ref tryAttribute() and @ref attributeOr() that
return a @ref Containers::NullOpt or a default value in case given attribute is
not found.

@snippet MagnumTrade.cpp MaterialData-usage

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
@ref PbrMetallicRoughnessMaterialData or @ref PbrSpecularGlossinessMaterialData.
Using @ref as() you can convert any @ref MaterialData instance to a reference
to one of those. These convenience APIs then take care of default values when
an attribute isn't present or handle fallbacks when an attribute can be defined
in multiple ways:

@snippet MagnumTrade.cpp MaterialData-usage-types

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

@snippet MagnumTrade.cpp MaterialData-usage-texture-complexity

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

@snippet MagnumTrade.cpp MaterialData-usage-layers

Like with base material attributes, builtin layers also have convenience
accessor APIs. The above can be written in a more compact way using
@link PbrClearCoatMaterialData @endlink:

@snippet MagnumTrade.cpp MaterialData-usage-layers-types

@section Trade-MaterialData-populating Populating an instance

A @ref MaterialData instance by default takes over ownership of an
@ref Corrade::Containers::Array containing @ref MaterialAttributeData
instances, together with @ref MaterialTypes suggesting available material types
(or an empty set, in case of a fully custom material). Attribute values can be
in one of the types from @ref MaterialAttributeType, and the type is in most
cases inferred implicitly. The class internally uses strings for attribute
names, but you're encouraged to use the predefined names from
@ref MaterialAttribute --- with those, the attribute gets checked additionally
that it's in an expected type. Attribute order doesn't matter, the array gets
internally sorted by name to allow a @f$ \mathcal{O}(\log n) @f$ lookup.

@snippet MagnumTrade.cpp MaterialData-populating

In addition to passing ownership of an array it's also possible to have the
@ref MaterialData instance refer to external data (for example in a
memory-mapped file, constant memory etc.). The additional second argument is
@ref DataFlags that's used only for safely disambiguating from the owning
constructor, and you'll pass a @ref Corrade::Containers::ArrayView instead of
an @ref Corrade::Containers::Array. Note that in this case, since the attribute
data is treated as immutable, you *have to* ensure the list is sorted by name.

@snippet MagnumTrade.cpp MaterialData-populating-non-owned

<b></b>

@m_class{m-note m-info}

@par
    Additionally, as shown above, in order to create a @cpp constexpr @ce
    @ref MaterialAttributeData array, you need to use
    @ref Corrade::Containers::StringView literals instead of plain C strings
    or the @ref MaterialAttribute enum, and be sure to call only
    @cpp constexpr @ce-enabled constructors of stored data types.

@subsection Trade-MaterialData-populating-custom Custom material attributes

While attribute names beginning with uppercase letters are reserved for builtin
Magnum attributes, anything beginning with a lowercase letter or a non-letter
can be a custom attribute. For greater flexibility, custom attributes can be
also strings or pointers, allowing you to store arbitrary properties or direct
texture pointers instead of IDs:

@snippet MagnumTrade.cpp MaterialData-populating-custom

@subsection Trade-MaterialData-populating-layers Adding material layers

Material layers are internally represented as ranges of the attribute array and
by default the whole attribute array is treated as a base material. The actual
split into layers can be described using an additional offset array passed to
@ref MaterialData(MaterialTypes, Containers::Array<MaterialAttributeData>&&, Containers::Array<UnsignedInt>&&, const void*),
where entry *i* specifies the end offset of layer *i* --- in the following
snippet we have two layers (one base material and one clear coat layer), the
base material being the first two attributes and the clear coat layer being
attributes in range @cpp 2 @ce to @cpp 6 @ce (thus four attributes):

@snippet MagnumTrade.cpp MaterialData-populating-layers

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

@snippet MagnumTrade.cpp MaterialData-populating-layers-custom

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
         */
        explicit MaterialData(MaterialTypes types, Containers::Array<MaterialAttributeData>&& attributeData, const void* importerState = nullptr) noexcept: MaterialData{types, std::move(attributeData), nullptr, importerState} {}

        /** @overload */
        /* Not noexcept because allocation happens inside */
        explicit MaterialData(MaterialTypes types, std::initializer_list<MaterialAttributeData> attributeData, const void* importerState = nullptr): MaterialData{types, attributeData, {}, importerState} {}

        /**
         * @brief Construct a non-owned material data
         * @param types                 Which material types are described by
         *      this data. Can be an empty set.
         * @param attributeDataFlags    Ignored. Used only for a safer
         *      distinction from the owning constructor.
         * @param attributeData         Attribute data
         * @param importerState         Importer-specific state
         *
         * The @p attributeData is expected to be already sorted by name,
         * without duplicates.
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
         * not larger than @p attributeData size, with *i*-th item specifying
         * end offset of *i*-th layer.
         */
        explicit MaterialData(MaterialTypes types, Containers::Array<MaterialAttributeData>&& attributeData, Containers::Array<UnsignedInt>&& layerData, const void* importerState = nullptr) noexcept;

        /** @overload */
        /* Not noexcept because allocation happens inside */
        explicit MaterialData(MaterialTypes types, std::initializer_list<MaterialAttributeData> attributeData, std::initializer_list<UnsignedInt> layerData, const void* importerState = nullptr);

        /**
         * @brief Construct a non-owned material data with layers
         * @param types                 Which material types are described by
         *      this data. Can be an empty set.
         * @param attributeDataFlags    Ignored. Used only for a safer
         *      distinction from the owning constructor.
         * @param attributeData         Attribute data
         * @param layerDataFlags        Ignored. Used only for a safer
         *      distinction from the owning constructor.
         * @param layerData             Layer offset data
         * @param importerState     Importer-specific state
         *
         * The @p data is expected to be already sorted by name, without
         * duplicates inside each layer. The @p layerData is expected to be
         * either empty or a monotonically non-decreasing sequence of offsets
         * not larger than @p attributeData size, with *i*-th item specifying
         * end offset of *i*-th layer.
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
         * @ref PbrMetallicRoughnessMaterialData,
         * @ref PbrSpecularGlossinessMaterialData or @ref PhongMaterialData.
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
            return T{std::move(const_cast<T&>(as<T>()))};
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
         * @see @ref releaseAttributeData()
         */
        Containers::ArrayView<const MaterialAttributeData> attributeData() const { return _data; }

        /**
         * @brief Layer count
         *
         * There's always at least the base material, so this function returns
         * at least @cpp 1 @ce.
         */
        UnsignedInt layerCount() const {
            return _layerOffsets.empty() ? 1 : _layerOffsets.size();
        }

        /**
         * @brief Whether a material has given named layer
         *
         * Layers with no name assigned are skipped. The base material (layer
         * @cpp 0 @ce is skipped as well) to avoid confusing base material with
         * a layer. If you want to create a material consisting of just a
         * layer, use @cpp 0 @ce for the first layer offset in the constructor.
         * @see @ref hasAttribute()
         */
        bool hasLayer(Containers::StringView layer) const;
        bool hasLayer(MaterialLayer layer) const; /**< @overload */

        /**
         * @brief ID of a named layer
         *
         * The @p layer is expected to exist.
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
         * the base material. If not present, the default is @cpp 0 @ce.
         * Available only if the @ref MaterialAttribute::LayerFactorTexture
         * attribute is present. The @p layer is expected to exist.
         * @see @ref hasLayer(), @ref hasAttribute()
         */
        UnsignedInt layerFactorTextureCoordinates(Containers::StringView layer) const;
        UnsignedInt layerFactorTextureCoordinates(MaterialLayer layer) const; /**< @overload */

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
         * @see @ref tryAttribute(), @ref attributeOr(), @ref hasLayer()
         */
        bool hasAttribute(UnsignedInt layer, Containers::StringView name) const;
        bool hasAttribute(UnsignedInt layer, MaterialAttribute name) const; /**< @overload */

        /**
         * @brief Whether a named material layer has given attribute
         *
         * The @p layer is expected to exist.
         * @see @ref tryAttribute(), @ref attributeOr(), @ref hasLayer()
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
         * @see @ref tryAttribute(), @ref attributeOr()
         */
        bool hasAttribute(Containers::StringView name) const {
            return hasAttribute(0, name);
        }
        bool hasAttribute(MaterialAttribute name) const {
            return hasAttribute(0, name);
        } /**< @overload */

        /**
         * @brief ID of a named attribute in given material layer
         *
         * The @p layer is expected to be smaller than @ref layerCount() const
         * and @p name is expected to exist in that layer.
         * @see @ref hasAttribute()
         */
        UnsignedInt attributeId(UnsignedInt layer, Containers::StringView name) const;
        UnsignedInt attributeId(UnsignedInt layer, MaterialAttribute name) const; /**< @overload */

        /**
         * @brief ID of a named attribute in a named material layer
         *
         * The @p layer is expected to exist and @p name is expected to exist
         * in that layer.
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
         * @brief Name of an attribute in given material layer
         *
         * The @p layer is expected to be smaller than @ref layerCount() const
         * and the @p id is expected to be smaller than @ref attributeCount(UnsignedInt) const
         * in that layer. The returned view always has
         * @ref Corrade::Containers::StringViewFlag::NullTerminated set.
         * @see @ref attributeType()
         */
        Containers::StringView attributeName(UnsignedInt layer, UnsignedInt id) const;

        /**
         * @brief Name of an attribute in a named material layer
         *
         * The @p layer is expected to exist and the @p id is expected to be smaller than @ref attributeCount(UnsignedInt) const
         * in that layer.
         * @see @ref hasLayer()
         */
        Containers::StringView attributeName(Containers::StringView layer, UnsignedInt id) const;
        Containers::StringView attributeName(MaterialLayer layer, UnsignedInt id) const; /**< @overload */

        /**
         * @brief Name of an attribute in the base material
         *
         * Equivalent to calling @ref attributeName(UnsignedInt, UnsignedInt) const
         * with @p layer set to @cpp 0 @ce.
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
         *      @ref Containers::StringView). This doesn't preserve the actual
         *      string size in case the string data contain zero bytes, thus
         *      prefer to use typed access in that case.
         */
        const void* attribute(UnsignedInt layer, UnsignedInt id) const;

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
         *      @ref Containers::StringView). This doesn't preserve the actual
         *      string size in case the string data contain zero bytes, thus prefer
         *      to use typed access in that case.
         *
         * @see @ref hasAttribute(), @ref tryAttribute(), @ref attributeOr()
         */
        const void* attribute(UnsignedInt layer, Containers::StringView name) const;
        const void* attribute(UnsignedInt layer, MaterialAttribute name) const; /**< @overload */

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
         *      @ref Containers::StringView). This doesn't preserve the actual
         *      string size in case the string data contain zero bytes, thus prefer
         *      to use typed access in that case.
         *
         * @see @ref hasLayer()
         */
        const void* attribute(Containers::StringView layer, UnsignedInt id) const;
        const void* attribute(MaterialLayer layer, UnsignedInt id) const; /**< @overload */

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
         *      @ref Containers::StringView). This doesn't preserve the actual
         *      string size in case the string data contain zero bytes, thus
         *      prefer to use typed access in that case.
         *
         * @see @ref hasLayer(), @ref hasAttribute()
         */
        const void* attribute(Containers::StringView layer, Containers::StringView name) const;
        const void* attribute(Containers::StringView layer, MaterialAttribute name) const; /**< @overload */
        const void* attribute(MaterialLayer layer, Containers::StringView name) const; /**< @overload */
        const void* attribute(MaterialLayer layer, MaterialAttribute name) const; /**< @overload */

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
         * @brief Value of an attribute in given material layer
         *
         * The @p layer is expected to be smaller than @ref layerCount() const
         * and @p id is expected to be smaller than
         * @ref attributeCount(UnsignedInt) const in that layer. Expects that
         * @p T corresponds to @ref attributeType(UnsignedInt, UnsignedInt) const
         * for given @p layer and @p id. In case of a string, the returned view
         * always has @ref Corrade::Containers::StringViewFlag::NullTerminated
         * set.
         */
        template<class T> T attribute(UnsignedInt layer, UnsignedInt id) const;

        /**
         * @brief Value of a named attribute in given material layer
         *
         * The @p layer is expected to be smaller than @ref layerCount() const
         * and @p name is expected to exist in that layer. Expects that @p T
         * corresponds to @ref attributeType(UnsignedInt, Containers::StringView) const
         * for given @p layer and @p name. In case of a string, the returned
         * view always has
         * @ref Corrade::Containers::StringViewFlag::NullTerminated set.
         * @see @ref hasLayer(), @ref hasAttribute()
         */
        template<class T> T attribute(UnsignedInt layer, Containers::StringView name) const;
        template<class T> T attribute(UnsignedInt layer, MaterialAttribute name) const; /**< @overload */

        /**
         * @brief Value of an attribute in a named material layer
         *
         * The @p layer is expected to exist and @p id is expected to be
         * smaller than @ref attributeCount(UnsignedInt) const in that layer.
         * Expects that @p T corresponds to
         * @ref attributeType(Containers::StringView, UnsignedInt) const
         * for given @p layer and @p id. In case of a string, the returned view
         * always has @ref Corrade::Containers::StringViewFlag::NullTerminated
         * set.
         * @see @ref hasLayer()
         */
        template<class T> T attribute(Containers::StringView layer, UnsignedInt id) const;
        template<class T> T attribute(MaterialLayer layer, UnsignedInt id) const; /**< @overload */

        /**
         * @brief Value of a named attribute in a named material layer
         *
         * The @p layer is expected to exist and @p name is expected to exist
         * in that layer. Expects that @p T corresponds to
         * @ref attributeType(Containers::StringView, Containers::StringView) const
         * for given @p layer and @p name. In case of a string, the returned
         * view always has
         * @ref Corrade::Containers::StringViewFlag::NullTerminated set.
         * @see @ref hasLayer(), @ref hasAttribute()
         */
        template<class T> T attribute(Containers::StringView layer, Containers::StringView name) const;
        template<class T> T attribute(Containers::StringView layer, MaterialAttribute name) const; /**< @overload */
        template<class T> T attribute(MaterialLayer layer, Containers::StringView name) const; /**< @overload */
        template<class T> T attribute(MaterialLayer layer, MaterialAttribute name) const; /**< @overload */

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
         * @brief Type-erased attribute value in given material layer, if exists
         *
         * Compared to @ref attribute(UnsignedInt, Containers::StringView name) const,
         * if @p name doesn't exist, returns @cpp nullptr @ce instead of
         * asserting. Expects that @p layer is smaller than @ref layerCount() const.
         * Cast the pointer to a concrete type based on @ref attributeType().
         * @see @ref hasAttribute(), @ref attributeOr()
         */
        const void* tryAttribute(UnsignedInt layer, Containers::StringView name) const;
        const void* tryAttribute(UnsignedInt layer, MaterialAttribute name) const; /**< @overload */

        /**
         * @brief Type-erased attribute value in a named material layer, if exists
         *
         * Compared to @ref attribute(Containers::StringView, Containers::StringView name) const,
         * if @p name doesn't exist, returns @cpp nullptr @ce instead of
         * asserting. Expects that @p layer exists. Cast the pointer to a
         * concrete type based on @ref attributeType().
         * @see @ref hasLayer(), @ref hasAttribute(), @ref attributeOr()
         */
        const void* tryAttribute(Containers::StringView layer, Containers::StringView name) const;
        const void* tryAttribute(Containers::StringView layer, MaterialAttribute name) const; /**< @overload */
        const void* tryAttribute(MaterialLayer layer, Containers::StringView name) const; /**< @overload */
        const void* tryAttribute(MaterialLayer layer, MaterialAttribute name) const; /**< @overload */

        /**
         * @brief Value of a named attribute in given material layer, if exists
         *
         * Compared to @ref attribute(UnsignedInt, Containers::StringView name) const,
         * if @p name doesn't exist, returns @ref Corrade::Containers::NullOpt
         * instead of asserting. Expects that @p layer is smaller than
         * @ref layerCount() const and that @p T corresponds to
         * @ref attributeType(UnsignedInt, Containers::StringView) const for
         * given @p layer and @p name.
         */
        template<class T> Containers::Optional<T> tryAttribute(UnsignedInt layer, Containers::StringView name) const;
        template<class T> Containers::Optional<T> tryAttribute(UnsignedInt layer, MaterialAttribute name) const; /**< @overload */

        /**
         * @brief Value of a named attribute in a named material layer, if exists
         *
         * Compared to @ref attribute(Containers::StringView, Containers::StringView name) const,
         * if @p name doesn't exist, returns @ref Corrade::Containers::NullOpt
         * instead of asserting. Expects that @p layer exists and that @p T
         * corresponds to @ref attributeType(Containers::StringView, Containers::StringView) const
         * for given @p layer and @p name.
         * @see @ref hasLayer()
         */
        template<class T> Containers::Optional<T> tryAttribute(Containers::StringView layer, Containers::StringView name) const;
        template<class T> Containers::Optional<T> tryAttribute(Containers::StringView layer, MaterialAttribute name) const; /**< @overload */
        template<class T> Containers::Optional<T> tryAttribute(MaterialLayer layer, Containers::StringView name) const; /**< @overload */
        template<class T> Containers::Optional<T> tryAttribute(MaterialLayer layer, MaterialAttribute name) const; /**< @overload */

        /**
         * @brief Type-erased attribute value in the base material, if exists
         *
         * Equivalent to calling @ref tryAttribute(UnsignedInt, Containers::StringView) const
         * with @p layer set to @cpp 0 @ce.
         */
        const void* tryAttribute(Containers::StringView name) const {
            return tryAttribute(0, name);
        }
        const void* tryAttribute(MaterialAttribute name) const {
            return tryAttribute(0, name);
        } /**< @overload */

        /**
         * @brief Value of a named attribute in the base material, if exists
         *
         * Equivalent to calling @ref tryAttribute(UnsignedInt, Containers::StringView) const
         * with @p layer set to @cpp 0 @ce.
         */
        template<class T> Containers::Optional<T> tryAttribute(Containers::StringView name) const {
            return tryAttribute<T>(0, name);
        }
        template<class T> Containers::Optional<T> tryAttribute(MaterialAttribute name) const {
            return tryAttribute<T>(0, name);
        } /**< @overload */

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
         * deleter when the data are not owned by the mesh, and while the
         * returned array type is mutable, the actual memory might be not.
         *
         * @attention Querying attributes after calling @ref releaseLayerData()
         *      has undefined behavior and might lead to crashes. This is done
         *      intentionally in order to simplify the interaction between this
         *      function and @ref releaseAttributeData().
         * @see @ref layerData()
         */
        Containers::Array<UnsignedInt> releaseLayerData();

        /**
         * @brief Release attribute data storage
         *
         * Releases the ownership of the attribute array and resets internal
         * attribute-related state to default. The material then behaves like
         * if it has no attributes. Note that the returned array has a custom
         * no-op deleter when the data are not owned by the mesh, and while the
         * returned array type is mutable, the actual memory might be not.
         *
         * @attention Querying layers after calling @ref releaseAttributeData()
         *      has undefined behavior and might lead to crashes. This is done
         *      intentionally in order to simplify the interaction between this
         *      function and @ref releaseLayerData().
         * @see @ref attributeData()
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

        static Containers::StringView layerString(MaterialLayer name);
        static Containers::StringView attributeString(MaterialAttribute name);
        /* Internal helpers that don't assert, unlike layerId() / attributeId() */
        UnsignedInt layerFor(Containers::StringView layer) const;
        UnsignedInt layerOffset(UnsignedInt layer) const {
            return layer && _layerOffsets ? _layerOffsets[layer - 1] : 0;
        }
        UnsignedInt attributeFor(UnsignedInt layer, Containers::StringView name) const;

        Containers::Array<MaterialAttributeData> _data;
        Containers::Array<UnsignedInt> _layerOffsets;
        MaterialTypes _types;
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
    , class
    #endif
> constexpr MaterialAttributeData::MaterialAttributeData(const Containers::StringView name, const T& value) noexcept:
    _data{Implementation::MaterialAttributeTypeFor<T>::type(),
        /* MSVC 2015 complains about "error C2065: 'T': undeclared identifier"
           in the lambda inside this macro. Sorry, the assert will be less
           useful on that stupid thing. */
        #ifndef CORRADE_MSVC2015_COMPATIBILITY
        (CORRADE_CONSTEXPR_ASSERT(name.size() + sizeof(T) + 2 <= Implementation::MaterialAttributeDataSize, "Trade::MaterialAttributeData: name" << name << "too long, expected at most" << Implementation::MaterialAttributeDataSize - sizeof(T) - 2 << "bytes for" << Implementation::MaterialAttributeTypeFor<T>::type() << "but got" << name.size()), name)
        #else
        (CORRADE_CONSTEXPR_ASSERT(name.size() + sizeof(T) + 2 <= Implementation::MaterialAttributeDataSize, "Trade::MaterialAttributeData: name" << name << "too long, got" << name.size() << "bytes"), name)
        #endif
    , value} {}

/* The 4 extra bytes are for a null byte after both the name and value, a type
   and a string size */
constexpr MaterialAttributeData::MaterialAttributeData(const Containers::StringView name, const Containers::StringView value) noexcept: _data{(CORRADE_CONSTEXPR_ASSERT(name.size() + value.size() + 4 <= Implementation::MaterialAttributeDataSize, "Trade::MaterialAttributeData: name" << name << "and value" << value << "too long, expected at most" << Implementation::MaterialAttributeDataSize - 4 << "bytes in total but got" << name.size() + value.size()), name), value} {}

template<class T> T MaterialAttributeData::value() const {
    CORRADE_ASSERT(Implementation::MaterialAttributeTypeFor<T>::type() == _data.type,
        "Trade::MaterialAttributeData::value(): improper type requested for" << (_data.data + 1) << "of" << _data.type, {});
    return *reinterpret_cast<const T*>(value());
}

#ifndef DOXYGEN_GENERATING_OUTPUT
template<> Containers::StringView MaterialAttributeData::value<Containers::StringView>() const;
#endif

template<class T> T MaterialData::attribute(const UnsignedInt layer, const UnsignedInt id) const {
    const void* const value = attribute(layer, id);
    #ifdef CORRADE_GRACEFUL_ASSERT
    if(!value) return {};
    #endif
    const Trade::MaterialAttributeData& data = _data[layerOffset(layer) + id];
    CORRADE_ASSERT(Implementation::MaterialAttributeTypeFor<T>::type() == data._data.type,
        "Trade::MaterialData::attribute(): improper type requested for" << (data._data.data + 1) << "of" << data._data.type, {});
    return *reinterpret_cast<const T*>(value);
}

#ifndef DOXYGEN_GENERATING_OUTPUT
template<> Containers::StringView MaterialData::attribute<Containers::StringView>(UnsignedInt, UnsignedInt) const;
#endif

template<class T> T MaterialData::attribute(const UnsignedInt layer, const Containers::StringView name) const {
    CORRADE_ASSERT(layer < layerCount(),
        "Trade::MaterialData::attribute(): index" << layer << "out of range for" << layerCount() << "layers", {});
    const UnsignedInt id = attributeFor(layer, name);
    CORRADE_ASSERT(id != ~UnsignedInt{},
        "Trade::MaterialData::attribute(): attribute" << name << "not found in layer" << layer, {});
    return attribute<T>(layer, id);
}

template<class T> T MaterialData::attribute(const UnsignedInt layer, const MaterialAttribute name) const {
    const Containers::StringView string = attributeString(name);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::attribute(): invalid name" << name, {});
    return attribute<T>(layer, string);
}

template<class T> T MaterialData::attribute(const Containers::StringView layer, const UnsignedInt id) const {
    const UnsignedInt layerId = layerFor(layer);
    CORRADE_ASSERT(layerId != ~UnsignedInt{},
        "Trade::MaterialData::attribute(): layer" << layer << "not found", {});
    CORRADE_ASSERT(id < attributeCount(layer),
        "Trade::MaterialData::attribute(): index" << id << "out of range for" << attributeCount(layer) << "attributes in layer" << layer, {});
    return attribute<T>(layerId, id);
}

template<class T> T MaterialData::attribute(const Containers::StringView layer, const Containers::StringView name) const {
    const UnsignedInt layerId = layerFor(layer);
    CORRADE_ASSERT(layerId != ~UnsignedInt{},
        "Trade::MaterialData::attribute(): layer" << layer << "not found", {});
    const UnsignedInt id = attributeFor(layerId, name);
    CORRADE_ASSERT(id != ~UnsignedInt{},
        "Trade::MaterialData::attribute(): attribute" << name << "not found in layer" << layer, {});
    return attribute<T>(layerId, id);
}

template<class T> T MaterialData::attribute(const Containers::StringView layer, const MaterialAttribute name) const {
    const Containers::StringView string = attributeString(name);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::attribute(): invalid name" << name, {});
    return attribute<T>(layer, string);
}

template<class T> T MaterialData::attribute(const MaterialLayer layer, const UnsignedInt id) const {
    const Containers::StringView string = layerString(layer);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::attribute(): invalid name" << layer, {});
    return attribute<T>(string, id);
}

template<class T> T MaterialData::attribute(const MaterialLayer layer, const Containers::StringView name) const {
    const Containers::StringView string = layerString(layer);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::attribute(): invalid name" << layer, {});
    return attribute<T>(string, name);
}

template<class T> T MaterialData::attribute(const MaterialLayer layer, const MaterialAttribute name) const {
    const Containers::StringView string = layerString(layer);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::attribute(): invalid name" << layer, {});
    return attribute<T>(string, name);
}

template<class T> Containers::Optional<T> MaterialData::tryAttribute(const UnsignedInt layer, const Containers::StringView name) const {
    CORRADE_ASSERT(layer < layerCount(),
        "Trade::MaterialData::tryAttribute(): index" << layer << "out of range for" << layerCount() << "layers", {});
    const UnsignedInt id = attributeFor(layer, name);
    if(id == ~UnsignedInt{}) return {};
    return attribute<T>(layer, id);
}

template<class T> Containers::Optional<T> MaterialData::tryAttribute(const UnsignedInt layer, const MaterialAttribute name) const {
    const Containers::StringView string = attributeString(name);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::tryAttribute(): invalid name" << name, {});
    return tryAttribute<T>(layer, string);
}

template<class T> Containers::Optional<T> MaterialData::tryAttribute(const Containers::StringView layer, const Containers::StringView name) const {
    const UnsignedInt layerId = layerFor(layer);
    CORRADE_ASSERT(layerId != ~UnsignedInt{},
        "Trade::MaterialData::tryAttribute(): layer" << layer << "not found", {});
    return tryAttribute<T>(layerId, name);
}

template<class T> Containers::Optional<T> MaterialData::tryAttribute(const Containers::StringView layer, const MaterialAttribute name) const {
    const Containers::StringView string = attributeString(name);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::tryAttribute(): invalid name" << name, {});
    return tryAttribute<T>(layer, string);
}

template<class T> Containers::Optional<T> MaterialData::tryAttribute(const MaterialLayer layer, const Containers::StringView name) const {
    const Containers::StringView string = layerString(layer);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::tryAttribute(): invalid name" << layer, {});
    return tryAttribute<T>(string, name);
}

template<class T> Containers::Optional<T> MaterialData::tryAttribute(const MaterialLayer layer, const MaterialAttribute name) const {
    const Containers::StringView string = layerString(layer);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::tryAttribute(): invalid name" << layer, {});
    return tryAttribute<T>(string, name);
}

template<class T> T MaterialData::attributeOr(const UnsignedInt layer, const Containers::StringView name, const T& defaultValue) const {
    CORRADE_ASSERT(layer < layerCount(),
        "Trade::MaterialData::attributeOr(): index" << layer << "out of range for" << layerCount() << "layers", {});
    const UnsignedInt id = attributeFor(layer, name);
    if(id == ~UnsignedInt{}) return defaultValue;
    return attribute<T>(layer, id);
}

template<class T> T MaterialData::attributeOr(const UnsignedInt layer, const MaterialAttribute name, const T& defaultValue) const {
    const Containers::StringView string = attributeString(name);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::attributeOr(): invalid name" << name, {});
    return attributeOr<T>(layer, string, defaultValue);
}

template<class T> T MaterialData::attributeOr(const Containers::StringView layer, const Containers::StringView name, const T& defaultValue) const {
    const UnsignedInt layerId = layerFor(layer);
    CORRADE_ASSERT(layerId != ~UnsignedInt{},
        "Trade::MaterialData::attributeOr(): layer" << layer << "not found", {});
    return attributeOr<T>(layerId, name, defaultValue);
}

template<class T> T MaterialData::attributeOr(const Containers::StringView layer, const MaterialAttribute name, const T& defaultValue) const {
    const Containers::StringView string = attributeString(name);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::attributeOr(): invalid name" << name, {});
    return attributeOr<T>(layer, string, defaultValue);
}

template<class T> T MaterialData::attributeOr(const MaterialLayer layer, const Containers::StringView name, const T& defaultValue) const {
    const Containers::StringView string = layerString(layer);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::attributeOr(): invalid name" << layer, {});
    return attributeOr<T>(string, name, defaultValue);
}

template<class T> T MaterialData::attributeOr(const MaterialLayer layer, const MaterialAttribute name, const T& defaultValue) const {
    const Containers::StringView string = layerString(layer);
    CORRADE_ASSERT(string.data(), "Trade::MaterialData::attributeOr(): invalid name" << layer, {});
    return attributeOr<T>(string, name, defaultValue);
}

}}

#endif
