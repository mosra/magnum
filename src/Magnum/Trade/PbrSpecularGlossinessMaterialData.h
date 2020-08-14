#ifndef Magnum_Trade_PbrSpecularGlossinessMaterialData_h
#define Magnum_Trade_PbrSpecularGlossinessMaterialData_h
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
 * @brief Class @ref Magnum::Trade::PbrSpecularGlossinessMaterialData
 * @m_since_latest
 */

#include "Magnum/Trade/MaterialData.h"

namespace Magnum { namespace Trade {

/**
@brief PBR specular/glossiness material data
@m_since_latest

See @ref Trade-MaterialData-usage-types for more information about how to use
this class.
@see @ref AbstractImporter::material(), @ref PhongMaterialData,
    @ref FlatMaterialData, @ref PbrMetallicRoughnessMaterialData,
    @ref PbrClearCoatMaterialData, @ref MaterialType::PbrSpecularGlossiness
*/
class MAGNUM_TRADE_EXPORT PbrSpecularGlossinessMaterialData: public MaterialData {
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
         * @brief Whether the material has a specular texture
         *
         * Returns @cpp true @ce if any of the
         * @ref MaterialAttribute::SpecularTexture or
         * @ref MaterialAttribute::SpecularGlossinessTexture attributes is
         * present, @cpp false @ce otherwise.
         * @see @ref hasGlossinessTexture(),
         *      @ref hasSpecularGlossinessTexture()
         */
        bool hasSpecularTexture() const;

        /**
         * @brief Whether the material has a glossiness texture
         *
         * Returns @cpp true @ce if any of the
         * @ref MaterialAttribute::GlossinessTexture or
         * @ref MaterialAttribute::SpecularGlossinessTexture attributes is
         * present, @cpp false @ce otherwise.
         * @see @ref hasSpecularTexture(),
         *      @ref hasSpecularGlossinessTexture()
         */
        bool hasGlossinessTexture() const;

        /**
         * @brief Whether the material has a combined specular/glossiness texture
         *
         * Returns @cpp true @ce if either the
         * @ref MaterialAttribute::SpecularGlossinessTexture attribute is
         * present or both @ref MaterialAttribute::SpecularTexture and
         * @ref MaterialAttribute::GlossinessTexture are present, point to
         * the same texture ID and @ref MaterialAttribute::GlossinessTextureSwizzle
         * is set to @ref MaterialTextureSwizzle::A, and ddditionally
         * @ref MaterialAttribute::SpecularTextureMatrix and
         * @ref MaterialAttribute::GlossinessTextureMatrix are both either not
         * present or have the same value, and
         * @ref MaterialAttribute::SpecularTextureCoordinates and
         * @ref MaterialAttribute::GlossinessTextureCoordinates are both either
         * not present or have the same value; @cpp false @ce otherwise.
         *
         * In other words, if this function returns @cpp true @ce,
         * @ref specularTexture(), @ref specularTextureMatrix() and
         * @ref specularTextureCoordinates() return values common for both
         * specular and glossiness texture, and the two are packed together
         * with specular occupying the RGB channels and glossiness the alpha.
         * @see @ref hasSpecularTexture(), @ref hasGlossinessTexture()
         */
        bool hasSpecularGlossinessTexture() const;

        /**
         * @brief Whether the material has texture transformation
         *
         * Returns @cpp true @ce if any of the
         * @ref MaterialAttribute::DiffuseTextureMatrix,
         * @ref MaterialAttribute::SpecularTextureMatrix,
         * @ref MaterialAttribute::GlossinessTextureMatrix,
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
         * @ref diffuseTextureMatrix(), @ref specularTextureMatrix(),
         * @ref glossinessTextureMatrix(), @ref normalTextureMatrix(),
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
         * @ref MaterialAttribute::DiffuseTextureCoordinates,
         * @ref MaterialAttribute::SpecularTextureCoordinates,
         * @ref MaterialAttribute::GlossinessTextureCoordinates,
         * @ref MaterialAttribute::NormalTextureCoordinates,
         * @ref MaterialAttribute::OcclusionTextureCoordinates,
         * @ref MaterialAttribute::EmissiveTextureCoordinates or
         * @ref MaterialAttribute::TextureCoordinates attributes is present and
         * has a non-zero value, @cpp false @ce otherwise.
         */
        bool hasTextureCoordinates() const;

        /**
         * @brief Whether the material has a common coordinate set for all textures
         *
         * Returns @cpp true @ce if, for each texture that is present,
         * @ref diffuseTextureCoordinates(), @ref specularTextureCoordinates(),
         * @ref glossinessTextureCoordinates(), @ref normalTextureCoordinates(),
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
         * Convenience access to the @ref MaterialAttribute::DiffuseColor
         * attribute. If not present, the default is @cpp 0xffffffff_srgbaf @ce.
         *
         * If the material has @ref MaterialAttribute::DiffuseTexture, the
         * color and texture is meant to be multiplied together.
         */
        Color4 diffuseColor() const;

        /**
         * @brief Base color texture ID
         *
         * Available only if @ref MaterialAttribute::DiffuseTexture is
         * present. Meant to be multiplied with @ref diffuseColor().
         * @see @ref hasAttribute(), @ref AbstractImporter::texture()
         */
        UnsignedInt diffuseTexture() const;

        /**
         * @brief Base color texture coordinate transformation matrix
         *
         * Convenience access to the @ref MaterialAttribute::DiffuseTextureMatrix
         * / @ref MaterialAttribute::TextureMatrix attributes. If neither is
         * present, the default is an identity matrix. Available only if the
         * material has @ref MaterialAttribute::DiffuseTexture.
         * @see @ref hasAttribute()
         */
        Matrix3 diffuseTextureMatrix() const;

        /**
         * @brief Base color texture coordinate set
         *
         * Convenience access to the @ref MaterialAttribute::DiffuseTextureCoordinates
         * / @ref MaterialAttribute::TextureCoordinates attributes. If neither is
         * present, the default is @cpp 0 @ce. Available only if the material
         * has @ref MaterialAttribute::DiffuseTexture.
         * @see @ref hasAttribute()
         */
        UnsignedInt diffuseTextureCoordinates() const;

        /**
         * @brief Specular color
         *
         * Convenience access to the @ref MaterialAttribute::SpecularColor
         * attribute. If not present, the default is @cpp 0xffffff00_srgbaf @ce.
         *
         * If the material has a specular texture, the color and texture is
         * meant to be multiplied together.
         * @see @ref hasSpecularTexture()
         */
        Color4 specularColor() const;

        /**
         * @brief Specular texture ID
         *
         * Available only if either @ref MaterialAttribute::SpecularTexture or
         * @ref MaterialAttribute::SpecularGlossinessTexture is present. Meant
         * to be multiplied with @ref specularColor().
         * @see @ref hasSpecularTexture(), @ref AbstractImporter::texture()
         */
        UnsignedInt specularTexture() const;

        /**
         * @brief Specular texture swizzle
         *
         * If @ref MaterialAttribute::SpecularGlossinessTexture is present,
         * returns always @ref MaterialTextureSwizzle::RGB. Otherwise returns
         * the @ref MaterialAttribute::SpecularTextureSwizzle attribute, or
         * @ref MaterialTextureSwizzle::RGB if it's not present. Available only
         * if the material has a specular texture.
         * @see @ref hasSpecularTexture()
         */
        MaterialTextureSwizzle specularTextureSwizzle() const;

        /**
         * @brief Specular texture coordinate transformation matrix
         *
         * Convenience access to the @ref MaterialAttribute::SpecularTextureMatrix
         * / @ref MaterialAttribute::TextureMatrix attributes. If neither is
         * present, the default is an identity matrix. Available only if the
         * material has a specular texture.
         * @see @ref hasSpecularTexture()
         */
        Matrix3 specularTextureMatrix() const;

        /**
         * @brief Specular texture coordinate set
         *
         * Convenience access to the @ref MaterialAttribute::SpecularTextureCoordinates
         * / @ref MaterialAttribute::TextureCoordinates attributes. If neither is
         * present, the default is @cpp 0 @ce. Available only if the material
         * has a specular texture.
         * @see @ref hasSpecularTexture()
         */
        UnsignedInt specularTextureCoordinates() const;

        /**
         * @brief Glossiness factor
         *
         * Convenience access to the @ref MaterialAttribute::Glossiness
         * attribute. If not present, the default is @cpp 1.0f @ce.
         *
         * If the material has a glossiness texture, the factor and texture is
         * meant to be multiplied together.
         * @see @ref hasGlossinessTexture()
         */
        Float glossiness() const;

        /**
         * @brief Glossiness texture ID
         *
         * Available only if either @ref MaterialAttribute::GlossinessTexture or
         * @ref MaterialAttribute::SpecularGlossinessTexture is present. Meant
         * to be multiplied with @ref glossiness().
         * @see @ref hasGlossinessTexture(), @ref AbstractImporter::texture()
         */
        UnsignedInt glossinessTexture() const;

        /**
         * @brief Glossiness texture swizzle
         *
         * If @ref MaterialAttribute::SpecularGlossinessTexture is present,
         * returns always @ref MaterialTextureSwizzle::A. Otherwise returns the
         * @ref MaterialAttribute::GlossinessTextureSwizzle attribute, or
         * @ref MaterialTextureSwizzle::R if it's not present. Available only
         * if the material has a glossiness texture.
         * @see @ref hasGlossinessTexture()
         */
        MaterialTextureSwizzle glossinessTextureSwizzle() const;

        /**
         * @brief Glossiness texture coordinate transformation matrix
         *
         * Convenience access to the @ref MaterialAttribute::GlossinessTextureMatrix
         * / @ref MaterialAttribute::TextureMatrix attributes. If neither is
         * present, the default is an identity matrix. Available only if the
         * material has a glossiness texture.
         * @see @ref hasGlossinessTexture()
         */
        Matrix3 glossinessTextureMatrix() const;

        /**
         * @brief Glossiness texture coordinate set
         *
         * Convenience access to the @ref MaterialAttribute::GlossinessTextureCoordinates
         * / @ref MaterialAttribute::TextureCoordinates attributes. If neither is
         * present, the default is @cpp 0 @ce. Available only if the material
         * has a glossiness texture.
         * @see @ref hasGlossinessTexture()
         */
        UnsignedInt glossinessTextureCoordinates() const;

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
         * @brief Normal texture swizzle
         *
         * Convenience access to the
         * @ref MaterialAttribute::NormalTextureSwizzle attribute. If not
         * present, the default is @ref MaterialTextureSwizzle::RGB. Available
         * only if @ref MaterialAttribute::NormalTexture is present.
         * @see @ref hasAttribute()
         */
        MaterialTextureSwizzle normalTextureSwizzle() const;

        /**
         * @brief Normal texture coordinate transformation matrix
         *
         * Convenience access to the @ref MaterialAttribute::NormalTextureMatrix
         * / @ref MaterialAttribute::TextureMatrix attributes. If neither is
         * present, the default is an identity matrix. Available only if the
         * material has @ref MaterialAttribute::NormalTexture.
         * @see @ref hasAttribute()
         */
        Matrix3 normalTextureMatrix() const;

        /**
         * @brief Normal texture coordinate set
         *
         * Convenience access to the @ref MaterialAttribute::NormalTextureCoordinates
         * / @ref MaterialAttribute::TextureCoordinates attributes. If neither is
         * present, the default is @cpp 0 @ce. Available only if the material
         * has @ref MaterialAttribute::NormalTexture.
         * @see @ref hasAttribute()
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
         * @see @ref hasAttribute(), @ref AbstractImporter::texture()
         */
        Matrix3 occlusionTextureMatrix() const;

        /**
         * @brief Occlusion texture coordinate set
         *
         * Convenience access to the @ref MaterialAttribute::OcclusionTextureCoordinates
         * / @ref MaterialAttribute::TextureCoordinates attributes. If neither is
         * present, the default is @cpp 0 @ce. Available only if the material
         * has @ref MaterialAttribute::OcclusionTexture.
         * @see @ref hasAttribute(), @ref AbstractImporter::texture()
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
         * @see @ref hasAttribute(), @ref AbstractImporter::texture()
         */
        Matrix3 emissiveTextureMatrix() const;

        /**
         * @brief Emissive texture coordinate set
         *
         * Convenience access to the @ref MaterialAttribute::EmissiveTextureCoordinates
         * / @ref MaterialAttribute::TextureCoordinates attributes. If neither
         * is present, the default is @cpp 0 @ce. Available only if the
         * material has @ref MaterialAttribute::EmissiveTexture.
         * @see @ref hasAttribute(), @ref AbstractImporter::texture()
         */
        UnsignedInt emissiveTextureCoordinates() const;

        /**
         * @brief Common texture coordinate transformation matrix for all textures
         *
         * Expects that @ref hasCommonTextureTransformation() is @cpp true @ce;
         * returns a coordinate set index that's the same for all of
         * @ref diffuseTextureMatrix(), @ref specularTextureMatrix(),
         * @ref glossinessTextureMatrix(), @ref normalTextureMatrix(),
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
         * @ref diffuseTextureCoordinates(), @ref specularTextureCoordinates(),
         * @ref glossinessTextureCoordinates(), @ref normalTextureCoordinates(),
         * @ref occlusionTextureCoordinates() and @ref emissiveTextureCoordinates()
         * where a texture is present. If no texture is present, returns
         * @cpp 0 @ce.
         */
        UnsignedInt commonTextureCoordinates() const;
};

}}

#endif
