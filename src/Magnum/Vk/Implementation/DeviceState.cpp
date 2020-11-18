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

#include "DeviceState.h"

#include "Magnum/Vk/Buffer.h"
#include "Magnum/Vk/Device.h"
#include "Magnum/Vk/Extensions.h"
#include "Magnum/Vk/Image.h"
#include "Magnum/Vk/Version.h"

namespace Magnum { namespace Vk { namespace Implementation {

DeviceState::DeviceState(Device& device) {
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
}

}}}
