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

#include "CubeMapTexture.h"

#include "Magnum/Image.h"
#include "Magnum/ImageView.h"
#ifndef MAGNUM_TARGET_GLES2
#include "Magnum/GL/BufferImage.h"
#endif
#include "Magnum/GL/Context.h"
#include "Magnum/GL/PixelFormat.h"
#include "Magnum/GL/Implementation/maxTextureSize.h"
#include "Magnum/GL/Implementation/RendererState.h"
#include "Magnum/GL/Implementation/State.h"
#include "Magnum/GL/Implementation/TextureState.h"

namespace Magnum { namespace GL {

static_assert(GL_TEXTURE_CUBE_MAP_POSITIVE_X - GL_TEXTURE_CUBE_MAP_POSITIVE_X == 0 &&
              GL_TEXTURE_CUBE_MAP_NEGATIVE_X - GL_TEXTURE_CUBE_MAP_POSITIVE_X == 1 &&
              GL_TEXTURE_CUBE_MAP_POSITIVE_Y - GL_TEXTURE_CUBE_MAP_POSITIVE_X == 2 &&
              GL_TEXTURE_CUBE_MAP_NEGATIVE_Y - GL_TEXTURE_CUBE_MAP_POSITIVE_X == 3 &&
              GL_TEXTURE_CUBE_MAP_POSITIVE_Z - GL_TEXTURE_CUBE_MAP_POSITIVE_X == 4 &&
              GL_TEXTURE_CUBE_MAP_NEGATIVE_Z - GL_TEXTURE_CUBE_MAP_POSITIVE_X == 5,
              "Unexpected GL enum values for cube faces");

Vector2i CubeMapTexture::maxSize() {
    return Vector2i{Implementation::maxCubeMapTextureSideSize()};
}

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
Vector2i CubeMapTexture::imageSize(const Int level) {
   const Implementation::TextureState& state = *Context::current().state().texture;

    Vector2i value;
    (this->*state.getCubeLevelParameterivImplementation)(level, GL_TEXTURE_WIDTH, &value[0]);
    (this->*state.getCubeLevelParameterivImplementation)(level, GL_TEXTURE_HEIGHT, &value[1]);
    return value;
}
#endif

#ifndef MAGNUM_TARGET_GLES
void CubeMapTexture::image(const Int level, Image3D& image) {
    createIfNotAlready();

    const Vector3i size{imageSize(level), 6};
    const std::size_t dataSize = Magnum::Implementation::imageDataSizeFor(image, size);

    /* Reallocate only if needed */
    Containers::Array<char> data{image.release()};
    if(data.size() < dataSize)
        data = Containers::Array<char>{dataSize};

    Buffer::unbindInternal(Buffer::TargetHint::PixelPack);
    Context::current().state().renderer->applyPixelStoragePack(image.storage());
    (this->*Context::current().state().texture->getFullCubeImageImplementation)(level, size, pixelFormat(image.format()), pixelType(image.format(), image.formatExtra()), data.size(), data, image.storage());
    image = Image3D{image.storage(), image.format(), image.formatExtra(), image.pixelSize(), size, std::move(data)};
}

Image3D CubeMapTexture::image(const Int level, Image3D&& image) {
    this->image(level, image);
    return std::move(image);
}

void CubeMapTexture::image(const Int level, const MutableImageView3D& image) {
    const Vector3i size{imageSize(level), 6};
    CORRADE_ASSERT(image.data().data() != nullptr || !size.product(),
        "GL::CubeMapTexture::image(): image view is nullptr", );
    CORRADE_ASSERT(image.size() == size,
        "GL::CubeMapTexture::image(): expected image view size" << size << "but got" << image.size(), );

    Buffer::unbindInternal(Buffer::TargetHint::PixelPack);
    Context::current().state().renderer->applyPixelStoragePack(image.storage());
    (this->*Context::current().state().texture->getFullCubeImageImplementation)(level, size, pixelFormat(image.format()), pixelType(image.format(), image.formatExtra()), image.data().size(), image.data(), image.storage());
}

void CubeMapTexture::image(const Int level, BufferImage3D& image, const BufferUsage usage) {
    createIfNotAlready();

    const Vector3i size{imageSize(level), 6};
    const std::size_t dataSize = Magnum::Implementation::imageDataSizeFor(image, size);

    /* Reallocate only if needed */
    if(image.dataSize() < dataSize)
        image.setData(image.storage(), image.format(), image.type(), size, {nullptr, dataSize}, usage);
    else
        image.setData(image.storage(), image.format(), image.type(), size, nullptr, usage);

    image.buffer().bindInternal(Buffer::TargetHint::PixelPack);
    Context::current().state().renderer->applyPixelStoragePack(image.storage());
    (this->*Context::current().state().texture->getFullCubeImageImplementation)(level, size, image.format(), image.type(), dataSize, nullptr, image.storage());
}

BufferImage3D CubeMapTexture::image(const Int level, BufferImage3D&& image, const BufferUsage usage) {
    this->image(level, image, usage);
    return std::move(image);
}

void CubeMapTexture::compressedImage(const Int level, CompressedImage3D& image) {
    createIfNotAlready();

    const Vector3i size{imageSize(level), 6};

    /* If the user-provided pixel storage doesn't tell us all properties about
       the compression, we need to ask GL for it */
    std::pair<std::size_t, std::size_t> dataOffsetSize;
    if(!image.storage().compressedBlockSize().product() || !image.storage().compressedBlockDataSize()) {
        /* Unlike in AbstractTexture::compressedImage(), here we have a
           separate offset and size because of the
           nv-cubemap-broken-full-compressed-image-query workaround, where it
           needs to go slice-by-slice, advancing the offset each time */
        dataOffsetSize.first = 0;
        dataOffsetSize.second = (this->*Context::current().state().texture->getCubeLevelCompressedImageSizeImplementation)(level)*6;
    } else dataOffsetSize = Magnum::Implementation::compressedImageDataOffsetSizeFor(image, size);

    /* Internal texture format */
    GLint format;
    (this->*Context::current().state().texture->getCubeLevelParameterivImplementation)(level, GL_TEXTURE_INTERNAL_FORMAT, &format);

    /* Reallocate only if needed */
    Containers::Array<char> data{image.release()};
    if(data.size() < dataOffsetSize.first + dataOffsetSize.second)
        data = Containers::Array<char>{dataOffsetSize.first + dataOffsetSize.second};

    Buffer::unbindInternal(Buffer::TargetHint::PixelPack);
    Context::current().state().renderer->applyPixelStoragePack(image.storage());
    (this->*Context::current().state().texture->getFullCompressedCubeImageImplementation)(level, size.xy(), dataOffsetSize.first, dataOffsetSize.second, data);
    image = CompressedImage3D{image.storage(), CompressedPixelFormat(format), size, std::move(data)};
}

CompressedImage3D CubeMapTexture::compressedImage(const Int level, CompressedImage3D&& image) {
    compressedImage(level, image);
    return std::move(image);
}

void CubeMapTexture::compressedImage(const Int level, const MutableCompressedImageView3D& image) {
    const Vector3i size{imageSize(level), 6};

    CORRADE_ASSERT(image.data().data() != nullptr || !size.product(),
        "GL::CubeMapTexture::compressedImage(): image view is nullptr", );
    CORRADE_ASSERT(image.size() == size,
        "GL::CubeMapTexture::compressedImage(): expected image view size" << size << "but got" << image.size(), );

    /* If the user-provided pixel storage doesn't tell us all properties about
       the compression, we need to ask GL for it */
    std::pair<std::size_t, std::size_t> dataOffsetSize;
    if(!image.storage().compressedBlockSize().product() || !image.storage().compressedBlockDataSize()) {
        /* Unlike in AbstractTexture::compressedImage(), here we have a
           separate offset and size because of the
           nv-cubemap-broken-full-compressed-image-query workaround, where it
           needs to go slice-by-slice, advancing the offset each time */
        dataOffsetSize.first = 0;
        dataOffsetSize.second = (this->*Context::current().state().texture->getCubeLevelCompressedImageSizeImplementation)(level)*6;
    } else dataOffsetSize = Magnum::Implementation::compressedImageDataOffsetSizeFor(image, size);

    CORRADE_ASSERT(image.data().size() == dataOffsetSize.first + dataOffsetSize.second,
        "GL::CubeMapTexture::compressedImage(): expected image view data size" << dataOffsetSize.first + dataOffsetSize.second << "bytes but got" << image.data().size(), );

    #ifndef CORRADE_NO_ASSERT
    /* Internal texture format */
    GLint format;
    (this->*Context::current().state().texture->getCubeLevelParameterivImplementation)(level, GL_TEXTURE_INTERNAL_FORMAT, &format);

    CORRADE_ASSERT(compressedPixelFormat(image.format()) == CompressedPixelFormat(format),
        "GL::CubeMapTexture::compressedImage(): expected image view format" << CompressedPixelFormat(format) << "but got" << compressedPixelFormat(image.format()), );
    #endif

    Buffer::unbindInternal(Buffer::TargetHint::PixelPack);
    Context::current().state().renderer->applyPixelStoragePack(image.storage());
    (this->*Context::current().state().texture->getFullCompressedCubeImageImplementation)(level, size.xy(), dataOffsetSize.first, dataOffsetSize.second, image.data());
}

void CubeMapTexture::compressedImage(const Int level, CompressedBufferImage3D& image, const BufferUsage usage) {
    createIfNotAlready();

    const Vector3i size{imageSize(level), 6};

    /* If the user-provided pixel storage doesn't tell us all properties about
       the compression, we need to ask GL for it */
    std::pair<std::size_t, std::size_t> dataOffsetSize;
    if(!image.storage().compressedBlockSize().product() || !image.storage().compressedBlockDataSize()) {
        /* Unlike in AbstractTexture::compressedImage(), here we have a
           separate offset and size because of the
           nv-cubemap-broken-full-compressed-image-query workaround, where it
           needs to go slice-by-slice, advancing the offset each time */
        dataOffsetSize.first = 0;
        dataOffsetSize.second = (this->*Context::current().state().texture->getCubeLevelCompressedImageSizeImplementation)(level)*6;
    } else dataOffsetSize = Magnum::Implementation::compressedImageDataOffsetSizeFor(image, size);

    /* Internal texture format */
    GLint format;
    (this->*Context::current().state().texture->getCubeLevelParameterivImplementation)(level, GL_TEXTURE_INTERNAL_FORMAT, &format);

    /* Reallocate only if needed */
    if(image.dataSize() < dataOffsetSize.first + dataOffsetSize.second)
        image.setData(image.storage(), CompressedPixelFormat(format), size, {nullptr, dataOffsetSize.first + dataOffsetSize.second}, usage);
    else
        image.setData(image.storage(), CompressedPixelFormat(format), size, nullptr, usage);

    image.buffer().bindInternal(Buffer::TargetHint::PixelPack);
    Context::current().state().renderer->applyPixelStoragePack(image.storage());
    (this->*Context::current().state().texture->getFullCompressedCubeImageImplementation)(level, size.xy(), dataOffsetSize.first, dataOffsetSize.second, nullptr);
}

CompressedBufferImage3D CubeMapTexture::compressedImage(const Int level, CompressedBufferImage3D&& image, const BufferUsage usage) {
    compressedImage(level, image, usage);
    return std::move(image);
}

void CubeMapTexture::image(const CubeMapCoordinate coordinate, const Int level, Image2D& image) {
    const Vector2i size = imageSize(level);
    const std::size_t dataSize = Magnum::Implementation::imageDataSizeFor(image, size);

    /* Reallocate only if needed */
    Containers::Array<char> data{image.release()};
    if(data.size() < dataSize)
        data = Containers::Array<char>{dataSize};

    Buffer::unbindInternal(Buffer::TargetHint::PixelPack);
    Context::current().state().renderer->applyPixelStoragePack(image.storage());
    (this->*Context::current().state().texture->getCubeImageImplementation)(coordinate, level, size, pixelFormat(image.format()), pixelType(image.format(), image.formatExtra()), data.size(), data);
    image = Image2D{image.storage(), image.format(), image.formatExtra(), image.pixelSize(), size, std::move(data)};
}

Image2D CubeMapTexture::image(const CubeMapCoordinate coordinate, const Int level, Image2D&& image) {
    this->image(coordinate, level, image);
    return std::move(image);
}

void CubeMapTexture::image(const CubeMapCoordinate coordinate, const Int level, const MutableImageView2D& image) {
    const Vector2i size = imageSize(level);
    CORRADE_ASSERT(image.data().data() != nullptr || !size.product(),
        "GL::CubeMapTexture::image(): image view is nullptr", );
    CORRADE_ASSERT(image.size() == size,
        "GL::CubeMapTexture::image(): expected image view size" << size << "but got" << image.size(), );

    Buffer::unbindInternal(Buffer::TargetHint::PixelPack);
    Context::current().state().renderer->applyPixelStoragePack(image.storage());
    (this->*Context::current().state().texture->getCubeImageImplementation)(coordinate, level, size, pixelFormat(image.format()), pixelType(image.format(), image.formatExtra()), image.data().size(), image.data());
}

void CubeMapTexture::image(const CubeMapCoordinate coordinate, const Int level, BufferImage2D& image, const BufferUsage usage) {
    const Vector2i size = imageSize(level);
    const std::size_t dataSize = Magnum::Implementation::imageDataSizeFor(image, size);

    /* Reallocate only if needed */
    if(image.dataSize() < dataSize)
        image.setData(image.storage(), image.format(), image.type(), size, {nullptr, dataSize}, usage);
    else
        image.setData(image.storage(), image.format(), image.type(), size, nullptr, usage);

    image.buffer().bindInternal(Buffer::TargetHint::PixelPack);
    Context::current().state().renderer->applyPixelStoragePack(image.storage());
    (this->*Context::current().state().texture->getCubeImageImplementation)(coordinate, level, size, image.format(), image.type(), dataSize, nullptr);
}

BufferImage2D CubeMapTexture::image(const CubeMapCoordinate coordinate, const Int level, BufferImage2D&& image, const BufferUsage usage) {
    this->image(coordinate, level, image, usage);
    return std::move(image);
}

void CubeMapTexture::compressedImage(const CubeMapCoordinate coordinate, const Int level, CompressedImage2D& image) {
    const Vector2i size = imageSize(level);

    /* If the user-provided pixel storage doesn't tell us all properties about
       the compression, we need to ask GL for it */
    std::size_t dataSize;
    if(!image.storage().compressedBlockSize().product() || !image.storage().compressedBlockDataSize())
        dataSize = (this->*Context::current().state().texture->getCubeLevelCompressedImageSizeImplementation)(level);
    else
        dataSize = Magnum::Implementation::compressedImageDataSizeFor(image, size);

    /* Internal texture format. Zero-init to avoid an assert about value
       already wrapped in compressedPixelFormatWrap() later if the drivers are
       extra shitty (Intel Windows drivers, I'm talking about you). */
    GLint format{};
    (this->*Context::current().state().texture->getCubeLevelParameterivImplementation)(level, GL_TEXTURE_INTERNAL_FORMAT, &format);

    /* Reallocate only if needed */
    Containers::Array<char> data{image.release()};
    if(data.size() < dataSize)
        data = Containers::Array<char>{dataSize};

    Buffer::unbindInternal(Buffer::TargetHint::PixelPack);
    Context::current().state().renderer->applyPixelStoragePack(image.storage());
    (this->*Context::current().state().texture->getCompressedCubeImageImplementation)(coordinate, level, size, data.size(), data);
    image = CompressedImage2D{image.storage(), CompressedPixelFormat(format), size, std::move(data)};
}

CompressedImage2D CubeMapTexture::compressedImage(const CubeMapCoordinate coordinate, const Int level, CompressedImage2D&& image) {
    compressedImage(coordinate, level, image);
    return std::move(image);
}

void CubeMapTexture::compressedImage(const CubeMapCoordinate coordinate, const Int level, const MutableCompressedImageView2D& image) {
    const Vector2i size = imageSize(level);

    CORRADE_ASSERT(image.data().data() != nullptr || !size.product(),
        "GL::CubeMapTexture::compressedImage(): image view is nullptr", );
    CORRADE_ASSERT(image.size() == size,
        "GL::CubeMapTexture::compressedImage(): expected image view size" << size << "but got" << image.size(), );

    #ifndef CORRADE_NO_ASSERT
    /* If the user-provided pixel storage doesn't tell us all properties about
       the compression, we need to ask GL for it */
    std::size_t dataSize;
    if(!image.storage().compressedBlockSize().product() || !image.storage().compressedBlockDataSize())
        dataSize = (this->*Context::current().state().texture->getCubeLevelCompressedImageSizeImplementation)(level);
    else
        dataSize = Magnum::Implementation::compressedImageDataSizeFor(image, size);

    CORRADE_ASSERT(image.data().size() == dataSize,
        "GL::CubeMapTexture::compressedImage(): expected image view data size" << dataSize << "bytes but got" << image.data().size(), );

    /* Internal texture format. Zero-init to avoid an assert about value
       already wrapped in compressedPixelFormatWrap() later if the drivers are
       extra shitty (Intel Windows drivers, I'm talking about you). */
    GLint format{};
    (this->*Context::current().state().texture->getCubeLevelParameterivImplementation)(level, GL_TEXTURE_INTERNAL_FORMAT, &format);

    CORRADE_ASSERT(compressedPixelFormat(image.format()) == CompressedPixelFormat(format),
        "GL::CubeMapTexture::compressedImage(): expected image view format" << CompressedPixelFormat(format) << "but got" << compressedPixelFormat(image.format()), );
    #endif

    Buffer::unbindInternal(Buffer::TargetHint::PixelPack);
    Context::current().state().renderer->applyPixelStoragePack(image.storage());
    (this->*Context::current().state().texture->getCompressedCubeImageImplementation)(coordinate, level, size, image.data().size(), image.data());
}

void CubeMapTexture::compressedImage(const CubeMapCoordinate coordinate, const Int level, CompressedBufferImage2D& image, const BufferUsage usage) {
    const Vector2i size = imageSize(level);

    /* If the user-provided pixel storage doesn't tell us all properties about
       the compression, we need to ask GL for it */
    std::size_t dataSize;
    if(!image.storage().compressedBlockSize().product() || !image.storage().compressedBlockDataSize())
        dataSize = (this->*Context::current().state().texture->getCubeLevelCompressedImageSizeImplementation)(level);
    else
        dataSize = Magnum::Implementation::compressedImageDataSizeFor(image, size);

    /* Internal texture format. Zero-init to avoid an assert about value
       already wrapped in compressedPixelFormatWrap() later if the drivers are
       extra shitty (Intel Windows drivers, I'm talking about you). */
    GLint format{};
    (this->*Context::current().state().texture->getCubeLevelParameterivImplementation)(level, GL_TEXTURE_INTERNAL_FORMAT, &format);

    /* Reallocate only if needed */
    if(image.dataSize() < dataSize)
        image.setData(image.storage(), CompressedPixelFormat(format), size, {nullptr, dataSize}, usage);
    else
        image.setData(image.storage(), CompressedPixelFormat(format), size, nullptr, usage);

    image.buffer().bindInternal(Buffer::TargetHint::PixelPack);
    Context::current().state().renderer->applyPixelStoragePack(image.storage());
    (this->*Context::current().state().texture->getCompressedCubeImageImplementation)(coordinate, level, size, dataSize, nullptr);
}

CompressedBufferImage2D CubeMapTexture::compressedImage(const CubeMapCoordinate coordinate, const Int level, CompressedBufferImage2D&& image, const BufferUsage usage) {
    compressedImage(coordinate, level, image, usage);
    return std::move(image);
}

Image3D CubeMapTexture::subImage(const Int level, const Range3Di& range, Image3D&& image) {
    this->subImage(level, range, image);
    return std::move(image);
}

BufferImage3D CubeMapTexture::subImage(const Int level, const Range3Di& range, BufferImage3D&& image, const BufferUsage usage) {
    this->subImage(level, range, image, usage);
    return std::move(image);
}

void CubeMapTexture::compressedSubImage(const Int level, const Range3Di& range, CompressedImage3D& image) {
    createIfNotAlready();

    /* Internal texture format. Zero-init to avoid an assert about value
       already wrapped in compressedPixelFormatWrap() later if the drivers are
       extra shitty (Intel Windows drivers, I'm talking about you). */
    GLint format{};
    (this->*Context::current().state().texture->getCubeLevelParameterivImplementation)(level, GL_TEXTURE_INTERNAL_FORMAT, &format);

    /* Calculate compressed subimage size. If the user-provided pixel storage
       doesn't tell us all properties about the compression, we need to ask GL
       for it. That requires GL_ARB_internalformat_query2. */
    std::size_t dataSize;
    if(!image.storage().compressedBlockSize().product() || !image.storage().compressedBlockDataSize())
        dataSize = compressedSubImageSize<3>(TextureFormat(format), range.size());
    else dataSize = Magnum::Implementation::compressedImageDataSizeFor(image, range.size());

    /* Reallocate only if needed */
    Containers::Array<char> data{image.release()};
    if(data.size() < dataSize)
        data = Containers::Array<char>{dataSize};

    Buffer::unbindInternal(Buffer::TargetHint::PixelPack);
    Context::current().state().renderer->applyPixelStoragePack(image.storage());
    glGetCompressedTextureSubImage(_id, level, range.min().x(), range.min().y(), range.min().z(), range.size().x(), range.size().y(), range.size().z(), data.size(), data);
    image = CompressedImage3D{CompressedPixelFormat(format), range.size(), std::move(data)};
}

CompressedImage3D CubeMapTexture::compressedSubImage(const Int level, const Range3Di& range, CompressedImage3D&& image) {
    compressedSubImage(level, range, image);
    return std::move(image);
}

void CubeMapTexture::compressedSubImage(const Int level, const Range3Di& range, const MutableCompressedImageView3D& image) {
    #ifndef CORRADE_NO_ASSERT
    CORRADE_ASSERT(image.data().data() != nullptr || !range.size().product(),
        "GL::CubeMapTexture::compressedSubImage(): image view is nullptr", );
    CORRADE_ASSERT(image.size() == range.size(),
        "GL::CubeMapTexture::compressedSubImage(): expected image view size" << range.size() << "but got" << image.size(), );

    createIfNotAlready();

    /* Internal texture format. Zero-init to avoid an assert about value
       already wrapped in compressedPixelFormatWrap() later if the drivers are
       extra shitty (Intel Windows drivers, I'm talking about you). */
    GLint format{};
    (this->*Context::current().state().texture->getCubeLevelParameterivImplementation)(level, GL_TEXTURE_INTERNAL_FORMAT, &format);

    CORRADE_ASSERT(compressedPixelFormat(image.format()) == CompressedPixelFormat(format),
        "GL::CubeMapTexture::compressedSubImage(): expected image view format" << CompressedPixelFormat(format) << "but got" << compressedPixelFormat(image.format()), );

    /* Calculate compressed subimage size. If the user-provided pixel storage
       doesn't tell us all properties about the compression, we need to ask GL
       for it. That requires GL_ARB_internalformat_query2. */
    std::size_t dataSize;
    if(!image.storage().compressedBlockSize().product() || !image.storage().compressedBlockDataSize())
        dataSize = compressedSubImageSize<3>(TextureFormat(format), range.size());
    else dataSize = Magnum::Implementation::compressedImageDataSizeFor(image, range.size());

    CORRADE_ASSERT(image.data().size() == dataSize,
        "GL::CubeMapTexture::compressedSubImage(): expected image view data size" << dataSize << "bytes but got" << image.data().size(), );
    #endif

    Buffer::unbindInternal(Buffer::TargetHint::PixelPack);
    Context::current().state().renderer->applyPixelStoragePack(image.storage());
    glGetCompressedTextureSubImage(_id, level, range.min().x(), range.min().y(), range.min().z(), range.size().x(), range.size().y(), range.size().z(), image.data().size(), image.data());
}

void CubeMapTexture::compressedSubImage(const Int level, const Range3Di& range, CompressedBufferImage3D& image, const BufferUsage usage) {
    createIfNotAlready();

    /* Internal texture format. Zero-init to avoid an assert about value
       already wrapped in compressedPixelFormatWrap() later if the drivers are
       extra shitty (Intel Windows drivers, I'm talking about you). */
    GLint format{};
    (this->*Context::current().state().texture->getCubeLevelParameterivImplementation)(level, GL_TEXTURE_INTERNAL_FORMAT, &format);

    /* Calculate compressed subimage size. If the user-provided pixel storage
       doesn't tell us all properties about the compression, we need to ask GL
       for it. That requires GL_ARB_internalformat_query2. */
    std::size_t dataSize;
    if(!image.storage().compressedBlockSize().product() || !image.storage().compressedBlockDataSize())
        dataSize = compressedSubImageSize<3>(TextureFormat(format), range.size());
    else dataSize = Magnum::Implementation::compressedImageDataSizeFor(image, range.size());

    /* Reallocate only if needed */
    if(image.dataSize() < dataSize)
        image.setData(image.storage(), CompressedPixelFormat(format), range.size(), {nullptr, dataSize}, usage);
    else
        image.setData(image.storage(), CompressedPixelFormat(format), range.size(), nullptr, usage);

    image.buffer().bindInternal(Buffer::TargetHint::PixelPack);
    Context::current().state().renderer->applyPixelStoragePack(image.storage());
    glGetCompressedTextureSubImage(_id, level, range.min().x(), range.min().y(), range.min().z(), range.size().x(), range.size().y(), range.size().z(), dataSize, nullptr);
}

CompressedBufferImage3D CubeMapTexture::compressedSubImage(const Int level, const Range3Di& range, CompressedBufferImage3D&& image, const BufferUsage usage) {
    compressedSubImage(level, range, image, usage);
    return std::move(image);
}

CubeMapTexture& CubeMapTexture::setSubImage(const Int level, const Vector3i& offset, const ImageView3D& image) {
    createIfNotAlready();

    Buffer::unbindInternal(Buffer::TargetHint::PixelUnpack);
    Context::current().state().renderer->applyPixelStorageUnpack(image.storage());
    (this->*Context::current().state().texture->cubeSubImage3DImplementation)(level, offset, image.size(), pixelFormat(image.format()), pixelType(image.format(), image.formatExtra()), image.data(), image.storage());
    return *this;
}

CubeMapTexture& CubeMapTexture::setSubImage(const Int level, const Vector3i& offset, BufferImage3D& image) {
    createIfNotAlready();

    image.buffer().bindInternal(Buffer::TargetHint::PixelUnpack);
    Context::current().state().renderer->applyPixelStorageUnpack(image.storage());
    (this->*Context::current().state().texture->cubeSubImage3DImplementation)(level, offset, image.size(), image.format(), image.type(), nullptr, image.storage());
    return *this;
}

CubeMapTexture& CubeMapTexture::setCompressedSubImage(const Int level, const Vector3i& offset, const CompressedImageView3D& image) {
    createIfNotAlready();

    Buffer::unbindInternal(Buffer::TargetHint::PixelUnpack);
    Context::current().state().renderer->applyPixelStorageUnpack(image.storage());
    glCompressedTextureSubImage3D(_id, level, offset.x(), offset.y(), offset.z(), image.size().x(), image.size().y(), image.size().z(), GLenum(compressedPixelFormat(image.format())), Magnum::Implementation::occupiedCompressedImageDataSize(image, image.data().size()), image.data());
    return *this;
}

CubeMapTexture& CubeMapTexture::setCompressedSubImage(const Int level, const Vector3i& offset, CompressedBufferImage3D& image) {
    createIfNotAlready();

    image.buffer().bindInternal(Buffer::TargetHint::PixelUnpack);
    Context::current().state().renderer->applyPixelStorageUnpack(image.storage());
    glCompressedTextureSubImage3D(_id, level, offset.x(), offset.y(), offset.z(), image.size().x(), image.size().y(), image.size().z(), GLenum(image.format()), Magnum::Implementation::occupiedCompressedImageDataSize(image, image.dataSize()), nullptr);
    return *this;
}
#endif

CubeMapTexture& CubeMapTexture::setSubImage(const CubeMapCoordinate coordinate, const Int level, const Vector2i& offset, const ImageView2D& image) {
    #ifndef MAGNUM_TARGET_GLES2
    Buffer::unbindInternal(Buffer::TargetHint::PixelUnpack);
    #endif
    Context::current().state().renderer->applyPixelStorageUnpack(image.storage());
    (this->*Context::current().state().texture->cubeSubImageImplementation)(coordinate, level, offset, image.size(), pixelFormat(image.format()), pixelType(image.format(), image.formatExtra()), image.data()
        #ifdef MAGNUM_TARGET_GLES2
        + Magnum::Implementation::pixelStorageSkipOffset(image)
        #endif
        );
    return *this;
}

#ifndef MAGNUM_TARGET_GLES2
CubeMapTexture& CubeMapTexture::setSubImage(const CubeMapCoordinate coordinate, const Int level, const Vector2i& offset, BufferImage2D& image) {
    image.buffer().bindInternal(Buffer::TargetHint::PixelUnpack);
    Context::current().state().renderer->applyPixelStorageUnpack(image.storage());
    (this->*Context::current().state().texture->cubeSubImageImplementation)(coordinate, level, offset, image.size(), image.format(), image.type(), nullptr);
    return *this;
}
#endif

CubeMapTexture& CubeMapTexture::setCompressedSubImage(const CubeMapCoordinate coordinate, const Int level, const Vector2i& offset, const CompressedImageView2D& image) {
    #ifndef MAGNUM_TARGET_GLES2
    Buffer::unbindInternal(Buffer::TargetHint::PixelUnpack);
    #endif
    Context::current().state().renderer->applyPixelStorageUnpack(image.storage());
    (this->*Context::current().state().texture->cubeCompressedSubImageImplementation)(coordinate, level, offset, image.size(), compressedPixelFormat(image.format()), image.data(), Magnum::Implementation::occupiedCompressedImageDataSize(image, image.data().size()));
    return *this;
}

#ifndef MAGNUM_TARGET_GLES2
CubeMapTexture& CubeMapTexture::setCompressedSubImage(const CubeMapCoordinate coordinate, const Int level, const Vector2i& offset, CompressedBufferImage2D& image) {
    image.buffer().bindInternal(Buffer::TargetHint::PixelUnpack);
    Context::current().state().renderer->applyPixelStorageUnpack(image.storage());
    (this->*Context::current().state().texture->cubeCompressedSubImageImplementation)(coordinate, level, offset, image.size(), image.format(), nullptr, Magnum::Implementation::occupiedCompressedImageDataSize(image, image.dataSize()));
    return *this;
}
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void CubeMapTexture::getLevelParameterImplementationDefault(const GLint level, const GLenum parameter, GLint* const values) {
    bindInternal();
    /* Using only parameters of +X in pre-DSA code path and assuming that all
       other faces are the same */
    glGetTexLevelParameteriv(GL_TEXTURE_CUBE_MAP_POSITIVE_X, level, parameter, values);
}

#ifndef MAGNUM_TARGET_GLES
void CubeMapTexture::getLevelParameterImplementationDSA(const GLint level, const GLenum parameter, GLint* const values) {
    glGetTextureLevelParameteriv(_id, level, parameter, values);
}
#endif
#endif

#ifndef MAGNUM_TARGET_GLES
GLint CubeMapTexture::getLevelCompressedImageSizeImplementationDefault(const GLint level) {
    bindInternal();
    /* Using only parameters of +X in pre-DSA code path and assuming that all
       other faces are the same */
    GLint value;
    glGetTexLevelParameteriv(GL_TEXTURE_CUBE_MAP_POSITIVE_X, level, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &value);

    return value;
}

GLint CubeMapTexture::getLevelCompressedImageSizeImplementationDefaultImmutableWorkaround(const GLint level) {
    const GLint value = getLevelCompressedImageSizeImplementationDefault(level);

    GLint immutable;
    glGetTexParameteriv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_IMMUTABLE_LEVELS, &immutable);
    return immutable ? value/6 : value;
}

GLint CubeMapTexture::getLevelCompressedImageSizeImplementationDSA(const GLint level) {
    GLint value;
    glGetTextureLevelParameteriv(_id, level, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &value);
    return value;
}

GLint CubeMapTexture::getLevelCompressedImageSizeImplementationDSANonImmutableWorkaround(const GLint level) {
    const GLint value = getLevelCompressedImageSizeImplementationDSA(level);

    GLint immutable;
    glGetTextureParameteriv(_id, GL_TEXTURE_IMMUTABLE_LEVELS, &immutable);
    return immutable ? value/6 : value;
}
#endif

#ifndef MAGNUM_TARGET_GLES
void CubeMapTexture::getImageImplementationDSA(const GLint level, const Vector3i&, const PixelFormat format, const PixelType type, const std::size_t dataSize, GLvoid* const data, const PixelStorage&) {
    glGetTextureImage(_id, level, GLenum(format), GLenum(type), dataSize, data);
}

void CubeMapTexture::getImageImplementationDSAAmdSliceBySlice(const GLint level, const Vector3i& size, const PixelFormat format, const PixelType type, std::size_t, GLvoid* const data, const PixelStorage& storage) {
    auto dataProperties = storage.dataProperties(pixelSize(format, type), size);
    const std::size_t stride = dataProperties.second.xy().product();
    for(Int i = 0; i != size.z(); ++i)
        glGetTextureSubImage(_id, level, 0, 0, i, size.x(), size.y(), 1, GLenum(format), GLenum(type), stride, static_cast<char*>(data) + dataProperties.first.sum() + stride*i);
}

void CubeMapTexture::getImageImplementationSliceBySlice(const GLint level, const Vector3i& size, const PixelFormat format, const PixelType type, std::size_t, GLvoid* const data, const PixelStorage& storage) {
    auto dataProperties = storage.dataProperties(pixelSize(format, type), size);
    const std::size_t stride = dataProperties.second.xy().product();
    for(Int i = 0; i != size.z(); ++i)
        getImageImplementationDefault(CubeMapCoordinate(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i), level, size.xy(), format, type, stride, static_cast<char*>(data) + stride*i);
}

void CubeMapTexture::getCompressedImageImplementationDSA(const GLint level, const Vector2i&, const std::size_t dataOffset, const std::size_t dataSize, GLvoid* const data) {
    glGetCompressedTextureImage(_id, level, dataOffset + dataSize, data);
}

void CubeMapTexture::getCompressedImageImplementationDSASingleSliceWorkaround(const GLint level, const Vector2i& size, const std::size_t dataOffset, const std::size_t dataSize, GLvoid* const data) {
    /* On NVidia (358.16) calling glGetCompressedTextureImage() extracts only
       the first face */
    for(Int face = 0; face != 6; ++face)
        glGetCompressedTextureSubImage(_id, level, 0, 0, face, size.x(), size.y(), 1, dataOffset + dataSize/6, static_cast<char*>(data) + dataSize*face/6);
}

void CubeMapTexture::getImageImplementationDefault(const CubeMapCoordinate coordinate, const GLint level, const Vector2i&, const PixelFormat format, const PixelType type, std::size_t, GLvoid* const data) {
    bindInternal();
    glGetTexImage(GLenum(coordinate), level, GLenum(format), GLenum(type), data);
}

void CubeMapTexture::getCompressedImageImplementationDefault(const CubeMapCoordinate coordinate, const GLint level, const Vector2i&, std::size_t, GLvoid* const data) {
    bindInternal();
    glGetCompressedTexImage(GLenum(coordinate), level, data);
}

void CubeMapTexture::getImageImplementationDSA(const CubeMapCoordinate coordinate, const GLint level, const Vector2i& size, const PixelFormat format, const PixelType type, const std::size_t dataSize, GLvoid* const data) {
    glGetTextureSubImage(_id, level, 0, 0, GLenum(coordinate) - GL_TEXTURE_CUBE_MAP_POSITIVE_X, size.x(), size.y(), 1, GLenum(format), GLenum(type), dataSize, data);
}

void CubeMapTexture::getCompressedImageImplementationDSA(const CubeMapCoordinate coordinate, const GLint level, const Vector2i& size, const std::size_t dataSize, GLvoid* const data) {
    glGetCompressedTextureSubImage(_id, level, 0, 0, GLenum(coordinate) - GL_TEXTURE_CUBE_MAP_POSITIVE_X, size.x(), size.y(), 1, dataSize, data);
}

void CubeMapTexture::getImageImplementationRobustness(const CubeMapCoordinate coordinate, const GLint level, const Vector2i&, const PixelFormat format, const PixelType type, const std::size_t dataSize, GLvoid* const data) {
    bindInternal();
    glGetnTexImageARB(GLenum(coordinate), level, GLenum(format), GLenum(type), dataSize, data);
}

void CubeMapTexture::getCompressedImageImplementationRobustness(const CubeMapCoordinate coordinate, const GLint level, const Vector2i&, const std::size_t dataSize, GLvoid* const data) {
    bindInternal();
    glGetnCompressedTexImageARB(GLenum(coordinate), level, dataSize, data);
}
#endif

#ifndef MAGNUM_TARGET_GLES
void CubeMapTexture::subImageImplementationDSA(const GLint level, const Vector3i& offset, const Vector3i& size, const PixelFormat format, const PixelType type, const GLvoid* const data, const PixelStorage&) {
    glTextureSubImage3D(_id, level, offset.x(), offset.y(), offset.z(), size.x(), size.y(), size.z(), GLenum(format), GLenum(type), data);
}

void CubeMapTexture::subImageImplementationDSASliceBySlice(const GLint level, const Vector3i& offset, const Vector3i& size, const PixelFormat format, const PixelType type, const GLvoid* const data, const PixelStorage& storage) {
    const std::size_t stride = std::get<1>(storage.dataProperties(pixelSize(format, type), size)).xy().product();
    for(Int i = 0; i != size.z(); ++i)
        subImageImplementationDSA(level, {offset.xy(), offset.z() + i}, {size.xy(), 1}, format, type, static_cast<const char*>(data) + stride*i, storage);
}

void CubeMapTexture::subImageImplementationSliceBySlice(const GLint level, const Vector3i& offset, const Vector3i& size, const PixelFormat format, const PixelType type, const GLvoid* const data, const PixelStorage& storage) {
    const std::size_t stride = std::get<1>(storage.dataProperties(pixelSize(format, type), size)).xy().product();
    for(Int i = 0; i != size.z(); ++i)
        subImageImplementationDefault(CubeMapCoordinate(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i), level, offset.xy(), size.xy(), format, type, static_cast<const char*>(data) + stride*i);
}
#endif

void CubeMapTexture::subImageImplementationDefault(const CubeMapCoordinate coordinate, const GLint level, const Vector2i& offset, const Vector2i& size, const PixelFormat format, const PixelType type, const GLvoid* const data) {
    bindInternal();
    glTexSubImage2D(GLenum(coordinate), level, offset.x(), offset.y(), size.x(), size.y(), GLenum(format), GLenum(type), data);
}

void CubeMapTexture::compressedSubImageImplementationDefault(const CubeMapCoordinate coordinate, const GLint level, const Vector2i& offset, const Vector2i& size, const CompressedPixelFormat format, const GLvoid* const data, const GLsizei dataSize) {
    bindInternal();
    glCompressedTexSubImage2D(GLenum(coordinate), level, offset.x(), offset.y(), size.x(), size.y(), GLenum(format), dataSize, data);
}

#ifndef MAGNUM_TARGET_GLES
void CubeMapTexture::subImageImplementationDSA(const CubeMapCoordinate coordinate, const GLint level, const Vector2i& offset, const Vector2i& size, const PixelFormat format, const PixelType type, const GLvoid* const data) {
    glTextureSubImage3D(_id, level, offset.x(), offset.y(), GLenum(coordinate) - GL_TEXTURE_CUBE_MAP_POSITIVE_X, size.x(), size.y(), 1, GLenum(format), GLenum(type), data);
}

void CubeMapTexture::compressedSubImageImplementationDSA(const CubeMapCoordinate coordinate, const GLint level, const Vector2i& offset, const Vector2i& size, const CompressedPixelFormat format, const GLvoid* const data, const GLsizei dataSize) {
    glCompressedTextureSubImage3D(_id, level, offset.x(), offset.y(), GLenum(coordinate) - GL_TEXTURE_CUBE_MAP_POSITIVE_X, size.x(), size.y(), 1, GLenum(format), dataSize, data);
}
#endif

}}
