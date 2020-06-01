#ifndef Magnum_Trade_TextureData_h
#define Magnum_Trade_TextureData_h
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
 * @brief Class @ref Magnum::Trade::TextureData
 */

#include "Magnum/Array.h"
#include "Magnum/Sampler.h"
#include "Magnum/Trade/visibility.h"

namespace Magnum { namespace Trade {

/**
@brief Texture data

@see @ref AbstractImporter::texture()
*/
class TextureData {
    public:
        /**
         * @brief Texture type
         *
         * @see @ref type()
         */
        enum class Type: UnsignedByte {
            Texture1D,  /**< One-dimensional texture */
            Texture2D,  /**< Two-dimensional texture */
            Texture3D,  /**< Three-dimensional texture */
            Cube        /**< Cube map texture */
        };

        /**
         * @brief Constructor
         * @param type                  Texture type
         * @param minificationFilter    Minification filter
         * @param magnificationFilter   Magnification filter
         * @param mipmapFilter          Mipmap filter
         * @param wrapping              Wrapping
         * @param image                 Texture image ID
         * @param importerState         Importer-specific state
         */
        TextureData(Type type, SamplerFilter minificationFilter, SamplerFilter magnificationFilter, SamplerMipmap mipmapFilter, Array3D<SamplerWrapping> wrapping, UnsignedInt image, const void* importerState = nullptr) noexcept: _type{type}, _minificationFilter{minificationFilter}, _magnificationFilter{magnificationFilter}, _mipmapFilter{mipmapFilter}, _wrapping{wrapping}, _image{image}, _importerState{importerState} {}

        /** @brief Copying is not allowed */
        TextureData(const TextureData&) = delete;

        /** @brief Move constructor */
        TextureData(TextureData&&) noexcept = default;

        /** @brief Copying is not allowed */
        TextureData& operator=(const TextureData&) = delete;

        /** @brief Move assignment */
        TextureData& operator=(TextureData&&) noexcept = default;

        /** @brief Texture type */
        Type type() const { return _type; }

        /** @brief Minification filter */
        SamplerFilter minificationFilter() const { return _minificationFilter; }

        /** @brief Magnification filter */
        SamplerFilter magnificationFilter() const { return _magnificationFilter; }

        /** @brief Mipmap filter */
        SamplerMipmap mipmapFilter() const { return _mipmapFilter; }

        /** @brief Wrapping */
        Array3D<SamplerWrapping> wrapping() const { return _wrapping; }

        /**
         * @brief Image ID
         *
         * ID of 1D, 2D or 3D image based on texture type. If type is
         * @ref Type::Cube the function returns first of six consecutive
         * IDs of cube map sides, ordered +X, -X, +Y, -Y, +Z, -Z.
         * @see @ref type(), @ref AbstractImporter::image1D(),
         *      @ref AbstractImporter::image2D(), @ref AbstractImporter::image3D()
         */
        UnsignedInt image() const { return _image; }

        /**
         * @brief Importer-specific state
         *
         * See @ref AbstractImporter::importerState() for more information.
         */
        const void* importerState() const { return _importerState; }

    private:
        Type _type;
        SamplerFilter _minificationFilter, _magnificationFilter;
        SamplerMipmap _mipmapFilter;
        Array3D<SamplerWrapping> _wrapping;
        UnsignedInt _image;
        const void* _importerState;
};

/** @debugoperatorclassenum{TextureData,TextureData::Type} */
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, TextureData::Type value);

}}

#endif
