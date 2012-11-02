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

#include "Framebuffer.h"

#include "BufferedImage.h"
#include "Image.h"

namespace Magnum {

#ifndef MAGNUM_TARGET_GLES2
void Framebuffer::mapDefaultForDraw(std::initializer_list<DefaultDrawAttachment> attachments) {
    GLenum* _attachments = new GLenum[attachments.size()];
    for(auto it = attachments.begin(); it != attachments.end(); ++it)
        _attachments[it-attachments.begin()] = static_cast<GLenum>(*it);

    bindDefault(Target::Draw);
    glDrawBuffers(attachments.size(), _attachments);
    delete[] _attachments;
}
#endif

void Framebuffer::mapForDraw(std::initializer_list<std::int8_t> colorAttachments) {
    GLenum* attachments = new GLenum[colorAttachments.size()];
    for(auto it = colorAttachments.begin(); it != colorAttachments.end(); ++it)
        attachments[it-colorAttachments.begin()] = *it + GL_COLOR_ATTACHMENT0;

    bind(Target::Draw);
    /** @todo Re-enable when extension wrangler is available for ES2 */
    #ifndef MAGNUM_TARGET_GLES2
    glDrawBuffers(colorAttachments.size(), attachments);
    #endif
    delete[] attachments;
}

void Framebuffer::read(const Vector2i& offset, const Vector2i& size, AbstractImage::Format format, AbstractImage::Type type, Image2D* image) {
    char* data = new char[AbstractImage::pixelSize(format, type)*size.product()];
    glReadPixels(offset.x(), offset.y(), size.x(), size.y(), static_cast<GLenum>(format), static_cast<GLenum>(type), data);
    image->setData(size, format, type, data);
}

#ifndef MAGNUM_TARGET_GLES2
void Framebuffer::read(const Vector2i& offset, const Vector2i& size, AbstractImage::Format format, AbstractImage::Type type, BufferedImage2D* image, Buffer::Usage usage) {
    /* If the buffer doesn't have sufficient size, resize it */
    /** @todo Explicitly reset also when buffer usage changes */
    if(image->size() != size || image->format() != format || image->type() != type)
        image->setData(size, format, type, nullptr, usage);

    image->buffer()->bind(Buffer::Target::PixelPack);
    glReadPixels(offset.x(), offset.y(), size.x(), size.y(), static_cast<GLenum>(format), static_cast<GLenum>(type), nullptr);
}
#endif

}
