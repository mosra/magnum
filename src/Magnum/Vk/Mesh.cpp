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

#include "Mesh.h"
#include "CommandBuffer.h"

#include <Corrade/Containers/Array.h>

#include "Magnum/Mesh.h"
#include "Magnum/Vk/Buffer.h"
#include "Magnum/Vk/Device.h"
#include "Magnum/Vk/MeshLayout.h"
#include "Magnum/Vk/RasterizationPipelineCreateInfo.h"
#include "Magnum/Vk/Implementation/DeviceState.h"

namespace Magnum { namespace Vk {

namespace {

constexpr MeshIndexType IndexTypeMapping[]{
    MeshIndexType::UnsignedByte,
    MeshIndexType::UnsignedShort,
    MeshIndexType::UnsignedInt
};

}

MeshIndexType meshIndexType(const Magnum::MeshIndexType type) {
    CORRADE_ASSERT(UnsignedInt(type) - 1 < Containers::arraySize(IndexTypeMapping),
        "Vk::meshIndexType(): invalid type" << type, {});
    return IndexTypeMapping[UnsignedInt(type) - 1];
}

Debug& operator<<(Debug& debug, const MeshIndexType value) {
    debug << "Vk::MeshIndexType" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case Vk::MeshIndexType::value: return debug << "::" << Debug::nospace << #value;
        _c(UnsignedByte)
        _c(UnsignedShort)
        _c(UnsignedInt)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    /* Vulkan docs have the values in decimal, so not converting to hex */
    return debug << "(" << Debug::nospace << Int(value) << Debug::nospace << ")";
}

struct Mesh::State {
    Containers::ArrayTuple vertexBufferData;
    Containers::ArrayView<VkBuffer> vertexBuffers;
    Containers::ArrayView<UnsignedLong> vertexBufferOffsets;
    Containers::ArrayView<UnsignedLong> vertexBufferStrides;
    Containers::ArrayView<Buffer> ownedVertexBuffers;

    VkBuffer indexBuffer{};
    Buffer ownedIndexBuffer{NoCreate};
    UnsignedLong indexBufferOffset{};
    MeshIndexType indexType{};
};

Mesh::Mesh(const MeshLayout& layout): Mesh{MeshLayout{layout.vkPipelineVertexInputStateCreateInfo(), layout.vkPipelineInputAssemblyStateCreateInfo()}} {}

Mesh::Mesh(MeshLayout&& layout): _layout{std::move(layout)} {
    /* Since we know the count of buffer bindings, we can directly allocate
       all needed memory upfront */
    if(const UnsignedInt count = _layout.vkPipelineVertexInputStateCreateInfo().vertexBindingDescriptionCount) {
        _state.emplace();
        _state->vertexBufferData = Containers::ArrayTuple{
            {Containers::ValueInit, count, _state->vertexBuffers},
            {Containers::ValueInit, count, _state->vertexBufferOffsets},
            {Containers::ValueInit, count, _state->vertexBufferStrides},
            {Containers::NoInit, count, _state->ownedVertexBuffers}
        };

        /** @tod use DirectInit once ArrayTuple can do that */
        for(Buffer& b: _state->ownedVertexBuffers) new(&b) Buffer{NoCreate};
    }
}

/* We don't have any internal self-pointers so a default is fine, and
   MeshLayout has its own constructor to handle those (if any) */
Mesh::Mesh(Mesh&&) noexcept = default;

Mesh::~Mesh() = default;

Mesh& Mesh::operator=(Mesh&&) noexcept = default;

UnsignedInt Mesh::count() const {
    /* The inverse value is used to detect & assert on forgotten setCount()
       in CommandBuffer::draw(), return 0 in that case as well */
    return _count == ~UnsignedInt{} ? 0 : _count;
}

std::size_t Mesh::addVertexBufferInternal(UnsignedInt binding, VkBuffer buffer, UnsignedLong offset) {
    /* Find this binding in the layout */
    for(std::size_t i = 0, max = _layout.vkPipelineVertexInputStateCreateInfo().vertexBindingDescriptionCount; i != max; ++i) {
        if(_layout.vkPipelineVertexInputStateCreateInfo().pVertexBindingDescriptions[i].binding == binding) {
            _state->vertexBuffers[i] = buffer;
            _state->vertexBufferOffsets[i] = offset;
            /* Save the stride as well in case a dynamic state would need it */
            _state->vertexBufferStrides[i] = _layout.vkPipelineVertexInputStateCreateInfo().pVertexBindingDescriptions[i].stride;
            return i;
        }
    }

    CORRADE_ASSERT_UNREACHABLE("Vk::Mesh::addVertexBuffer(): binding" << binding << "not present among" << _layout.vkPipelineVertexInputStateCreateInfo().vertexBindingDescriptionCount << "bindings in the layout", ~std::size_t{});
}

Mesh& Mesh::addVertexBuffer(const UnsignedInt binding, const VkBuffer buffer, const UnsignedLong offset) {
    addVertexBufferInternal(binding, buffer, offset);
    return *this;
}

Mesh& Mesh::addVertexBuffer(const UnsignedInt binding, Buffer&& buffer, const UnsignedLong offset) {
    const std::size_t index = addVertexBufferInternal(binding, buffer, offset);
    #ifdef CORRADE_GRACEFUL_ASSERT
    if(index == ~std::size_t{}) return *this;
    #endif
    _state->ownedVertexBuffers[index] = std::move(buffer);
    return *this;
}

Mesh& Mesh::setIndexBuffer(const VkBuffer buffer, const UnsignedLong offset, const MeshIndexType indexType) {
    /* If the mesh has no vertex buffer bindings, the state isn't populated
       in the constructor. Do it here. */
    if(!_state) _state.emplace();

    _state->indexBuffer = buffer;
    _state->indexBufferOffset = offset;
    _state->indexType = indexType;
    return *this;
}

Mesh& Mesh::setIndexBuffer(const VkBuffer buffer, const UnsignedLong offset, const Magnum::MeshIndexType indexType) {
    return setIndexBuffer(buffer, offset, meshIndexType(indexType));
}

Mesh& Mesh::setIndexBuffer(Buffer&& buffer, const UnsignedLong offset, const MeshIndexType indexType) {
    setIndexBuffer(buffer, offset, indexType);
    _state->ownedIndexBuffer = std::move(buffer);
    return *this;
}

Mesh& Mesh::setIndexBuffer(Buffer&& buffer, const UnsignedLong offset, const Magnum::MeshIndexType indexType) {
    return setIndexBuffer(std::move(buffer), offset, meshIndexType(indexType));
}

Containers::ArrayView<const VkBuffer> Mesh::vertexBuffers() {
    return _state ? _state->vertexBuffers : nullptr;
}

Containers::ArrayView<const UnsignedLong> Mesh::vertexBufferOffsets() const {
    return _state ? _state->vertexBufferOffsets : nullptr;
}

Containers::ArrayView<const UnsignedLong> Mesh::vertexBufferStrides() const {
    return _state ? _state->vertexBufferStrides : nullptr;
}

bool Mesh::isIndexed() const { return _state && _state->indexBuffer; }

VkBuffer Mesh::indexBuffer() {
    CORRADE_ASSERT(isIndexed(), "Vk::Mesh::indexBuffer(): the mesh is not indexed", {});
    return _state->indexBuffer;
}

UnsignedLong Mesh::indexBufferOffset() const {
    CORRADE_ASSERT(isIndexed(), "Vk::Mesh::indexBufferOffset(): the mesh is not indexed", {});
    return _state->indexBufferOffset;
}

MeshIndexType Mesh::indexType() const {
    CORRADE_ASSERT(isIndexed(), "Vk::Mesh::indexType(): the mesh is not indexed", {});
    return _state->indexType;
}

CommandBuffer& CommandBuffer::draw(Mesh& mesh) {
    CORRADE_ASSERT(mesh.isCountSet(),
        "Vk::CommandBuffer::draw(): Mesh::setCount() was never called, probably a mistake?", *this);

    if(!mesh.count() || !mesh.instanceCount()) return *this;

    if(_dynamicRasterizationStates & DynamicRasterizationState::MeshPrimitive)
        (**_device).CmdSetPrimitiveTopologyEXT(_handle, mesh.layout().vkPipelineInputAssemblyStateCreateInfo().topology);

    const VkVertexInputBindingDescription* bindings = mesh.layout().vkPipelineVertexInputStateCreateInfo().pVertexBindingDescriptions;
    for(std::size_t i = 0, max = mesh.layout().vkPipelineVertexInputStateCreateInfo().vertexBindingDescriptionCount; i != max; ++i) {
        /** @todo don't call this for each binding, detect ranges */
        _device->state().cmdBindVertexBuffersImplementation(*this,
            bindings[i].binding, 1,
            mesh.vertexBuffers() + i,
            mesh.vertexBufferOffsets() + i,
            _dynamicRasterizationStates & DynamicRasterizationState::VertexInputBindingStride ?
                mesh.vertexBufferStrides() + i : nullptr
        );
    }

    if(mesh.isIndexed()) {
        (**_device).CmdBindIndexBuffer(_handle, mesh.indexBuffer(), mesh.indexBufferOffset(), VkIndexType(mesh.indexType()));
        (**_device).CmdDrawIndexed(_handle, mesh.count(), mesh.instanceCount(), mesh.indexOffset(), mesh.vertexOffset(), mesh.instanceOffset());
    } else {
        (**_device).CmdDraw(_handle, mesh.count(), mesh.instanceCount(), mesh.vertexOffset(), mesh.instanceOffset());
    }

    return *this;
}

void CommandBuffer::bindVertexBuffersImplementationDefault(CommandBuffer& self, const UnsignedInt firstBinding, const UnsignedInt bindingCount, const VkBuffer* const buffers, const UnsignedLong* const offsets, const UnsignedLong* const strides) {
    CORRADE_ASSERT(!strides,
        "Vk::CommandBuffer::draw(): dynamic strides supplied for an implementation without extended dynamic state",
        /* Calling this even in case the assert blows up to avoid validation
           layer errors about unbound attributes when CORRADE_GRACEFUL_ASSERT
           is enabled */
        (**self._device).CmdBindVertexBuffers(self, firstBinding, bindingCount, buffers, offsets));
    #ifdef CORRADE_NO_ASSERT
    static_cast<void>(strides);
    #endif
    (**self._device).CmdBindVertexBuffers(self, firstBinding, bindingCount, buffers, offsets);
}

void CommandBuffer::bindVertexBuffersImplementationEXT(CommandBuffer& self, const UnsignedInt firstBinding, const UnsignedInt bindingCount, const VkBuffer* const buffers, const UnsignedLong* const offsets, const UnsignedLong* const strides) {
    return (**self._device).CmdBindVertexBuffers2EXT(self, firstBinding, bindingCount, buffers, offsets, nullptr, strides);
}

}}
