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

#include "Framebuffer.h"

#include <Corrade/Containers/Array.h>

#include "Magnum/Image.h"
#include "Magnum/GL/Context.h"
#include "Magnum/GL/CubeMapTexture.h"
#include "Magnum/GL/DefaultFramebuffer.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/Renderbuffer.h"
#include "Magnum/GL/Texture.h"
#ifndef MAGNUM_TARGET_GLES2
#include "Magnum/GL/BufferImage.h"
#ifndef MAGNUM_TARGET_WEBGL
#include "Magnum/GL/MultisampleTexture.h"
#endif
#include "Magnum/GL/TextureArray.h"
#endif
#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
#include "Magnum/GL/CubeMapTextureArray.h"
#endif
#ifndef MAGNUM_TARGET_GLES
#include "Magnum/GL/RectangleTexture.h"
#endif
#ifndef MAGNUM_TARGET_WEBGL
#include "Magnum/GL/Implementation/DebugState.h"
#endif
#include "Magnum/GL/Implementation/State.h"
#include "Magnum/GL/Implementation/FramebufferState.h"
#ifndef MAGNUM_TARGET_GLES2
#include "Magnum/Math/Color.h"
#endif

namespace Magnum { namespace GL {

const Framebuffer::DrawAttachment Framebuffer::DrawAttachment::None = Framebuffer::DrawAttachment(GL_NONE);
const Framebuffer::BufferAttachment Framebuffer::BufferAttachment::Depth = Framebuffer::BufferAttachment(GL_DEPTH_ATTACHMENT);
const Framebuffer::BufferAttachment Framebuffer::BufferAttachment::Stencil = Framebuffer::BufferAttachment(GL_STENCIL_ATTACHMENT);
/** @todo where to get GL_DEPTH_STENCIL_ATTACHMENT for WebGL? */
#ifndef MAGNUM_TARGET_GLES2
const Framebuffer::BufferAttachment Framebuffer::BufferAttachment::DepthStencil = Framebuffer::BufferAttachment(GL_DEPTH_STENCIL_ATTACHMENT);
#elif defined(MAGNUM_TARGET_WEBGL)
const Framebuffer::BufferAttachment Framebuffer::BufferAttachment::DepthStencil = Framebuffer::BufferAttachment(0x821A);
#endif
#if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
const Framebuffer::InvalidationAttachment Framebuffer::InvalidationAttachment::Depth = Framebuffer::InvalidationAttachment(GL_DEPTH_ATTACHMENT);
const Framebuffer::InvalidationAttachment Framebuffer::InvalidationAttachment::Stencil = Framebuffer::InvalidationAttachment(GL_STENCIL_ATTACHMENT);
#endif

Int Framebuffer::maxColorAttachments() {
    #ifdef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_WEBGL
    if(!Context::current().isExtensionSupported<Extensions::EXT::draw_buffers>() &&
       !Context::current().isExtensionSupported<Extensions::NV::fbo_color_attachments>())
        return 0;
    #else
    if(!Context::current().isExtensionSupported<Extensions::WEBGL::draw_buffers>())
        return 0;
    #endif
    #endif

    GLint& value = Context::current().state().framebuffer->maxColorAttachments;

    if(value == 0) {
        #ifndef MAGNUM_TARGET_GLES2
        glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &value);
        #else
        glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS_EXT, &value);
        #endif
    }

    return value;
}

Framebuffer::Framebuffer(const Range2Di& viewport): AbstractFramebuffer{0, viewport, ObjectFlag::DeleteOnDestruction} {
    CORRADE_INTERNAL_ASSERT(viewport != Implementation::FramebufferState::DisengagedViewport);
    _viewport = viewport;
    (this->*Context::current().state().framebuffer->createImplementation)();
    CORRADE_INTERNAL_ASSERT(_id != Implementation::State::DisengagedBinding);
}

void Framebuffer::createImplementationDefault() {
    glGenFramebuffers(1, &_id);
}

#ifndef MAGNUM_TARGET_GLES
void Framebuffer::createImplementationDSA() {
    glCreateFramebuffers(1, &_id);
    _flags |= ObjectFlag::Created;
}
#endif

Framebuffer::~Framebuffer() {
    /* Moved out or not deleting on destruction, nothing to do */
    if(!_id || !(_flags & ObjectFlag::DeleteOnDestruction)) return;

    /* If bound, remove itself from state */
    Implementation::FramebufferState& state = *Context::current().state().framebuffer;
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

#ifndef MAGNUM_TARGET_WEBGL
std::string Framebuffer::label() {
    createIfNotAlready();
    return Context::current().state().debug->getLabelImplementation(GL_FRAMEBUFFER, _id);
}

Framebuffer& Framebuffer::setLabelInternal(const Containers::ArrayView<const char> label) {
    createIfNotAlready();
    Context::current().state().debug->labelImplementation(GL_FRAMEBUFFER, _id, label);
    return *this;
}
#endif

Framebuffer::Status Framebuffer::checkStatus(const FramebufferTarget target) {
    return Status((this->*Context::current().state().framebuffer->checkStatusImplementation)(target));
}

#ifndef MAGNUM_TARGET_GLES2
Framebuffer& Framebuffer::clearColor(const Int attachment, const Color4& color) {
    (this->*Context::current().state().framebuffer->clearFImplementation)(GL_COLOR, attachment, color.data());
    return *this;
}

Framebuffer& Framebuffer::clearColor(const Int attachment, const Vector4i& color) {
    (this->*Context::current().state().framebuffer->clearIImplementation)(GL_COLOR, attachment, color.data());
    return *this;
}

Framebuffer& Framebuffer::clearColor(const Int attachment, const Vector4ui& color) {
    (this->*Context::current().state().framebuffer->clearUIImplementation)(GL_COLOR, attachment, color.data());
    return *this;
}
#endif

Framebuffer& Framebuffer::mapForDraw(std::initializer_list<std::pair<UnsignedInt, DrawAttachment>> attachments) {
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

Framebuffer& Framebuffer::mapForDraw(const DrawAttachment attachment) {
    #ifndef MAGNUM_TARGET_GLES
    (this->*Context::current().state().framebuffer->drawBufferImplementation)(GLenum(attachment));
    #else
    (this->*Context::current().state().framebuffer->drawBuffersImplementation)(1, reinterpret_cast<const GLenum*>(&attachment));
    #endif
    return *this;
}

#if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
Framebuffer& Framebuffer::mapForRead(const ColorAttachment attachment) {
    (this->*Context::current().state().framebuffer->readBufferImplementation)(GLenum(attachment));
    return *this;
}

void Framebuffer::invalidate(std::initializer_list<InvalidationAttachment> attachments) {
    /** @todo C++14: use VLA to avoid heap allocation */
    Containers::Array<GLenum> _attachments(attachments.size());
    for(std::size_t i = 0; i != attachments.size(); ++i)
        _attachments[i] = GLenum(*(attachments.begin()+i));

    (this->*Context::current().state().framebuffer->invalidateImplementation)(attachments.size(), _attachments);
}

#ifndef MAGNUM_TARGET_GLES2
void Framebuffer::invalidate(std::initializer_list<InvalidationAttachment> attachments, const Range2Di& rectangle) {
    /** @todo C++14: use VLA to avoid heap allocation */
    Containers::Array<GLenum> _attachments(attachments.size());
    for(std::size_t i = 0; i != attachments.size(); ++i)
        _attachments[i] = GLenum(*(attachments.begin()+i));

    (this->*Context::current().state().framebuffer->invalidateSubImplementation)(attachments.size(), _attachments, rectangle);
}
#endif
#endif

Framebuffer& Framebuffer::attachRenderbuffer(const BufferAttachment attachment, Renderbuffer& renderbuffer) {
    (this->*Context::current().state().framebuffer->renderbufferImplementation)(attachment, renderbuffer.id());
    return *this;
}

#ifndef MAGNUM_TARGET_GLES
Framebuffer& Framebuffer::attachTexture(const BufferAttachment attachment, Texture1D& texture, const Int level) {
    (this->*Context::current().state().framebuffer->texture1DImplementation)(attachment, texture.id(), level);
    return *this;
}
#endif

Framebuffer& Framebuffer::attachTexture(const BufferAttachment attachment, Texture2D& texture, const Int level) {
    (this->*Context::current().state().framebuffer->texture2DImplementation)(attachment, GL_TEXTURE_2D, texture.id(), level);
    return *this;
}

#ifndef MAGNUM_TARGET_GLES
Framebuffer& Framebuffer::attachTexture(const BufferAttachment attachment, RectangleTexture& texture) {
    (this->*Context::current().state().framebuffer->texture2DImplementation)(attachment, GL_TEXTURE_RECTANGLE, texture.id(), 0);
    return *this;
}
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
Framebuffer& Framebuffer::attachTexture(const BufferAttachment attachment, MultisampleTexture2D& texture) {
    (this->*Context::current().state().framebuffer->texture2DImplementation)(attachment, GL_TEXTURE_2D_MULTISAMPLE, texture.id(), 0);
    return *this;
}
#endif

Framebuffer& Framebuffer::attachCubeMapTexture(const BufferAttachment attachment, CubeMapTexture& texture, CubeMapCoordinate coordinate, const Int level) {
    (this->*Context::current().state().framebuffer->textureCubeMapImplementation)(attachment, GLenum(coordinate), texture.id(), level);
    return *this;
}

#if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
Framebuffer& Framebuffer::attachTextureLayer(const BufferAttachment attachment, Texture3D& texture, Int level, Int layer) {
    (this->*Context::current().state().framebuffer->textureLayerImplementation)(attachment, texture.id(), level, layer);
    return *this;
}
#endif

#ifndef MAGNUM_TARGET_GLES
Framebuffer& Framebuffer::attachTextureLayer(const BufferAttachment attachment, Texture1DArray& texture, Int level, Int layer) {
    (this->*Context::current().state().framebuffer->textureLayerImplementation)(attachment, texture.id(), level, layer);
    return *this;
}
#endif

#ifndef MAGNUM_TARGET_GLES2
Framebuffer& Framebuffer::attachTextureLayer(const BufferAttachment attachment, Texture2DArray& texture, Int level, Int layer) {
    (this->*Context::current().state().framebuffer->textureLayerImplementation)(attachment, texture.id(), level, layer);
    return *this;
}
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
Framebuffer& Framebuffer::attachTextureLayer(const BufferAttachment attachment, CubeMapTextureArray& texture, Int level, Int layer) {
    (this->*Context::current().state().framebuffer->textureLayerImplementation)(attachment, texture.id(), level, layer);
    return *this;
}

Framebuffer& Framebuffer::attachTextureLayer(const BufferAttachment attachment, MultisampleTexture2DArray& texture, Int layer) {
    (this->*Context::current().state().framebuffer->textureLayerImplementation)(attachment, texture.id(), 0, layer);
    return *this;
}
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
Framebuffer& Framebuffer::attachLayeredTexture(const BufferAttachment attachment, Texture3D& texture, const Int level) {
    (this->*Context::current().state().framebuffer->textureImplementation)(attachment, texture.id(), level);
    return *this;
}

#ifndef MAGNUM_TARGET_GLES
Framebuffer& Framebuffer::attachLayeredTexture(const BufferAttachment attachment, Texture1DArray& texture, const Int level) {
    (this->*Context::current().state().framebuffer->textureImplementation)(attachment, texture.id(), level);
    return *this;
}
#endif

Framebuffer& Framebuffer::attachLayeredTexture(const BufferAttachment attachment, Texture2DArray& texture, const Int level) {
    (this->*Context::current().state().framebuffer->textureImplementation)(attachment, texture.id(), level);
    return *this;
}

Framebuffer& Framebuffer::attachLayeredTexture(const BufferAttachment attachment, CubeMapTexture& texture, const Int level) {
    (this->*Context::current().state().framebuffer->textureImplementation)(attachment, texture.id(), level);
    return *this;
}

Framebuffer& Framebuffer::attachLayeredTexture(const BufferAttachment attachment, CubeMapTextureArray& texture, const Int level) {
    (this->*Context::current().state().framebuffer->layeredTextureCubeMapArrayImplementation)(attachment, texture.id(), level);
    return *this;
}

Framebuffer& Framebuffer::attachLayeredTexture(const BufferAttachment attachment, MultisampleTexture2DArray& texture) {
    (this->*Context::current().state().framebuffer->textureImplementation)(attachment, texture.id(), 0);
    return *this;
}
#endif

Framebuffer& Framebuffer::detach(const BufferAttachment attachment) {
    (this->*Context::current().state().framebuffer->renderbufferImplementation)(attachment, 0);
    return *this;
}

void Framebuffer::renderbufferImplementationDefault(const BufferAttachment attachment, const GLuint renderbufferId) {
    glFramebufferRenderbuffer(GLenum(bindInternal()), GLenum(attachment), GL_RENDERBUFFER, renderbufferId);
}

#ifndef MAGNUM_TARGET_GLES
void Framebuffer::renderbufferImplementationDSA(const BufferAttachment attachment, const GLuint renderbufferId) {
    glNamedFramebufferRenderbuffer(_id, GLenum(attachment), GL_RENDERBUFFER, renderbufferId);
}

void Framebuffer::texture1DImplementationDefault(BufferAttachment attachment, GLuint textureId, GLint mipLevel) {
    glFramebufferTexture1D(GLenum(bindInternal()), GLenum(attachment), GL_TEXTURE_1D, textureId, mipLevel);
}

#endif

void Framebuffer::texture2DImplementationDefault(BufferAttachment attachment, GLenum textureTarget, GLuint textureId, GLint mipLevel) {
    glFramebufferTexture2D(GLenum(bindInternal()), GLenum(attachment), textureTarget, textureId, mipLevel);
}

#ifndef MAGNUM_TARGET_GLES
void Framebuffer::texture2DImplementationDSA(const BufferAttachment attachment, GLenum, const GLuint textureId, const GLint mipLevel) {
    glNamedFramebufferTexture(_id, GLenum(attachment), textureId, mipLevel);
}

void Framebuffer::textureCubeMapImplementationDSA(const BufferAttachment attachment, const GLenum textureTarget, const GLuint textureId, const GLint mipLevel) {
    glNamedFramebufferTextureLayer(_id, GLenum(attachment), textureId, mipLevel, textureTarget - GL_TEXTURE_CUBE_MAP_POSITIVE_X);
}
#endif

#if !defined(MAGNUM_TARGET_WEBGL) && !defined(MAGNUM_TARGET_GLES2)
void Framebuffer::textureImplementationDefault(BufferAttachment attachment, GLuint textureId, GLint mipLevel) {
    glFramebufferTexture(GLenum(bindInternal()), GLenum(attachment), textureId, mipLevel);
}

#ifdef MAGNUM_TARGET_GLES
void Framebuffer::textureImplementationEXT(BufferAttachment attachment, GLuint textureId, GLint mipLevel) {
    glFramebufferTextureEXT(GLenum(bindInternal()), GLenum(attachment), textureId, mipLevel);
}
#endif
#endif

#ifndef MAGNUM_TARGET_GLES
void Framebuffer::textureImplementationDSA(const BufferAttachment attachment, const GLuint textureId, const GLint mipLevel) {
    glNamedFramebufferTexture(_id, GLenum(attachment), textureId, mipLevel);
}
#endif

#if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
void Framebuffer::textureLayerImplementationDefault(BufferAttachment attachment, GLuint textureId, GLint mipLevel, GLint layer) {
    #ifndef MAGNUM_TARGET_GLES2
    glFramebufferTextureLayer(GLenum(bindInternal()), GLenum(attachment), textureId, mipLevel, layer);
    #else
    glFramebufferTexture3DOES(GLenum(bindInternal()), GLenum(attachment), GL_TEXTURE_3D_OES, textureId, mipLevel, layer);
    #endif
}
#endif

#ifndef MAGNUM_TARGET_GLES
void Framebuffer::textureLayerImplementationDSA(const BufferAttachment attachment, const GLuint textureId, const GLint mipLevel, const GLint layer) {
    glNamedFramebufferTextureLayer(_id, GLenum(attachment), textureId, mipLevel, layer);
}
#endif

#ifndef DOXYGEN_GENERATING_OUTPUT
Debug& operator<<(Debug& debug, const Framebuffer::Status value) {
    debug << "GL::Framebuffer::Status" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case Framebuffer::Status::value: return debug << "::" #value;
        _c(Complete)
        _c(IncompleteAttachment)
        _c(IncompleteMissingAttachment)
        #ifdef MAGNUM_TARGET_GLES2
        _c(IncompleteDimensions)
        #endif
        #ifndef MAGNUM_TARGET_GLES
        _c(IncompleteDrawBuffer)
        _c(IncompleteReadBuffer)
        #endif
        _c(Unsupported)
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        _c(IncompleteMultisample)
        #endif
        #ifndef MAGNUM_TARGET_GLES
        _c(IncompleteLayerTargets)
        #endif
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(GLenum(value)) << Debug::nospace << ")";
}
#endif

}}
