#ifndef Magnum_Primitives_Plane_h
#define Magnum_Primitives_Plane_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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
 * @brief Class @ref Magnum::Primitives::Plane
 */

#include "Magnum/Trade/Trade.h"
#include "Magnum/Primitives/visibility.h"

namespace Magnum { namespace Primitives {

/**
@brief 3D plane primitive

2x2 plane.
@see @ref Square
*/
class MAGNUM_PRIMITIVES_EXPORT Plane {
    public:
        /** @brief Whether to generate texture coordinates */
        enum class TextureCoords: UnsignedByte {
            DontGenerate,    /**< Don't generate texture coordinates */

            /** Generate texture coordinates with origin in bottom left corner. */
            Generate
        };

        /**
         * @brief Solid plane
         *
         * Non-indexed @ref MeshPrimitive::TriangleStrip with normals in
         * positive Z direction.
         */
        static Trade::MeshData3D solid(TextureCoords textureCoords = TextureCoords::DontGenerate);

        /**
         * @brief Wireframe plane
         *
         * Non-indexed @ref MeshPrimitive::LineLoop.
         */
        static Trade::MeshData3D wireframe();

        Plane() = delete;
};

}}

#endif
