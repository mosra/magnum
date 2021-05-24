#ifndef Magnum_Vk_DescriptorSetLayout_h
#define Magnum_Vk_DescriptorSetLayout_h
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

/** @file
 * @brief Class @ref Magnum::Vk::DescriptorSetLayout
 * @m_since_latest
 */

#include "Magnum/Tags.h"
#include "Magnum/Vk/Handle.h"
#include "Magnum/Vk/visibility.h"
#include "Magnum/Vk/Vk.h"
#include "Magnum/Vk/Vulkan.h"

namespace Magnum { namespace Vk {

/**
@brief Descriptor set layout
@m_since_latest

Wraps a @type_vk_keyword{DescriptorSetLayout}. A descriptor set layout
specifies what descriptors (such as uniform buffers or samplers) can be used by
shaders in a @ref Pipeline, concrete descriptors are then bound using a
@ref DescriptorSet.

@section Vk-DescriptorSetLayout-creation Descriptor set layout creation

The @ref DescriptorSetLayoutCreateInfo class takes one or more
@ref DescriptorSetLayoutBinding entries, where each specifies the binding
number, descriptor type, descriptor count in case of descriptor arrays and
which shader stages are designed to access the binding. In the following
example one uniform buffer binding @cpp 0 @ce is accessible by any stages and
one combined image/sampler binding @cpp 1 @ce is accessed only by
@link ShaderStage::Fragment @endlink:

@snippet MagnumVk.cpp DescriptorSetLayout-creation

<b></b>

@m_class{m-note m-info}

@par
    Note that in C++11 you have to use double brackets (@cpp {{ }} @ce) to
    initialize the @ref DescriptorSetLayoutBinding entries, as shown above.
    With C++14 and newer it should work with just a single pair of brackets.

@subsection Vk-DescriptorSetLayout-creation-immutable-samplers Immutable samplers

For @ref DescriptorType::Sampler and @ref DescriptorType::CombinedImageSampler
it's possible to specify a list of immutable @ref Sampler "Samplers" in place
of the descriptor count argument. The descriptor count is then implicitly taken
from size of the array. The above snippet with immutable samplers would look
like this:

@snippet MagnumVk.cpp DescriptorSetLayout-creation-immutable-samplers

@subsection Vk-DescriptorSetLayout-creation-binding-flags Descriptor binding flags

With Vulkan 1.2 or @vk_extension{EXT,descriptor_indexing} it's possible to
specify additional flags per binding. All of them require a certain
@ref DeviceFeature to be supported and enabled, see particular
@ref DescriptorSetLayoutBinding::Flag for more information:

@snippet MagnumVk.cpp DescriptorSetLayout-creation-binding-flags

@section Vk-DescriptorSetLayout-usage Descriptor set layout usage

A descriptor set layout is used in a @ref PipelineLayout creation and
subsequently for @ref DescriptorSet allocation from a @ref DescriptorPool. See
the corresponding class documentation for more information.
*/
class MAGNUM_VK_EXPORT DescriptorSetLayout {
    public:
        /**
         * @brief Wrap existing Vulkan handle
         * @param device            Vulkan device the descriptor set layout is
         *      created on
         * @param handle            The @type_vk{DescriptorSetLayout} handle
         * @param flags             Handle flags
         *
         * The @p handle is expected to be originating from @p device. Unlike
         * a descriptor set layout created using a constructor, the Vulkan
         * descriptor set layout is by default not deleted on destruction, use
         * @p flags for different behavior.
         * @see @ref release()
         */
        static DescriptorSetLayout wrap(Device& device, VkDescriptorSetLayout handle, HandleFlags flags = {});

        /**
         * @brief Constructor
         * @param device    Vulkan device to create the descriptor set layout
         *      on
         * @param info      Descriptor set layout creation info
         *
         * @see @fn_vk_keyword{CreateDescriptorSetLayout}
         */
        explicit DescriptorSetLayout(Device& device, const DescriptorSetLayoutCreateInfo& info);

        /**
         * @brief Construct without creating the descriptor set layout
         *
         * The constructed instance is equivalent to moved-from state. Useful
         * in cases where you will overwrite the instance later anyway. Move
         * another object over it to make it useful.
         */
        explicit DescriptorSetLayout(NoCreateT);

        /** @brief Copying is not allowed */
        DescriptorSetLayout(const DescriptorSetLayout&) = delete;

        /** @brief Move constructor */
        DescriptorSetLayout(DescriptorSetLayout&& other) noexcept;

        /**
         * @brief Destructor
         *
         * Destroys associated @type_vk{DescriptorSetLayout} handle, unless the
         * instance was created using @ref wrap() without
         * @ref HandleFlag::DestroyOnDestruction specified.
         * @see @fn_vk_keyword{DestroyDescriptorSetLayout}, @ref release()
         */
        ~DescriptorSetLayout();

        /** @brief Copying is not allowed */
        DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;

        /** @brief Move assignment */
        DescriptorSetLayout& operator=(DescriptorSetLayout&& other) noexcept;

        /** @brief Underlying @type_vk{DescriptorSetLayout} handle */
        VkDescriptorSetLayout handle() { return _handle; }
        /** @overload */
        operator VkDescriptorSetLayout() { return _handle; }

        /** @brief Handle flags */
        HandleFlags handleFlags() const { return _flags; }

        /**
         * @brief Release the underlying Vulkan descriptor set layout
         *
         * Releases ownership of the Vulkan descriptor set layout and returns
         * its handle so @fn_vk{DestroyDescriptorSetLayout} is not called on
         * destruction. The internal state is then equivalent to moved-from
         * state.
         * @see @ref wrap()
         */
        VkDescriptorSetLayout release();

    private:
        /* Can't be a reference because of the NoCreate constructor */
        Device* _device;

        VkDescriptorSetLayout _handle;
        HandleFlags _flags;
};

}}

#endif
