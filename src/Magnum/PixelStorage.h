#ifndef Magnum_PixelStorage_h
#define Magnum_PixelStorage_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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
 * @brief Class @ref Magnum::PixelStorage, @ref Magnum::CompressedPixelStorage
 */

#include <cstddef>
#include <tuple>

#include "Magnum/Magnum.h"
#include "Magnum/visibility.h"
#include "Magnum/Math/Vector3.h"

namespace Magnum {

namespace Implementation { struct RendererState; }

/**
@brief Pixel storage parameters

Descibes how to interpret data which are read from or stored into @ref Image,
@ref ImageView, @ref BufferImage and @ref Trade::ImageData using
@ref Texture::setImage() "*Texture::setImage()", @ref Texture::setSubImage() "*Texture::setSubImage()",
@ref Texture::image() "*Texture::image()", @ref Texture::subImage() "*Texture::subImage()"
and @ref AbstractFramebuffer::read() "*Framebuffer::read()".

## Performance optimizations

The storage mode is applied either right before doing image upload using
@fn_gl{PixelStore} with @def_gl{UNPACK_*} parameters or right before doing
image download using @fn_gl{PixelStore} with @def_gl{PACK_*}. The engine tracks
currently used pixel pack/unpack parameters to avoid unnecessary calls to
@fn_gl{PixelStore}. See also @ref Context::resetState() and
@ref Context::State::PixelStorage.

@see @ref CompressedPixelStorage
*/
class MAGNUM_EXPORT PixelStorage {
    friend AbstractFramebuffer;
    friend AbstractTexture;
    friend CubeMapTexture;

    public:
        /**
         * @brief Pixel size for given format/type combination (in bytes)
         *
         * @see @ref dataProperties()
         */
        static std::size_t pixelSize(PixelFormat format, PixelType type);

        /**
         * @brief Default constructor
         *
         * Sets all parameters to default values, i.e. all values set to
         * `false`/`0` except for alignment, which is `4`.
         */
        constexpr /*implicit*/ PixelStorage() noexcept;

        /** @brief Equality comparison */
        bool operator==(const PixelStorage& other) const;

        /** @brief Non-equality comparison */
        bool operator!=(const PixelStorage& other) const {
            return !operator==(other);
        }

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Whether to reverse byte order
         *
         * @requires_gl Not available in OpenGL ES or WebGL.
         */
        constexpr bool swapBytes() const { return _swapBytes; }

        /**
         * @brief Enable or disable byte order reversion
         *
         * Not applicable for @ref CompressedPixelStorage. Default is `false`.
         * @see @fn_gl{PixelStore} with @def_gl{PACK_SWAP_BYTES}/
         *      @def_gl{UNPACK_SWAP_BYTES}
         * @requires_gl Not available in OpenGL ES or WebGL.
         */
        PixelStorage& setSwapBytes(bool enabled) {
            _swapBytes = enabled;
            return *this;
        }
        #endif

        /** @brief Row alignment */
        constexpr Int alignment() const { return _alignment; }

        /**
         * @brief Set row alignment
         *
         * Not applicable for @ref CompressedPixelStorage. Valid values are
         * `1`, `2`, `4` and `8`. Default is `4`.
         * @see @fn_gl{PixelStore} with @def_gl{PACK_ALIGNMENT}/
         *      @def_gl{UNPACK_ALIGNMENT}
         */
        PixelStorage& setAlignment(Int alignment) {
            _alignment = alignment;
            return *this;
        }

        #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
        /**
         * @brief Row length
         *
         * @requires_gles30 Extension @extension{EXT,unpack_subimage}/
         *      @extension{NV,pack_subimage} in OpenGL ES 2.0.
         * @requires_webgl20 Row length specification is not available in WebGL
         *      1.0.
         */
        constexpr Int rowLength() const { return _rowLength; }

        /**
         * @brief Set row length
         *
         * Used only on 2D and 3D images. If set to `0`, size information from
         * actual image is used. Default is `0`.
         * @see @fn_gl{PixelStore} with @def_gl{UNPACK_ROW_LENGTH}/
         *      @def_gl{PACK_ROW_LENGTH}
         * @requires_gles30 Extension @extension{EXT,unpack_subimage}/
         *      @extension{NV,pack_subimage} in OpenGL ES 2.0.
         * @requires_webgl20 Row length specification is not available in WebGL
         *      1.0.
         */
        PixelStorage& setRowLength(Int length) {
            _rowLength = length;
            return *this;
        }
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Image height
         *
         * @requires_gles30 Image height specification is not available in
         *      OpenGL ES 2.0
         * @requires_webgl20 Image height specification is not available in
         *      WebGL 1.0.
         */
        constexpr Int imageHeight() const { return _imageHeight; }

        /**
         * @brief Set image height
         *
         * Used only on 3D images. If set to `0`, size information from actual
         * image is used. Default is `0`.
         * @see @fn_gl{PixelStore} with @def_gl{UNPACK_IMAGE_HEIGHT}/
         *      @def_gl{PACK_IMAGE_HEIGHT}
         * @requires_gles30 Image height specification is not available in
         *      OpenGL ES 2.0
         * @requires_webgl20 Image height specification is not available in
         *      WebGL 1.0.
         * @requires_gl Image height specification is available only for unpack
         *      in OpenGL ES and WebGL.
         */
        PixelStorage& setImageHeight(Int height) {
            _imageHeight = height;
            return *this;
        }
        #endif

        /** @brief Pixel, row and image skipping */
        constexpr Vector3i skip() const { return _skip; }

        /**
         * @brief Set pixel, row and image skipping
         *
         * The Y value is used only for 2D and 3D images, the Z value is used
         * only for 3D images. Default is `0`. On OpenGL ES 2.0 and WebGL 1.0
         * the functionality is emulated by increasing the data pointer.
         * @see @fn_gl{PixelStore} with @def_gl{UNPACK_SKIP_PIXELS}/
         *      @def_gl{PACK_SKIP_PIXELS}, @def_gl{UNPACK_SKIP_ROWS}/
         *      @def_gl{PACK_SKIP_ROWS}, @def_gl{UNPACK_SKIP_IMAGES}/
         *      @def_gl{PACK_SKIP_IMAGES}
         * @requires_gl Image skip specification is available only for unpack
         *      in OpenGL ES and WebGL.
         */
        PixelStorage& setSkip(const Vector3i& skip) {
            _skip = skip;
            return *this;
        }

        /**
         * @brief Data properties for given parameters
         *
         * Returns byte offset in each direction, (row length, row count, layer
         * count) and pixel size for image of given @p size with current pixel
         * storage parameters, @p format and @p type. The offset reflects the
         * @ref skip() parameter. Sum of the byte offset vector gives the
         * byte offset of first pixel in the data array.
         * @see @ref pixelSize()
         */
        std::tuple<Math::Vector3<std::size_t>, Math::Vector3<std::size_t>, std::size_t> dataProperties(PixelFormat format, PixelType type, const Vector3i& size) const;

    #ifndef DOXYGEN_GENERATING_OUTPUT
    protected:
    #else
    private:
    #endif
        /* Bool parameter is ugly, but this is implementation detail of
           internal API so who cares */
        void MAGNUM_LOCAL applyInternal(bool isUnpack);

        #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
        Int _rowLength;
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        Int _imageHeight;
        #endif
        Vector3i _skip;

    private:
        /* Used internally in *Texture::image(), *Texture::subImage(),
           *Texture::setImage(), *Texture::setSubImage() and
           *Framebuffer::read() */
        void MAGNUM_LOCAL applyUnpack();
        void MAGNUM_LOCAL applyPack() { applyInternal(false); }

        #ifndef MAGNUM_TARGET_GLES
        bool _swapBytes;
        #endif
        Int _alignment;
};

#ifndef MAGNUM_TARGET_GLES
/**
@brief Compressed pixel storage parameters

Descibes how to interpret data which are read from or stored into
@ref CompressedImage, @ref CompressedImageView, @ref CompressedBufferImage and
@ref Trade::ImageData using @ref Texture::setCompressedImage() "*Texture::setCompressedImage(),
@ref Texture::setCompressedSubImage() "*Texture::setCompressedSubImage()",
@ref Texture::compressedImage() "*Texture::compressedImage()" and
@ref Texture::compressedSubImage() "*Texture::compressedSubImage()".

Includes all parameters from @ref PixelStorage, except for @ref swapBytes() and
@ref alignment(), which are ignored for compressed images.

@requires_gl42 Extension @extension{ARB,compressed_texture_pixel_storage}
@requires_gl Compressed pixel storage is hardcoded in OpenGL ES and WebGL.
*/
class MAGNUM_EXPORT CompressedPixelStorage: public PixelStorage {
    friend AbstractTexture;
    friend CubeMapTexture;

    public:
        /**
         * @brief Default constructor
         *
         * Sets all parameters to default values, i.e. all values set to
         * `false`/`0` except for alignment, which is `4`.
         */
        #ifndef CORRADE_MSVC2015_COMPATIBILITY /* What am I doing wrong? */
        constexpr
        #endif
        /*implicit*/ CompressedPixelStorage() noexcept: _blockSize{0}, _blockDataSize{0} {}

        /** @brief Equality comparison */
        bool operator==(const CompressedPixelStorage& other) const;

        /** @brief Non-equality comparison */
        bool operator!=(const CompressedPixelStorage& other) const {
            return !operator==(other);
        }

        /** @brief Compressed block size */
        constexpr Vector3i compressedBlockSize() const { return _blockSize; }

        /**
         * @brief Set compressed block size
         *
         * If set to `0` for given dimension, size information from particular
         * compressed format is used. Default is `0` in all dimensions.
         */
        CompressedPixelStorage& setCompressedBlockSize(const Vector3i& size) {
            _blockSize = size;
            return *this;
        }

        /** @brief Compressed block data size (in bytes) */
        constexpr Int compressedBlockDataSize() const { return _blockDataSize; }

        /**
         * @brief Set compressed block data size (in bytes)
         *
         * If set to `0`, size information from particular compressed format is
         * used. Default is `0` in all dimensions.
         */
        CompressedPixelStorage& setCompressedBlockDataSize(Int size) {
            _blockDataSize = size;
            return *this;
        }

        /**
         * @brief Data properties for given parameters
         *
         * Returns byte offset in each dimension, count of blocks in each
         * dimension and block data size for image of given @p size with
         * current pixel storage parameters. Adding byte offset and product of
         * the vector multiplied with block data size gives minimal byte count
         * to store given data.
         *
         * Expects @ref compressedBlockSize() and @ref compressedBlockDataSize()
         * to be non-zero.
         */
        std::tuple<Math::Vector3<std::size_t>, Math::Vector3<std::size_t>, std::size_t> dataProperties(const Vector3i& size) const;

        /* Overloads to remove WTF-factor from method chaining order */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        CompressedPixelStorage& setRowLength(Int length) {
            PixelStorage::setRowLength(length);
            return *this;
        }
        CompressedPixelStorage& setImageHeight(Int height) {
            PixelStorage::setImageHeight(height);
            return *this;
        }
        CompressedPixelStorage& setSkip(const Vector3i& skip) {
            PixelStorage::setSkip(skip);
            return *this;
        }
        #endif

    private:
        #ifndef MAGNUM_TARGET_GLES
        using PixelStorage::swapBytes;
        using PixelStorage::setSwapBytes;
        #endif
        using PixelStorage::alignment;
        using PixelStorage::setAlignment;

        /* Bool parameter is ugly, but this is implementation detail of
           internal API so who cares */
        void MAGNUM_LOCAL applyInternal(bool isUnpack);

        /* Used internally in *Texture::compressedImage(), *Texture::compressedSubImage(),
           *Texture::setCompressedImage() and *Texture::setCompressedSubImage() */
        void MAGNUM_LOCAL applyUnpack() { applyInternal(true); }
        void MAGNUM_LOCAL applyPack() { applyInternal(false); }

        Vector3i _blockSize;
        Int _blockDataSize;
};
#endif

constexpr PixelStorage::PixelStorage() noexcept:
    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    _rowLength{0},
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    _imageHeight{0},
    #endif
    _skip{0},
    #ifndef MAGNUM_TARGET_GLES
    _swapBytes{false},
    #endif
    _alignment{4} {}

namespace Implementation {
    /* Used in *Image::dataProperties() */
    template<std::size_t dimensions, class T> std::tuple<Math::Vector<dimensions, std::size_t>, Math::Vector<dimensions, std::size_t>, std::size_t> imageDataProperties(const T& image) {
        Math::Vector3<std::size_t> offset, dataSize;
        std::size_t pixelSize;
        std::tie(offset, dataSize, pixelSize) = image.storage().dataProperties(image.format(), image.type(), Vector3i::pad(image.size(), 1));
        return std::make_tuple(Math::Vector<dimensions, std::size_t>::pad(offset), Math::Vector<dimensions, std::size_t>::pad(dataSize), pixelSize);
    }

    #ifndef MAGNUM_TARGET_GLES2
    /* Used in Compressed*Image::dataProperties() */
    template<std::size_t dimensions, class T> std::tuple<Math::Vector<dimensions, std::size_t>, Math::Vector<dimensions, std::size_t>, std::size_t> compressedImageDataProperties(const T& image) {
        Math::Vector3<std::size_t> offset, blockCount;
        std::size_t blockSize;
        std::tie(offset, blockCount, blockSize) = image.storage().dataProperties(Vector3i::pad(image.size(), 1));
        return std::make_tuple(Math::Vector<dimensions, std::size_t>::pad(offset), Math::Vector<dimensions, std::size_t>::pad(blockCount), blockSize);
    }
    #endif

    /* Used in image query functions */
    template<std::size_t dimensions, class T> std::size_t imageDataSizeFor(const T& image, const Math::Vector<dimensions, Int>& size) {
        Math::Vector3<std::size_t> offset, dataSize;
        std::size_t pixelSize;
        std::tie(offset, dataSize, pixelSize) = image.storage().dataProperties(image.format(), image.type(), Vector3i::pad(size, 1));

        /* Smallest line/rectangle/cube that covers the area */
        std::size_t dataOffset = 0;
        if(offset.z())
            dataOffset += offset.z();
        else if(offset.y()) {
            #ifndef MAGNUM_TARGET_GLES2
            if(!image.storage().imageHeight())
            #endif
            {
                dataOffset += offset.y();
            }
        } else if(offset.x()) {
            #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
            if(!image.storage().rowLength())
            #endif
            {
                dataOffset += offset.x();
            }
        }
        return dataOffset + dataSize.product();
    }

    /* Used in data size assertions */
    template<class T> inline std::size_t imageDataSize(const T& image) {
        return imageDataSizeFor(image, image.size());
    }

    #ifdef MAGNUM_BUILD_DEPRECATED
    /* Uses default pixel storage */
    template<std::size_t dimensions> std::size_t imageDataSizeFor(PixelFormat format, PixelType type, const Math::Vector<dimensions, Int>& size) {
        return std::get<1>(PixelStorage{}.dataProperties(format, type, Vector3i::pad(size, 1))).product();
    }
    #endif

    #ifndef MAGNUM_TARGET_GLES
    template<std::size_t dimensions, class T> std::pair<std::size_t, std::size_t> compressedImageDataOffsetSizeFor(const T& image, const Math::Vector<dimensions, Int>& size) {
        CORRADE_INTERNAL_ASSERT(image.storage().compressedBlockSize().product() && image.storage().compressedBlockDataSize());

        Math::Vector3<std::size_t> offset, blockCount;
        std::size_t blockDataSize;
        std::tie(offset, blockCount, blockDataSize) = image.storage().dataProperties(Vector3i::pad(size, 1));

        const auto realBlockCount = Math::Vector3<std::size_t>{(Vector3i::pad(size, 1) + image.storage().compressedBlockSize() - Vector3i{1})/image.storage().compressedBlockSize()};

        return {offset.sum(), (blockCount.product() - (blockCount.x() - realBlockCount.x()) - (blockCount.y() - realBlockCount.y())*blockCount.x())*blockDataSize};
    }

    /* Used in image query functions */
    template<std::size_t dimensions, class T> std::size_t compressedImageDataSizeFor(const T& image, const Math::Vector<dimensions, Int>& size) {
        auto r = compressedImageDataOffsetSizeFor(image, size);
        return r.first + r.second;
    }

    /* Use in compressed image upload functions */
    template<class T> std::size_t occupiedCompressedImageDataSize(const T& image, std::size_t dataSize) {
        return image.storage().compressedBlockSize().product() && image.storage().compressedBlockDataSize()
            ? compressedImageDataOffsetSizeFor(image, image.size()).second : dataSize;
    }
    #else
    template<class T> std::size_t occupiedCompressedImageDataSize(const T&, std::size_t dataSize) {
        return dataSize;
    }
    #endif

    #ifdef MAGNUM_TARGET_GLES2
    template<std::size_t dimensions, class T> std::ptrdiff_t pixelStorageSkipOffsetFor(const T& image, const Math::Vector<dimensions, Int>& size) {
        return std::get<0>(image.storage().dataProperties(image.format(), image.type(), Vector3i::pad(size, 1))).sum();
    }
    template<class T> std::ptrdiff_t pixelStorageSkipOffset(const T& image) {
        return pixelStorageSkipOffsetFor(image, image.size());
    }
    #endif
}

}

#endif
