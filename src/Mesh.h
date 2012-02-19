#ifndef Magnum_Mesh_h
#define Magnum_Mesh_h
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
 * @brief Class Magnum::Mesh
 */

#include <map>
#include <vector>
#include <set>

#include "Magnum.h"
#include "TypeTraits.h"

namespace Magnum {

class Buffer;

/**
 * @brief Base class for managing non-indexed meshes
 *
 * @todo Support for normalized values (e.g. for color as char[4] passed to
 *      shader as floating-point vec4)
 */
class MAGNUM_EXPORT Mesh {
    Mesh(const Mesh& other) = delete;
    Mesh(Mesh&& other) = delete;
    Mesh& operator=(const Mesh& other) = delete;
    Mesh& operator=(Mesh&& other) = delete;

    public:
        /** @brief Primitive type */
        enum Primitive {
            /**
             * Single points
             */
            Points = GL_POINTS,

            /**
             * Each pair of vertices defines a single line, lines aren't
             * connected together.
             */
            Lines = GL_LINES,

            /**
             * Polyline
             */
            LineStrip = GL_LINE_STRIP,

            /**
             * Polyline, last vertex is connected to first.
             */
            LineLoop = GL_LINE_LOOP,

            /**
             * Each three vertices define one triangle.
             */
            Triangles = GL_TRIANGLES,

            /**
             * First three vertices define first triangle, each following
             * vertex defines another triangle.
             */
            TriangleStrip = GL_TRIANGLE_STRIP,

            /**
             * First vertex is center, each following vertex is connected to
             * previous and center vertex.
             */
            TriangleFan = GL_TRIANGLE_FAN
        };

        /**
         * @brief Implicit constructor
         *
         * Allows creating the object without knowing anything about mesh data.
         * Note that you have to call setPrimitive() and setVertexCount()
         * manually for mesh to draw properly.
         */
        inline Mesh(): _primitive(Triangles), _vertexCount(0), finalized(false) {}

        /**
         * @brief Constructor
         * @param primitive     Primitive type
         * @param vertexCount   Vertex count
         */
        inline Mesh(Primitive primitive, GLsizei vertexCount): _primitive(primitive), _vertexCount(vertexCount), finalized(false) {}

        /**
         * @brief Destructor
         *
         * Deletes all associated buffers.
         */
        virtual ~Mesh();

        /**
         * @brief Whether the mesh is finalized
         *
         * When the mesh is finalized, no new attributes can be bound.
         */
        inline bool isFinalized() const { return finalized; }

        /** @brief Primitive type */
        inline Primitive primitive() const { return _primitive; }

        /** @brief Set primitive type */
        inline void setPrimitive(Primitive primitive) { _primitive = primitive; }

        /** @brief Vertex count */
        inline GLsizei vertexCount() const { return _vertexCount; }

        /**
         * @brief Set vertex count
         *
         * This forces recalculation of attribute positions upon next drawing.
         */
        inline void setVertexCount(GLsizei vertexCount) {
            _vertexCount = vertexCount;
            finalized = false;
        };

        /**
         * @brief Add buffer
         * @param interleaved   If storing more than one attribute data in the
         *      buffer, the data of one attribute can be either kept together
         *      or interleaved with data for another attributes, so data for
         *      every vertex will be in one continuous place.
         *
         * Adds new buffer to the mesh. The buffer can be then filled with
         * Buffer::setData().
         */
        Buffer* addBuffer(bool interleaved);

        /**
         * @brief Bind attribute
         * @param buffer        Buffer where bind the attribute to (pointer
         *      returned by addBuffer())
         * @param attribute     Attribute
         *
         * Binds attribute of given type with given buffer. If the attribute is
         * already bound, given buffer isn't managed with this mesh (wasn't
         * initialized with addBuffer) or the mesh was already drawn, the
         * function does nothing.
         */
        template<class T> inline void bindAttribute(Buffer* buffer, GLuint attribute) {
            bindAttribute(buffer, attribute, TypeTraits<T>::count(), TypeTraits<T>::glType());
        }

        /**
         * @brief Draw the mesh
         *
         * Binds attributes to buffers and draws the mesh. Expects an active
         * shader with all uniforms set.
         */
        virtual void draw();

    protected:
        /** @brief Vertex attribute */
        struct Attribute {
            GLuint attribute;           /**< @brief Attribute ID */
            GLint size;                 /**< @brief How many items of @c type are in the attribute */
            Type type;                  /**< @brief Attribute item type */
            GLsizei stride;             /**< @brief Distance of two adjacent attributes of this type in interleaved buffer */
            const GLvoid* pointer;      /**< @brief Pointer to first attribute of this type in the buffer */
        };

        /**
         * @brief Buffers with their attributes
         * @return Map of associated buffers, evey buffer has:
         * - boolean value which signalizes whether the buffer is interleaved
         * - list of bound attributes
         */
        inline const std::map<Buffer*, std::pair<bool, std::vector<Attribute> > >& buffers() { return _buffers; }

        /**
         * @brief List of all bound attributes
         *
         * List of all bound attributes bound with bindAttribute().
         */
        inline const std::set<GLuint>& attributes() { return _attributes; }

        /**
         * @brief Finalize the mesh
         *
         * Computes location and stride of each attribute in its buffer. After
         * this function is called, no new attribute can be bound.
         */
        void finalize();

        /**
         * @brief Set the mesh dirty
         *
         * Sets the attribute locations in buffers as dirty, so they are
         * recalculated on next drawing.
         */
        inline void setDirty() { finalized = false; }

    private:
        Primitive _primitive;
        GLsizei _vertexCount;
        bool finalized;

        std::map<Buffer*, std::pair<bool, std::vector<Attribute> > > _buffers;
        std::set<GLuint> _attributes;

        void bindAttribute(Buffer* buffer, GLuint attribute, GLint size, Type type);
};

}

#endif
