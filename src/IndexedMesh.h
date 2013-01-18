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

#include "corradeCompatibility.h"

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

Example usage -- creating a cube mesh, assigning vertex buffer with
interleaved vertex attributes and compressed index buffer for use with
Shaders::PhongShader:
@code
Buffer *vertexBuffer, *indexBuffer;
Mesh* mesh;

Primitives::Cube cube;
MeshTools::interleave(mesh, vertexBuffer, Buffer::Usage::StaticDraw, *cube.positions(0), *cube.normals(0));
MeshTools::compressIndices(mesh, indexBuffer, Buffer::Usage::StaticDraw, *cube.indices());
mesh->setPrimitive(plane.primitive())
    ->addInterleavedVertexBuffer(vertexBuffer, 0, Shaders::PhongShader::Position(), Shaders::PhongShader::Normal());
@endcode

@section IndexedMesh-drawing Rendering meshes

From user point-of-view the operation is the same as for
@ref Mesh-drawing "non-indexed meshes".

@section IndexedMesh-performance-optimization Performance optimizations

If @extension{APPLE,vertex_array_object}, OpenGL ES 3.0 or
@es_extension{OES,vertex_array_object} on OpenGL ES 2.0 is supported, next to
@ref Mesh-performance-optimization "optimizations in Mesh itself" the index
buffer is bound on object construction instead of binding it in every draw()
call.
*/
class MAGNUM_EXPORT IndexedMesh: public Mesh {
    friend class Context;

    public:
        /**
         * @brief Index type
         *
         * @see setIndexType(), indexSize()
         */
        enum class IndexType: GLenum {
            UnsignedByte = GL_UNSIGNED_BYTE,    /**< Unsigned byte */
            UnsignedShort = GL_UNSIGNED_SHORT,  /**< Unsigned short */

            /**
             * Unsigned int
             * @requires_gles30 %Extension @es_extension{OES,element_index_uint}
             */
            UnsignedInt = GL_UNSIGNED_INT
        };

        /** @brief Size of given index type */
        static std::size_t indexSize(IndexType type);

        /**
         * @brief Constructor
         * @param primitive     Primitive type
         *
         * Creates indexed mesh with zero vertex count, zero index count and
         * no vertex or index buffers.
         */
        inline explicit IndexedMesh(Primitive primitive = Primitive::Triangles): Mesh(primitive), _indexBuffer(nullptr), _indexCount(0), _indexType(IndexType::UnsignedInt) {}

        /**
         * @brief Set index buffer
         *
         * @see setIndexCount(), setIndexType(), MeshTools::compressIndices(),
         *      @fn_gl{BindVertexArray}, @fn_gl{BindBuffer} (if
         *      @extension{APPLE,vertex_array_object} is available)
         */
        IndexedMesh* setIndexBuffer(Buffer* buffer);

        /** @brief Index count */
        inline GLsizei indexCount() const { return _indexCount; }

        /**
         * @brief Set index count
         * @return Pointer to self (for method chaining)
         *
         * Default is zero.
         * @see setIndexBuffer(), setIndexType(), MeshTools::compressIndices()
         */
        inline IndexedMesh* setIndexCount(GLsizei count) {
            _indexCount = count;
            return this;
        }

        /** @brief Index type */
        inline IndexType indexType() const { return _indexType; }

        /**
         * @brief Set index type
         * @return Pointer to self (for method chaining)
         *
         * Default is @ref IndexType "IndexType::UnsignedInt".
         * @see setIndexBuffer(), setIndexCount(), MeshTools::compressIndices()
         */
        inline IndexedMesh* setIndexType(IndexType type) {
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
        void draw() override;

        /* Overloads to remove WTF-factor from method chaining order */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        inline IndexedMesh* setPrimitive(Primitive primitive) {
            Mesh::setPrimitive(primitive);
            return this;
        }
        inline IndexedMesh* setVertexCount(GLsizei vertexCount) {
            Mesh::setVertexCount(vertexCount);
            return this;
        }
        template<class ...T> inline IndexedMesh* addVertexBuffer(Buffer* buffer, const T&... attributes) {
            Mesh::addVertexBuffer(buffer, attributes...);
            return this;
        }
        template<class ...T> inline IndexedMesh* addInterleavedVertexBuffer(Buffer* buffer, GLintptr offset, const T&... attributes) {
            Mesh::addInterleavedVertexBuffer(buffer, offset, attributes...);
            return this;
        }
        template<GLuint location, class T> inline IndexedMesh* addVertexBufferStride(Buffer* buffer, GLintptr offset, GLsizei stride, const AbstractShaderProgram::Attribute<location, T>& attribute) {
            Mesh::addVertexBufferStride(buffer, offset, stride, attribute);
            return this;
        }
        #endif

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
        IndexType _indexType;
};

/** @debugoperator{Magnum::IndexedMesh} */
Debug MAGNUM_EXPORT operator<<(Debug debug, IndexedMesh::IndexType value);

}

namespace Corrade { namespace Utility {

/** @configurationvalue{Magnum::IndexedMesh} */
template<> struct MAGNUM_EXPORT ConfigurationValue<Magnum::IndexedMesh::IndexType> {
    ConfigurationValue() = delete;

    /**
     * @brief Write enum value as string
     *
     * If the value is invalid, returns empty string.
     */
    static std::string toString(Magnum::IndexedMesh::IndexType value, ConfigurationValueFlags);

    /**
     * @brief Read enum value as string
     *
     * If the value is invalid, returns @ref Magnum::IndexedMesh::IndexType "IndexedMesh::IndexType::UnsignedInt".
     */
    static Magnum::IndexedMesh::IndexType fromString(const std::string& stringValue, ConfigurationValueFlags);
};

}}

#endif
