#ifndef Magnum_GL_Implementation_FramebufferState_h
#define Magnum_GL_Implementation_FramebufferState_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
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

#include "Magnum/GL/Framebuffer.h"

namespace Magnum { namespace GL { namespace Implementation {

struct FramebufferState {
    constexpr static const Range2Di DisengagedViewport{{}, {-1, -1}};

    explicit FramebufferState(Context& context, Containers::StaticArrayView<Implementation::ExtensionCount, const char*> extensions);

    void reset();

    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    void(*blitImplementation)(AbstractFramebuffer&, AbstractFramebuffer&, const Range2Di&, const Range2Di&, FramebufferBlitMask, FramebufferBlitFilter);
    #endif
    GLenum(*checkStatusImplementation)(AbstractFramebuffer&, FramebufferTarget);

    #ifndef MAGNUM_TARGET_GLES2
    void(*clearIImplementation)(AbstractFramebuffer&, GLenum, GLint, const GLint*);
    void(*clearUIImplementation)(AbstractFramebuffer&, GLenum, GLint, const GLuint*);
    void(*clearFImplementation)(AbstractFramebuffer&, GLenum, GLint, const GLfloat*);
    void(*clearFIImplementation)(AbstractFramebuffer&, GLenum, GLfloat, GLint);
    #endif

    void(*drawBuffersImplementation)(AbstractFramebuffer&, GLsizei, const GLenum*);
    #ifndef MAGNUM_TARGET_GLES
    void(*drawBufferImplementation)(AbstractFramebuffer&, GLenum);
    #endif
    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    void(*readBufferImplementation)(AbstractFramebuffer&, GLenum);
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
    void(*invalidateImplementation)(AbstractFramebuffer&, GLsizei, const GLenum*);
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    void(*invalidateSubImplementation)(AbstractFramebuffer&, GLsizei, const GLenum*, const Range2Di&);
    #endif
    #if defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    void(*bindImplementation)(AbstractFramebuffer&, FramebufferTarget);
    FramebufferTarget(*bindInternalImplementation)(AbstractFramebuffer&);
    #endif

    GLenum(*implementationColorReadFormatTypeImplementation)(AbstractFramebuffer&, GLenum what);

    void(*createImplementation)(Framebuffer&);
    void(*renderbufferImplementation)(Framebuffer&, Framebuffer::BufferAttachment, GLuint);
    #ifndef MAGNUM_TARGET_GLES
    void(*texture1DImplementation)(Framebuffer&, Framebuffer::BufferAttachment, GLuint, GLint);
    #endif
    void(*texture2DImplementation)(Framebuffer&, Framebuffer::BufferAttachment, GLenum, GLuint, GLint);
    void(*textureCubeMapImplementation)(Framebuffer&, Framebuffer::BufferAttachment, GLenum, GLuint, GLint);
    #if !defined(MAGNUM_TARGET_WEBGL) && !defined(MAGNUM_TARGET_GLES2)
    void(*textureImplementation)(Framebuffer&, Framebuffer::BufferAttachment, GLuint, GLint);
    void(*layeredTextureCubeMapArrayImplementation)(Framebuffer&, Framebuffer::BufferAttachment, GLuint, GLint);
    #endif
    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    void(*textureLayerImplementation)(Framebuffer&, Framebuffer::BufferAttachment, GLuint, GLint, GLint);
    #endif
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    void(*parameterImplementation)(Framebuffer&, GLenum, GLint);
    #endif

    void(*createRenderbufferImplementation)(Renderbuffer&);
    void(*renderbufferStorageImplementation)(Renderbuffer&, RenderbufferFormat, const Vector2i&);
    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    void(*renderbufferStorageMultisampleImplementation)(Renderbuffer&, GLsizei, RenderbufferFormat, const Vector2i&);
    #endif

    /* Cannot be a direct pointer to a GL function because the non-robust
       version doesn't take the size argument */
    void(*readImplementation)(const Range2Di&, PixelFormat, PixelType, std::size_t, GLvoid*);

    GLuint readBinding, drawBinding, renderbufferBinding;
    GLint maxDrawBuffers, maxColorAttachments, maxRenderbufferSize;
    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    GLint maxSamples;
    #endif
    #ifndef MAGNUM_TARGET_GLES
    GLint maxDualSourceDrawBuffers;
    #endif
    Range2Di defaultViewport, viewport;
    Vector2i maxViewportSize;
};

}}}

#endif
