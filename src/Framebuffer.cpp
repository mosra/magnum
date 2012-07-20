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

namespace Magnum {

Framebuffer::ClearMask Framebuffer::clearMask = Framebuffer::Clear::Color;

void Framebuffer::setFeature(Feature feature, bool enabled) {
    /* Enable or disable the feature */
    enabled ? glEnable(static_cast<GLenum>(feature)) : glDisable(static_cast<GLenum>(feature));

    /* Update clear mask, if needed */
    ClearMask clearMaskChange;
    if(feature == Feature::DepthTest) clearMaskChange = Clear::Depth;
    else if(feature == Feature::StencilTest) clearMaskChange = Clear::Stencil;
    else return;

    enabled ? clearMask |= clearMaskChange : clearMask &= ~clearMaskChange;
}

void Framebuffer::setViewport(const Math::Vector2<GLint>& position, const Math::Vector2<GLsizei>& size) {
    glViewport(position.x(), position.y(), size.x(), size.y());
}

#ifndef MAGNUM_TARGET_GLES
void Framebuffer::mapDefaultForDraw(std::initializer_list<DefaultDrawAttachment> attachments) {
    GLenum* _attachments = new GLenum[attachments.size()];
    for(auto it = attachments.begin(); it != attachments.end(); ++it)
        _attachments[it-attachments.begin()] = static_cast<GLenum>(*it);

    bindDefault(Target::Draw);
    glDrawBuffers(attachments.size(), _attachments);
    delete[] _attachments;
}

void Framebuffer::mapForDraw(std::initializer_list<int> colorAttachments) {
    GLenum* attachments = new GLenum[colorAttachments.size()];
    for(auto it = colorAttachments.begin(); it != colorAttachments.end(); ++it)
        attachments[it-colorAttachments.begin()] = *it + GL_COLOR_ATTACHMENT0;

    bind(Target::Draw);
    glDrawBuffers(colorAttachments.size(), attachments);
    delete[] attachments;
}
#endif

void Framebuffer::read(const Math::Vector2<GLint>& offset, const Math::Vector2<GLsizei>& dimensions, AbstractImage::Components components, AbstractImage::ComponentType type, Image2D* image) {
    char* data = new char[AbstractImage::pixelSize(components, type)*dimensions.product()];
    glReadPixels(offset.x(), offset.y(), dimensions.x(), dimensions.y(), static_cast<GLenum>(components), static_cast<GLenum>(type), data);
    image->setData(dimensions, components, type, data);
}

#ifndef MAGNUM_TARGET_GLES
void Framebuffer::read(const Math::Vector2<GLint>& offset, const Math::Vector2<GLsizei>& dimensions, AbstractImage::Components components, AbstractImage::ComponentType type, BufferedImage2D* image, Buffer::Usage usage) {
    /* If the buffer doesn't have sufficient size, resize it */
    /** @todo Explicitly reset also when buffer usage changes */
    if(image->dimensions() != dimensions || image->components() != components || image->type() != type)
        image->setData(dimensions, components, type, nullptr, usage);

    image->buffer()->bind(Buffer::Target::PixelPack);
    glReadPixels(offset.x(), offset.y(), dimensions.x(), dimensions.y(), static_cast<GLenum>(components), static_cast<GLenum>(type), nullptr);
}
#endif

}
