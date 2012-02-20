#ifndef Magnum_Primitives_Icosphere_h
#define Magnum_Primitives_Icosphere_h
/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class Magnum::Primitives::Icosahedron, Magnum::Primitives::Icosphere
 */

#include "AbstractPrimitive.h"
#include "SizeTraits.h"
#include "MeshTools/Subdivide.h"
#include "MeshTools/Clean.h"

namespace Magnum { namespace Primitives {

/**
@brief Icosahedron

@todo Use own computed (and more precise) icosahedron data, not these stolen
from Blender.
*/
class Icosahedron {
    public:
        static const Vector4 vertices[];    /**< @brief Vertices */
        static const GLubyte indices[];     /**< @brief Indices */
};

/**
 * @brief %Icosphere primitive
 * @tparam subdivisions     Number of subdivisions
 */
template<size_t subdivisions> class Icosphere: public AbstractPrimitive<typename SizeTraits<Log<256, Pow<4, subdivisions>::value*20*3>::value>::SizeType> {
    public:
        Icosphere() {
            if(vertexCount() == 0) subdivide();
        }

        inline Mesh::Primitive primitive() const { return Mesh::Triangles; }
        inline size_t vertexCount() const { return builder()->vertexCount(); }
        inline size_t indexCount() const { return builder()->indexCount(); }

        inline void build(IndexedMesh* mesh, Buffer* vertexBuffer) {
            /* mesh is prepared by the builder, no need to call prepareMesh */
            builder()->build(mesh, vertexBuffer, Buffer::Usage::StaticDraw, Buffer::Usage::StaticDraw);
        }

    private:
        static MeshBuilder<Vector4>* builder() {
            static MeshBuilder<Vector4>* _builder = nullptr;
            if(!_builder) _builder = new MeshBuilder<Vector4>();
            return _builder;
        }

        static void subdivide() {
            builder()->setData(Icosahedron::vertices, Icosahedron::indices, 12, 60);

            for(size_t i = 0; i != subdivisions; ++i)
                MeshTools::subdivide(*builder(), [](const Vector4& a, const Vector4& b) {
                    return (a+b).xyz().normalized();
                });

            if(subdivisions) MeshTools::clean(*builder());
        }
};

}}

#endif
