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
@brief Base class for managing non-indexed meshes

VAOs are used for desktop OpenGL (not in OpenGL ES).
@requires_gl30 Extension @extension{APPLE,vertex_array_object}
@requires_gl30 Extension @extension{EXT,gpu_shader4} (for unsigned integer attributes)

@todo Support for normalized values (e.g. for color as char[4] passed to
     shader as floating-point vec4)
@todo Support for provoking vertex (OpenGL 3.2, @extension{ARB,provoking_vertex})
@todo Support for packed unsigned integer types for attributes (OpenGL 3.3, @extension{ARB,vertex_type_2_10_10_10_rev})
@todo Support for fixed precision type for attributes (OpenGL 4.1, @extension{ARB,ES2_compatibility})
@todo Support for double type for attributes (OpenGL 4.1, @extension{ARB,vertex_attrib_64bit})
@todo Support for indirect draw buffer (OpenGL 4.0, @extension{ARB,draw_indirect})
@todo Redo in a way that allows glMultiDrawArrays, glDrawArraysInstanced etc.
 */
class MAGNUM_EXPORT Mesh {
    Mesh(const Mesh& other) = delete;
    Mesh& operator=(const Mesh& other) = delete;

    public:
        /** @name Polygon drawing settings */

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Polygon mode
         *
         * @see setPolygonMode()
         * @requires_gl OpenGL ES behaves always like
         *      <tt>PolygonMode::%Fill</tt>. See setPrimitive() for possible
         *      workaround.
         */
        enum class PolygonMode: GLenum {
            /**
             * Interior of the polygon is filled (default).
             */
            Fill = GL_FILL,

            /**
             * Boundary edges are filled. See also setLineWidth().
             */
            Line = GL_LINE,

            /**
             * Starts of boundary edges are drawn as points. See also
             * setPointSize().
             */
            Point = GL_POINT
        };
        #endif

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Set polygon drawing mode
         *
         * Initial value is `PolygonMode::%Fill`.
         * @requires_gl OpenGL ES behaves always like the default. See
         *      setPrimitive() for possible workaround.
         */
        inline static void setPolygonMode(PolygonMode mode) {
            glPolygonMode(GL_FRONT_AND_BACK, static_cast<GLenum>(mode));
        }
        #endif

        /**
         * @brief Set line width
         *
         * Initial value is 1.
         */
        inline static void setLineWidth(GLfloat width) {
            glLineWidth(width);
        }

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Set point size
         *
         * Initial value is `1.0f`.
         * @see setProgramPointSize()
         * @requires_gl Set directly in vertex shader using @c gl_PointSize
         *      builtin variable.
         */
        inline static void setPointSize(GLfloat size) {
            glPointSize(size);
        }

        /**
         * @brief Enable/disable programmable point size
         *
         * If enabled, the point size is taken from vertex/geometry shader
         * builtin `gl_PointSize`. Initially disabled on desktop OpenGL.
         * @see setPointSize()
         * @requires_gl Always enabled on OpenGL ES.
         */
        inline static void setProgramPointSize(bool enabled) {
            enabled ? glEnable(GL_PROGRAM_POINT_SIZE) : glDisable(GL_PROGRAM_POINT_SIZE);
        }
        #endif

        /*@}*/

        /**
         * @brief Primitive type
         *
         * @see primitive(), setPrimitive()
         */
        enum class Primitive: GLenum {
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
         * @brief Buffer type
         *
         * If storing more than one attribute data in the buffer, the data of
         * one attribute can be either kept together or interleaved with data
         * for another attributes, so data for every vertex will be in one
         * continuous place.
         * @see addBuffer()
         */
        enum class BufferType: bool {
            Interleaved,    /**< Interleaved buffer */
            NonInterleaved  /**< Non-interleaved buffer */
        };

        /**
         * @brief Implicit constructor
         * @param primitive     Primitive type
         *
         * Allows creating the object without knowing anything about mesh
         * data. Note that you have to call setVertexCount() manually for mesh
         * to draw properly.
         */
        inline Mesh(Primitive primitive = Primitive::Triangles): _primitive(primitive), _vertexCount(0), finalized(false) {
            #ifndef MAGNUM_TARGET_GLES
            glGenVertexArrays(1, &vao);
            #endif
        }

        /**
         * @brief Constructor
         * @param primitive     Primitive type
         * @param vertexCount   Vertex count
         */
        inline Mesh(Primitive primitive, GLsizei vertexCount): _primitive(primitive), _vertexCount(vertexCount), finalized(false) {
            #ifndef MAGNUM_TARGET_GLES
            glGenVertexArrays(1, &vao);
            #endif
        }

        /** @brief Move constructor */
        Mesh(Mesh&& other);

        /**
         * @brief Destructor
         *
         * Deletes all associated buffers.
         */
        inline virtual ~Mesh() { destroy(); }

        /** @brief Move assignment */
        Mesh& operator=(Mesh&& other);

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
        }

        /**
         * @brief Add buffer
         * @param interleaved   Whether the buffer is interleaved
         *
         * Adds new buffer to the mesh. The buffer can be then filled with
         * Buffer::setData(). See also isInterleaved().
         *
         * @todo Move interleaveability to Buffer itself?
         */
        Buffer* addBuffer(BufferType interleaved);

        /**
         * @brief Whether given buffer is interleaved
         * @return True if the buffer belongs to the mesh and the buffer is
         *      interleaved, false otherwise.
         *
         * See also addBuffer().
         */
        inline bool isInterleaved(Buffer* buffer) const {
            auto found = _buffers.find(buffer);
            return found != _buffers.end() && found->second.first == BufferType::Interleaved;
        }

        /**
         * @brief Bind attribute
         * @tparam attribute    Attribute, defined in the shader
         * @param buffer        Buffer where bind the attribute to (pointer
         *      returned by addBuffer())
         *
         * Binds attribute of given type with given buffer. If the attribute is
         * already bound, given buffer isn't managed with this mesh (wasn't
         * initialized with addBuffer) or the mesh was already drawn, the
         * function does nothing.
         */
        template<class Attribute> inline void bindAttribute(Buffer* buffer) {
            bindAttribute(buffer, Attribute::Location, TypeTraits<typename Attribute::Type>::count(), TypeTraits<typename Attribute::Type>::type());
        }

        /**
         * @brief Draw the mesh
         *
         * Expects an active shader with all uniforms set.
         */
        virtual void draw();

    protected:
        #ifndef DOXYGEN_GENERATING_OUTPUT
        /** @brief Bind all buffers */
        void bindBuffers();

        /** @brief Bind vertex array or all buffers */
        void bind();

        /** @brief Unbind vertex array or all buffers */
        void unbind();

        /**
         * @brief Finalize the mesh
         *
         * Computes location and stride of each attribute in its buffer. After
         * this function is called, no new attribute can be bound.
         */
        MAGNUM_LOCAL void finalize();
        #endif

    private:
        /** @brief Vertex attribute */
        struct MAGNUM_LOCAL Attribute {
            GLuint attribute;           /**< @brief %Attribute ID */
            GLint size;                 /**< @brief How many items of `type` are in the attribute */
            Type type;                  /**< @brief %Attribute item type */
            GLsizei stride;             /**< @brief Distance of two adjacent attributes of this type in interleaved buffer */
            const GLvoid* pointer;      /**< @brief Pointer to first attribute of this type in the buffer */
        };

        #ifndef MAGNUM_TARGET_GLES
        GLuint vao;
        #endif
        Primitive _primitive;
        GLsizei _vertexCount;
        bool finalized;

        /**
         * @brief Buffers with their attributes
         *
         * Map of associated buffers, evey buffer has:
         * - boolean value which signalizes whether the buffer is interleaved
         * - list of bound attributes
         */
        std::map<Buffer*, std::pair<BufferType, std::vector<Attribute> > > _buffers;

        /**
         * @brief List of all bound attributes
         *
         * List of all bound attributes bound with bindAttribute().
         */
        std::set<GLuint> _attributes;

        MAGNUM_EXPORT void bindAttribute(Buffer* buffer, GLuint attribute, GLint size, Type type);

        void destroy();
};

}

#endif
