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

#include "DeviceState.h"

#include "Magnum/Vk/Buffer.h"
#include "Magnum/Vk/CommandBuffer.h"
#include "Magnum/Vk/Device.h"
#include "Magnum/Vk/DeviceProperties.h"
#include "Magnum/Vk/Extensions.h"
#include "Magnum/Vk/Image.h"
#include "Magnum/Vk/RenderPass.h"
#include "Magnum/Vk/Version.h"
#include "Magnum/Vk/Implementation/DriverWorkaround.h"

namespace Magnum { namespace Vk { namespace Implementation {

using namespace Containers::Literals;

DeviceState::DeviceState(Device& device, Containers::Array<std::pair<Containers::StringView, bool>>& encounteredWorkarounds) {
    if(device.isVersionSupported(Version::Vk11)) {
        getDeviceQueueImplementation = &Device::getQueueImplementation11;
    } else {
        getDeviceQueueImplementation = &Device::getQueueImplementationDefault;
    }

    if(device.isVersionSupported(Version::Vk11)) {
        getBufferMemoryRequirementsImplementation = &Buffer::getMemoryRequirementsImplementation11;
        getImageMemoryRequirementsImplementation = &Image::getMemoryRequirementsImplementation11;
    } else if(device.isExtensionEnabled<Extensions::KHR::get_memory_requirements2>()) {
        getBufferMemoryRequirementsImplementation = &Buffer::getMemoryRequirementsImplementationKHR;
        getImageMemoryRequirementsImplementation = &Image::getMemoryRequirementsImplementationKHR;
    } else {
        getBufferMemoryRequirementsImplementation = &Buffer::getMemoryRequirementsImplementationDefault;
        getImageMemoryRequirementsImplementation = &Image::getMemoryRequirementsImplementationDefault;
    }

    if(device.isVersionSupported(Version::Vk11)) {
        bindImageMemoryImplementation = &Image::bindMemoryImplementation11;
        bindBufferMemoryImplementation = &Buffer::bindMemoryImplementation11;
    } else if(device.isExtensionEnabled<Extensions::KHR::bind_memory2>()) {
        bindImageMemoryImplementation = &Image::bindMemoryImplementationKHR;
        bindBufferMemoryImplementation = &Buffer::bindMemoryImplementationKHR;
    } else {
        bindImageMemoryImplementation = &Image::bindMemoryImplementationDefault;
        bindBufferMemoryImplementation = &Buffer::bindMemoryImplementationDefault;
    }

    if(device.isVersionSupported(Version::Vk12)) {
        createRenderPassImplementation = &RenderPass::createImplementation12;
        cmdBeginRenderPassImplementation = &CommandBuffer::beginRenderPassImplementation12;
        cmdNextSubpassImplementation = &CommandBuffer::nextSubpassImplementation12;
        cmdEndRenderPassImplementation = &CommandBuffer::endRenderPassImplementation12;
    } else if(device.isExtensionEnabled<Extensions::KHR::create_renderpass2>()) {
        createRenderPassImplementation = &RenderPass::createImplementationKHR;
        cmdBeginRenderPassImplementation = &CommandBuffer::beginRenderPassImplementationKHR;
        cmdNextSubpassImplementation = &CommandBuffer::nextSubpassImplementationKHR;
        cmdEndRenderPassImplementation = &CommandBuffer::endRenderPassImplementationKHR;
    } else {
        createRenderPassImplementation = &RenderPass::createImplementationDefault;
        cmdBeginRenderPassImplementation = &CommandBuffer::beginRenderPassImplementationDefault;
        cmdNextSubpassImplementation = &CommandBuffer::nextSubpassImplementationDefault;
        cmdEndRenderPassImplementation = &CommandBuffer::endRenderPassImplementationDefault;
    }

    if(device.isExtensionEnabled<Extensions::EXT::extended_dynamic_state>()) {
        cmdBindVertexBuffersImplementation = &CommandBuffer::bindVertexBuffersImplementationEXT;
    } else {
        cmdBindVertexBuffersImplementation = &CommandBuffer::bindVertexBuffersImplementationDefault;
    }

    if(device.isExtensionEnabled<Extensions::KHR::copy_commands2>()) {
        cmdCopyBufferImplementation = &CommandBuffer::copyBufferImplementationKHR;
        cmdCopyImageImplementation = &CommandBuffer::copyImageImplementationKHR;
        cmdCopyBufferToImageImplementation = &CommandBuffer::copyBufferToImageImplementationKHR;
        cmdCopyImageToBufferImplementation = &CommandBuffer::copyImageToBufferImplementationKHR;
    } else {
        cmdCopyBufferImplementation = &CommandBuffer::copyBufferImplementationDefault;

        /* SwiftShader doesn't implement KHR_copy_commands2 yet so we only need
           to work around the classical code path. When it will, the image
           array tests will blow up, notifying about this omission (though I
           hope the bug gets fixed before KHR_copy_commands2 are implemented). */
        if(device.properties().name().hasPrefix("SwiftShader"_s) && !Implementation::isDriverWorkaroundDisabled(encounteredWorkarounds, "swiftshader-image-copy-extent-instead-of-layers"_s)) {
            cmdCopyImageImplementation = &CommandBuffer::copyImageImplementationSwiftShader;
            cmdCopyBufferToImageImplementation = &CommandBuffer::copyBufferToImageImplementationSwiftShader;
            cmdCopyImageToBufferImplementation = &CommandBuffer::copyImageToBufferImplementationSwiftShader;
        } else {
            cmdCopyImageImplementation = &CommandBuffer::copyImageImplementationDefault;
            cmdCopyBufferToImageImplementation = &CommandBuffer::copyBufferToImageImplementationDefault;
            cmdCopyImageToBufferImplementation = &CommandBuffer::copyImageToBufferImplementationDefault;
        }
    }
}

}}}
