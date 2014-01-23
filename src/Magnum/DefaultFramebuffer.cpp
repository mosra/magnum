/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014
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

#include "DefaultFramebuffer.h"

#include <Corrade/Containers/Array.h>

#include "Magnum/Context.h"
#include "Magnum/Extensions.h"
#include "Magnum/Implementation/State.h"
#include "Magnum/Implementation/FramebufferState.h"

namespace Magnum {

DefaultFramebuffer defaultFramebuffer;

DefaultFramebuffer::DefaultFramebuffer() { _id = 0; }

DefaultFramebuffer::Status DefaultFramebuffer::checkStatus(const FramebufferTarget target) {
    return Status((this->*Context::current()->state().framebuffer->checkStatusImplementation)(target));
}

#ifndef MAGNUM_TARGET_GLES2
DefaultFramebuffer& DefaultFramebuffer::mapForDraw(std::initializer_list<std::pair<UnsignedInt, DrawAttachment>> attachments) {
    /* Max attachment location */
    std::size_t max = 0;
    for(const auto& attachment: attachments)
        if(attachment.first > max) max = attachment.first;

    /* Create linear array from associative */
    /** @todo C++14: use VLA to avoid heap allocation */
    static_assert(GL_NONE == 0, "Expecting zero GL_NONE for zero-initialization");
    auto _attachments = Containers::Array<GLenum>::zeroInitialized(max+1);
    for(const auto& attachment: attachments)
        _attachments[attachment.first] = GLenum(attachment.second);

    (this->*Context::current()->state().framebuffer->drawBuffersImplementation)(max+1, _attachments);
    return *this;
}

DefaultFramebuffer& DefaultFramebuffer::mapForDraw(const DrawAttachment attachment) {
    (this->*Context::current()->state().framebuffer->drawBufferImplementation)(GLenum(attachment));
    return *this;
}
#endif

DefaultFramebuffer& DefaultFramebuffer::mapForRead(const ReadAttachment attachment) {
    (this->*Context::current()->state().framebuffer->readBufferImplementation)(GLenum(attachment));
    return *this;
}

void DefaultFramebuffer::invalidate(std::initializer_list<InvalidationAttachment> attachments) {
    /** @todo C++14: use VLA to avoid heap allocation */
    Containers::Array<GLenum> _attachments(attachments.size());
    for(std::size_t i = 0; i != attachments.size(); ++i)
        _attachments[i] = GLenum(*(attachments.begin()+i));

    invalidateImplementation(attachments.size(), _attachments);
}

void DefaultFramebuffer::invalidate(std::initializer_list<InvalidationAttachment> attachments, const Range2Di& rectangle) {
    /** @todo C++14: use VLA to avoid heap allocation */
    Containers::Array<GLenum> _attachments(attachments.size());
    for(std::size_t i = 0; i != attachments.size(); ++i)
        _attachments[i] = GLenum(*(attachments.begin()+i));

    invalidateImplementation(attachments.size(), _attachments, rectangle);
}

void DefaultFramebuffer::initializeContextBasedFunctionality(Context& context) {
    Implementation::FramebufferState* state = context.state().framebuffer;

    /* Initial framebuffer size */
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    defaultFramebuffer._viewport = state->viewport = Range2Di::fromSize({viewport[0], viewport[1]}, {viewport[2], viewport[3]});

    /* Fake initial glViewport() call for ApiTrace */
    #ifndef MAGNUM_TARGET_GLES
    if(context.isExtensionSupported<Extensions::GL::GREMEDY::string_marker>())
        glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
    #endif
}

#ifndef DOXYGEN_GENERATING_OUTPUT
Debug operator<<(Debug debug, const DefaultFramebuffer::Status value) {
    switch(value) {
        #define _c(value) case DefaultFramebuffer::Status::value: return debug << "DefaultFramebuffer::Status::" #value;
        _c(Complete)
        _c(Undefined)
        #undef _c
    }

    return debug << "DefaultFramebuffer::Status::(invalid)";
}
#endif

}
