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
 * @brief Enum @ref Magnum::VertexFormat, function @ref Magnum::vertexFormatSize()
 */

#include <Corrade/Utility/StlForwardString.h>

#include "Magnum/Magnum.h"
#include "Magnum/visibility.h"

namespace Magnum {

/**
@brief Vertex format
@m_since_latest

Like @ref PixelFormat, but for mesh attributes --- including double-precision
types and matrices.
@see @ref Trade::MeshData, @ref Trade::MeshAttributeData,
    @ref Trade::MeshAttribute
*/
enum class VertexFormat: UnsignedInt {
    /* Zero reserved for an invalid type (but not being a named value) */

    Float = 1,          /**< @ref Float */
    UnsignedByte,       /**< @ref UnsignedByte */
    Byte,               /**< @ref Byte */
    UnsignedShort,      /**< @ref UnsignedShort */
    Short,              /**< @ref Short */
    UnsignedInt,        /**< @ref UnsignedInt */
    Int,                /**< @ref Int */

    /**
     * @ref Vector2. Usually used for 2D positions and 2D texture coordinates.
     */
    Vector2,

    /**
     * @ref Vector3 or @ref Color3. Usually used for 3D positions, normals and
     * three-component colors.
     */
    Vector3,

    /**
     * @ref Vector4 or @ref Color4. Usually used for four-component colors.
     */
    Vector4
};

/**
@brief Size of given vertex format
@m_since_latest
*/
MAGNUM_EXPORT UnsignedInt vertexFormatSize(VertexFormat format);

/**
@debugoperatorenum{VertexFormat}
@m_since_latest
*/
MAGNUM_EXPORT Debug& operator<<(Debug& debug, VertexFormat value);

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
