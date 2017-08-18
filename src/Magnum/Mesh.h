#ifndef Magnum_Mesh_h
#define Magnum_Mesh_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
              Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class @ref Magnum::Mesh
 */

#include <vector>
#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Utility/ConfigurationValue.h>

#include "Magnum/AbstractObject.h"
#include "Magnum/Attribute.h"
#include "Magnum/Tags.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Utility/Macros.h>
#endif

namespace Magnum {

/**
 * @brief Mesh primitive type
 *
 * @see @ref Mesh::primitive(), @ref Mesh::setPrimitive()
 */
enum class MeshPrimitive: GLenum {
    /** Single points. */
    Points = GL_POINTS,

    /**
     * First two vertices define first line segment, each following
     * vertex defines another segment.
     */
    LineStrip = GL_LINE_STRIP,

    /** Line strip, last and first vertex are connected together. */
    LineLoop = GL_LINE_LOOP,

    /**
     * Each pair of vertices defines a single line, lines aren't
     * connected together.
     */
    Lines = GL_LINES,

    #ifndef MAGNUM_TARGET_GLES
    /**
     * Line strip with adjacency information.
     * @requires_gl32 Extension @extension{ARB,geometry_shader4}
     * @requires_gl Geometry shaders are not available in OpenGL ES or WebGL.
     */
    LineStripAdjacency = GL_LINE_STRIP_ADJACENCY,

    /**
     * Lines with adjacency information.
     * @requires_gl32 Extension @extension{ARB,geometry_shader4}
     * @requires_gl Geometry shaders are not available in OpenGL ES or WebGL.
     */
    LinesAdjacency = GL_LINES_ADJACENCY,
    #endif

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

    /** Each three vertices define one triangle. */
    Triangles = GL_TRIANGLES,

    #ifndef MAGNUM_TARGET_GLES
    /**
     * Triangle strip with adjacency information.
     * @requires_gl32 Extension @extension{ARB,geometry_shader4}
     * @requires_gl Geometry shaders are not available in OpenGL ES or WebGL.
     */
    TriangleStripAdjacency = GL_TRIANGLE_STRIP_ADJACENCY,

    /**
     * Triangles with adjacency information.
     * @requires_gl32 Extension @extension{ARB,geometry_shader4}
     * @requires_gl Geometry shaders are not available in OpenGL ES or WebGL.
     */
    TrianglesAdjacency = GL_TRIANGLES_ADJACENCY,

    /**
     * Patches.
     * @requires_gl40 Extension @extension{ARB,tessellation_shader}
     * @requires_gl Tessellation shaders are not available in OpenGL ES or
     *      WebGL.
     */
    Patches = GL_PATCHES
    #endif
};

namespace Implementation { struct MeshState; }

/**
@brief Mesh

@anchor Mesh-configuration
## Mesh configuration

You have to specify at least primitive and vertex/index count using
@ref setPrimitive() and @ref setCount(). Then fill your vertex buffers with
data, add them to the mesh and specify @ref Attribute "shader attribute" layout
inside the buffers using @ref addVertexBuffer(). You can also use
@ref MeshTools::interleave() to conveniently interleave vertex data.

If you want indexed mesh, fill your index buffer with data and specify its
layout using @ref setIndexBuffer(). You can also use @ref MeshTools::compressIndices()
to conveniently compress the indices based on the range used.

There is also @ref MeshTools::compile() function which operates directly on
@ref Trade::MeshData2D / @ref Trade::MeshData3D and returns fully configured
mesh and vertex/index buffers for use with stock shaders.

Note that neither vertex buffers nor index buffer is managed (e.g. deleted on
destruction) by the mesh, so you have to manage them on your own and ensure
that they are available for whole mesh lifetime. On the other hand it allows
you to use one buffer for more meshes (each mesh for example configured for
different usage) or store data for more meshes in one buffer.

If vertex/index count or instance count is zero, the mesh is empty and no draw
commands are issued when calling @ref draw().

### Example mesh configuration

#### Basic non-indexed mesh

@code
// Custom shader, needing only position data
class MyShader: public AbstractShaderProgram {
    public:
        typedef Attribute<0, Vector3> Position;

    // ...
};

// Fill vertex buffer with position data
static constexpr Vector3 positions[30] = {
    // ...
};
Buffer vertexBuffer;
vertexBuffer.setData(positions, BufferUsage::StaticDraw);

// Configure the mesh, add vertex buffer
Mesh mesh;
mesh.setPrimitive(MeshPrimitive::Triangles)
    .setCount(30)
    .addVertexBuffer(vertexBuffer, 0, MyShader::Position{});
@endcode

#### Interleaved vertex data

@code
// Non-indexed primitive with positions and normals
Trade::MeshData3D plane = Primitives::Plane::solid();

// Fill vertex buffer with interleaved position and normal data
Buffer vertexBuffer;
vertexBuffer.setData(MeshTools::interleave(plane.positions(0), plane.normals(0)), BufferUsage::StaticDraw);

// Configure the mesh, add vertex buffer
Mesh mesh;
mesh.setPrimitive(plane.primitive())
    .setCount(plane.positions(0).size())
    .addVertexBuffer(buffer, 0, Shaders::Phong::Position{}, Shaders::Phong::Normal{});
@endcode

#### Indexed mesh

@code
// Custom shader
class MyShader: public AbstractShaderProgram {
    public:
        typedef Attribute<0, Vector3> Position;

    // ...
};

// Fill vertex buffer with position data
static constexpr Vector3 positions[300] = {
    // ...
};
Buffer vertexBuffer;
vertexBuffer.setData(positions, BufferUsage::StaticDraw);

// Fill index buffer with index data
static constexpr GLubyte indices[75] = {
    // ...
};
Buffer indexBuffer;
indexBuffer.setData(indices, BufferUsage::StaticDraw);

// Configure the mesh, add both vertex and index buffer
Mesh mesh;
mesh.setPrimitive(MeshPrimitive::Triangles)
    .setCount(75)
    .addVertexBuffer(vertexBuffer, 0, MyShader::Position{})
    .setIndexBuffer(indexBuffer, 0, Mesh::IndexType::UnsignedByte, 176, 229);
@endcode

Or using @ref MeshTools::interleave() and @ref MeshTools::compressIndices():

@code
// Indexed primitive
Trade::MeshData3D cube = Primitives::Cube::solid();

// Fill vertex buffer with interleaved position and normal data
Buffer vertexBuffer;
vertexBuffer.setData(MeshTools::interleave(cube.positions(0), cube.normals(0)), BufferUsage::StaticDraw);

// Compress index data
Containers::Array<char> indexData;
Mesh::IndexType indexType;
UnsignedInt indexStart, indexEnd;
std::tie(indexData, indexType, indexStart, indexEnd) = MeshTools::compressIndices(cube.indices());

// Fill index buffer
Buffer indexBuffer;
indexBuffer.setData(data);

// Configure the mesh, add both vertex and index buffer
Mesh mesh;
mesh.setPrimitive(plane.primitive())
    .setCount(cube.indices().size())
    .addVertexBuffer(vertexBuffer, 0, Shaders::Phong::Position{}, Shaders::Phong::Normal{})
    .setIndexBuffer(indexBuffer, 0, indexType, indexStart, indexEnd);
@endcode

Or, if you plan to use the mesh with stock shaders, you can just use
@ref MeshTools::compile().

#### Specific formats of vertex data

@code
// Custom shader with colors specified as four floating-point values
class MyShader: public AbstractShaderProgram {
    public:
        typedef Attribute<0, Vector3> Position;
        typedef Attribute<1, Color4> Color;

    // ...
};

// Initial mesh configuration
Mesh mesh;
mesh.setPrimitive(...)
    .setCount(30);

// Fill position buffer with positions specified as two-component XY (i.e.,
// no Z component, which is meant to be always 0)
Vector2 positions[30] = {
    // ...
};
Buffer positionBuffer;
positionBuffer.setData(positions, BufferUsage::StaticDraw);

// Specify layout of positions buffer -- only two components, unspecified Z
// component will be automatically set to 0
mesh.addVertexBuffer(positionBuffer, 0,
    MyShader::Position{MyShader::Position::Components::Two});

// Fill color buffer with colors specified as four-byte BGRA (e.g. directly
// from TGA file)
GLubyte colors[4*30] = {
    // ...
};
Buffer colorBuffer;
colorBuffer.setData(colors, BufferUsage::StaticDraw);

// Specify layout of color buffer -- BGRA, each component unsigned byte and we
// want to normalize them from [0, 255] to [0.0f, 1.0f]
mesh.addVertexBuffer(colorBuffer, 0, MyShader::Color{
    MyShader::Color::Components::BGRA,
    MyShader::Color::DataType::UnsignedByte,
    MyShader::Color::DataOption::Normalized});
@endcode

@anchor Mesh-configuration-dynamic
#### Dynamically specified attributes

In some cases, for example when the shader code is fully generated at runtime,
it's not possible to know attribute locations and types at compile time. In
that case, there are overloads of @ref addVertexBuffer() and
@ref addVertexBufferInstanced() that take @ref DynamicAttribute instead of
@ref Attribute typedefs. Adding a RGB attribute at location 3 normalized from
unsigned byte to float with one byte padding at the end could then look like
this:
@code
mesh.addVertexBuffer(colorBuffer, 0, 4, DynamicAttribute{
    DynamicAttribute::Kind::GenericNormalized, 3,
    DynamicAttribute::Components::Three,
    DynamicAttribute::DataType::UnsignedByte});
});
@endcode

## Rendering meshes

Basic workflow is: bind specific framebuffer for drawing (if needed), set up
respective shader (see
@ref AbstractShaderProgram-rendering-workflow "AbstractShaderProgram documentation"
for more infromation) and call @ref Mesh::draw().

## WebGL restrictions

@ref MAGNUM_TARGET_WEBGL "WebGL" puts some restrictions on vertex buffer
layout, see @ref addVertexBuffer() documentation for details.

@anchor Mesh-performance-optimization
## Performance optimizations

If @extension{ARB,vertex_array_object} (part of OpenGL 3.0), OpenGL ES 3.0,
WebGL 2.0, @extension{OES,vertex_array_object} in OpenGL ES 2.0 or
@webgl_extension{OES,vertex_array_object} in WebGL 1.0 is supported, VAOs are
used instead of binding the buffers and specifying vertex attribute pointers
in each @ref draw() call. The engine tracks currently bound VAO and currently
active shader program to avoid unnecessary calls to @fn_gl{BindVertexArray} and
@fn_gl{UseProgram}. Mesh limits and implementation-defined values (such as
@ref maxElementIndex()) are cached, so repeated queries don't result in
repeated @fn_gl{Get} calls.

If @extension{EXT,direct_state_access} desktop extension and VAOs are
available, DSA functions are used for specifying attribute locations to avoid
unnecessary calls to @fn_gl{BindBuffer} and @fn_gl{BindVertexArray}. See
documentation of @ref addVertexBuffer() for more information.

If index range is specified in @ref setIndexBuffer(), range-based version of
drawing commands are used on desktop OpenGL and OpenGL ES 3.0. See also
@ref draw() for more information.
 */
class MAGNUM_EXPORT Mesh: public AbstractObject {
    friend MeshView;
    friend Implementation::MeshState;

    public:
        /**
         * @brief Index type
         *
         * @see @ref setIndexBuffer(), @ref indexSize()
         */
        enum class IndexType: GLenum {
            UnsignedByte = GL_UNSIGNED_BYTE,    /**< Unsigned byte */
            UnsignedShort = GL_UNSIGNED_SHORT,  /**< Unsigned short */

            /**
             * Unsigned int
             * @requires_gles30 Extension @extension{OES,element_index_uint}
             *       in OpenGL ES 2.0.
             * @requires_webgl20 Extension @webgl_extension{OES,element_index_uint}
             *      in WebGL 1.0.
             */
            UnsignedInt = GL_UNSIGNED_INT
        };

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @copybrief AbstractShaderProgram::maxVertexAttributes()
         * @deprecated Use @ref AbstractShaderProgram::maxVertexAttributes() instead.
         */
        CORRADE_DEPRECATED("use AbstractShaderProgram::maxVertexAttributes() instead") static Int maxVertexAttributes();
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Max supported index value
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If extension @extension{ARB,ES3_compatibility} (part
         * of OpenGL 4.3) is not available, returns max representable 32-bit
         * value (@f$ 2^32 - 1 @f$).
         * @see @ref setIndexBuffer(), @fn_gl{Get} with @def_gl{MAX_ELEMENT_INDEX}
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
         * @see @ref setIndexBuffer(), @fn_gl{Get} with @def_gl{MAX_ELEMENTS_INDICES}
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
         * @see @ref setIndexBuffer(), @fn_gl{Get} with @def_gl{MAX_ELEMENTS_VERTICES}
         * @requires_gles30 Ranged element draw is not supported in OpenGL ES
         *      2.0.
         * @requires_webgl20 Ranged element draw is not supported in WebGL 1.0.
         */
        static Int maxElementsVertices();
        #endif

        /**
         * @brief Size of given index type
         *
         * @see @ref indexSize() const
         */
        static std::size_t indexSize(IndexType type);

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
         * @requires_gl30 Extension @extension{ARB,vertex_array_object}
         * @requires_gles30 Extension @extension{OES,vertex_array_object} in
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
         * If @extension{ARB,vertex_array_object} (part of OpenGL 3.0), OpenGL
         * ES 3.0, WebGL 2.0, @extension{OES,vertex_array_object} in OpenGL
         * ES 2.0 or @webgl_extension{OES,vertex_array_object} in WebGL 1.0 is
         * available, vertex array object is created. If @extension{ARB,direct_state_access}
         * (part of OpenGL 4.5) is not available, the vertex array object is
         * created on first use.
         * @see @ref Mesh(NoCreateT), @ref wrap(), @ref setPrimitive(),
         *      @ref setCount(), @fn_gl{CreateVertexArrays}, eventually
         *      @fn_gl{GenVertexArrays}
         */
        explicit Mesh(MeshPrimitive primitive = MeshPrimitive::Triangles);

        /**
         * @brief Construct without creating the underlying OpenGL object
         *
         * The constructed instance is equivalent to moved-from state. Useful
         * in cases where you will overwrite the instance later anyway. Move
         * another object over it to make it useful.
         *
         * This function can be safely used for constructing (and later
         * destructing) objects even without any OpenGL context being active.
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
         * If @extension{ARB,vertex_array_object} (part of OpenGL 3.0), OpenGL
         * ES 3.0, WebGL 2.0, @extension{OES,vertex_array_object} in OpenGL
         * ES 2.0 or @webgl_extension{OES,vertex_array_object} in WebGL 1.0 is
         * available, associated vertex array object is deleted.
         * @see @ref wrap(), @ref release(), @fn_gl{DeleteVertexArrays}
         */
        ~Mesh();

        /** @brief Copying is not allowed */
        Mesh& operator=(const Mesh&) = delete;

        /** @brief Move assignment */
        Mesh& operator=(Mesh&& other) noexcept;

        /**
         * @brief OpenGL vertex array ID
         *
         * If neither @extension{ARB,vertex_array_object} (part of OpenGL 3.0)
         * nor OpenGL ES 3.0 / WebGL 2.0 nor @extension{OES,vertex_array_object}
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
         * @requires_gl30 Extension @extension{ARB,vertex_array_object}
         * @requires_gles30 Extension @extension{OES,vertex_array_object} in
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
         * OpenGL calls. If OpenGL 4.3 is not supported and neither
         * @extension{KHR,debug} (covered also by @extension{ANDROID,extension_pack_es31a})
         * nor @extension{EXT,debug_label} desktop or ES extension is
         * available, this function returns empty string.
         * @see @fn_gl{GetObjectLabel} with @def_gl{VERTEX_ARRAY} or
         *      @fn_gl_extension{GetObjectLabel,EXT,debug_label} with
         *      @def_gl{VERTEX_ARRAY_OBJECT_EXT}
         * @requires_gles Debug output is not available in WebGL.
         */
        std::string label();

        /**
         * @brief Set mesh label
         * @return Reference to self (for method chaining)
         *
         * Default is empty string. If OpenGL 4.3 is not supported and neither
         * @extension{KHR,debug} (covered also by @extension{ANDROID,extension_pack_es31a})
         * nor @extension{EXT,debug_label} desktop or ES extension is
         * available, this function does nothing.
         * @see @ref maxLabelLength(), @fn_gl{ObjectLabel} with
         *      @def_gl{VERTEX_ARRAY} or @fn_gl_extension{LabelObject,EXT,debug_label}
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
        bool isIndexed() const { return _indexBuffer; }

        /**
         * @brief Index size
         *
         * @see @ref indexSize(IndexType)
         */
        std::size_t indexSize() const { return indexSize(_indexType); }

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

        /** @brief Vertex/index count */
        Int count() const { return _count; }

        /**
         * @brief Set vertex/index count
         * @return Reference to self (for method chaining)
         *
         * If the mesh is indexed, the value is treated as index count,
         * otherwise the value is vertex count. If set to `0`, no draw commands
         * are issued when calling @ref draw(AbstractShaderProgram&). Ignored
         * when calling @ref draw(AbstractShaderProgram&, TransformFeedback&, UnsignedInt).
         * Default is `0`.
         * @see @ref isIndexed(), @ref setBaseVertex(), @ref setInstanceCount()
         */
        Mesh& setCount(Int count) {
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
         * @ref draw(AbstractShaderProgram&, TransformFeedback&, UnsignedInt).
         * Default is `0`.
         * @see @ref setCount(), @ref setBaseInstance()
         * @requires_gl32 Extension @extension{ARB,draw_elements_base_vertex}
         *      for indexed meshes
         * @requires_gl Base vertex cannot be specified for indexed meshes in
         *      OpenGL ES or WebGL.
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
         * If set to `1`, non-instanced draw commands are issued when calling
         * @ref draw(AbstractShaderProgram&) or
         * @ref draw(AbstractShaderProgram&, TransformFeedback&, UnsignedInt).
         * If set to `0`, no draw commands are issued altogether. Default is
         * `1`.
         * @see @ref setBaseInstance(), @ref setCount(),
         *      @ref addVertexBufferInstanced()
         * @requires_gl31 Extension @extension{ARB,draw_instanced} if using
         *      @ref draw(AbstractShaderProgram&)
         * @requires_gl42 Extension @extension{ARB,transform_feedback_instanced}
         *      if using @ref draw(AbstractShaderProgram&, TransformFeedback&, UnsignedInt)
         * @requires_gles30 Extension @extension{ANGLE,instanced_arrays},
         *      @extension2{EXT,draw_instanced,draw_instanced} or
         *      @extension{NV,draw_instanced} in OpenGL ES 2.0.
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
         * Ignored when calling @ref draw(AbstractShaderProgram&, TransformFeedback&, UnsignedInt).
         * Default is `0`.
         * @see @ref setInstanceCount(), @ref setBaseVertex()
         * @requires_gl42 Extension @extension{ARB,base_instance}
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
         * See @ref Mesh-configuration "class documentation" for simple usage
         * example. For more involved example imagine that you have buffer
         * with 76 bytes of some other data at the beginning (possibly material
         * configuration) and then the interleaved vertex array. Each vertex
         * consists of weight, position, texture coordinate and normal. You
         * want to draw it with @ref Shaders::Phong, but it accepts only
         * position and normal, so you have to skip weight and texture
         * coordinate in each vertex:
         * @code
         * Buffer buffer;
         * Mesh mesh;
         * mesh.addVertexBuffer(buffer, 76, // initial array offset
         *     4,                           // skip vertex weight (Float)
         *     Shaders::Phong::Position(),  // vertex position
         *     8,                           // skip texture coordinates (Vector2)
         *     Shaders::Phong::Normal());   // vertex normal
         * @endcode
         *
         * You can also achieve the same effect by calling @ref addVertexBuffer()
         * more times with explicitly specified gaps before and after the
         * attributes. This can be used for e.g. runtime-dependent
         * configuration, as it isn't dependent on the variadic template:
         * @code
         * mesh.addVertexBuffer(buffer, 76, 4, Shaders::Phong::Position(), 20)
         *     .addVertexBuffer(buffer, 76, 24, Shaders::Phong::Normal(), 0);
         * @endcode
         *
         * If specifying more than one attribute, the function assumes that
         * the array is interleaved. Adding non-interleaved vertex buffer can
         * be done by specifying one attribute at a time with specific offset.
         * Above example with weight, position, texture coordinate and normal
         * arrays one after another (non-interleaved):
         * @code
         * Int vertexCount = 352;
         * mesh.addVertexBuffer(buffer, 76 + 4*vertexCount, Shaders::Phong::Position())
         *     .addVertexBuffer(buffer, 76 + 24*vertexCount, Shaders::Phong::Normal());
         * @endcode
         *
         * If @extension{ARB,vertex_array_object} (part of OpenGL 3.0), OpenGL
         * ES 3.0, WebGL 2.0, @extension{OES,vertex_array_object} in OpenGL
         * ES 2.0 or @webgl_extension{OES,vertex_array_object} in WebGL 1.0 is
         * available, the vertex array object is used to hold the parameters.
         *
         * @attention The buffer passed as parameter is not managed by the
         *      mesh, you must ensure it will exist for whole lifetime of the
         *      mesh and delete it afterwards.
         *
         * @see @ref addVertexBufferInstanced(), @ref setPrimitive(),
         *      @ref setCount(), @fn_gl{BindVertexArray},
         *      @fn_gl{EnableVertexAttribArray}, @fn_gl{BindBuffer},
         *      @fn_gl{VertexAttribPointer} or
         *      @fn_gl_extension{EnableVertexArrayAttrib,EXT,direct_state_access},
         *      @fn_gl_extension{VertexArrayVertexAttribOffset,EXT,direct_state_access}
         * @requires_gles In WebGL the data must be properly aligned (e.g. all
         *      float data must start at addresses divisible by four). Also the
         *      maximum stride of attribute data must be at most 255 bytes.
         *      This is not required anywhere else, but doing so may have
         *      performance benefits.
         */
        template<class ...T> inline Mesh& addVertexBuffer(Buffer& buffer, GLintptr offset, const T&... attributes) {
            addVertexBufferInternal(buffer, offset, strideOfInterleaved(attributes...), 0, attributes...);
            return *this;
        }

        /**
         * @brief Add instanced vertex buffer
         * @return Reference to self (for method chaining)
         *
         * Similar to the above function, the @p divisor parameter specifies
         * number of instances that will pass until new data are fetched from
         * the buffer. Setting it to `0` is equivalent to calling
         * @ref addVertexBuffer().
         *
         * If @extension{ARB,vertex_array_object} (part of OpenGL 3.0), OpenGL
         * ES 3.0, WebGL 2.0, @extension{OES,vertex_array_object} in OpenGL
         * ES 2.0 or @webgl_extension{OES,vertex_array_object} in WebGL 1.0 is
         * available, the vertex array object is used to hold the parameters.
         *
         * @see @ref setPrimitive(), @ref setCount(), @ref setInstanceCount(),
         *      @ref setBaseInstance(),
         *      @fn_gl{BindVertexArray}, @fn_gl{EnableVertexAttribArray},
         *      @fn_gl{BindBuffer}, @fn_gl{VertexAttribPointer},
         *      @fn_gl{VertexAttribDivisor} or
         *      @fn_gl_extension{EnableVertexArrayAttrib,EXT,direct_state_access},
         *      @fn_gl_extension{VertexArrayVertexAttribOffset,EXT,direct_state_access},
         *      @fn_gl_extension{VertexArrayVertexAttribDivisor,EXT,direct_state_access}
         * @requires_gl33 Extension @extension{ARB,instanced_arrays}
         * @requires_gles30 Extension @extension{ANGLE,instanced_arrays},
         *      @extension{EXT,instanced_arrays} or
         *      @extension{NV,instanced_arrays} in OpenGL ES 2.0.
         * @requires_webgl20 Extension @webgl_extension{ANGLE,instanced_arrays}
         *      in WebGL 1.0.
         */
        template<class ...T> inline Mesh& addVertexBufferInstanced(Buffer& buffer, UnsignedInt divisor, GLintptr offset, const T&... attributes) {
            addVertexBufferInternal(buffer, offset, strideOfInterleaved(attributes...), divisor, attributes...);
            return *this;
        }

        /**
         * @brief Add buffer with dynamic vertex attributes for use with given shader
         * @return Reference to self (for method chaining)
         *
         * Equivalent to @ref addVertexBuffer(Buffer&, GLintptr, const T&...)
         * but with the possibility to fully specify the attribute properties
         * at runtime, including base type and location. See
         * @ref Mesh-configuration-dynamic "class documentation" for usage
         * example.
         */
        Mesh& addVertexBuffer(Buffer& buffer, GLintptr offset, GLsizei stride, const DynamicAttribute& attribute) {
            return addVertexBufferInstanced(buffer, 0, offset, stride, attribute);
        }

        /**
         * @brief Add buffer with dynamic vertex attributes for use with given shader
         * @return Reference to self (for method chaining)
         *
         * Equivalent to @ref addVertexBufferInstanced(Buffer&, UnsignedInt, GLintptr, const T&...)
         * but with the possibility to fully specify the attribute properties
         * at runtime, including base type and location. See
         * @ref Mesh-configuration-dynamic "class documentation" for usage
         * example.
         */
        Mesh& addVertexBufferInstanced(Buffer& buffer, UnsignedInt divisor, GLintptr offset, GLsizei stride, const DynamicAttribute& attribute);

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
         * improving draw performance. Specifying `0` for both parameters
         * behaves the same as @ref setIndexBuffer(Buffer&, GLintptr, IndexType).
         * On OpenGL ES 2.0 this function behaves always as
         * @ref setIndexBuffer(Buffer&, GLintptr, IndexType), as this
         * functionality is not available there.
         *
         * If @extension{ARB,vertex_array_object} (part of OpenGL 3.0), OpenGL
         * ES 3.0, WebGL 2.0, @extension{OES,vertex_array_object} in OpenGL
         * ES 2.0 or @webgl_extension{OES,vertex_array_object} in WebGL 1.0 is
         * available, the vertex array object is used to hold the parameters.
         *
         * Ignored when calling @ref draw(AbstractShaderProgram&, TransformFeedback&, UnsignedInt).
         *
         * @see @ref maxElementIndex(), @ref maxElementsIndices(),
         *      @ref maxElementsVertices(), @ref setCount(), @ref isIndexed(),
         *      @fn_gl{BindVertexArray}, @fn_gl{BindBuffer}
         */
        Mesh& setIndexBuffer(Buffer& buffer, GLintptr offset, IndexType type, UnsignedInt start, UnsignedInt end);

        /**
         * @brief Set index buffer
         * @param buffer        Index buffer
         * @param offset        Offset into the buffer
         * @param type          Index data type
         * @return Reference to self (for method chaining)
         *
         * Alternative to @ref setIndexBuffer(Buffer&, GLintptr, IndexType, UnsignedInt, UnsignedInt)
         * with unspecified index limits, see its documentation for more
         * information. Prefer to set index limits for better performance.
         */
        Mesh& setIndexBuffer(Buffer& buffer, GLintptr offset, IndexType type) {
            return setIndexBuffer(buffer, offset, type, 0, 0);
        }

        /**
         * @brief Draw the mesh
         * @param shader    Shader to use for drawing
         *
         * Expects that the shader is compatible with this mesh and is fully
         * set up. If vertex/index count or instance count is `0`, no draw
         * commands are issued. See also
         * @ref AbstractShaderProgram-rendering-workflow "AbstractShaderProgram documentation"
         * for more information. If @extension{ARB,vertex_array_object} (part
         * of OpenGL 3.0), OpenGL ES 3.0, WebGL 2.0, @extension{OES,vertex_array_object}
         * in OpenGL ES 2.0 or @webgl_extension{OES,vertex_array_object} in
         * WebGL 1.0 is available, the associated vertex array object is bound
         * instead of setting up the mesh from scratch.
         * @see @ref setCount(), @ref setInstanceCount(),
         *      @ref draw(AbstractShaderProgram&, TransformFeedback&, UnsignedInt),
         *      @ref MeshView::draw(AbstractShaderProgram&),
         *      @ref MeshView::draw(AbstractShaderProgram&, std::initializer_list<std::reference_wrapper<MeshView>>),
         *      @fn_gl{UseProgram}, @fn_gl{EnableVertexAttribArray},
         *      @fn_gl{BindBuffer}, @fn_gl{VertexAttribPointer},
         *      @fn_gl{DisableVertexAttribArray} or @fn_gl{BindVertexArray},
         *      @fn_gl{DrawArrays}/@fn_gl{DrawArraysInstanced}/
         *      @fn_gl{DrawArraysInstancedBaseInstance} or @fn_gl{DrawElements}/
         *      @fn_gl{DrawRangeElements}/@fn_gl{DrawElementsBaseVertex}/
         *      @fn_gl{DrawRangeElementsBaseVertex}/@fn_gl{DrawElementsInstanced}/
         *      @fn_gl{DrawElementsInstancedBaseInstance}/
         *      @fn_gl{DrawElementsInstancedBaseVertex}/
         *      @fn_gl{DrawElementsInstancedBaseVertexBaseInstance}
         * @requires_gl32 Extension @extension{ARB,draw_elements_base_vertex}
         *      if the mesh is indexed and @ref baseVertex() is not `0`.
         * @requires_gl33 Extension @extension{ARB,instanced_arrays} if
         *      @ref instanceCount() is more than `1`.
         * @requires_gl42 Extension @extension{ARB,base_instance} if
         *      @ref baseInstance() is not `0`.
         * @requires_gles30 Extension @extension{ANGLE,instanced_arrays},
         *      @extension{EXT,instanced_arrays} or
         *      @extension{NV,instanced_arrays} in OpenGL ES 2.0 if
         *      @ref instanceCount() is more than `1`.
         * @requires_webgl20 Extension @webgl_extension{ANGLE,instanced_arrays}
         *      in WebGL 1.0 if @ref instanceCount() is more than `1`.
         * @requires_gl Specifying base vertex for indexed meshes is not
         *      available in OpenGL ES or WebGL.
         */
        void draw(AbstractShaderProgram& shader);
        void draw(AbstractShaderProgram&& shader) { draw(shader); } /**< @overload */

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Draw the mesh with vertices coming out of transform feedback
         * @param shader    Shader to use for drawing
         * @param xfb       Transform feedback to use for vertex count
         * @param stream    Transform feedback stream ID
         *
         * Expects that the @p shader is compatible with this mesh, is fully
         * set up and that the output buffer(s) from @p xfb are used as vertex
         * buffers in this mesh. Everything set by @ref setCount(),
         * @ref setBaseInstance(), @ref setBaseVertex() and @ref setIndexBuffer()
         * is ignored, the mesh is drawn as non-indexed and the vertex count is
         * taken from the @p xfb object. If @p stream is `0`, non-stream draw
         * command is used. If @extension{ARB,vertex_array_object} (part of
         * OpenGL 3.0) is available, the associated vertex array object is
         * bound instead of setting up the mesh from scratch.
         * @see @ref setInstanceCount(), @ref draw(AbstractShaderProgram&),
         *      @ref MeshView::draw(AbstractShaderProgram&, TransformFeedback&, UnsignedInt),
         *      @fn_gl{UseProgram}, @fn_gl{EnableVertexAttribArray},
         *      @fn_gl{BindBuffer}, @fn_gl{VertexAttribPointer},
         *      @fn_gl{DisableVertexAttribArray} or @fn_gl{BindVertexArray},
         *      @fn_gl{DrawTransformFeedback}/@fn_gl{DrawTransformFeedbackInstanced} or
         *      @fn_gl{DrawTransformFeedbackStream}/@fn_gl{DrawTransformFeedbackStreamInstanced}
         * @requires_gl40 Extension @extension{ARB,transform_feedback2}
         * @requires_gl40 Extension @extension{ARB,transform_feedback3} if
         *      @p stream is not `0`
         * @requires_gl42 Extension @extension{ARB,transform_feedback_instanced}
         *      if @ref instanceCount() is more than `1`.
         */
        void draw(AbstractShaderProgram& shader, TransformFeedback& xfb, UnsignedInt stream = 0);

        /** @overload */
        void draw(AbstractShaderProgram&& shader, TransformFeedback& xfb, UnsignedInt stream = 0) {
            draw(shader, xfb, stream);
        }
        #endif

    private:
        struct MAGNUM_LOCAL AttributeLayout;

        explicit Mesh(GLuint id, MeshPrimitive primitive, ObjectFlags flags);

        void MAGNUM_LOCAL createIfNotAlready();

        #ifndef MAGNUM_TARGET_WEBGL
        Mesh& setLabelInternal(Containers::ArrayView<const char> label);
        #endif

        /* Computing stride of interleaved vertex attributes */
        template<UnsignedInt location, class T, class ...U> static GLsizei strideOfInterleaved(const Attribute<location, T>& attribute, const U&... attributes) {
            return attribute.vectorSize()*Attribute<location, T>::VectorCount + strideOfInterleaved(attributes...);
        }
        template<class ...T> static GLsizei strideOfInterleaved(GLintptr gap, const T&... attributes) {
            return gap + strideOfInterleaved(attributes...);
        }
        static GLsizei strideOfInterleaved() { return 0; }

        /* Adding interleaved vertex attributes */
        template<UnsignedInt location, class T, class ...U> void addVertexBufferInternal(Buffer& buffer, GLintptr offset, GLsizei stride, GLuint divisor, const Attribute<location, T>& attribute, const U&... attributes) {
            addVertexAttribute(buffer, attribute, offset, stride, divisor);

            /* Add size of this attribute to offset for next attribute */
            addVertexBufferInternal(buffer, offset+attribute.vectorSize()*Attribute<location, T>::VectorCount, stride, divisor, attributes...);
        }
        template<class ...T> void addVertexBufferInternal(Buffer& buffer, GLintptr offset, GLsizei stride, GLuint divisor, GLintptr gap, const T&... attributes) {
            /* Add the gap to offset for next attribute */
            addVertexBufferInternal(buffer, offset+gap, stride, divisor, attributes...);
        }
        void addVertexBufferInternal(Buffer&, GLsizei, GLuint, GLintptr) {}

        template<UnsignedInt location, class T> void addVertexAttribute(typename std::enable_if<std::is_same<typename Implementation::Attribute<T>::ScalarType, Float>::value, Buffer&>::type buffer, const Attribute<location, T>& attribute, GLintptr offset, GLsizei stride, GLuint divisor) {
            for(UnsignedInt i = 0; i != Attribute<location, T>::VectorCount; ++i)
                attributePointerInternal(buffer,
                    location+i,
                    GLint(attribute.components()),
                    GLenum(attribute.dataType()),
                    attribute.dataOptions() & Attribute<location, T>::DataOption::Normalized ? DynamicAttribute::Kind::GenericNormalized : DynamicAttribute::Kind::Generic,
                    GLintptr(offset+i*attribute.vectorSize()),
                    stride,
                    divisor);
        }

        #ifndef MAGNUM_TARGET_GLES2
        template<UnsignedInt location, class T> void addVertexAttribute(typename std::enable_if<std::is_integral<typename Implementation::Attribute<T>::ScalarType>::value, Buffer&>::type buffer, const Attribute<location, T>& attribute, GLintptr offset, GLsizei stride, GLuint divisor) {
            attributePointerInternal(buffer,
                location,
                GLint(attribute.components()),
                GLenum(attribute.dataType()),
                DynamicAttribute::Kind::Integral,
                offset,
                stride,
                divisor);
        }

        #ifndef MAGNUM_TARGET_GLES
        template<UnsignedInt location, class T> void addVertexAttribute(typename std::enable_if<std::is_same<typename Implementation::Attribute<T>::ScalarType, Double>::value, Buffer&>::type buffer, const Attribute<location, T>& attribute, GLintptr offset, GLsizei stride, GLuint divisor) {
            for(UnsignedInt i = 0; i != Attribute<location, T>::VectorCount; ++i)
                attributePointerInternal(buffer,
                    location+i,
                    GLint(attribute.components()),
                    GLenum(attribute.dataType()),
                    DynamicAttribute::Kind::Long,
                    GLintptr(offset+i*attribute.vectorSize()),
                    stride,
                    divisor);
        }
        #endif
        #endif

        void MAGNUM_LOCAL bindVAO();

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

        void MAGNUM_LOCAL createImplementationDefault();
        void MAGNUM_LOCAL createImplementationVAO();
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL createImplementationVAODSA();
        #endif

        void MAGNUM_LOCAL destroyImplementationDefault();
        void MAGNUM_LOCAL destroyImplementationVAO();

        void attributePointerInternal(const Buffer& buffer, GLuint location, GLint size, GLenum type, DynamicAttribute::Kind kind, GLintptr offset, GLsizei stride, GLuint divisor);
        void MAGNUM_LOCAL attributePointerInternal(AttributeLayout& attribute);
        void MAGNUM_LOCAL attributePointerImplementationDefault(AttributeLayout& attribute);
        void MAGNUM_LOCAL attributePointerImplementationVAO(AttributeLayout& attribute);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL attributePointerImplementationDSAEXT(AttributeLayout& attribute);
        #endif
        void MAGNUM_LOCAL vertexAttribPointer(AttributeLayout& attribute);

        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL vertexAttribDivisorImplementationVAO(GLuint index, GLuint divisor);
        void MAGNUM_LOCAL vertexAttribDivisorImplementationDSAEXT(GLuint index, GLuint divisor);
        #elif defined(MAGNUM_TARGET_GLES2)
        void MAGNUM_LOCAL vertexAttribDivisorImplementationANGLE(GLuint index, GLuint divisor);
        #ifndef MAGNUM_TARGET_WEBGL
        void MAGNUM_LOCAL vertexAttribDivisorImplementationEXT(GLuint index, GLuint divisor);
        void MAGNUM_LOCAL vertexAttribDivisorImplementationNV(GLuint index, GLuint divisor);
        #endif
        #endif

        void MAGNUM_LOCAL bindIndexBufferImplementationDefault(Buffer&);
        void MAGNUM_LOCAL bindIndexBufferImplementationVAO(Buffer& buffer);

        void MAGNUM_LOCAL bindImplementationDefault();
        void MAGNUM_LOCAL bindImplementationVAO();

        void MAGNUM_LOCAL unbindImplementationDefault();
        void MAGNUM_LOCAL unbindImplementationVAO();

        #ifdef MAGNUM_TARGET_GLES2
        void MAGNUM_LOCAL drawArraysInstancedImplementationANGLE(GLint baseVertex, GLsizei count, GLsizei instanceCount);
        #ifndef MAGNUM_TARGET_WEBGL
        void MAGNUM_LOCAL drawArraysInstancedImplementationEXT(GLint baseVertex, GLsizei count, GLsizei instanceCount);
        void MAGNUM_LOCAL drawArraysInstancedImplementationNV(GLint baseVertex, GLsizei count, GLsizei instanceCount);
        #endif

        void MAGNUM_LOCAL drawElementsInstancedImplementationANGLE(GLsizei count, GLintptr indexOffset, GLsizei instanceCount);
        #ifndef MAGNUM_TARGET_WEBGL
        void MAGNUM_LOCAL drawElementsInstancedImplementationEXT(GLsizei count, GLintptr indexOffset, GLsizei instanceCount);
        void MAGNUM_LOCAL drawElementsInstancedImplementationNV(GLsizei count, GLintptr indexOffset, GLsizei instanceCount);
        #endif
        #endif

        GLuint _id;
        MeshPrimitive _primitive;
        ObjectFlags _flags;
        Int _count, _baseVertex, _instanceCount;
        #ifndef MAGNUM_TARGET_GLES
        UnsignedInt _baseInstance;
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        UnsignedInt _indexStart, _indexEnd;
        #endif
        GLintptr _indexOffset;
        IndexType _indexType;
        Buffer* _indexBuffer;

        std::vector<AttributeLayout> _attributes;
};

/** @debugoperatorenum{Magnum::MeshPrimitive} */
MAGNUM_EXPORT Debug& operator<<(Debug& debug, MeshPrimitive value);

/** @debugoperatorclassenum{Magnum::Mesh,Magnum::Mesh::IndexType} */
MAGNUM_EXPORT Debug& operator<<(Debug& debug, Mesh::IndexType value);

inline GLuint Mesh::release() {
    const GLuint id = _id;
    _id = 0;
    return id;
}

}

namespace Corrade { namespace Utility {

/** @configurationvalue{Magnum::MeshPrimitive} */
template<> struct MAGNUM_EXPORT ConfigurationValue<Magnum::MeshPrimitive> {
    ConfigurationValue() = delete;

    /**
     * @brief Writes enum value as string
     *
     * If the value is invalid, returns empty string.
     */
    static std::string toString(Magnum::MeshPrimitive value, ConfigurationValueFlags);

    /**
     * @brief Reads enum value as string
     *
     * If the value is invalid, returns @ref Magnum::MeshPrimitive::Points "MeshPrimitive::Points".
     */
    static Magnum::MeshPrimitive fromString(const std::string& stringValue, ConfigurationValueFlags);
};

/** @configurationvalue{Magnum::Mesh::IndexType} */
template<> struct MAGNUM_EXPORT ConfigurationValue<Magnum::Mesh::IndexType> {
    ConfigurationValue() = delete;

    /**
     * @brief Write enum value as string
     *
     * If the value is invalid, returns empty string.
     */
    static std::string toString(Magnum::Mesh::IndexType value, ConfigurationValueFlags);

    /**
     * @brief Read enum value as string
     *
     * If the value is invalid, returns @ref Magnum::Mesh::IndexType::UnsignedInt "Mesh::IndexType::UnsignedInt".
     */
    static Magnum::Mesh::IndexType fromString(const std::string& stringValue, ConfigurationValueFlags);
};

}}

#endif
