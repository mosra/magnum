#ifndef Magnum_GL_Mesh_h
#define Magnum_GL_Mesh_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
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
 * @brief Class @ref Magnum::GL::Mesh, enum @ref Magnum::GL::MeshPrimitive, @ref Magnum::GL::MeshIndexType, function @ref Magnum::GL::meshPrimitive(), @ref Magnum::GL::meshIndexType(), @ref Magnum::GL::meshIndexTypeSize()
 */

#include <Corrade/Containers/Array.h>
#include <Corrade/Utility/ConfigurationValue.h>

#include "Magnum/Tags.h"
#include "Magnum/GL/AbstractObject.h"
#include "Magnum/GL/Attribute.h"
#include "Magnum/GL/Buffer.h"
#include "Magnum/GL/GL.h"

/* The __EMSCRIPTEN_major__ etc macros used to be passed implicitly, version
   3.1.4 moved them to a version header and version 3.1.23 dropped the
   backwards compatibility. To work consistently on all versions, including the
   header only if the version macros aren't present.
   https://github.com/emscripten-core/emscripten/commit/f99af02045357d3d8b12e63793cef36dfde4530a
   https://github.com/emscripten-core/emscripten/commit/f76ddc702e4956aeedb658c49790cc352f892e4c */
#if defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(__EMSCRIPTEN_major__)
#include <emscripten/version.h>
#endif

#ifdef MAGNUM_BUILD_DEPRECATED
/* For label() / setLabel(), which used to be a std::string. Not ideal for the
   return type, but at least something. */
#include <Corrade/Containers/StringStl.h>
#endif

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
@brief Mesh index type

@see @ref Magnum::MeshIndexType, @ref meshIndexType(),
    @ref meshIndexTypeSize(), @ref Mesh::setIndexBuffer()
@m_enum_values_as_keywords
*/
enum class MeshIndexType: GLenum {
    /**
     * @relativeref{Magnum,UnsignedByte}.
     *
     * Even though OpenGL historically supports 8-bit indices, using this type
     * is discouraged on contemporary GPU architectures. Prefer using 16-bit
     * indices instead.
     */
    UnsignedByte = GL_UNSIGNED_BYTE,

    /** @relativeref{Magnum,UnsignedShort} */
    UnsignedShort = GL_UNSIGNED_SHORT,

    /**
     * @relativeref{Magnum,UnsignedInt}
     * @requires_gles30 Extension @gl_extension{OES,element_index_uint}
     *       in OpenGL ES 2.0.
     * @requires_webgl20 Extension @webgl_extension{OES,element_index_uint}
     *      in WebGL 1.0.
     */
    UnsignedInt = GL_UNSIGNED_INT
};

/**
@brief Convert generic mesh index type to OpenGL mesh index type

In case @ref isMeshIndexTypeImplementationSpecific() returns @cpp false @ce for
@p type, maps it to a corresponding OpenGL type. In case
@ref isMeshIndexTypeImplementationSpecific() returns @cpp true @ce, assumes
@p type stores a Vulkan-specific format and returns @ref meshIndexTypeUnwrap()
cast to @ref MeshIndexType.
@see @ref meshPrimitive(), @ref meshIndexTypeSize()
*/
MAGNUM_GL_EXPORT MeshIndexType meshIndexType(Magnum::MeshIndexType type);

/**
@brief Size of given mesh index type
@m_since_latest

@see @ref Magnum::meshIndexTypeSize()
*/
MAGNUM_GL_EXPORT UnsignedInt meshIndexTypeSize(MeshIndexType type);

namespace Implementation {

struct MeshState;

template<class...> struct IsDynamicAttribute: std::false_type {};
template<class T> struct IsDynamicAttribute<T, DynamicAttribute>: std::true_type {};

}

/**
@brief Mesh

Wraps an OpenGL vertex array object, or a collection of buffers and attribute
bindings in case vertex array objects are not available or are disabled.

@section GL-Mesh-configuration-compile Quick usage with MeshTools::compile()

If you have a @ref Trade::MeshData instance that you got for example from
@ref Trade::AbstractImporter::mesh() or from the @ref Primitives library, the
simplest possible way is to use @ref MeshTools::compile():

@snippet Trade.cpp MeshData-gpu-opengl

This one-liner uploads the data and configures the mesh for all attributes
known by Magnum that are present in it, making it suitable to be drawn by
builtin shaders. It's however rather opaque and the @ref Trade::MeshData may be
an overly generic abstraction if you already have your vertex data in known
types. Continue below to see how to configure a mesh for builtin shaders with
lower-level APIs.

@m_class{m-note m-success}

@par
    A generic mesh setup using the high-level utility is used in the
    @ref examples-primitives and @ref examples-viewer examples.

@section GL-Mesh-configuration Mesh configuration

A mesh is, at the very least, a @ref MeshPrimitive and associated vertex/index
count. To prevent accidentally drawing empty meshes, you're required to call
@ref setCount() always, the primitive is however implicitly
@ref MeshPrimitive::Triangles and you can change it either in the constructor
or via @ref setPrimitive(). If @ref setCount() (or @ref setInstanceCount()) is
zero, the mesh is considered empty and no draw commands are issued when calling
@ref AbstractShaderProgram::draw().

While a mesh can be attribute-less and rely on a specialized vertex shader to
generate positions and other data, in most cases it has one or more associated
vertex buffers and corresponding attribute bindings added using
@ref addVertexBuffer(). In the following snippet, a single position attribute
is specified, making it suitable to be rendered with the @ref Shaders::FlatGL
shader. The @relativeref{Magnum,Vector3} type we use for the data matches the
type expected by @ref Shaders::FlatGL3D::Position, so the default constructor
is sufficient for it. The @ref GL-Mesh-configuration-formats section below
shows cases where the types don't match.

@snippet GL.cpp Mesh-vertices

Here's a mesh with a position and a normal interleaved together, as is needed
for @ref Shaders::PhongGL. See the docs of @ref addVertexBuffer() for
detailed description how particular attributes, offsets and paddings are
specified. Note that @ref Shaders::FlatGL::Position and
@ref Shaders::PhongGL::Position are both aliases to
@ref Shaders::GenericGL::Position, meaning you can render a mesh configured for
the Phong shader with the Flat shader as well:

@snippet GL.cpp Mesh-vertices-interleaved

Indexed meshes have the index buffer and corresponding index type set using
@ref setIndexBuffer().

@snippet GL.cpp Mesh-indices

<b></b>

@m_class{m-note m-warning}

@par
    Note that, by default, the mesh doesn't deal with buffer ownership. You
    have to ensure the index and vertex buffers stay in scope for as long as
    the mesh is used, otherwise you'll end up with broken rendering or driver
    crashes. See @ref GL-Mesh-buffer-ownership below for a way to transfer
    buffer ownership to the mesh.

<b></b>

@m_class{m-note m-success}

@par
    A basic non-indexed mesh setup using the low-level interface is shown in
    the @ref examples-triangle "Triangle example", an indexed mesh then in the
    following @ref examples-texturedquad "Textured Quad example".

@subsection GL-Mesh-configuration-tools Using MeshTools

Real-world use cases rarely have a statically defined @cpp struct @ce with the
desired vertex attribute layout. If you have loose attribute arrays, you can
use @ref MeshTools::interleave() to interleave them together. The usage
including the padding specification, is similar to @ref addVertexBuffer(). The
above vertex buffer setup but with separate position and normal arrays that get interleaved can be expressed like this:

@snippet GL.cpp Mesh-vertices-interleaved-tool

For indices it's often beneficial to store them in a 16-bit type if they don't
need the full 32-bit range. That's what @ref MeshTools::compressIndices() is
for:

@snippet GL.cpp Mesh-indices-tool

The ultimate generic tool is the already-shown @ref MeshTools::compile(),
together with all @ref MeshTools APIs that operate on @ref Trade::MeshData
instances. See the class documentation for additional ways of accessing and
processing the data contained there.

@subsection GL-Mesh-configuration-formats Advanced formats of vertex data

Even though a shader accepts, say, a 32-bit floating-point vector, the actual
mesh data don't need to match that and can be in a smaller type to save on
memory bandwidth. The GPU vertex fetching hardware will then unpack them as
necessary. The following snippet shows a setup similar to the above position +
normal mesh, except that the position is a @relativeref{Magnum,Vector3h} and
the normal is a packed normalized @relativeref{Magnum,Vector3s}, together with
padding for having vertex boundaries aligned to four bytes to make the GPU
happier:

@snippet GL.cpp Mesh-formats

@subsection GL-Mesh-configuration-dynamic Dynamically specified attributes

In some cases, for example when the shader code is fully generated at runtime,
it's not possible to know attribute locations and types at compile time. In
that case, there are overloads of @ref addVertexBuffer() and
@ref addVertexBufferInstanced() that take a @ref DynamicAttribute instead of
the @ref Attribute typedefs, however then you're responsible for explicitly
specifying also the stride. Adding a RGB attribute at location 3 normalized
from unsigned byte to float with one byte padding at the end (or, in other
words, stride of four bytes) could then look like this:

@snippet GL.cpp Mesh-dynamic

The @ref DynamicAttribute also allows @ref VertexFormat to be used for
specifying attribute types instead of the rather verbose
@ref GL::Attribute::Components, @relativeref{GL::Attribute,DataType} and
@relativeref{GL::Attribute,DataOptions} tuple that GL itself accepts. The above
packed position + normal attribute specification would then look like this:

@snippet GL.cpp Mesh-formats-vertexformat

@subsection GL-Mesh-buffer-ownership Transferring buffer ownership

If a vertex/index buffer is used only by a single mesh, it's possible to
transfer its ownership to the mesh itself to simplify resource management on
the application side. Simply use the @ref addVertexBuffer() /
@ref addVertexBufferInstanced() and @ref setIndexBuffer() overloads that take
a @ref Buffer as a rvalue. While this allows you to discard the buffer
instances and pass just the mesh around, it also means you lose a way to access
or update the buffers afterwards.

@snippet GL.cpp Mesh-buffer-ownership

If adding the same buffer multiple times or using it for both vertex and index
data, be sure to transfer the ownership last to avoid the other functions
getting only a moved-out instance. For example:

@snippet GL.cpp Mesh-buffer-ownership-multiple

@section GL-Mesh-rendering Rendering meshes

With a framebuffer bound and a compatible shader set up, it's only a matter of
calling @ref AbstractShaderProgram::draw():

@snippet GL.cpp Mesh-draw

@section GL-Mesh-webgl-restrictions WebGL restrictions

@ref MAGNUM_TARGET_WEBGL "WebGL" puts some restrictions on vertex buffer
layout, see @ref addVertexBuffer() documentation for details.

A WebGL restriction that allows @ref Buffer "Buffers" to be bound only to one
unique target transitively affects meshes as well, requiring
@ref Buffer::TargetHint::ElementArray to be used for index buffers. To simplify
dealing with this restriction, the @ref addVertexBuffer() and
@ref setIndexBuffer() functions check proper target hint when adding vertex and
index buffers under WebGL.

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

        /** @overload
         * @brief Construct with a generic primitive type
         *
         * Note that implementation-specific values are passed as-is with
         * @ref meshPrimitiveUnwrap(). It's the user responsibility to ensure
         * an implementation-specific value actually represents a valid OpenGL
         * primitive type.
         */
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
        Containers::String label();

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
        Mesh& setLabel(Containers::StringView label);
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

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Index type size
         *
         * Expects that the mesh is indexed.
         * @m_deprecated_since_latest Use @ref meshIndexTypeSize() on the value
         *      returned from @ref indexType() instead.
         */
        CORRADE_DEPRECATED("use meshIndexTypeSize() on indexType() instead") UnsignedInt indexTypeSize() const;
        #endif

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

        /** @overload
         * @brief Set a generic primitive type
         *
         * Note that implementation-specific values are passed as-is with
         * @ref meshPrimitiveUnwrap(). It's the user responsibility to ensure
         * an implementation-specific value actually represents a valid OpenGL
         * primitive type.
         */
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
         * @requires_gles32 Extension @gl_extension{OES,draw_elements_base_vertex}
         *      or @gl_extension{EXT,draw_elements_base_vertex} for indexed
         *      meshes on OpenGL ES 3.1 and older
         * @requires_webgl_extension WebGL 2.0 and extension
         *      @webgl_extension{WEBGL,draw_instanced_base_vertex_base_instance}
         *      for indexed meshes
         */
        Mesh& setBaseVertex(Int baseVertex) {
            _baseVertex = baseVertex;
            return *this;
        }

        /**
         * @brief Index offset
         * @m_since_latest
         */
        GLintptr indexOffset() const { return _indexOffset; }

        /**
         * @brief Set index offset
         * @param offset    First index
         * @param start     Minimum array index contained in the buffer
         * @param end       Maximum array index contained in the buffer
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * The offset gets multiplied by index type size and added to the base
         * offset that was specified in @ref Mesh::setIndexBuffer(). The
         * @p start and @p end parameters may help to improve memory access
         * performance, as only a portion of vertex buffer needs to be
         * acccessed. On OpenGL ES 2.0 this function behaves the same as
         * @ref setIndexOffset(GLintptr), as index range functionality is not
         * available there. Ignored when calling
         * @ref AbstractShaderProgram::drawTransformFeedback().
         *
         * Expects that the mesh is indexed.
         * @see @ref setCount(), @ref isIndexed()
         */
        /* MinGW/MSVC needs inline also here to avoid linkage conflicts */
        inline Mesh& setIndexOffset(GLintptr offset, UnsignedInt start, UnsignedInt end);

        /**
         * @brief Set index offset
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * The offset gets multiplied by index type size and added to the base
         * offset that was specified in @ref Mesh::setIndexBuffer(). Prefer to
         * use @ref setIndexOffset(GLintptr, UnsignedInt, UnsignedInt) for
         * potential better performance on certain drivers. Ignored when
         * calling @ref AbstractShaderProgram::drawTransformFeedback().
         *
         * Expects that the mesh is indexed.
         * @see @ref setCount(), @ref isIndexed()
         */
        Mesh& setIndexOffset(GLintptr offset);

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

        #ifndef MAGNUM_TARGET_GLES2
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
         * @requires_es_extension OpenGL ES 3.1 and extension
         *      @m_class{m-doc-external} [ANGLE_base_vertex_base_instance](https://chromium.googlesource.com/angle/angle/+/master/extensions/ANGLE_base_vertex_base_instance.txt)
         * @requires_webgl_extension WebGL 2.0 and extension
         *      @webgl_extension{WEBGL,draw_instanced_base_vertex_base_instance}
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
         * The @p offset is a byte offset from the beginning of the buffer, the
         * attribute list is combination of @ref Attribute "attribute definitions"
         * (specified in implementation of given shader) and offsets between
         * interleaved attributes.
         *
         * See @ref GL-Mesh-configuration "class documentation" for simple
         * usage example. For more involved example imagine that you have a
         * buffer with 76 bytes of some other data at the beginning (possibly
         * material configuration) and then the interleaved vertex array. Each
         * vertex consists of a weight, position, texture coordinate and
         * a normal. You want to draw it with @ref Shaders::PhongGL, but it
         * accepts only a position and a normal, so you have to skip the weight
         * and the texture coordinate in each vertex:
         *
         * @snippet GL.cpp Mesh-addVertexBuffer1
         *
         * You can also achieve the same effect by calling @ref addVertexBuffer()
         * more times with explicitly specified gaps before and after the
         * attributes. This can be used for e.g. runtime-dependent
         * configuration, as it isn't dependent on the variadic template:
         *
         * @snippet GL.cpp Mesh-addVertexBuffer2
         *
         * If specifying more than one attribute, the function assumes that
         * the array is interleaved. Adding non-interleaved vertex buffer can
         * be done by specifying one attribute at a time with specific offset.
         * Above example with the position and normal arrays one after another
         * (non-interleaved):
         *
         * @snippet GL.cpp Mesh-addVertexBuffer3
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
            , typename std::enable_if<!Implementation::IsDynamicAttribute<T...>::value, int>::type = 0
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
            , typename std::enable_if<!Implementation::IsDynamicAttribute<T...>::value, int>::type = 0
            #endif
        > Mesh& addVertexBufferInstanced(Buffer& buffer, UnsignedInt divisor, GLintptr offset, const T&... attributes) {
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
            , typename std::enable_if<!Implementation::IsDynamicAttribute<T...>::value, int>::type = 0
            #endif
        > Mesh& addVertexBuffer(Buffer&& buffer, GLintptr offset, const T&... attributes) {
            addVertexBuffer<T...>(buffer, offset, attributes...);
            acquireVertexBuffer(Utility::move(buffer));
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
            , typename std::enable_if<!Implementation::IsDynamicAttribute<T...>::value, int>::type = 0
            #endif
        > Mesh& addVertexBufferInstanced(Buffer&& buffer, UnsignedInt divisor, GLintptr offset, const T&... attributes) {
            addVertexBufferInstanced<T...>(buffer, divisor, offset, attributes...);
            acquireVertexBuffer(Utility::move(buffer));
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
            acquireVertexBuffer(Utility::move(buffer));
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
            acquireVertexBuffer(Utility::move(buffer));
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
         *      @ref setIndexOffset(), @fn_gl{BindVertexArray},
         *      @fn_gl{BindBuffer} or @fn_gl_keyword{VertexArrayElementBuffer}
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
         * information. Prefer to set index limits for better performance on
         * certain drivers.
         */
        Mesh& setIndexBuffer(Buffer& buffer, GLintptr offset, MeshIndexType type) {
            return setIndexBuffer(buffer, offset, type, 0, 0);
        }

        /** @overload
         * @brief Set index buffer with a generic index type
         *
         * Note that implementation-specific values are passed as-is with
         * @ref meshIndexTypeUnwrap(). It's the user responsibility to ensure
         * an implementation-specific value actually represents a valid OpenGL
         * index type.
         */
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

        /** @overload
         * @brief Set index buffer with a generic index type and ownership transfer
         *
         * Note that implementation-specific values are passed as-is with
         * @ref meshIndexTypeUnwrap(). It's the user responsibility to ensure
         * an implementation-specific value actually represents a valid OpenGL
         * index type.
         */
        Mesh& setIndexBuffer(Buffer&& buffer, GLintptr offset, Magnum::MeshIndexType type, UnsignedInt start, UnsignedInt end) {
            return setIndexBuffer(Utility::move(buffer), offset, meshIndexType(type), start, end);
        }

        /**
         * @brief Set index buffer with ownership transfer
         *
         * Unlike @ref setIndexBuffer(Buffer&, GLintptr, MeshIndexType) this
         * function takes ownership of @p buffer. See
         * @ref GL-Mesh-buffer-ownership for more information.
         */
        Mesh& setIndexBuffer(Buffer&& buffer, GLintptr offset, MeshIndexType type) {
            return setIndexBuffer(Utility::move(buffer), offset, type, 0, 0);
        }

        /** @overload
         * @brief Set index buffer with a generic index type and ownership transfer
         *
         * Note that implementation-specific values are passed as-is with
         * @ref meshIndexTypeUnwrap(). It's the user responsibility to ensure
         * an implementation-specific value actually represents a valid OpenGL
         * index type.
         */
        Mesh& setIndexBuffer(Buffer&& buffer, GLintptr offset, Magnum::MeshIndexType type) {
            return setIndexBuffer(Utility::move(buffer), offset, meshIndexType(type), 0, 0);
        }

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

        /* Used by wrap() */
        explicit Mesh(GLuint id, MeshPrimitive primitive, ObjectFlags flags);

        void MAGNUM_GL_LOCAL createIfNotAlready();

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

        template<UnsignedInt location, class T, typename std::enable_if<std::is_same<typename Implementation::Attribute<T>::ScalarType, Float>::value, int>::type = 0> void addVertexAttribute(Buffer& buffer, const Attribute<location, T>& attribute, GLintptr offset, GLsizei stride, GLuint divisor) {
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
        template<UnsignedInt location, class T, typename std::enable_if<std::is_integral<typename Implementation::Attribute<T>::ScalarType>::value, int>::type = 0> void addVertexAttribute(Buffer& buffer, const Attribute<location, T>& attribute, GLintptr offset, GLsizei stride, GLuint divisor) {
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
        template<UnsignedInt location, class T, typename std::enable_if<std::is_same<typename Implementation::Attribute<T>::ScalarType, Double>::value, int>::type = 0> void addVertexAttribute(Buffer& buffer, const Attribute<location, T>& attribute, GLintptr offset, GLsizei stride, GLuint divisor) {
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

        #ifndef MAGNUM_TARGET_GLES2
        MAGNUM_GL_LOCAL void drawInternal(Int count, Int baseVertex, Int instanceCount, UnsignedInt baseInstance, GLintptr indexOffset, Int indexStart, Int indexEnd);
        #else
        MAGNUM_GL_LOCAL void drawInternal(Int count, Int baseVertex, Int instanceCount, GLintptr indexOffset);
        #endif

        MAGNUM_GL_LOCAL void drawInternal(const Containers::ArrayView<const UnsignedInt>& counts, const Containers::ArrayView<const UnsignedInt>& vertexOffsets,
            #ifdef CORRADE_TARGET_32BIT
            const Containers::ArrayView<const UnsignedInt>& indexOffsets
            #else
            const Containers::ArrayView<const UnsignedLong>& indexOffsets
            #endif
        );
        #ifdef MAGNUM_TARGET_GLES
        MAGNUM_GL_LOCAL void drawInternal(const Containers::ArrayView<const UnsignedInt>& counts, const Containers::ArrayView<const UnsignedInt>& instanceCounts, const Containers::ArrayView<const UnsignedInt>& vertexOffsets,
            #ifdef CORRADE_TARGET_32BIT
            const Containers::ArrayView<const UnsignedInt>& indexOffsets
            #else
            const Containers::ArrayView<const UnsignedLong>& indexOffsets
            #endif
            #ifndef MAGNUM_TARGET_GLES2
            , const Containers::ArrayView<const UnsignedInt>& instanceOffsets
            #endif
        );
        #endif
        MAGNUM_GL_LOCAL void drawInternalStrided(const Containers::StridedArrayView1D<const UnsignedInt>& counts, const Containers::StridedArrayView1D<const UnsignedInt>& vertexOffsets, const Containers::StridedArrayView1D<const UnsignedInt>& indexOffsets);
        #ifndef CORRADE_TARGET_32BIT
        MAGNUM_GL_LOCAL void drawInternalStrided(const Containers::StridedArrayView1D<const UnsignedInt>& counts, const Containers::StridedArrayView1D<const UnsignedInt>& vertexOffsets, const Containers::StridedArrayView1D<const UnsignedLong>& indexOffsets);
        #endif
        #ifdef MAGNUM_TARGET_GLES
        MAGNUM_GL_LOCAL void drawInternalStrided(const Containers::StridedArrayView1D<const UnsignedInt>& counts, const Containers::StridedArrayView1D<const UnsignedInt>& instanceCounts, const Containers::StridedArrayView1D<const UnsignedInt>& vertexOffsets, const Containers::StridedArrayView1D<const UnsignedInt>& indexOffsets
            #ifndef MAGNUM_TARGET_GLES2
            , const Containers::StridedArrayView1D<const UnsignedInt>& instanceOffsets
            #endif
        );
        #ifndef CORRADE_TARGET_32BIT
        MAGNUM_GL_LOCAL void drawInternalStrided(const Containers::StridedArrayView1D<const UnsignedInt>& counts, const Containers::StridedArrayView1D<const UnsignedInt>& instanceCounts, const Containers::StridedArrayView1D<const UnsignedInt>& vertexOffsets, const Containers::StridedArrayView1D<const UnsignedLong>& indexOffsets
            #ifndef MAGNUM_TARGET_GLES2
            , const Containers::StridedArrayView1D<const UnsignedInt>& instanceOffsets
            #endif
        );
        #endif
        #endif

        #ifndef MAGNUM_TARGET_GLES
        MAGNUM_GL_LOCAL void drawInternal(TransformFeedback& xfb, UnsignedInt stream, Int instanceCount);
        #endif

        static void MAGNUM_GL_LOCAL createImplementationDefault(Mesh& self);
        static void MAGNUM_GL_LOCAL createImplementationVAO(Mesh& self);
        #ifndef MAGNUM_TARGET_GLES
        static void MAGNUM_GL_LOCAL createImplementationVAODSA(Mesh& self);
        #endif

        static void MAGNUM_GL_LOCAL destroyImplementationDefault(Mesh& self);
        static void MAGNUM_GL_LOCAL destroyImplementationVAO(Mesh& self);

        void attributePointerInternal(const Buffer& buffer, GLuint location, GLint size, GLenum type, DynamicAttribute::Kind kind, GLintptr offset, GLsizei stride, GLuint divisor);
        void MAGNUM_GL_LOCAL attributePointerInternal(AttributeLayout&& attribute);
        static void MAGNUM_GL_LOCAL attributePointerImplementationDefault(Mesh& self, AttributeLayout&& attribute);
        static void MAGNUM_GL_LOCAL attributePointerImplementationVAO(Mesh& self, AttributeLayout&& attribute);
        #ifndef MAGNUM_TARGET_GLES
        static void MAGNUM_GL_LOCAL attributePointerImplementationVAODSA(Mesh& self, AttributeLayout&& attribute);
        #ifdef CORRADE_TARGET_WINDOWS
        static void MAGNUM_GL_LOCAL attributePointerImplementationVAODSAIntelWindows(Mesh& self, AttributeLayout&& attribute);
        #endif
        #endif
        #ifdef MAGNUM_TARGET_GLES
        static void MAGNUM_GL_LOCAL attributePointerImplementationDefaultAngleAlwaysInstanced(Mesh& self, AttributeLayout&& attribute);
        static void MAGNUM_GL_LOCAL attributePointerImplementationVAOAngleAlwaysInstanced(Mesh& self, AttributeLayout&& attribute);
        #endif
        void MAGNUM_GL_LOCAL vertexAttribPointer(AttributeLayout& attribute);

        #ifndef MAGNUM_TARGET_GLES
        static void MAGNUM_GL_LOCAL vertexAttribDivisorImplementationVAO(Mesh& self, GLuint index, GLuint divisor);
        static void MAGNUM_GL_LOCAL vertexAttribDivisorImplementationVAODSA(Mesh& self, GLuint index, GLuint divisor);
        #elif defined(MAGNUM_TARGET_GLES2)
        static void MAGNUM_GL_LOCAL vertexAttribDivisorImplementationANGLE(Mesh& self, GLuint index, GLuint divisor);
        #ifndef MAGNUM_TARGET_WEBGL
        static void MAGNUM_GL_LOCAL vertexAttribDivisorImplementationEXT(Mesh& self, GLuint index, GLuint divisor);
        static void MAGNUM_GL_LOCAL vertexAttribDivisorImplementationNV(Mesh& self, GLuint index, GLuint divisor);
        #endif
        #endif

        void acquireVertexBuffer(Buffer&& buffer);
        static void MAGNUM_GL_LOCAL acquireVertexBufferImplementationDefault(Mesh& self, Buffer&& buffer);
        static void MAGNUM_GL_LOCAL acquireVertexBufferImplementationVAO(Mesh& self, Buffer&& buffer);

        static void MAGNUM_GL_LOCAL bindIndexBufferImplementationDefault(Mesh& self, Buffer&);
        static void MAGNUM_GL_LOCAL bindIndexBufferImplementationVAO(Mesh& self, Buffer& buffer);
        #ifndef MAGNUM_TARGET_GLES
        static void MAGNUM_GL_LOCAL bindIndexBufferImplementationVAODSA(Mesh& self, Buffer& buffer);
        #endif

        static void MAGNUM_GL_LOCAL bindImplementationDefault(Mesh& self);
        static void MAGNUM_GL_LOCAL bindImplementationVAO(Mesh& self);

        static void MAGNUM_GL_LOCAL unbindImplementationDefault(Mesh& self);
        static void MAGNUM_GL_LOCAL unbindImplementationVAO(Mesh& self);

        #ifdef MAGNUM_TARGET_GLES
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        #if !defined(MAGNUM_TARGET_GLES2) && (!defined(MAGNUM_TARGET_WEBGL) || __EMSCRIPTEN_major__*10000 + __EMSCRIPTEN_minor__*100 + __EMSCRIPTEN_tiny__ >= 13915)
        static void MAGNUM_GL_LOCAL drawElementsBaseVertexImplementationANGLE(GLenum mode, GLsizei count, GLenum type, const void* indices, GLint baseVertex);
        #endif
        static void MAGNUM_GL_LOCAL drawElementsBaseVertexImplementationAssert(GLenum, GLsizei, GLenum, const void*, GLint);
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        #if !defined(MAGNUM_TARGET_WEBGL) || __EMSCRIPTEN_major__*10000 + __EMSCRIPTEN_minor__*100 + __EMSCRIPTEN_tiny__ >= 13915
        static void MAGNUM_GL_LOCAL drawRangeElementsBaseVertexImplementationANGLE(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void* indices, GLint baseVertex);
        #endif
        static void MAGNUM_GL_LOCAL drawRangeElementsBaseVertexImplementationAssert(GLenum, GLuint, GLuint, GLsizei, GLenum, const void*, GLint);

        static void MAGNUM_GL_LOCAL drawArraysInstancedBaseInstanceImplementationAssert(GLenum, GLint, GLsizei, GLsizei, GLuint);

        #if !defined(MAGNUM_TARGET_WEBGL) || __EMSCRIPTEN_major__*10000 + __EMSCRIPTEN_minor__*100 + __EMSCRIPTEN_tiny__ >= 13915
        static void MAGNUM_GL_LOCAL drawElementsInstancedBaseInstanceImplementationANGLE(GLenum mode, GLsizei count, GLenum type, const void* indices, GLsizei instanceCount, GLuint baseInstance);
        #endif
        static void MAGNUM_GL_LOCAL drawElementsInstancedBaseInstanceImplementationAssert(GLenum, GLsizei, GLenum, const void*, GLsizei, GLuint);

        static void MAGNUM_GL_LOCAL drawElementsInstancedBaseVertexBaseInstanceImplementationAssert(GLenum, GLsizei, GLenum, const void*, GLsizei, GLint, GLuint);

        #if !defined(MAGNUM_TARGET_WEBGL) || __EMSCRIPTEN_major__*10000 + __EMSCRIPTEN_minor__*100 + __EMSCRIPTEN_tiny__ >= 13915
        static void MAGNUM_GL_LOCAL drawElementsInstancedBaseVertexImplementationANGLE(GLenum mode, GLsizei count, GLenum type, const void* indices, GLsizei instanceCount, GLint baseVertex);
        #endif
        static void MAGNUM_GL_LOCAL drawElementsInstancedBaseVertexImplementationAssert(GLenum, GLsizei, GLenum, const void*, GLsizei, GLint);
        #endif
        #endif

        #ifdef MAGNUM_TARGET_GLES
        #if !defined(MAGNUM_TARGET_GLES2) && (!defined(MAGNUM_TARGET_WEBGL) || __EMSCRIPTEN_major__*10000 + __EMSCRIPTEN_minor__*100 + __EMSCRIPTEN_tiny__ >= 20005)
        static MAGNUM_GL_LOCAL void multiDrawElementsBaseVertexImplementationANGLE(GLenum mode, const GLsizei* count, GLenum type, const void* const* indices, GLsizei drawCount, const GLint* baseVertex);
        #endif
        static MAGNUM_GL_LOCAL void     multiDrawElementsBaseVertexImplementationAssert(GLenum, const GLsizei*, GLenum, const void* const*, GLsizei, const GLint*);
        #ifndef MAGNUM_TARGET_GLES2
        static MAGNUM_GL_LOCAL void multiDrawArraysInstancedBaseInstanceImplementationAssert(GLenum, const GLint*, const GLsizei*, const GLsizei*, const GLuint*, GLsizei);
        static MAGNUM_GL_LOCAL void multiDrawElementsInstancedBaseVertexBaseInstanceImplementationAssert(GLenum, const GLint*, GLenum, const void* const*, const GLsizei*, const GLint*, const GLuint*, GLsizei);
        #endif
        #endif

        /* _id, _primitive, _flags set from constructors */
        GLuint _id;
        MeshPrimitive _primitive;
        ObjectFlags _flags;
        /* using a separate bool for _count instead of Optional to make use of
           the 3-byte gap after _flags */
        bool _countSet{};
        /* 1 byte free */
        #ifdef MAGNUM_TARGET_GLES
        /* See the "angle-instanced-attributes-always-draw-instanced" workaround */
        bool _instanced{};
        #endif
        Int _count{}, _baseVertex{}, _instanceCount{1};
        #ifndef MAGNUM_TARGET_GLES2
        UnsignedInt _baseInstance{};
        UnsignedInt _indexStart{}, _indexEnd{};
        #endif
        MeshIndexType _indexType{};
        /* 4 bytes free on ES2 */
        GLintptr _indexBufferOffset{}, _indexOffset{};
        Buffer _indexBuffer{NoCreate};

        /* Stores attribute layouts in case VAOs are not supported or disabled,
           abused for capturing buffer ownership if VAOs are supported. */
        Containers::Array<AttributeLayout> _attributes;
};

/** @debugoperatorenum{MeshPrimitive} */
MAGNUM_GL_EXPORT Debug& operator<<(Debug& debug, MeshPrimitive value);

/** @debugoperatorenum{MeshIndexType} */
MAGNUM_GL_EXPORT Debug& operator<<(Debug& debug, MeshIndexType value);

inline Mesh& Mesh::setIndexOffset(GLintptr first, UnsignedInt start, UnsignedInt end) {
    setIndexOffset(first);
    #ifndef MAGNUM_TARGET_GLES2
    _indexStart = start;
    _indexEnd = end;
    #else
    static_cast<void>(start);
    static_cast<void>(end);
    #endif
    return *this;
}

inline GLuint Mesh::release() {
    const GLuint id = _id;
    _id = 0;
    return id;
}

}}

#endif
