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

#include "MeshLayout.h"

#include <Corrade/Containers/GrowableArray.h>

#include "Magnum/Mesh.h"
#include "Magnum/Vk/VertexFormat.h"
#include "Magnum/Vk/Implementation/structureHelpers.h"

namespace Magnum { namespace Vk {

namespace {

constexpr MeshPrimitive MeshPrimitiveMapping[]{
    MeshPrimitive::Points,
    MeshPrimitive::Lines,
    MeshPrimitive(~Int{}), /* LineLoop */
    MeshPrimitive::LineStrip,
    MeshPrimitive::Triangles,
    MeshPrimitive::TriangleStrip,
    MeshPrimitive::TriangleFan,
    MeshPrimitive(~Int{}), /* Instances */
    MeshPrimitive(~Int{}), /* Faces */
    MeshPrimitive(~Int{})  /* Edges */
};

}

bool hasMeshPrimitive(const Magnum::MeshPrimitive primitive) {
    if(isMeshPrimitiveImplementationSpecific(primitive))
        return true;

    CORRADE_ASSERT(UnsignedInt(primitive) - 1 < Containers::arraySize(MeshPrimitiveMapping),
        "Vk::hasMeshPrimitive(): invalid primitive" << primitive, {});
    return UnsignedInt(MeshPrimitiveMapping[UnsignedInt(primitive) - 1]) != ~UnsignedInt{};
}

MeshPrimitive meshPrimitive(const Magnum::MeshPrimitive primitive) {
    if(isMeshPrimitiveImplementationSpecific(primitive))
        return meshPrimitiveUnwrap<MeshPrimitive>(primitive);

    CORRADE_ASSERT(UnsignedInt(primitive) - 1 < Containers::arraySize(MeshPrimitiveMapping),
        "Vk::meshPrimitive(): invalid primitive" << primitive, {});
    const MeshPrimitive out = MeshPrimitiveMapping[UnsignedInt(primitive) - 1];
    CORRADE_ASSERT(out != MeshPrimitive(~UnsignedInt{}),
        "Vk::meshPrimitive(): unsupported primitive" << primitive, {});
    return out;
}

struct MeshLayout::State {
    Containers::Array<VkVertexInputBindingDescription> bindings;
    Containers::Array<VkVertexInputBindingDivisorDescriptionEXT> bindingDivisors;
    Containers::Array<VkVertexInputAttributeDescription> attributes;
    VkPipelineVertexInputDivisorStateCreateInfoEXT vertexDivisorInfo{};
};

MeshLayout::MeshLayout(const MeshPrimitive primitive): _vertexInfo{}, _assemblyInfo{} {
    _vertexInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    _assemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    _assemblyInfo.topology = VkPrimitiveTopology(primitive);
}

MeshLayout::MeshLayout(const Magnum::MeshPrimitive primitive): MeshLayout{meshPrimitive(primitive)} {}

MeshLayout::MeshLayout(NoInitT) noexcept {}

MeshLayout::MeshLayout(const VkPipelineVertexInputStateCreateInfo& vertexInfo, const VkPipelineInputAssemblyStateCreateInfo& assemblyInfo):
    /* Can't use {} with GCC 4.8 here because it tries to initialize the first
       member instead of doing a copy */
    _vertexInfo(vertexInfo), _assemblyInfo(assemblyInfo) {}

MeshLayout::MeshLayout(MeshLayout&& other) noexcept:
    /* Can't use {} with GCC 4.8 here because it tries to initialize the first
       member instead of doing a copy */
    _vertexInfo(other._vertexInfo),
    _assemblyInfo(other._assemblyInfo),
    _state{std::move(other._state)}
{
    /* Ensure the previous instance doesn't reference state that's now ours */
    /** @todo this is now more like a destructible move, do it more selectively
        and clear only what's really ours and not external? */
    other._vertexInfo.pNext = nullptr;
    other._vertexInfo.vertexBindingDescriptionCount = 0;
    other._vertexInfo.pVertexBindingDescriptions = nullptr;
    other._vertexInfo.vertexAttributeDescriptionCount = 0;
    other._vertexInfo.pVertexAttributeDescriptions = nullptr;
    other._assemblyInfo.pNext = nullptr;
}

MeshLayout::~MeshLayout() = default;

MeshLayout& MeshLayout::operator=(MeshLayout&& other) noexcept {
    using std::swap;
    swap(other._vertexInfo, _vertexInfo);
    swap(other._assemblyInfo, _assemblyInfo);
    swap(other._state, _state);
    return *this;
}

#ifndef CORRADE_NO_ASSERT
bool MeshLayout::hasNoExternalPointers() const {
    return
        /* Vertex info should only point to the vertex divisor info */
        (!_vertexInfo.pNext || (_state && _vertexInfo.pNext == &_state->vertexDivisorInfo)) &&
        /* Vertex divisor info should not point anywhere, if it exists */
        (!_state || !_state->vertexDivisorInfo.pNext) &&
        /* Assembly info should not point anywhere */
        !_assemblyInfo.pNext &&

        /* Vertex binding descriptions should point to our data, if any,
           otherwise it should be null and the count zero */
        ((!_vertexInfo.vertexBindingDescriptionCount && !_vertexInfo.pVertexBindingDescriptions) || (_state && _vertexInfo.pVertexBindingDescriptions == _state->bindings)) &&
        /* Attribute descriptions should point to our data, if any,
           otherwise it should be null and the count zero */
        ((!_vertexInfo.vertexAttributeDescriptionCount && !_vertexInfo.pVertexAttributeDescriptions) || (_state && _vertexInfo.pVertexAttributeDescriptions == _state->attributes)) &&
        /* Vertex divisor descriptions should point to our data, if any */
        (!_state || _state->vertexDivisorInfo.pVertexBindingDivisors == _state->bindingDivisors);
}
#endif

bool MeshLayout::operator==(const MeshLayout& other) const {
    CORRADE_ASSERT(hasNoExternalPointers() && other.hasNoExternalPointers(),
        "Vk::MeshLayout: can't compare structures with external pointers", {});

    #define _c(field) other.field == field
    /* First compare the top-level fields */
    if(!(_c(_vertexInfo.flags) &&
         _c(_vertexInfo.vertexBindingDescriptionCount) &&
         _c(_vertexInfo.vertexAttributeDescriptionCount) &&
         _c(_assemblyInfo.flags) &&
         _c(_assemblyInfo.topology) &&
         _c(_assemblyInfo.primitiveRestartEnable))) return false;

    /* Then continue only if both have the state struct -- if only one has it,
       it can be still equal if the counts are zero for both (which is verified
       by the assert above) */
    if(!other._state || !_state) return true;

    return _c(_state->vertexDivisorInfo.vertexBindingDivisorCount) &&
        /* These assume the bindings and locations are sorted (as the asserts
           enforce), otherwise this wouldn't be enough */
        /** @todo use Utility::equal() once it exists, this is way too
            error-prone */
        std::memcmp(other._state->bindings, _state->bindings, _vertexInfo.vertexBindingDescriptionCount*sizeof(decltype(_state->bindings)::Type)) == 0 &&
        std::memcmp(other._state->bindingDivisors, _state->bindingDivisors, _state->vertexDivisorInfo.vertexBindingDivisorCount*sizeof(decltype(_state->bindingDivisors)::Type)) == 0 &&
        std::memcmp(other._state->attributes, _state->attributes, _vertexInfo.vertexAttributeDescriptionCount*sizeof(decltype(_state->attributes)::Type)) == 0;
    #undef _c
}

MeshLayout& MeshLayout::addBinding(const UnsignedInt binding, const UnsignedInt stride) & {
    if(!_state) _state.emplace();

    /* Ensure order for efficient comparisons */
    CORRADE_ASSERT(_state->bindings.empty() || _state->bindings.back().binding < binding,
        "Vk::MeshLayout::addBinding(): binding" << binding << "can't be ordered after" << _state->bindings.back().binding, *this);

    VkVertexInputBindingDescription description{};
    description.binding = binding;
    description.stride = stride;
    description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    arrayAppend(_state->bindings, description);
    _vertexInfo.vertexBindingDescriptionCount = _state->bindings.size();
    _vertexInfo.pVertexBindingDescriptions = _state->bindings;

    return *this;
}

MeshLayout&& MeshLayout::addBinding(const UnsignedInt binding, const UnsignedInt stride) && {
    return std::move(addBinding(binding, stride));
}

MeshLayout& MeshLayout::addInstancedBinding(const UnsignedInt binding, const UnsignedInt stride, const UnsignedInt divisor) & {
    if(!_state) _state.emplace();

    /* Ensure order for efficient comparisons */
    CORRADE_ASSERT(_state->bindings.empty() || _state->bindings.back().binding < binding,
        "Vk::MeshLayout::addInstancedBinding(): binding" << binding << "can't be ordered after" << _state->bindings.back().binding, *this);

    VkVertexInputBindingDescription description{};
    description.binding = binding;
    description.stride = stride;
    description.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
    arrayAppend(_state->bindings, description);
    _vertexInfo.vertexBindingDescriptionCount = _state->bindings.size();
    _vertexInfo.pVertexBindingDescriptions = _state->bindings;

    if(divisor != 1) {
        VkVertexInputBindingDivisorDescriptionEXT divisorDescription{};
        divisorDescription.binding = binding;
        divisorDescription.divisor = divisor;
        arrayAppend(_state->bindingDivisors, divisorDescription);
        _state->vertexDivisorInfo.vertexBindingDivisorCount = _state->bindingDivisors.size();
        _state->vertexDivisorInfo.pVertexBindingDivisors = _state->bindingDivisors;

        /* Attach the structure if not already */
        if(!_state->vertexDivisorInfo.sType)
            Implementation::structureConnectOne(_vertexInfo.pNext, _state->vertexDivisorInfo, VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_DIVISOR_STATE_CREATE_INFO_EXT);
    }

    return *this;
}

MeshLayout&& MeshLayout::addInstancedBinding(const UnsignedInt binding, const UnsignedInt stride, const UnsignedInt divisor) && {
    return std::move(addInstancedBinding(binding, stride, divisor));
}

MeshLayout& MeshLayout::addAttribute(const UnsignedInt location, const UnsignedInt binding, const VertexFormat format, const UnsignedInt offset) & {
    if(!_state) _state.emplace();

    /* Ensure order for efficient comparisons */
    CORRADE_ASSERT(_state->attributes.empty() || _state->attributes.back().location < location,
        "Vk::MeshLayout::addAttribute(): location" << location << "can't be ordered after" << _state->attributes.back().location, *this);

    VkVertexInputAttributeDescription description{};
    description.location = location;
    description.binding = binding;
    description.format = VkFormat(format);
    description.offset = offset;
    arrayAppend(_state->attributes, description);
    _vertexInfo.vertexAttributeDescriptionCount = _state->attributes.size();
    _vertexInfo.pVertexAttributeDescriptions = _state->attributes;

    return *this;
}

MeshLayout&& MeshLayout::addAttribute(const UnsignedInt location, const UnsignedInt binding, const VertexFormat format, const UnsignedInt offset) && {
    return std::move(addAttribute(location, binding, format, offset));
}

MeshLayout& MeshLayout::addAttribute(const UnsignedInt location, const UnsignedInt binding, const Magnum::VertexFormat format, const UnsignedInt offset) & {
    return addAttribute(location, binding, vertexFormat(format), offset);
}

MeshLayout&& MeshLayout::addAttribute(const UnsignedInt location, const UnsignedInt binding, const Magnum::VertexFormat format, const UnsignedInt offset) && {
    return std::move(addAttribute(location, binding, format, offset));
}

Debug& operator<<(Debug& debug, const MeshPrimitive value) {
    debug << "Vk::MeshPrimitive" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case Vk::MeshPrimitive::value: return debug << "::" << Debug::nospace << #value;
        _c(Points)
        _c(Lines)
        _c(LineStrip)
        _c(Triangles)
        _c(TriangleStrip)
        _c(TriangleFan)
        _c(LinesAdjacency)
        _c(LineStripAdjacency)
        _c(TrianglesAdjacency)
        _c(TriangleStripAdjacency)
        _c(Patches)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    /* Vulkan docs have the values in decimal, so not converting to hex */
    return debug << "(" << Debug::nospace << Int(value) << Debug::nospace << ")";
}

}}
