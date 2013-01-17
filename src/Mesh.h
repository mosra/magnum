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
#include <Utility/Assert.h>

#include "AbstractShaderProgram.h"
#include "TypeTraits.h"

namespace Magnum {

/**
@brief Non-indexed mesh

@section Mesh-configuration Mesh configuration

To properly configure mesh, you have to set primitive either in constructor or
using setPrimitive() and call setVertexCount(). Then create vertex buffers and
fill them with vertex data. You can also use MeshTools::interleave() to
conveniently set vertex count and buffer data. At last assign them to mesh and
@ref AbstractShaderProgram::Attribute "shader attributes" using
addVertexBuffer(), addInterleavedVertexBuffer() or addVertexBufferStride().

Note that the buffer is not managed (e.g. deleted on destruction) by the mesh,
so you have to manage it on your own. On the other hand it allows you to use
one buffer for more meshes (each mesh for example configured for different
shader) or store more than only vertex data in one buffer.

Example usage -- filling buffer with position data, configuring the mesh and
assigning the buffer to mesh to use with custom shader:
@code
class MyShader: public AbstractShaderProgram {
    public:
        typedef Attribute<0, Point3D> Position;

    // ...
};
Buffer* buffer;
Mesh* mesh;

static constexpr Point3D positions[30] = {
    // ...
};
buffer->setData(positions, Buffer::Usage::StaticDraw);

mesh->setPrimitive(Mesh::Primitive::Triangles)
    ->setVertexCount(30)
    ->addVertexBuffer(buffer, MyShader::Position());
@endcode

Example usage -- creating a plane mesh and assigning buffer with interleaved
vertex attributes for use with Shaders::PhongShader:
@code
Buffer* buffer;
Mesh* mesh;

Primitives::Plane plane;
MeshTools::interleave(mesh, buffer, Buffer::Usage::StaticDraw, *plane.positions(0), *plane.normals(0));
mesh->setPrimitive(plane.primitive())
    ->addInterleavedVertexBuffer(buffer, 0, Shaders::PhongShader::Position(), Shaders::PhongShader::Normal());
@endcode

Example usage -- passing color attribute as normalized unsigned byte with BGRA
component ordering (e.g. directly from @ref Trade::TgaImporter "TGA file"):
@code
class MyShader: public AbstractShaderProgram {
    public:
        typedef Attribute<1, Color4<>> Color;

    // ...
};
Buffer* buffer;
Mesh* mesh;

mesh->addVertexBuffer(buffer, MyShader::Color(Type::UsignedByte, MyShader::Color::Normalized|MyShader::Color::BGRA));
@endcode

@section Mesh-drawing Rendering meshes

Basic workflow is: bind specific framebuffer for drawing (if needed), set up
respective shader and bind required textures (see
@ref AbstractShaderProgram-rendering-workflow "AbstractShaderProgram documentation"
for more infromation) and call Mesh::draw().

@section Mesh-performance-optimization Performance optimizations

If @extension{APPLE,vertex_array_object}, OpenGL ES 3.0 or
@es_extension{OES,vertex_array_object} on OpenGL ES 2.0 is supported, VAOs are
used instead of binding the buffers and specifying vertex attribute pointers
in each draw() call. The engine tracks currently bound VAO to avoid
unnecessary calls to @fn_gl{BindVertexArray}.

If extension @extension{EXT,direct_state_access} and VAOs are available,
DSA functions are used for specifying attribute locations to avoid unnecessary
calls to @fn_gl{BindBuffer} and @fn_gl{BindVertexArray}. See documentation of
addVertexBuffer(), addInterleavedVertexBuffer(), addVertexBufferStride() for
more information.

@see IndexedMesh
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
         * @requires_gl32 %Extension @extension{ARB,provoking_vertex}. Older
         *      versions behave always like
         *      <tt>ProvokingMode::%LastVertexConvention</tt>.
         * @requires_gl OpenGL ES behaves always like
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
         * @requires_gl32 %Extension @extension{ARB,provoking_vertex}. Older
         *      versions behave always like the default.
         * @requires_gl OpenGL ES behaves always like the default.
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
        inline explicit Mesh(Primitive primitive = Primitive::Triangles): _primitive(primitive), _vertexCount(0) {
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
         *
         * Default is @ref Primitive "Primitive::Triangles".
         * @see setVertexCount(), addVertexBuffer(),
         *      addInterleavedVertexBuffer(), addVertexBufferStride()
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
         * Default is zero.
         * @see setPrimitive(), addVertexBuffer(), addInterleavedVertexBuffer(),
         *      addVertexBufferStride(), MeshTools::interleave()
         */
        Mesh* setVertexCount(GLsizei vertexCount);

        /**
         * @brief Add buffer with non-interleaved vertex attributes for use with given shader
         * @return Pointer to self (for method chaining)
         *
         * Attribute list is combination of
         * @ref AbstractShaderProgram::Attribute "attribute definitions"
         * (specified in implementation of given shader) and offsets between
         * attribute arrays.
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
         *     Shaders::PhongShader::Position(),    // position array
         *     sizeof(Vector2)*mesh->vertexCount(), // skip texture coordinate array
         *     Shaders::PhongShader::Normal());     // normal array
         * @endcode
         *
         * Vou can also achieve the same effect by calling this function more
         * times with absolute offsets:
         * @code
         * mesh->addVertexBuffer(buffer, 35, Shaders::PhongShader::Position());
         *     ->addVertexBuffer(buffer, 35 + (sizeof(Shaders::PhongShader::Position::Type) + sizeof(Vector2))*
         *          mesh->vertexCount(), Shaders::PhongShader::Normal());
         * @endcode
         *
         * @attention Non-zero vertex count must be set before calling this
         *      function.
         * @attention The buffer passed as parameter is not managed by the
         *      mesh, you must ensure it will exist for whole lifetime of the
         *      mesh and delete it afterwards.
         *
         * @see addInterleavedVertexBuffer(), addVertexBufferStride(),
         *      setPrimitive(), setVertexCount(), @fn_gl{BindVertexArray},
         *      @fn_gl{EnableVertexAttribArray}, @fn_gl{BindBuffer},
         *      @fn_gl{VertexAttribPointer} or
         *      @fn_gl_extension{EnableVertexArrayAttrib,EXT,direct_state_access},
         *      @fn_gl_extension{VertexArrayVertexAttribOffset,EXT,direct_state_access}
         *      if @extension{APPLE,vertex_array_object} is available
         */
        template<class ...T> inline Mesh* addVertexBuffer(Buffer* buffer, const T&... attributes) {
            CORRADE_ASSERT(_vertexCount != 0, "Mesh: vertex count must be set before binding attributes", this);

            addVertexBufferInternal(buffer, 0, attributes...);
            return this;
        }

        /**
         * @brief Add buffer with interleaved vertex attributes for use with given shader
         * @return Pointer to self (for method chaining)
         *
         * Parameter @p offset is offset of the interleaved array from the
         * beginning, attribute list is combination of
         * @ref AbstractShaderProgram::Attribute "attribute definitions"
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
         *     35,                                  // skip other data
         *     sizeof(GLfloat),                     // skip vertex weight
         *     Shaders::PhongShader::Position(),    // vertex position
         *     sizeof(Vector2),                     // skip texture coordinates
         *     Shaders::PhongShader::Normal());     // vertex normal
         * @endcode
         *
         * You can also achieve the same effect by calling addVertexBufferStride()
         * more times with absolute offset from the beginning and stride
         * between vertex attributes:
         * @code
         * GLsizei stride =                         // size of one vertex
         *     sizeof(GLfloat) +
         *     sizeof(Shaders::PhongShader::Position::Type) +
         *     sizeof(Vector2) +
         *     sizeof(Shaders::PhongShader::Normal::Type);
         *
         * mesh->addVertexBufferStride(buffer, 35 + sizeof(GLfloat),
         *          stride, Shaders::PhongShader::Position());
         *     ->addVertexBufferStride(buffer, 35 + sizeof(GLfloat) +
         *          sizeof(Shaders::PhongShader::Position::Type) + sizeof(Vector2),
         *          stride, Shaders::PhongShader::Normal());
         * @endcode
         *
         * @attention The buffer passed as parameter is not managed by the
         *      mesh, you must ensure it will exist for whole lifetime of the
         *      mesh and delete it afterwards.
         *
         * @see addVertexBufferStride(), addVertexBuffer(), setPrimitive(),
         *      setVertexCount(), @fn_gl{BindVertexArray},
         *      @fn_gl{EnableVertexAttribArray}, @fn_gl{BindBuffer},
         *      @fn_gl{VertexAttribPointer} or
         *      @fn_gl_extension{EnableVertexArrayAttrib,EXT,direct_state_access},
         *      @fn_gl_extension{VertexArrayVertexAttribOffset,EXT,direct_state_access}
         *      if @extension{APPLE,vertex_array_object} is available
         */
        template<class ...T> inline Mesh* addInterleavedVertexBuffer(Buffer* buffer, GLintptr offset, const T&... attributes) {
            addInterleavedVertexBufferInternal(buffer, offset, strideOfInterleaved(attributes...), attributes...);
            return this;
        }

        /**
         * @brief Add buffer with interleaved vertex attributes for use with given shader
         * @return Pointer to self (for method chaining)
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
        #ifndef DOXYGEN_GENERATING_OUTPUT
        struct MAGNUM_LOCAL Attribute {
            Buffer* buffer;
            GLuint location;
            GLint size;
            GLenum type;
            bool normalized;
            GLintptr offset;
            GLsizei stride;
        };

        #ifndef MAGNUM_TARGET_GLES2
        struct MAGNUM_LOCAL IntegerAttribute {
            Buffer* buffer;
            GLuint location;
            GLint size;
            GLenum type;
            GLintptr offset;
            GLsizei stride;
        };

        #ifndef MAGNUM_TARGET_GLES
        struct MAGNUM_LOCAL LongAttribute {
            Buffer* buffer;
            GLuint location;
            GLint size;
            GLenum type;
            GLintptr offset;
            GLsizei stride;
        };
        #endif
        #endif
        #endif

        static void MAGNUM_LOCAL initializeContextBasedFunctionality(Context* context);

        /* Adding non-interleaved vertex attributes */
        template<GLuint location, class T, class ...U> inline void addVertexBufferInternal(Buffer* buffer, GLintptr offset, const AbstractShaderProgram::Attribute<location, T>& attribute, const U&... attributes) {
            addVertexAttribute(buffer, attribute, offset, 0);

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
        template<GLuint location, class T, class ...U> inline void addInterleavedVertexBufferInternal(Buffer* buffer, GLintptr offset, GLsizei stride, const AbstractShaderProgram::Attribute<location, T>& attribute, const U&... attributes) {
            addVertexAttribute(buffer, attribute, offset, stride);

            /* Add size of this attribute to offset for next attribute */
            addInterleavedVertexBufferInternal(buffer, offset+TypeTraits<T>::count()*TypeTraits<T>::size(), stride, attributes...);
        }
        template<class ...T> inline void addInterleavedVertexBufferInternal(Buffer* buffer, GLintptr offset, GLsizei stride, GLintptr gap, const T&... attributes) {
            /* Add the gap to offset for next attribute */
            addInterleavedVertexBufferInternal(buffer, offset+gap, stride, attributes...);
        }
        inline void addInterleavedVertexBufferInternal(Buffer*, GLsizei, GLintptr) {}

        template<GLuint location, class T> inline void addVertexAttribute(typename std::enable_if<std::is_same<typename TypeTraits<T>::AttributeType, GLfloat>::value, Buffer*>::type buffer, const AbstractShaderProgram::Attribute<location, T>& attribute, GLintptr offset, GLsizei stride) {
            for(GLuint i = 0; i != Implementation::Attribute<T>::vectorCount(); ++i) {
                attributes.push_back({
                    buffer,
                    location+i,
                    Implementation::Attribute<T>::components(attribute.dataOptions()),
                    static_cast<GLenum>(attribute.dataType()),
                    !!(attribute.dataOptions() & AbstractShaderProgram::Attribute<location, T>::DataOption::Normalized),
                    offset,
                    stride
                });
            }

            (this->*attributePointerImplementation)(attributes.back());
        }

        #ifndef MAGNUM_TARGET_GLES2
        template<GLuint location, class T> inline void addVertexAttribute(typename std::enable_if<std::is_integral<typename TypeTraits<T>::AttributeType>::value, Buffer*>::type buffer, const AbstractShaderProgram::Attribute<location, T>& attribute, GLintptr offset, GLsizei stride) {
            integerAttributes.push_back({
                buffer,
                location,
                Implementation::Attribute<T>::components(),
                static_cast<GLenum>(attribute.dataType()),
                offset,
                stride
            });

            (this->*attributeIPointerImplementation)(integerAttributes.back());
        }

        #ifndef MAGNUM_TARGET_GLES
        template<GLuint location, class T> inline void addVertexAttribute(typename std::enable_if<std::is_same<typename TypeTraits<T>::AttributeType, GLdouble>::value, Buffer*>::type buffer, const AbstractShaderProgram::Attribute<location, T>& attribute, GLintptr offset, GLsizei stride) {
            for(GLuint i = 0; i != Implementation::Attribute<T>::vectorCount(); ++i) {
                longAttributes.push_back({
                    buffer,
                    location+i,
                    Implementation::Attribute<T>::components(),
                    static_cast<GLenum>(attribute.dataType()),
                    offset,
                    stride
                });

                (this->*attributeLPointerImplementation)(longAttributes.back());
            }
        }
        #endif
        #endif

        static void MAGNUM_LOCAL bindVAO(GLuint vao);

        void MAGNUM_LOCAL bind();

        inline void unbind() {
            (this->*unbindImplementation)();
        }

        void MAGNUM_LOCAL vertexAttribPointer(const Attribute& attribute);
        #ifndef MAGNUM_TARGET_GLES2
        void MAGNUM_LOCAL vertexAttribPointer(const IntegerAttribute& attribute);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL vertexAttribPointer(const LongAttribute& attribute);
        #endif
        #endif

        typedef void(Mesh::*CreateImplementation)();
        void MAGNUM_LOCAL createImplementationDefault();
        void MAGNUM_LOCAL createImplementationVAO();
        static CreateImplementation createImplementation;

        typedef void(Mesh::*DestroyImplementation)();
        void MAGNUM_LOCAL destroyImplementationDefault();
        void MAGNUM_LOCAL destroyImplementationVAO();
        static MAGNUM_LOCAL DestroyImplementation destroyImplementation;

        typedef void(Mesh::*AttributePointerImplementation)(const Attribute&);
        void MAGNUM_LOCAL attributePointerImplementationDefault(const Attribute& attribute);
        void MAGNUM_LOCAL attributePointerImplementationVAO(const Attribute& attribute);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL attributePointerImplementationDSA(const Attribute& attribute);
        #endif
        static AttributePointerImplementation attributePointerImplementation;

        #ifndef MAGNUM_TARGET_GLES2
        typedef void(Mesh::*AttributeIPointerImplementation)(const IntegerAttribute&);
        void MAGNUM_LOCAL attributePointerImplementationDefault(const IntegerAttribute& attribute);
        void MAGNUM_LOCAL attributePointerImplementationVAO(const IntegerAttribute& attribute);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL attributePointerImplementationDSA(const IntegerAttribute& attribute);
        #endif
        static AttributeIPointerImplementation attributeIPointerImplementation;

        #ifndef MAGNUM_TARGET_GLES
        typedef void(Mesh::*AttributeLPointerImplementation)(const LongAttribute&);
        void MAGNUM_LOCAL attributePointerImplementationDefault(const LongAttribute& attribute);
        void MAGNUM_LOCAL attributePointerImplementationVAO(const LongAttribute& attribute);
        void MAGNUM_LOCAL attributePointerImplementationDSA(const LongAttribute& attribute);
        static AttributeLPointerImplementation attributeLPointerImplementation;
        #endif
        #endif

        typedef void(Mesh::*BindImplementation)();
        void MAGNUM_LOCAL bindImplementationDefault();
        void MAGNUM_LOCAL bindImplementationVAO();
        static MAGNUM_LOCAL BindImplementation bindImplementation;

        typedef void(Mesh::*UnbindImplementation)();
        void MAGNUM_LOCAL unbindImplementationDefault();
        void MAGNUM_LOCAL unbindImplementationVAO();
        static MAGNUM_LOCAL UnbindImplementation unbindImplementation;

        GLuint vao;
        Primitive _primitive;
        GLsizei _vertexCount;

        std::vector<Attribute> attributes;
        #ifndef MAGNUM_TARGET_GLES2
        std::vector<IntegerAttribute> integerAttributes;
        #ifndef MAGNUM_TARGET_GLES
        std::vector<LongAttribute> longAttributes;
        #endif
        #endif
};

/** @debugoperator{Magnum::Mesh} */
Debug MAGNUM_EXPORT operator<<(Debug debug, Mesh::Primitive value);

}

namespace Corrade { namespace Utility {

/** @configurationvalue{Magnum::Mesh} */
template<> struct MAGNUM_EXPORT ConfigurationValue<Magnum::Mesh::Primitive> {
    ConfigurationValue() = delete;

    /**
     * @brief Writes enum value as string
     *
     * If the value is invalid, returns empty string.
     */
    static std::string toString(Magnum::Mesh::Primitive value, ConfigurationValueFlags);

    /**
     * @brief Reads enum value as string
     *
     * If the value is invalid, returns @ref Magnum::Mesh::Primitive "Mesh::Primitive::Points".
     */
    static Magnum::Mesh::Primitive fromString(const std::string& stringValue, ConfigurationValueFlags);
};

}}

#endif
