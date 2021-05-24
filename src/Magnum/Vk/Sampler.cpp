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

#include "Sampler.h"
#include "SamplerCreateInfo.h"

#include <Corrade/Containers/ArrayView.h>

#include "Magnum/Sampler.h"
#include "Magnum/Math/Vector3.h"
#include "Magnum/Vk/Assert.h"
#include "Magnum/Vk/Device.h"
#include "Magnum/Vk/Result.h"

namespace Magnum { namespace Vk {

namespace {

constexpr SamplerFilter FilterMapping[]{
    SamplerFilter::Nearest,
    SamplerFilter::Linear
};

constexpr SamplerMipmap SamplerMipmapMapping[]{
    SamplerMipmap::Nearest, /* See samplerMipmap() for details */
    SamplerMipmap::Nearest,
    SamplerMipmap::Linear
};

constexpr SamplerWrapping SamplerWrappingMapping[]{
    SamplerWrapping::Repeat,
    SamplerWrapping::MirroredRepeat,
    SamplerWrapping::ClampToEdge,
    SamplerWrapping::ClampToBorder,
    SamplerWrapping::MirrorClampToEdge
};

}

SamplerFilter samplerFilter(const Magnum::SamplerFilter filter) {
    CORRADE_ASSERT(UnsignedInt(filter) < Containers::arraySize(FilterMapping),
        "Vk::samplerFilter(): invalid filter" << filter, {});
    return FilterMapping[UnsignedInt(filter)];
}

SamplerMipmap samplerMipmap(const Magnum::SamplerMipmap mipmap) {
    CORRADE_ASSERT(UnsignedInt(mipmap) < Containers::arraySize(SamplerMipmapMapping),
        "Vk::samplerMipmap(): invalid mode" << mipmap, {});
    return SamplerMipmapMapping[UnsignedInt(mipmap)];
}

SamplerWrapping samplerWrapping(const Magnum::SamplerWrapping wrapping) {
    CORRADE_ASSERT(UnsignedInt(wrapping) < Containers::arraySize(SamplerWrappingMapping),
        "Vk::samplerWrapping(): invalid wrapping" << wrapping, {});
    return SamplerWrappingMapping[UnsignedInt(wrapping)];
}

template<std::size_t dimensions> Math::Vector<dimensions, SamplerWrapping> samplerWrapping(const Math::Vector<dimensions, Magnum::SamplerWrapping>& wrapping) {
    Math::Vector<dimensions, SamplerWrapping> out{NoInit};
    for(std::size_t i = 0; i != dimensions; ++i)
        out[i] = samplerWrapping(wrapping[i]);
    return out;
}

/* Export needed by MSVC, for others it's enough to have on the declaration in
   the header */
template MAGNUM_VK_EXPORT Math::Vector<1, SamplerWrapping> samplerWrapping(const Math::Vector<1, Magnum::SamplerWrapping>&);
template MAGNUM_VK_EXPORT Math::Vector<2, SamplerWrapping> samplerWrapping(const Math::Vector<2, Magnum::SamplerWrapping>&);
template MAGNUM_VK_EXPORT Math::Vector<3, SamplerWrapping> samplerWrapping(const Math::Vector<3, Magnum::SamplerWrapping>&);

Debug& operator<<(Debug& debug, const SamplerFilter value) {
    debug << "Vk::SamplerFilter" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case Vk::SamplerFilter::value: return debug << "::" << Debug::nospace << #value;
        _c(Nearest)
        _c(Linear)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    /* Vulkan docs have the values in decimal, so not converting to hex */
    return debug << "(" << Debug::nospace << Int(value) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const SamplerMipmap value) {
    debug << "Vk::SamplerMipmap" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case Vk::SamplerMipmap::value: return debug << "::" << Debug::nospace << #value;
        _c(Nearest)
        _c(Linear)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    /* Vulkan docs have the values in decimal, so not converting to hex */
    return debug << "(" << Debug::nospace << Int(value) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const SamplerWrapping value) {
    debug << "Vk::SamplerWrapping" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case Vk::SamplerWrapping::value: return debug << "::" << Debug::nospace << #value;
        _c(Repeat)
        _c(MirroredRepeat)
        _c(ClampToEdge)
        _c(ClampToBorder)
        _c(MirrorClampToEdge)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    /* Vulkan docs have the values in decimal, so not converting to hex */
    return debug << "(" << Debug::nospace << Int(value) << Debug::nospace << ")";
}

SamplerCreateInfo::SamplerCreateInfo(const Flags flags): _info{} {
    _info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    _info.flags = VkDescriptorSetLayoutCreateFlags(flags);
    _info.minFilter = VkFilter(SamplerFilter::Nearest);
    _info.magFilter = VkFilter(SamplerFilter::Nearest);
    /* These are 0, which makes them the obvious candidates, however it's also
       helpful in case the texture coordinates are completely off -- with
       ClampToEdge (that I wanted to use at first) it would make debugging much
       harder as the output could be just a single color in worst cases */
    _info.addressModeU = VkSamplerAddressMode(SamplerWrapping::Repeat);
    _info.addressModeV = VkSamplerAddressMode(SamplerWrapping::Repeat);
    _info.addressModeW = VkSamplerAddressMode(SamplerWrapping::Repeat);
    _info.minLod = -1000.0f;
    _info.maxLod = 1000.0f;
}

SamplerCreateInfo::SamplerCreateInfo(NoInitT) noexcept {}

SamplerCreateInfo::SamplerCreateInfo(const VkSamplerCreateInfo& info):
    /* Can't use {} with GCC 4.8 here because it tries to initialize the first
       member instead of doing a copy */
    _info(info) {}

SamplerCreateInfo& SamplerCreateInfo::setMinificationFilter(const SamplerFilter filter, const SamplerMipmap mipmap) {
    _info.minFilter = VkFilter(filter);
    _info.mipmapMode = VkSamplerMipmapMode(mipmap);
    return *this;
}

SamplerCreateInfo& SamplerCreateInfo::setMinificationFilter(const Magnum::SamplerFilter filter, const Magnum::SamplerMipmap mipmap) {
    return setMinificationFilter(samplerFilter(filter), samplerMipmap(mipmap));
}

SamplerCreateInfo& SamplerCreateInfo::setMagnificationFilter(const SamplerFilter filter) {
    _info.magFilter = VkFilter(filter);
    return *this;
}

SamplerCreateInfo& SamplerCreateInfo::setMagnificationFilter(const Magnum::SamplerFilter filter) {
    return setMagnificationFilter(samplerFilter(filter));
}

SamplerCreateInfo& SamplerCreateInfo::setWrapping(const Math::Vector3<SamplerWrapping>& wrapping) {
    _info.addressModeU = VkSamplerAddressMode(wrapping.x());
    _info.addressModeV = VkSamplerAddressMode(wrapping.y());
    _info.addressModeW = VkSamplerAddressMode(wrapping.z());
    return *this;
}

SamplerCreateInfo& SamplerCreateInfo::setWrapping(const Math::Vector3<Magnum::SamplerWrapping>& wrapping) {
    return setWrapping(samplerWrapping(wrapping));
}

SamplerCreateInfo& SamplerCreateInfo::setWrapping(const SamplerWrapping wrapping) {
    return setWrapping(Math::Vector3<SamplerWrapping>{wrapping});
}

SamplerCreateInfo& SamplerCreateInfo::setWrapping(const Magnum::SamplerWrapping wrapping) {
    return setWrapping(Math::Vector3<Magnum::SamplerWrapping>{wrapping});
}

Sampler Sampler::wrap(Device& device, const VkSampler handle, const HandleFlags flags) {
    Sampler out{NoCreate};
    out._device = &device;
    out._handle = handle;
    out._flags = flags;
    return out;
}

Sampler::Sampler(Device& device, const SamplerCreateInfo& info): _device{&device}, _flags{HandleFlag::DestroyOnDestruction} {
    MAGNUM_VK_INTERNAL_ASSERT_SUCCESS(device->CreateSampler(device, info, nullptr, &_handle));
}

Sampler::Sampler(NoCreateT): _device{}, _handle{} {}

Sampler::Sampler(Sampler&& other) noexcept: _device{other._device}, _handle{other._handle}, _flags{other._flags} {
    other._handle = {};
}

Sampler::~Sampler() {
    if(_handle && (_flags & HandleFlag::DestroyOnDestruction))
        (**_device).DestroySampler(*_device, _handle, nullptr);
}

Sampler& Sampler::operator=(Sampler&& other) noexcept {
    using std::swap;
    swap(other._device, _device);
    swap(other._handle, _handle);
    swap(other._flags, _flags);
    return *this;
}

VkSampler Sampler::release() {
    const VkSampler handle = _handle;
    _handle = {};
    return handle;
}

}}
