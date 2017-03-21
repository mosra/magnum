#ifndef Magnum_Primitives_UVSphere_h
#define Magnum_Primitives_UVSphere_h
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
 * @brief Class @ref Magnum::Primitives::UVSphere
 */

#include "Magnum/Trade/Trade.h"
#include "Magnum/Primitives/visibility.h"

namespace Magnum { namespace Primitives {

/**
@brief 3D UV sphere primitive

Sphere with radius `1`.
*/
class MAGNUM_PRIMITIVES_EXPORT UVSphere {
    public:
        /** @brief Whether to generate texture coordinates */
        enum class TextureCoords: UnsignedByte {
            Generate,       /**< Generate texture coordinates */
            DontGenerate    /**< Don't generate texture coordinates */
        };

        /**
         * @brief Solid UV sphere
         * @param rings         Number of (face) rings. Must be larger or equal
         *      to 2.
         * @param segments      Number of (face) segments. Must be larger or
         *      equal to 3.
         * @param textureCoords Whether to generate texture coordinates.
         *
         * Indexed @ref MeshPrimitive::Triangles with normals and optional 2D
         * texture coordinates. If texture coordinates are generated, vertices
         * of one segment are duplicated for texture wrapping.
         */
        static Trade::MeshData3D solid(UnsignedInt rings, UnsignedInt segments, TextureCoords textureCoords = TextureCoords::DontGenerate);

        /**
         * @brief Wireframe UV sphere
         * @param rings         Number of (line) rings. Must be larger or equal
         *      to 2 and multiple of 2.
         * @param segments      Number of (line) segments. Must be larger or
         *      equal to 4 and multiple of 4.
         *
         * Indexed @ref MeshPrimitive::Lines.
         */
        static Trade::MeshData3D wireframe(UnsignedInt rings, UnsignedInt segments);
};

}}

#endif
