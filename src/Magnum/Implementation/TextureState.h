#ifndef Magnum_Implementation_TextureState_h
#define Magnum_Implementation_TextureState_h
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

#include <string>
#include <vector>

#include "Magnum/CubeMapTexture.h"

#ifdef CORRADE_GCC45_COMPATIBILITY
#ifndef MAGNUM_TARGET_GLES
#include "Magnum/BufferTexture.h"
#endif
#include "Magnum/ColorFormat.h"
#include "Magnum/TextureFormat.h"
#endif

/* Otherwise we get a pretty nice memory corruption only with a warning about
   architecture-dependent alignment of `setBufferImplementation` variable */
#ifdef CORRADE_MSVC2013_COMPATIBILITY
#ifndef MAGNUM_TARGET_GLES
#include "Magnum/BufferTexture.h"
#else
#include "Magnum/AbstractTexture.h"
#endif
#endif

namespace Magnum { namespace Implementation {

struct TextureState {
    explicit TextureState(Context& context, std::vector<std::string>& extensions);
    ~TextureState();

    void reset();

    void(*unbindImplementation)(GLint);
    void(*bindMultiImplementation)(GLint, Containers::ArrayReference<AbstractTexture* const>);
    void(AbstractTexture::*createImplementation)();
    void(AbstractTexture::*bindImplementation)(GLint);
    void(AbstractTexture::*parameteriImplementation)(GLenum, GLint);
    void(AbstractTexture::*parameterfImplementation)(GLenum, GLfloat);
    #ifndef MAGNUM_TARGET_GLES2
    void(AbstractTexture::*parameterivImplementation)(GLenum, const GLint*);
    #endif
    void(AbstractTexture::*parameterfvImplementation)(GLenum, const GLfloat*);
    #ifndef MAGNUM_TARGET_GLES
    void(AbstractTexture::*parameterIuivImplementation)(GLenum, const GLuint*);
    void(AbstractTexture::*parameterIivImplementation)(GLenum, const GLint*);
    #endif
    void(AbstractTexture::*setMaxAnisotropyImplementation)(GLfloat);
    #ifndef MAGNUM_TARGET_GLES2
    void(AbstractTexture::*getLevelParameterivImplementation)(GLint, GLenum, GLint*);
    #endif
    void(AbstractTexture::*mipmapImplementation)();
    #ifndef MAGNUM_TARGET_GLES
    void(AbstractTexture::*storage1DImplementation)(GLsizei, TextureFormat, const Math::Vector<1, GLsizei>&);
    #endif
    void(AbstractTexture::*storage2DImplementation)(GLsizei, TextureFormat, const Vector2i&);
    void(AbstractTexture::*storage3DImplementation)(GLsizei, TextureFormat, const Vector3i&);
    #ifndef MAGNUM_TARGET_GLES2
    void(AbstractTexture::*storage2DMultisampleImplementation)(GLsizei, TextureFormat, const Vector2i&, GLboolean);
    #endif
    #ifndef MAGNUM_TARGET_GLES
    void(AbstractTexture::*storage3DMultisampleImplementation)(GLsizei, TextureFormat, const Vector3i&, GLboolean);
    void(AbstractTexture::*getImageImplementation)(GLint, ColorFormat, ColorType, std::size_t, GLvoid*);
    #endif
    #ifndef MAGNUM_TARGET_GLES
    void(AbstractTexture::*subImage1DImplementation)(GLint, const Math::Vector<1, GLint>&, const Math::Vector<1, GLsizei>&, ColorFormat, ColorType, const GLvoid*);
    #endif
    void(AbstractTexture::*subImage2DImplementation)(GLint, const Vector2i&, const Vector2i&, ColorFormat, ColorType, const GLvoid*);
    void(AbstractTexture::*subImage3DImplementation)(GLint, const Vector3i&, const Vector3i&, ColorFormat, ColorType, const GLvoid*);
    void(AbstractTexture::*invalidateImageImplementation)(GLint);
    void(AbstractTexture::*invalidateSubImageImplementation)(GLint, const Vector3i&, const Vector3i&);

    #ifndef MAGNUM_TARGET_GLES
    void(BufferTexture::*setBufferImplementation)(BufferTextureFormat, Buffer&);
    void(BufferTexture::*setBufferRangeImplementation)(BufferTextureFormat, Buffer&, GLintptr, GLsizeiptr);
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    Vector2i(CubeMapTexture::*getCubeImageSizeImplementation)(Int);
    #endif
    #ifndef MAGNUM_TARGET_GLES
    void(CubeMapTexture::*getCubeImageImplementation)(CubeMapTexture::Coordinate, GLint, const Vector2i&, ColorFormat, ColorType, std::size_t, GLvoid*);
    #endif
    void(CubeMapTexture::*cubeSubImageImplementation)(CubeMapTexture::Coordinate, GLint, const Vector2i&, const Vector2i&, ColorFormat, ColorType, const GLvoid*);

    GLint maxSize,
        max3DSize,
        maxCubeMapSize;
    #ifndef MAGNUM_TARGET_GLES2
    GLint maxArrayLayers;
    #endif
    #ifndef MAGNUM_TARGET_GLES
    GLint maxRectangleSize,
        maxBufferSize;
    #endif
    GLint maxTextureUnits;
    #ifndef MAGNUM_TARGET_GLES2
    GLfloat maxLodBias;
    #endif
    GLfloat maxMaxAnisotropy;
    GLint currentTextureUnit;
    #ifndef MAGNUM_TARGET_GLES2
    GLint maxColorSamples,
        maxDepthSamples,
        maxIntegerSamples;
    #endif
    #ifndef MAGNUM_TARGET_GLES
    GLint bufferOffsetAlignment;
    #endif

    std::vector<std::pair<GLenum, GLuint>> bindings;
};

}}

#endif
