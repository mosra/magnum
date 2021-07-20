#ifndef Magnum_Trade_TextureData_h
#define Magnum_Trade_TextureData_h
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
 * @brief Class @ref Magnum::Trade::TextureData
 */

#include "Magnum/Sampler.h"
#include "Magnum/Math/Vector3.h"
#include "Magnum/Trade/visibility.h"

#ifdef MAGNUM_BUILD_DEPRECATED
/* For implicit conversions to Vector<SamplerWrapping>, not used otherwise */
#include "Magnum/Array.h"
#endif

namespace Magnum { namespace Trade {

/**
@brief Texture type
@m_since_latest

@see @ref TextureData::type()
*/
enum class TextureType: UnsignedByte {
    /**
     * One-dimensional texture. The @ref TextureData::image() ID corresponds to
     * an image from @ref AbstractImporter::image1D().
     */
    Texture1D,

    /**
     * One-dimensional texture array. The @ref TextureData::image() ID
     * corresponds to an image from @ref AbstractImporter::image2D().
     * @m_since_latest
     */
    Texture1DArray,

    /**
     * Two-dimensional texture. The @ref TextureData::image() ID corresponds to
     * an image from @ref AbstractImporter::image2D().
     */
    Texture2D,

    /**
     * Two-dimensional texture array. The @ref TextureData::image() ID
     * corresponds to an image from @ref AbstractImporter::image3D().
     * @m_since_latest
     */
    Texture2DArray,

    /**
     * Three-dimensional texture. The @ref TextureData::image() ID corresponds
     * to an image from @ref AbstractImporter::image3D().
     */
    Texture3D,

    /**
     * Cube map texture. The @ref TextureData::image() ID corresponds to an
     * image from @ref AbstractImporter::image3D(), which is assumed to have
     * exactly 6 layers in order +X, -X, +Y, -Y, +Z, -Z.
     * @m_since_latest
     */
    CubeMap,

    /**
     * Cube map texture array. The @ref TextureData::image() ID
     * corresponds to an image from @ref AbstractImporter::image3D(), which is
     * assumed to have the layer count divisible by 6, each set in order +X,
     * -X, +Y, -Y, +Z, -Z.
     * @m_since_latest
     */
    CubeMapArray,

    #ifdef MAGNUM_BUILD_DEPRECATED
    /**
     * Cube map texture.
     * @m_deprecated_since_latest Use @ref TextureType::CubeMap instead.
     */
    Cube CORRADE_DEPRECATED_ENUM("use TextureType::CubeMap instead") = CubeMap
    #endif
};

/**
@brief Texture data

@see @ref AbstractImporter::texture()
*/
class TextureData {
    public:
        #ifdef MAGNUM_BUILD_DEPRECATED
        /** @brief @copybrief TextureType
         * @m_deprecated_since_latest Use @ref TextureType instead.
         */
        typedef CORRADE_DEPRECATED("use TextureType instead") TextureType Type;
        #endif

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
        explicit TextureData(TextureType type, SamplerFilter minificationFilter, SamplerFilter magnificationFilter, SamplerMipmap mipmapFilter, const Math::Vector3<SamplerWrapping>& wrapping, UnsignedInt image, const void* importerState = nullptr) noexcept: _type{type}, _minificationFilter{minificationFilter}, _magnificationFilter{magnificationFilter}, _mipmapFilter{mipmapFilter}, _wrapping{wrapping}, _image{image}, _importerState{importerState} {}

        /**
         * @brief Construct with the same wrapping for all dimensions
         *
         * Same as calling @ref TextureData(TextureType, SamplerFilter, SamplerFilter, SamplerMipmap, const Math::Vector3<SamplerWrapping>&, UnsignedInt, const void*)
         * with the same @p wrapping value for all dimensions.
         */
        explicit TextureData(TextureType type, SamplerFilter minificationFilter, SamplerFilter magnificationFilter, SamplerMipmap mipmapFilter, SamplerWrapping wrapping, UnsignedInt image, const void* importerState = nullptr) noexcept: _type{type}, _minificationFilter{minificationFilter}, _magnificationFilter{magnificationFilter}, _mipmapFilter{mipmapFilter}, _wrapping{wrapping}, _image{image}, _importerState{importerState} {}

        /** @brief Copying is not allowed */
        TextureData(const TextureData&) = delete;

        /** @brief Move constructor */
        TextureData(TextureData&&) noexcept = default;

        /** @brief Copying is not allowed */
        TextureData& operator=(const TextureData&) = delete;

        /** @brief Move assignment */
        TextureData& operator=(TextureData&&) noexcept = default;

        /** @brief Texture type */
        TextureType type() const { return _type; }

        /** @brief Minification filter */
        SamplerFilter minificationFilter() const { return _minificationFilter; }

        /** @brief Magnification filter */
        SamplerFilter magnificationFilter() const { return _magnificationFilter; }

        /** @brief Mipmap filter */
        SamplerMipmap mipmapFilter() const { return _mipmapFilter; }

        /** @brief Wrapping */
        Math::Vector3<SamplerWrapping> wrapping() const { return _wrapping; }

        /**
         * @brief Image ID
         *
         * ID of a 1D, 2D or 3D image depending on @ref type().
         * @see @ref type(), @ref AbstractImporter::image1D(),
         *      @ref AbstractImporter::image2D(),
         *      @ref AbstractImporter::image3D()
         */
        UnsignedInt image() const { return _image; }

        /**
         * @brief Importer-specific state
         *
         * See @ref AbstractImporter::importerState() for more information.
         */
        const void* importerState() const { return _importerState; }

    private:
        TextureType _type;
        SamplerFilter _minificationFilter, _magnificationFilter;
        SamplerMipmap _mipmapFilter;
        Math::Vector3<SamplerWrapping> _wrapping;
        UnsignedInt _image;
        const void* _importerState;
};

/** @debugoperatorenum{TextureType} */
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, TextureType value);

}}

#endif
