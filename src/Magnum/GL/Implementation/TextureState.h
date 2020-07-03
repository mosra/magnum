#ifndef Magnum_GL_Implementation_TextureState_h
#define Magnum_GL_Implementation_TextureState_h
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
#include <Corrade/Containers/Array.h>
#include <Corrade/Utility/StlForwardTuple.h>

#include "Magnum/Magnum.h"
#include "Magnum/GL/GL.h"
#include "Magnum/GL/OpenGL.h"

#ifdef _MSC_VER
#include "Magnum/GL/AbstractTexture.h"

#ifndef MAGNUM_TARGET_GLES2
/* Otherwise the member function pointers will have different size based on
   whether the header was included or not. CAUSES SERIOUS MEMORY CORRUPTION AND
   IS NOT CAUGHT BY ANY WARNING WHATSOEVER! AARGH! */
#include "Magnum/GL/BufferTexture.h"
#include "Magnum/GL/CubeMapTexture.h"
#endif
#endif

#if defined(CORRADE_TARGET_APPLE) && !defined(MAGNUM_TARGET_GLES)
#include "Magnum/Math/BoolVector.h"
#endif

namespace Magnum { namespace GL { namespace Implementation {

struct TextureState {
    explicit TextureState(Context& context, std::vector<std::string>& extensions);
    ~TextureState();

    void reset();

    Int(*compressedBlockDataSizeImplementation)(GLenum, TextureFormat);
    void(*unbindImplementation)(GLint);
    void(*bindMultiImplementation)(GLint, Containers::ArrayView<AbstractTexture* const>);
    void(AbstractTexture::*createImplementation)();
    void(AbstractTexture::*bindImplementation)(GLint);
    void(AbstractTexture::*bindInternalImplementation)(GLint);
    void(AbstractTexture::*parameteriImplementation)(GLenum, GLint);
    void(AbstractTexture::*parameterfImplementation)(GLenum, GLfloat);
    #ifndef MAGNUM_TARGET_GLES2
    void(AbstractTexture::*parameterivImplementation)(GLenum, const GLint*);
    #endif
    void(AbstractTexture::*parameterfvImplementation)(GLenum, const GLfloat*);
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    void(AbstractTexture::*parameterIuivImplementation)(GLenum, const GLuint*);
    void(AbstractTexture::*parameterIivImplementation)(GLenum, const GLint*);
    #endif
    void(AbstractTexture::*setMaxAnisotropyImplementation)(GLfloat);
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    void(AbstractTexture::*getLevelParameterivImplementation)(GLint, GLenum, GLint*);
    #endif
    void(AbstractTexture::*mipmapImplementation)();
    #ifndef MAGNUM_TARGET_GLES
    void(AbstractTexture::*storage1DImplementation)(GLsizei, TextureFormat, const Math::Vector<1, GLsizei>&);
    #endif
    void(AbstractTexture::*storage2DImplementation)(GLsizei, TextureFormat, const Vector2i&);
    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    void(AbstractTexture::*storage3DImplementation)(GLsizei, TextureFormat, const Vector3i&);
    #endif
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    void(AbstractTexture::*storage2DMultisampleImplementation)(GLsizei, TextureFormat, const Vector2i&, GLboolean);
    void(AbstractTexture::*storage3DMultisampleImplementation)(GLsizei, TextureFormat, const Vector3i&, GLboolean);
    #endif
    #ifndef MAGNUM_TARGET_GLES
    void(AbstractTexture::*getImageImplementation)(GLint, PixelFormat, PixelType, std::size_t, GLvoid*);
    void(AbstractTexture::*getCompressedImageImplementation)(GLint, std::size_t, GLvoid*);
    #endif
    #ifndef MAGNUM_TARGET_GLES
    void(AbstractTexture::*subImage1DImplementation)(GLint, const Math::Vector<1, GLint>&, const Math::Vector<1, GLsizei>&, PixelFormat, PixelType, const GLvoid*);
    void(AbstractTexture::*compressedSubImage1DImplementation)(GLint, const Math::Vector<1, GLint>&, const Math::Vector<1, GLsizei>&, CompressedPixelFormat, const GLvoid*, GLsizei);
    #endif
    void (AbstractTexture::*image2DImplementation)(GLenum, GLint, TextureFormat, const Vector2i&, PixelFormat, PixelType, const GLvoid*, const PixelStorage&);
    void(AbstractTexture::*subImage2DImplementation)(GLint, const Vector2i&, const Vector2i&, PixelFormat, PixelType, const GLvoid*, const PixelStorage&);
    void(AbstractTexture::*compressedSubImage2DImplementation)(GLint, const Vector2i&, const Vector2i&, CompressedPixelFormat, const GLvoid*, GLsizei);
    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    void (AbstractTexture::*image3DImplementation)(GLint, TextureFormat, const Vector3i&, PixelFormat, PixelType, const GLvoid*, const PixelStorage&);
    void(AbstractTexture::*subImage3DImplementation)(GLint, const Vector3i&, const Vector3i&, PixelFormat, PixelType, const GLvoid*, const PixelStorage&);
    void(AbstractTexture::*compressedSubImage3DImplementation)(GLint, const Vector3i&, const Vector3i&, CompressedPixelFormat, const GLvoid*, GLsizei);
    #endif
    void(AbstractTexture::*invalidateImageImplementation)(GLint);
    void(AbstractTexture::*invalidateSubImageImplementation)(GLint, const Vector3i&, const Vector3i&);

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    void(BufferTexture::*setBufferImplementation)(BufferTextureFormat, Buffer*);
    void(BufferTexture::*setBufferRangeImplementation)(BufferTextureFormat, Buffer&, GLintptr, GLsizeiptr);
    #endif

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    void(CubeMapTexture::*getCubeLevelParameterivImplementation)(GLint, GLenum, GLint*);
    #endif
    #ifndef MAGNUM_TARGET_GLES
    GLint(CubeMapTexture::*getCubeLevelCompressedImageSizeImplementation)(GLint);
    void(CubeMapTexture::*getCubeImageImplementation)(CubeMapCoordinate, GLint, const Vector2i&, PixelFormat, PixelType, std::size_t, GLvoid*);
    void(CubeMapTexture::*getFullCubeImageImplementation)(GLint, const Vector3i&, PixelFormat, PixelType, std::size_t, GLvoid*, const PixelStorage&);
    void(CubeMapTexture::*getFullCompressedCubeImageImplementation)(GLint, const Vector2i&, std::size_t, std::size_t, GLvoid*);
    void(CubeMapTexture::*getCompressedCubeImageImplementation)(CubeMapCoordinate, GLint, const Vector2i&, std::size_t, GLvoid*);
    void(CubeMapTexture::*cubeSubImage3DImplementation)(GLint, const Vector3i&, const Vector3i&, PixelFormat, PixelType, const GLvoid*, const PixelStorage&);
    #endif
    void(CubeMapTexture::*cubeSubImageImplementation)(CubeMapCoordinate, GLint, const Vector2i&, const Vector2i&, PixelFormat, PixelType, const GLvoid*);
    void(CubeMapTexture::*cubeCompressedSubImageImplementation)(CubeMapCoordinate, GLint, const Vector2i&, const Vector2i&, CompressedPixelFormat, const GLvoid*, GLsizei);

    GLint maxSize,
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        max3DSize,
        #endif
        maxCubeMapSize;
    #ifndef MAGNUM_TARGET_GLES2
    GLint maxArrayLayers;
    #endif
    #ifndef MAGNUM_TARGET_GLES
    GLint maxRectangleSize;
    #endif
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    GLint maxBufferSize;
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
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    GLint bufferOffsetAlignment;
    #endif

    Containers::Array<std::pair<GLenum, GLuint>> bindings;
    #if defined(CORRADE_TARGET_APPLE) && !defined(MAGNUM_TARGET_GLES)
    Math::BoolVector<80> bufferTextureBound;
    #endif
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    /* Texture object ID, level, layered, layer, access */
    Containers::Array<std::tuple<GLuint, GLint, GLboolean, GLint, GLenum>> imageBindings;
    #endif
};

}}}

#endif
