#ifndef Magnum_Vk_ShaderSet_h
#define Magnum_Vk_ShaderSet_h
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
 * @brief Class @ref Magnum::Vk::ShaderSet, @ref Magnum::Vk::ShaderSpecialization
 * @m_since_latest
 */

#include <Corrade/Containers/Pointer.h>

#include "Magnum/Tags.h"
#include "Magnum/Vk/Vk.h"
#include "Magnum/Vk/Vulkan.h"
#include "Magnum/Vk/visibility.h"

namespace Magnum { namespace Vk {

/**
@brief Shader specialization
@m_since_latest

Used by @ref ShaderSet for specifying shader specialization constants. See its
documentation for more information.
*/
class ShaderSpecialization {
    public:
        /**
         * @brief Construct an integer specialization constant
         * @param id        Specialization constant ID
         * @param value     Specialized value
         */
        /*implicit*/ ShaderSpecialization(UnsignedInt id, Int value): _id{id}, _data{reinterpret_cast<const UnsignedInt&>(value)} {}

        /**
         * @brief Construct a float specialization constant
         * @param id        Specialization constant ID
         * @param value     Specialized value
         */
        /*implicit*/ ShaderSpecialization(UnsignedInt id, Float value): _id{id}, _data{reinterpret_cast<const UnsignedInt&>(value)} {}

        /**
         * @brief Construct a boolean specialization constant
         * @param id        Specialization constant ID
         * @param value     Specialized value
         */
        /*implicit*/ ShaderSpecialization(UnsignedInt id, bool value): _id{id}, _data{value} {}

        /** @brief Specialization constant ID */
        UnsignedInt id() const { return _id; }

        /**
         * @brief Specialization value data
         *
         * The contents can be an integer, a float or a boolean extended to
         * four bytes based on what constructor got used.
         */
        UnsignedInt data() const { return _data; }

    private:
        UnsignedInt _id;

        /* It would be great if this was explicitly said in either the SPIR-V
           or Vulkan spec, but AFAICT, specialization is only possible for
           booleans (which have to be four bytes), ints and floats, not
           composite types (and at least in the GL_KHR_vulkan_glsl spec these
           are enumerated as the only allowed types). Looking at the SPIR-V
           spec, OpSpecConstant gets turned into OpConstant and that can only
           be an int or float as well.

           In conclusion, I don't see why there has to be the size specified
           if it's required to be always 4 bytes. Maybe future-proofing blah
           blah, which may as well never happen. Here I'm making my life
           simpler by explicitly supporting only the three allowed types,
           putting them all into an int. */
        UnsignedInt _data;
};

/**
@brief Shader set
@m_since_latest

A collection of @ref Shader instances together with populated
@type_vk_keyword{PipelineShaderStageCreateInfo} structures for use in a
pipeline.

@section Vk-ShaderSet-usage Usage

Based on whether the shader set is for a rasterization, compute or ray tracing
pipeline, you'll call @ref addShader() with all @ref Shader stages that the
pipeline needs. At the very least you need to specify what stage is the shader
for and the entrypoint name --- usually it'd be @cpp main() @ce, but there can
be also SPIR-V shader modules with multiple entry points, which is why this
parameter is needed.

@snippet MagnumVk.cpp ShaderSet-usage

<b></b>

@m_class{m-note m-success}

@par
    The above code uses the @link Containers::Literals::operator""_s() @endlink
    literal, which lets the library know that given string is global and
    null-terminated. Such strings then don't need to be copied internally to
    keep them in scope until they're consumed by Vulkan APIs.

@subsection Vk-ShaderSet-usage-specializations Specialization constants

If the shader module exposes specialization constants, those can be specialized
via an additional parameter, taking a list of @ref ShaderSpecialization
instances. The constant can be an integer, float or a boolean; constant IDs not
present in the SPIR-V module are ignored.

@snippet MagnumVk.cpp ShaderSet-usage-specializations

@subsection Vk-ShaderSet-usage-ownership-transfer Shader ownership transfer

To create a self-contained shader set it's possible to move the @ref Shader
instances into the class using the @ref addShader(ShaderStage, Shader&&, Containers::StringView, Containers::ArrayView<const ShaderSpecialization>)
overload. If you have a multi-entrypoint shader, move only the last specified
stage, for example:

@snippet MagnumVk.cpp ShaderSet-usage-ownership-transfer
*/
class MAGNUM_VK_EXPORT ShaderSet {
    public:
        /**
         * @brief Constructor
         *
         * Creates an empty shader set. At least one shader has to be present,
         * call @ref addShader() to add it.
         */
        explicit ShaderSet();

        /** @brief Copying is not allowed */
        ShaderSet(const ShaderSet&) = delete;

        /** @brief Move constructor */
        ShaderSet(ShaderSet&& other) noexcept;

        /**
         * @brief Destructor
         *
         * If any shaders were added using @ref addShader(ShaderStage, Shader&&, Containers::StringView, Containers::ArrayView<const ShaderSpecialization>),
         * their owned instances are destructed at this point.
         */
        ~ShaderSet();

        /** @brief Copying is not allowed */
        ShaderSet& operator=(const ShaderSet&) = delete;

        /** @brief Move assignment */
        ShaderSet& operator=(ShaderSet&& other) noexcept;

        /**
         * @brief Add a shader
         * @param stage             Shader stage
         * @param shader            A @ref Shader or a raw Vulkan shader handle
         * @param entrypoint        Entrypoint name
         * @param specializations   Specialization constant values
         * @return Reference to self (for method chaining)
         *
         * The function makes a copy of @p entrypoint if it's not global or
         * null-terminated, use the @link Containers::Literals::operator""_s() @endlink
         * literal to prevent that where possible.
         *
         * The populated @type_vk{VkPipelineShaderStageCreateInfo} is
         * subsequently available through @ref stages() for direct editing. The
         * following fields are pre-filled in addition to `sType`, everything
         * else is zero-filled:
         *
         * -    `stage`
         * -    `module` to @p shader
         * -    `pName` to @p entrypoint
         * -    `pSpecializationInfo`, if @p specializations are non-empty
         * -    @cpp pSpecializationInfo->mapEntryCount @ce,
         *      @cpp pSpecializationInfo->pMapEntries @ce,
         *      @cpp pSpecializationInfo->pMapEntries[i].constantID @ce,
         *      @cpp pSpecializationInfo->pMapEntries[i].offset @ce,
         *      @cpp pSpecializationInfo->pMapEntries[i].size @ce,
         *      @cpp pSpecializationInfo->dataSize @ce and
         *      @cpp pSpecializationInfo->pData @ce to processed and linearized
         *      contents of @p specializations
         */
        ShaderSet& addShader(ShaderStage stage, VkShaderModule shader, Containers::StringView entrypoint, Containers::ArrayView<const ShaderSpecialization> specializations);

        /** @overload */
        /* Having a default here to avoid having to include ArrayView or have
           two addShader() implementations, one with and one without */
        ShaderSet& addShader(ShaderStage stage, VkShaderModule shader, Containers::StringView entrypoint, std::initializer_list<ShaderSpecialization> specializations = {});

        /**
         * @brief Add a shader and take over its ownership
         * @return Reference to self (for method chaining)
         *
         * Compared to @ref addShader(ShaderStage, VkShaderModule, Containers::StringView, Containers::ArrayView<const ShaderSpecialization>)
         * the @p shader instance ownership is transferred to the class and
         * thus doesn't have to be managed separately.
         */
        ShaderSet& addShader(ShaderStage stage, Shader&& shader, Containers::StringView entrypoint, Containers::ArrayView<const ShaderSpecialization> specializations);

        /** @overload */
        /* Having a default here to avoid having to include ArrayView or have
           two addShader() implementations, one with and one without */
        ShaderSet& addShader(ShaderStage stage, Shader&& shader, Containers::StringView entrypoint, std::initializer_list<ShaderSpecialization> specializations = {});

        /**
         * @brief Shader stages
         *
         * Exposes all data added with @ref addShader() calls. If
         * @ref addShader() was not called yet, the returned view is empty.
         */
        Containers::ArrayView<VkPipelineShaderStageCreateInfo> stages();
        /** @overload */
        Containers::ArrayView<const VkPipelineShaderStageCreateInfo> stages() const;

    private:
        VkPipelineShaderStageCreateInfo _stages[6];
        VkSpecializationInfo _specializations[6];
        std::size_t _stageCount;

        struct State;
        Containers::Pointer<State> _state;
};

}}

#endif
