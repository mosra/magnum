#ifndef Magnum_MeshView_h
#define Magnum_MeshView_h
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
 * @brief Class @ref Magnum::MeshView
 */

#include <functional>
#include <initializer_list>

#include "Magnum/Magnum.h"
#include "Magnum/OpenGL.h"
#include "Magnum/visibility.h"

namespace Magnum {

namespace Implementation { struct MeshState; }

/**
@brief Mesh view

Allows different interpretation of given @ref Mesh data via different vertex or
index count and offset. It is then possible to reuse one mesh buffer
configuration for different views. Mesh primitive, index type, attribute
bindings and attached buffers are reused from original mesh.

The same rules as in @ref Mesh apply, i.e. if the view has non-zero index
count, it is treated as indexed mesh, otherwise it is treated as non-indexed
mesh. If both index and vertex count is zero, the view is treated as empty and
no draw commands are issued when calling @ref draw().

You must ensure that the original mesh remains available for whole view
lifetime.
*/
class MAGNUM_EXPORT MeshView {
    friend Implementation::MeshState;

    public:
        /**
         * @brief Draw multiple meshes at once
         *
         * In OpenGL ES, if @extension2{EXT,multi_draw_arrays,multi_draw_arrays}
         * is not present, the functionality is emulated using sequence of
         * @ref draw(AbstractShaderProgram&) calls.
         *
         * If @extension{ARB,vertex_array_object} (part of OpenGL 3.0), OpenGL
         * ES 3.0, WebGL 2.0, @extension{OES,vertex_array_object} in OpenGL
         * ES 2.0 or @webgl_extension{OES,vertex_array_object} in WebGL 1.0 is
         * available, the associated vertex array object is bound instead of
         * setting up the mesh from scratch.
         * @attention All meshes must be views of the same original mesh and
         *      must not be instanced.
         * @see @ref draw(AbstractShaderProgram&), @fn_gl{UseProgram},
         *      @fn_gl{EnableVertexAttribArray}, @fn_gl{BindBuffer},
         *      @fn_gl{VertexAttribPointer}, @fn_gl{DisableVertexAttribArray}
         *      or @fn_gl{BindVertexArray}, @fn_gl{MultiDrawArrays} or
         *      @fn_gl{MultiDrawElements}/@fn_gl{MultiDrawElementsBaseVertex}
         * @requires_gl32 Extension @extension{ARB,draw_elements_base_vertex}
         *      if the mesh is indexed and @ref baseVertex() is not `0`.
         * @requires_gl Specifying base vertex for indexed meshes is not
         *      available in OpenGL ES or WebGL.
         */
        static void draw(AbstractShaderProgram& shader, std::initializer_list<std::reference_wrapper<MeshView>> meshes);

        /** @overload */
        static void draw(AbstractShaderProgram&& shader, std::initializer_list<std::reference_wrapper<MeshView>> meshes) {
            draw(shader, meshes);
        }

        /**
         * @brief Constructor
         * @param original  Original, already configured mesh
         */
        explicit MeshView(Mesh& original);

        /** @brief Copy constructor */
        MeshView(const MeshView& other) = default;

        /** @brief Movement is not allowed */
        MeshView(MeshView&& other) = delete;

        /** @brief Copy assignment */
        MeshView& operator=(const MeshView&) = default;

        /** @brief Movement is not allowed */
        MeshView& operator=(MeshView&& other) = delete;

        /** @brief Vertex/index count */
        Int count() const { return _count; }

        /**
         * @brief Set vertex/index count
         * @return Reference to self (for method chaining)
         *
         * Ignored when calling @ref draw(AbstractShaderProgram&, TransformFeedback&, UnsignedInt).
         * Default is `0`.
         */
        MeshView& setCount(Int count) {
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
         * when drawing. Ignored when calling @ref draw(AbstractShaderProgram&, TransformFeedback&, UnsignedInt).
         * Default is `0`.
         * @requires_gl32 Extension @extension{ARB,draw_elements_base_vertex}
         *      for indexed meshes
         * @requires_gl Base vertex cannot be specified for indexed meshes in
         *      OpenGL ES or WebGL.
         */
        MeshView& setBaseVertex(Int baseVertex) {
            _baseVertex = baseVertex;
            return *this;
        }

        /**
         * @brief Set index range
         * @param first     First vertex
         * @param start     Minimum array index contained in the buffer
         * @param end       Maximum array index contained in the buffer
         * @return Reference to self (for method chaining)
         *
         * The @p start and @p end parameters may help to improve memory access
         * performance, as only a portion of vertex buffer needs to be
         * acccessed. On OpenGL ES 2.0 this function behaves the same as
         * @ref setIndexRange(Int), as index range functionality is not
         * available there. Ignored when calling
         * @ref draw(AbstractShaderProgram&, TransformFeedback&, UnsignedInt).
         * @see @ref setCount()
         */
        /* MinGW/MSVC needs inline also here to avoid linkage conflicts */
        inline MeshView& setIndexRange(Int first, UnsignedInt start, UnsignedInt end);

        /**
         * @brief Set index range
         * @param first     First index
         * @return Reference to self (for method chaining)
         *
         * Prefer to use @ref setIndexRange(Int, UnsignedInt, UnsignedInt) for
         * better performance. Ignored when calling
         * @ref draw(AbstractShaderProgram&, TransformFeedback&, UnsignedInt).
         * @see @ref setCount()
         */
        MeshView& setIndexRange(Int first);

        /** @brief Instance count */
        Int instanceCount() const { return _instanceCount; }

        /**
         * @brief Set instance count
         * @return Reference to self (for method chaining)
         *
         * Default is `1`.
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
        MeshView& setInstanceCount(Int count) {
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
         * @requires_gl42 Extension @extension{ARB,base_instance}
         * @requires_gl Base instance cannot be specified in OpenGL ES or
         *      WebGL.
         */
        MeshView& setBaseInstance(UnsignedInt baseInstance) {
            _baseInstance = baseInstance;
            return *this;
        }
        #endif

        /**
         * @brief Draw the mesh
         *
         * See @ref Mesh::draw(AbstractShaderProgram&) for more information.
         * @see @ref draw(AbstractShaderProgram&, std::initializer_list<std::reference_wrapper<MeshView>>),
         *      @ref draw(AbstractShaderProgram&, TransformFeedback&, UnsignedInt)
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
         *
         * Everything set by @ref setCount(), @ref setBaseInstance(),
         * @ref setBaseVertex(), @ref setIndexRange() and @ref Mesh::setIndexBuffer()
         * is ignored, the mesh is drawn as non-indexed and the vertex count is
         * taken from the @p xfb object. See
         * @ref Mesh::draw(AbstractShaderProgram&, TransformFeedback&, UnsignedInt)
         * for more information.
         * @see @ref draw(AbstractShaderProgram&)
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
        #ifndef MAGNUM_TARGET_WEBGL
        static MAGNUM_LOCAL void multiDrawImplementationDefault(std::initializer_list<std::reference_wrapper<MeshView>> meshes);
        #endif
        static MAGNUM_LOCAL void multiDrawImplementationFallback(std::initializer_list<std::reference_wrapper<MeshView>> meshes);

        std::reference_wrapper<Mesh> _original;

        Int _count, _baseVertex, _instanceCount;
        #ifndef MAGNUM_TARGET_GLES
        UnsignedInt _baseInstance;
        #endif
        GLintptr _indexOffset;
        #ifndef MAGNUM_TARGET_GLES2
        UnsignedInt _indexStart, _indexEnd;
        #endif
};

inline MeshView::MeshView(Mesh& original): _original(original), _count(0), _baseVertex(0), _instanceCount{1},
    #ifndef MAGNUM_TARGET_GLES
    _baseInstance{0},
    #endif
    _indexOffset(0)
    #ifndef MAGNUM_TARGET_GLES2
    , _indexStart(0), _indexEnd(0)
    #endif
    {}

inline MeshView& MeshView::setIndexRange(Int first, UnsignedInt start, UnsignedInt end) {
    setIndexRange(first);
    #ifndef MAGNUM_TARGET_GLES2
    _indexStart = start;
    _indexEnd = end;
    #else
    static_cast<void>(start);
    static_cast<void>(end);
    #endif
    return *this;
}

}

#endif
