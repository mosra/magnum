#ifndef Magnum_VertexFormat_h
#define Magnum_VertexFormat_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
              Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Enum @ref Magnum::VertexFormat, function @ref Magnum::isVertexFormatImplementationSpecific(), @ref Magnum::vertexFormatWrap(), @ref Magnum::vertexFormatUnwrap(), @ref Magnum::vertexFormatSize(), @ref Magnum::vertexFormatComponentCount(), @ref Magnum::vertexFormatComponentFormat(), @ref Magnum::isVertexFormatNormalized()
 */

#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/StlForwardString.h>

#include "Magnum/Magnum.h"
#include "Magnum/visibility.h"

namespace Magnum {

/**
@brief Vertex format
@m_since_latest

Like @ref PixelFormat, but for mesh attributes --- including double-precision
types and matrices. Can act also as a wrapper for implementation-specific mesh
attribute type values using @ref vertexFormatWrap() and
@ref vertexFormatUnwrap(). Distinction between generic and
implementation-specific types can be done using
@ref isVertexFormatImplementationSpecific().

@see @ref Trade::MeshData, @ref Trade::MeshAttributeData,
    @ref Trade::MeshAttribute
*/
enum class VertexFormat: UnsignedInt {
    /* Zero reserved for an invalid type (but not being a named value) */

    /** @ref Float */
    Float = 1,

    /** @ref Half */
    Half,

    /** @ref Double */
    Double,

    /** @ref UnsignedByte */
    UnsignedByte,

    /**
     * @ref UnsignedByte, with range @f$ [0, 255] @f$ interpreted as
     * @f$ [0.0, 1.0] @f$.
     */
    UnsignedByteNormalized,

    /** @ref Byte */
    Byte,

    /**
     * @ref Byte, with range @f$ [-127, 127] @f$ interpreted as
     * @f$ [-1.0, 1.0] @f$.
     */
    ByteNormalized,

    /** @ref UnsignedShort */
    UnsignedShort,

    /**
     * @ref UnsignedShort, with range @f$ [0, 65535] @f$ interpreted as
     * @f$ [0.0, 1.0] @f$.
     */
    UnsignedShortNormalized,

    /** @ref Short */
    Short,

    /**
     * @ref Short, with range @f$ [-32767, 32767] @f$ interpreted as
     * @f$ [-1.0, 1.0] @f$.
     */
    ShortNormalized,

    /** @ref UnsignedInt */
    UnsignedInt,

    /** @ref Int */
    Int,

    /**
     * @ref Vector2. Usually used for 2D positions and 2D texture coordinates.
     */
    Vector2,

    /**
     * @ref Vector2h. Can be used instead of @ref VertexFormat::Vector2 for 2D
     * positions and 2D texture coordinates.
     */
    Vector2h,

    /** @ref Vector2d */
    Vector2d,

    /**
     * @ref Vector2ub. Can be used instead of @ref VertexFormat::Vector2 for
     * packed 2D positions and 2D texture coordinates, in which case the range
     * @f$ [0, 255] @f$ is interpreted as @f$ [0.0, 255.0] @f$.
     */
    Vector2ub,

    /**
     * @ref Vector2ub, with range @f$ [0, 255] @f$ interpreted as
     * @f$ [0.0, 1.0] @f$. Can be used instead of @ref VertexFormat::Vector2
     * for packed 2D positions and 2D texture coordinates.
     */
    Vector2ubNormalized,

    /**
     * @ref Vector2b. Can be used instead of @ref VertexFormat::Vector2 for
     * packed 2D positions and 2D texture coordinates, in which case the range
     * @f$ [-128, 127] @f$ is interpreted as @f$ [-128.0, 127.0] @f$.
     */
    Vector2b,

    /**
     * @ref Vector2b, with range @f$ [-127, 127] @f$ interpreted as
     * @f$ [-1.0, 1.0] @f$. Can be used instead of @ref VertexFormat::Vector2
     * for packed 2D positions and 2D texture coordinates.
     */
    Vector2bNormalized,

    /**
     * @ref Vector2us. Can be used instead of @ref VertexFormat::Vector2 for
     * packed 2D positions and 2D texture coordinates, in which case the range
     * @f$ [0, 65535] @f$ is interpreted as @f$ [0.0, 65535.0] @f$.
     */
    Vector2us,

    /**
     * @ref Vector2us, with range @f$ [0, 65535] @f$ interpreted as
     * @f$ [0.0, 1.0] @f$. Can be used instead of @ref VertexFormat::Vector2
     * for packed 2D positions and 2D texture coordinates.
     */
    Vector2usNormalized,

    /**
     * @ref Vector2s. Can be used instead of @ref VertexFormat::Vector2 for
     * packed 2D positions and 2D texture coordinates, in which case the range
     * @f$ [-32768, 32767] @f$ is interpreted as @f$ [-32768.0, 32767.0] @f$.
     */
    Vector2s,

    /**
     * @ref Vector2s, with range @f$ [-32767, 32767] @f$ interpreted as
     * @f$ [-1.0, 1.0] @f$. Can be used instead of @ref VertexFormat::Vector2
     * for packed 2D positions and 2D texture coordinates.
     */
    Vector2sNormalized,

    /** @ref Vector2ui */
    Vector2ui,

    /** @ref Vector2i */
    Vector2i,

    /**
     * @ref Vector3 or @ref Color3. Usually used for 3D positions, normals and
     * three-component colors.
     */
    Vector3,

    /**
     * @ref Vector3h. Can be used instead of @ref VertexFormat::Vector3 for
     * packed 3D positions and three-component colors.
     */
    Vector3h,

    /** @ref Vector3d */
    Vector3d,

    /**
     * @ref Vector3ub. Can be used instead of @ref VertexFormat::Vector3 for
     * packed 3D positions, in which case the range @f$ [0, 255] @f$ is
     * interpreted as @f$ [0.0, 255.0] @f$.
     */
    Vector3ub,

    /**
     * @ref Vector3ub, with range @f$ [0, 255] @f$ interpreted as
     * @f$ [0.0, 1.0] @f$. Can be used instead of @ref VertexFormat::Vector3
     * for packed 3D positions and three-component colors.
     */
    Vector3ubNormalized,

    /**
     * @ref Vector3b. Can be used instead of @ref VertexFormat::Vector3 for
     * packed 3D positions, in which case the range @f$ [-128, 127] @f$ is
     * interpreted as @f$ [-128.0, 127.0] @f$.
     */
    Vector3b,

    /**
     * @ref Vector3b, with range @f$ [-127, 127] @f$ interpreted as
     * @f$ [-1.0, 1.0] @f$. Can be used instead of
     * @ref VertexFormat::Vector3 for packed 3D positions and normals.
     */
    Vector3bNormalized,

    /**
     * @ref Vector3us. Can be used instead of @ref VertexFormat::Vector3 for
     * packed 2D positions, in which case the range @f$ [0, 65535] @f$ is
     * interpreted as @f$ [0.0, 65535.0] @f$.
     */
    Vector3us,

    /**
     * @ref Vector3us, with range @f$ [0, 65535] @f$ interpreted as
     * @f$ [0.0, 1.0] @f$. Can be used instead of @ref VertexFormat::Vector2
     * for packed 3D positions and three-component colors.
     */
    Vector3usNormalized,

    /**
     * @ref Vector3s. Can be used instead of @ref VertexFormat::Vector3 for
     * packed 3D positions, in which case the range @f$ [-32768, 32767] @f$ is
     * interpreted as @f$ [-32768.0, 32767.0] @f$.
     */
    Vector3s,

    /**
     * @ref Vector3s, with range @f$ [-32767, 32767] @f$ interpreted as
     * @f$ [-1.0, 1.0] @f$. Can be used instead of @ref VertexFormat::Vector3
     * for packed 3D positions and normals.
     */
    Vector3sNormalized,

    /** @ref Vector3ui */
    Vector3ui,

    /** @ref Vector3i */
    Vector3i,

    /**
     * @ref Vector4 or @ref Color4. Usually used for four-component colors.
     */
    Vector4,

    /**
     * @ref Vector4h. Can be used instead of @ref VertexFormat::Vector4 for
     * four-component colors.
     */
    Vector4h,

    /** @ref Vector4d */
    Vector4d,

    /** @ref Vector4ub */
    Vector4ub,

    /**
     * @ref Vector4ub, with range @f$ [0, 255] @f$ interpreted as
     * @f$ [0.0, 1.0] @f$. Can be used instead of @ref VertexFormat::Vector4
     * for packed linear four-component colors.
     */
    Vector4ubNormalized,

    /** @ref Vector4b */
    Vector4b,

    /**
     * @ref Vector4b, with range @f$ [-127, 127] @f$ interpreted as
     * @f$ [-1.0, 1.0] @f$.
     */
    Vector4bNormalized,

    /** @ref Vector4us */
    Vector4us,

    /**
     * @ref Vector4us, with range @f$ [0, 65535] @f$ interpreted as
     * @f$ [0.0, 1.0] @f$. Can be used instead of @ref VertexFormat::Vector4
     * for packed linear four-component colors.
     */
    Vector4usNormalized,

    /** @ref Vector4s */
    Vector4s,

    /**
     * @ref Vector4s, with range @f$ [-32767, 32767] @f$ interpreted as
     * @f$ [-1.0, 1.0] @f$.
     */
    Vector4sNormalized,

    /** @ref Vector4ui */
    Vector4ui,

    /** @ref Vector4i */
    Vector4i
};

/**
@debugoperatorenum{VertexFormat}
@m_since_latest
*/
MAGNUM_EXPORT Debug& operator<<(Debug& debug, VertexFormat value);

/**
@brief Whether a @ref VertexFormat value wraps an implementation-specific identifier
@m_since_latest

Returns @cpp true @ce if value of @p format has its highest bit set,
@cpp false @ce otherwise. Use @ref vertexFormatWrap() and @ref vertexFormatUnwrap()
to wrap/unwrap an implementation-specific indentifier to/from
@ref VertexFormat.
*/
constexpr bool isVertexFormatImplementationSpecific(VertexFormat format) {
    return UnsignedInt(format) & (1u << 31);
}

/**
@brief Wrap an implementation-specific vertex format identifier in @ref VertexFormat
@m_since_latest

Sets the highest bit on @p type to mark it as implementation-specific. Expects
that @p type fits into the remaining bits. Use @ref vertexFormatUnwrap()
for the inverse operation.
@see @ref isVertexFormatImplementationSpecific()
*/
template<class T> constexpr VertexFormat vertexFormatWrap(T implementationSpecific) {
    static_assert(sizeof(T) <= 4, "types larger than 32bits are not supported");
    return CORRADE_CONSTEXPR_ASSERT(!(UnsignedInt(implementationSpecific) & (1u << 31)),
        "vertexFormatWrap(): implementation-specific value" << reinterpret_cast<void*>(implementationSpecific) << "already wrapped or too large"),
        VertexFormat((1u << 31)|UnsignedInt(implementationSpecific));
}

/**
@brief Unwrap an implementation-specific vertex format identifier from @ref VertexFormat
@m_since_latest

Unsets the highest bit from @p type to extract the implementation-specific
value. Expects that @p type has it set. Use @ref vertexFormatWrap() for
the inverse operation.
@see @ref isVertexFormatImplementationSpecific()
*/
template<class T = UnsignedInt> constexpr T vertexFormatUnwrap(VertexFormat format) {
    return CORRADE_CONSTEXPR_ASSERT(UnsignedInt(format) & (1u << 31),
        "vertexFormatUnwrap():" << format << "isn't a wrapped implementation-specific value"),
        T(UnsignedInt(format) & ~(1u << 31));
}

/**
@brief Size of given vertex format
@m_since_latest

To get size of a single component, call this function on a result of
@ref vertexFormatComponentFormat().
*/
MAGNUM_EXPORT UnsignedInt vertexFormatSize(VertexFormat format);

/**
@brief Component format of given vertex format
@m_since_latest

The function also removes the normalization aspect from the type --- use
@ref isVertexFormatNormalized() to query that. Returns for example
@ref VertexFormat::Short for @ref VertexFormat::ShortNormalized or
@ref VertexFormat::UnsignedByte for @ref VertexFormat::Vector3ub.
Calling @ref vertexFormatComponentCount() on the return value will always
give @cpp 1 @ce; calling @ref isVertexFormatNormalized() on the return
value will always give @cpp false @ce.
@see @ref vertexFormat(VertexFormat, UnsignedInt, bool)
*/
MAGNUM_EXPORT VertexFormat vertexFormatComponentFormat(VertexFormat format);

/**
@brief Component count of given vertex format
@m_since_latest

Returns @cpp 1 @ce for scalar types and e.g. @cpp 3 @ce for
@ref VertexFormat::Vector3ub.
@see @ref vertexFormat(VertexFormat, UnsignedInt, bool)
*/
MAGNUM_EXPORT UnsignedInt vertexFormatComponentCount(VertexFormat format);

/**
@brief Component count of given vertex format
@m_since_latest

Returns @cpp true @ce for `*Normalized` types, @cpp false @ce otherwise. In
particular, floating-point types are *not* treated as normalized, even though
for example colors might commonly have values only in the @f$ [0.0, 1.0] @f$
range (or normals in the @f$ [-1.0, 1.0] @f$ range).
@see @ref vertexFormat(VertexFormat, UnsignedInt, bool)
*/
MAGNUM_EXPORT bool isVertexFormatNormalized(VertexFormat format);

/**
@brief Assemble a vertex format from parts
@m_since_latest

Converts @p format to a new format of desired component count and
normalization. Expects that @p componentCount is not larger than @cpp 4 @ce and
@p normalized is @cpp true @ce only for 8- and 16-byte integer types.
@see @ref vertexFormatComponentFormat(),
    @ref vertexFormatComponentCount(),
    @ref isVertexFormatNormalized()
*/
MAGNUM_EXPORT VertexFormat vertexFormat(VertexFormat format, UnsignedInt componentCount, bool normalized);

}

namespace Corrade { namespace Utility {

/**
@configurationvalue{Magnum::VertexFormat}
@m_since_latest
*/
template<> struct MAGNUM_EXPORT ConfigurationValue<Magnum::VertexFormat> {
    ConfigurationValue() = delete;

    /**
     * @brief Write enum value as string
     *
     * If the value is invalid, returns empty string.
     */
    static std::string toString(Magnum::VertexFormat value, ConfigurationValueFlags);

    /**
     * @brief Read enum value as string
     *
     * If the value is invalid, returns a zero (invalid) type.
     */
    static Magnum::VertexFormat fromString(const std::string& stringValue, ConfigurationValueFlags);
};

}}

#endif
