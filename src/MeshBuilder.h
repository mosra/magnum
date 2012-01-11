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

/**
@brief Mesh builder
@tparam Vertex      Vertex data

Class for building meshes with triangle primitive from scratch or from
prefabricated data and modifying them (adding/removing faces, cleaning duplicate
vertices etc.).

@todo Make it more generic for meshes with texture coordinates etc.
*/
template<class Vertex> class MeshBuilder {
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
         * @brief Subdivide mesh
         * @param interpolator  Functor or function pointer which interpolates
         *      two adjacent vertices: <tt>Vertex interpolator(Vertex a, Vertex
         *      b)</tt>
         *
         * Goes through all triangle faces and subdivides them in four new.
         * Cleaning the mesh is up to user.
         */
        template<class Interpolator> void subdivide(Interpolator interpolator) {
            size_t indexCount = _indices.size();
            _indices.reserve(_indices.size()*4);

            /* Subdivide each face to four new */
            for(size_t i = 0; i != indexCount; i += 3) {
                /* Interpolate each side */
                unsigned int newVertices[3];
                for(int j = 0; j != 3; ++j)
                    newVertices[j] = addVertex(interpolator(_vertices[_indices[i+j]], _vertices[_indices[i+(j+1)%3]]));

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
                addFace(_indices[i], newVertices[0], newVertices[2]);
                addFace(newVertices[0], _indices[i+1], newVertices[1]);
                addFace(newVertices[2], newVertices[1], _indices[i+2]);
                for(size_t j = 0; j != 3; ++j)
                    _indices[i+j] = newVertices[j];
            }
        }

        /**
         * @brief Clean the mesh
         * @param epsilon       Epsilon value, vertices nearer than this
         *      distance will be melt together.
         *
         * Removes duplicate vertices from the mesh. With template parameter
         * @c vertexSize you can specify how many initial vertex fields are
         * important (for example, when dealing with perspective in 3D space,
         * only first three fields of otherwise 4D vertex are important).
         */
        template<size_t vertexSize = Vertex::Size> void cleanMesh(typename Vertex::Type epsilon = EPSILON) {
            if(_indices.empty()) return;

            /* Get mesh bounds */
            Vertex min, max;
            for(size_t i = 0; i != Vertex::Size; ++i) {
                min[i] = std::numeric_limits<typename Vertex::Type>::max();
                max[i] = std::numeric_limits<typename Vertex::Type>::min();
            }
            for(auto it = _vertices.cbegin(); it != _vertices.cend(); ++it)
                for(size_t i = 0; i != vertexSize; ++i)
                    if((*it)[i] < min[i])
                        min[i] = (*it)[i];
                    else if((*it)[i] > max[i])
                        max[i] = (*it)[i];

            /* Make epsilon so large that size_t can index all vertices inside
               mesh bounds. */
            Vertex size = max-min;
            for(size_t i = 0; i != Vertex::Size; ++i)
                if(static_cast<typename Vertex::Type>(size[i]/std::numeric_limits<size_t>::max()) > epsilon)
                    epsilon = static_cast<typename Vertex::Type>(size[i]/std::numeric_limits<size_t>::max());

            /* First go with original vertex coordinates, then move them by
               epsilon/2 in each direction. */
            Vertex moved;
            for(size_t moving = 0; moving <= vertexSize; ++moving) {

                /* Under each index is pointer to face which contains given vertex
                and index of vertex in the face. */
                std::unordered_map<Math::Vector<size_t, vertexSize>, HashedVertex, IndexHash<vertexSize>> table;

                /* Reserve space for all vertices */
                table.reserve(_vertices.size());

                /* Go through all faces' vertices */
                for(auto it = _indices.begin(); it != _indices.end(); ++it) {
                    /* Index of a vertex in vertexSize-dimensional table */
                    size_t index[vertexSize];
                    for(size_t ii = 0; ii != vertexSize; ++ii)
                        index[ii] = (_vertices[*it][ii]+moved[ii]-min[ii])/epsilon;

                    /* Try inserting the vertex into table, if it already
                        exists, change vertex pointer of the face to already
                        existing vertex */
                    HashedVertex v(*it, table.size());
                    auto result = table.insert(std::pair<Math::Vector<size_t, vertexSize>, HashedVertex>(index, v));
                    *it = result.first->second.newIndex;
                }

                /* Shrink vertices array */
                std::vector<Vertex> vertices(table.size());
                for(auto it = table.cbegin(); it != table.cend(); ++it)
                    vertices[it->second.newIndex] = _vertices[it->second.oldIndex];
                std::swap(vertices, _vertices);

                /* Move vertex coordinates by epsilon/2 in next direction */
                if(moving != Vertex::Size) {
                    moved = Vertex();
                    moved[moving] = epsilon/2;
                }
            }
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

        template<size_t vertexSize> class IndexHash {
            public:
                inline size_t operator()(const Math::Vector<size_t, vertexSize>& data) const {
                    size_t a = 0;
                    for(size_t i = 0; i != vertexSize; ++i)
                        a ^= data[i];
                    return a;
                }
        };

        struct HashedVertex {
            unsigned int oldIndex, newIndex;

            HashedVertex(unsigned int oldIndex, unsigned int newIndex): oldIndex(oldIndex), newIndex(newIndex) {}
        };

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
