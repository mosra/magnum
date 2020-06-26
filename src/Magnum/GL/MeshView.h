#ifndef Magnum_GL_MeshView_h
#define Magnum_GL_MeshView_h
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
 * @brief Class @ref Magnum::GL::MeshView
 */

#include <initializer_list>
#include <Corrade/Containers/Reference.h>

#include "Magnum/Magnum.h"
#include "Magnum/GL/GL.h"
#include "Magnum/GL/OpenGL.h"
#include "Magnum/GL/visibility.h"

namespace Magnum { namespace GL {

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
no draw commands are issued when calling @ref AbstractShaderProgram::draw().

You must ensure that the original mesh remains available for whole view
lifetime.
*/
class MAGNUM_GL_EXPORT MeshView {
    public:
        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Draw multiple meshes at once
         * @m_deprecated_since{2020,06} Use
         *      @ref AbstractShaderProgram::draw(Containers::ArrayView<const Containers::Reference<MeshView>>)
         *      instead.
         */
        CORRADE_DEPRECATED("use AbstractShaderProgram::draw() instead") static void draw(AbstractShaderProgram& shader, Containers::ArrayView<const Containers::Reference<MeshView>> meshes);

        /**
         * @overload
         * @m_deprecated_since{2020,06} Use
         *      @ref AbstractShaderProgram::draw(Containers::ArrayView<const Containers::Reference<MeshView>>)
         *      instead.
         */
        CORRADE_DEPRECATED("use AbstractShaderProgram::draw() instead") static void draw(AbstractShaderProgram&& shader, Containers::ArrayView<const Containers::Reference<MeshView>> meshes);

        /**
         * @overload
         * @m_deprecated_since{2020,06} Use
         *      @ref AbstractShaderProgram::draw(std::initializer_list<Containers::Reference<MeshView>>)
         *      instead.
         */
        CORRADE_DEPRECATED("use AbstractShaderProgram::draw() instead") static void draw(AbstractShaderProgram& shader, std::initializer_list<Containers::Reference<MeshView>> meshes);

        /**
         * @overload
         * @m_deprecated_since{2020,06} Use
         *      @ref AbstractShaderProgram::draw(std::initializer_list<Containers::Reference<MeshView>>)
         *      instead.
         */
        CORRADE_DEPRECATED("use AbstractShaderProgram::draw() instead") static void draw(AbstractShaderProgram&& shader, std::initializer_list<Containers::Reference<MeshView>> meshes);
        #endif

        /**
         * @brief Constructor
         * @param original  Original, already configured mesh
         */
        explicit MeshView(Mesh& original);

        /** @brief Original mesh */
        Mesh& mesh() { return _original; }
        const Mesh& mesh() const { return _original; } /**< @overload */

        /** @brief Vertex/index count */
        Int count() const { return _count; }

        /**
         * @brief Set vertex/index count
         * @return Reference to self (for method chaining)
         *
         * Ignored when calling @ref AbstractShaderProgram::drawTransformFeedback().
         * @attention To prevent nothing being rendered by accident, this
         *      function has to be always called, even to just set the count to
         *      @cpp 0 @ce.
         */
        MeshView& setCount(Int count) {
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
         * @requires_gl32 Extension @gl_extension{ARB,draw_elements_base_vertex}
         *      for indexed meshes
         * @requires_gles32 Base vertex cannot be specified for indexed meshes
         *      in OpenGL ES 3.1 or WebGL.
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
         * @ref AbstractShaderProgram::drawTransformFeedback().
         *
         * Expects that the original mesh is indexed.
         * @see @ref setCount(), @ref mesh(), @ref Mesh::isIndexed()
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
         * @ref AbstractShaderProgram::drawTransformFeedback().
         *
         * Expects that the original mesh is indexed.
         * @see @ref setCount(), @ref mesh(), @ref Mesh::isIndexed()
         */
        MeshView& setIndexRange(Int first);

        /** @brief Instance count */
        Int instanceCount() const { return _instanceCount; }

        /**
         * @brief Set instance count
         * @return Reference to self (for method chaining)
         *
         * Default is @cpp 1 @ce.
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
         * Ignored when calling @ref AbstractShaderProgram::drawTransformFeedback().
         * Default is @cpp 0 @ce.
         * @requires_gl42 Extension @gl_extension{ARB,base_instance}
         * @requires_gl Base instance cannot be specified in OpenGL ES or
         *      WebGL.
         */
        MeshView& setBaseInstance(UnsignedInt baseInstance) {
            _baseInstance = baseInstance;
            return *this;
        }
        #endif

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Draw the mesh
         * @m_deprecated_since{2020,06} Use @ref AbstractShaderProgram::draw()
         *      instead.
         */
        CORRADE_DEPRECATED("use AbstractShaderProgram::draw() instead") MeshView& draw(AbstractShaderProgram& shader);

        /**
         * @overload
         * @m_deprecated_since{2020,06} Use @ref AbstractShaderProgram::draw()
         *      instead.
         */
        CORRADE_DEPRECATED("use AbstractShaderProgram::draw() instead") MeshView& draw(AbstractShaderProgram&& shader);

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Draw the mesh with vertices coming out of transform feedback
         * @m_deprecated_since{2020,06} Use
         *      @ref AbstractShaderProgram::drawTransformFeedback() instead.
         */
        CORRADE_DEPRECATED("use AbstractShaderProgram::drawTransformFeedback() instead") MeshView& draw(AbstractShaderProgram& shader, TransformFeedback& xfb, UnsignedInt stream = 0);

        /**
         * @overload
         * @m_deprecated_since{2020,06} Use
                @ref AbstractShaderProgram::drawTransformFeedback() instead.
         */
        CORRADE_DEPRECATED("use AbstractShaderProgram::drawTransformFeedback() instead") MeshView& draw(AbstractShaderProgram&& shader, TransformFeedback& xfb, UnsignedInt stream = 0);
        #endif
        #endif

    private:
        friend AbstractShaderProgram;
        friend Implementation::MeshState;

        #ifndef MAGNUM_TARGET_WEBGL
        static MAGNUM_GL_LOCAL void multiDrawImplementationDefault(Containers::ArrayView<const Containers::Reference<MeshView>> meshes);
        #endif
        static MAGNUM_GL_LOCAL void multiDrawImplementationFallback(Containers::ArrayView<const Containers::Reference<MeshView>> meshes);

        Containers::Reference<Mesh> _original;

        bool _countSet{};
        Int _count{}, _baseVertex{}, _instanceCount{1};
        #ifndef MAGNUM_TARGET_GLES
        UnsignedInt _baseInstance{};
        #endif
        GLintptr _indexOffset{};
        #ifndef MAGNUM_TARGET_GLES2
        UnsignedInt _indexStart{}, _indexEnd{};
        #endif
};

inline MeshView::MeshView(Mesh& original): _original{original} {}

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

}}

#endif
