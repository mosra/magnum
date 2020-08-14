#ifndef Magnum_Trade_PbrMetallicRoughnessMaterialData_h
#define Magnum_Trade_PbrMetallicRoughnessMaterialData_h
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
 * @brief Class @ref Magnum::Trade::PbrMetallicRoughnessMaterialData
 * @m_since_latest
 */

#include "Magnum/Trade/MaterialData.h"

namespace Magnum { namespace Trade {

/**
@brief PBR metallic/roughness material data
@m_since_latest

See @ref Trade-MaterialData-usage-types for more information about how to use
this class.
@see @ref AbstractImporter::material(), @ref FlatMaterialData,
    @ref PhongMaterialData, @ref PbrSpecularGlossinessMaterialData,
    @ref PbrClearCoatMaterialData, @ref MaterialType::PbrMetallicRoughness
*/
class MAGNUM_TRADE_EXPORT PbrMetallicRoughnessMaterialData: public MaterialData {
    public:
        #ifndef DOXYGEN_GENERATING_OUTPUT
        /* Allow constructing subclasses directly. While not used in the
           general Importer workflow, it allows users to create instances with
           desired convenience APIs easier (and simplifies testing). It's
           however hidden from the docs as constructing instances this way
           isn't really common and it would add a lot of noise. */
        using MaterialData::MaterialData;
        #endif

        /**
         * @brief Whether the material has a metalness texture
         *
         * Returns @cpp true @ce if any of the
         * @ref MaterialAttribute::MetalnessTexture or
         * @ref MaterialAttribute::NoneRoughnessMetallicTexture attributes is
         * present, @cpp false @ce otherwise.
         * @see @ref hasRoughnessTexture(), @ref hasNoneRoughnessMetallicTexture()
         */
        bool hasMetalnessTexture() const;

        /**
         * @brief Whether the material has a roughness texture
         *
         * Returns @cpp true @ce if any of the
         * @ref MaterialAttribute::RoughnessTexture or
         * @ref MaterialAttribute::NoneRoughnessMetallicTexture attributes is
         * present, @cpp false @ce otherwise.
         * @see @ref hasMetalnessTexture(), @ref hasNoneRoughnessMetallicTexture()
         */
        bool hasRoughnessTexture() const;

        /**
         * @brief Whether the material has a combined roughness/metallic texture
         *
         * Returns @cpp true @ce if either the
         * @ref MaterialAttribute::NoneRoughnessMetallicTexture attribute is
         * present or both @ref MaterialAttribute::RoughnessTexture and
         * @ref MaterialAttribute::MetalnessTexture are present, point to the
         * same texture ID, @ref MaterialAttribute::RoughnessTextureSwizzle is
         * set to @ref MaterialTextureSwizzle::G and
         * @ref MaterialAttribute::MetalnessTextureSwizzle is set to
         * @ref MaterialTextureSwizzle::B, and ddditionally
         * @ref MaterialAttribute::RoughnessTextureMatrix and
         * @ref MaterialAttribute::MetalnessTextureMatrix are both either not
         * present or have the same value, and
         * @ref MaterialAttribute::RoughnessTextureCoordinates and
         * @ref MaterialAttribute::MetalnessTextureCoordinates are both either
         * not present or have the same value; @cpp false @ce otherwise.
         *
         * In other words, if this function returns @cpp true @ce,
         * @ref roughnessTexture(), @ref roughnessTextureMatrix() and
         * @ref roughnessTextureCoordinates() return values common for both
         * metalness and roughness texture, and the two are packed together
         * with roughness occupying the G channel and metalness the B channel.
         * This packing is common in glTF metallic/roughness materials, which
         * in turn is compatible with how UE4 assets are usually packed.
         * Original rationale for this [can be seen here](https://github.com/KhronosGroup/glTF/issues/857).
         *
         * The red and alpha channels are ignored and can be repurposed for
         * other maps such as occlusion or transmission. This check is a subset
         * of @ref hasOcclusionRoughnessMetallicTexture() --- if that function
         * returns @cpp true @ce, this will return @cpp true @ce as well.
         * @see @ref hasMetalnessTexture(), @ref hasRoughnessTexture(),
         *      @ref hasRoughnessMetallicOcclusionTexture(),
         *      @ref hasNormalRoughnessMetallicTexture()
         */
        bool hasNoneRoughnessMetallicTexture() const;

        /**
         * @brief Whether the material has a combined occlusion/roughness/metallic texture
         *
         * Returns @cpp true @ce if @ref MaterialAttribute::OcclusionTexture,
         * @ref MaterialAttribute::RoughnessTexture and
         * @ref MaterialAttribute::MetalnessTexture are all present, point to
         * the same texture ID, @ref MaterialAttribute::OcclusionTextureSwizzle
         * is set to @ref MaterialTextureSwizzle::R (or omitted, in which case
         * it's the default), @ref MaterialAttribute::RoughnessTextureSwizzle
         * is set to @ref MaterialTextureSwizzle::G and
         * @ref MaterialAttribute::MetalnessTextureSwizzle is set to
         * @ref MaterialTextureSwizzle::B, and additionally
         * @ref MaterialAttribute::OcclusionTextureMatrix,
         * @ref MaterialAttribute::RoughnessTextureMatrix and
         * @ref MaterialAttribute::MetalnessTextureMatrix are all other not
         * present or have the same value, and
         * @ref MaterialAttribute::OcclusionTextureCoordinates,
         * @ref MaterialAttribute::RoughnessTextureCoordinates and
         * @ref MaterialAttribute::MetalnessTextureCoordinates are all either
         * not present or have the same value; @cpp false @ce otherwise.
         *
         * In other words, if this function returns @cpp true @ce,
         * @ref occlusionTexture(), @ref occlusionTextureMatrix() and
         * @ref occlusionTextureCoordinates() return values common for
         * occlusion, roughness and metalness textures, and the three are
         * packed together with occlusion occupying the R channel, roughness
         * the G channel and metalness the B channel. This packing is common in
         * glTF metallic/roughness materials, which in turn is compatible with
         * how UE4 assets are usually packed. Original rationale for this [can
         * be seen here](https://github.com/KhronosGroup/glTF/issues/857),
         * there's also a [MSFT_packing_occlusionRoughnessMetallic](https://github.com/KhronosGroup/glTF/blob/master/extensions/2.0/Vendor/MSFT_packing_occlusionRoughnessMetallic/README.md)
         * glTF extension using this packing in a more explicit way.
         *
         * The alpha channel is ignored and can be repurposed for other maps
         * such as transmission. This check is a superset of
         * @ref hasNoneRoughnessMetallicTexture() --- if this function returns
         * @cpp true @ce, that one will return @cpp true @ce as well.
         * @see @ref hasMetalnessTexture(), @ref hasRoughnessTexture(),
         *      @ref hasNoneRoughnessMetallicTexture(),
         *      @ref hasRoughnessMetallicOcclusionTexture(),
         *      @ref hasNormalRoughnessMetallicTexture()
         */
        bool hasOcclusionRoughnessMetallicTexture() const;

        /**
         * @brief Whether the material has a combined roughness/metallic/occlusion texture
         *
         * Returns @cpp true @ce if @ref MaterialAttribute::RoughnessTexture,
         * @ref MaterialAttribute::MetalnessTexture and
         * @ref MaterialAttribute::OcclusionTexture are all present, point to
         * the same texture ID, @ref MaterialAttribute::RoughnessTextureSwizzle
         * is set to @ref MaterialTextureSwizzle::R (or omitted, in which case
         * it's the default), @ref MaterialAttribute::MetalnessTextureSwizzle
         * is set to @ref MaterialTextureSwizzle::G and
         * @ref MaterialAttribute::OcclusionTextureSwizzle is set to
         * @ref MaterialTextureSwizzle::B, and additionally
         * @ref MaterialAttribute::RoughnessTextureMatrix,
         * @ref MaterialAttribute::MetalnessTextureMatrix and
         * @ref MaterialAttribute::OcclusionTextureMatrix are all other not
         * present or have the same value, and
         * @ref MaterialAttribute::RoughnessTextureCoordinates,
         * @ref MaterialAttribute::MetalnessTextureCoordinates and
         * @ref MaterialAttribute::OcclusionTextureCoordinates are all either
         * not present or have the same value; @cpp false @ce otherwise.
         *
         * In other words, if this function returns @cpp true @ce,
         * @ref roughnessTexture(), @ref roughnessTextureMatrix() and
         * @ref roughnessTextureCoordinates() return values common for
         * roughness, metalness and occlusion textures, and the three are
         * packed together with roughness occupying the R channel, metalness
         * the G channel and occlusion the B channel. This check is present in
         * order to provide support for the [MSFT_packing_occlusionRoughnessMetallic](https://github.com/KhronosGroup/glTF/blob/master/extensions/2.0/Vendor/MSFT_packing_occlusionRoughnessMetallic/README.md)
         * glTF extension.
         *
         * The alpha channel is ignored and can be repurposed for other maps
         * such as transmission.
         * @see @ref hasMetalnessTexture(), @ref hasRoughnessTexture(),
         *      @ref hasNoneRoughnessMetallicTexture(),
         *      @ref hasOcclusionRoughnessMetallicTexture(),
         *      @ref hasNormalRoughnessMetallicTexture()
         */
        bool hasRoughnessMetallicOcclusionTexture() const;

        /**
         * @brief Whether the material has a combined normal/roughness/metallic texture
         *
         * Returns @cpp true @ce if @ref MaterialAttribute::NormalTexture,
         * @ref MaterialAttribute::RoughnessTexture and
         * @ref MaterialAttribute::MetalnessTexture are all present, point to
         * the same texture ID, @ref MaterialAttribute::NormalTextureSwizzle
         * is set to @ref MaterialTextureSwizzle::RG (with the third channel
         * implicit), @ref MaterialAttribute::RoughnessTextureSwizzle
         * is set to @ref MaterialTextureSwizzle::B and
         * @ref MaterialAttribute::MetalnessTextureSwizzle is set to
         * @ref MaterialTextureSwizzle::A, and additionally
         * @ref MaterialAttribute::NormalTextureMatrix,
         * @ref MaterialAttribute::RoughnessTextureMatrix and
         * @ref MaterialAttribute::MetalnessTextureMatrix are all other not
         * present or have the same value, and
         * @ref MaterialAttribute::NormalTextureCoordinates,
         * @ref MaterialAttribute::RoughnessTextureCoordinates and
         * @ref MaterialAttribute::MetalnessTextureCoordinates are all either
         * not present or have the same value; @cpp false @ce otherwise.
         *
         * In other words, if this function returns @cpp true @ce,
         * @ref normalTexture(), @ref normalTextureMatrix() and
         * @ref normalTextureCoordinates() return values common for normal,
         * roughness and metalness textures, and the three are packed together
         * with normals occupying the RG channel, roughness the B channel and
         * metalness the A channel. This check is present in order to provide
         * support for the [MSFT_packing_normalRoughnessMetallic](https://github.com/KhronosGroup/glTF/blob/master/extensions/2.0/Vendor/MSFT_packing_normalRoughnessMetallic/README.md)
         * glTF extension.
         *
         * @see @ref hasMetalnessTexture(), @ref hasRoughnessTexture(),
         *      @ref hasNoneRoughnessMetallicTexture(),
         *      @ref hasRoughnessMetallicOcclusionTexture(),
         *      @ref hasOcclusionRoughnessMetallicTexture()
         */
        bool hasNormalRoughnessMetallicTexture() const;

        /**
         * @brief Whether the material has texture transformation
         *
         * Returns @cpp true @ce if any of the
         * @ref MaterialAttribute::BaseColorTextureMatrix,
         * @ref MaterialAttribute::MetalnessTextureMatrix,
         * @ref MaterialAttribute::RoughnessTextureMatrix,
         * @ref MaterialAttribute::NormalTextureMatrix,
         * @ref MaterialAttribute::OcclusionTextureMatrix,
         * @ref MaterialAttribute::EmissiveTextureMatrix or
         * @ref MaterialAttribute::TextureMatrix attributes is present,
         * @cpp false @ce otherwise.
         * @see @ref hasCommonTextureTransformation()
         */
        bool hasTextureTransformation() const;

        /**
         * @brief Whether the material has a common transformation for all textures
         *
         * Returns @cpp true @ce if, for each texture that is present,
         * @ref baseColorTextureMatrix(), @ref metalnessTextureMatrix(),
         * @ref roughnessTextureMatrix(), @ref normalTextureMatrix(),
         * @ref occlusionTextureMatrix() and @ref emissiveTextureMatrix() have
         * the same value, @cpp false @ce otherwise. In particular, returns
         * @cpp true @ce also if there's no texture transformation at all. Use
         * @ref hasTextureTransformation() to distinguish that case.
         */
        bool hasCommonTextureTransformation() const;

        /**
         * @brief Whether the material uses extra texture coordinate sets
         *
         * Returns @cpp true @ce if any of the
         * @ref MaterialAttribute::BaseColorTextureCoordinates,
         * @ref MaterialAttribute::MetalnessTextureCoordinates,
         * @ref MaterialAttribute::RoughnessTextureCoordinates,
         * @ref MaterialAttribute::NormalTextureCoordinates,
         * @ref MaterialAttribute::OcclusionTextureCoordinates,
         * @ref MaterialAttribute::EmissiveTextureCoordinates or
         * @ref MaterialAttribute::TextureCoordinates attributes is present and
         * has a non-zero value, @cpp false @ce otherwise.
         * @see @ref hasCommonTextureCoordinates()
         */
        bool hasTextureCoordinates() const;

        /**
         * @brief Whether the material has a common coordinate set for all textures
         *
         * Returns @cpp true @ce if, for each texture that is present,
         * @ref baseColorTextureCoordinates(), @ref metalnessTextureCoordinates(),
         * @ref roughnessTextureCoordinates(), @ref normalTextureCoordinates(),
         * @ref occlusionTextureCoordinates() and @ref emissiveTextureCoordinates()
         * have the same value, @cpp false @ce otherwise. In particular,
         * returns @cpp true @ce also if there's no extra texture coordinate
         * set used at all. Use @ref hasTextureCoordinates() to distinguish
         * that case.
         */
        bool hasCommonTextureCoordinates() const;

        /**
         * @brief Base color
         *
         * Convenience access to the @ref MaterialAttribute::BaseColor
         * attribute. If not present, the default is @cpp 0xffffffff_srgbaf @ce.
         *
         * If the material has @ref MaterialAttribute::BaseColorTexture, the
         * color and texture is meant to be multiplied together.
         */
        Color4 baseColor() const;

        /**
         * @brief Base color texture ID
         *
         * Available only if @ref MaterialAttribute::BaseColorTexture is
         * present. Meant to be multiplied with @ref baseColor().
         * @see @ref hasAttribute(), @ref AbstractImporter::texture()
         */
        UnsignedInt baseColorTexture() const;

        /**
         * @brief Base color texture coordinate transformation matrix
         *
         * Convenience access to the @ref MaterialAttribute::BaseColorTextureMatrix
         * / @ref MaterialAttribute::TextureMatrix attributes. If neither is
         * present, the default is an identity matrix. Available only if the
         * material has @ref MaterialAttribute::BaseColorTexture.
         * @see @ref hasAttribute()
         */
        Matrix3 baseColorTextureMatrix() const;

        /**
         * @brief Base color texture coordinate set
         *
         * Convenience access to the @ref MaterialAttribute::BaseColorTextureCoordinates
         * / @ref MaterialAttribute::TextureCoordinates attributes. If neither
         * is present, the default is @cpp 0 @ce. Available only if the
         * material has @ref MaterialAttribute::BaseColorTexture.
         * @see @ref hasAttribute()
         */
        UnsignedInt baseColorTextureCoordinates() const;

        /**
         * @brief Metalness factor
         *
         * Convenience access to the @ref MaterialAttribute::Metalness
         * attribute. If not present, the default is @cpp 1.0f @ce.
         *
         * If the material has a metalness texture, the factor and texture is
         * meant to be multiplied together.
         * @see @ref hasMetalnessTexture()
         */
        Float metalness() const;

        /**
         * @brief Metalness texture ID
         *
         * Available only if either @ref MaterialAttribute::MetalnessTexture or
         * @ref MaterialAttribute::NoneRoughnessMetallicTexture is present.
         * Meant to be multiplied with @ref metalness().
         * @see @ref hasMetalnessTexture(), @ref AbstractImporter::texture()
         */
        UnsignedInt metalnessTexture() const;

        /**
         * @brief Metalness texture swizzle
         *
         * If @ref MaterialAttribute::NoneRoughnessMetallicTexture is present,
         * returns always @ref MaterialTextureSwizzle::B. Otherwise returns the
         * @ref MaterialAttribute::MetalnessTextureSwizzle attribute, or
         * @ref MaterialTextureSwizzle::R, if it's not present. Available only
         * if the material has a metalness texture.
         * @see @ref hasMetalnessTexture()
         */
        MaterialTextureSwizzle metalnessTextureSwizzle() const;

        /**
         * @brief Metalness texture coordinate transformation matrix
         *
         * Convenience access to the @ref MaterialAttribute::MetalnessTextureMatrix
         * / @ref MaterialAttribute::TextureMatrix attributes. If neither is
         * present, the default is an identity matrix. Available only if the
         * material has a metalness texture.
         * @see @ref hasMetalnessTexture()
         */
        Matrix3 metalnessTextureMatrix() const;

        /**
         * @brief Metalness texture coordinate set
         *
         * Convenience access to the @ref MaterialAttribute::MetalnessTextureCoordinates
         * / @ref MaterialAttribute::TextureCoordinates attributes. If neither
         * is present, the default is @cpp 0 @ce. Available only if the
         * material has a metalness texture.
         * @see @ref hasMetalnessTexture()
         */
        UnsignedInt metalnessTextureCoordinates() const;

        /**
         * @brief Roughness factor
         *
         * Convenience access to the @ref MaterialAttribute::Roughness
         * attribute. If not present, the default is @cpp 1.0f @ce.
         *
         * If the material has a roughness texture, the factor and texture is
         * meant to be multiplied together.
         * @see @ref hasRoughnessTexture()
         */
        Float roughness() const;

        /**
         * @brief Roughness texture ID
         *
         * Available only if either @ref MaterialAttribute::RoughnessTexture or
         * @ref MaterialAttribute::NoneRoughnessMetallicTexture is present. Meant
         * to be multiplied with @ref roughness().
         * @see @ref hasRoughnessTexture(), @ref AbstractImporter::texture()
         */
        UnsignedInt roughnessTexture() const;

        /**
         * @brief Roughness texture swizzle
         *
         * If @ref MaterialAttribute::NoneRoughnessMetallicTexture is present,
         * returns always @ref MaterialTextureSwizzle::G. Otherwise returns the
         * @ref MaterialAttribute::RoughnessTextureSwizzle attribute, or
         * @ref MaterialTextureSwizzle::R, if it's not present. Available only
         * if the material has a roughness texture.
         * @see @ref hasRoughnessTexture()
         */
        MaterialTextureSwizzle roughnessTextureSwizzle() const;

        /**
         * @brief Roughness texture coordinate transformation matrix
         *
         * Convenience access to the @ref MaterialAttribute::RoughnessTextureMatrix
         * / @ref MaterialAttribute::TextureMatrix attributes. If neither is
         * present, the default is an identity matrix. Available only if the
         * material has a roughness texture.
         * @see @ref hasRoughnessTexture()
         */
        Matrix3 roughnessTextureMatrix() const;

        /**
         * @brief Roughness texture coordinate set
         *
         * Convenience access to the @ref MaterialAttribute::RoughnessTextureCoordinates
         * / @ref MaterialAttribute::TextureCoordinates attributes. If neither
         * is present, the default is @cpp 0 @ce. Available only if the
         * material has a roughness texture.
         * @see @ref hasRoughnessTexture()
         */
        UnsignedInt roughnessTextureCoordinates() const;

        /**
         * @brief Normal texture ID
         *
         * Available only if @ref MaterialAttribute::NormalTexture is present.
         * @see @ref hasAttribute(), @ref AbstractImporter::texture()
         */
        UnsignedInt normalTexture() const;

        /**
         * @brief Normal texture scale
         *
         * Convenience access to the @ref MaterialAttribute::NormalTextureScale
         * attribute. If not present, the default is @cpp 1.0f @ce.
         * Available only if @ref MaterialAttribute::NormalTexture is present.
         * @see @ref hasAttribute()
         */
        Float normalTextureScale() const;

        /**
         * @brief Normal texture coordinate transformation matrix
         *
         * Convenience access to the @ref MaterialAttribute::NormalTextureMatrix
         * / @ref MaterialAttribute::TextureMatrix attributes. If neither is
         * present, the default is an identity matrix. Available only if the
         * material has @ref MaterialAttribute::NormalTexture.
         * @see @ref hasAttribute(), @ref AbstractImporter::texture()
         */
        Matrix3 normalTextureMatrix() const;

        /**
         * @brief Normal texture swizzle
         * @m_since_latest
         *
         * Convenience access to the
         * @ref MaterialAttribute::NormalTextureSwizzle attribute. If not
         * present, the default is @ref MaterialTextureSwizzle::RGB. Available
         * only if @ref MaterialAttribute::NormalTexture is present.
         * @see @ref hasAttribute()
         */
        MaterialTextureSwizzle normalTextureSwizzle() const;

        /**
         * @brief Normal texture coordinate set
         *
         * Convenience access to the @ref MaterialAttribute::NormalTextureCoordinates
         * / @ref MaterialAttribute::TextureCoordinates attributes. If neither is
         * present, the default is @cpp 0 @ce. Available only if the material
         * has @ref MaterialAttribute::NormalTexture.
         * @see @ref hasAttribute(), @ref AbstractImporter::texture()
         */
        UnsignedInt normalTextureCoordinates() const;

        /**
         * @brief Occlusion texture ID
         *
         * Available only if @ref MaterialAttribute::OcclusionTexture is present.
         * @see @ref hasAttribute(), @ref AbstractImporter::texture()
         */
        UnsignedInt occlusionTexture() const;

        /**
         * @brief Occlusion texture strength
         *
         * Convenience access to the @ref MaterialAttribute::OcclusionTextureStrength
         * attribute. If not present, the default is @cpp 1.0f @ce.
         * Available only if @ref MaterialAttribute::OcclusionTexture is
         * present.
         * @see @ref hasAttribute()
         */
        Float occlusionTextureStrength() const;

        /**
         * @brief Occlusion texture swizzle
         *
         * Convenience access to the
         * @ref MaterialAttribute::OcclusionTextureSwizzle attribute. If not
         * present, the default is @ref MaterialTextureSwizzle::R. Available
         * only if @ref MaterialAttribute::OcclusionTexture is present.
         * @see @ref hasAttribute()
         */
        MaterialTextureSwizzle occlusionTextureSwizzle() const;

        /**
         * @brief Occlusion texture coordinate transformation matrix
         *
         * Convenience access to the @ref MaterialAttribute::OcclusionTextureMatrix
         * / @ref MaterialAttribute::TextureMatrix attributes. If neither is
         * present, the default is an identity matrix. Available only if the
         * material has @ref MaterialAttribute::OcclusionTexture.
         * @see @ref hasAttribute()
         */
        Matrix3 occlusionTextureMatrix() const;

        /**
         * @brief Occlusion texture coordinate set
         *
         * Convenience access to the @ref MaterialAttribute::OcclusionTextureCoordinates
         * / @ref MaterialAttribute::TextureCoordinates attributes. If neither
         * is present, the default is @cpp 0 @ce. Available only if the
         * material has @ref MaterialAttribute::OcclusionTexture.
         * @see @ref hasAttribute()
         */
        UnsignedInt occlusionTextureCoordinates() const;

        /**
         * @brief Emissive color
         *
         * Convenience access to the @ref MaterialAttribute::EmissiveColor
         * attribute. If not present, the default is @cpp 0x000000_srgbf @ce
         * (i.e, no emission).
         *
         * If the material has @ref MaterialAttribute::EmissiveTexture, the
         * color and texture is meant to be multiplied together.
         */
        Color3 emissiveColor() const;

        /**
         * @brief Emissive texture ID
         *
         * Available only if @ref MaterialAttribute::EmissiveTexture is present.
         * Meant to be multiplied with @ref emissiveColor().
         * @see @ref hasAttribute(), @ref AbstractImporter::texture()
         */
        UnsignedInt emissiveTexture() const;

        /* No EmissiveTextureSwizzle attribute right now (implicitly RGB) */

        /**
         * @brief Emissive texture coordinate transformation matrix
         *
         * Convenience access to the @ref MaterialAttribute::EmissiveTextureMatrix
         * / @ref MaterialAttribute::TextureMatrix attributes. If neither is
         * present, the default is an identity matrix. Available only if the
         * material has @ref MaterialAttribute::EmissiveTexture.
         * @see @ref hasAttribute()
         */
        Matrix3 emissiveTextureMatrix() const;

        /**
         * @brief Emissive texture coordinate set
         *
         * Convenience access to the @ref MaterialAttribute::EmissiveTextureCoordinates
         * / @ref MaterialAttribute::TextureCoordinates attributes. If neither
         * is present, the default is @cpp 0 @ce. Available only if the
         * material has @ref MaterialAttribute::EmissiveTexture.
         * @see @ref hasAttribute()
         */
        UnsignedInt emissiveTextureCoordinates() const;

        /**
         * @brief Common texture coordinate transformation matrix for all textures
         *
         * Expects that @ref hasCommonTextureTransformation() is @cpp true @ce;
         * returns a coordinate set index that's the same for all of
         * @ref baseColorTextureMatrix(), @ref metalnessTextureMatrix(),
         * @ref roughnessTextureMatrix(), @ref normalTextureMatrix(),
         * @ref occlusionTextureMatrix() and @ref emissiveTextureMatrix() where
         * a texture is present. If no texture is present, returns an identity
         * matrix.
         */
        Matrix3 commonTextureMatrix() const;

        /**
         * @brief Common texture coordinate set index for all textures
         *
         * Expects that @ref hasCommonTextureCoordinates() is @cpp true @ce;
         * returns a coordinate set index that's the same for all of
         * @ref baseColorTextureCoordinates(), @ref metalnessTextureCoordinates(),
         * @ref roughnessTextureCoordinates(), @ref normalTextureCoordinates(),
         * @ref occlusionTextureCoordinates() and @ref emissiveTextureCoordinates()
         * where a texture is present. If no texture is present, returns
         * @cpp 0 @ce.
         */
        UnsignedInt commonTextureCoordinates() const;
};

}}

#endif
