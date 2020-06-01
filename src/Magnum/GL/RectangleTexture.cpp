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

#include "RectangleTexture.h"

#ifndef MAGNUM_TARGET_GLES
#include "Magnum/Image.h"
#include "Magnum/GL/BufferImage.h"
#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"

#include "Magnum/GL/Implementation/State.h"
#include "Magnum/GL/Implementation/TextureState.h"

namespace Magnum { namespace GL {

Vector2i RectangleTexture::maxSize() {
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_rectangle>())
        return {};

    GLint& value = Context::current().state().texture->maxRectangleSize;

    if(value == 0)
        glGetIntegerv(GL_MAX_RECTANGLE_TEXTURE_SIZE, &value);

    return Vector2i{value};
}

Image2D RectangleTexture::image(Image2D&& image) {
    this->image(image);
    return std::move(image);
}

BufferImage2D RectangleTexture::image(BufferImage2D&& image, const BufferUsage usage) {
    this->image(image, usage);
    return std::move(image);
}

CompressedImage2D RectangleTexture::compressedImage(CompressedImage2D&& image) {
    compressedImage(image);
    return std::move(image);
}

CompressedBufferImage2D RectangleTexture::compressedImage(CompressedBufferImage2D&& image, const BufferUsage usage) {
    compressedImage(image, usage);
    return std::move(image);
}

Image2D RectangleTexture::subImage(const Range2Di& range, Image2D&& image) {
    this->subImage(range, image);
    return std::move(image);
}

BufferImage2D RectangleTexture::subImage(const Range2Di& range, BufferImage2D&& image, const BufferUsage usage) {
    this->subImage(range, image, usage);
    return std::move(image);
}

CompressedImage2D RectangleTexture::compressedSubImage(const Range2Di& range, CompressedImage2D&& image) {
    compressedSubImage(range, image);
    return std::move(image);
}

CompressedBufferImage2D RectangleTexture::compressedSubImage(const Range2Di& range, CompressedBufferImage2D&& image, const BufferUsage usage) {
    compressedSubImage(range, image, usage);
    return std::move(image);
}

}}
#endif
