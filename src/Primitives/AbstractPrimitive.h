#ifndef Magnum_Primitives_AbstractPrimitive_h
#define Magnum_Primitives_AbstractPrimitive_h
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
 * @brief Class Magnum::Primitives::AbstractPrimitive
 */

#include "TypeTraits.h"

#include "IndexedMesh.h"

namespace Magnum {

class Buffer;
class IndexedMesh;

namespace Primitives {

/**
@brief Base class for primitives
@tparam _IndexType   Type used for indices. Only unsigned integral types are
    allowed, as TypeTraits is used to check whether given type can be used.
*/
template<class _IndexType> class AbstractPrimitive {
    public:
        /** @brief Type used for indices */
        typedef typename TypeTraits<_IndexType>::IndexType IndexType;

        /** @brief Destructor */
        inline virtual ~AbstractPrimitive() {}

        /** @brief Primitive type */
        virtual Mesh::Primitive primitive() const = 0;

        /** @brief Count of vertices */
        virtual size_t vertexCount() const = 0;

        /** @brief Count of indices */
        virtual size_t indexCount() const = 0;

        /**
         * @brief Build the primitive
         * @param mesh          Destination mesh
         * @param vertexBuffer  Vertex buffer where to put the data
         *
         * Prepares the mesh and fills the vertex and index buffer with the
         * data.
         * @note Subclasses should call prepareMesh() at the beginning of their
         * implementation, unless the mesh is prepared another way (e.g. using
         * MeshBuilder).
         */
        virtual void build(IndexedMesh* mesh, Buffer* vertexBuffer) = 0;

    protected:
        /**
         * @brief Prepare the mesh for primitive data
         * @param mesh          Mesh to prepare
         *
         * Sets primitive type, vertex count and index count and type on the
         * mesh using primitive(), vertexCount(), indexCount() and IndexType.
         */
        void prepareMesh(IndexedMesh* mesh) const {
            mesh->setPrimitive(primitive());
            mesh->setVertexCount(vertexCount());
            mesh->setIndexCount(indexCount());
            mesh->setIndexType(TypeTraits<IndexType>::glType());
        }
};

}}

#endif
