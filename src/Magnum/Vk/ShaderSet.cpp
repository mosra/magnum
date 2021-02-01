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

#include "ShaderSet.h"

#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Containers/String.h>
#include <Corrade/Containers/StringView.h>
#include <Corrade/Utility/Algorithms.h>

#include "Magnum/Vk/Shader.h"

namespace Magnum { namespace Vk {

struct ShaderSet::State {
    Containers::Array<Shader> ownedShaders;
    Containers::Array<Containers::String> entrypointNames;
    Containers::Array<char> specializationData;
    Containers::Array<VkSpecializationMapEntry> specializations;
};

ShaderSet::ShaderSet(): _stages{}, _specializations{}, _stageCount{} {}

ShaderSet::ShaderSet(ShaderSet&& other) noexcept: _stageCount{other._stageCount}, _state{std::move(other._state)} {
    /* C++, WHY THE FUCK can't you copy C arrays, why do I have to do that for
       you?! */
    Utility::copy(other._stages, _stages);
    Utility::copy(other._specializations, _specializations);
    /* The easiest is to just make the original stage list empty and leave
       whatever dangling internal pointers are there. Otherwise we'd need to
       clear even the entrypoint field in case the name is owned, which would
       make the whole list invalid and thus pointless. */
    other._stageCount = 0;
}

ShaderSet::~ShaderSet() = default;

ShaderSet& ShaderSet::operator=(ShaderSet&& other) noexcept {
    using std::swap;
    swap(other._stages, _stages);
    swap(other._specializations, _specializations);
    swap(other._stageCount, _stageCount);
    swap(other._state, _state);
    return *this;
}

ShaderSet& ShaderSet::addShader(const ShaderStage stage, const VkShaderModule shader, const Containers::StringView entrypoint, const Containers::ArrayView<const ShaderSpecialization> specializations) {
    CORRADE_ASSERT(_stageCount < Containers::arraySize(_stages),
        "Vk::ShaderSet::addShader(): too many stages, expected at most" << Containers::arraySize(_stages), *this);

    _stages[_stageCount].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    _stages[_stageCount].stage = VkShaderStageFlagBits(stage);
    _stages[_stageCount].module = shader;

    /* Not using String::nullTerminatedGlobalView() unconditionally because
       this way we can avoid allocating the state struct altogether */
    if(entrypoint.flags() >= (Containers::StringViewFlag::Global|Containers::StringViewFlag::NullTerminated)) {
        _stages[_stageCount].pName = entrypoint.data();
    } else {
        if(!_state) _state.emplace();
        /* Ensure the data are never SSO'd and so when the array reallocates we
           don't need to rewire existing name pointers */
        _stages[_stageCount].pName = arrayAppend(_state->entrypointNames, Containers::InPlaceInit, Containers::AllocatedInit, entrypoint).data();
    }

    /* Specialization, also only if there are any to avoid allocating the state
       struct when not neccessary */
    if(!specializations.empty()) {
        if(!_state) _state.emplace();

        /* Remember the original base data pointers so we can reroute the
           structures after a potential reallocation */
        const char* const previousBaseDataPointer = _state->specializationData.data();
        const VkSpecializationMapEntry* const previousBasePointer = _state->specializations.data();

        /* The data is (currently) always four bytes, so we don't need to do
           any extra work to calculate the total data size over all
           specializations */
        const Containers::ArrayView<char> newSpecializationData = arrayAppend(_state->specializationData, Containers::NoInit, specializations.size()*4);
        const Containers::ArrayView<VkSpecializationMapEntry> newSpecializations = arrayAppend(_state->specializations, Containers::NoInit, specializations.size());

        /* Reroute the existing structures for possible reallocations */
        for(std::size_t i = 0; i != _stageCount; ++i) {
            if(!_specializations[i].dataSize) continue;

            CORRADE_INTERNAL_ASSERT(_specializations[i].pData >= previousBaseDataPointer && _specializations[i].pData < previousBaseDataPointer + _state->specializationData.size() - specializations.size()*4);
            _specializations[i].pData = _state->specializationData.data() + (static_cast<const char*>(_specializations[i].pData) - previousBaseDataPointer);

            CORRADE_INTERNAL_ASSERT(_specializations[i].pMapEntries >= previousBasePointer && _specializations[i].pMapEntries < previousBasePointer + _state->specializationData.size() - specializations.size());
            _specializations[i].pMapEntries = _state->specializations.data() + (_specializations[i].pMapEntries - previousBasePointer);
        }

        /* Add new specializations */
        const auto newSpecializationDataInteger = Containers::arrayCast<UnsignedInt>(newSpecializationData);
        for(std::size_t i = 0; i != specializations.size(); ++i) {
            newSpecializations[i].constantID = specializations[i].id();
            newSpecializations[i].offset = i*4;
            newSpecializations[i].size = 4;
            newSpecializationDataInteger[i] = specializations[i].data();
        }

        _specializations[_stageCount].mapEntryCount = newSpecializations.size();
        _specializations[_stageCount].pMapEntries = newSpecializations;
        _specializations[_stageCount].dataSize = newSpecializationData.size();
        _specializations[_stageCount].pData = newSpecializationData;
        _stages[_stageCount].pSpecializationInfo = _specializations + _stageCount;
    }

    ++_stageCount;
    return *this;
}

ShaderSet& ShaderSet::addShader(const ShaderStage stage, const VkShaderModule shader, const Containers::StringView entrypoint, const std::initializer_list<ShaderSpecialization> specializations) {
    return addShader(stage, shader, entrypoint, Containers::arrayView(specializations));
}

ShaderSet& ShaderSet::addShader(const ShaderStage stage, Shader&& shader, const Containers::StringView entrypoint, const Containers::ArrayView<const ShaderSpecialization> specializations) {
    if(!_state) _state.emplace();
    return addShader(stage, arrayAppend(_state->ownedShaders, std::move(shader)), entrypoint, specializations);
}

ShaderSet& ShaderSet::addShader(const ShaderStage stage, Shader&& shader, const Containers::StringView entrypoint, const std::initializer_list<ShaderSpecialization> specializations) {
    return addShader(stage, std::move(shader), entrypoint, Containers::arrayView(specializations));
}

Containers::ArrayView<VkPipelineShaderStageCreateInfo> ShaderSet::stages() {
    return {_stages, _stageCount};
}

Containers::ArrayView<const VkPipelineShaderStageCreateInfo> ShaderSet::stages() const {
    return {_stages, _stageCount};
}

}}
