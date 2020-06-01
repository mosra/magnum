#ifndef Magnum_GL_Implementation_FramebufferState_h
#define Magnum_GL_Implementation_FramebufferState_h
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

#include <string>
#include <vector>

#include "Magnum/GL/Framebuffer.h"

#ifdef _MSC_VER
/* Otherwise the member function pointers will have different size based on
   whether the header was included or not. CAUSES SERIOUS MEMORY CORRUPTION AND
   IS NOT CAUGHT BY ANY WARNING WHATSOEVER! AARGH! */
#include "Magnum/GL/Renderbuffer.h"
#endif

namespace Magnum { namespace GL { namespace Implementation {

struct FramebufferState {
    constexpr static const Range2Di DisengagedViewport{{}, {-1, -1}};

    explicit FramebufferState(Context& context, std::vector<std::string>& extensions);

    void reset();

    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    void(*blitImplementation)(AbstractFramebuffer&, AbstractFramebuffer&, const Range2Di&, const Range2Di&, FramebufferBlitMask, FramebufferBlitFilter);
    #endif
    GLenum(AbstractFramebuffer::*checkStatusImplementation)(FramebufferTarget);

    #ifndef MAGNUM_TARGET_GLES2
    void(AbstractFramebuffer::*clearIImplementation)(GLenum, GLint, const GLint*);
    void(AbstractFramebuffer::*clearUIImplementation)(GLenum, GLint, const GLuint*);
    void(AbstractFramebuffer::*clearFImplementation)(GLenum, GLint, const GLfloat*);
    void(AbstractFramebuffer::*clearFIImplementation)(GLenum, GLfloat, GLint);
    #endif

    void(AbstractFramebuffer::*drawBuffersImplementation)(GLsizei, const GLenum*);
    #ifndef MAGNUM_TARGET_GLES
    void(AbstractFramebuffer::*drawBufferImplementation)(GLenum);
    #endif
    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    void(AbstractFramebuffer::*readBufferImplementation)(GLenum);
    #endif
    #ifndef MAGNUM_TARGET_GLES
    void(*copySub1DImplementation)(const Range2Di&, AbstractTexture&, Int, Int);
    #endif
    void(*copySub2DImplementation)(const Range2Di&, AbstractTexture&, GLenum, Int, const Vector2i&);
    void(*copySubCubeMapImplementation)(const Range2Di&, AbstractTexture&, GLenum, Int, const Vector2i&);
    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    void(*copySub3DImplementation)(const Range2Di&, AbstractTexture&, Int, const Vector3i&);
    #endif
    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    void(AbstractFramebuffer::*invalidateImplementation)(GLsizei, const GLenum*);
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    void(AbstractFramebuffer::*invalidateSubImplementation)(GLsizei, const GLenum*, const Range2Di&);
    #endif
    #if defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    void(AbstractFramebuffer::*bindImplementation)(FramebufferTarget);
    FramebufferTarget(AbstractFramebuffer::*bindInternalImplementation)();
    #endif

    GLenum(AbstractFramebuffer::*implementationColorReadFormatTypeImplementation)(GLenum what);

    void(Framebuffer::*createImplementation)();
    void(Framebuffer::*renderbufferImplementation)(Framebuffer::BufferAttachment, GLuint);
    #ifndef MAGNUM_TARGET_GLES
    void(Framebuffer::*texture1DImplementation)(Framebuffer::BufferAttachment, GLuint, GLint);
    #endif
    void(Framebuffer::*texture2DImplementation)(Framebuffer::BufferAttachment, GLenum, GLuint, GLint);
    void(Framebuffer::*textureCubeMapImplementation)(Framebuffer::BufferAttachment, GLenum, GLuint, GLint);
    #if !defined(MAGNUM_TARGET_WEBGL) && !defined(MAGNUM_TARGET_GLES2)
    void(Framebuffer::*textureImplementation)(Framebuffer::BufferAttachment, GLuint, GLint);
    void(Framebuffer::*layeredTextureCubeMapArrayImplementation)(Framebuffer::BufferAttachment, GLuint, GLint);
    #endif
    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    void(Framebuffer::*textureLayerImplementation)(Framebuffer::BufferAttachment, GLuint, GLint, GLint);
    #endif

    void(Renderbuffer::*createRenderbufferImplementation)();
    void(Renderbuffer::*renderbufferStorageImplementation)(RenderbufferFormat, const Vector2i&);
    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    void(Renderbuffer::*renderbufferStorageMultisampleImplementation)(GLsizei, RenderbufferFormat, const Vector2i&);
    #endif

    void(*readImplementation)(const Range2Di&, PixelFormat, PixelType, std::size_t, GLvoid*);

    GLuint readBinding, drawBinding, renderbufferBinding;
    GLint maxDrawBuffers, maxColorAttachments, maxRenderbufferSize;
    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    GLint maxSamples;
    #endif
    #ifndef MAGNUM_TARGET_GLES
    GLint maxDualSourceDrawBuffers;
    #endif
    Range2Di viewport;
    Vector2i maxViewportSize;
};

}}}

#endif
