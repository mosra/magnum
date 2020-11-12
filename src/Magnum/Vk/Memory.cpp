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

#include "Memory.h"

#include <Corrade/Containers/EnumSet.hpp>
#include <Corrade/Utility/Debug.h>

namespace Magnum { namespace Vk {

MemoryRequirements::MemoryRequirements(): _requirements{} {
    _requirements.sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2;
}

MemoryRequirements::MemoryRequirements(NoInitT) noexcept {}

MemoryRequirements::MemoryRequirements(const VkMemoryRequirements2& requirements):
    /* Can't use {} with GCC 4.8 here because it tries to initialize the first
       member instead of doing a copy */
    _requirements(requirements) {}

Debug& operator<<(Debug& debug, const MemoryFlag value) {
    debug << "Vk::MemoryFlag" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case Vk::MemoryFlag::value: return debug << "::" << Debug::nospace << #value;
        _c(DeviceLocal)
        _c(HostVisible)
        _c(HostCoherent)
        _c(HostCached)
        _c(LazilyAllocated)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    /* Flag bits should be in hex, unlike plain values */
    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedInt(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const MemoryFlags value) {
    return Containers::enumSetDebugOutput(debug, value, "Vk::MemoryFlags{}", {
        Vk::MemoryFlag::DeviceLocal,
        Vk::MemoryFlag::HostVisible,
        Vk::MemoryFlag::HostCoherent,
        Vk::MemoryFlag::HostCached,
        Vk::MemoryFlag::LazilyAllocated});
}

}}
