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

#include "TextureArray.h"

#ifndef MAGNUM_TARGET_GLES2
#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/Implementation/maxTextureSize.h"

#ifndef MAGNUM_TARGET_GLES
#include "Magnum/Image.h"
#include "Magnum/GL/BufferImage.h"
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

#ifndef MAGNUM_TARGET_GLES
template<UnsignedInt dimensions> Image<dimensions+1> TextureArray<dimensions>::image(const Int level, Image<dimensions+1>&& image) {
    this->image(level, image);
    return std::move(image);
}

template<UnsignedInt dimensions> BufferImage<dimensions+1> TextureArray<dimensions>::image(const Int level, BufferImage<dimensions+1>&& image, const BufferUsage usage) {
    this->image(level, image, usage);
    return std::move(image);
}

template<UnsignedInt dimensions> CompressedImage<dimensions+1> TextureArray<dimensions>::compressedImage(const Int level, CompressedImage<dimensions+1>&& image) {
    compressedImage(level, image);
    return std::move(image);
}

template<UnsignedInt dimensions> CompressedBufferImage<dimensions+1> TextureArray<dimensions>::compressedImage(const Int level, CompressedBufferImage<dimensions+1>&& image, const BufferUsage usage) {
    compressedImage(level, image, usage);
    return std::move(image);
}

template<UnsignedInt dimensions> Image<dimensions+1> TextureArray<dimensions>::subImage(const Int level, const RangeTypeFor<dimensions+1, Int>& range, Image<dimensions+1>&& image) {
    this->subImage(level, range, image);
    return std::move(image);
}

template<UnsignedInt dimensions> BufferImage<dimensions+1> TextureArray<dimensions>::subImage(const Int level, const RangeTypeFor<dimensions+1, Int>& range, BufferImage<dimensions+1>&& image, const BufferUsage usage) {
    this->subImage(level, range, image, usage);
    return std::move(image);
}

template<UnsignedInt dimensions> CompressedImage<dimensions+1> TextureArray<dimensions>::compressedSubImage(const Int level, const RangeTypeFor<dimensions+1, Int>& range, CompressedImage<dimensions+1>&& image) {
    compressedSubImage(level, range, image);
    return std::move(image);
}

template<UnsignedInt dimensions> CompressedBufferImage<dimensions+1> TextureArray<dimensions>::compressedSubImage(const Int level, const RangeTypeFor<dimensions+1, Int>& range, CompressedBufferImage<dimensions+1>&& image, const BufferUsage usage) {
    compressedSubImage(level, range, image, usage);
    return std::move(image);
}
#endif

#ifndef MAGNUM_TARGET_GLES
template class MAGNUM_GL_EXPORT TextureArray<1>;
#endif
template class MAGNUM_GL_EXPORT TextureArray<2>;

}}
#endif
