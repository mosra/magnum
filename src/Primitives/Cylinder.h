#ifndef Magnum_Primitives_Cylinder_h
#define Magnum_Primitives_Cylinder_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class Magnum::Primitives::Cylinder
 */

#include <Containers/EnumSet.h>

#include "Magnum.h"
#include "Primitives/magnumPrimitivesVisibility.h"
#include "Trade/Trade.h"

namespace Magnum { namespace Primitives {

/**
@brief 3D cylinder primitive

%Cylinder along Y axis of radius `1`.
*/
class MAGNUM_PRIMITIVES_EXPORT Cylinder {
    public:
        /**
         * @brief %Flags
         *
         * @see Flags, Cylinder()
         */
        enum class Flag {
            GenerateTextureCoords = 1,  /**< @brief Generate texture coordinates */
            CapEnds                     /**< @brief Cap ends */
        };

        /** @brief %Flags */
        typedef Containers::EnumSet<Flag, int> Flags;

        /**
         * @brief Solid cylinder
         * @param rings         Number of (face) rings. Must be larger or
         *      equal to 1.
         * @param segments      Number of (face) segments. Must be larger or
         *      equal to 3.
         * @param halfLength    Half the cylinder length
         * @param flags         Flags
         *
         * Indexed @ref Mesh::Primitive "Triangles" with normals, optional 2D
         * texture coordinates and optional capped ends. If texture coordinates
         * are generated, vertices of one segment are duplicated for texture
         * wrapping.
         */
        static Trade::MeshData3D solid(UnsignedInt rings, UnsignedInt segments, Float halfLength, Flags flags = Flags());

        /**
         * @brief Wireframe cylinder
         * @param rings         Number of (line) rings. Must be larger or equal
         *      to 1.
         * @param segments      Number of (line) segments. Must be larger or
         *      equal to 4 and multiple of 4.
         * @param halfLength    Half the cylinder length
         *
         * Indexed @ref Mesh::Primitive "Lines".
         */
        static Trade::MeshData3D wireframe(UnsignedInt rings, UnsignedInt segments, Float halfLength);
};

CORRADE_ENUMSET_OPERATORS(Cylinder::Flags)

}}

#endif
