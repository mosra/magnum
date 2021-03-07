/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021 Vladimír Vondruš <mosra@centrum.cz>

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

#define _MAGNUM_NO_DEPRECATED_VK_ENUMS

#include "Enums.h"

#include "Magnum/Vk/MeshLayout.h"
#include "Magnum/Vk/Mesh.h"
#include "Magnum/Vk/PixelFormat.h"
#include "Magnum/Vk/SamplerCreateInfo.h"
#include "Magnum/Vk/VertexFormat.h"

namespace Magnum { namespace Vk {

bool hasVkPrimitiveTopology(const Magnum::MeshPrimitive primitive) {
    return hasMeshPrimitive(primitive);
}

VkPrimitiveTopology vkPrimitiveTopology(const Magnum::MeshPrimitive primitive) {
    return VkPrimitiveTopology(meshPrimitive(primitive));
}

bool hasVkIndexType(Magnum::MeshIndexType) {
    return true;
}

VkIndexType vkIndexType(const Magnum::MeshIndexType type) {
    return VkIndexType(meshIndexType(type));
}

bool hasVkFormat(const Magnum::VertexFormat format) {
    return hasVertexFormat(format);
}

bool hasVkFormat(const Magnum::PixelFormat format) {
    return hasPixelFormat(format);
}

bool hasVkFormat(const Magnum::CompressedPixelFormat format) {
    return hasPixelFormat(format);
}

VkFormat vkFormat(const Magnum::VertexFormat format) {
    return VkFormat(vertexFormat(format));
}

VkFormat vkFormat(const Magnum::PixelFormat format) {
    return VkFormat(pixelFormat(format));
}

VkFormat vkFormat(const Magnum::CompressedPixelFormat format) {
    return VkFormat(pixelFormat(format));
}

VkFilter vkFilter(const Magnum::SamplerFilter filter) {
    return VkFilter(samplerFilter(filter));
}

VkSamplerMipmapMode vkSamplerMipmapMode(const Magnum::SamplerMipmap mipmap) {
    return VkSamplerMipmapMode(samplerMipmap(mipmap));
}

bool hasVkSamplerAddressMode(Magnum::SamplerWrapping) {
    return true;
}

VkSamplerAddressMode vkSamplerAddressMode(const Magnum::SamplerWrapping wrapping) {
    return VkSamplerAddressMode(samplerWrapping(wrapping));
}

}}
