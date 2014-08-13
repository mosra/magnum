#ifndef Magnum_Implementation_FramebufferState_h
#define Magnum_Implementation_FramebufferState_h
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

#include <string>
#include <vector>

#include "Magnum/Framebuffer.h"

#ifdef CORRADE_GCC45_COMPATIBILITY
#include "Magnum/RenderbufferFormat.h"
#endif

/* We did't get memory corruption like in e.g. TextureState here, but include
   the header just to be sure */
#ifndef CORRADE_MSVC2013_COMPATIBILITY
#include "Magnum/Renderbuffer.h"
#endif

namespace Magnum { namespace Implementation {

struct FramebufferState {
    explicit FramebufferState(Context& context, std::vector<std::string>& extensions);

    void reset();

    #ifdef MAGNUM_TARGET_GLES2
    void(*blitImplementation)(const Range2Di&, const Range2Di&, FramebufferBlitMask, FramebufferBlitFilter);
    #endif
    GLenum(AbstractFramebuffer::*checkStatusImplementation)(FramebufferTarget);
    void(AbstractFramebuffer::*drawBuffersImplementation)(GLsizei, const GLenum*);
    void(AbstractFramebuffer::*drawBufferImplementation)(GLenum);
    void(AbstractFramebuffer::*readBufferImplementation)(GLenum);
    void(AbstractFramebuffer::*invalidateImplementation)(GLsizei, const GLenum*);
    #ifndef MAGNUM_TARGET_GLES2
    void(AbstractFramebuffer::*invalidateSubImplementation)(GLsizei, const GLenum*, const Range2Di&);
    #endif

    void(Framebuffer::*renderbufferImplementation)(Framebuffer::BufferAttachment, Renderbuffer&);
    #ifndef MAGNUM_TARGET_GLES
    void(Framebuffer::*texture1DImplementation)(Framebuffer::BufferAttachment, GLuint, GLint);
    #endif
    void(Framebuffer::*texture2DImplementation)(Framebuffer::BufferAttachment, GLenum, GLuint, GLint);
    void(Framebuffer::*textureLayerImplementation)(Framebuffer::BufferAttachment, GLuint, GLint, GLint);

    void(Renderbuffer::*renderbufferStorageImplementation)(RenderbufferFormat, const Vector2i&);
    void(Renderbuffer::*renderbufferStorageMultisampleImplementation)(GLsizei, RenderbufferFormat, const Vector2i&);

    void(*readImplementation)(const Vector2i&, const Vector2i&, ColorFormat, ColorType, std::size_t, GLvoid*);

    FramebufferTarget readTarget, drawTarget;

    GLuint readBinding, drawBinding, renderbufferBinding;
    GLint maxDrawBuffers, maxColorAttachments, maxRenderbufferSize, maxSamples;
    #ifndef MAGNUM_TARGET_GLES
    GLint maxDualSourceDrawBuffers;
    #endif
    Range2Di viewport;
    Vector2i maxViewportSize;
};

}}

#endif
