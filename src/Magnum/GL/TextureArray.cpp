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

#include "TextureArray.h"

#ifndef MAGNUM_TARGET_WEBGL
#include <Corrade/Containers/StringView.h>
#endif

#ifndef MAGNUM_TARGET_GLES2
#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/Implementation/maxTextureSize.h"

#ifndef MAGNUM_TARGET_GLES
#include "Magnum/Image.h"
#include "Magnum/GL/BufferImage.h"
#endif

#ifndef MAGNUM_TARGET_WEBGL
#include "Magnum/GL/Texture.h"
#include "Magnum/GL/CubeMapTexture.h"
#include "Magnum/GL/CubeMapTextureArray.h"
#endif

namespace Magnum { namespace GL {

namespace {
    template<UnsignedInt> struct VectorOrScalar;
    template<> struct VectorOrScalar<1> { using Type = Int; };
    template<> struct VectorOrScalar<2> { using Type = Vector2i; };
}

template<UnsignedInt dimensions> VectorTypeFor<dimensions+1, Int> TextureArray<dimensions>::maxSize() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_array>())
        return {};
    #endif

    return {typename VectorOrScalar<dimensions>::Type{Implementation::maxTextureSideSize()},
            Implementation::maxTextureArrayLayers()};
}

#ifndef MAGNUM_TARGET_WEBGL
template<UnsignedInt dimensions> TextureArray<dimensions> TextureArray<dimensions>::view(TextureArray<dimensions>& original, const TextureFormat internalFormat, const Int levelOffset, const Int levelCount, const Int layerOffset, const Int layerCount) {
    /* glTextureView() doesn't work with glCreateTextures() as it needs an
       object without a name bound, so have to construct manually. The object
       is marked as Created as glTextureView() binds the name. */
    GLuint id;
    glGenTextures(1, &id);
    TextureArray<dimensions> out{id, ObjectFlag::Created|ObjectFlag::DeleteOnDestruction};
    out.viewInternal(original, internalFormat, levelOffset, levelCount, layerOffset, layerCount);
    return out;
}

template<UnsignedInt dimensions> TextureArray<dimensions> TextureArray<dimensions>::view(Texture<dimensions>& original, const TextureFormat internalFormat, const Int levelOffset, const Int levelCount) {
    /* glTextureView() doesn't work with glCreateTextures() as it needs an
       object without a name bound, so have to construct manually. The object
       is marked as Created as glTextureView() binds the name. */
    GLuint id;
    glGenTextures(1, &id);
    TextureArray<dimensions> out{id, ObjectFlag::Created|ObjectFlag::DeleteOnDestruction};
    out.viewInternal(original, internalFormat, levelOffset, levelCount, 0, 1);
    return out;
}

/* On Windows (MSVC, clang-cl and MinGw) these need an explicit export
   otherwise the symbol doesn't get exported. */
template<> template<> MAGNUM_GL_EXPORT Texture2DArray Texture2DArray::view(CubeMapTexture& original, const TextureFormat internalFormat, const Int levelOffset, const Int levelCount, const Int layerOffset, const Int layerCount) {
    /* glTextureView() doesn't work with glCreateTextures() as it needs an
       object without a name bound, so have to construct manually. The object
       is marked as Created as glTextureView() binds the name. */
    GLuint id;
    glGenTextures(1, &id);
    Texture2DArray out{id, ObjectFlag::Created|ObjectFlag::DeleteOnDestruction};
    out.viewInternal(original, internalFormat, levelOffset, levelCount, layerOffset, layerCount);
    return out;
}

template<> template<> MAGNUM_GL_EXPORT Texture2DArray Texture2DArray::view(CubeMapTextureArray& original, const TextureFormat internalFormat, const Int levelOffset, const Int levelCount, const Int layerOffset, const Int layerCount) {
    /* glTextureView() doesn't work with glCreateTextures() as it needs an
       object without a name bound, so have to construct manually. The object
       is marked as Created as glTextureView() binds the name. */
    GLuint id;
    glGenTextures(1, &id);
    Texture2DArray out{id, ObjectFlag::Created|ObjectFlag::DeleteOnDestruction};
    out.viewInternal(original, internalFormat, levelOffset, levelCount, layerOffset, layerCount);
    return out;
}
#endif

#ifndef MAGNUM_TARGET_GLES
template<UnsignedInt dimensions> Image<dimensions+1> TextureArray<dimensions>::image(const Int level, Image<dimensions+1>&& image) {
    this->image(level, image);
    return Utility::move(image);
}

template<UnsignedInt dimensions> BufferImage<dimensions+1> TextureArray<dimensions>::image(const Int level, BufferImage<dimensions+1>&& image, const BufferUsage usage) {
    this->image(level, image, usage);
    return Utility::move(image);
}

template<UnsignedInt dimensions> CompressedImage<dimensions+1> TextureArray<dimensions>::compressedImage(const Int level, CompressedImage<dimensions+1>&& image) {
    compressedImage(level, image);
    return Utility::move(image);
}

template<UnsignedInt dimensions> CompressedBufferImage<dimensions+1> TextureArray<dimensions>::compressedImage(const Int level, CompressedBufferImage<dimensions+1>&& image, const BufferUsage usage) {
    compressedImage(level, image, usage);
    return Utility::move(image);
}

template<UnsignedInt dimensions> Image<dimensions+1> TextureArray<dimensions>::subImage(const Int level, const RangeTypeFor<dimensions+1, Int>& range, Image<dimensions+1>&& image) {
    this->subImage(level, range, image);
    return Utility::move(image);
}

template<UnsignedInt dimensions> BufferImage<dimensions+1> TextureArray<dimensions>::subImage(const Int level, const RangeTypeFor<dimensions+1, Int>& range, BufferImage<dimensions+1>&& image, const BufferUsage usage) {
    this->subImage(level, range, image, usage);
    return Utility::move(image);
}

template<UnsignedInt dimensions> CompressedImage<dimensions+1> TextureArray<dimensions>::compressedSubImage(const Int level, const RangeTypeFor<dimensions+1, Int>& range, CompressedImage<dimensions+1>&& image) {
    compressedSubImage(level, range, image);
    return Utility::move(image);
}

template<UnsignedInt dimensions> CompressedBufferImage<dimensions+1> TextureArray<dimensions>::compressedSubImage(const Int level, const RangeTypeFor<dimensions+1, Int>& range, CompressedBufferImage<dimensions+1>&& image, const BufferUsage usage) {
    compressedSubImage(level, range, image, usage);
    return Utility::move(image);
}
#endif

#ifndef MAGNUM_TARGET_WEBGL
template<UnsignedInt dimensions> TextureArray<dimensions>& TextureArray<dimensions>::setLabel(Containers::StringView label) {
    AbstractTexture::setLabel(label);
    return *this;
}
#endif

#ifndef MAGNUM_TARGET_GLES
template class
    /* GCC needs the export macro on the class definition (and here it warns
       that the type is already defined so the export is ignored), while Clang
       and MSVC need it here (and ignore it on the declaration) */
    #if defined(CORRADE_TARGET_CLANG) || defined(CORRADE_TARGET_MSVC)
    MAGNUM_GL_EXPORT
    #endif
    TextureArray<1>;
#endif
template class
    #if defined(CORRADE_TARGET_CLANG) || defined(CORRADE_TARGET_MSVC)
    MAGNUM_GL_EXPORT
    #endif
    TextureArray<2>;

}}
#endif
