/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015
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

#include "CubeMapTexture.h"

#ifndef MAGNUM_TARGET_GLES2
#include "Magnum/BufferImage.h"
#endif
#include "Magnum/Context.h"
#include "Magnum/Image.h"

#include "Implementation/maxTextureSize.h"
#include "Implementation/State.h"
#include "Implementation/TextureState.h"

namespace Magnum {

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
    return (this->*Context::current()->state().texture->getCubeImageSizeImplementation)(level);
}
#endif

#ifndef MAGNUM_TARGET_GLES
void CubeMapTexture::image(const Int level, Image3D& image) {
    const Vector3i size{imageSize(level), 6};
    const std::size_t dataSize = image.dataSize(size);
    char* data = new char[dataSize];
    Buffer::unbindInternal(Buffer::TargetHint::PixelPack);
    glGetTextureImage(_id, level, GLenum(image.format()), GLenum(image.type()), dataSize, data);
    image.setData(image.format(), image.type(), size, data);
}

Image3D CubeMapTexture::image(const Int level, Image3D&& image) {
    this->image(level, image);
    return std::move(image);
}

void CubeMapTexture::image(const Int level, BufferImage3D& image, const BufferUsage usage) {
    const Vector3i size{imageSize(level), 6};
    const std::size_t dataSize = image.dataSize(size);
    if(image.size() != size)
        image.setData(image.format(), image.type(), size, nullptr, usage);

    image.buffer().bindInternal(Buffer::TargetHint::PixelPack);
    glGetTextureImage(_id, level, GLenum(image.format()), GLenum(image.type()), dataSize, nullptr);
}

BufferImage3D CubeMapTexture::image(const Int level, BufferImage3D&& image, const BufferUsage usage) {
    this->image(level, image, usage);
    return std::move(image);
}

void CubeMapTexture::compressedImage(const Int level, CompressedImage3D& image) {
    const Vector3i size{imageSize(level), 6};
    GLint dataSize;
    (this->*Context::current()->state().texture->getLevelParameterivImplementation)(level, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &dataSize);
    GLint format;
    (this->*Context::current()->state().texture->getLevelParameterivImplementation)(level, GL_TEXTURE_INTERNAL_FORMAT, &format);
    Containers::Array<char> data{std::size_t(dataSize)};

    Buffer::unbindInternal(Buffer::TargetHint::PixelPack);
    glGetCompressedTextureImage(_id, level, dataSize, data);
    image.setData(CompressedColorFormat(format), size, std::move(data));
}

CompressedImage3D CubeMapTexture::compressedImage(const Int level, CompressedImage3D&& image) {
    compressedImage(level, image);
    return std::move(image);
}

void CubeMapTexture::compressedImage(const Int level, CompressedBufferImage3D& image, const BufferUsage usage) {
    const Vector3i size{imageSize(level), 6};
    GLint dataSize;
    (this->*Context::current()->state().texture->getLevelParameterivImplementation)(level, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &dataSize);
    GLint format;
    (this->*Context::current()->state().texture->getLevelParameterivImplementation)(level, GL_TEXTURE_INTERNAL_FORMAT, &format);

    image.buffer().bindInternal(Buffer::TargetHint::PixelPack);
    image.setData(CompressedColorFormat(format), size, {nullptr, std::size_t(dataSize)}, usage);
    glGetCompressedTextureImage(_id, level, dataSize, nullptr);
}

CompressedBufferImage3D CubeMapTexture::compressedImage(const Int level, CompressedBufferImage3D&& image, const BufferUsage usage) {
    compressedImage(level, image, usage);
    return std::move(image);
}

void CubeMapTexture::image(const Coordinate coordinate, const Int level, Image2D& image) {
    const Vector2i size = imageSize(level);
    const std::size_t dataSize = image.dataSize(size);
    char* data = new char[dataSize];

    Buffer::unbindInternal(Buffer::TargetHint::PixelPack);
    (this->*Context::current()->state().texture->getCubeImageImplementation)(coordinate, level, size, image.format(), image.type(), dataSize, data);
    image.setData(image.format(), image.type(), size, data);
}

Image2D CubeMapTexture::image(const Coordinate coordinate, const Int level, Image2D&& image) {
    this->image(coordinate, level, image);
    return std::move(image);
}

void CubeMapTexture::image(const Coordinate coordinate, const Int level, BufferImage2D& image, const BufferUsage usage) {
    const Vector2i size = imageSize(level);
    const std::size_t dataSize = image.dataSize(size);
    if(image.size() != size)
        image.setData(image.format(), image.type(), size, nullptr, usage);

    image.buffer().bindInternal(Buffer::TargetHint::PixelPack);
    (this->*Context::current()->state().texture->getCubeImageImplementation)(coordinate, level, size, image.format(), image.type(), dataSize, nullptr);
}

BufferImage2D CubeMapTexture::image(const Coordinate coordinate, const Int level, BufferImage2D&& image, const BufferUsage usage) {
    this->image(coordinate, level, image, usage);
    return std::move(image);
}

void CubeMapTexture::compressedImage(const Coordinate coordinate, const Int level, CompressedImage2D& image) {
    const Vector2i size = imageSize(level);
    GLint dataSize;
    (this->*Context::current()->state().texture->getLevelParameterivImplementation)(level, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &dataSize);
    GLint format;
    (this->*Context::current()->state().texture->getLevelParameterivImplementation)(level, GL_TEXTURE_INTERNAL_FORMAT, &format);
    Containers::Array<char> data{std::size_t(dataSize)};

    Buffer::unbindInternal(Buffer::TargetHint::PixelPack);
    (this->*Context::current()->state().texture->getCompressedCubeImageImplementation)(coordinate, level, size, dataSize, data);
    image.setData(CompressedColorFormat(format), size, std::move(data));
}

CompressedImage2D CubeMapTexture::compressedImage(const Coordinate coordinate, const Int level, CompressedImage2D&& image) {
    compressedImage(coordinate, level, image);
    return std::move(image);
}

void CubeMapTexture::compressedImage(const Coordinate coordinate, const Int level, CompressedBufferImage2D& image, const BufferUsage usage) {
    const Vector2i size = imageSize(level);
    GLint dataSize;
    (this->*Context::current()->state().texture->getLevelParameterivImplementation)(level, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &dataSize);
    GLint format;
    (this->*Context::current()->state().texture->getLevelParameterivImplementation)(level, GL_TEXTURE_INTERNAL_FORMAT, &format);

    image.setData(CompressedColorFormat(format), size, {nullptr, std::size_t(dataSize)}, usage);
    image.buffer().bindInternal(Buffer::TargetHint::PixelPack);
    (this->*Context::current()->state().texture->getCompressedCubeImageImplementation)(coordinate, level, size, dataSize, nullptr);
}

CompressedBufferImage2D CubeMapTexture::compressedImage(const Coordinate coordinate, const Int level, CompressedBufferImage2D&& image, const BufferUsage usage) {
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

CubeMapTexture& CubeMapTexture::setSubImage(const Int level, const Vector3i& offset, const ImageView3D& image) {
    Buffer::unbindInternal(Buffer::TargetHint::PixelUnpack);
    glTextureSubImage3D(_id, level, offset.x(), offset.y(), offset.z(), image.size().x(), image.size().y(), image.size().z(), GLenum(image.format()), GLenum(image.type()), image.data());
    return *this;
}

CubeMapTexture& CubeMapTexture::setSubImage(const Int level, const Vector3i& offset, BufferImage3D& image) {
    image.buffer().bindInternal(Buffer::TargetHint::PixelUnpack);
    glTextureSubImage3D(_id, level, offset.x(), offset.y(), offset.z(), image.size().x(), image.size().y(), image.size().z(), GLenum(image.format()), GLenum(image.type()), nullptr);
    return *this;
}

CubeMapTexture& CubeMapTexture::setCompressedSubImage(const Int level, const Vector3i& offset, const CompressedImageView3D& image) {
    Buffer::unbindInternal(Buffer::TargetHint::PixelUnpack);
    glCompressedTextureSubImage3D(_id, level, offset.x(), offset.y(), offset.z(), image.size().x(), image.size().y(), image.size().z(), GLenum(image.format()), image.data().size(), image.data());
    return *this;
}

CubeMapTexture& CubeMapTexture::setCompressedSubImage(const Int level, const Vector3i& offset, CompressedBufferImage3D& image) {
    image.buffer().bindInternal(Buffer::TargetHint::PixelUnpack);
    glCompressedTextureSubImage3D(_id, level, offset.x(), offset.y(), offset.z(), image.size().x(), image.size().y(), image.size().z(), GLenum(image.format()), image.dataSize(), nullptr);
    return *this;
}
#endif

CubeMapTexture& CubeMapTexture::setSubImage(const Coordinate coordinate, const Int level, const Vector2i& offset, const ImageView2D& image) {
    #ifndef MAGNUM_TARGET_GLES2
    Buffer::unbindInternal(Buffer::TargetHint::PixelUnpack);
    #endif
    (this->*Context::current()->state().texture->cubeSubImageImplementation)(coordinate, level, offset, image.size(), image.format(), image.type(), image.data());
    return *this;
}

#ifndef MAGNUM_TARGET_GLES2
CubeMapTexture& CubeMapTexture::setSubImage(const Coordinate coordinate, const Int level, const Vector2i& offset, BufferImage2D& image) {
    image.buffer().bindInternal(Buffer::TargetHint::PixelUnpack);
    (this->*Context::current()->state().texture->cubeSubImageImplementation)(coordinate, level, offset, image.size(), image.format(), image.type(), nullptr);
    return *this;
}
#endif

CubeMapTexture& CubeMapTexture::setCompressedSubImage(const Coordinate coordinate, const Int level, const Vector2i& offset, const CompressedImageView2D& image) {
    #ifndef MAGNUM_TARGET_GLES2
    Buffer::unbindInternal(Buffer::TargetHint::PixelUnpack);
    #endif
    (this->*Context::current()->state().texture->cubeCompressedSubImageImplementation)(coordinate, level, offset, image.size(), image.format(), image.data());
    return *this;
}

#ifndef MAGNUM_TARGET_GLES2
CubeMapTexture& CubeMapTexture::setCompressedSubImage(const Coordinate coordinate, const Int level, const Vector2i& offset, CompressedBufferImage2D& image) {
    image.buffer().bindInternal(Buffer::TargetHint::PixelUnpack);
    (this->*Context::current()->state().texture->cubeCompressedSubImageImplementation)(coordinate, level, offset, image.size(), image.format(), {nullptr, image.dataSize()});
    return *this;
}
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
Vector2i CubeMapTexture::getImageSizeImplementationDefault(const Int level) {
    Vector2i size;
    bindInternal();
    glGetTexLevelParameteriv(GL_TEXTURE_CUBE_MAP_POSITIVE_X, level, GL_TEXTURE_WIDTH, &size.x());
    glGetTexLevelParameteriv(GL_TEXTURE_CUBE_MAP_POSITIVE_X, level, GL_TEXTURE_HEIGHT, &size.y());
    return size;
}

#ifndef MAGNUM_TARGET_GLES
Vector2i CubeMapTexture::getImageSizeImplementationDSA(const Int level) {
    Vector2i size;
    glGetTextureLevelParameteriv(_id, level, GL_TEXTURE_WIDTH, &size.x());
    glGetTextureLevelParameteriv(_id, level, GL_TEXTURE_HEIGHT, &size.y());
    return size;
}

Vector2i CubeMapTexture::getImageSizeImplementationDSAEXT(const Int level) {
    _flags |= ObjectFlag::Created;
    Vector2i size;
    glGetTextureLevelParameterivEXT(_id, GL_TEXTURE_CUBE_MAP_POSITIVE_X, level, GL_TEXTURE_WIDTH, &size.x());
    glGetTextureLevelParameterivEXT(_id, GL_TEXTURE_CUBE_MAP_POSITIVE_X, level, GL_TEXTURE_HEIGHT, &size.y());
    return size;
}
#endif
#endif

#ifndef MAGNUM_TARGET_GLES
void CubeMapTexture::getImageImplementationDefault(const Coordinate coordinate, const GLint level, const Vector2i&, const ColorFormat format, const ColorType type, std::size_t, GLvoid* const data) {
    bindInternal();
    glGetTexImage(GLenum(coordinate), level, GLenum(format), GLenum(type), data);
}

void CubeMapTexture::getCompressedImageImplementationDefault(const Coordinate coordinate, const GLint level, const Vector2i&, std::size_t, GLvoid* const data) {
    bindInternal();
    glGetCompressedTexImage(GLenum(coordinate), level, data);
}

void CubeMapTexture::getImageImplementationDSA(const Coordinate coordinate, const GLint level, const Vector2i& size, const ColorFormat format, const ColorType type, const std::size_t dataSize, GLvoid* const data) {
    glGetTextureSubImage(_id, level, 0, 0, GLenum(coordinate) - GL_TEXTURE_CUBE_MAP_POSITIVE_X, size.x(), size.y(), 1, GLenum(format), GLenum(type), dataSize, data);
}

void CubeMapTexture::getCompressedImageImplementationDSA(const Coordinate coordinate, const GLint level, const Vector2i& size, const std::size_t dataSize, GLvoid* const data) {
    glGetCompressedTextureSubImage(_id, level, 0, 0, GLenum(coordinate) - GL_TEXTURE_CUBE_MAP_POSITIVE_X, size.x(), size.y(), 1, dataSize, data);
}

void CubeMapTexture::getImageImplementationDSAEXT(const Coordinate coordinate, const GLint level, const Vector2i&, const ColorFormat format, const ColorType type, std::size_t, GLvoid* const data) {
    _flags |= ObjectFlag::Created;
    glGetTextureImageEXT(_id, GLenum(coordinate), level, GLenum(format), GLenum(type), data);
}

void CubeMapTexture::getCompressedImageImplementationDSAEXT(const Coordinate coordinate, const GLint level, const Vector2i&, std::size_t, GLvoid* const data) {
    _flags |= ObjectFlag::Created;
    glGetCompressedTextureImageEXT(_id, GLenum(coordinate), level, data);
}

void CubeMapTexture::getImageImplementationRobustness(const Coordinate coordinate, const GLint level, const Vector2i&, const ColorFormat format, const ColorType type, const std::size_t dataSize, GLvoid* const data) {
    bindInternal();
    glGetnTexImageARB(GLenum(coordinate), level, GLenum(format), GLenum(type), dataSize, data);
}

void CubeMapTexture::getCompressedImageImplementationRobustness(const Coordinate coordinate, const GLint level, const Vector2i&, const std::size_t dataSize, GLvoid* const data) {
    bindInternal();
    glGetnCompressedTexImageARB(GLenum(coordinate), level, dataSize, data);
}
#endif

void CubeMapTexture::subImageImplementationDefault(const Coordinate coordinate, const GLint level, const Vector2i& offset, const Vector2i& size, const ColorFormat format, const ColorType type, const GLvoid* const data) {
    bindInternal();
    glTexSubImage2D(GLenum(coordinate), level, offset.x(), offset.y(), size.x(), size.y(), GLenum(format), GLenum(type), data);
}

void CubeMapTexture::compressedSubImageImplementationDefault(const Coordinate coordinate, const GLint level, const Vector2i& offset, const Vector2i& size, const CompressedColorFormat format, const Containers::ArrayView<const GLvoid> data) {
    bindInternal();
    glCompressedTexSubImage2D(GLenum(coordinate), level, offset.x(), offset.y(), size.x(), size.y(), GLenum(format), data.size(), data);
}

#ifndef MAGNUM_TARGET_GLES
void CubeMapTexture::subImageImplementationDSA(const Coordinate coordinate, const GLint level, const Vector2i& offset, const Vector2i& size, const ColorFormat format, const ColorType type, const GLvoid* const data) {
    glTextureSubImage3D(_id, level, offset.x(), offset.y(), GLenum(coordinate) - GL_TEXTURE_CUBE_MAP_POSITIVE_X, size.x(), size.y(), 1, GLenum(format), GLenum(type), data);
}

void CubeMapTexture::compressedSubImageImplementationDSA(const Coordinate coordinate, const GLint level, const Vector2i& offset, const Vector2i& size, const CompressedColorFormat format, const Containers::ArrayView<const GLvoid> data) {
    glCompressedTextureSubImage3D(_id, level, offset.x(), offset.y(), GLenum(coordinate) - GL_TEXTURE_CUBE_MAP_POSITIVE_X, size.x(), size.y(), 1, GLenum(format), data.size(), data);
}

void CubeMapTexture::subImageImplementationDSAEXT(const Coordinate coordinate, const GLint level, const Vector2i& offset, const Vector2i& size, const ColorFormat format, const ColorType type, const GLvoid* const data) {
    _flags |= ObjectFlag::Created;
    glTextureSubImage2DEXT(_id, GLenum(coordinate), level, offset.x(), offset.y(), size.x(), size.y(), GLenum(format), GLenum(type), data);
}

void CubeMapTexture::compressedSubImageImplementationDSAEXT(const Coordinate coordinate, const GLint level, const Vector2i& offset, const Vector2i& size, const CompressedColorFormat format, const Containers::ArrayView<const GLvoid> data) {
    _flags |= ObjectFlag::Created;
    glTextureSubImage2DEXT(_id, GLenum(coordinate), level, offset.x(), offset.y(), size.x(), size.y(), GLenum(format), data.size(), data);
}
#endif

}
