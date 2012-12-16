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
#include "Context.h"
#include "Extensions.h"
#include "Image.h"

#include "Implementation/FramebufferState.h"
#include "Implementation/State.h"

namespace Magnum {

#ifndef DOXYGEN_GENERATING_OUTPUT
AbstractFramebuffer::Target AbstractFramebuffer::readTarget = AbstractFramebuffer::Target::ReadDraw;
AbstractFramebuffer::Target AbstractFramebuffer::drawTarget = AbstractFramebuffer::Target::ReadDraw;
#endif

void AbstractFramebuffer::bind(Target target) {
    Implementation::FramebufferState* state = Context::current()->state()->framebuffer;

    /* If already bound, done, otherwise update tracked state */
    if(target == Target::Read) {
        if(state->readBinding == _id) return;
        state->readBinding = _id;
    } else if(target == Target::Draw) {
        if(state->drawBinding == _id) return;
        state->drawBinding = _id;
    } else if(target == Target::ReadDraw) {
        if(state->readBinding == _id && state->drawBinding == _id) return;
        state->readBinding = state->drawBinding = _id;
    } else CORRADE_INTERNAL_ASSERT(false);

    glBindFramebuffer(static_cast<GLenum>(target), _id);
}

void AbstractFramebuffer::read(const Vector2i& offset, const Vector2i& size, AbstractImage::Format format, AbstractImage::Type type, Image2D* image) {
    bind(readTarget);
    char* data = new char[AbstractImage::pixelSize(format, type)*size.product()];
    glReadPixels(offset.x(), offset.y(), size.x(), size.y(), static_cast<GLenum>(format), static_cast<GLenum>(type), data);
    image->setData(size, format, type, data);
}

#ifndef MAGNUM_TARGET_GLES2
void AbstractFramebuffer::read(const Vector2i& offset, const Vector2i& size, AbstractImage::Format format, AbstractImage::Type type, BufferImage2D* image, Buffer::Usage usage) {
    bind(readTarget);
    /* If the buffer doesn't have sufficient size, resize it */
    /** @todo Explicitly reset also when buffer usage changes */
    if(image->size() != size || image->format() != format || image->type() != type)
        image->setData(size, format, type, nullptr, usage);

    image->buffer()->bind(Buffer::Target::PixelPack);
    glReadPixels(offset.x(), offset.y(), size.x(), size.y(), static_cast<GLenum>(format), static_cast<GLenum>(type), nullptr);
}
#endif

void AbstractFramebuffer::initializeContextBasedFunctionality(Context* context) {
    #ifndef MAGNUM_TARGET_GLES
    if(context->isExtensionSupported<Extensions::GL::EXT::framebuffer_blit>()) {
        Debug() << "AbstractFramebuffer: using" << Extensions::GL::EXT::framebuffer_blit::string() << "features";

        readTarget = Target::Read;
        drawTarget = Target::Draw;
    }
    #else
    static_cast<void>(context);
    #endif
}

}
