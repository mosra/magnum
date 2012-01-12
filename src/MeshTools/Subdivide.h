#ifndef Magnum_MeshTools_Subdivide_h
#define Magnum_MeshTools_Subdivide_h
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
 * @brief Class Magnum::MeshTools::Subdivide
 */

#include "AbstractTool.h"

namespace Magnum { namespace MeshTools {

/**
@brief %Mesh subdivisor implementation

See subdivide() for full documentation.
*/
template<class Vertex, class Interpolator> class Subdivide: public AbstractTool<Vertex> {
    public:
        /** @copydoc AbstractTool::AbstractTool() */
        inline Subdivide(MeshBuilder<Vertex>& builder): AbstractTool<Vertex>(builder) {}

        /**
         * @brief Functor
         *
         * See subdivide() for full documentation.
         */
        void run(Interpolator interpolator) {
            size_t indexCount = this->indices.size();
            this->indices.reserve(this->indices.size()*4);

            /* Subdivide each face to four new */
            for(size_t i = 0; i != indexCount; i += 3) {
                /* Interpolate each side */
                unsigned int newVertices[3];
                for(int j = 0; j != 3; ++j)
                    newVertices[j] = this->builder.addVertex(interpolator(this->vertices[this->indices[i+j]], this->vertices[this->indices[i+(j+1)%3]]));

                /*
                 * Add three new faces (0, 1, 3) and update original (2)
                 *
                 *                orig 0
                 *                /   \
                 *               /  0  \
                 *              /       \
                 *          new 0 ----- new 2
                 *          /   \       /  \
                 *         /  1  \  2  / 3  \
                 *        /       \   /      \
                 *   orig 1 ----- new 1 ---- orig 2
                 */
                this->builder.addFace(this->indices[i], newVertices[0], newVertices[2]);
                this->builder.addFace(newVertices[0], this->indices[i+1], newVertices[1]);
                this->builder.addFace(newVertices[2], newVertices[1], this->indices[i+2]);
                for(size_t j = 0; j != 3; ++j)
                    this->indices[i+j] = newVertices[j];
            }
        }
};

/**
@brief %Subdivide the mesh
@tparam Vertex          Vertex data type (the same as in MeshBuilder)
@tparam Interpolator    See @c interpolator function parameter
@param builder          %Mesh builder to operate on
@param interpolator     Functor or function pointer which interpolates
    two adjacent vertices: <tt>Vertex interpolator(Vertex a, Vertex b)</tt>

Goes through all triangle faces and subdivides them into four new. Cleaning
duplicate vertices in the mesh is up to user.

This is convenience function supplementing direct usage of Subdivide class,
instead of
@code
MeshBuilder<T> builder;
MeshTools::Subdivide<T, Interpolator>(builder).run(interpolator);
@endcode
you can just write
@code
MeshTools::subdivide(builder, interpolator);
@endcode
*/
template<class Vertex, class Interpolator> inline void subdivide(MeshBuilder<Vertex>& builder, Interpolator interpolator) {
    Subdivide<Vertex, Interpolator>(builder).run(interpolator);
}

}}

#endif
