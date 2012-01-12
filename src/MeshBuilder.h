#ifndef Magnum_MeshBuilder_h
#define Magnum_MeshBuilder_h
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
 * @brief Class Magnum::MeshBuilder
 */

#include <set>
#include <map>
#include <limits>
#include <unordered_map>

#include "Buffer.h"
#include "IndexedMesh.h"
#include "SizeTraits.h"

namespace Magnum {

namespace MeshTools {
    template<class Vertex> class AbstractTool;
}

/**
@brief Mesh builder
@tparam Vertex      Vertex data

Class for building meshes with triangle primitive from scratch or from
prefabricated data and modifying them using MeshBuilder alone or tools from
MeshTools namespace.

@todo Make it more generic for meshes with texture coordinates etc.
*/
template<class Vertex> class MeshBuilder {
    friend class MeshTools::AbstractTool<Vertex>;

    public:

        /**
         * @brief Destructor
         *
         * Removed all vertices and faces.
         */
        inline virtual ~MeshBuilder() { clear(); }

        /**
         * @brief Clear mesh data
         *
         * The data are cleared automatically in destructor and when calling
         * setData(). If you don't want to use the data after building the mesh,
         * destroy the MeshBuilder or call this function.
         */
        void clear() {
            _vertices.clear();
            _indices.clear();
        }

        /** @brief Vertex count */
        inline unsigned int vertexCount() const { return _vertices.size(); }

        /** @brief Array with vertices */
        inline const std::vector<Vertex>& vertices() const { return _vertices; }

        /** @brief Index count */
        inline unsigned int indexCount() const { return _indices.size(); }

        /** @brief Array with indices */
        inline const std::vector<unsigned int>& indices() const { return _indices; }

        /**
         * @brief Set mesh data
         * @param vertexData    Vertex data
         * @param indices       Vertex indices
         * @param vertexCount   Vertex count
         * @param indexCount    Index count
         *
         * Replaces mesh builder data with given data. Type of indices is
         * detected from given pointer.
         */
        template<class IndexType> void setData(const Vertex* vertexData, const IndexType* indices, unsigned int vertexCount, unsigned int indexCount) {
            clear();

            /* Vertex array */
            std::vector<Vertex> vertices;
            vertices.reserve(vertexCount);
            /* Using TypeTraits::IndexType to ensure we have allowed type for indexing */
            for(typename TypeTraits<IndexType>::IndexType i = 0; i != vertexCount; ++i)
                addVertex(Vertex(vertexData[i]));

            /* Index array */
            _indices.reserve(indexCount);
            for(unsigned int i = 0; i != indexCount; ++i)
                _indices.push_back(indices[i]);
        }

        /** @brief Add vertex */
        inline unsigned int addVertex(const Vertex& v) {
            _vertices.push_back(v);
            return _vertices.size()-1;
        }

        /** @brief Add face */
        inline void addFace(unsigned int first, unsigned int second, unsigned int third) {
            _indices.push_back(first);
            _indices.push_back(second);
            _indices.push_back(third);
        }

        /**
         * @brief Build indexed mesh and fill existing buffers with it
         * @param mesh                  Mesh. The mesh primitive is set to
         *      Mesh::Triangles, if it is not already, vertex and index count
         *      is updated to values from the builder.
         * @param vertexBuffer          Vertex buffer created as interleaved
         *      with Mesh::addBuffer(). Otherwise the behaviour is undefined.
         * @param vertexBufferUsage     Usage of the vertex buffer.
         * @param indexBufferUsage      Usage of the index buffer.
         *
         * Builds indexed mesh from the data and fills given mesh buffers with
         * them.
         * @note The mesh is @b not cleaned before building.
         */
        void build(IndexedMesh* mesh, Buffer* vertexBuffer, Buffer::Usage vertexBufferUsage, Buffer::Usage indexBufferUsage) {
            mesh->setPrimitive(Mesh::Triangles);
            mesh->setVertexCount(_vertices.size());
            vertexBuffer->setData(sizeof(Vertex)*_vertices.size(), _vertices.data(), vertexBufferUsage);
            SizeBasedCall<IndexBuilder>(_vertices.size())(mesh, _indices, indexBufferUsage);
        }

        /**
         * @brief Build indexed data and create new mesh from them
         * @param vertexBufferUsage     Usage of the vertex buffer.
         * @param indexBufferUsage      Usage of the index buffer.
         *
         * @see build(IndexedMesh*, Buffer*, Buffer::Usage, Buffer::Usage)
         */
        IndexedMesh* build(Buffer::Usage vertexBufferUsage, Buffer::Usage indexBufferUsage) {
            IndexedMesh mesh = new IndexedMesh(Mesh::Triangles, 0, 0, GL_UNSIGNED_BYTE);
            Buffer* vertexBuffer = mesh.addBuffer(true);

            build(mesh, vertexBuffer, vertexBufferUsage, indexBufferUsage);
            return mesh;
        }

    private:
        std::vector<unsigned int> _indices;
        std::vector<Vertex> _vertices;

        struct IndexBuilder {
            template<class IndexType> static void run(IndexedMesh* mesh, const std::vector<unsigned int>& _indices, Buffer::Usage indexBufferUsage) {
                /* Compress face array to index array. Using
                   TypeTraits::IndexType to ensure we have allowed type for
                   indexing */
                std::vector<typename TypeTraits<IndexType>::IndexType> indices;
                indices.reserve(_indices.size());
                for(auto it = _indices.cbegin(); it != _indices.cend(); ++it)
                    indices.push_back(*it);

                /* Update mesh parameters and fill index buffer */
                mesh->setIndexCount(indices.size());
                mesh->setIndexType(TypeTraits<IndexType>::glType());
                mesh->indexBuffer()->setData(sizeof(IndexType)*indices.size(), indices.data(), indexBufferUsage);
            }
        };
};

}

#endif
