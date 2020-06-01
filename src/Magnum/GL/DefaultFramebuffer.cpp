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

#include "DefaultFramebuffer.h"

#include <Corrade/Containers/Array.h>

#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/Implementation/State.h"
#include "Magnum/GL/Implementation/FramebufferState.h"
#ifndef MAGNUM_TARGET_GLES2
#include "Magnum/Math/Color.h"
#endif

namespace Magnum { namespace GL {

DefaultFramebuffer defaultFramebuffer;

DefaultFramebuffer::Status DefaultFramebuffer::checkStatus(const FramebufferTarget target) {
    return Status((this->*Context::current().state().framebuffer->checkStatusImplementation)(target));
}

#ifndef MAGNUM_TARGET_GLES2
DefaultFramebuffer& DefaultFramebuffer::clearColor(const Color4& color) {
    (this->*Context::current().state().framebuffer->clearFImplementation)(GL_COLOR, 0, color.data());
    return *this;
}

DefaultFramebuffer& DefaultFramebuffer::clearColor(const Vector4i& color) {
    (this->*Context::current().state().framebuffer->clearIImplementation)(GL_COLOR, 0, color.data());
    return *this;
}

DefaultFramebuffer& DefaultFramebuffer::clearColor(const Vector4ui& color) {
    (this->*Context::current().state().framebuffer->clearUIImplementation)(GL_COLOR, 0, color.data());
    return *this;
}
#endif

DefaultFramebuffer& DefaultFramebuffer::mapForDraw(std::initializer_list<std::pair<UnsignedInt, DrawAttachment>> attachments) {
    /* Max attachment location */
    std::size_t max = 0;
    for(const auto& attachment: attachments)
        if(attachment.first > max) max = attachment.first;

    /* Create linear array from associative */
    /** @todo C++14: use VLA to avoid heap allocation */
    static_assert(GL_NONE == 0, "Expecting zero GL_NONE for zero-initialization");
    Containers::Array<GLenum> _attachments{Containers::ValueInit, max+1};
    for(const auto& attachment: attachments)
        _attachments[attachment.first] = GLenum(attachment.second);

    (this->*Context::current().state().framebuffer->drawBuffersImplementation)(max+1, _attachments);
    return *this;
}

DefaultFramebuffer& DefaultFramebuffer::mapForDraw(const DrawAttachment attachment) {
    #ifndef MAGNUM_TARGET_GLES
    (this->*Context::current().state().framebuffer->drawBufferImplementation)(GLenum(attachment));
    #else
    (this->*Context::current().state().framebuffer->drawBuffersImplementation)(1, reinterpret_cast<const GLenum*>(&attachment));
    #endif
    return *this;
}

#if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
DefaultFramebuffer& DefaultFramebuffer::mapForRead(const ReadAttachment attachment) {
    (this->*Context::current().state().framebuffer->readBufferImplementation)(GLenum(attachment));
    return *this;
}

void DefaultFramebuffer::invalidate(std::initializer_list<InvalidationAttachment> attachments) {
    /** @todo C++14: use VLA to avoid heap allocation */
    Containers::Array<GLenum> _attachments(attachments.size());
    for(std::size_t i = 0; i != attachments.size(); ++i)
        _attachments[i] = GLenum(*(attachments.begin()+i));

    (this->*Context::current().state().framebuffer->invalidateImplementation)(attachments.size(), _attachments);
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void DefaultFramebuffer::invalidate(std::initializer_list<InvalidationAttachment> attachments, const Range2Di& rectangle) {
    /** @todo C++14: use VLA to avoid heap allocation */
    Containers::Array<GLenum> _attachments(attachments.size());
    for(std::size_t i = 0; i != attachments.size(); ++i)
        _attachments[i] = GLenum(*(attachments.begin()+i));

    (this->*Context::current().state().framebuffer->invalidateSubImplementation)(attachments.size(), _attachments, rectangle);
}
#endif

void DefaultFramebuffer::initializeContextBasedFunctionality(Context& context) {
    Implementation::FramebufferState& state = *context.state().framebuffer;

    /* Initial framebuffer size */
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    defaultFramebuffer._viewport = state.viewport = Range2Di::fromSize({viewport[0], viewport[1]}, {viewport[2], viewport[3]});
    CORRADE_INTERNAL_ASSERT(defaultFramebuffer._viewport != Implementation::FramebufferState::DisengagedViewport);
}

#ifndef DOXYGEN_GENERATING_OUTPUT
Debug& operator<<(Debug& debug, const DefaultFramebuffer::Status value) {
    debug << "GL::DefaultFramebuffer::Status" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case DefaultFramebuffer::Status::value: return debug << "::" #value;
        _c(Complete)
        #ifndef MAGNUM_TARGET_WEBGL
        _c(Undefined)
        #endif
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(GLenum(value)) << Debug::nospace << ")";
}
#endif

}}
