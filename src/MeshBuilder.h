#ifndef Magnum_MeshBuilder_h
#define Magnum_MeshBuilder_h
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
        typedef size_t VertexPointer;   /**< @brief Type for indexing vertices */

        /** @brief Triangle face */
        struct Face {
            /** @brief Implicit constructor */
            Face() {}

            /** @brief Constructor */
            Face(VertexPointer first, VertexPointer second, VertexPointer third) {
                vertices[0] = first;
                vertices[1] = second;
                vertices[2] = third;
            }

            /** @brief Vertex data */
            VertexPointer vertices[3];

            /** @brief Comparison operator */
            bool operator==(const Face& other) const {
                return other.vertices[0] == vertices[0] &&
                       other.vertices[1] == vertices[1] &&
                       other.vertices[2] == vertices[2];
            }
        };

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
            _faces.clear();
        }

        /** @brief Vertex count */
        inline size_t vertexCount() const { return _vertices.size(); }

        /** @brief Array with vertices */
        inline const std::vector<Vertex>& vertices() const { return _vertices; }
        inline std::vector<Vertex>& vertices() { return _vertices; } /**< @copydoc vertices() const */

        /** @brief Face count */
        inline size_t faceCount() const { return _faces.size(); }

        /** @brief Array with faces */
        inline const std::vector<Face>& faces() const { return _faces; }
        inline std::vector<Face>& faces() { return _faces; } /**< @copydoc faces() const */

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
        template<class IndexType> void setData(const Vertex* vertexData, const IndexType* indices, GLsizei vertexCount, GLsizei indexCount) {
            clear();

            /* Map vertex indices to vertex pointers */
            std::vector<Vertex> vertices;
            vertices.reserve(vertexCount);
            /* Using TypeTraits::IndexType to ensure we have allowed type for indexing */
            for(typename TypeTraits<IndexType>::IndexType i = 0; i != vertexCount; ++i)
                addVertex(Vertex(vertexData[i]));

            /* Faces array */
            _faces.reserve(indexCount/3);
            for(size_t i = 0; i < static_cast<size_t>(indexCount); i += 3)
                addFace(indices[i], indices[i+1], indices[i+2]);
        }

        /** @brief Add vertex */
        inline VertexPointer addVertex(const Vertex& v) {
            _vertices.push_back(v);
            return _vertices.size()-1;
        }

        /** @brief Add face */
        inline void addFace(const Face& face) {
            _faces.push_back(face);
        }

        /** @brief Add face */
        inline void addFace(VertexPointer first, VertexPointer second, VertexPointer third) {
            _faces.push_back(Face(first, second, third));
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
            size_t faceCount = _faces.size();
            _faces.reserve(_faces.size()*4);

            /* Subdivide each face to four new */
            for(size_t i = 0; i != faceCount; ++i) {
                /* Interpolate each side */
                VertexPointer newVertices[3];
                for(int j = 0; j != 3; ++j)
                    newVertices[j] = addVertex(interpolator(_vertices[_faces[i].vertices[j]], _vertices[_faces[i].vertices[(j+1)%3]]));

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
                addFace(_faces[i].vertices[0], newVertices[0], newVertices[2]);
                addFace(newVertices[0], _faces[i].vertices[1], newVertices[1]);
                addFace(newVertices[2], newVertices[1], _faces[i].vertices[2]);
                for(size_t j = 0; j != 3; ++j)
                    _faces[i].vertices[j] = newVertices[j];
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
            if(_faces.empty()) return;

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
                for(auto it = _faces.begin(); it != _faces.end(); ++it) {
                    for(size_t i = 0; i != 3; ++i) {

                        /* Index of a vertex in vertexSize-dimensional table */
                        size_t index[vertexSize];
                        for(size_t ii = 0; ii != vertexSize; ++ii)
                            index[ii] = (_vertices[it->vertices[i]][ii]+moved[ii]-min[ii])/epsilon;

                        /* Try inserting the vertex into table, if it already
                           exists, change vertex pointer of the face to already
                           existing vertex */
                        HashedVertex v(it->vertices[i], table.size());
                        auto result = table.insert(std::pair<Math::Vector<size_t, vertexSize>, HashedVertex>(index, v));
                        it->vertices[i] = result.first->second.newVertexPointer;
                    }
                }

                /* Shrink vertices array */
                std::vector<Vertex> vertices(table.size());
                for(auto it = table.cbegin(); it != table.cend(); ++it)
                    vertices[it->second.newVertexPointer] = _vertices[it->second.oldVertexPointer];
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
        inline void build(IndexedMesh* mesh, Buffer* vertexBuffer, Buffer::Usage vertexBufferUsage, Buffer::Usage indexBufferUsage) {
            SizeBasedCall<InternalBuilder>(_vertices.size())(this, mesh, vertexBuffer, vertexBufferUsage, indexBufferUsage);
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
        std::vector<Face> _faces;
        std::vector<Vertex> _vertices;

        typedef size_t FacePointer;

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
            VertexPointer oldVertexPointer, newVertexPointer;

            HashedVertex(VertexPointer oldVertexPointer, VertexPointer newVertexPointer): oldVertexPointer(oldVertexPointer), newVertexPointer(newVertexPointer) {}
        };

        struct InternalBuilder {
            template<class IndexType> static void run(MeshBuilder<Vertex>* builder, IndexedMesh* mesh, Buffer* vertexBuffer, Buffer::Usage vertexBufferUsage, Buffer::Usage indexBufferUsage) {
                /* Compress face array to index array. Using TypeTraits::IndexType
                to ensure we have allowed type for indexing */
                std::vector<typename TypeTraits<IndexType>::IndexType> indices;
                indices.reserve(builder->_faces.size()*3);
                for(auto it = builder->_faces.cbegin(); it != builder->_faces.cend(); ++it) {
                    indices.push_back(it->vertices[0]);
                    indices.push_back(it->vertices[1]);
                    indices.push_back(it->vertices[2]);
                }

                /* Update mesh parameters and fill it with data */
                mesh->setPrimitive(Mesh::Triangles);
                mesh->setVertexCount(builder->_vertices.size());
                mesh->setIndexCount(indices.size());
                mesh->setIndexType(TypeTraits<IndexType>::glType());

                vertexBuffer->setData(sizeof(Vertex)*builder->_vertices.size(), builder->_vertices.data(), vertexBufferUsage);
                mesh->indexBuffer()->setData(sizeof(IndexType)*indices.size(), indices.data(), indexBufferUsage);
            }
        };
};

}

#endif
