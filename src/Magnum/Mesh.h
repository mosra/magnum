#ifndef Magnum_Mesh_h
#define Magnum_Mesh_h
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
 * @brief Enum @ref Magnum::MeshPrimitive, @ref Magnum::MeshIndexType, function @ref Magnum::isMeshPrimitiveImplementationSpecific(), @ref Magnum::meshPrimitiveWrap(), @ref Magnum::meshPrimitiveUnwrap(), @ref Magnum::meshIndexTypeSize()
 */

#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/StlForwardString.h>

#include "Magnum/Magnum.h"
#include "Magnum/visibility.h"

namespace Magnum {

/**
@brief Mesh primitive type

Can act also as a wrapper for implementation-specific mesh primitive types
using @ref meshPrimitiveWrap() and @ref meshPrimitiveUnwrap(). Distinction
between generic and implementation-specific primitive types can be done using
@ref isMeshPrimitiveImplementationSpecific().

In case of OpenGL, corresponds to @ref GL::MeshPrimitive and is convertible to
it using @ref GL::meshPrimitive(). See documentation of each value for more
information about the mapping.

In case of Vulkan, corresponds to @type_vk_keyword{PrimitiveTopology} and is
convertible to it using @ref Vk::vkPrimitiveTopology(). See documentation of
each value for more information about the mapping. Note that not every mode is available there, use @ref Vk::hasVkPrimitiveTopology() to check for its
presence.

For D3D, corresponds to @m_class{m-doc-external} [D3D_PRIMITIVE_TOPOLOGY](https://docs.microsoft.com/en-us/windows/win32/api/d3dcommon/ne-d3dcommon-d3d_primitive_topology);
for Metal, corresponds to @m_class{m-doc-external} [MTLPrimitiveType](https://developer.apple.com/documentation/metal/mtlprimitivetype?language=objc).
See documentation of each value for more information about the mapping.

@see @ref MeshTools::primitiveCount(MeshPrimitive, UnsignedInt)
*/
enum class MeshPrimitive: UnsignedInt {
    /* Zero reserved for an invalid type (but not being a named value) */

    /**
     * Single points.
     *
     * Corresponds to @ref GL::MeshPrimitive::Points;
     * @val_vk_keyword{PRIMITIVE_TOPOLOGY_POINT_LIST,PrimitiveTopology};
     * @m_class{m-doc-external} [D3D_PRIMITIVE_TOPOLOGY_POINTLIST](https://docs.microsoft.com/en-us/windows/win32/api/d3dcommon/ne-d3dcommon-d3d_primitive_topology)
     * or @m_class{m-doc-external} [MTLPrimitiveTypePoint](https://developer.apple.com/documentation/metal/mtlprimitivetype/mtlprimitivetypepoint?language=objc).
     * @m_keywords{D3D_PRIMITIVE_TOPOLOGY_POINTLIST MTLPrimitiveTypePoint}
     */
    Points = 1,

    /**
     * Each pair of vertices defines a single line, lines aren't
     * connected together.
     *
     * Corresponds to @ref GL::MeshPrimitive::Lines /
     * @val_vk_keyword{PRIMITIVE_TOPOLOGY_LINE_LIST,PrimitiveTopology};
     * @m_class{m-doc-external} [D3D_PRIMITIVE_TOPOLOGY_LINELIST](https://docs.microsoft.com/en-us/windows/win32/api/d3dcommon/ne-d3dcommon-d3d_primitive_topology)
     * or @m_class{m-doc-external} [MTLPrimitiveTypeLine](https://developer.apple.com/documentation/metal/mtlprimitivetype/mtlprimitivetypeline?language=objc).
     * @m_keywords{D3D_PRIMITIVE_TOPOLOGY_LINELIST MTLPrimitiveTypeLine}
     */
    Lines,

    /**
     * Line strip, last and first vertex are connected together.
     *
     * Corresponds to @ref GL::MeshPrimitive::LineLoop. Not supported on
     * Vulkan, D3D or Metal.
     * @see @ref MeshTools::generateLineLoopIndices()
     */
    LineLoop,

    /**
     * First two vertices define first line segment, each following
     * vertex defines another segment.
     *
     * Corresponds to @ref GL::MeshPrimitive::LineStrip /
     * @val_vk_keyword{PRIMITIVE_TOPOLOGY_LINE_STRIP,PrimitiveTopology};
     * @m_class{m-doc-external} [D3D_PRIMITIVE_TOPOLOGY_LINESTRIP](https://docs.microsoft.com/en-us/windows/win32/api/d3dcommon/ne-d3dcommon-d3d_primitive_topology)
     * or @m_class{m-doc-external} [MTLPrimitiveTypeLineStrip](https://developer.apple.com/documentation/metal/mtlprimitivetype/mtlprimitivetypelinestrip?language=objc).
     * @m_keywords{D3D_PRIMITIVE_TOPOLOGY_LINESTRIP MTLPrimitiveTypeLineStrip}
     * @see @ref MeshTools::generateLineStripIndices()
     */
    LineStrip,

    /**
     * Each three vertices define one triangle.
     *
     * Corresponds to @ref GL::MeshPrimitive::Triangles /
     * @val_vk_keyword{PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,PrimitiveTopology};
     * @m_class{m-doc-external} [D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST](https://docs.microsoft.com/en-us/windows/win32/api/d3dcommon/ne-d3dcommon-d3d_primitive_topology)
     * or @m_class{m-doc-external} [MTLPrimitiveTypeTriangle](https://developer.apple.com/documentation/metal/mtlprimitivetype/mtlprimitivetypetriangle?language=objc).
     * @m_keywords{D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST MTLPrimitiveTypeTriangle}
     */
    Triangles,

    /**
     * First three vertices define first triangle, each following
     * vertex defines another triangle.
     *
     * Corresponds to @ref GL::MeshPrimitive::TriangleStrip /
     * @val_vk_keyword{PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,PrimitiveTopology} or
     * @m_class{m-doc-external} [D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP](https://docs.microsoft.com/en-us/windows/win32/api/d3dcommon/ne-d3dcommon-d3d_primitive_topology)
     * or @m_class{m-doc-external} [MTLPrimitiveTypeTriangleStrip](https://developer.apple.com/documentation/metal/mtlprimitivetype/mtlprimitivetypetrianglestrip?language=objc).
     * @m_keywords{D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP MTLPrimitiveTypeTriangleStrip}
     * @see @ref MeshTools::generateTriangleStripIndices()
     */
    TriangleStrip,

    /**
     * First vertex is center, each following vertex is connected to
     * previous and center vertex.
     *
     * Corresponds to @ref GL::MeshPrimitive::TriangleFan /
     * @val_vk_keyword{PRIMITIVE_TOPOLOGY_TRIANGLE_FAN,PrimitiveTopology}. Not
     * supported on D3D or Metal.
     * @see @ref MeshTools::generateTriangleFanIndices()
     */
    TriangleFan,

    /**
     * Per-instance data.
     * @m_since{2020,06}
     *
     * Has no direct mapping to GPU APIs, but can be used to annotate
     * @ref Trade::MeshData containing per-instance data (such as colors,
     * transformations or texture layers) and then used to populate an instance
     * buffer. Index buffer has no defined meaning for instance data.
     */
    Instances,

    /**
     * Per-face data.
     * @m_since{2020,06}
     *
     * Can be used to annotate @ref Trade::MeshData containing data that are
     * per-face, as opposed to per-vertex. Has no direct mapping to common GPU
     * APIs, there it either has to be converted to per-vertex (which usually
     * involves slightly duplicating the original per-vertex data) or accessed
     * via a direct buffer/texture fetch from a shader using e.g.
     * @glsl gl_VertexID @ce. Index buffer can be used to deduplicate per-face
     * data.
     */
    Faces,

    /**
     * Per-edge data.
     * @m_since{2020,06}
     *
     * Can be used to annotate @ref Trade::MeshData containing data that are
     * per-edge, as opposed to per-vertex. This is different from
     * @ref MeshPrimitive::Lines as it has just one entry per line segment,
     * instead of two. Has no direct mapping to common GPU APIs, there it has
     * to be converted to per-vertex (which usually involves slightly
     * duplicating the original per-vertex data). Index buffer can be used to
     * deduplicate per-face data. Can also be used for example to describe a
     * half-edge mesh representation.
     * @see @ref Trade::meshAttributeCustom()
     */
    Edges
};

/** @debugoperatorenum{MeshPrimitive} */
MAGNUM_EXPORT Debug& operator<<(Debug& debug, MeshPrimitive value);

/**
@brief Whether a @ref MeshPrimitive value wraps an implementation-specific identifier
@m_since{2020,06}

Returns @cpp true @ce if value of @p primitive has its highest bit set,
@cpp false @ce otherwise. Use @ref meshPrimitiveWrap() and @ref meshPrimitiveUnwrap()
to wrap/unwrap an implementation-specific indentifier to/from
@ref MeshPrimitive.
*/
constexpr bool isMeshPrimitiveImplementationSpecific(MeshPrimitive primitive) {
    return UnsignedInt(primitive) & (1u << 31);
}

/**
@brief Wrap an implementation-specific mesh primitive identifier in @ref MeshPrimitive
@m_since{2020,06}

Sets the highest bit on @p primitive to mark it as implementation-specific.
Expects that @p primitive fits into the remaining bits. Use
@ref meshPrimitiveUnwrap() for the inverse operation.
@see @ref isMeshPrimitiveImplementationSpecific()
*/
template<class T> constexpr MeshPrimitive meshPrimitiveWrap(T implementationSpecific) {
    static_assert(sizeof(T) <= 4, "types larger than 32bits are not supported");
    return CORRADE_CONSTEXPR_ASSERT(!(UnsignedInt(implementationSpecific) & (1u << 31)),
        "meshPrimitiveWrap(): implementation-specific value" << reinterpret_cast<void*>(implementationSpecific) << "already wrapped or too large"),
        MeshPrimitive((1u << 31)|UnsignedInt(implementationSpecific));
}

/**
@brief Unwrap an implementation-specific mesh primitive identifier from @ref MeshPrimitive
@m_since{2020,06}

Unsets the highest bit from @p primitive to extract the implementation-specific
value. Expects that @p primitive has it set. Use @ref meshPrimitiveWrap() for
the inverse operation.
@see @ref isMeshPrimitiveImplementationSpecific()
*/
template<class T = UnsignedInt> constexpr T meshPrimitiveUnwrap(MeshPrimitive primitive) {
    return CORRADE_CONSTEXPR_ASSERT(UnsignedInt(primitive) & (1u << 31),
        "meshPrimitiveUnwrap():" << primitive << "isn't a wrapped implementation-specific value"),
        T(UnsignedInt(primitive) & ~(1u << 31));
}

/**
@brief Mesh index type

In case of OpenGL, corresponds to @ref GL::MeshIndexType and is convertible to
it using @ref GL::meshIndexType(). See documentation of each value for more
information about the mapping.

In case of Vulkan, corresponds to @type_vk_keyword{IndexType} and is
convertible to it using @ref Vk::vkIndexType(). See documentation of each value
for more information about the mapping. Note that not every type is available
there, use @ref Vk::hasVkIndexType() to check for its presence.
@see @ref meshIndexTypeSize()
*/
enum class MeshIndexType: UnsignedByte {
    /* Zero reserved for an invalid type (but not being a named value) */

    /**
     * Unsigned byte
     *
     * Corresponds to @ref GL::MeshIndexType::UnsignedByte /
     * @val_vk_keyword{INDEX_TYPE_UINT8_EXT,IndexType}. Note that using this
     * type is discouraged, at least AMD GPUs are known to suggest (via debug
     * output) using 16-byte types instead for better efficiency.
     */
    UnsignedByte = 1,

    /**
     * Unsigned short
     *
     * Corresponds to @ref GL::MeshIndexType::UnsignedShort /
     * @val_vk_keyword{INDEX_TYPE_UINT16,IndexType}.
     */
    UnsignedShort,

    /**
     * Unsigned int
     *
     * Corresponds to @ref GL::MeshIndexType::UnsignedInt /
     * @val_vk_keyword{INDEX_TYPE_UINT32,IndexType}.
     */
    UnsignedInt
};

/** @brief Size of given mesh index type */
MAGNUM_EXPORT UnsignedInt meshIndexTypeSize(MeshIndexType type);

/** @debugoperatorenum{MeshIndexType} */
MAGNUM_EXPORT Debug& operator<<(Debug& debug, MeshIndexType value);

}

namespace Corrade { namespace Utility {

/** @configurationvalue{Magnum::MeshPrimitive} */
template<> struct MAGNUM_EXPORT ConfigurationValue<Magnum::MeshPrimitive> {
    ConfigurationValue() = delete;

    /**
     * @brief Writes enum value as string
     *
     * If the value is invalid, returns empty string.
     */
    static std::string toString(Magnum::MeshPrimitive value, ConfigurationValueFlags);

    /**
     * @brief Reads enum value as string
     *
     * If the value is invalid, returns a zero (invalid) primitive.
     */
    static Magnum::MeshPrimitive fromString(const std::string& stringValue, ConfigurationValueFlags);
};

/** @configurationvalue{Magnum::MeshIndexType} */
template<> struct MAGNUM_EXPORT ConfigurationValue<Magnum::MeshIndexType> {
    ConfigurationValue() = delete;

    /**
     * @brief Write enum value as string
     *
     * If the value is invalid, returns empty string.
     */
    static std::string toString(Magnum::MeshIndexType value, ConfigurationValueFlags);

    /**
     * @brief Read enum value as string
     *
     * If the value is invalid, returns a zero (invalid) type.
     */
    static Magnum::MeshIndexType fromString(const std::string& stringValue, ConfigurationValueFlags);
};

}}

#endif
