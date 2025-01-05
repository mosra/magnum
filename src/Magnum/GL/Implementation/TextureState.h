#ifndef Magnum_GL_Implementation_TextureState_h
#define Magnum_GL_Implementation_TextureState_h
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

#include <Corrade/Containers/Pair.h>
#include <Corrade/Containers/ArrayView.h>

#include "Magnum/Magnum.h"
#include "Magnum/GL/GL.h"
#include "Magnum/GL/OpenGL.h"

#if defined(CORRADE_TARGET_APPLE) && !defined(MAGNUM_TARGET_GLES)
#include "Magnum/Math/BitVector.h"
#endif

namespace Magnum { namespace GL { namespace Implementation {

struct TextureState {
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    struct ImageBinding {
        GLuint id;
        GLint level;
        GLboolean layered;
        GLint layer;
        GLenum access;

        /* Used inside AbstractTexture to check if the state changed */
        bool operator==(const ImageBinding& other) {
            return other.id == id &&
                   other.level == level &&
                   other.layered == layered &&
                   other.layer == layer &&
                   other.access == access;
        }
        bool operator!=(const ImageBinding& other) {
            return !operator==(other);
        }
    };
    #endif

    explicit TextureState(Context& context,
        Containers::ArrayView<Containers::Pair<GLenum, GLuint>> bindings,
        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        Containers::ArrayView<ImageBinding> imageBindings,
        #endif
        Containers::StaticArrayView<Implementation::ExtensionCount, const char*> extensions);

    void reset();

    Int(*compressedBlockDataSizeImplementation)(GLenum, TextureFormat);
    void(*unbindImplementation)(GLint);
    void(*bindMultiImplementation)(GLint, Containers::ArrayView<AbstractTexture* const>);
    void(*createImplementation)(AbstractTexture&);
    void(*bindImplementation)(AbstractTexture&, GLint);
    void(*bindInternalImplementation)(AbstractTexture&, GLint);
    void(*parameteriImplementation)(AbstractTexture&, GLenum, GLint);
    void(*parameterfImplementation)(AbstractTexture&, GLenum, GLfloat);
    #ifndef MAGNUM_TARGET_GLES2
    void(*parameterivImplementation)(AbstractTexture&, GLenum, const GLint*);
    #endif
    void(*parameterfvImplementation)(AbstractTexture&, GLenum, const GLfloat*);
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    void(*parameterIuivImplementation)(AbstractTexture&, GLenum, const GLuint*);
    void(*parameterIivImplementation)(AbstractTexture&, GLenum, const GLint*);
    #endif
    void(*setMaxAnisotropyImplementation)(AbstractTexture&, GLfloat);
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    void(*getLevelParameterivImplementation)(AbstractTexture&, GLint, GLenum, GLint*);
    #endif
    void(*mipmapImplementation)(AbstractTexture&);
    #ifndef MAGNUM_TARGET_GLES
    void(*storage1DImplementation)(AbstractTexture&, GLsizei, TextureFormat, const Math::Vector<1, GLsizei>&);
    #endif
    void(*storage2DImplementation)(AbstractTexture&, GLsizei, TextureFormat, const Vector2i&);
    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    void(*storage3DImplementation)(AbstractTexture&, GLsizei, TextureFormat, const Vector3i&);
    #endif
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    void(*storage2DMultisampleImplementation)(AbstractTexture&, GLsizei, TextureFormat, const Vector2i&, GLboolean);
    void(*storage3DMultisampleImplementation)(AbstractTexture&, GLsizei, TextureFormat, const Vector3i&, GLboolean);
    #endif
    #ifndef MAGNUM_TARGET_GLES
    void(*getImageImplementation)(AbstractTexture&, GLint, PixelFormat, PixelType, std::size_t, GLvoid*);
    void(*getCompressedImageImplementation)(AbstractTexture&, GLint, std::size_t, GLvoid*);
    #endif
    #ifndef MAGNUM_TARGET_GLES
    void(*subImage1DImplementation)(AbstractTexture&, GLint, const Math::Vector<1, GLint>&, const Math::Vector<1, GLsizei>&, PixelFormat, PixelType, const GLvoid*);
    void(*compressedSubImage1DImplementation)(AbstractTexture&, GLint, const Math::Vector<1, GLint>&, const Math::Vector<1, GLsizei>&, CompressedPixelFormat, const GLvoid*, GLsizei);
    #endif
    void(*image2DImplementation)(AbstractTexture&, GLenum, GLint, TextureFormat, const Vector2i&, PixelFormat, PixelType, const GLvoid*, const PixelStorage&);
    void(*subImage2DImplementation)(AbstractTexture&, GLint, const Vector2i&, const Vector2i&, PixelFormat, PixelType, const GLvoid*, const PixelStorage&);
    void(*compressedSubImage2DImplementation)(AbstractTexture&, GLint, const Vector2i&, const Vector2i&, CompressedPixelFormat, const GLvoid*, GLsizei);
    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    void(*image3DImplementation)(AbstractTexture&, GLint, TextureFormat, const Vector3i&, PixelFormat, PixelType, const GLvoid*, const PixelStorage&);
    void(*subImage3DImplementation)(AbstractTexture&, GLint, const Vector3i&, const Vector3i&, PixelFormat, PixelType, const GLvoid*, const PixelStorage&);
    void(*compressedSubImage3DImplementation)(AbstractTexture&, GLint, const Vector3i&, const Vector3i&, CompressedPixelFormat, const GLvoid*, GLsizei);
    #endif
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    void(APIENTRY *viewImplementation)(GLuint, GLenum, GLuint, GLenum, GLuint, GLuint, GLuint, GLuint);
    #endif
    void(*invalidateImageImplementation)(AbstractTexture&, GLint);
    void(*invalidateSubImageImplementation)(AbstractTexture&, GLint, const Vector3i&, const Vector3i&);

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    void(*setBufferImplementation)(BufferTexture&, BufferTextureFormat, Buffer*);
    void(*setBufferRangeImplementation)(BufferTexture&, BufferTextureFormat, Buffer&, GLintptr, GLsizeiptr);
    #endif

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    void(*getCubeLevelParameterivImplementation)(CubeMapTexture&, GLint, GLenum, GLint*);
    #endif
    #ifndef MAGNUM_TARGET_GLES
    GLint(*getCubeLevelCompressedImageSizeImplementation)(CubeMapTexture&, GLint);
    void(*getCubeImageImplementation)(CubeMapTexture&, CubeMapCoordinate, GLint, const Vector2i&, PixelFormat, PixelType, std::size_t, GLvoid*);
    void(*getCubeImage3DImplementation)(CubeMapTexture&, GLint, const Vector3i&, PixelFormat, PixelType, std::size_t, GLvoid*, const PixelStorage&);
    void(*getCompressedCubeImage3DImplementation)(CubeMapTexture&, GLint, const Vector2i&, std::size_t, std::size_t, GLvoid*);
    void(*getCompressedCubeImageImplementation)(CubeMapTexture&, CubeMapCoordinate, GLint, const Vector2i&, std::size_t, GLvoid*);
    #endif
    void(*cubeSubImage3DImplementation)(CubeMapTexture&, GLint, const Vector3i&, const Vector3i&, PixelFormat, PixelType, const GLvoid*, const PixelStorage&);
    void(*cubeSubImageImplementation)(CubeMapTexture&, CubeMapCoordinate, GLint, const Vector2i&, const Vector2i&, PixelFormat, PixelType, const GLvoid*);
    void(*cubeCompressedSubImageImplementation)(CubeMapTexture&, CubeMapCoordinate, GLint, const Vector2i&, const Vector2i&, CompressedPixelFormat, const GLvoid*, GLsizei);

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

    /* These arrays get allocated in State in a single allocation and views to
       them are passed here via the constructor. */

    /* Texture type, texture object ID. While not true, for simplicity this
       assumes that each slot can have just one ID bound, not one ID per
       texture type. */
    Containers::ArrayView<Containers::Pair<GLenum, GLuint>> bindings;
    #if defined(CORRADE_TARGET_APPLE) && !defined(MAGNUM_TARGET_GLES)
    Math::BitVector<80> bufferTextureBound;
    #endif
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    /* Texture object ID, level, layered, layer, access */
    Containers::ArrayView<ImageBinding> imageBindings;
    #endif
};

}}}

#endif
