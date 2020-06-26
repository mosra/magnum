#ifndef Magnum_Primitives_Circle_h
#define Magnum_Primitives_Circle_h
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
 * @brief Function @ref Magnum::Primitives::circle2DSolid(), @ref Magnum::Primitives::circle2DWireframe(), @ref Magnum::Primitives::circle3DSolid(), @ref Magnum::Primitives::circle3DWireframe()
 */

#include <Corrade/Containers/EnumSet.h>
#include <Corrade/Utility/Macros.h>

#include "Magnum/Primitives/visibility.h"
#include "Magnum/Trade/Trade.h"

namespace Magnum { namespace Primitives {

/**
@brief 2D circle flag
@m_since{2020,06}

@see @ref Circle2DFlags, @ref circle2DSolid()
*/
enum class Circle2DFlag: UnsignedByte {
    TextureCoordinates = 1 << 0 /**< Generate texture coordinates */
};

/**
@brief 2D circle flags
@m_since{2020,06}

@see @ref circle2DSolid()
*/
typedef Containers::EnumSet<Circle2DFlag> Circle2DFlags;

CORRADE_ENUMSET_OPERATORS(Circle2DFlags)

#ifdef MAGNUM_BUILD_DEPRECATED
/**
@brief Whether to generate circle texture coordinates
@m_deprecated_since{2020,06} Use @ref Circle2DFlags or @ref Circle3DFlags
    instead.
*/
enum class CORRADE_DEPRECATED_ENUM("use Circle2DFlags or Circle3DFlags instead") CircleTextureCoords: UnsignedByte {
    DontGenerate,       /**< Don't generate texture coordinates */
    Generate            /**< Generate texture coordinates */
};
#endif

/**
@brief Solid 2D circle
@param segments     Number of segments. Must be greater or equal to @cpp 3 @ce.
@param flags        Flags
@m_since{2020,06}

Circle with radius @cpp 1.0f @ce, centered at origin.
@ref MeshPrimitive::TriangleFan with @ref MeshIndexType::UnsignedInt indices,
interleaved @ref VertexFormat::Vector2 positions and optional
@ref VertexFormat::Vector2 texture coordinates.

@image html primitives-circle2dsolid.png width=256px

@see @ref circle2DWireframe(), @ref circle3DSolid(),
    @ref MeshTools::generateTriangleFanIndices()
*/
MAGNUM_PRIMITIVES_EXPORT Trade::MeshData circle2DSolid(UnsignedInt segments, Circle2DFlags flags = {});

#ifdef MAGNUM_BUILD_DEPRECATED
/**
@brief @copybrief circle2DSolid(UnsignedInt, Circle2DFlags)
@m_deprecated_since{2020,06} Use @ref circle2DSolid(UnsignedInt, Circle2DFlags)
    instead.
*/
CORRADE_IGNORE_DEPRECATED_PUSH
MAGNUM_PRIMITIVES_EXPORT CORRADE_DEPRECATED("use circle2DSolid() with Circle2DFlags instead") Trade::MeshData circle2DSolid(UnsignedInt segments, CircleTextureCoords textureCoords);
CORRADE_IGNORE_DEPRECATED_POP
#endif

/**
@brief Wireframe 2D circle
@param segments         Number of segments. Must be greater or equal to
    @cpp 3 @ce.

Circle with radius @cpp 1.0f @ce, centered at origin. Non-indexed
@ref MeshPrimitive::LineLoop with @ref VertexFormat::Vector2 positions.

@image html primitives-circle2dwireframe.png width=256px

@see @ref circle2DSolid(), @ref circle3DWireframe(),
    @ref MeshTools::generateLineLoopIndices()
*/
MAGNUM_PRIMITIVES_EXPORT Trade::MeshData circle2DWireframe(UnsignedInt segments);

/**
@brief 3D circle flag
@m_since{2020,06}

@see @ref Circle3DFlags, @ref circle3DSolid()
*/
enum class Circle3DFlag: UnsignedByte {
    TextureCoordinates = 1 << 0, /**< Generate texture coordinates */

    /**
     * Generate four-component tangents. The last component can be used to
     * reconstruct a bitangent as described in the documentation of
     * @ref Trade::MeshAttribute::Tangent.
     * @m_since{2020,06}
     */
    Tangents = 1 << 1
};

/**
@brief 3D circle flags
@m_since{2020,06}

@see @ref circle3DSolid()
*/
typedef Containers::EnumSet<Circle3DFlag> Circle3DFlags;

CORRADE_ENUMSET_OPERATORS(Circle3DFlags)

/**
@brief Solid 3D circle
@param segments     Number of segments. Must be greater or equal to @cpp 3 @ce.
@param flags        Flags
@m_since{2020,06}

Circle on the XY plane with radius @cpp 1.0f @ce, centered at origin.
Non-indexed @ref MeshPrimitive::TriangleFan with interleaved
@ref VertexFormat::Vector3 positions, @ref VertexFormat::Vector3 normals in
positive Z direction, optional @ref VertexFormat::Vector4 tangents and optional
@ref VertexFormat::Vector2 texture coordinates.

@image html primitives-circle3dsolid.png width=256px

@see @ref circle3DWireframe(), @ref circle2DSolid(),
    @ref MeshTools::generateTriangleFanIndices()
*/
MAGNUM_PRIMITIVES_EXPORT Trade::MeshData circle3DSolid(UnsignedInt segments, Circle3DFlags flags = {});

#ifdef MAGNUM_BUILD_DEPRECATED
/**
@brief @copybrief circle3DSolid(UnsignedInt, Circle3DFlags)
@m_deprecated_since{2020,06} Use @ref circle3DSolid(UnsignedInt, Circle3DFlags)
    instead.
*/
CORRADE_IGNORE_DEPRECATED_PUSH
MAGNUM_PRIMITIVES_EXPORT CORRADE_DEPRECATED("use circle3DSolid() with Circle2DFlags instead") Trade::MeshData circle3DSolid(UnsignedInt segments, CircleTextureCoords textureCoords);
CORRADE_IGNORE_DEPRECATED_POP
#endif

/**
@brief Wireframe 3D circle
@param segments  Number of segments. Must be greater or equal to @cpp 3 @ce.

Circle on the XY plane with radius @cpp 1.0f @ce, centered at origin.
Non-indexed @ref MeshPrimitive::LineLoop with @ref VertexFormat::Vector2
positions.

@image html primitives-circle3dwireframe.png width=256px

@see @ref circle2DSolid(), @ref circle3DWireframe(),
    @ref MeshTools::generateLineLoopIndices()
*/
MAGNUM_PRIMITIVES_EXPORT Trade::MeshData circle3DWireframe(UnsignedInt segments);

}}

#endif
