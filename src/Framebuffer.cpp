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

#include "BufferImage.h"
#include "Context.h"
#include "Image.h"
#include "Renderbuffer.h"
#include "Texture.h"

#include "Implementation/State.h"
#include "Implementation/FramebufferState.h"

namespace Magnum {

Framebuffer::Framebuffer(const Vector2i& viewportPosition, const Vector2i& viewportSize) {
    _viewportPosition = viewportPosition;
    _viewportSize = viewportSize;

    glGenFramebuffers(1, &_id);
}

Framebuffer::~Framebuffer() {
    /* If bound, remove itself from state */
    Implementation::FramebufferState* state = Context::current()->state()->framebuffer;
    if(state->readBinding == _id) state->readBinding = 0;
    if(state->drawBinding == _id) state->drawBinding = 0;

    glDeleteFramebuffers(1, &_id);
}

void Framebuffer::mapForDraw(std::initializer_list<std::pair<GLuint, std::int8_t>> attachments) {
    /* Max attachment location */
    std::size_t max = 0;
    for(const auto& attachment: attachments)
        if(attachment.first > max) max = attachment.first;

    /* Create linear array from associative */
    GLenum* _attachments = new GLenum[max+1];
    std::fill_n(_attachments, max, GL_NONE);
    for(const auto& attachment: attachments)
        _attachments[attachment.first] = attachment.second < 0 ? GL_NONE : GL_COLOR_ATTACHMENT0 + attachment.second;

    bindInternal(drawTarget);
    /** @todo Re-enable when extension wrangler is available for ES2 */
    #ifndef MAGNUM_TARGET_GLES2
    glDrawBuffers(max+1, _attachments);
    #endif
    delete[] _attachments;
}

void Framebuffer::mapForDraw(std::int8_t attachment) {
    bindInternal(drawTarget);
    glDrawBuffer(static_cast<GLenum>(attachment));
}

void Framebuffer::mapForRead(std::uint8_t colorAttachment) {
    bindInternal(Target::Read);
    /** @todo Get some extension wrangler instead to avoid undeclared glReadBuffer() on ES2 */
    #ifndef MAGNUM_TARGET_GLES2
    glReadBuffer(GL_COLOR_ATTACHMENT0 + colorAttachment);
    #else
    static_cast<void>(colorAttachment);
    #endif
}

void Framebuffer::attachRenderbuffer(Target target, DepthStencilAttachment depthStencilAttachment, Renderbuffer* renderbuffer) {
    bindInternal(target);
    glFramebufferRenderbuffer(static_cast<GLenum>(target), static_cast<GLenum>(depthStencilAttachment), GL_RENDERBUFFER, renderbuffer->id());
}

void Framebuffer::attachRenderbuffer(Target target, std::uint8_t colorAttachment, Renderbuffer* renderbuffer) {
    bindInternal(target);
    glFramebufferRenderbuffer(static_cast<GLenum>(target), GL_COLOR_ATTACHMENT0 + colorAttachment, GL_RENDERBUFFER, renderbuffer->id());
}

#ifndef MAGNUM_TARGET_GLES
void Framebuffer::attachTexture1D(Target target, DepthStencilAttachment depthStencilAttachment, Texture1D* texture, GLint mipLevel) {
    /** @todo Check for texture target compatibility */
    bindInternal(target);
    glFramebufferTexture1D(static_cast<GLenum>(target), static_cast<GLenum>(depthStencilAttachment), static_cast<GLenum>(texture->target()), texture->id(), mipLevel);
}

void Framebuffer::attachTexture1D(Target target, std::uint8_t colorAttachment, Texture1D* texture, GLint mipLevel) {
    /** @todo Check for texture target compatibility */
    bindInternal(target);
    glFramebufferTexture1D(static_cast<GLenum>(target), GL_COLOR_ATTACHMENT0 + colorAttachment, static_cast<GLenum>(texture->target()), texture->id(), mipLevel);
}
#endif

void Framebuffer::attachTexture2D(Target target, DepthStencilAttachment depthStencilAttachment, Texture2D* texture, GLint mipLevel) {
    /** @todo Check for texture target compatibility */
    bindInternal(target);
    glFramebufferTexture2D(static_cast<GLenum>(target), static_cast<GLenum>(depthStencilAttachment), static_cast<GLenum>(texture->target()), texture->id(), mipLevel);
}

void Framebuffer::attachTexture2D(Target target, std::uint8_t colorAttachment, Texture2D* texture, GLint mipLevel) {
    /** @todo Check for texture target compatibility */
    bindInternal(target);
    glFramebufferTexture2D(static_cast<GLenum>(target), GL_COLOR_ATTACHMENT0 + colorAttachment, static_cast<GLenum>(texture->target()), texture->id(), mipLevel);
}

void Framebuffer::attachCubeMapTexture(Target target, DepthStencilAttachment depthStencilAttachment, CubeMapTexture* texture, CubeMapTexture::Coordinate coordinate, GLint mipLevel) {
    /** @todo Check for internal format compatibility */
    bindInternal(target);
    glFramebufferTexture2D(static_cast<GLenum>(target), static_cast<GLenum>(depthStencilAttachment), static_cast<GLenum>(coordinate), texture->id(), mipLevel);
}

void Framebuffer::attachCubeMapTexture(Target target, std::uint8_t colorAttachment, CubeMapTexture* texture, CubeMapTexture::Coordinate coordinate, GLint mipLevel) {
    /** @todo Check for internal format compatibility */
    bindInternal(target);
    glFramebufferTexture2D(static_cast<GLenum>(target), GL_COLOR_ATTACHMENT0 + colorAttachment, static_cast<GLenum>(coordinate), texture->id(), mipLevel);
}

void Framebuffer::attachTexture3D(Target target, DepthStencilAttachment depthStencilAttachment, Texture3D* texture, GLint mipLevel, GLint layer) {
    /** @todo Check for texture target compatibility */
    bindInternal(target);
    /** @todo Get some extension wrangler for glFramebufferTexture3D() (extension only) */
    #ifndef MAGNUM_TARGET_GLES
    glFramebufferTexture3D(static_cast<GLenum>(target), static_cast<GLenum>(depthStencilAttachment), static_cast<GLenum>(texture->target()), texture->id(), mipLevel, layer);
    #else
    static_cast<void>(depthStencilAttachment);
    static_cast<void>(texture);
    static_cast<void>(mipLevel);
    static_cast<void>(layer);
    #endif
}

void Framebuffer::attachTexture3D(Target target, std::uint8_t colorAttachment, Texture3D* texture, GLint mipLevel, GLint layer) {
    /** @todo Check for texture target compatibility */
    bindInternal(target);
    /** @todo Get some extension wrangler for glFramebufferTexture3D() (extension only) */
    #ifndef MAGNUM_TARGET_GLES
    glFramebufferTexture3D(static_cast<GLenum>(target), GL_COLOR_ATTACHMENT0 + colorAttachment, static_cast<GLenum>(texture->target()), texture->id(), mipLevel, layer);
    #else
    static_cast<void>(colorAttachment);
    static_cast<void>(texture);
    static_cast<void>(mipLevel);
    static_cast<void>(layer);
    #endif
}

}
