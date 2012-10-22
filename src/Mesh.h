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

#include <vector>

#include "AbstractShaderProgram.h"
#include "TypeTraits.h"

namespace Magnum {

class Buffer;
class Context;

/**
@brief Base class for managing non-indexed meshes

@section Mesh-configuration Mesh configuration

To properly configure mesh, you have to set primitive either in constructor or
using setPrimitive() and call setVertexCount(). Then create vertex buffers,
fill them with vertex data and assign them to mesh and given shader locations
using addVertexBuffer() or addInterleavedVertexBuffer(). You can also use
MeshTools::interleave() to conveniently set vertex count and buffer data.

Note that the buffer is not managed (e.g. deleted on destruction) by the mesh,
so you have to manage it on your own. On the other hand it allows you to use
one buffer for more meshes (each mesh for example configured for different
shader) or store more than only vertex data in one buffer.

Example usage -- filling buffer with position data, configuring the mesh and
assigning the buffer to mesh to use with custom shader:
@code
Buffer* buffer;
Mesh* mesh;

static constexpr Point3D positions[30] = {
    // ...
};
buffer->setData(positions, Buffer::Usage::StaticDraw);

mesh->setPrimitve(Mesh::Primitive::Triangles)
    ->setVertexCount(30)
    ->addVertexBuffer(buffer, MyShader::Position);
@endcode

Example usage -- creating a plane mesh and assigning buffer with interleaved
vertex attributes for use with phong shader:
@code
Buffer* buffer;
Mesh* mesh;

Primitives::Plane plane;
MeshTools::interleave(mesh, buffer, Buffer::Usage::StaticDraw, *plane.positions(0), *plane.normals(0));
mesh->setPrimitive(plane.primitive())
    ->addInterleavedVertexBuffer(buffer, 0, Shaders::PhongShader::Position, Shaders::PhongShader::Normal);
@endcode

@section Mesh-drawing Rendering meshes

Basic workflow is to set up respective shader (see @ref AbstractShaderProgram-rendering-workflow "AbstractShaderProgram documentation" for more infromation) and call Mesh::draw().

@section Mesh-performance-optimization Performance optimizations

If @extension{APPLE,vertex_array_object} is supported, VAOs are used instead
of binding the buffers and specifying vertex attribute pointers in each
draw() call. The engine tracks currently bound VAO to avoid unnecessary calls
to @fn_gl{BindVertexArray}.

@requires_gl30 Extension @extension{EXT,gpu_shader4} (for unsigned integer attributes)

@todo Support for normalized values (e.g. for color as char[4] passed to
     shader as floating-point vec4)
@todo Support for packed unsigned integer types for attributes (OpenGL 3.3, @extension{ARB,vertex_type_2_10_10_10_rev})
@todo Support for fixed precision type for attributes (OpenGL 4.1, @extension{ARB,ES2_compatibility})
@todo Support for double type for attributes (OpenGL 4.1, @extension{ARB,vertex_attrib_64bit})
@todo Support for indirect draw buffer (OpenGL 4.0, @extension{ARB,draw_indirect})
@todo Redo in a way that allows glMultiDrawArrays, glDrawArraysInstanced etc.
 */
class MAGNUM_EXPORT Mesh {
    friend class IndexedMesh;
    friend class Context;

    Mesh(const Mesh& other) = delete;
    Mesh& operator=(const Mesh& other) = delete;

    public:
        /** @name Polygon drawing settings */

        /**
         * @brief Front facing polygon winding
         *
         * @see setFrontFace()
         */
        enum FrontFace: GLenum {
            /** @brief Counterclockwise polygons are front facing (default). */
            CounterClockWise = GL_CCW,

            /** @brief Clockwise polygons are front facing. */
            ClockWise = GL_CW
        };

        /**
         * @brief Set front-facing polygon winding
         *
         * Initial value is `FrontFace::%CounterClockWise`.
         * @see @fn_gl{FrontFace}
         */
        inline static void setFrontFace(FrontFace mode) {
            glFrontFace(static_cast<GLenum>(mode));
        }

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Provoking vertex
         *
         * @see setProvokingVertex()
         * @requires_gl OpenGL ES behaves always like
         *      <tt>ProvokingMode::%LastVertexConvention</tt>.
         * @requires_gl32 Extension @extension{ARB,provoking_vertex}. Older
         *      versions behave always like
         *      <tt>ProvokingMode::%LastVertexConvention</tt>.
         */
        enum class ProvokingVertex: GLenum {
            /** @brief Use first vertex of each polygon. */
            FirstVertexConvention = GL_FIRST_VERTEX_CONVENTION,

            /** @brief Use last vertex of each polygon (default). */
            LastVertexConvention = GL_LAST_VERTEX_CONVENTION
        };

        /**
         * @brief Set provoking vertex
         *
         * Initial value is <tt>ProvokingMode::%LastVertexConvention</tt>.
         * @see @fn_gl{ProvokingVertex}
         * @requires_gl OpenGL ES behaves always like the default.
         * @requires_gl32 Extension @extension{ARB,provoking_vertex}. Older
         *      versions behave always like the default.
         */
        inline static void setProvokingVertex(ProvokingVertex mode) {
            glProvokingVertex(static_cast<GLenum>(mode));
        }
        #endif

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
         * @see @fn_gl{PolygonMode}
         * @requires_gl OpenGL ES behaves always like the default. See
         *      setPrimitive() for possible workaround.
         */
        inline static void setPolygonMode(PolygonMode mode) {
            glPolygonMode(GL_FRONT_AND_BACK, static_cast<GLenum>(mode));
        }
        #endif

        /**
         * @brief Mode affected by polygon offset
         *
         * @see setPolygonOffsetMode(), setPolygonOffset()
         */
        enum class PolygonOffsetMode: GLenum {
            /** Offset filled polygons. */
            Fill = GL_POLYGON_OFFSET_FILL

            #ifndef MAGNUM_TARGET_GLES
            ,

            /**
             * Offset lines.
             * @requires_gl Only <tt>PolygonOffset::%Fill</tt> is supported.
             */
            Line = GL_POLYGON_OFFSET_LINE,

            /**
             * Offset points.
             * @requires_gl Only <tt>PolygonOffset::%Fill</tt> is supported.
             */
            Point = GL_POLYGON_OFFSET_POINT
            #endif
        };

        /**
         * @brief Enable/disable polygon offset for given mode
         *
         * Initially disabled for all modes.
         * @see setPolygonOffset(), @fn_gl{Enable}/@fn_gl{Disable}
         */
        inline static void setPolygonOffsetMode(PolygonOffsetMode mode, bool enabled) {
            enabled ? glEnable(static_cast<GLenum>(mode)) : glDisable(static_cast<GLenum>(mode));
        }

        /**
         * @brief Set polygon offset
         * @param factor    Scale factor
         * @param units     Offset units
         *
         * @attention You have to call setPolygonOffsetMode() to enable
         *      polygon offset for desired polygon modes.
         * @see @fn_gl{PolygonOffset}
         */
        inline static void setPolygonOffset(GLfloat factor, GLfloat units) {
            glPolygonOffset(factor, units);
        }

        /**
         * @brief Set line width
         *
         * Initial value is `1.0f`.
         * @see @fn_gl{LineWidth}
         */
        inline static void setLineWidth(GLfloat width) {
            glLineWidth(width);
        }

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Set point size
         *
         * Initial value is `1.0f`.
         * @see setProgramPointSize(), @fn_gl{PointSize}
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
         * @see setPointSize(), @fn_gl{Enable}/@fn_gl{Disable} with @def_gl{PROGRAM_POINT_SIZE}
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
         * @brief Constructor
         * @param primitive     Primitive type
         *
         * Creates mesh with no vertex buffers and zero vertex count.
         * @see setPrimitive(), setVertexCount(), @fn_gl{GenVertexArrays} (if
         *      @extension{APPLE,vertex_array_object} is available)
         */
        inline Mesh(Primitive primitive = Primitive::Triangles): _primitive(primitive), _vertexCount(0) {
            (this->*createImplementation)();
        }

        /** @brief Move constructor */
        Mesh(Mesh&& other);

        /**
         * @brief Destructor
         *
         * @see @fn_gl{DeleteVertexArrays} (if
         *      @extension{APPLE,vertex_array_object} is available)
         */
        virtual ~Mesh();

        /** @brief Move assignment */
        Mesh& operator=(Mesh&& other);

        /** @brief Primitive type */
        inline Primitive primitive() const { return _primitive; }

        /**
         * @brief Set primitive type
         * @return Pointer to self (for method chaining)
         */
        inline Mesh* setPrimitive(Primitive primitive) {
            _primitive = primitive;
            return this;
        }

        /** @brief Vertex count */
        inline GLsizei vertexCount() const { return _vertexCount; }

        /**
         * @brief Set vertex count
         * @return Pointer to self (for method chaining)
         *
         * @attention All bound attributes are reset after calling this
         *      function, so you must call
         *      addVertexBuffer()/addInterleavedVertexBuffer() afterwards.
         * @see MeshTools::interleave()
         */
        inline Mesh* setVertexCount(GLsizei vertexCount) {
            _vertexCount = vertexCount;
            attributes.clear();
            return this;
        }

        /**
         * @brief Add buffer with non-interleaved vertex attributes for use with given shader
         *
         * Attribute list is combination of attribute definitions (specified
         * in implementation of given shader) and offsets between attribute
         * arrays.
         *
         * See @ref Mesh-configuration "class documentation" for simple usage
         * example. For more involved example imagine that you have buffer
         * with 35 bytes of some other data at the beginning (possibly material
         * configuration), then position array, then texture coordinate array
         * and then normal array. You want to draw it with Shaders::PhongShader,
         * but it accepts only position and normal, so you have to skip the
         * texture coordinate array:
         * @code
         * Mesh* mesh;
         * Buffer* buffer;
         * mesh->addVertexBuffer(buffer,
         *     35,                                  // skip other data
         *     Shaders::PhongShader::Position,      // position array
         *     sizeof(Vector2)*mesh->vertexCount(), // skip texture coordinate array
         *     Shaders::PhongShader::Normal);       // normal array
         * @endcode
         *
         * Vou can also achieve the same effect by calling this function more
         * times with absolute offsets:
         * @code
         * mesh->addVertexBuffer(buffer, 35, Shaders::PhongShader::Position);
         *     ->addVertexBuffer(buffer, 35 + (sizeof(Shaders::PhongShader::Position::Type) + sizeof(Vector2))*
         *          mesh->vertexCount(), Shaders::PhongShader::Normal);
         * @endcode
         *
         * @attention Non-zero vertex count must be set before calling this
         *      function.
         * @attention The buffer passed as parameter is not managed by the
         *      mesh, you must ensure it will exist for whole lifetime of the
         *      mesh and delete it afterwards.
         *
         * @see addInterleavedVertexBuffer(), @fn_gl{BindVertexArray},
         *      @fn_gl{EnableVertexAttribArray}, @fn_gl{BindBuffer},
         *      @fn_gl{VertexAttribPointer} (if
         *      @extension{APPLE,vertex_array_object} is available)
         */
        template<class ...T> inline Mesh* addVertexBuffer(Buffer* buffer, const T&... attributes) {
            addVertexBufferInternal(buffer, 0, attributes...);
            return this;
        }

        /**
         * @brief Add buffer with interleaved vertex attributes for use with given shader
         *
         * Parameter @p offset is offset of the interleaved array from the
         * beginning, attribute list is combination of attribute definitions
         * (specified in implementation of given shader) and offsets between
         * attributes.
         *
         * See @ref Mesh-configuration "class documentation" for simple usage
         * example. For more involved example imagine that you have buffer
         * with 35 bytes of some other data at the beginning (possibly material
         * configuration) and then the interleaved vertex array. Each vertex
         * consists of weight, position, texture coordinate and normal. You
         * want to draw it with Shaders::PhongShader, but it accepts only
         * position and normal, so you have to skip weight and texture
         * coordinate in each vertex:
         * @code
         * Mesh* mesh;
         * Buffer* buffer;
         * mesh->addInterleavedVertexBuffer(buffer,
         *     35,                              // skip other data
         *     sizeof(GLfloat),                 // skip vertex weight
         *     Shaders::PhongShader::Position,  // vertex position
         *     sizeof(Vector2),                 // skip texture coordinates
         *     Shaders::PhongShader::Normal);   // vertex normal
         * @endcode
         *
         * You can also achieve the same effect by calling addVertexBufferStride()
         * more times with absolute offset from the beginning and stride
         * between vertex attributes:
         * @code
         * GLsizei stride =                     // size of one vertex
         *     sizeof(GLfloat) +
         *     sizeof(Shaders::PhongShader::Position::Type) +
         *     sizeof(Vector2) +
         *     sizeof(Shaders::PhongShader::Normal::Type);
         *
         * mesh->addVertexBufferStride(buffer, 35 + sizeof(GLfloat),
         *          stride, Shaders::PhongShader::Position);
         *     ->addVertexBufferStride(buffer, 35 + sizeof(GLfloat) +
         *          sizeof(Shaders::PhongShader::Position::Type) + sizeof(Vector2),
         *          stride, Shaders::PhongShader::Normal);
         * @endcode
         *
         * @attention Non-zero vertex count must be set before calling this
         *      function.
         * @attention The buffer passed as parameter is not managed by the
         *      mesh, you must ensure it will exist for whole lifetime of the
         *      mesh and delete it afterwards.
         *
         * @see addVertexBufferStride(), addVertexBuffer(),
         *      @fn_gl{BindVertexArray}, @fn_gl{EnableVertexAttribArray},
         *      @fn_gl{BindBuffer}, @fn_gl{VertexAttribPointer} (if
         *      @extension{APPLE,vertex_array_object} is available)
         */
        template<class ...T> inline Mesh* addInterleavedVertexBuffer(Buffer* buffer, GLintptr offset, const T&... attributes) {
            addInterleavedVertexBufferInternal(buffer, offset, strideOfInterleaved(attributes...), attributes...);
            return this;
        }

        /**
         * @brief Add buffer with interleaved vertex attributes for use with given shader
         *
         * See addInterleavedVertexBuffer() for more information.
         */
        template<GLuint location, class T> inline Mesh* addVertexBufferStride(Buffer* buffer, GLintptr offset, GLsizei stride, const AbstractShaderProgram::Attribute<location, T>& attribute) {
            addInterleavedVertexBufferInternal(buffer, offset, stride, attribute);
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
         *      is available), @fn_gl{DrawArrays}
         */
        virtual void draw();

    private:
        struct MAGNUM_LOCAL Attribute {
            Buffer* buffer;
            GLuint location;
            GLint count;
            Type type;
            GLintptr offset;
            GLsizei stride;
        };

        static void MAGNUM_LOCAL initializeContextBasedFunctionality(Context* context);

        /* Adding non-interleaved vertex attributes */
        template<GLuint location, class T, class ...U> inline void addVertexBufferInternal(Buffer* buffer, GLintptr offset, const AbstractShaderProgram::Attribute<location, T>&, const U&... attributes) {
            addVertexAttribute(buffer, location, TypeTraits<T>::count(), TypeTraits<T>::type(), offset, 0);

            /* Add size of this attribute array to offset for next attribute */
            addVertexBufferInternal(buffer, offset+TypeTraits<T>::count()*TypeTraits<T>::size()*_vertexCount, attributes...);
        }
        template<class ...T> inline void addVertexBufferInternal(Buffer* buffer, GLintptr offset, GLintptr gap, const T&... attributes) {
            /* Add the gap to offset for next attribute */
            addVertexBufferInternal(buffer, offset+gap, attributes...);
        }
        inline void addVertexBufferInternal(Buffer*, GLintptr) {}

        /* Computing stride of interleaved vertex attributes */
        template<GLuint location, class T, class ...U> inline static GLsizei strideOfInterleaved(const AbstractShaderProgram::Attribute<location, T>&, const U&... attributes) {
            return TypeTraits<T>::count()*TypeTraits<T>::size() + strideOfInterleaved(attributes...);
        }
        template<class ...T> inline static GLsizei strideOfInterleaved(GLintptr gap, const T&... attributes) {
            return gap + strideOfInterleaved(attributes...);
        }
        inline static GLsizei strideOfInterleaved() { return 0; }

        /* Adding interleaved vertex attributes */
        template<GLuint location, class T, class ...U> inline void addInterleavedVertexBufferInternal(Buffer* buffer, GLintptr offset, GLsizei stride, const AbstractShaderProgram::Attribute<location, T>&, const U&... attributes) {
            addVertexAttribute(buffer, location, TypeTraits<T>::count(), TypeTraits<T>::type(), offset, stride);

            /* Add size of this attribute to offset for next attribute */
            addInterleavedVertexBufferInternal(buffer, offset+TypeTraits<T>::count()*TypeTraits<T>::size(), stride, attributes...);
        }
        template<class ...T> inline void addInterleavedVertexBufferInternal(Buffer* buffer, GLintptr offset, GLsizei stride, GLintptr gap, const T&... attributes) {
            /* Add the gap to offset for next attribute */
            addInterleavedVertexBufferInternal(buffer, offset+gap, stride, attributes...);
        }
        inline void addInterleavedVertexBufferInternal(Buffer*, GLsizei, GLintptr) {}

        void MAGNUM_EXPORT addVertexAttribute(Buffer* buffer, GLuint location, GLint count, Type type, GLintptr offset, GLsizei stride);

        static void MAGNUM_LOCAL bindVAO(GLuint vao);

        void MAGNUM_LOCAL bind();

        inline void unbind() {
            (this->*unbindImplementation)();
        }

        void MAGNUM_LOCAL vertexAttribPointer(const Attribute& attribute);

        typedef void(Mesh::*CreateImplementation)();
        void MAGNUM_LOCAL createImplementationDefault();
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL createImplementationVAO();
        #endif
        static CreateImplementation createImplementation;

        typedef void(Mesh::*DestroyImplementation)();
        void MAGNUM_LOCAL destroyImplementationDefault();
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL destroyImplementationVAO();
        #endif
        static MAGNUM_LOCAL DestroyImplementation destroyImplementation;

        typedef void(Mesh::*BindAttributeImplementation)(const Attribute&);
        void MAGNUM_LOCAL bindAttributeImplementationDefault(const Attribute& attribute);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL bindAttributeImplementationVAO(const Attribute& attribute);
        #endif
        static MAGNUM_LOCAL BindAttributeImplementation bindAttributeImplementation;

        typedef void(Mesh::*BindImplementation)();
        void MAGNUM_LOCAL bindImplementationDefault();
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL bindImplementationVAO();
        #endif
        static MAGNUM_LOCAL BindImplementation bindImplementation;

        typedef void(Mesh::*UnbindImplementation)();
        void MAGNUM_LOCAL unbindImplementationDefault();
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL unbindImplementationVAO();
        #endif
        static MAGNUM_LOCAL UnbindImplementation unbindImplementation;

        GLuint vao;
        Primitive _primitive;
        GLsizei _vertexCount;

        std::vector<Attribute> attributes;
};

}

#endif
