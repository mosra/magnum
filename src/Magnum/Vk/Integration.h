#ifndef Magnum_Vk_Integration_h
#define Magnum_Vk_Integration_h
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
@brief Conversion of Vulkan math types

Provides conversion for the following types:

| Magnum type                           | Equivalent Vulkan type        |
| ------------------------------------- | ----------------------------- |
| @ref Magnum::Vector2i "Vector2i"      | @type_vk_keyword{Offset2D}, @type_vk_keyword{Extent2D} |
| @ref Magnum::Vector3i "Vector3i"      | @type_vk_keyword{Offset3D}, @type_vk_keyword{Extent3D} |
| @ref Magnum::Vector2ui "Vector2ui"    | @type_vk_keyword{Extent2D}    |
| @ref Magnum::Vector3ui "Vector3ui"    | @type_vk_keyword{Extent3D}    |
| @ref Magnum::Vector4 "Vector4", @ref Magnum::Color4 "Color4", @ref Magnum::Vector4i "Vector4i", @ref Magnum::Vector4ui "Vector4ui" | @type_vk_keyword{ClearColorValue} |
| @ref Magnum::Vector3 "Vector3", @ref Magnum::Color3 "Color3" | @type_vk{ClearColorValue} |
| @ref Magnum::Range3D "Range3D"        | @type_vk_keyword{Viewport}    |
| @ref Magnum::Range2Di "Range2Di"      | @type_vk_keyword{Rect2D}      |
| @ref Magnum::Range3Di "Range3Di"      | @type_vk_keyword{ClearRect}   |

@type_vk{ClearColorValue} is an @cpp union @ce, so it's convertible from/to a
floating-point type as well as integer types, but you have to ensure the type
is correct for the API call it'll be used in. Conversion of
@type_vk{ClearColorValue} to @ref Magnum::Color3 "Color3" is not allowed, as it
would lead to loss of the alpha value. In the other direction, alpha is set to
@cpp 1.0f @ce.

Third dimension of @type_vk{Viewport} is a depth range, third dimension of
@type_vk{ClearRect} is an attachment layer range. In both cases you can use
@ref Magnum::Math::Range3D::xy() "Range3D::xy()" to slice it into a
two-dimensional range type.

Since Magnum uses a signed type for all offsets, sizes and rectangles, the
unsigned @type_vk{Extent2D} / @type_vk{Extent3D} types are convertible to
signed types as well. The @type_vk{Rect2D} and @type_vk{ClearRect} is a mixed
unsigned + signed type, which corresponds to a signed range on Magnum side.

Example usage:

@snippet MagnumVk.cpp Integration

@see @ref types-thirdparty-integration
*/

#include "Magnum/Vk/Vulkan.h"
#include "Magnum/Math/Range.h"

#ifndef DOXYGEN_GENERATING_OUTPUT
namespace Magnum { namespace Math { namespace Implementation {

template<> struct VectorConverter<2, Int, VkOffset2D> {
    constexpr static Vector<2, Int> from(const VkOffset2D& other) {
        return {other.x, other.y};
    }

    constexpr static VkOffset2D to(const Vector<2, Int>& other) {
        return {other[0], other[1]};
    }
};

template<> struct VectorConverter<3, Int, VkOffset3D> {
    constexpr static Vector<3, Int> from(const VkOffset3D& other) {
        return {other.x, other.y, other.z};
    }

    constexpr static VkOffset3D to(const Vector<3, Int>& other) {
        return {other[0], other[1], other[2]};
    }
};

template<> struct VectorConverter<2, UnsignedInt, VkExtent2D> {
    constexpr static Vector<2, UnsignedInt> from(const VkExtent2D& other) {
        return {other.width, other.height};
    }

    constexpr static VkExtent2D to(const Vector<2, UnsignedInt>& other) {
        return {other[0], other[1]};
    }
};

template<> struct VectorConverter<2, Int, VkExtent2D> {
    constexpr static Vector<2, Int> from(const VkExtent2D& other) {
        return {Int(other.width), Int(other.height)};
    }

    constexpr static VkExtent2D to(const Vector<2, Int>& other) {
        return {UnsignedInt(other[0]), UnsignedInt(other[1])};
    }
};

template<> struct VectorConverter<3, UnsignedInt, VkExtent3D> {
    constexpr static Vector<3, UnsignedInt> from(const VkExtent3D& other) {
        return {other.width, other.height, other.depth};
    }

    constexpr static VkExtent3D to(const Vector<3, UnsignedInt>& other) {
        return {other[0], other[1], other[2]};
    }
};

template<> struct VectorConverter<3, Int, VkExtent3D> {
    constexpr static Vector<3, Int> from(const VkExtent3D& other) {
        return {Int(other.width), Int(other.height), Int(other.depth)};
    }

    constexpr static VkExtent3D to(const Vector<3, Int>& other) {
        return {UnsignedInt(other[0]), UnsignedInt(other[1]), UnsignedInt(other[2])};
    }
};

template<> struct VectorConverter<4, Float, VkClearColorValue> {
    constexpr static Vector<4, Float> from(const VkClearColorValue& other) {
        return {other.float32[0], other.float32[1], other.float32[2], other.float32[3]};
    }

    /* Can this even be constexpr? */
    static VkClearColorValue to(const Vector<4, Float>& other) {
        VkClearColorValue out;
        out.float32[0] = other[0];
        out.float32[1] = other[1];
        out.float32[2] = other[2];
        out.float32[3] = other[3];
        return out;
    }
};

template<> struct VectorConverter<4, Int, VkClearColorValue> {
    constexpr static Vector<4, Int> from(const VkClearColorValue& other) {
        return {other.int32[0], other.int32[1], other.int32[2], other.int32[3]};
    }

    /* Can this even be constexpr? */
    static VkClearColorValue to(const Vector<4, Int>& other) {
        VkClearColorValue out;
        out.int32[0] = other[0];
        out.int32[1] = other[1];
        out.int32[2] = other[2];
        out.int32[3] = other[3];
        return out;
    }
};

template<> struct VectorConverter<4, UnsignedInt, VkClearColorValue> {
    constexpr static Vector<4, UnsignedInt> from(const VkClearColorValue& other) {
        return {other.uint32[0], other.uint32[1], other.uint32[2], other.uint32[3]};
    }

    /* Can this even be constexpr? */
    static VkClearColorValue to(const Vector<4, UnsignedInt>& other) {
        VkClearColorValue out;
        out.uint32[0] = other[0];
        out.uint32[1] = other[1];
        out.uint32[2] = other[2];
        out.uint32[3] = other[3];
        return out;
    }
};

template<> struct VectorConverter<3, Float, VkClearColorValue> {
    /* VkClearColorValue -> Vector3 not provided, as it loses information */

    /* Can this even be constexpr? */
    static VkClearColorValue to(const Vector<3, Float>& other) {
        VkClearColorValue out;
        out.float32[0] = other[0];
        out.float32[1] = other[1];
        out.float32[2] = other[2];
        out.float32[3] = 1.0f;
        return out;
    }
};

template<> struct RangeConverter<3, Float, VkViewport> {
    constexpr static Range<3, Float> from(const VkViewport& other) {
        return {{other.x, other.y, other.minDepth},
            {other.x + other.width, other.y + other.height, other.maxDepth}};
    }

    constexpr static VkViewport to(const Range<3, Float>& other) {
        return {other.min().x(), other.min().y(),
                other.max().x() - other.min().x(),
                other.max().y() - other.min().y(),
                other.min().z(), other.max().z()};
    }
};

template<> struct RangeConverter<2, Int, VkRect2D> {
    constexpr static Range<2, Int> from(const VkRect2D& other) {
        /* Can't use fromSize() as it's not constexpr */
        return {Vector<2, Int>(other.offset),
            {other.offset.x + Int(other.extent.width),
             other.offset.y + Int(other.extent.height)}};
    }

    constexpr static VkRect2D to(const Range<2, Int>& other) {
        /* Can't use VkExtent2D(other.size()) as it's not constexpr */
        return {VkOffset2D(other.min()),
            {UnsignedInt(other.max().x() - other.min().x()),
             UnsignedInt(other.max().y() - other.min().y())}};
    }
};

template<> struct RangeConverter<3, Int, VkClearRect> {
    constexpr static Range<3, Int> from(const VkClearRect& other) {
        /* Can't use fromSize() as it's not constexpr */
        return {{Vector<2, Int>(other.rect.offset), Int(other.baseArrayLayer)},
            {other.rect.offset.x + Int(other.rect.extent.width),
             other.rect.offset.y + Int(other.rect.extent.height),
             Int(other.baseArrayLayer + other.layerCount)}};
    }

    constexpr static VkClearRect to(const Range<3, Int>& other) {
        return {{VkOffset2D(other.min().xy()),
            {UnsignedInt(other.max().x() - other.min().x()),
             UnsignedInt(other.max().y() - other.min().y())}},
            UnsignedInt(other.min().z()),
            UnsignedInt(other.max().z() - other.min().z())};
    }
};

}}}
#endif

#endif
