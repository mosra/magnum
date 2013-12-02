/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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

#include "AbstractFramebuffer.h"

#include "BufferImage.h"
#include "Context.h"
#include "Extensions.h"
#include "Image.h"

#include "Implementation/FramebufferState.h"
#include "Implementation/State.h"

namespace Magnum {

AbstractFramebuffer::CheckStatusImplementation AbstractFramebuffer::checkStatusImplementation = &AbstractFramebuffer::checkStatusImplementationDefault;

AbstractFramebuffer::ReadImplementation AbstractFramebuffer::readImplementation = &AbstractFramebuffer::readImplementationDefault;

AbstractFramebuffer::DrawBuffersImplementation AbstractFramebuffer::drawBuffersImplementation = &AbstractFramebuffer::drawBuffersImplementationDefault;
AbstractFramebuffer::DrawBufferImplementation AbstractFramebuffer::drawBufferImplementation = &AbstractFramebuffer::drawBufferImplementationDefault;
AbstractFramebuffer::ReadBufferImplementation AbstractFramebuffer::readBufferImplementation = &AbstractFramebuffer::readBufferImplementationDefault;

#ifdef MAGNUM_TARGET_GLES2
FramebufferTarget AbstractFramebuffer::readTarget = FramebufferTarget::ReadDraw;
FramebufferTarget AbstractFramebuffer::drawTarget = FramebufferTarget::ReadDraw;
#endif

Vector2i AbstractFramebuffer::maxViewportSize() {
    Vector2i& value = Context::current()->state().framebuffer->maxViewportSize;

    /* Get the value, if not already cached */
    if(value == Vector2i())
        glGetIntegerv(GL_MAX_VIEWPORT_DIMS, value.data());

    return value;
}

Int AbstractFramebuffer::maxDrawBuffers() {
    #ifdef MAGNUM_TARGET_GLES2
    if(!Context::current()->isExtensionSupported<Extensions::GL::NV::draw_buffers>())
        return 0;
    #endif

    GLint& value = Context::current()->state().framebuffer->maxDrawBuffers;

    /* Get the value, if not already cached */
    if(value == 0) {
        #ifndef MAGNUM_TARGET_GLES2
        glGetIntegerv(GL_MAX_DRAW_BUFFERS, &value);
        #else
        glGetIntegerv(GL_MAX_DRAW_BUFFERS_NV, &value);
        #endif
    }

    return value;
}

#ifndef MAGNUM_TARGET_GLES
Int AbstractFramebuffer::maxDualSourceDrawBuffers() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::blend_func_extended>())
        return 0;

    GLint& value = Context::current()->state().framebuffer->maxDualSourceDrawBuffers;

    /* Get the value, if not already cached */
    if(value == 0)
        glGetIntegerv(GL_MAX_DUAL_SOURCE_DRAW_BUFFERS, &value);

    return value;
}
#endif

void AbstractFramebuffer::bind(FramebufferTarget target) {
    bindInternal(target);
    setViewportInternal();
}

void AbstractFramebuffer::bindInternal(FramebufferTarget target) {
    Implementation::FramebufferState* state = Context::current()->state().framebuffer;

    /* If already bound, done, otherwise update tracked state */
    if(target == FramebufferTarget::Read) {
        if(state->readBinding == _id) return;
        state->readBinding = _id;
    } else if(target == FramebufferTarget::Draw) {
        if(state->drawBinding == _id) return;
        state->drawBinding = _id;
    } else if(target == FramebufferTarget::ReadDraw) {
        if(state->readBinding == _id && state->drawBinding == _id) return;
        state->readBinding = state->drawBinding = _id;
    } else CORRADE_ASSERT_UNREACHABLE();

    glBindFramebuffer(GLenum(target), _id);
}

FramebufferTarget AbstractFramebuffer::bindInternal() {
    Implementation::FramebufferState* state = Context::current()->state().framebuffer;

    /* Return target to which the framebuffer is already bound */
    if(state->readBinding == _id && state->drawBinding == _id)
        return FramebufferTarget::ReadDraw;
    if(state->readBinding == _id)
        return FramebufferTarget::Read;
    if(state->drawBinding == _id)
        return FramebufferTarget::Draw;

    /* Or bind it, if not already */
    state->readBinding = _id;

    #ifndef MAGNUM_TARGET_GLES2
    glBindFramebuffer(GLenum(FramebufferTarget::Read), _id);
    return FramebufferTarget::Read;
    #else
    if(readTarget == FramebufferTarget::ReadDraw) state->drawBinding = _id;
    glBindFramebuffer(GLenum(readTarget), _id);
    return readTarget;
    #endif
}

void AbstractFramebuffer::blit(AbstractFramebuffer& source, AbstractFramebuffer& destination, const Range2Di& sourceRectangle, const Range2Di& destinationRectangle, FramebufferBlitMask mask, FramebufferBlitFilter filter) {
    source.bindInternal(FramebufferTarget::Read);
    destination.bindInternal(FramebufferTarget::Draw);
    /** @todo Get some extension wrangler instead to avoid undeclared glBlitFramebuffer() on ES2 */
    #ifndef MAGNUM_TARGET_GLES2
    glBlitFramebuffer(sourceRectangle.left(), sourceRectangle.bottom(), sourceRectangle.right(), sourceRectangle.top(), destinationRectangle.left(), destinationRectangle.bottom(), destinationRectangle.right(), destinationRectangle.top(), GLbitfield(mask), GLenum(filter));
    #else
    static_cast<void>(sourceRectangle);
    static_cast<void>(destinationRectangle);
    static_cast<void>(mask);
    static_cast<void>(filter);
    #endif
}

AbstractFramebuffer& AbstractFramebuffer::setViewport(const Range2Di& rectangle) {
    _viewport = rectangle;

    /* Update the viewport if the framebuffer is currently bound */
    if(Context::current()->state().framebuffer->drawBinding == _id)
        setViewportInternal();

    return *this;
}

void AbstractFramebuffer::setViewportInternal() {
    Implementation::FramebufferState* state = Context::current()->state().framebuffer;

    CORRADE_INTERNAL_ASSERT(state->drawBinding == _id);

    /* Already up-to-date, nothing to do */
    if(state->viewport == _viewport)
        return;

    /* Update the state and viewport */
    state->viewport = _viewport;
    glViewport(_viewport.left(), _viewport.bottom(), _viewport.sizeX(), _viewport.sizeY());
}

void AbstractFramebuffer::clear(FramebufferClearMask mask) {
    #ifndef MAGNUM_TARGET_GLES2
    bindInternal(FramebufferTarget::Draw);
    #else
    bindInternal(drawTarget);
    #endif
    glClear(GLbitfield(mask));
}

void AbstractFramebuffer::read(const Vector2i& offset, const Vector2i& size, Image2D& image) {
    #ifndef MAGNUM_TARGET_GLES2
    bindInternal(FramebufferTarget::Read);
    #else
    bindInternal(readTarget);
    #endif
    const std::size_t dataSize = image.pixelSize()*size.product();
    char* const data = new char[dataSize];
    readImplementation(offset, size, image.format(), image.type(), dataSize, data);
    image.setData(image.format(), image.type(), size, data);
}

#ifndef MAGNUM_TARGET_GLES2
void AbstractFramebuffer::read(const Vector2i& offset, const Vector2i& size, BufferImage2D& image, BufferUsage usage) {
    #ifndef MAGNUM_TARGET_GLES2
    bindInternal(FramebufferTarget::Read);
    #else
    bindInternal(readTarget);
    #endif
    /* If the buffer doesn't have sufficient size, resize it */
    /** @todo Explicitly reset also when buffer usage changes */
    if(image.size() != size)
        image.setData(size, image.format(), image.type(), nullptr, usage);

    image.buffer().bind(Buffer::Target::PixelPack);
    /** @todo De-duplicate buffer size computation */
    readImplementation(offset, size, image.format(), image.type(), image.pixelSize()*size.product(), nullptr);
}
#endif

void AbstractFramebuffer::invalidateImplementation(GLsizei count, GLenum* attachments) {
    /** @todo Re-enable when extension wrangler is available for ES2 */
    #ifndef MAGNUM_TARGET_GLES2
    glInvalidateFramebuffer(GLenum(bindInternal()), count, attachments);
    #else
    //glDiscardFramebufferEXT(GLenum(bindInternal()), count, attachments);
    static_cast<void>(count);
    static_cast<void>(attachments);
    #endif
}

void AbstractFramebuffer::invalidateImplementation(GLsizei count, GLenum* attachments, const Range2Di& rectangle) {
    /** @todo Re-enable when extension wrangler is available for ES2 */
    #ifndef MAGNUM_TARGET_GLES2
    glInvalidateSubFramebuffer(GLenum(bindInternal()), count, attachments, rectangle.left(), rectangle.bottom(), rectangle.sizeX(), rectangle.sizeY());
    #else
    //glDiscardSubFramebufferEXT(GLenum(bindInternal()), count, attachments, rectangle.left(), rectangle.bottom(), rectangle.width(), rectangle.height());
    static_cast<void>(count);
    static_cast<void>(attachments);
    static_cast<void>(rectangle);
    #endif
}

void AbstractFramebuffer::initializeContextBasedFunctionality(Context& context) {
    #ifndef MAGNUM_TARGET_GLES
    if(context.isExtensionSupported<Extensions::GL::EXT::direct_state_access>()) {
        Debug() << "AbstractFramebuffer: using" << Extensions::GL::EXT::direct_state_access::string() << "features";

        checkStatusImplementation = &AbstractFramebuffer::checkStatusImplementationDSA;
        drawBuffersImplementation = &AbstractFramebuffer::drawBuffersImplementationDSA;
        drawBufferImplementation = &AbstractFramebuffer::drawBufferImplementationDSA;
        readBufferImplementation = &AbstractFramebuffer::readBufferImplementationDSA;
    }
    #endif

    #ifdef MAGNUM_TARGET_GLES2
    /* Optimistically set separate binding targets and check if one of the
       extensions providing them is available */
    readTarget = FramebufferTarget::Read;
    drawTarget = FramebufferTarget::Draw;

    if(context.isExtensionSupported<Extensions::GL::ANGLE::framebuffer_blit>())
        Debug() << "AbstractFramebuffer: using" << Extensions::GL::ANGLE::framebuffer_blit::string() << "features";

    else if(context.isExtensionSupported<Extensions::GL::APPLE::framebuffer_multisample>())
        Debug() << "AbstractFramebuffer: using" << Extensions::GL::APPLE::framebuffer_multisample::string() << "features";

    else if(context.isExtensionSupported<Extensions::GL::NV::framebuffer_blit>())
        Debug() << "AbstractFramebuffer: using" << Extensions::GL::NV::framebuffer_blit::string() << "features";

    /* NV_framebuffer_multisample requires NV_framebuffer_blit, which has these
       enums. However, on my system only NV_framebuffer_multisample is
       supported, but NV_framebuffer_blit isn't. I will hold my breath and
       assume these enums are available. */
    else if(context.isExtensionSupported<Extensions::GL::NV::framebuffer_multisample>())
        Debug() << "AbstractFramebuffer: using" << Extensions::GL::NV::framebuffer_multisample::string() << "features";

    /* If no such extension is available, reset back to unified target */
    else readTarget = drawTarget = FramebufferTarget::ReadDraw;
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if(context.isExtensionSupported<Extensions::GL::ARB::robustness>())
    #else
    if(context.isExtensionSupported<Extensions::GL::EXT::robustness>())
    #endif
    {
        #ifndef MAGNUM_TARGET_GLES
        Debug() << "AbstractFramebuffer: using" << Extensions::GL::ARB::robustness::string() << "features";
        #else
        //Debug() << "AbstractFramebuffer: using" << Extensions::GL::EXT::robustness::string() << "features";
        #endif

        /** @todo Enable when extension wrangler for ES is available */
        #ifndef MAGNUM_TARGET_GLES
        readImplementation = &AbstractFramebuffer::readImplementationRobustness;
        #endif
    }
}

GLenum AbstractFramebuffer::checkStatusImplementationDefault(const FramebufferTarget target) {
    bindInternal(target);
    return glCheckFramebufferStatus(GLenum(target));
}

#ifndef MAGNUM_TARGET_GLES
GLenum AbstractFramebuffer::checkStatusImplementationDSA(const FramebufferTarget target) {
    return glCheckNamedFramebufferStatusEXT(_id, GLenum(target));
}
#endif

void AbstractFramebuffer::drawBuffersImplementationDefault(GLsizei count, const GLenum* buffers) {
    /** @todo Re-enable when extension wrangler is available for ES2 */
    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_GLES2
    bindInternal(FramebufferTarget::Draw);
    #else
    bindInternal(drawTarget);
    #endif
    glDrawBuffers(count, buffers);
    #else
    static_cast<void>(count);
    static_cast<void>(buffers);
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void AbstractFramebuffer::drawBuffersImplementationDSA(GLsizei count, const GLenum* buffers) {
    glFramebufferDrawBuffersEXT(_id, count, buffers);
}
#endif

void AbstractFramebuffer::drawBufferImplementationDefault(GLenum buffer) {
    /** @todo Re-enable when extension wrangler is available for ES2 */
    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_GLES2
    bindInternal(FramebufferTarget::Draw);
    #else
    bindInternal(drawTarget);
    #endif
    #ifndef MAGNUM_TARGET_GLES3
    glDrawBuffer(buffer);
    #else
    glDrawBuffers(1, &buffer);
    #endif
    #else
    static_cast<void>(buffer);
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void AbstractFramebuffer::drawBufferImplementationDSA(GLenum buffer) {
    glFramebufferDrawBufferEXT(_id, buffer);
}
#endif

void AbstractFramebuffer::readBufferImplementationDefault(GLenum buffer) {
    /** @todo Get some extension wrangler instead to avoid undeclared glReadBuffer() on ES2 */
    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_GLES2
    bindInternal(FramebufferTarget::Read);
    #else
    bindInternal(readTarget);
    #endif
    glReadBuffer(buffer);
    #else
    static_cast<void>(buffer);
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void AbstractFramebuffer::readBufferImplementationDSA(GLenum buffer) {
    glFramebufferReadBufferEXT(_id, buffer);
}
#endif

void AbstractFramebuffer::readImplementationDefault(const Vector2i& offset, const Vector2i& size, const ColorFormat format, const ColorType type, const std::size_t, GLvoid* const data) {
    glReadPixels(offset.x(), offset.y(), size.x(), size.y(), GLenum(format), GLenum(type), data);
}

void AbstractFramebuffer::readImplementationRobustness(const Vector2i& offset, const Vector2i& size, const ColorFormat format, const ColorType type, const std::size_t dataSize, GLvoid* const data) {
    /** @todo Enable when extension wrangler for ES is available */
    #ifndef MAGNUM_TARGET_GLES
    glReadnPixelsARB(offset.x(), offset.y(), size.x(), size.y(), GLenum(format), GLenum(type), dataSize, data);
    #else
    CORRADE_INTERNAL_ASSERT(false);
    //glReadnPixelsEXT(offset.x(), offset.y(), size.x(), size.y(), GLenum(format), GLenum(type), data);
    static_cast<void>(offset);
    static_cast<void>(size);
    static_cast<void>(format);
    static_cast<void>(type);
    static_cast<void>(dataSize);
    static_cast<void>(data);
    #endif
}

}
