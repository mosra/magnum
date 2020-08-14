#ifndef Magnum_Trade_FlatMaterialData_h
#define Magnum_Trade_FlatMaterialData_h
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
 * @brief Class @ref Magnum::Trade::FlatMaterialData
 * @m_since_latest
 */

#include "Magnum/Trade/MaterialData.h"

namespace Magnum { namespace Trade {

/**
@brief Flat material data
@m_since_latest

See @ref Trade-MaterialData-usage-types for more information about how to use
this class.
@see @ref AbstractImporter::material(), @ref PhongMaterialData,
    @ref PbrMetallicRoughnessMaterialData,
    @ref PbrSpecularGlossinessMaterialData, @ref PbrClearCoatMaterialData,
    @ref MaterialType::Flat
*/
class MAGNUM_TRADE_EXPORT FlatMaterialData: public MaterialData {
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
         * @ref MaterialAttribute::BaseColorTexture or
         * @ref MaterialAttribute::DiffuseTexture attributes is present,
         * @cpp false @ce otherwise.
         */
        bool hasTexture() const;

        /**
         * @brief Whether the material has texture transformation
         *
         * Returns @cpp true @ce if the material is textured and a
         * @ref MaterialAttribute::BaseColorTextureMatrix,
         * @ref MaterialAttribute::DiffuseTextureMatrix or
         * @ref MaterialAttribute::TextureMatrix attribute matching the texture
         * is present, @cpp false @ce otherwise. In particular, if there's for
         * example a @ref MaterialAttribute::BaseColorTexture but only a
         * @ref MaterialAttribute::DiffuseTextureMatrix,
         * returns @cpp false @ce.
         */
        bool hasTextureTransformation() const;

        /* Since there's just one texture, no need for any
           hasCommonTextureTransformation(), hasCommonTextureCoordinates(),
           commonTextureMatrix() or commonTextureCoordinates() APIs */

        /**
         * @brief Whether the material uses extra texture coordinate sets
         *
         * Returns @cpp true @ce if the material is textured an a
         * @ref MaterialAttribute::BaseColorTextureCoordinates,
         * @ref MaterialAttribute::DiffuseTextureCoordinates or
         * @ref MaterialAttribute::TextureCoordinates attribute matching the
         * texture is present and has a non-zero value, @cpp false @ce
         * otherwise. In particular, if there's for example a
         * @ref MaterialAttribute::BaseColorTexture but
         * only a @ref MaterialAttribute::DiffuseTextureCoordinates, returns
         * @cpp false @ce.
         */
        bool hasTextureCoordinates() const;

        /**
         * @brief Color
         *
         * Convenience access to the @ref MaterialAttribute::BaseColor /
         * @ref MaterialAttribute::DiffuseColor attributes. If neither of them
         * is present, the default is @cpp 0xffffffff_srgbaf @ce.
         *
         * If the material has a texture, the color attribute matching the
         * texture is picked (instead of combining e.g. a
         * @ref MaterialAttribute::BaseColor with @ref MaterialAttribute::DiffuseTexture). The color and texture is meant to
         * be multiplied together.
         * @see @ref hasAttribute(), @ref hasTexture()
         */
        Color4 color() const;

        /**
         * @brief Texture ID
         *
         * Available only if either @ref MaterialAttribute::BaseColorTexture or
         * @ref MaterialAttribute::DiffuseTexture is present. Meant to be
         * multiplied with @ref color().
         * @see @ref hasTexture(), @ref AbstractImporter::texture()
         */
        UnsignedInt texture() const;

        /**
         * @brief Texture coordinate transformation matrix
         *
         * Convenience access to the @ref MaterialAttribute::DiffuseTextureMatrix
         * / @ref MaterialAttribute::BaseColorTextureMatrix /
         * @ref MaterialAttribute::TextureMatrix attributes, picking the one
         * matching the texture (instead of combining e.g. a
         * @ref MaterialAttribute::BaseColorTexture with
         * @ref MaterialAttribute::DiffuseTextureMatrix). If no matching
         * attribute is present, the default is an identity matrix. Available
         * only if the material has a texture.
         * @see @ref hasTexture()
         */
        Matrix3 textureMatrix() const;

        /**
         * @brief Texture coordinate set
         *
         * Convenience access to the @ref MaterialAttribute::DiffuseTextureCoordinates
         * / @ref MaterialAttribute::BaseColorTextureCoordinates /
         * @ref MaterialAttribute::TextureCoordinates attributes, picking the
         * one matching the texture (instead of combining e.g. a
         * @ref MaterialAttribute::BaseColorTexture with
         * @ref MaterialAttribute::DiffuseTextureCoordinates). If no matching
         * attribute is present, the default is @cpp 0 @ce. Available only if
         * the material has a texture.
         * @see @ref hasTexture()
         */
        UnsignedInt textureCoordinates() const;
};

}}

#endif
