#ifndef Magnum_Trade_PbrClearCoatMaterialData_h
#define Magnum_Trade_PbrClearCoatMaterialData_h
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
 * @brief Class @ref Magnum::Trade::PbrClearCoatMaterialData
 * @m_since_latest
 */

#include "Magnum/Trade/MaterialLayerData.h"

namespace Magnum { namespace Trade {

/**
@brief Clear coat material layer data
@m_since_latest

Exposes properties of a @ref MaterialLayer::ClearCoat layer. All APIs expect
that the layer is present in the material. See @ref Trade-MaterialData-usage-types
for more information about how to use this class.
@see @ref AbstractImporter::material(), @ref FlatMaterialData,
    @ref PhongMaterialData, @ref PbrMetallicRoughnessMaterialData,
    @ref PbrSpecularGlossinessMaterialData, @ref MaterialType::PbrClearCoat
*/
class MAGNUM_TRADE_EXPORT PbrClearCoatMaterialData: public MaterialLayerData<MaterialLayer::ClearCoat> {
    public:
        #ifndef DOXYGEN_GENERATING_OUTPUT
        /* Allow constructing subclasses directly. While not used in the
           general Importer workflow, it allows users to create instances with
           desired convenience APIs easier (and simplifies testing). It's
           however hidden from the docs as constructing instances this way
           isn't really common and it would add a lot of noise. */
        using MaterialLayerData<MaterialLayer::ClearCoat>::MaterialLayerData;
        #endif

        /**
         * @brief Whether the material has a combined layer factor / roughness texture
         *
         * Returns @cpp true @ce if both
         * @ref MaterialAttribute::LayerFactorTexture and
         * @ref MaterialAttribute::RoughnessTexture attributes are present,
         * point to the same texture ID,
         * @ref MaterialAttribute::LayerFactorTextureSwizzle is either not
         * present or set to @ref MaterialTextureSwizzle::R and
         * @ref MaterialAttribute::RoughnessTextureSwizzle is set to
         * @ref MaterialTextureSwizzle::G, and additionally
         * @ref MaterialAttribute::LayerFactorTextureMatrix and
         * @ref MaterialAttribute::RoughnessTextureMatrix are both either not
         * present or have the same value, and
         * @ref MaterialAttribute::LayerFactorTextureCoordinates and
         * @ref MaterialAttribute::RoughnessTextureCoordinates are both either
         * not present or have the smae value; @cpp false @ce otherwise.
         *
         * In other words, if this function returns @cpp true @ce,
         * @ref layerFactorTexture(), @ref layerFactorTextureMatrix() and
         * @ref layerFactorTextureCoordinates() return values common for both
         * layer factor and roughness texture, and the two are packed together
         * with layer factor occupying the R channel and roughness the G
         * channel. This check is present in order to provide support for the
         * [KHR_materials_clearcoat](https://github.com/KhronosGroup/glTF/tree/master/extensions/2.0/Khronos/KHR_materials_clearcoat/README.md)
         * glTF extension.
         * @see @ref hasAttribute()
         */
        bool hasLayerFactorRoughnessTexture() const;

        /**
         * @brief Whether the material has texture transformation
         *
         * Returns @cpp true @ce if any of the
         * @ref MaterialAttribute::LayerFactorTextureMatrix,
         * @ref MaterialAttribute::RoughnessTextureMatrix,
         * @ref MaterialAttribute::NormalTextureMatrix or
         * @ref MaterialAttribute::TextureMatrix attributes are present in this
         * layer or if @ref MaterialAttribute::TextureMatrix is present in the
         * base material, @cpp false @ce otherwise
         */
        bool hasTextureTransformation() const;

        /**
         * @brief Whether the material has a common transformation for all textures
         *
         * Returns @cpp true @ce if, for each texture that is present,
         * @ref layerFactorTextureMatrix(), @ref roughnessTextureMatrix() and
         * @ref normalTextureMatrix() have the same value, @cpp false @ce
         * otherwise. In particular, returns @cpp true @ce also if there's no
         * texture transformation at all. Use @ref hasTextureTransformation()
         * to distinguish that case.
         */
        bool hasCommonTextureTransformation() const;

        /**
         * @brief Whether the material uses extra texture coordinate sets
         *
         * Returns @cpp true @ce if any of the
         * @ref MaterialAttribute::LayerFactorTextureCoordinates,
         * @ref MaterialAttribute::RoughnessTextureCoordinates,
         * @ref MaterialAttribute::NormalTextureCoordinates or
         * @ref MaterialAttribute::TextureCoordinates attributes are present in
         * this material or if @ref MaterialAttribute::TextureCoordinates is
         * present in the base material, @cpp false @ce otherwise.
         */
        bool hasTextureCoordinates() const;

        /**
         * @brief Whether the material has a common coordinate set for all textures
         *
         * Returns @cpp true @ce if, for each texture that is present,
         * @ref layerFactorTextureCoordinates(),
         * @ref roughnessTextureCoordinates() and
         * @ref normalTextureCoordinates() have the same value, @cpp false @ce
         * otherwise. In particular, returns @cpp true @ce also if there's no
         * extra texture coordinate set used at all. Use
         * @ref hasTextureCoordinates() to distinguish that case.
         */
        bool hasCommonTextureCoordinates() const;

        /**
         * @brief Roughness factor
         *
         * Convenience access to the @ref MaterialAttribute::Roughness
         * attribute in this layer. If not present, the default is @cpp 1.0f @ce.
         *
         * If the layer has a @ref MaterialAttribute::RoughnessTexture, the
         * factor and texture is meant to be multiplied together.
         */
        Float roughness() const;

        /**
         * @brief Roughness texture ID
         *
         * Available only if @ref MaterialAttribute::RoughnessTexture is
         * present in this layer. Meant to be multiplied with @ref roughness().
         * @see @ref AbstractImporter::texture()
         */
        UnsignedInt roughnessTexture() const;

        /**
         * @brief Roughness texture swizzle
         *
         * Convenience access to the @ref MaterialAttribute::RoughnessTextureSwizzle
         * attribute in this layer. If not present, the default is @cpp 1.0f @ce.
         * Available only if @ref MaterialAttribute::RoughnessTexture is
         * present in this layer.
         * @see @ref hasAttribute()
         */
        MaterialTextureSwizzle roughnessTextureSwizzle() const;

        /**
         * @brief Roughness texture coordinate transformation matrix
         *
         * Convenience access to the @ref MaterialAttribute::RoughnessTextureMatrix
         * / @ref MaterialAttribute::TextureMatrix attributes in this layer or
         * a @ref MaterialAttribute::TextureMatrix attribute in the base
         * material. If neither is present, the default is an identity matrix.
         * Available only if @ref MaterialAttribute::RoughnessTexture is
         * present in this layer.
         * @see @ref hasAttribute()
         */
        Matrix3 roughnessTextureMatrix() const;

        /**
         * @brief Roughness texture coordinate set
         *
         * Convenience access to the @ref MaterialAttribute::RoughnessTextureCoordinates
         * / @ref MaterialAttribute::TextureCoordinates attributes in this
         * layer or a @ref MaterialAttribute::TextureCoordinates attribute in
         * the base material. If neither is present, the default is @cpp 0 @ce.
         * Available only if @ref MaterialAttribute::RoughnessTexture is
         * present in this layer.
         * @see @ref hasAttribute()
         */
        UnsignedInt roughnessTextureCoordinates() const;

        /**
         * @brief Normal texture ID
         *
         * Available only if @ref MaterialAttribute::NormalTexture is present
         * in this layer.
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
         * @ref MaterialAttribute::NormalTextureSwizzle attribute in this
         * layer. If not present, the default is
         * @ref MaterialTextureSwizzle::RGB. Available only if
         * @ref MaterialAttribute::NormalTexture is present in this layer.
         *
         * The texture can be also just two-component, in which case the
         * remaining component is implicit and calculated as
         * @f$ z = \sqrt{1 - x^2 - y^2} @f$.
         * @see @ref hasAttribute()
         */
        MaterialTextureSwizzle normalTextureSwizzle() const;

        /**
         * @brief Normal texture coordinate transformation matrix
         *
         * Convenience access to the @ref MaterialAttribute::NormalTextureMatrix
         * / @ref MaterialAttribute::TextureMatrix attributes in this layer or
         * a @ref MaterialAttribute::TextureMatrix attribute in the base
         * material. If neither is present, the default is an identity matrix.
         * Available only if @ref MaterialAttribute::NormalTexture is present
         * in this layer.
         * @see @ref hasAttribute()
         */
        Matrix3 normalTextureMatrix() const;

        /**
         * @brief Normal texture coordinate set
         *
         * Convenience access to the @ref MaterialAttribute::NormalTextureCoordinates
         * / @ref MaterialAttribute::TextureCoordinates attributes in this
         * layer or a @ref MaterialAttribute::TextureCoordinates attribute in
         * the base material. If neither is present, the default is @cpp 0 @ce.
         * Available only if @ref MaterialAttribute::NormalTexture is present
         * in this layer.
         * @see @ref hasAttribute()
         */
        UnsignedInt normalTextureCoordinates() const;

        /**
         * @brief Common texture coordinate transformation matrix for all textures
         *
         * Expects that @ref hasCommonTextureTransformation() is @cpp true @ce;
         * returns a coordinate set index that's the same for all of
         * @ref layerFactorTextureMatrix(), @ref roughnessTextureMatrix() and
         * @ref normalTextureMatrix() where a texture is present. If no texture
         * is present, returns an identity matrix.
         */
        Matrix3 commonTextureMatrix() const;

        /**
         * @brief Common texture coordinate set index for all textures
         *
         * Expects that @ref hasCommonTextureCoordinates() is @cpp true @ce;
         * returns a coordinate set index that's the same for all of
         * @ref layerFactorTextureCoordinates(), @ref roughnessTextureCoordinates()
         * and @ref normalTextureCoordinates() where a texture is present. If
         * no texture is present, returns @cpp 0 @ce.
         */
        UnsignedInt commonTextureCoordinates() const;
};

}}

#endif
