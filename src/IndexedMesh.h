#ifndef Magnum_IndexedMesh_h
#define Magnum_IndexedMesh_h
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
 * @brief Class Magnum::IndexedMesh
 */

#include "Mesh.h"
#include "Buffer.h"

namespace Magnum {

/**
 * @brief Indexed mesh
 */
class MAGNUM_EXPORT IndexedMesh: public Mesh {
    public:
        /**
         * @brief Implicit constructor
         * @param primitive     Primitive type
         *
         * Allows creating the object without knowing anything about mesh data.
         * Note that you have to call setVertexCount(), setIndexCount() and
         * setIndexType() manually for mesh to draw properly.
         */
        inline IndexedMesh(Primitive primitive = Primitive::Triangles): Mesh(primitive), _indexBuffer(Buffer::Target::ElementArray), _indexCount(0), _indexType(Type::UnsignedShort) {}

        /**
         * @brief Constructor
         * @param primitive     Primitive type
         * @param vertexCount   Count of unique vertices
         * @param indexCount    Count of indices
         * @param indexType     Type of indices (indexable, see TypeTraits)
         */
        inline IndexedMesh(Primitive primitive, GLsizei vertexCount, GLsizei indexCount, Type indexType = Type::UnsignedShort): Mesh(primitive, vertexCount), _indexBuffer(Buffer::Target::ElementArray), _indexCount(indexCount), _indexType(indexType) {}

        /** @brief Index count */
        inline GLsizei indexCount() const { return _indexCount; }

        /**
         * @brief Set index count
         * @return Pointer to self (for method chaining)
         *
         * @see MeshTools::compressIndices()
         * @todo definalize after that?
         */
        inline IndexedMesh* setIndexCount(GLsizei count) {
            _indexCount = count;
            return this;
        }

        /** @brief Index type */
        inline Type indexType() const { return _indexType; }

        /**
         * @brief Set index type
         * @return Pointer to self (for method chaining)
         */
        inline IndexedMesh* setIndexType(Type type) {
            _indexType = type;
            return this;
        }

        /**
         * @brief Index buffer
         *
         * Returns pointer to index buffer, which should be then filled with
         * indices (of type specified in constructor).
         */
        inline Buffer* indexBuffer() { return &_indexBuffer; }

        /**
         * @brief Draw the mesh
         *
         * Expects an active shader with all uniforms set.
         * @see Buffer::bind(), bind(), unbind(), finalize(), @fn_gl{DrawElements}
         */
        void draw();

    protected:
        /**
         * @brief Finalize the mesh
         *
         * @see Mesh::finalize(), Buffer::bind()
         */
        MAGNUM_LOCAL void finalize();

    private:
        Buffer _indexBuffer;
        GLsizei _indexCount;
        Type _indexType;
};

}

#endif
