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

#include "Shader.h"
#include "ShaderCreateInfo.h"

#include <Corrade/Containers/Array.h>
#include <Corrade/Utility/Algorithms.h>

#include "Magnum/Vk/Assert.h"
#include "Magnum/Vk/Device.h"
#include "Magnum/Vk/Handle.h"
#include "Magnum/Vk/Implementation/DeviceState.h"
#include "Magnum/Vk/Implementation/spirvPatching.h"

namespace Magnum { namespace Vk {

ShaderCreateInfo::ShaderCreateInfo(Containers::ArrayView<const void> code, Flags flags): _info{} {
    _info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    _info.flags = VkShaderModuleCreateFlags(flags);
    _info.pCode = reinterpret_cast<const UnsignedInt*>(code.data());
    /* Yes, the size is in bytes, while the code pointer is an int. Have fun
       explaining this in every damn Vulkan tutorial. */
    _info.codeSize = code.size();
}

ShaderCreateInfo::ShaderCreateInfo(NoInitT) noexcept {}

ShaderCreateInfo::ShaderCreateInfo(const VkShaderModuleCreateInfo& info):
    /* Can't use {} with GCC 4.8 here because it tries to initialize the first
       member instead of doing a copy */
    _info(info) {}

ShaderCreateInfo::ShaderCreateInfo(ShaderCreateInfo&& other) noexcept:
    /* Can't use {} with GCC 4.8 here because it tries to initialize the first
       member instead of doing a copy */
    _info(other._info),
    _originalDeleter{other._originalDeleter}, _deleter{other._deleter}
{
    /* If we have a deleter and thus own the array, clear the original data
       pointer as well as it inevitably becomes dangling */
    if(_deleter) {
        other._info.pCode = {};
        other._info.codeSize = {};
    }
    other._originalDeleter = {};
    other._deleter = {};
}

ShaderCreateInfo::~ShaderCreateInfo() {
    if(_deleter) _deleter(_originalDeleter, _info.pCode, _info.codeSize);
}

ShaderCreateInfo& ShaderCreateInfo::operator=(ShaderCreateInfo&& other) noexcept {
    using std::swap;
    swap(other._info, _info);
    swap(other._originalDeleter, _originalDeleter);
    swap(other._deleter, _deleter);
    return *this;
}

Shader Shader::wrap(Device& device, const VkShaderModule handle, const HandleFlags flags) {
    Shader out{NoCreate};
    out._device = &device;
    out._handle = handle;
    out._flags = flags;
    return out;
}

Shader::Shader(Device& device, const ShaderCreateInfo& info): _device{&device}, _flags{HandleFlag::DestroyOnDestruction} {
    MAGNUM_VK_INTERNAL_ASSERT_SUCCESS(device.state().createShaderImplementation(device, *info, nullptr, _handle));
}

Shader::Shader(NoCreateT): _device{}, _handle{} {}

Shader::Shader(Shader&& other) noexcept: _device{other._device}, _handle{other._handle}, _flags{other._flags} {
    other._handle = {};
}

Shader::~Shader() {
    if(_handle && (_flags & HandleFlag::DestroyOnDestruction))
        (**_device).DestroyShaderModule(*_device, _handle, nullptr);
}

Shader& Shader::operator=(Shader&& other) noexcept {
    using std::swap;
    swap(other._device, _device);
    swap(other._handle, _handle);
    swap(other._flags, _flags);
    return *this;
}

VkShaderModule Shader::release() {
    const VkShaderModule handle = _handle;
    _handle = {};
    return handle;
}

VkResult Shader::createImplementationDefault(Device& device, const VkShaderModuleCreateInfo& info, const VkAllocationCallbacks* callbacks, VkShaderModule& handle) {
    return device->CreateShaderModule(device, &info, callbacks, &handle);
}

VkResult Shader::createImplementationSwiftShaderMultiEntryPointPatching(Device& device, const VkShaderModuleCreateInfo& info, const VkAllocationCallbacks* callbacks, VkShaderModule& handle) {
    /* Can't use {} with GCC 4.8 here because it tries to initialize the first
       member instead of doing a copy */
    VkShaderModuleCreateInfo patchedInfo(info);

    /** @todo there's too many casts and it's all slightly weird, figure out a
        better API (the problem is that spirvData() skips the header and so we
        can't use its output to copy anything anywhere, but even if we could
        we'd need to call it again on the copied mutable data and that's even
        worse than what's there now) */

    /* Even though our ShaderCreateInfo *might* have the code owned and we thus
       might not need to copy it, the owned code may also be read-only for
       whatever reason (memory-mapped location etc). Thus, to prevent issues,
       we go the safe route and copy always. */
    Containers::Array<char> mutableCode{Containers::NoInit, info.codeSize};
    Utility::copy(Containers::arrayView(reinterpret_cast<const char*>(info.pCode), info.codeSize), mutableCode);

    /* If the code looks like SPIR-V, patch it. If not, supply the original and
       let SwiftShader deal with it. */
    if(Containers::ArrayView<const UnsignedInt> spirv = ShaderTools::Implementation::spirvData(mutableCode, mutableCode.size())) {
        Implementation::spirvPatchSwiftShaderConflictingMultiEntrypointLocations(spirv);
        patchedInfo.pCode = reinterpret_cast<UnsignedInt*>(mutableCode.data());
    }

    return createImplementationDefault(device, patchedInfo, callbacks, handle);
}

}}
