/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

#include "AbstractFramebuffer.h"

#include "BufferImage.h"
#include "Image.h"

namespace Magnum {

void AbstractFramebuffer::read(const Vector2i& offset, const Vector2i& size, AbstractImage::Format format, AbstractImage::Type type, Image2D* image) {
    bind(Target::ReadDraw);
    char* data = new char[AbstractImage::pixelSize(format, type)*size.product()];
    glReadPixels(offset.x(), offset.y(), size.x(), size.y(), static_cast<GLenum>(format), static_cast<GLenum>(type), data);
    image->setData(size, format, type, data);
}

#ifndef MAGNUM_TARGET_GLES2
void AbstractFramebuffer::read(const Vector2i& offset, const Vector2i& size, AbstractImage::Format format, AbstractImage::Type type, BufferImage2D* image, Buffer::Usage usage) {
    bind(Target::ReadDraw);
    /* If the buffer doesn't have sufficient size, resize it */
    /** @todo Explicitly reset also when buffer usage changes */
    if(image->size() != size || image->format() != format || image->type() != type)
        image->setData(size, format, type, nullptr, usage);

    image->buffer()->bind(Buffer::Target::PixelPack);
    glReadPixels(offset.x(), offset.y(), size.x(), size.y(), static_cast<GLenum>(format), static_cast<GLenum>(type), nullptr);
}
#endif

}
