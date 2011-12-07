#ifndef Magnum_Primitives_Cube_h
#define Magnum_Primitives_Cube_h
/*
    Copyright © 2010, 2011 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

/** @file
 * @brief Class Magnum::Primitives::Cube
 */

#include "AbstractPrimitive.h"

namespace Magnum { namespace Primitives {

/** @brief Cube primitive */
class Cube: public AbstractPrimitive<GLubyte> {
    public:
        inline Mesh::Primitive primitive() const { return Mesh::Triangles; }
        inline size_t vertexCount() const { return 8; }
        inline size_t indexCount() const { return 36; }

        void build(IndexedMesh* mesh, Buffer* vertexBuffer);

    private:
        static const Vector4 _vertices[];
        static const IndexType _indices[];
};

}}

#endif
