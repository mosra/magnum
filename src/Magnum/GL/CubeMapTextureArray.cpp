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

#include "CubeMapTextureArray.h"

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
#include "Magnum/Image.h"
#include "Magnum/GL/BufferImage.h"
#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/Implementation/maxTextureSize.h"

namespace Magnum { namespace GL {

Vector3i CubeMapTextureArray::maxSize() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_cube_map_array>())
        return {};
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_cube_map_array>())
        return {};
    #endif

    return Vector3i{Vector2i{Implementation::maxCubeMapTextureSideSize()},
                             Implementation::maxTextureArrayLayers()};
}

#ifndef MAGNUM_TARGET_GLES
Image3D CubeMapTextureArray::image(const Int level, Image3D&& image) {
    this->image(level, image);
    return std::move(image);
}

BufferImage3D CubeMapTextureArray::image(const Int level, BufferImage3D&& image, const BufferUsage usage) {
    this->image(level, image, usage);
    return std::move(image);
}

CompressedImage3D CubeMapTextureArray::compressedImage(const Int level, CompressedImage3D&& image) {
    compressedImage(level, image);
    return std::move(image);
}

CompressedBufferImage3D CubeMapTextureArray::compressedImage(const Int level, CompressedBufferImage3D&& image, const BufferUsage usage) {
    compressedImage(level, image, usage);
    return std::move(image);
}

Image3D CubeMapTextureArray::subImage(const Int level, const Range3Di& range, Image3D&& image) {
    this->subImage(level, range, image);
    return std::move(image);
}

BufferImage3D CubeMapTextureArray::subImage(const Int level, const Range3Di& range, BufferImage3D&& image, const BufferUsage usage) {
    this->subImage(level, range, image, usage);
    return std::move(image);
}

CompressedImage3D CubeMapTextureArray::compressedSubImage(const Int level, const Range3Di& range, CompressedImage3D&& image) {
    compressedSubImage(level, range, image);
    return std::move(image);
}

CompressedBufferImage3D CubeMapTextureArray::compressedSubImage(const Int level, const Range3Di& range, CompressedBufferImage3D&& image, const BufferUsage usage) {
    compressedSubImage(level, range, image, usage);
    return std::move(image);
}
#endif

}}
#endif
