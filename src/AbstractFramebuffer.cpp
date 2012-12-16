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
    bindInternal(target);
    setViewportInternal();
}

#ifndef DOXYGEN_GENERATING_OUTPUT
void AbstractFramebuffer::bindInternal(Target target) {
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
#endif

void AbstractFramebuffer::blit(AbstractFramebuffer& source, AbstractFramebuffer& destination, const Vector2i& sourceBottomLeft, const Vector2i& sourceTopRight, const Vector2i& destinationBottomLeft, const Vector2i& destinationTopRight, AbstractFramebuffer::BlitMask mask, AbstractFramebuffer::BlitFilter filter) {
    source.bindInternal(AbstractFramebuffer::Target::Read);
    destination.bindInternal(AbstractFramebuffer::Target::Draw);
    /** @todo Get some extension wrangler instead to avoid undeclared glBlitFramebuffer() on ES2 */
    #ifndef MAGNUM_TARGET_GLES2
    glBlitFramebuffer(sourceBottomLeft.x(), sourceBottomLeft.y(), sourceTopRight.x(), sourceTopRight.y(), destinationBottomLeft.x(), destinationBottomLeft.y(), destinationTopRight.x(), destinationTopRight.y(), static_cast<GLbitfield>(mask), static_cast<GLenum>(filter));
    #else
    static_cast<void>(sourceBottomLeft);
    static_cast<void>(sourceTopRight);
    static_cast<void>(destinationBottomLeft);
    static_cast<void>(destinationTopRight);
    static_cast<void>(mask);
    static_cast<void>(filter);
    #endif
}

void AbstractFramebuffer::setViewport(const Vector2i& position, const Vector2i& size) {
    _viewportPosition = position;
    _viewportSize = size;

    /* Update the viewport if the framebuffer is currently bound */
    if(Context::current()->state()->framebuffer->drawBinding == _id)
        setViewportInternal();
}

#ifndef DOXYGEN_GENERATING_OUTPUT
void AbstractFramebuffer::setViewportInternal() {
    Implementation::FramebufferState* state = Context::current()->state()->framebuffer;

    CORRADE_INTERNAL_ASSERT(state->drawBinding == _id);

    /* Already up-to-date, nothing to do */
    if(state->viewportPosition == _viewportPosition && state->viewportSize == _viewportSize)
        return;

    /* Update the state and viewport */
    state->viewportPosition = _viewportPosition;
    state->viewportSize = _viewportSize;
    glViewport(_viewportPosition.x(), _viewportPosition.y(), _viewportSize.x(), _viewportSize.y());
}
#endif

void AbstractFramebuffer::clear(ClearMask mask) {
    bindInternal(drawTarget);
    glClear(static_cast<GLbitfield>(mask));
}

void AbstractFramebuffer::read(const Vector2i& offset, const Vector2i& size, AbstractImage::Format format, AbstractImage::Type type, Image2D* image) {
    bindInternal(readTarget);
    char* data = new char[AbstractImage::pixelSize(format, type)*size.product()];
    glReadPixels(offset.x(), offset.y(), size.x(), size.y(), static_cast<GLenum>(format), static_cast<GLenum>(type), data);
    image->setData(size, format, type, data);
}

#ifndef MAGNUM_TARGET_GLES2
void AbstractFramebuffer::read(const Vector2i& offset, const Vector2i& size, AbstractImage::Format format, AbstractImage::Type type, BufferImage2D* image, Buffer::Usage usage) {
    bindInternal(readTarget);
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
