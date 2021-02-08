#ifndef Magnum_Vk_VertexFormat_h
#define Magnum_Vk_VertexFormat_h
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

#include "Magnum/Magnum.h"
#include "Magnum/Vk/Vulkan.h"
#include "Magnum/Vk/visibility.h"

/** @file
 * @brief Enum @ref Magnum::Vk::VertexFormat, function @ref Magnum::Vk::hasVertexFormat(), @ref Magnum::Vk::vertexFormat()
 * @m_since_latest
 */

namespace Magnum { namespace Vk {

/**
@brief Vertex format
@m_since_latest

Wraps a subset of @type_vk_keyword{Format} that's usable as a vertex format.
Unless said otherwise (mostly in case of 8- and 16-bit three-component
formats and doubles), all formats are guaranteed to be supported by the spec.

@m_enum_values_as_keywords

@see @ref Magnum::VertexFormat, @ref hasVertexFormat(), @ref vertexFormat()
*/
enum class VertexFormat: Int {
    /** @ref Magnum::Float "Float" */
    Float = VK_FORMAT_R32_SFLOAT,

    /** @ref Magnum::Half "Half" */
    Half = VK_FORMAT_R16_SFLOAT,

    /**
     * @ref Magnum::Double "Double".
     *
     * Compared to @ref VertexFormat::Float, this format is not guaranteed to
     * be supported.
     */
    Double = VK_FORMAT_R64_SFLOAT,

    /** @ref Magnum::UnsignedByte "UnsignedByte" */
    UnsignedByte = VK_FORMAT_R8_UINT,

    /**
     * @ref Magnum::UnsignedByte "UnsignedByte", with range @f$ [0, 255] @f$
     * interpreted as @f$ [0.0, 1.0] @f$.
     */
    UnsignedByteNormalized = VK_FORMAT_R8_UNORM,

    /** @ref Magnum::Byte "Byte" */
    Byte = VK_FORMAT_R8_SINT,

    /**
     * @ref Magnum::Byte "Byte", with range @f$ [-127, 127] @f$ interpreted as
     * @f$ [-1.0, 1.0] @f$.
     */
    ByteNormalized = VK_FORMAT_R8_SNORM,

    /** @ref Magnum::UnsignedShort "UnsignedShort" */
    UnsignedShort = VK_FORMAT_R16_UINT,

    /**
     * @ref Magnum::UnsignedShort "UnsignedShort", with range
     * @f$ [0, 65535] @f$ interpreted as @f$ [0.0, 1.0] @f$.
     */
    UnsignedShortNormalized = VK_FORMAT_R16_UNORM,

    /** @ref Magnum::Short "Short" */
    Short = VK_FORMAT_R16_SINT,

    /**
     * @ref Magnum::Short "Short", with range @f$ [-32767, 32767] @f$
     * interpreted as @f$ [-1.0, 1.0] @f$.
     */
    ShortNormalized = VK_FORMAT_R16_SNORM,

    /** @ref Magnum::UnsignedInt "UnsignedInt" */
    UnsignedInt = VK_FORMAT_R32_UINT,

    /** @ref Magnum::Int "Int" */
    Int = VK_FORMAT_R32_SINT,

    /** @ref Magnum::Vector2 "Vector2" */
    Vector2 = VK_FORMAT_R32G32_SFLOAT,

    /** @ref Magnum::Vector2h "Vector2h" */
    Vector2h = VK_FORMAT_R16G16_SFLOAT,

    /**
     * @ref Magnum::Vector2d "Vector2d".
     *
     * Compared to @ref VertexFormat::Vector2, this format is not guaranteed to
     * be supported.
     */
    Vector2d = VK_FORMAT_R64G64_SFLOAT,

    /** @ref Magnum::Vector2ub "Vector2ub" */
    Vector2ub = VK_FORMAT_R8G8_UINT,

    /**
     * @ref Magnum::Vector2ub "Vector2ub", with range @f$ [0, 255] @f$
     * interpreted as @f$ [0.0, 1.0] @f$.
     */
    Vector2ubNormalized = VK_FORMAT_R8G8_UNORM,

    /** @ref Magnum::Vector2b "Vector2b" */
    Vector2b = VK_FORMAT_R8G8_SINT,

    /**
     * @ref Magnum::Vector2b "Vector2b", with range @f$ [-127, 127] @f$
     * interpreted as @f$ [-1.0, 1.0] @f$.
     */
    Vector2bNormalized = VK_FORMAT_R8G8_SNORM,

    /** @ref Magnum::Vector2us "Vector2us" */
    Vector2us = VK_FORMAT_R16G16_UINT,

    /**
     * @ref Magnum::Vector2us "Vector2us", with range @f$ [0, 65535] @f$
     * interpreted as @f$ [0.0, 1.0] @f$.
     */
    Vector2usNormalized = VK_FORMAT_R16G16_UNORM,

    /** @ref Magnum::Vector2s "Vector2s" */
    Vector2s = VK_FORMAT_R16G16_SINT,

    /**
     * @ref Magnum::Vector2s "Vector2s", with range @f$ [-32767, 32767] @f$
     * interpreted as @f$ [-1.0, 1.0] @f$.
     */
    Vector2sNormalized = VK_FORMAT_R16G16_SNORM,

    /** @ref Magnum::Vector2ui "Vector2ui" */
    Vector2ui = VK_FORMAT_R32G32_UINT,

    /** @ref Magnum::Vector2i "Vector2i" */
    Vector2i = VK_FORMAT_R32G32_SINT,

    /** @ref Magnum::Vector3 "Vector3" or @ref Magnum::Color3 "Color3" */
    Vector3 = VK_FORMAT_R32G32B32_SFLOAT,

    /**
     * @ref Magnum::Vector3h "Vector3h" or @ref Magnum::Color3h "Color3h".
     *
     * Compared to @ref VertexFormat::Vector3 or @ref VertexFormat::Vector2h /
     * @ref VertexFormat::Vector4h, this format is not guaranteed to be
     * supported.
     */
    Vector3h = VK_FORMAT_R16G16B16_SFLOAT,

    /**
     * @ref Magnum::Vector3d "Vector3d".
     *
     * Compared to @ref VertexFormat::Vector3, this format is not guaranteed to
     * be supported.
     */
    Vector3d = VK_FORMAT_R64G64B64_SFLOAT,

    /**
     * @ref Magnum::Vector3ub "Vector3ub".
     *
     * Compared to @ref VertexFormat::Vector3ui or @ref VertexFormat::Vector2ub
     * / @ref VertexFormat::Vector4ub, this format is not guaranteed to be
     * supported.
     */
    Vector3ub = VK_FORMAT_R8G8B8_UINT,

    /**
     * @ref Magnum::Vector3ub "Vector3ub" or @ref Magnum::Color3ub "Color3ub",
     * with range @f$ [0, 255] @f$ interpreted as @f$ [0.0, 1.0] @f$.
     *
     * Compared to @ref VertexFormat::Vector3 or
     * @ref VertexFormat..Vector2ubNormalized /
     * @ref VertexFormat::Vector4ubNormalized, this format is not guaranteed to
     * be supported.
     */
    Vector3ubNormalized = VK_FORMAT_R8G8B8_UNORM,

    /**
     * @ref Magnum::Vector3b "Vector3b".
     *
     * Compared to @ref VertexFormat::Vector3i or @ref VertexFormat::Vector2b /
     * @ref VertexFormat::Vector4b, this format is not guaranteed to be
     * supported.
     */
    Vector3b = VK_FORMAT_R8G8B8_SINT,

    /**
     * @ref Magnum::Vector3b "Vector3b", with range @f$ [-127, 127] @f$
     * )interpreted as @f$ [-1.0, 1.0] @f$.
     *
     * Compared to @ref VertexFormat::Vector3 or
     * @ref VertexFormat::Vector2bNormalized /
     * @ref VertexFormat::Vector4bNormalized, this format is not guaranteed to
     * be supported.
     */
    Vector3bNormalized = VK_FORMAT_R8G8B8_SNORM,

    /**
     * @ref Magnum::Vector3us "Vector3us".
     *
     * Compared to @ref VertexFormat::Vector3ui or @ref VertexFormat::Vector2us
     * / @ref VertexFormat::Vector4us, this format is not guaranteed to be
     * supported.
     */
    Vector3us = VK_FORMAT_R16G16B16_UINT,

    /**
     * @ref Magnum::Vector3us "Vector3us" or @ref Magnum::Color3us "Color3us",
     * with range @f$ [0, 65535] @f$ interpreted as @f$ [0.0, 1.0] @f$.
     *
     * Compared to @ref VertexFormat::Vector3 or
     * @ref VertexFormat::Vector3usNormalized /
     * @ref VertexFormat::Vector4usNormalized, this format is not guaranteed to
     * be supported.
     */
    Vector3usNormalized = VK_FORMAT_R16G16B16_UNORM,

    /**
     * @ref Magnum::Vector3s "Vector3s".
     *
     * Compared to @ref VertexFormat::Vector3i or @ref VertexFormat::Vector2b /
     * @ref VertexFormat::Vector4b, this format is not guaranteed to be
     * supported.
     */
    Vector3s = VK_FORMAT_R16G16B16_SINT,

    /**
     * @ref Magnum::Vector3s "Vector3s", with range @f$ [-32767, 32767] @f$
     * interpreted as @f$ [-1.0, 1.0] @f$.
     *
     * Compared to @ref VertexFormat::Vector3 or
     * @ref VertexFormat::Vector3sNormalized /
     * @ref VertexFormat::Vector4sNormalized, this format is not guaranteed to
     * be supported.
     */
    Vector3sNormalized = VK_FORMAT_R16G16B16_SNORM,

    /** @ref Magnum::Vector3ui "Vector3ui" */
    Vector3ui = VK_FORMAT_R32G32B32_UINT,

    /** @ref Magnum::Vector3i "Vector3i" */
    Vector3i = VK_FORMAT_R32G32B32_SINT,

    /** @ref Magnum::Vector4 "Vector4" or @ref Magnum::Color4 "Color4" */
    Vector4 = VK_FORMAT_R32G32B32A32_SFLOAT,

    /** @ref Magnum::Vector4h "Vector4h" or @ref Magnum::Color4h "Color4h" */
    Vector4h = VK_FORMAT_R16G16B16A16_SFLOAT,

    /**
     * @ref Magnum::Vector4d "Vector4d".
     *
     * Compared to @ref VertexFormat::Vector4, this format is not guaranteed to
     * be supported.
     */
    Vector4d = VK_FORMAT_R64G64B64A64_SFLOAT,

    /** @ref Magnum::Vector4ub "Vector4ub" */
    Vector4ub = VK_FORMAT_R8G8B8A8_UINT,

    /**
     * @ref Magnum::Vector4ub "Vector4ub" or @ref Magnum::Color4ub "Color4ub",
     * with range @f$ [0, 255] @f$ interpreted as @f$ [0.0, 1.0] @f$.
     */
    Vector4ubNormalized = VK_FORMAT_R8G8B8A8_UNORM,

    /** @ref Magnum::Vector4b "Vector4b" */
    Vector4b = VK_FORMAT_R8G8B8A8_SINT,

    /**
     * @ref Magnum::Vector4b "Vector4b", with range @f$ [-127, 127] @f$
     * interpreted as @f$ [-1.0, 1.0] @f$.
     */
    Vector4bNormalized = VK_FORMAT_R8G8B8A8_SNORM,

    /** @ref Magnum::Vector4us "Vector4us" */
    Vector4us = VK_FORMAT_R16G16B16A16_UINT,

    /**
     * @ref Magnum::Vector4us "Vector4us" or @ref Magnum::Color4us "Color4us",
     * with range @f$ [0, 65535] @f$ interpreted as @f$ [0.0, 1.0] @f$.
     */
    Vector4usNormalized = VK_FORMAT_R16G16B16A16_UNORM,

    /** @ref Magnum::Vector4s "Vector4s" */
    Vector4s = VK_FORMAT_R16G16B16A16_SINT,

    /**
     * @ref Magnum::Vector4s "Vector4s", with range @f$ [-32767, 32767] @f$
     * interpreted as @f$ [-1.0, 1.0] @f$.
     */
    Vector4sNormalized = VK_FORMAT_R16G16B16A16_SNORM,

    /** @ref Magnum::Vector4ui "Vector4ui" */
    Vector4ui = VK_FORMAT_R32G32B32A32_UINT,

    /** @ref Magnum::Vector4i "Vector4i" */
    Vector4i = VK_FORMAT_R32G32B32A32_SINT,
};

/**
@debugoperatorenum{VertexFormat}
@m_since_latest
*/
MAGNUM_VK_EXPORT Debug& operator<<(Debug& debug, VertexFormat value);

/**
@brief Check availability of a generic vertex format
@m_since_latest

Returns @cpp false @ce if Vulkan doesn't support such format, @cpp true @ce
otherwise. Moreover, returns @cpp true @ce also for all formats that are
@ref isVertexFormatImplementationSpecific(). The @p format value is expected
to be valid. Note that for matrix formats the function only returns a
corresponding vector type, and the user is expected to bind the remaining
vectors to consecutive attribute locations based on what
@ref vertexFormatVectorCount() and @ref vertexFormatVectorStride() return.

@note Support of some formats depends on presence of a particular Vulkan
    extension. Such check is outside of the scope of this function and you are
    expected to verify extension availability before using such format.

@see @ref vertexFormat()
*/
MAGNUM_VK_EXPORT bool hasVertexFormat(Magnum::VertexFormat format);

/**
@brief Convert a generic vertex format to Vulkan vertex format
@m_since_latest

In case @ref isVertexFormatImplementationSpecific() returns @cpp false @ce for
@p format, maps it to a corresponding Vulkan format. In case
@ref isVertexFormatImplementationSpecific() returns @cpp true @ce, assumes
@p format stores a Vulkan-specific format and returns @ref vertexFormatUnwrap()
cast to @ref VertexFormat.

Not all generic vertex formats have a Vulkan equivalent and this function
expects that given format is available. Use @ref hasVertexFormat() to query
availability of given format.
@see @ref meshIndexType(), @ref meshPrimitive()
*/
MAGNUM_VK_EXPORT VertexFormat vertexFormat(Magnum::VertexFormat format);

}}

#endif
