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

namespace Magnum {

/**
@brief Indexed mesh

@section IndexedMesh-configuration Indexed mesh configuration

Next to @ref Mesh-configuration "everything needed for non-indexed mesh" you
have to call also setIndexCount() and setIndexType(). Then create index buffer
and assign it to the mesh using setIndexBuffer() or use
MeshTools::compressIndices() to conveniently fill the index buffer and set
index count and type.

Similarly as in Mesh itself the index buffer is not managed by the mesh, so
you have to manage it on your own. On the other hand it allows you to use
one index buffer for more meshes (with different vertex data in each mesh, for
example) or store more than only index data in one buffer.

@section IndexedMesh-drawing Rendering meshes

From user point-of-view the operation is the same as for
@ref Mesh-drawing "non-indexed meshes".

@section IndexedMesh-performance-optimization Performance optimizations

If @extension{APPLE,vertex_array_object} is supported, next to
@ref Mesh-performance-optimization "optimizations in Mesh itself" the index
buffer is bound on object construction instead of in every draw() call.
*/
class MAGNUM_EXPORT IndexedMesh: public Mesh {
    friend class Context;

    public:
        /**
         * @brief Constructor
         * @param primitive     Primitive type
         *
         * Creates indexed mesh with no index buffer, zero vertex count and
         * zero index count.
         * @see setPrimitive(), setVertexCount(), setIndexBuffer(),
         *      setIndexCount(), setIndexType()
         */
        inline IndexedMesh(Primitive primitive = Primitive::Triangles): Mesh(primitive), _indexBuffer(nullptr), _indexCount(0), _indexType(Type::UnsignedShort) {}

        /**
         * @brief Set index buffer
         *
         * @see MeshTools::compressIndices(), @fn_gl{BindVertexArray},
         *      @fn_gl{BindBuffer} (if @extension{APPLE,vertex_array_object}
         *      is available)
         */
        IndexedMesh* setIndexBuffer(Buffer* buffer);

        /** @brief Index count */
        inline GLsizei indexCount() const { return _indexCount; }

        /**
         * @brief Set index count
         * @return Pointer to self (for method chaining)
         *
         * @see MeshTools::compressIndices()
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
         *
         * @see MeshTools::compressIndices()
         */
        inline IndexedMesh* setIndexType(Type type) {
            _indexType = type;
            return this;
        }

        /**
         * @brief Draw the mesh
         *
         * Expects an active shader with all uniforms set. See
         * @ref AbstractShaderProgram-rendering-workflow "AbstractShaderProgram documentation"
         * for more information.
         * @see @fn_gl{EnableVertexAttribArray}, @fn_gl{BindBuffer},
         *      @fn_gl{VertexAttribPointer}, @fn_gl{DisableVertexAttribArray}
         *      or @fn_gl{BindVertexArray} (if @extension{APPLE,vertex_array_object}
         *      is available), @fn_gl{DrawElements}
         */
        void draw();

    private:
        static void MAGNUM_LOCAL initializeContextBasedFunctionality(Context* context);

        void MAGNUM_LOCAL bind();

        typedef void(IndexedMesh::*BindIndexBufferImplementation)();
        void MAGNUM_LOCAL bindIndexBufferImplementationDefault();
        void MAGNUM_LOCAL bindIndexBufferImplementationVAO();
        static MAGNUM_LOCAL BindIndexBufferImplementation bindIndexBufferImplementation;

        typedef void(IndexedMesh::*BindIndexedImplementation)();
        void MAGNUM_LOCAL bindIndexedImplementationDefault();
        void MAGNUM_LOCAL bindIndexedImplementationVAO();
        static MAGNUM_LOCAL BindIndexedImplementation bindIndexedImplementation;

        Buffer* _indexBuffer;
        GLsizei _indexCount;
        Type _indexType;
};

}

#endif
