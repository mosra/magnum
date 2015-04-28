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

#include "Framebuffer.h"

#include <Corrade/Containers/Array.h>

#include "Magnum/Context.h"
#include "Magnum/DefaultFramebuffer.h"
#include "Magnum/Extensions.h"
#include "Magnum/Image.h"
#include "Magnum/Renderbuffer.h"
#include "Magnum/Texture.h"

#ifndef MAGNUM_TARGET_GLES2
#include "Magnum/BufferImage.h"
#include "Magnum/MultisampleTexture.h"
#include "Magnum/TextureArray.h"
#endif

#ifndef MAGNUM_TARGET_GLES
#include "Magnum/CubeMapTextureArray.h"
#include "Magnum/RectangleTexture.h"
#endif

#include "Implementation/DebugState.h"
#include "Implementation/State.h"
#include "Implementation/FramebufferState.h"

namespace Magnum {

const Framebuffer::DrawAttachment Framebuffer::DrawAttachment::None = Framebuffer::DrawAttachment(GL_NONE);
const Framebuffer::BufferAttachment Framebuffer::BufferAttachment::Depth = Framebuffer::BufferAttachment(GL_DEPTH_ATTACHMENT);
const Framebuffer::BufferAttachment Framebuffer::BufferAttachment::Stencil = Framebuffer::BufferAttachment(GL_STENCIL_ATTACHMENT);
#ifndef MAGNUM_TARGET_GLES2
const Framebuffer::BufferAttachment Framebuffer::BufferAttachment::DepthStencil = Framebuffer::BufferAttachment(GL_DEPTH_STENCIL_ATTACHMENT);
#endif
const Framebuffer::InvalidationAttachment Framebuffer::InvalidationAttachment::Depth = Framebuffer::InvalidationAttachment(GL_DEPTH_ATTACHMENT);
const Framebuffer::InvalidationAttachment Framebuffer::InvalidationAttachment::Stencil = Framebuffer::InvalidationAttachment(GL_STENCIL_ATTACHMENT);

Int Framebuffer::maxColorAttachments() {
    #ifdef MAGNUM_TARGET_GLES2
    if(!Context::current()->isExtensionSupported<Extensions::GL::NV::fbo_color_attachments>())
        return 0;
    #endif

    GLint& value = Context::current()->state().framebuffer->maxColorAttachments;

    if(value == 0) {
        #ifndef MAGNUM_TARGET_GLES2
        glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &value);
        #else
        glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS_NV, &value);
        #endif
    }

    return value;
}

Framebuffer::Framebuffer(const Range2Di& viewport) {
    CORRADE_INTERNAL_ASSERT(viewport != Implementation::FramebufferState::DisengagedViewport);
    _viewport = viewport;
    (this->*Context::current()->state().framebuffer->createImplementation)();
    CORRADE_INTERNAL_ASSERT(_id != Implementation::State::DisengagedBinding);
}

void Framebuffer::createImplementationDefault() {
    glGenFramebuffers(1, &_id);
    _created = false;
}

#ifndef MAGNUM_TARGET_GLES
void Framebuffer::createImplementationDSA() {
    glCreateFramebuffers(1, &_id);
    _created = true;
}
#endif

Framebuffer::~Framebuffer() {
    /* Moved out, nothing to do */
    if(!_id) return;

    /* If bound, remove itself from state */
    Implementation::FramebufferState& state = *Context::current()->state().framebuffer;
    if(state.readBinding == _id) state.readBinding = 0;

    /* For draw binding reset also viewport */
    if(state.drawBinding == _id) {
        state.drawBinding = 0;

        /**
         * @todo Less ugly solution (need to call setViewportInternal() to
         * reset the viewport to size of default framebuffer)
         */
        defaultFramebuffer.bind();
    }

    glDeleteFramebuffers(1, &_id);
}

std::string Framebuffer::label() {
    createIfNotAlready();
    return Context::current()->state().debug->getLabelImplementation(GL_FRAMEBUFFER, _id);
}

Framebuffer& Framebuffer::setLabelInternal(const Containers::ArrayReference<const char> label) {
    createIfNotAlready();
    Context::current()->state().debug->labelImplementation(GL_FRAMEBUFFER, _id, label);
    return *this;
}

Framebuffer::Status Framebuffer::checkStatus(const FramebufferTarget target) {
    return Status((this->*Context::current()->state().framebuffer->checkStatusImplementation)(target));
}

Framebuffer& Framebuffer::mapForDraw(std::initializer_list<std::pair<UnsignedInt, DrawAttachment>> attachments) {
    /* Max attachment location */
    std::size_t max = 0;
    for(auto it = attachments.begin(); it != attachments.end(); ++it)
        if(it->first > max) max = it->first;

    /* Create linear array from associative */
    /** @todo C++14: use VLA to avoid heap allocation */
    static_assert(GL_NONE == 0, "Expecting zero GL_NONE for zero-initialization");
    auto _attachments = Containers::Array<GLenum>::zeroInitialized(max+1);
    for(auto it = attachments.begin(); it != attachments.end(); ++it)
        _attachments[it->first] = GLenum(it->second);

    (this->*Context::current()->state().framebuffer->drawBuffersImplementation)(max+1, _attachments);
    return *this;
}

Framebuffer& Framebuffer::mapForDraw(const DrawAttachment attachment) {
    (this->*Context::current()->state().framebuffer->drawBufferImplementation)(GLenum(attachment));
    return *this;
}

Framebuffer& Framebuffer::mapForRead(const ColorAttachment attachment) {
    (this->*Context::current()->state().framebuffer->readBufferImplementation)(GLenum(attachment));
    return *this;
}

void Framebuffer::invalidate(std::initializer_list<InvalidationAttachment> attachments) {
    /** @todo C++14: use VLA to avoid heap allocation */
    Containers::Array<GLenum> _attachments(attachments.size());
    for(std::size_t i = 0; i != attachments.size(); ++i)
        _attachments[i] = GLenum(*(attachments.begin()+i));

    (this->*Context::current()->state().framebuffer->invalidateImplementation)(attachments.size(), _attachments);
}

#ifndef MAGNUM_TARGET_GLES2
void Framebuffer::invalidate(std::initializer_list<InvalidationAttachment> attachments, const Range2Di& rectangle) {
    /** @todo C++14: use VLA to avoid heap allocation */
    Containers::Array<GLenum> _attachments(attachments.size());
    for(std::size_t i = 0; i != attachments.size(); ++i)
        _attachments[i] = GLenum(*(attachments.begin()+i));

    (this->*Context::current()->state().framebuffer->invalidateSubImplementation)(attachments.size(), _attachments, rectangle);
}
#endif

Framebuffer& Framebuffer::attachRenderbuffer(const BufferAttachment attachment, Renderbuffer& renderbuffer) {
    (this->*Context::current()->state().framebuffer->renderbufferImplementation)(attachment, renderbuffer);
    return *this;
}

#ifndef MAGNUM_TARGET_GLES
Framebuffer& Framebuffer::attachTexture(const BufferAttachment attachment, Texture1D& texture, const Int level) {
    (this->*Context::current()->state().framebuffer->texture1DImplementation)(attachment, texture.id(), level);
    return *this;
}
#endif

Framebuffer& Framebuffer::attachTexture(const BufferAttachment attachment, Texture2D& texture, const Int level) {
    (this->*Context::current()->state().framebuffer->texture2DImplementation)(attachment, GL_TEXTURE_2D, texture.id(), level);
    return *this;
}

#ifndef MAGNUM_TARGET_GLES
Framebuffer& Framebuffer::attachTexture(const BufferAttachment attachment, RectangleTexture& texture) {
    (this->*Context::current()->state().framebuffer->texture2DImplementation)(attachment, GL_TEXTURE_RECTANGLE, texture.id(), 0);
    return *this;
}
#endif

#ifndef MAGNUM_TARGET_GLES2
Framebuffer& Framebuffer::attachTexture(const BufferAttachment attachment, MultisampleTexture2D& texture) {
    (this->*Context::current()->state().framebuffer->texture2DImplementation)(attachment, GL_TEXTURE_2D_MULTISAMPLE, texture.id(), 0);
    return *this;
}
#endif

Framebuffer& Framebuffer::attachCubeMapTexture(const BufferAttachment attachment, CubeMapTexture& texture, CubeMapTexture::Coordinate coordinate, const Int level) {
    (this->*Context::current()->state().framebuffer->texture2DImplementation)(attachment, GLenum(coordinate), texture.id(), level);
    return *this;
}

Framebuffer& Framebuffer::attachTextureLayer(Framebuffer::BufferAttachment attachment, Texture3D& texture, Int level, Int layer) {
    (this->*Context::current()->state().framebuffer->textureLayerImplementation)(attachment, texture.id(), level, layer);
    return *this;
}

#ifndef MAGNUM_TARGET_GLES
Framebuffer& Framebuffer::attachTextureLayer(Framebuffer::BufferAttachment attachment, Texture1DArray& texture, Int level, Int layer) {
    (this->*Context::current()->state().framebuffer->textureLayerImplementation)(attachment, texture.id(), level, layer);
    return *this;
}
#endif

#ifndef MAGNUM_TARGET_GLES2
Framebuffer& Framebuffer::attachTextureLayer(Framebuffer::BufferAttachment attachment, Texture2DArray& texture, Int level, Int layer) {
    (this->*Context::current()->state().framebuffer->textureLayerImplementation)(attachment, texture.id(), level, layer);
    return *this;
}
#endif

#ifndef MAGNUM_TARGET_GLES
Framebuffer& Framebuffer::attachTextureLayer(Framebuffer::BufferAttachment attachment, CubeMapTextureArray& texture, Int level, Int layer) {
    (this->*Context::current()->state().framebuffer->textureLayerImplementation)(attachment, texture.id(), level, layer);
    return *this;
}

Framebuffer& Framebuffer::attachTextureLayer(Framebuffer::BufferAttachment attachment, MultisampleTexture2DArray& texture, Int layer) {
    (this->*Context::current()->state().framebuffer->textureLayerImplementation)(attachment, texture.id(), 0, layer);
    return *this;
}
#endif

#ifdef MAGNUM_BUILD_DEPRECATED
Framebuffer& Framebuffer::attachTexture2D(BufferAttachment attachment, Texture2D& texture, Int mipLevel) {
    #if defined(__GNUC__) && !defined(CORRADE_GCC45_COMPATIBILITY)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    #endif
    (this->*Context::current()->state().framebuffer->texture2DImplementation)(attachment, GLenum(texture.target()), texture.id(), mipLevel);
    #if defined(__GNUC__) && !defined(CORRADE_GCC45_COMPATIBILITY)
    #pragma GCC diagnostic pop
    #endif
    return *this;
}
#endif

void Framebuffer::renderbufferImplementationDefault(BufferAttachment attachment, Renderbuffer& renderbuffer) {
    glFramebufferRenderbuffer(GLenum(bindInternal()), GLenum(attachment), GL_RENDERBUFFER, renderbuffer.id());
}

#ifndef MAGNUM_TARGET_GLES
void Framebuffer::renderbufferImplementationDSA(const BufferAttachment attachment, Renderbuffer& renderbuffer) {
    glNamedFramebufferRenderbuffer(_id, GLenum(attachment), GL_RENDERBUFFER, renderbuffer.id());
}

void Framebuffer::renderbufferImplementationDSAEXT(BufferAttachment attachment, Renderbuffer& renderbuffer) {
    _created = true;
    glNamedFramebufferRenderbufferEXT(_id, GLenum(attachment), GL_RENDERBUFFER, renderbuffer.id());
}

void Framebuffer::texture1DImplementationDefault(BufferAttachment attachment, GLuint textureId, GLint mipLevel) {
    glFramebufferTexture1D(GLenum(bindInternal()), GLenum(attachment), GL_TEXTURE_1D, textureId, mipLevel);
}

void Framebuffer::texture1DImplementationDSA(const BufferAttachment attachment, const GLuint textureId, const GLint mipLevel) {
    glNamedFramebufferTexture(_id, GLenum(attachment), textureId, mipLevel);
}

void Framebuffer::texture1DImplementationDSAEXT(BufferAttachment attachment, GLuint textureId, GLint mipLevel) {
    _created = true;
    glNamedFramebufferTexture1DEXT(_id, GLenum(attachment), GL_TEXTURE_1D, textureId, mipLevel);
}
#endif

void Framebuffer::texture2DImplementationDefault(BufferAttachment attachment, GLenum textureTarget, GLuint textureId, GLint mipLevel) {
    glFramebufferTexture2D(GLenum(bindInternal()), GLenum(attachment), textureTarget, textureId, mipLevel);
}

#ifndef MAGNUM_TARGET_GLES
void Framebuffer::texture2DImplementationDSA(const BufferAttachment attachment, GLenum, const GLuint textureId, const GLint mipLevel) {
    glNamedFramebufferTexture(_id, GLenum(attachment), textureId, mipLevel);
}

void Framebuffer::texture2DImplementationDSAEXT(BufferAttachment attachment, GLenum textureTarget, GLuint textureId, GLint mipLevel) {
    _created = true;
    glNamedFramebufferTexture2DEXT(_id, GLenum(attachment), textureTarget, textureId, mipLevel);
}
#endif

void Framebuffer::textureLayerImplementationDefault(BufferAttachment attachment, GLuint textureId, GLint mipLevel, GLint layer) {
    #ifndef MAGNUM_TARGET_GLES2
    glFramebufferTextureLayer(GLenum(bindInternal()), GLenum(attachment), textureId, mipLevel, layer);
    #elif !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_NACL)
    glFramebufferTexture3DOES(GLenum(bindInternal()), GLenum(attachment), GL_TEXTURE_3D_OES, textureId, mipLevel, layer);
    #else
    static_cast<void>(attachment);
    static_cast<void>(textureId);
    static_cast<void>(mipLevel);
    static_cast<void>(layer);
    CORRADE_ASSERT_UNREACHABLE();
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void Framebuffer::textureLayerImplementationDSA(const BufferAttachment attachment, const GLuint textureId, const GLint mipLevel, const GLint layer) {
    glNamedFramebufferTextureLayer(_id, GLenum(attachment), textureId, mipLevel, layer);
}

void Framebuffer::textureLayerImplementationDSAEXT(BufferAttachment attachment, GLuint textureId, GLint mipLevel, GLint layer) {
    _created = true;
    glNamedFramebufferTextureLayerEXT(_id, GLenum(attachment), textureId, mipLevel, layer);
}
#endif

#ifndef DOXYGEN_GENERATING_OUTPUT
Debug operator<<(Debug debug, const Framebuffer::Status value) {
    switch(value) {
        #define _c(value) case Framebuffer::Status::value: return debug << "Framebuffer::Status::" #value;
        _c(Complete)
        _c(IncompleteAttachment)
        _c(IncompleteMissingAttachment)
        #ifndef MAGNUM_TARGET_GLES
        _c(IncompleteDrawBuffer)
        _c(IncompleteReadBuffer)
        #endif
        _c(Unsupported)
        _c(IncompleteMultisample)
        #ifndef MAGNUM_TARGET_GLES
        _c(IncompleteLayerTargets)
        #endif
        #undef _c
    }

    return debug << "Framebuffer::Status::(invalid)";
}
#endif

}
