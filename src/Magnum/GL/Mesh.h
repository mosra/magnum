#ifndef Magnum_GL_Mesh_h
#define Magnum_GL_Mesh_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

/** @file
 * @brief Class @ref Magnum::GL::Mesh, enum @ref Magnum::GL::MeshPrimitive, @ref Magnum::GL::MeshIndexType, function @ref Magnum::GL::meshPrimitive(), @ref Magnum::GL::meshIndexType()
 */

#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Utility/ConfigurationValue.h>

#include "Magnum/Tags.h"
#include "Magnum/GL/AbstractObject.h"
#include "Magnum/GL/Attribute.h"
#include "Magnum/GL/Buffer.h"
#include "Magnum/GL/GL.h"

namespace Magnum { namespace GL {

/**
@brief Mesh primitive type

@see @ref Magnum::MeshPrimitive, @ref meshPrimitive(), @ref Mesh::primitive(),
    @ref Mesh::setPrimitive()
@m_enum_values_as_keywords
*/
enum class MeshPrimitive: GLenum {
    /** Single points. */
    Points = GL_POINTS,

    /**
     * Each pair of vertices defines a single line, lines aren't
     * connected together.
     */
    Lines = GL_LINES,

    /** Line strip, last and first vertex are connected together. */
    LineLoop = GL_LINE_LOOP,

    /**
     * First two vertices define first line segment, each following
     * vertex defines another segment.
     */
    LineStrip = GL_LINE_STRIP,

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    /**
     * Lines with adjacency information.
     * @requires_gl32 Extension @gl_extension{ARB,geometry_shader4}
     * @requires_gles30 Not defined in OpenGL ES 2.0.
     * @requires_gles32 Extension @gl_extension{ANDROID,extension_pack_es31a} /
     *      @gl_extension{EXT,geometry_shader}
     * @requires_gles Geometry shaders are not available in WebGL.
     */
    LinesAdjacency = GL_LINES_ADJACENCY,

    /**
     * Line strip with adjacency information.
     * @requires_gl32 Extension @gl_extension{ARB,geometry_shader4}
     * @requires_gles30 Not defined in OpenGL ES 2.0.
     * @requires_gles32 Extension @gl_extension{ANDROID,extension_pack_es31a} /
     *      @gl_extension{EXT,geometry_shader}
     * @requires_gles Geometry shaders are not available in WebGL.
     */
    LineStripAdjacency = GL_LINE_STRIP_ADJACENCY,
    #endif

    /** Each three vertices define one triangle. */
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
    TriangleFan = GL_TRIANGLE_FAN,

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    /**
     * Triangles with adjacency information.
     * @requires_gl32 Extension @gl_extension{ARB,geometry_shader4}
     * @requires_gles30 Not defined in OpenGL ES 2.0.
     * @requires_gles32 Extension @gl_extension{ANDROID,extension_pack_es31a} /
     *      @gl_extension{EXT,geometry_shader}
     * @requires_gles Geometry shaders are not available in WebGL.
     */
    TrianglesAdjacency = GL_TRIANGLES_ADJACENCY,

    /**
     * Triangle strip with adjacency information.
     * @requires_gl32 Extension @gl_extension{ARB,geometry_shader4}
     * @requires_gles30 Not defined in OpenGL ES 2.0.
     * @requires_gles32 Extension @gl_extension{ANDROID,extension_pack_es31a} /
     *      @gl_extension{EXT,geometry_shader}
     * @requires_gles Geometry shaders are not available in WebGL.
     */
    TriangleStripAdjacency = GL_TRIANGLE_STRIP_ADJACENCY,

    /**
     * Patches.
     * @requires_gl40 Extension @gl_extension{ARB,tessellation_shader}
     * @requires_gles30 Not defined in OpenGL ES 2.0.
     * @requires_gles32 Extension @gl_extension{ANDROID,extension_pack_es31a} /
     *      @gl_extension{EXT,tessellation_shader}
     * @requires_gles Tessellation shaders are not available in WebGL.
     */
    Patches = GL_PATCHES
    #endif
};

/**
@brief Check availability of a generic mesh primitive
@m_since{2020,06}

Returns @cpp false @ce if OpenGL doesn't support such primitive, @cpp true @ce
otherwise. Moreover, returns @cpp true @ce also for all formats that are
@ref isMeshPrimitiveImplementationSpecific(). The @p primitive value is
expected to be valid.
@see @ref meshPrimitive()
*/
MAGNUM_GL_EXPORT bool hasMeshPrimitive(Magnum::MeshPrimitive primitive);

/**
@brief Convert generic mesh primitive to OpenGL mesh primitive

In case @ref isMeshPrimitiveImplementationSpecific() returns @cpp false @ce for
@p primitive, maps it to a corresponding OpenGL mesh primitive. In case
@ref isMeshPrimitiveImplementationSpecific() returns @cpp true @ce, assumes
@p primitive stores OpenGL-specific mesh primitive and returns
@ref meshPrimitiveUnwrap() cast to @ref GL::MeshPrimitive.

Not all generic mesh primitives are available in OpenGL and this function
expects that given primitive is available. Use @ref hasMeshPrimitive() to
query availability of given primitive.
@see @ref meshIndexType()
*/
MAGNUM_GL_EXPORT MeshPrimitive meshPrimitive(Magnum::MeshPrimitive primitive);

/**
@brief Index type

@see @ref Magnum::MeshIndexType, @ref meshIndexType(),
    @ref meshIndexTypeSize(), @ref Mesh::setIndexBuffer()
@m_enum_values_as_keywords
*/
enum class MeshIndexType: GLenum {
    UnsignedByte = GL_UNSIGNED_BYTE,    /**< Unsigned byte */
    UnsignedShort = GL_UNSIGNED_SHORT,  /**< Unsigned short */

    /**
     * Unsigned int
     * @requires_gles30 Extension @gl_extension{OES,element_index_uint}
     *       in OpenGL ES 2.0.
     * @requires_webgl20 Extension @webgl_extension{OES,element_index_uint}
     *      in WebGL 1.0.
     */
    UnsignedInt = GL_UNSIGNED_INT
};

/**
@brief Convert generic mesh index type to OpenGL mesh index type

@see @ref meshPrimitive(), @ref meshIndexTypeSize()
*/
MAGNUM_GL_EXPORT MeshIndexType meshIndexType(Magnum::MeshIndexType type);

namespace Implementation {

struct MeshState;

template<class...> struct IsDynamicAttribute: std::false_type {};
template<class T> struct IsDynamicAttribute<T, DynamicAttribute>: std::true_type {};

}

/**
@brief Mesh

@section GL-Mesh-configuration Mesh configuration

You have to specify at least primitive and vertex/index count using
@ref setPrimitive() and @ref setCount(). Then fill your vertex buffers with
data, add them to the mesh and specify @ref Attribute "shader attribute" layout
inside the buffers using @ref addVertexBuffer(). You can also use
@ref MeshTools::interleave() to conveniently interleave vertex data.

If you want indexed mesh, fill your index buffer with data and specify its
layout using @ref setIndexBuffer(). You can also use @ref MeshTools::compressIndices()
to conveniently compress the indices based on the range used.

There is also @ref MeshTools::compile() function which operates directly on
@ref Trade::MeshData and returns fully configured mesh and vertex/index buffers
for use with stock shaders.

@attention Note that, by default, neither vertex buffers nor index buffer is
    managed (e.g. deleted on destruction) by the mesh, so you have to manage
    them on your own and ensure that they are available for whole mesh
    lifetime. See @ref GL-Mesh-buffer-ownership for a way to transfer buffer
    ownership to the mesh.

If vertex/index count or instance count is zero, the mesh is empty and no draw
commands are issued when calling @ref AbstractShaderProgram::draw().

@subsection GL-Mesh-configuration-example Example mesh configuration

@subsubsection GL-Mesh-configuration-example-basic Basic non-indexed mesh

@snippet MagnumGL.cpp Mesh-nonindexed

@subsubsection GL-Mesh-configuration-interleaved Interleaved vertex data

@snippet MagnumGL.cpp Mesh-interleaved

@subsubsection GL-Mesh-configuration-indexed Indexed mesh

@snippet MagnumGL.cpp Mesh-indexed

Or using @ref MeshTools::interleave() and @ref MeshTools::compressIndices():

@snippet MagnumGL.cpp Mesh-indexed-tools

Or, if you plan to use the mesh with stock shaders, you can just use
@ref MeshTools::compile().

@subsubsection GL-Mesh-configuration-formats Specific formats of vertex data

@snippet MagnumGL.cpp Mesh-formats

@subsubsection GL-Mesh-configuration-dynamic Dynamically specified attributes

In some cases, for example when the shader code is fully generated at runtime,
it's not possible to know attribute locations and types at compile time. In
that case, there are overloads of @ref addVertexBuffer() and
@ref addVertexBufferInstanced() that take @ref DynamicAttribute instead of
@ref Attribute typedefs. Adding a RGB attribute at location 3 normalized from
unsigned byte to float with one byte padding at the end could then look like
this:

@snippet MagnumGL.cpp Mesh-dynamic

@section GL-Mesh-buffer-ownership Transfering buffer ownership

If a vertex/index buffer is used only by a single mesh, it's possible to
transfer its ownership to the mesh itself to simplify resource management on
the user side. Simply use the @ref addVertexBuffer() /
@ref addVertexBufferInstanced() and @ref setIndexBuffer() overloads that take
a @ref Buffer as a rvalue:

@snippet MagnumGL.cpp Mesh-buffer-ownership

While this allows you to destruct the buffer instances and pass just the mesh
around, this also means you lose a way to access or update the buffers. If
adding the same buffer multiple times or using it for both vertex and index
data, be sure to transfer the ownership last to avoid the other functions
getting only a moved-out instance. For example:

@snippet MagnumGL.cpp Mesh-buffer-ownership-multiple

@section GL-Mesh-rendering Rendering meshes

Basic workflow is: bind specific framebuffer for drawing (if needed), set up
respective shader (see
@ref GL-AbstractShaderProgram-rendering-workflow "AbstractShaderProgram documentation"
for more infromation) and call @ref AbstractShaderProgram::draw().

@section GL-Mesh-webgl-restrictions WebGL restrictions

@ref MAGNUM_TARGET_WEBGL "WebGL" puts some restrictions on vertex buffer
layout, see @ref addVertexBuffer() documentation for details.

@section GL-Mesh-performance-optimization Performance optimizations

If @gl_extension{ARB,vertex_array_object} (part of OpenGL 3.0), OpenGL ES 3.0,
WebGL 2.0, @gl_extension{OES,vertex_array_object} in OpenGL ES 2.0 or
@webgl_extension{OES,vertex_array_object} in WebGL 1.0 is supported, VAOs are
used instead of binding the buffers and specifying vertex attribute pointers
in each @ref AbstractShaderProgram::draw() call. The engine tracks currently
bound VAO and currently active shader program to avoid unnecessary calls to @fn_gl_keyword{BindVertexArray}
and @fn_gl_keyword{UseProgram}. Mesh limits and implementation-defined values
(such as @ref maxElementIndex()) are cached, so repeated queries don't result
in repeated @fn_gl{Get} calls.

If @gl_extension{ARB,direct_state_access} desktop extension and VAOs are
available, DSA functions are used for specifying attribute locations to avoid
unnecessary calls to @fn_gl{BindBuffer} and @fn_gl{BindVertexArray}. See
documentation of @ref addVertexBuffer() for more information.

If index range is specified in @ref setIndexBuffer(), range-based version of
drawing commands are used on desktop OpenGL and OpenGL ES 3.0. See also
@ref AbstractShaderProgram::draw() for more information.
 */
class MAGNUM_GL_EXPORT Mesh: public AbstractObject {
    public:
        /**
         * @brief Max vertex attribute stride
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If OpenGL 4.4 or OpenGL 3.1 supporting this query
         * isn't available, returns max representable 32-bit value
         * (@cpp 0xffffffffu @ce). On WebGL 1 and 2 the max stride is specified
         * to be @cpp 255 @ce with no corresponding limit query.
         * @see @ref addVertexBuffer(), @fn_gl{Get} with
         *      @def_gl_keyword{MAX_VERTEX_ATTRIB_STRIDE}
         */
        static UnsignedInt maxVertexAttributeStride();

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Max supported index value
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If extension @gl_extension{ARB,ES3_compatibility} (part
         * of OpenGL 4.3) is not available, returns max representable 32-bit
         * value (@cpp 0xffffffffu @ce).
         * @see @ref setIndexBuffer(), @fn_gl{Get} with
         *      @def_gl_keyword{MAX_ELEMENT_INDEX}
         * @requires_gles30 No upper limit is specified for index values in
         *      OpenGL ES 2.0.
         * @requires_webgl20 No upper limit is specified for index values in
         *      WebGL 1.0.
         */
        #ifndef MAGNUM_TARGET_WEBGL
        static Long maxElementIndex();
        #else
        static Int maxElementIndex();
        #endif

        /**
         * @brief Max recommended index count
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls.
         * @see @ref setIndexBuffer(), @fn_gl{Get} with
         *      @def_gl_keyword{MAX_ELEMENTS_INDICES}
         * @requires_gles30 Ranged element draw is not supported in OpenGL ES
         *      2.0.
         * @requires_webgl20 Ranged element draw is not supported in WebGL 1.0.
         */
        static Int maxElementsIndices();

        /**
         * @brief Max recommended vertex count
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls.
         * @see @ref setIndexBuffer(), @fn_gl{Get} with
         *      @def_gl_keyword{MAX_ELEMENTS_VERTICES}
         * @requires_gles30 Ranged element draw is not supported in OpenGL ES
         *      2.0.
         * @requires_webgl20 Ranged element draw is not supported in WebGL 1.0.
         */
        static Int maxElementsVertices();
        #endif

        /**
         * @brief Wrap existing OpenGL vertex array object
         * @param id            OpenGL vertex array ID
         * @param primitive     Primitive type
         * @param flags         Object creation flags
         *
         * The @p id is expected to be of an existing OpenGL vertex array
         * object. Unlike vertex array created using constructor, the OpenGL
         * object is by default not deleted on destruction, use @p flags for
         * different behavior.
         * @see @ref release()
         * @requires_gl30 Extension @gl_extension{ARB,vertex_array_object}
         * @requires_gles30 Extension @gl_extension{OES,vertex_array_object} in
         *      OpenGL ES 2.0.
         * @requires_webgl20 Extension @webgl_extension{OES,vertex_array_object}
         *      in WebGL 1.0.
         */
        static Mesh wrap(GLuint id, MeshPrimitive primitive = MeshPrimitive::Triangles, ObjectFlags flags = {}) {
            return Mesh{id, primitive, flags};
        }

        /** @overload */
        static Mesh wrap(GLuint id, ObjectFlags flags) {
            return wrap(id, MeshPrimitive::Triangles, flags);
        }

        /**
         * @brief Constructor
         * @param primitive     Primitive type
         *
         * If @gl_extension{ARB,vertex_array_object} (part of OpenGL 3.0), OpenGL
         * ES 3.0, WebGL 2.0, @gl_extension{OES,vertex_array_object} in OpenGL
         * ES 2.0 or @webgl_extension{OES,vertex_array_object} in WebGL 1.0 is
         * available, vertex array object is created. If
         * @gl_extension{ARB,direct_state_access} (part of OpenGL 4.5) is not
         * available, the vertex array object is created on first use.
         * @see @ref Mesh(NoCreateT), @ref wrap(), @ref setPrimitive(),
         *      @ref setCount(), @fn_gl_keyword{CreateVertexArrays}, eventually
         *      @fn_gl_keyword{GenVertexArrays}
         */
        explicit Mesh(MeshPrimitive primitive = MeshPrimitive::Triangles);

        /** @overload */
        explicit Mesh(Magnum::MeshPrimitive primitive): Mesh{meshPrimitive(primitive)} {}

        /**
         * @brief Construct without creating the underlying OpenGL object
         *
         * The constructed instance is equivalent to moved-from state. Useful
         * in cases where you will overwrite the instance later anyway. Move
         * another object over it to make it useful.
         *
         * This function can be safely used for constructing (and later
         * destructing) objects even without any OpenGL context being active.
         * However note that this is a low-level and a potentially dangerous
         * API, see the documentation of @ref NoCreate for alternatives.
         * @see @ref Mesh(MeshPrimitive), @ref wrap()
         */
        explicit Mesh(NoCreateT) noexcept;

        /** @brief Copying is not allowed */
        Mesh(const Mesh&) = delete;

        /** @brief Move constructor */
        Mesh(Mesh&& other) noexcept;

        /**
         * @brief Destructor
         *
         * If @gl_extension{ARB,vertex_array_object} (part of OpenGL 3.0), OpenGL
         * ES 3.0, WebGL 2.0, @gl_extension{OES,vertex_array_object} in OpenGL
         * ES 2.0 or @webgl_extension{OES,vertex_array_object} in WebGL 1.0 is
         * available, associated vertex array object is deleted.
         * @see @ref wrap(), @ref release(), @fn_gl_keyword{DeleteVertexArrays}
         */
        ~Mesh();

        /** @brief Copying is not allowed */
        Mesh& operator=(const Mesh&) = delete;

        /** @brief Move assignment */
        Mesh& operator=(Mesh&& other) noexcept;

        /**
         * @brief OpenGL vertex array ID
         *
         * If neither @gl_extension{ARB,vertex_array_object} (part of OpenGL 3.0)
         * nor OpenGL ES 3.0 / WebGL 2.0 nor @gl_extension{OES,vertex_array_object}
         * in OpenGL ES 2.0 / @webgl_extension{OES,vertex_array_object} in
         * WebGL 1.0 is available, returns `0`.
         */
        GLuint id() const { return _id; }

        /**
         * @brief Release OpenGL object
         *
         * Releases ownership of OpenGL vertex array object and returns its ID
         * so it is not deleted on destruction. The internal state is then
         * equivalent to moved-from state.
         * @see @ref wrap()
         * @requires_gl30 Extension @gl_extension{ARB,vertex_array_object}
         * @requires_gles30 Extension @gl_extension{OES,vertex_array_object} in
         *      OpenGL ES 2.0.
         * @requires_webgl20 Extension @webgl_extension{OES,vertex_array_object}
         *      in WebGL 1.0.
         */
        GLuint release();

        #ifndef MAGNUM_TARGET_WEBGL
        /**
         * @brief Mesh label
         *
         * The result is *not* cached, repeated queries will result in repeated
         * OpenGL calls. If OpenGL 4.3 / OpenGL ES 3.2 is not supported and
         * neither @gl_extension{KHR,debug} (covered also by
         * @gl_extension{ANDROID,extension_pack_es31a}) nor @gl_extension{EXT,debug_label}
         * desktop or ES extension is available, this function returns empty
         * string.
         * @see @fn_gl_keyword{GetObjectLabel} with @def_gl{VERTEX_ARRAY} or
         *      @fn_gl_extension_keyword{GetObjectLabel,EXT,debug_label} with
         *      @def_gl{VERTEX_ARRAY_OBJECT_EXT}
         * @requires_gles Debug output is not available in WebGL.
         */
        std::string label();

        /**
         * @brief Set mesh label
         * @return Reference to self (for method chaining)
         *
         * Default is empty string. If OpenGL 4.3 / OpenGL ES 3.2 is not
         * supported and neither @gl_extension{KHR,debug} (covered also by
         * @gl_extension{ANDROID,extension_pack_es31a}) nor @gl_extension{EXT,debug_label}
         * desktop or ES extension is available, this function does nothing.
         * @see @ref maxLabelLength(), @fn_gl_keyword{ObjectLabel} with
         *      @def_gl_keyword{VERTEX_ARRAY} or @fn_gl_extension_keyword{LabelObject,EXT,debug_label}
         *      with @def_gl{VERTEX_ARRAY_OBJECT_EXT}
         * @requires_gles Debug output is not available in WebGL.
         */
        Mesh& setLabel(const std::string& label) {
            return setLabelInternal({label.data(), label.size()});
        }

        /** @overload */
        template<std::size_t size> Mesh& setLabel(const char(&label)[size]) {
            return setLabelInternal({label, size - 1});
        }
        #endif

        /**
         * @brief Whether the mesh is indexed
         *
         * @see @ref setIndexBuffer(), @ref setCount()
         */
        bool isIndexed() const { return _indexBuffer.id(); }

        /**
         * @brief Index type
         *
         * Expects that the mesh is indexed.
         * @see @ref isIndexed()
         */
        MeshIndexType indexType() const;

        /**
         * @brief Index type size
         *
         * Expects that the mesh is indexed.
         * @see @ref isIndexed(), @ref meshIndexTypeSize(Magnum::MeshIndexType)
         */
        UnsignedInt indexTypeSize() const;

        /** @brief Primitive type */
        MeshPrimitive primitive() const { return _primitive; }

        /**
         * @brief Set primitive type
         * @return Reference to self (for method chaining)
         *
         * Default is @ref MeshPrimitive::Triangles.
         */
        Mesh& setPrimitive(MeshPrimitive primitive) {
            _primitive = primitive;
            return *this;
        }

        /** @overload */
        Mesh& setPrimitive(Magnum::MeshPrimitive primitive) {
            return setPrimitive(meshPrimitive(primitive));
        }

        /** @brief Vertex/index count */
        Int count() const { return _count; }

        /**
         * @brief Set vertex/index count
         * @return Reference to self (for method chaining)
         *
         * If the mesh is indexed, the value is treated as index count,
         * otherwise the value is vertex count. If set to @cpp 0 @ce, no draw
         * commands are issued when calling @ref AbstractShaderProgram::draw().
         * Ignored when calling @ref AbstractShaderProgram::drawTransformFeedback().
         *
         * @attention To prevent nothing being rendered by accident, this
         *      function has to be always called, even to just set the count to
         *      @cpp 0 @ce.
         *
         * @see @ref isIndexed(), @ref setBaseVertex(), @ref setInstanceCount()
         */
        Mesh& setCount(Int count) {
            _countSet = true;
            _count = count;
            return *this;
        }

        /** @brief Base vertex */
        Int baseVertex() const { return _baseVertex; }

        /**
         * @brief Set base vertex
         * @return Reference to self (for method chaining)
         *
         * Sets number of vertices of which the vertex buffer will be offset
         * when drawing. Ignored when calling
         * @ref AbstractShaderProgram::drawTransformFeedback(). Default is
         * @cpp 0 @ce.
         * @see @ref setCount(), @ref setBaseInstance()
         * @requires_gl32 Extension @gl_extension{ARB,draw_elements_base_vertex}
         *      for indexed meshes
         * @requires_gles32 Base vertex cannot be specified for indexed meshes
         *      in OpenGL ES 3.1 or WebGL.
         */
        Mesh& setBaseVertex(Int baseVertex) {
            _baseVertex = baseVertex;
            return *this;
        }

        /** @brief Instance count */
        Int instanceCount() const { return _instanceCount; }

        /**
         * @brief Set instance count
         * @return Reference to self (for method chaining)
         *
         * If set to @cpp 1 @ce, non-instanced draw commands are issued when
         * calling @ref AbstractShaderProgram::draw() or
         * @ref AbstractShaderProgram::drawTransformFeedback().
         * If set to @cpp 0 @ce, no draw commands are issued at all. Default is
         * @cpp 1 @ce.
         * @see @ref setBaseInstance(), @ref setCount(),
         *      @ref addVertexBufferInstanced()
         * @requires_gl31 Extension @gl_extension{ARB,draw_instanced} if using
         *      @ref AbstractShaderProgram::draw()
         * @requires_gl42 Extension @gl_extension{ARB,transform_feedback_instanced}
         *      if using @ref AbstractShaderProgram::drawTransformFeedback()
         * @requires_gles30 Extension @gl_extension{ANGLE,instanced_arrays},
         *      @gl_extension{EXT,instanced_arrays},
         *      @gl_extension{EXT,draw_instanced},
         *      @gl_extension{NV,instanced_arrays},
         *      @gl_extension{NV,draw_instanced} in OpenGL ES 2.0.
         * @requires_webgl20 Extension @webgl_extension{ANGLE,instanced_arrays}
         *      in WebGL 1.0.
         */
        Mesh& setInstanceCount(Int count) {
            _instanceCount = count;
            return *this;
        }

        #ifndef MAGNUM_TARGET_GLES
        /** @brief Base instance */
        UnsignedInt baseInstance() const { return _baseInstance; }

        /**
         * @brief Set base instance
         * @return Reference to self (for method chaining)
         *
         * Ignored when calling @ref AbstractShaderProgram::drawTransformFeedback().
         * Default is @cpp 0 @ce.
         * @see @ref setInstanceCount(), @ref setBaseVertex()
         * @requires_gl42 Extension @gl_extension{ARB,base_instance}
         * @requires_gl Base instance cannot be specified in OpenGL ES or
         *      WebGL.
         */
        Mesh& setBaseInstance(UnsignedInt baseInstance) {
            _baseInstance = baseInstance;
            return *this;
        }
        #endif

        /**
         * @brief Add buffer with (interleaved) vertex attributes for use with given shader
         * @return Reference to self (for method chaining)
         *
         * Parameter @p offset is offset of the array from the beginning,
         * attribute list is combination of @ref Attribute "attribute definitions"
         * (specified in implementation of given shader) and offsets between
         * interleaved attributes.
         *
         * See @ref GL-Mesh-configuration "class documentation" for simple
         * usage example. For more involved example imagine that you have a
         * buffer with 76 bytes of some other data at the beginning (possibly
         * material configuration) and then the interleaved vertex array. Each
         * vertex consists of a weight, position, texture coordinate and
         * a normal. You want to draw it with @ref Shaders::Phong, but it
         * accepts only a position and a normal, so you have to skip the weight
         * and the texture coordinate in each vertex:
         *
         * @snippet MagnumGL.cpp Mesh-addVertexBuffer1
         *
         * You can also achieve the same effect by calling @ref addVertexBuffer()
         * more times with explicitly specified gaps before and after the
         * attributes. This can be used for e.g. runtime-dependent
         * configuration, as it isn't dependent on the variadic template:
         *
         * @snippet MagnumGL.cpp Mesh-addVertexBuffer2
         *
         * If specifying more than one attribute, the function assumes that
         * the array is interleaved. Adding non-interleaved vertex buffer can
         * be done by specifying one attribute at a time with specific offset.
         * Above example with weight, position, texture coordinate and normal
         * arrays one after another (non-interleaved):
         *
         * @snippet MagnumGL.cpp Mesh-addVertexBuffer3
         *
         * If @gl_extension{ARB,vertex_array_object} (part of OpenGL 3.0), OpenGL
         * ES 3.0, WebGL 2.0, @gl_extension{OES,vertex_array_object} in OpenGL
         * ES 2.0 or @webgl_extension{OES,vertex_array_object} in WebGL 1.0 is
         * available, the vertex array object is used to hold the parameters.
         *
         * @attention The buffer passed as parameter is not managed by the
         *      mesh, you must ensure it will exist for whole lifetime of the
         *      mesh and delete it afterwards.
         *
         * @see @ref addVertexBufferInstanced(), @ref setPrimitive(),
         *      @ref setCount(), @ref maxVertexAttributeStride(),
         *      @fn_gl_keyword{BindVertexArray},
         *      @fn_gl_keyword{EnableVertexAttribArray}, @fn_gl{BindBuffer},
         *      @fn_gl_keyword{VertexAttribPointer} or
         *      @fn_gl2{EnableVertexArrayAttrib,EnableVertexAttribArray},
         *      @fn_gl2{VertexArrayAttribFormat,VertexAttribFormat},
         *      @fn_gl2{VertexArrayAttribBinding,VertexAttribBinding} and
         *      @fn_gl2{VertexArrayVertexBuffer,BindVertexBuffer}
         * @requires_gles In WebGL the data must be properly aligned (e.g. all
         *      float data must start at addresses divisible by four). Also the
         *      maximum stride of attribute data must be at most 255 bytes.
         *      This is not required anywhere else, but doing so may have
         *      performance benefits.
         */
        template<class ...T
            #ifndef DOXYGEN_GENERATING_OUTPUT
            /* So it doesn't clash with the DynamicAttribute overload */
            , class = typename std::enable_if<!Implementation::IsDynamicAttribute<T...>::value>::type
            #endif
        > Mesh& addVertexBuffer(Buffer& buffer, GLintptr offset, const T&... attributes) {
            addVertexBufferInternal(buffer, offset, strideOfInterleaved(attributes...), 0, attributes...);
            return *this;
        }

        /**
         * @brief Add instanced vertex buffer
         * @return Reference to self (for method chaining)
         *
         * Similar to the above function, the @p divisor parameter specifies
         * number of instances that will pass until new data are fetched from
         * the buffer. Setting it to @cpp 0 @ce is equivalent to calling
         * @ref addVertexBuffer().
         *
         * If @gl_extension{ARB,vertex_array_object} (part of OpenGL 3.0), OpenGL
         * ES 3.0, WebGL 2.0, @gl_extension{OES,vertex_array_object} in OpenGL
         * ES 2.0 or @webgl_extension{OES,vertex_array_object} in WebGL 1.0 is
         * available, the vertex array object is used to hold the parameters.
         *
         * @see @ref setPrimitive(), @ref setCount(), @ref setInstanceCount(),
         *      @ref setBaseInstance(), @ref maxVertexAttributeStride(),
         *      @fn_gl{BindVertexArray}, @fn_gl_keyword{EnableVertexAttribArray},
         *      @fn_gl{BindBuffer}, @fn_gl_keyword{VertexAttribPointer},
         *      @fn_gl_keyword{VertexAttribDivisor} or
         *      @fn_gl2{EnableVertexArrayAttrib,EnableVertexAttribArray},
         *      @fn_gl2{VertexArrayAttribFormat,VertexAttribFormat},
         *      @fn_gl2{VertexArrayAttribBinding,VertexAttribBinding},
         *      @fn_gl2{VertexArrayVertexBuffer,BindVertexBuffer} and
         *      @fn_gl2{VertexArrayBindingDivisor,VertexBindingDivisor}
         * @requires_gl33 Extension @gl_extension{ARB,instanced_arrays}
         * @requires_gles30 Extension @gl_extension{ANGLE,instanced_arrays},
         *      @gl_extension{EXT,instanced_arrays} or
         *      @gl_extension{NV,instanced_arrays} in OpenGL ES 2.0.
         * @requires_webgl20 Extension @webgl_extension{ANGLE,instanced_arrays}
         *      in WebGL 1.0.
         */
        template<class ...T
            #ifndef DOXYGEN_GENERATING_OUTPUT
            /* So it doesn't clash with the DynamicAttribute overload */
            , class = typename std::enable_if<!Implementation::IsDynamicAttribute<T...>::value>::type
            #endif
        > inline Mesh& addVertexBufferInstanced(Buffer& buffer, UnsignedInt divisor, GLintptr offset, const T&... attributes) {
            addVertexBufferInternal(buffer, offset, strideOfInterleaved(attributes...), divisor, attributes...);
            return *this;
        }

        /**
         * @brief Add vertex buffer with dynamic vertex attributes
         * @return Reference to self (for method chaining)
         *
         * Equivalent to @ref addVertexBuffer(Buffer&, GLintptr, const T&... attributes)
         * but with the possibility to fully specify the attribute properties
         * at runtime, including base type and location. See
         * @ref GL-Mesh-configuration-dynamic "class documentation" for usage
         * example.
         */
        Mesh& addVertexBuffer(Buffer& buffer, GLintptr offset, GLsizei stride, const DynamicAttribute& attribute) {
            return addVertexBufferInstanced(buffer, 0, offset, stride, attribute);
        }

        /**
         * @brief Add instanced vertex buffer with dynamic vertex attributes
         * @return Reference to self (for method chaining)
         *
         * Equivalent to @ref addVertexBufferInstanced(Buffer&, UnsignedInt, GLintptr, const T&... attributes)
         * but with the possibility to fully specify the attribute properties
         * at runtime, including base type and location. See
         * @ref GL-Mesh-configuration-dynamic "class documentation" for usage
         * example.
         */
        Mesh& addVertexBufferInstanced(Buffer& buffer, UnsignedInt divisor, GLintptr offset, GLsizei stride, const DynamicAttribute& attribute);

        /**
         * @brief Add vertex buffer with ownership transfer
         * @return Reference to self (for method chaining)
         *
         * Unlike @ref addVertexBuffer(Buffer&, GLintptr, const T&... attributes)
         * this function takes ownership of @p buffer. See
         * @ref GL-Mesh-buffer-ownership for more information.
         */
        template<class ...T
            #ifndef DOXYGEN_GENERATING_OUTPUT
            /* So it doesn't clash with the DynamicAttribute overload */
            , class = typename std::enable_if<!Implementation::IsDynamicAttribute<T...>::value>::type
            #endif
        > inline Mesh& addVertexBuffer(Buffer&& buffer, GLintptr offset, const T&... attributes) {
            addVertexBuffer<T...>(buffer, offset, attributes...);
            acquireVertexBuffer(std::move(buffer));
            return *this;
        }

        /**
         * @brief Add instanced vertex buffer with ownership transfer
         * @return Reference to self (for method chaining)
         *
         * Unlike @ref addVertexBufferInstanced(Buffer&, UnsignedInt, GLintptr, const T&... attributes)
         * this function takes ownership of @p buffer. See
         * @ref GL-Mesh-buffer-ownership for more information.
         */
        template<class ...T
            #ifndef DOXYGEN_GENERATING_OUTPUT
            /* So it doesn't clash with the DynamicAttribute overload */
            , class = typename std::enable_if<!Implementation::IsDynamicAttribute<T...>::value>::type
            #endif
        > inline Mesh& addVertexBufferInstanced(Buffer&& buffer, UnsignedInt divisor, GLintptr offset, const T&... attributes) {
            addVertexBufferInstanced<T...>(buffer, divisor, offset, attributes...);
            acquireVertexBuffer(std::move(buffer));
            return *this;
        }

        /**
         * @brief Add vertex buffer with dynamic vertex attributes with ownership transfer
         * @return Reference to self (for method chaining)
         *
         * Unlike @ref addVertexBuffer(Buffer&, GLintptr, GLsizei, const DynamicAttribute&)
         * this function takes ownership of @p buffer. See
         * @ref GL-Mesh-buffer-ownership for more information.
         */
        Mesh& addVertexBuffer(Buffer&& buffer, GLintptr offset, GLsizei stride, const DynamicAttribute& attribute) {
            addVertexBuffer(buffer, offset, stride, attribute);
            acquireVertexBuffer(std::move(buffer));
            return *this;
        }

        /**
         * @brief Add instanced vertex buffer with dynamic vertex attributes with ownership transfer
         * @return Reference to self (for method chaining)
         *
         * Unlike @ref addVertexBufferInstanced(Buffer&, UnsignedInt, GLintptr, GLsizei, const DynamicAttribute&)
         * this function takes ownership of @p buffer. See
         * @ref GL-Mesh-buffer-ownership for more information.
         */
        Mesh& addVertexBufferInstanced(Buffer&& buffer, UnsignedInt divisor, GLintptr offset, GLsizei stride, const DynamicAttribute& attribute) {
            addVertexBufferInstanced(buffer, divisor, offset, stride, attribute);
            acquireVertexBuffer(std::move(buffer));
            return *this;
        }

        /**
         * @brief Set index buffer
         * @param buffer        Index buffer
         * @param offset        Offset into the buffer
         * @param type          Index data type
         * @param start         Minimum array index contained in the buffer
         * @param end           Maximum array index contained in the buffer
         * @return Reference to self (for method chaining)
         *
         * The smaller range is specified with @p start and @p end the less
         * memory operations are needed (and possibly some optimizations),
         * improving draw performance. Specifying @cpp 0 @ce for both
         * parameters behaves the same as @ref setIndexBuffer(Buffer&, GLintptr, MeshIndexType).
         * On OpenGL ES 2.0 this function behaves always as
         * @ref setIndexBuffer(Buffer&, GLintptr, MeshIndexType), as this
         * functionality is not available there.
         *
         * If @gl_extension{ARB,vertex_array_object} (part of OpenGL 3.0), OpenGL
         * ES 3.0, WebGL 2.0, @gl_extension{OES,vertex_array_object} in OpenGL
         * ES 2.0 or @webgl_extension{OES,vertex_array_object} in WebGL 1.0 is
         * available, the vertex array object is used to hold the parameters.
         *
         * Ignored when calling @ref AbstractShaderProgram::drawTransformFeedback().
         * @see @ref maxElementIndex(), @ref maxElementsIndices(),
         *      @ref maxElementsVertices(), @ref setCount(), @ref isIndexed(),
         *      @fn_gl{BindVertexArray}, @fn_gl{BindBuffer} or
         *      @fn_gl_keyword{VertexArrayElementBuffer}
         */
        Mesh& setIndexBuffer(Buffer& buffer, GLintptr offset, MeshIndexType type, UnsignedInt start, UnsignedInt end);

        /** @overload */
        Mesh& setIndexBuffer(Buffer& buffer, GLintptr offset, Magnum::MeshIndexType type, UnsignedInt start, UnsignedInt end) {
            return setIndexBuffer(buffer, offset, meshIndexType(type), start, end);
        }

        /**
         * @brief Set index buffer
         * @param buffer        Index buffer
         * @param offset        Offset into the buffer
         * @param type          Index data type
         * @return Reference to self (for method chaining)
         *
         * Alternative to @ref setIndexBuffer(Buffer&, GLintptr, MeshIndexType, UnsignedInt, UnsignedInt)
         * with unspecified index limits, see its documentation for more
         * information. Prefer to set index limits for better performance.
         */
        Mesh& setIndexBuffer(Buffer& buffer, GLintptr offset, MeshIndexType type) {
            return setIndexBuffer(buffer, offset, type, 0, 0);
        }

        /** @overload */
        Mesh& setIndexBuffer(Buffer& buffer, GLintptr offset, Magnum::MeshIndexType type) {
            return setIndexBuffer(buffer, offset, meshIndexType(type), 0, 0);
        }

        /**
         * @brief Set index buffer with ownership transfer
         *
         * Unlike @ref setIndexBuffer(Buffer&, GLintptr, MeshIndexType, UnsignedInt, UnsignedInt)
         * this function takes ownership of @p buffer. See
         * @ref GL-Mesh-buffer-ownership for more information.
         */
        Mesh& setIndexBuffer(Buffer&& buffer, GLintptr offset, MeshIndexType type, UnsignedInt start, UnsignedInt end);

        /** @overload */
        Mesh& setIndexBuffer(Buffer&& buffer, GLintptr offset, Magnum::MeshIndexType type, UnsignedInt start, UnsignedInt end) {
            return setIndexBuffer(std::move(buffer), offset, meshIndexType(type), start, end);
        }

        /**
         * @brief Set index buffer with ownership transfer
         *
         * Unlike @ref setIndexBuffer(Buffer&, GLintptr, MeshIndexType) this
         * function takes ownership of @p buffer. See
         * @ref GL-Mesh-buffer-ownership for more information.
         */
        Mesh& setIndexBuffer(Buffer&& buffer, GLintptr offset, MeshIndexType type) {
            return setIndexBuffer(std::move(buffer), offset, type, 0, 0);
        }
        Mesh& setIndexBuffer(Buffer&& buffer, GLintptr offset, Magnum::MeshIndexType type) {
            return setIndexBuffer(std::move(buffer), offset, meshIndexType(type), 0, 0);
        } /**< @overload */

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Draw the mesh
         * @m_deprecated_since{2020,06} Use @ref AbstractShaderProgram::draw()
         *      instead.
         */
        CORRADE_DEPRECATED("use AbstractShaderProgram::draw() instead") Mesh& draw(AbstractShaderProgram& shader);

        /**
         * @overload
         * @m_deprecated_since{2020,06} Use @ref AbstractShaderProgram::draw()
         *      instead.
         */
        CORRADE_DEPRECATED("use AbstractShaderProgram::draw() instead") Mesh& draw(AbstractShaderProgram&& shader);

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Draw the mesh with vertices coming out of transform feedback
         * @m_deprecated_since{2020,06} Use
         *      @ref AbstractShaderProgram::drawTransformFeedback() instead.
         */
        CORRADE_DEPRECATED("use AbstractShaderProgram::drawTransformFeedback() instead") Mesh& draw(AbstractShaderProgram& shader, TransformFeedback& xfb, UnsignedInt stream = 0);

        /**
         * @overload
         * @m_deprecated_since{2020,06} Use
         *      @ref AbstractShaderProgram::drawTransformFeedback() instead.
         */
        CORRADE_DEPRECATED("use AbstractShaderProgram::drawTransformFeedback() instead") Mesh& draw(AbstractShaderProgram&& shader, TransformFeedback& xfb, UnsignedInt stream = 0);
        #endif
        #endif

    private:
        friend AbstractShaderProgram;
        friend MeshView;
        friend Implementation::MeshState;

        struct MAGNUM_GL_LOCAL AttributeLayout;

        explicit Mesh(GLuint id, MeshPrimitive primitive, ObjectFlags flags);

        void MAGNUM_GL_LOCAL createIfNotAlready();

        #ifndef MAGNUM_TARGET_WEBGL
        Mesh& setLabelInternal(Containers::ArrayView<const char> label);
        #endif

        /* Computing stride of interleaved vertex attributes */
        template<UnsignedInt location, class T, class ...U> static GLsizei strideOfInterleaved(const Attribute<location, T>& attribute, const U&... attributes) {
            return attribute.vectorStride()*Attribute<location, T>::Vectors + strideOfInterleaved(attributes...);
        }
        template<class ...T> static GLsizei strideOfInterleaved(GLintptr gap, const T&... attributes) {
            return gap + strideOfInterleaved(attributes...);
        }
        static GLsizei strideOfInterleaved() { return 0; }

        /* Adding interleaved vertex attributes */
        template<UnsignedInt location, class T, class ...U> void addVertexBufferInternal(Buffer& buffer, GLintptr offset, GLsizei stride, GLuint divisor, const Attribute<location, T>& attribute, const U&... attributes) {
            addVertexAttribute(buffer, attribute, offset, stride, divisor);

            /* Add size of this attribute to offset for next attribute */
            addVertexBufferInternal(buffer, offset+attribute.vectorStride()*Attribute<location, T>::Vectors, stride, divisor, attributes...);
        }
        template<class ...T> void addVertexBufferInternal(Buffer& buffer, GLintptr offset, GLsizei stride, GLuint divisor, GLintptr gap, const T&... attributes) {
            /* Add the gap to offset for next attribute */
            addVertexBufferInternal(buffer, offset+gap, stride, divisor, attributes...);
        }
        void addVertexBufferInternal(Buffer&, GLintptr, GLsizei, GLuint) {}

        template<UnsignedInt location, class T> void addVertexAttribute(typename std::enable_if<std::is_same<typename Implementation::Attribute<T>::ScalarType, Float>::value, Buffer&>::type buffer, const Attribute<location, T>& attribute, GLintptr offset, GLsizei stride, GLuint divisor) {
            for(UnsignedInt i = 0; i != Attribute<location, T>::Vectors; ++i)
                attributePointerInternal(buffer,
                    location+i,
                    GLint(attribute.components()),
                    GLenum(attribute.dataType()),
                    Implementation::kindFor<location, T>(attribute.dataOptions()),
                    GLintptr(offset+i*attribute.vectorStride()),
                    stride,
                    divisor);
        }

        #ifndef MAGNUM_TARGET_GLES2
        template<UnsignedInt location, class T> void addVertexAttribute(typename std::enable_if<std::is_integral<typename Implementation::Attribute<T>::ScalarType>::value, Buffer&>::type buffer, const Attribute<location, T>& attribute, GLintptr offset, GLsizei stride, GLuint divisor) {
            attributePointerInternal(buffer,
                location,
                GLint(attribute.components()),
                GLenum(attribute.dataType()),
                Implementation::kindFor<location, T>(attribute.dataOptions()),
                offset,
                stride,
                divisor);
        }

        #ifndef MAGNUM_TARGET_GLES
        template<UnsignedInt location, class T> void addVertexAttribute(typename std::enable_if<std::is_same<typename Implementation::Attribute<T>::ScalarType, Double>::value, Buffer&>::type buffer, const Attribute<location, T>& attribute, GLintptr offset, GLsizei stride, GLuint divisor) {
            for(UnsignedInt i = 0; i != Attribute<location, T>::Vectors; ++i)
                attributePointerInternal(buffer,
                    location+i,
                    GLint(attribute.components()),
                    GLenum(attribute.dataType()),
                    Implementation::kindFor<location, T>(attribute.dataOptions()),
                    GLintptr(offset+i*attribute.vectorStride()),
                    stride,
                    divisor);
        }
        #endif
        #endif

        /* Unconditionally binds a specified VAO and updates the state tracker.
           Used also in Buffer::bindSomewhereInternal() and Context::resetState(). */
        static void MAGNUM_GL_LOCAL bindVAOImplementationDefault(GLuint id);
        static void MAGNUM_GL_LOCAL bindVAOImplementationVAO(GLuint id);

        void MAGNUM_GL_LOCAL bindVAO();

        #ifndef MAGNUM_TARGET_GLES
        void drawInternal(Int count, Int baseVertex, Int instanceCount, UnsignedInt baseInstance, GLintptr indexOffset, Int indexStart, Int indexEnd);
        #elif !defined(MAGNUM_TARGET_GLES2)
        void drawInternal(Int count, Int baseVertex, Int instanceCount, GLintptr indexOffset, Int indexStart, Int indexEnd);
        #else
        void drawInternal(Int count, Int baseVertex, Int instanceCount, GLintptr indexOffset);
        #endif

        #ifndef MAGNUM_TARGET_GLES
        void drawInternal(TransformFeedback& xfb, UnsignedInt stream, Int instanceCount);
        #endif

        void MAGNUM_GL_LOCAL createImplementationDefault(bool);
        void MAGNUM_GL_LOCAL createImplementationVAO(bool createObject);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_GL_LOCAL createImplementationVAODSA(bool createObject);
        #endif

        void MAGNUM_GL_LOCAL moveConstructImplementationDefault(Mesh&& other);
        void MAGNUM_GL_LOCAL moveConstructImplementationVAO(Mesh&& other);
        void MAGNUM_GL_LOCAL moveAssignImplementationDefault(Mesh&& other);
        void MAGNUM_GL_LOCAL moveAssignImplementationVAO(Mesh&& other);

        void MAGNUM_GL_LOCAL destroyImplementationDefault(bool);
        void MAGNUM_GL_LOCAL destroyImplementationVAO(bool deleteObject);

        void attributePointerInternal(const Buffer& buffer, GLuint location, GLint size, GLenum type, DynamicAttribute::Kind kind, GLintptr offset, GLsizei stride, GLuint divisor);
        void MAGNUM_GL_LOCAL attributePointerInternal(AttributeLayout&& attribute);
        void MAGNUM_GL_LOCAL attributePointerImplementationDefault(AttributeLayout&& attribute);
        void MAGNUM_GL_LOCAL attributePointerImplementationVAO(AttributeLayout&& attribute);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_GL_LOCAL attributePointerImplementationVAODSA(AttributeLayout&& attribute);
        #ifdef CORRADE_TARGET_WINDOWS
        void MAGNUM_GL_LOCAL attributePointerImplementationVAODSAIntelWindows(AttributeLayout&& attribute);
        #endif
        #endif
        void MAGNUM_GL_LOCAL vertexAttribPointer(AttributeLayout& attribute);

        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_GL_LOCAL vertexAttribDivisorImplementationVAO(GLuint index, GLuint divisor);
        void MAGNUM_GL_LOCAL vertexAttribDivisorImplementationVAODSA(GLuint index, GLuint divisor);
        #elif defined(MAGNUM_TARGET_GLES2)
        void MAGNUM_GL_LOCAL vertexAttribDivisorImplementationANGLE(GLuint index, GLuint divisor);
        #ifndef MAGNUM_TARGET_WEBGL
        void MAGNUM_GL_LOCAL vertexAttribDivisorImplementationEXT(GLuint index, GLuint divisor);
        void MAGNUM_GL_LOCAL vertexAttribDivisorImplementationNV(GLuint index, GLuint divisor);
        #endif
        #endif

        void acquireVertexBuffer(Buffer&& buffer);
        void MAGNUM_GL_LOCAL acquireVertexBufferImplementationDefault(Buffer&& buffer);
        void MAGNUM_GL_LOCAL acquireVertexBufferImplementationVAO(Buffer&& buffer);

        void MAGNUM_GL_LOCAL bindIndexBufferImplementationDefault(Buffer&);
        void MAGNUM_GL_LOCAL bindIndexBufferImplementationVAO(Buffer& buffer);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_GL_LOCAL bindIndexBufferImplementationVAODSA(Buffer& buffer);
        #endif

        void MAGNUM_GL_LOCAL bindImplementationDefault();
        void MAGNUM_GL_LOCAL bindImplementationVAO();

        void MAGNUM_GL_LOCAL unbindImplementationDefault();
        void MAGNUM_GL_LOCAL unbindImplementationVAO();

        #ifdef MAGNUM_TARGET_GLES2
        void MAGNUM_GL_LOCAL drawArraysInstancedImplementationANGLE(GLint baseVertex, GLsizei count, GLsizei instanceCount);
        #ifndef MAGNUM_TARGET_WEBGL
        void MAGNUM_GL_LOCAL drawArraysInstancedImplementationEXT(GLint baseVertex, GLsizei count, GLsizei instanceCount);
        void MAGNUM_GL_LOCAL drawArraysInstancedImplementationNV(GLint baseVertex, GLsizei count, GLsizei instanceCount);
        #endif

        void MAGNUM_GL_LOCAL drawElementsInstancedImplementationANGLE(GLsizei count, GLintptr indexOffset, GLsizei instanceCount);
        #ifndef MAGNUM_TARGET_WEBGL
        void MAGNUM_GL_LOCAL drawElementsInstancedImplementationEXT(GLsizei count, GLintptr indexOffset, GLsizei instanceCount);
        void MAGNUM_GL_LOCAL drawElementsInstancedImplementationNV(GLsizei count, GLintptr indexOffset, GLsizei instanceCount);
        #endif
        #endif

        /* _id, _primitive, _flags set from constructors */
        GLuint _id;
        MeshPrimitive _primitive;
        ObjectFlags _flags;
        /* using a separate bool for _count instead of Optional to make use of
           the 3-byte gap after _flags */
        bool _countSet{};
        /* Whether the _attributes storage was constructed (it's not when the
           object is constructed using NoCreate). Also fits in the gap. */
        bool _constructed{};
        Int _count{}, _baseVertex{}, _instanceCount{1};
        #ifndef MAGNUM_TARGET_GLES
        UnsignedInt _baseInstance{};
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        UnsignedInt _indexStart{}, _indexEnd{};
        #endif
        GLintptr _indexOffset{};
        MeshIndexType _indexType{};
        Buffer _indexBuffer{NoCreate};

        /* Storage for either std::vector<AttributeLayout> (in case of no VAOs)
           or std::vector<Buffer> (in case of VAOs). 4 pointers should be one
           pointer more than enough. */
        struct { std::intptr_t data[4]; } _attributes;
};

/** @debugoperatorenum{MeshPrimitive} */
MAGNUM_GL_EXPORT Debug& operator<<(Debug& debug, MeshPrimitive value);

/** @debugoperatorenum{MeshIndexType} */
MAGNUM_GL_EXPORT Debug& operator<<(Debug& debug, MeshIndexType value);

inline GLuint Mesh::release() {
    const GLuint id = _id;
    _id = 0;
    return id;
}

}}

#endif
