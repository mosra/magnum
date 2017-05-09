#ifndef Magnum_Vk_Shader_h
#define Magnum_Vk_Shader_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2016 Jonathan Hale <squareys@googlemail.com>

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

/** @file
 * @brief Class @ref Magnum::Vk::Shader
 */

#include <Corrade/Containers/ArrayView.h>
#include "Magnum/Magnum.h"
#include "Magnum/Vk/visibility.h"

#include "Magnum/Vk/Device.h"

#include <vulkan.h>

namespace Magnum { namespace Vk {

class MAGNUM_VK_EXPORT Shader {
    public:

        Shader(Device& device, const Containers::ArrayView<const char>& shaderCode):
            _device{&device} {
            VkShaderModuleCreateInfo moduleCreateInfo;
            moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            moduleCreateInfo.pNext = nullptr;
            moduleCreateInfo.codeSize = shaderCode.size();
            moduleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());
            moduleCreateInfo.flags = 0;

            const VkResult err = vkCreateShaderModule(*_device, &moduleCreateInfo, nullptr, &_shaderModule);
            MAGNUM_VK_ASSERT_ERROR(err);
        }

        Shader(NoCreateT): _device{nullptr} {
        }

        /** @brief Copying is not allowed */
        Shader(const Shader&) = delete;

        /** @brief Move constructor */
        Shader(Shader&& other);

        /**
         * @brief Destructor
         *
         * @see @fn_vk{DestroyShader}
         */
        ~Shader();

        /** @brief Copying is not allowed */
        Shader& operator=(const Shader&) = delete;

        /** @brief Move assignment is not allowed */
        Shader& operator=(Shader&& other) {
            std::swap(_device, other._device);
            std::swap(_shaderModule, other._shaderModule);

            return *this;
        }

        VkShaderModule vkShaderModule() {
            return _shaderModule;
        }

    private:
        Device* _device;
        VkShaderModule _shaderModule;
};

}}

#endif
