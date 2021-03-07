#ifndef Magnum_Vk_SamplerCreateInfo_h
#define Magnum_Vk_SamplerCreateInfo_h
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
 * @brief Class @ref Magnum::Vk::SamplerCreateInfo, enum @ref Magnum::Vk::SamplerFilter, @ref Magnum::Vk::SamplerMipmap, @ref Magnum::Vk::SamplerWrapping, function @ref Magnum::Vk::samplerFilter(), @ref Magnum::Vk::samplerMipmap(), @ref Magnum::Vk::samplerWrapping()
 * @m_since_latest
 */

#include <Corrade/Containers/EnumSet.h>

#include "Magnum/Tags.h"
#include "Magnum/Vk/visibility.h"
#include "Magnum/Vk/Vk.h"
#include "Magnum/Vk/Vulkan.h"

namespace Magnum { namespace Vk {

/**
@brief Texture sampler filtering
@m_since_latest

Wraps a @type_vk_keyword{Filter}.
@see @ref Magnum::SamplerFilter, @ref samplerFilter(),
    @ref SamplerCreateInfo::setMinificationFilter(),
    @ref SamplerCreateInfo::setMagnificationFilter()
*/
enum class SamplerFilter: Int {
    /** Nearest neighbor filtering */
    Nearest = VK_FILTER_NEAREST,

    /** Linear interpolation filtering */
    Linear = VK_FILTER_LINEAR
};

/**
@debugoperatorenum{SamplerFilter}
@m_since_latest
*/
MAGNUM_VK_EXPORT Debug& operator<<(Debug& debug, SamplerFilter value);

/**
@brief Convert a generic sampler filter to Vulkan sampler filter
@m_since_latest

@see @ref samplerMipmap(), @ref samplerWrapping()
*/
MAGNUM_VK_EXPORT SamplerFilter samplerFilter(Magnum::SamplerFilter filter);

/**
@brief Texture sampler mip level selection
@m_since_latest

Wraps a @type_vk_keyword{SamplerMipmapMode}.
@see @ref Magnum::SamplerMipmap, @ref samplerMipmap(),
    @ref SamplerCreateInfo::setMinificationFilter()
*/
enum class SamplerMipmap: Int {
    /** Select nearest mip level */
    Nearest = VK_SAMPLER_MIPMAP_MODE_NEAREST,

    /** Linear interpolation of nearest mip levels */
    Linear = VK_SAMPLER_MIPMAP_MODE_LINEAR
};

/**
@debugoperatorenum{SamplerWrapping}
@m_since_latest
*/
MAGNUM_VK_EXPORT Debug& operator<<(Debug& debug, SamplerWrapping value);

/**
@brief Convert a generic sampler mipmap mode to Vulkan sampler mipmap mode
@m_since_latest

Vulkan doesn't support the @ref Magnum::SamplerMipmap::Base value directly,
instead @ref SamplerMipmap::Nearest is used and you have to configure the
sampler to use just a single mipmap level.
@see @ref samplerFilter(), @ref samplerWrapping()
*/
MAGNUM_VK_EXPORT SamplerMipmap samplerMipmap(Magnum::SamplerMipmap mipmap);

/**
@brief Texture sampler wrapping
@m_since_latest

@see @ref Magnum::SamplerWrapping, @ref samplerWrapping(),
    @ref SamplerCreateInfo::setWrapping()
*/
enum class SamplerWrapping: Int {
    /** Repeat the texture */
    Repeat = VK_SAMPLER_ADDRESS_MODE_REPEAT,

    /** Repeat a mirrored texture */
    MirroredRepeat = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT,

    /**
     * Clamp to edge. Coordinates out of range will be clamped to the first /
     * last column / row / layer in given direction.
     */
    ClampToEdge = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,

    /**
     * Clamp to border color. Coordinates of out range will be clamped to the
     * border color.
     */
    ClampToBorder = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,

    /**
     * Mirror the texture once in negative coordinates and clamp to edge after
     * that.
     * @requires_vk12 Extension @vk_extension{KHR,sampler_mirror_clamp_to_edge}
     * @todoc The extension mentions something about vanilla 1.2 not having
     *      this unless the extension is also listed (wtf??)
     */
    MirrorClampToEdge = VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE
};

/**
@debugoperatorenum{SamplerMipmap}
@m_since_latest
*/
MAGNUM_VK_EXPORT Debug& operator<<(Debug& debug, SamplerMipmap value);

/**
@brief Convert a generic sampler wrapping mode to Vulkan sampler wrapping mode
@m_since_latest

@see @ref samplerFilter(), @ref samplerMipmap()
*/
MAGNUM_VK_EXPORT SamplerWrapping samplerWrapping(Magnum::SamplerWrapping wrapping);

/**
 * @brief @copybrief samplerWrapping()
 * @m_deprecated_since_latest Use @ref samplerWrapping() instead.
 */
template<std::size_t dimensions> MAGNUM_VK_EXPORT Math::Vector<dimensions, SamplerWrapping> samplerWrapping(const Math::Vector<dimensions, Magnum::SamplerWrapping>& wrapping);

/**
@brief Sampler creation info
@m_since_latest

Wraps a @type_vk_keyword{SamplerCreateInfo}. See
@ref Vk-Sampler-creation "Sampler creation" for usage information.
@todo @type_vk{SamplerReductionMode}, anisotropy
*/
class MAGNUM_VK_EXPORT SamplerCreateInfo {
    public:
        /**
         * @brief Sampler creation flag
         *
         * Wraps @type_vk_keyword{SamplerCreateFlagBits}.
         * @see @ref Flags, @ref SamplerCreateInfo(Flags)
         * @m_enum_values_as_keywords
         */
        enum class Flag: UnsignedInt {
            /** @todo all the flags from extensions */
        };

        /**
         * @brief Sampler creation flags
         *
         * Type-safe wrapper for @type_vk_keyword{SamplerCreateFlags}.
         * @see @ref SamplerCreateInfo(Flags)
         */
        typedef Containers::EnumSet<Flag> Flags;

        /**
         * @brief Constructor
         * @param flags         Sampler creation flags
         *
         * The following @type_vk{SamplerCreateInfo} fields are pre-filled in
         * addition to `sType`, everything else is zero-filled:
         *
         * -    `flags`
         * -    `minFilter` and `magFilter` to @ref SamplerFilter::Nearest
         * -    `mipmapMode` to @ref SamplerMipmap::Nearest
         * -    `addressModeU`, `addressModeV` and `addressModeW` to
         *      @ref SamplerWrapping::Repeat
         * -    `minLod` to @cpp -1000.0f @ce
         * -    `maxLod` to @cpp 1000.0f @ce
         *
         * The min/max LOD defaults are chosen to be the same as OpenGL
         * defaults.
         * @see @ref setMinificationFilter(), @ref setMagnificationFilter(),
         *      @ref setWrapping()
         */
        explicit SamplerCreateInfo(Flags flags = {});

        /**
         * @brief Construct without initializing the contents
         *
         * Note that not even the `sType` field is set --- the structure has to
         * be fully initialized afterwards in order to be usable.
         */
        explicit SamplerCreateInfo(NoInitT) noexcept;

        /**
         * @brief Construct from existing data
         *
         * Copies the existing values verbatim, pointers are kept unchanged
         * without taking over the ownership. Modifying the newly created
         * instance will not modify the original data nor the pointed-to data.
         */
        explicit SamplerCreateInfo(const VkSamplerCreateInfo& info);

        /**
         * @brief Set minification filter
         * @return Reference to self (for method chaining)
         *
         * Sets the following @type_vk{SamplerCreateInfo} fields:
         *
         * -    `minFilter` to @p filter
         * -    `mipmapMode` to @p mipmap
         *
         * @see @ref setMagnificationFilter(), @ref setWrapping()
         */
        SamplerCreateInfo& setMinificationFilter(SamplerFilter filter, SamplerMipmap mipmap);
        /** @overload */
        SamplerCreateInfo& setMinificationFilter(Magnum::SamplerFilter filter, Magnum::SamplerMipmap mipmap);

        /**
         * @brief Set magnification filter
         * @return Reference to self (for method chaining)
         *
         * Sets the following @type_vk{SamplerCreateInfo} fields:
         *
         * -    `magFilter` to @p filter
         *
         * @see @ref setMinificationFilter(), @ref setWrapping()
         */
        SamplerCreateInfo& setMagnificationFilter(SamplerFilter filter);
        /** @overload */
        SamplerCreateInfo& setMagnificationFilter(Magnum::SamplerFilter filter);

        /**
         * @brief Set wrapping
         * @return Reference to self (for method chaining)
         *
         * Sets the following @type_vk{SamplerCreateInfo} fields:
         *
         * -    `addressModeU`, `addressModeV` and `addressModeW` to
         *      the respective components of @p wrapping
         *
         * @see @ref setMinificationFilter(), @ref setMagnificationFilter()
         */
        SamplerCreateInfo& setWrapping(const Math::Vector3<SamplerWrapping>& wrapping);
        /** @overload */
        SamplerCreateInfo& setWrapping(const Math::Vector3<Magnum::SamplerWrapping>& wrapping);

        /**
         * @brief Set wrapping for all dimensions at once
         * @return Reference to self (for method chaining)
         *
         * Same as calling @ref setWrapping(const Math::Vector3<SamplerWrapping>&)
         * with @p wrapping set for all components.
         */
        SamplerCreateInfo& setWrapping(SamplerWrapping wrapping);
        /** @overload */
        SamplerCreateInfo& setWrapping(Magnum::SamplerWrapping wrapping);

        /** @brief Underlying @type_vk{SamplerCreateInfo} structure */
        VkSamplerCreateInfo& operator*() { return _info; }
        /** @overload */
        const VkSamplerCreateInfo& operator*() const { return _info; }
        /** @overload */
        VkSamplerCreateInfo* operator->() { return &_info; }
        /** @overload */
        const VkSamplerCreateInfo* operator->() const { return &_info; }
        /** @overload */
        operator const VkSamplerCreateInfo*() const { return &_info; }

    private:
        VkSamplerCreateInfo _info;
};

CORRADE_ENUMSET_OPERATORS(SamplerCreateInfo::Flags)

}}

/* Make the definition complete -- it doesn't make sense to have a CreateInfo
   without the corresponding object anyway. */
#include "Magnum/Vk/Sampler.h"

#endif
