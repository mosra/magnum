#ifndef Magnum_Sampler_h
#define Magnum_Sampler_h
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

/** @file
 * @brief Enum @ref Magnum::SamplerFilter, @ref Magnum::SamplerMipmap, @ref Magnum::SamplerWrapping
 */

#include "Magnum/Magnum.h"
#include "Magnum/visibility.h"

namespace Magnum {

/**
@brief Texture sampler filtering

In case of OpenGL, corresponds to @ref GL::SamplerFilter and is convertible to
it using @ref GL::samplerFilter(). See documentation of each value for more
information about the mapping.

In case of Vulkan, corresponds to @type_vk_keyword{Filter} and is convertible
to it using @ref Vk::vkFilter(). See documentation of each value for more
information about the mapping.
@see @ref SamplerMipmap, @ref SamplerWrapping
*/
enum class SamplerFilter: UnsignedInt {
    /* Unlike with MeshIndexType, MeshPrimitive, VertexFormat, PixelFormat
       etc., this enum doesn't have zero as an invalid value -- Nearest is a
       good default */

    /**
     * Nearest neighbor filtering.
     *
     * Corresponds to @ref GL::SamplerFilter::Nearest /
     * @val_vk_keyword{FILTER_NEAREST,Filter}.
     */
    Nearest = 0,

    /**
     * Linear interpolation filtering.
     *
     * Corresponds to @ref GL::SamplerFilter::Linear /
     * @val_vk_keyword{FILTER_LINEAR,Filter}.
     */
    Linear
};

/**
@brief Texture sampler mip level selection

In case of OpenGL, corresponds to @ref GL::SamplerMipmap and is convertible to
it using @ref GL::samplerMipmap(). See documentation of each value for more
information about the mapping.

In case of Vulkan, corresponds to @type_vk_keyword{SamplerMipmapMode} and is
convertible to it using @ref Vk::vkSamplerMipmapMode(). See documentation of
each value for more information about the mapping.
@see @ref SamplerFilter, @ref SamplerWrapping
*/
enum class SamplerMipmap: UnsignedInt {
    /* Unlike with MeshIndexType, MeshPrimitive, VertexFormat, PixelFormat
       etc., this enum doesn't have zero as an invalid value -- Base is a
       good default */

    /**
     * Select base mip level
     *
     * Corresponds to @ref GL::SamplerMipmap::Base. On Vulkan, the
     * corresponding mode is
     * @val_vk_keyword{SAMPLER_MIPMAP_MODE_NEAREST,SamplerMipmapMode} and you
     * have to configure the sampler to use just a single mipmap level.
     */
    Base = 0,

    /**
     * Select nearest mip level.
     *
     * Corresponds to @ref GL::SamplerMipmap::Nearest /
     * @val_vk_keyword{SAMPLER_MIPMAP_MODE_NEAREST,SamplerMipmapMode}.
     */
    Nearest,

    /**
     * Linear interpolation of nearest mip levels.
     *
     * Corresponds to @ref GL::SamplerMipmap::Linear /
     * @val_vk_keyword{SAMPLER_MIPMAP_MODE_LINEAR,SamplerMipmapMode}.
     */
    Linear
};

/**
@brief Texture sampler wrapping

In case of OpenGL, corresponds to @ref GL::SamplerWrapping and is convertible
to it using @ref GL::samplerWrapping(). See documentation of each value for
more information about the mapping. Note that not every mode is available on
all targets, use @ref GL::hasSamplerWrapping() to check for its presence.

In case of Vulkan, corresponds to @type_vk_keyword{SamplerAddressMode} and is
convertible to it using @ref Vk::vkSamplerAddressMode(). See documentation of
each value for more information about the mapping. Note that not every mode is available there, use @ref Vk::hasVkSamplerAddressMode() to check for its
presence.
@see @ref SamplerFilter, @ref SamplerMipmap
*/
enum class SamplerWrapping: UnsignedInt {
    /* Unlike with MeshIndexType, MeshPrimitive, VertexFormat, PixelFormat
       etc., this enum doesn't have zero as an invalid value -- Repeat is a
       good default */

    /**
     * Repeat texture.
     *
     * Corresponds to @ref GL::SamplerWrapping::Repeat /
     * @val_vk_keyword{SAMPLER_ADDRESS_MODE_REPEAT,SamplerAddressMode}.
     */
    Repeat = 0,

    /**
     * Repeat mirrored texture.
     *
     * Corresponds to @ref GL::SamplerWrapping::MirroredRepeat /
     * @val_vk_keyword{SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT,SamplerAddressMode}.
     */
    MirroredRepeat,

    /**
     * Clamp to edge. Coordinates out of the range will be clamped to
     * first / last column / row in given direction.
     *
     * Corresponds to @ref GL::SamplerWrapping::ClampToEdge /
     * @val_vk_keyword{SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,SamplerAddressMode}.
     */
    ClampToEdge,

    /**
     * Clamp to border color. Coordinates out of range will be clamped
     * to border color.
     *
     * Corresponds to @ref GL::SamplerWrapping::ClampToBorder /
     * @val_vk_keyword{SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,SamplerAddressMode}.
     */
    ClampToBorder,

    /**
     * Mirror the texture once in negative coordinates and clamp to
     * edge after that.
     *
     * Corresponds to @ref GL::SamplerWrapping::MirrorClampToEdge. /
     * @val_vk_keyword{SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE,SamplerAddressMode}.
     */
    MirrorClampToEdge
};

/** @debugoperatorenum{SamplerFilter} */
MAGNUM_EXPORT Debug& operator<<(Debug& debug, SamplerFilter value);

/** @debugoperatorenum{SamplerMipmap} */
MAGNUM_EXPORT Debug& operator<<(Debug& debug, SamplerMipmap value);

/** @debugoperatorenum{SamplerWrapping} */
MAGNUM_EXPORT Debug& operator<<(Debug& debug, SamplerWrapping value);

}

#endif
