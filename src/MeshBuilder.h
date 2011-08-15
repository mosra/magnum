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

#include "Buffer.h"
#include "IndexedMesh.h"

namespace Magnum {

/**
@brief Mesh builder

Class for building meshes with triangle primitive from scratch or from
prefabricated data and modifying them (adding/removing faces, cleaning duplicate
vertices etc.).

Vertex template can be absolutely anything (single integer, structure, class)
but it must have at least operator== implemented.
*/
template<class Vertex> class MeshBuilder {
    public:
        /** @brief Triangle face */
        struct Face {
            /** @brief Implicit constructor */
            Face() {}

            /** @brief Constructor */
            Face(Vertex* first, Vertex* second, Vertex* third) {
                vertices[0] = first;
                vertices[1] = second;
                vertices[2] = third;
            }

            /** @brief Vertex data */
            Vertex* vertices[3];
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
         * Should be called after the mesh is built and the builder is not
         * needed. This function is also automatically called in destructor or
         * when calling setData().
         */
        void clear() {
            for(typename std::set<Vertex*>::iterator it = _vertices.begin(); it != _vertices.end(); ++it)
                delete *it;

            for(typename std::set<Face*>::iterator it = _faces.begin(); it != _faces.end(); ++it)
                delete *it;

            _vertices.clear();
            _faces.clear();
            vertexFaces.clear();
        }

        /** @brief Array with vertices */
        inline const std::set<Vertex*>& vertices() const { return _vertices; }

        /** @brief Array with fixed vertices */
        std::vector<Vertex> fixedVertices() const {
            std::vector<Vertex> vertices;
            for(typename std::set<Vertex*>::const_iterator it = _vertices.begin(); it != _vertices.end(); ++it)
                vertices.push_back(**it);

            return vertices;
        }

        /** @brief Array with faces */
        inline const std::set<Face*>& faces() const { return _faces; }

        /**
         * @brief Set mesh data
         * @param vertices      Vertices
         * @param indices       Vertex indices
         * @param vertexCount   Vertex count
         * @param indexCount    Index count
         *
         * Replaces mesh builder data with given data. Type of indices is
         * detected from given pointer.
         */
        inline void setData(const Vertex* vertices, const GLubyte* indices, GLsizei vertexCount, GLsizei indexCount) {
            setData<GLubyte>(vertices, indices, vertexCount, indexCount, GL_UNSIGNED_BYTE);
        }

        /** @copydoc setData() */
        inline void setData(const Vertex* vertices, const GLushort* indices, GLsizei vertexCount, GLsizei indexCount) {
            setData<GLushort>(vertices, indices, vertexCount, indexCount, GL_UNSIGNED_SHORT);
        }

        /** @copydoc setData() */
        inline void setData(const Vertex* vertices, const GLuint* indices, GLsizei vertexCount, GLsizei indexCount) {
            setData<GLuint>(vertices, indices, vertexCount, indexCount, GL_UNSIGNED_INT);
        }

        /**
         * @brief Remove face from the mesh
         * @param face      Pointer to face which to remove
         *
         * If face vertices are not shared with another face, they are removed
         * too.
         */
        void removeFace(Face* face) {
            /* Remove face from vertexFaces */
            for(int i = 0; i != 3; ++i) {
                typename std::multimap<Vertex*, Face*>::iterator it = vertexFaces.lower_bound(face->vertices[i]);
                typename std::multimap<Vertex*, Face*>::iterator end = vertexFaces.upper_bound(face->vertices[i]);
                for(; it != end; ++it) if(it->second == face) {
                    vertexFaces.erase(it);
                    break;
                }

                /* If the vertex is not part of any face anymore, remove it too */
                if(vertexFaces.count(face->vertices[i]) == 0) {
                    _vertices.erase(face->vertices[i]);
                    delete face->vertices[i];
                }
            }

            /* Remove face from faces and delete it */
            _faces.erase(face);
            delete face;
        }

        /**
         * @brief Add face
         * @param face      Pointer to face which to add
         */
        void addFace(Face* face) {
            /* Don't insert the same face twice */
            if(_faces.find(face) != _faces.end()) return;

            /* Add vertex to vertexData, if it is not yet there */
            for(int i = 0; i != 3; ++i)
                _vertices.insert(face->vertices[i]);

            /* Add vertices to vertexFaces */
            for(int i = 0; i != 3; ++i)
                vertexFaces.insert(std::pair<Vertex*, Face*>(face->vertices[i], face));

            /* Add face to face list */
            _faces.insert(face);
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
            /* Copy of current faces */
            std::set<Face*> f = faces();

            /* Subdivide each face to four new */
            for(typename std::set<Face*>::const_iterator face = f.begin(); face != f.end(); ++face) {
                /* Interpolate each side */
                Vertex* newVertices[3];
                for(int i = 0; i != 3; ++i)
                    newVertices[i] = new Vertex(interpolator(*(*face)->vertices[i], *(*face)->vertices[(i+1)%3]));

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
                addFace(new Face((*face)->vertices[0], newVertices[0], newVertices[2]));
                addFace(new Face(newVertices[0], (*face)->vertices[1], newVertices[1]));
                addFace(new Face(newVertices[2], newVertices[1], (*face)->vertices[2]));
                for(int i = 0; i != 3; ++i)
                    (*face)->vertices[i] = newVertices[i];
            }
        }

        /**
         * @brief Clean the mesh
         *
         * Removes duplicate vertices from the mesh.
         */
        void cleanMesh() {
            /* Vertices scheduled for deletion */
            std::vector<Vertex*> trashcan;

            /* Foreach all vertices and for each find similar in the rest of the array */
            for(typename std::set<Vertex*>::iterator it = _vertices.begin(); it != _vertices.end(); ++it) {
                typename std::set<Vertex*>::iterator next = it;
                for(typename std::set<Vertex*>::iterator similarIt = ++next; similarIt != _vertices.end(); ++similarIt) {
                    if(**it == **similarIt) {
                        /* Range of faces sharing that similar vertex */
                        typename std::multimap<Vertex*, Face*>::iterator begin = vertexFaces.lower_bound(*similarIt);
                        typename std::multimap<Vertex*, Face*>::iterator end = vertexFaces.upper_bound(*similarIt);

                        /* Updated array of faces, now sharing the original vertex */
                        std::multimap<Vertex*, Face*> updatedFaces;

                        /* Replace similar vertex in faces with this */
                        for(typename std::multimap<Vertex*, Face*>::iterator faceIt = begin; faceIt != end; ++faceIt) {
                            for(int i = 0; i != 3; ++i) if(*similarIt == faceIt->second->vertices[i]) {
                                faceIt->second->vertices[i] = *it;
                            }

                            updatedFaces.insert(std::pair<Vertex*, Face*>(*it, faceIt->second));
                        }

                        /* Remove old faces, insert updated */
                        vertexFaces.erase(begin, end);
                        vertexFaces.insert(updatedFaces.begin(), updatedFaces.end());

                        /* Schedule vertex for deletion */
                        trashcan.push_back(*similarIt);
                    }
                }
            }

            /* Delete all scheduled vertices */
            for(typename std::vector<Vertex*>::const_iterator it = trashcan.begin(); it != trashcan.end(); ++it) {
                _vertices.erase(*it);
                delete *it;
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
            if(_faces.size()*3 <= 0xFF)
                buildInternal<GLubyte>(mesh, vertexBuffer, vertexBufferUsage, indexBufferUsage, GL_UNSIGNED_BYTE);
            else if(_faces.size()*3 <= 0xFFFF)
                buildInternal<GLushort>(mesh, vertexBuffer, vertexBufferUsage, indexBufferUsage, GL_UNSIGNED_SHORT);
            else
                buildInternal<GLuint>(mesh, vertexBuffer, vertexBufferUsage, indexBufferUsage, GL_UNSIGNED_INT);
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
        std::set<Vertex*> _vertices;
        std::multimap<Vertex*, Face*> vertexFaces;
        std::set<Face*> _faces;

        template<class IndexType> void setData(const Vertex* vertexData, const IndexType* indices, GLsizei vertexCount, GLsizei indexCount, GLenum indexType) {
            clear();

            /* Map vertex indices to vertex pointers */
            std::map<IndexType, Vertex*> vertexMapping;
            for(IndexType i = 0; i != vertexCount; ++i) {
                Vertex* v = new Vertex(vertexData[i]);
                _vertices.insert(v);
                vertexMapping.insert(std::pair<IndexType, Vertex*>(i, v));
            }

            /* Faces array */
            for(IndexType i = 0; i < indexCount; i += 3) {
                Face* f = new Face;
                for(int ii = 0; ii != 3; ++ii) {
                    f->vertices[ii] = vertexMapping[indices[i+ii]];
                    vertexFaces.insert(std::pair<Vertex*, Face*>(f->vertices[ii], f));
                }
                _faces.insert(f);
            }
        }

        template<class IndexType> void buildInternal(IndexedMesh* mesh, Buffer* vertexBuffer, Buffer::Usage vertexBufferUsage, Buffer::Usage indexBufferUsage, GLenum indexType) {
            /* Update the mesh parameters */
            mesh->setPrimitive(Mesh::Triangles);
            mesh->setVertexCount(_vertices.size());
            mesh->setIndexCount(_faces.size()*3);
            mesh->setIndexType(indexType);

            /* Convert vertex pointers to fixed vector and map vertex data
               pointers to vertex indices */
            std::vector<Vertex> vertices;
            std::map<Vertex*, IndexType> indicesMapping;
            IndexType i = 0;
            for(typename std::set<Vertex*>::const_iterator it = _vertices.begin(); it != _vertices.end(); ++it) {
                vertices.push_back(**it);
                indicesMapping.insert(std::pair<Vertex*, IndexType>(*it, i++));
            }

            /* Create indices array */
            std::vector<IndexType> indices;
            indices.reserve(_faces.size()*3);
            for(typename std::set<Face*>::const_iterator it = _faces.begin(); it != _faces.end(); ++it) {
                for(int i = 0; i != 3; ++i)
                    indices.push_back(indicesMapping[(*it)->vertices[i]]);
            }

            /* Create mesh */
            vertexBuffer->setData(sizeof(Vertex)*vertices.size(), vertices.data(), vertexBufferUsage);
            mesh->indexBuffer()->setData(sizeof(IndexType)*indices.size(), indices.data(), indexBufferUsage);
        }
};

}

#endif
