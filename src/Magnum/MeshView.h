#ifndef Magnum_MeshView_h
#define Magnum_MeshView_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014
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

#include "Magnum/Magnum.h"
#include "Magnum/OpenGL.h"
#include "Magnum/visibility.h"

namespace Magnum {

/**
@brief %Mesh view

Allows different interpretation of given @ref Mesh data via different vertex or
index count and offset. It is then possible to reuse one mesh buffer
configuration for different views. %Mesh primitive, index type, attribute
bindings and attached buffers are reused from original mesh.

The same rules as in @ref Mesh apply, i.e. if the view has non-zero index
count, it is treated as indexed mesh, otherwise it is treated as non-indexed
mesh. If both index and vertex count is zero, the view is treated as empty and
no draw commands are issued when calling @ref draw().

You must ensure that the original mesh remains available for whole view
lifetime.
*/
class MAGNUM_EXPORT MeshView {
    public:
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

        /**
         * @brief Set vertex/index count
         * @return Reference to self (for method chaining)
         *
         * Default is `0`.
         */
        MeshView& setCount(Int count) {
            _count = count;
            return *this;
        }

        /**
         * @brief Set base vertex
         * @return Reference to self (for method chaining)
         *
         * Sets number of vertices of which the vertex buffer will be offset
         * when drawing. Default is `0`.
         * @requires_gl32 %Extension @extension{ARB,draw_elements_base_vertex}
         *      for indexed meshes
         * @requires_gl Base vertex cannot be specified for indexed meshes in
         *      OpenGL ES.
         */
        MeshView& setBaseVertex(Int baseVertex) {
            _baseVertex = baseVertex;
            return *this;
        }

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Set vertex range
         * @param first     First vertex
         * @param count     Vertex count
         *
         * @deprecated Use @ref Magnum::MeshView::setCount() "setCount()" and
         *      @ref Magnum::MeshView::setBaseVertex() "setBaseVertex()"
         *      instead.
         */
        CORRADE_DEPRECATED("use setCount() and setBaseVertex() instead") MeshView& setVertexRange(Int first, Int count) {
            return setCount(count), setBaseVertex(first);
        }
        #endif

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
         * available there.
         * @see @ref setCount()
         */
        MeshView& setIndexRange(Int first, UnsignedInt start, UnsignedInt end);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Set index range
         * @param first     First index
         * @param count     Index count
         * @param start     Minimum array index contained in the buffer
         * @param end       Maximum array index contained in the buffer
         *
         * @deprecated Use @ref Magnum::MeshView::setCount() "setCount()" and
         *      @ref Magnum::MeshView::setIndexRange(Int, UnsignedInt, UnsignedInt) "setIndexRange(Int, UnsignedInt, UnsignedInt)"
         *      instead.
         */
        CORRADE_DEPRECATED("use setCount() and setIndexRange(Int, UnsignedInt, UnsignedInt) instead") MeshView& setIndexRange(Int first, Int count, UnsignedInt start, UnsignedInt end) {
            return setCount(count), setIndexRange(first, start, end);
        }
        #endif

        /**
         * @brief Set index range
         * @param first     First index
         * @return Reference to self (for method chaining)
         *
         * Prefer to use @ref setIndexRange(Int, UnsignedInt, UnsignedInt) for
         * better performance.
         * @see @ref setCount()
         */
        MeshView& setIndexRange(Int first);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Set index range
         * @param first     First index
         * @param count     Index count
         *
         * @deprecated Use @ref Magnum::MeshView::setCount() "setCount()" and
         *      @ref Magnum::MeshView::setIndexRange(Int) "setIndexRange(Int)"
         *      instead.
         */
        CORRADE_DEPRECATED("use setCount() and setIndexRange(Int) instead") MeshView& setIndexRange(Int first, Int count) {
            return setCount(count), setIndexRange(first);
        }
        #endif

        /**
         * @brief Draw the mesh
         *
         * See @ref Mesh::draw() for more information.
         */
        void draw(AbstractShaderProgram& shader);
        void draw(AbstractShaderProgram&& shader) { draw(shader); } /**< @overload */

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @copybrief draw(AbstractShaderProgram&)
         * @deprecated Use @ref Magnum::MeshView::draw(AbstractShaderProgram&) "draw(AbstractShaderProgram&)"
         *      instead.
         */
        CORRADE_DEPRECATED("use draw(AbstractShaderProgram&) instead") void draw();
        #endif

    private:
        Mesh* _original;

        Int _count, _baseVertex;
        GLintptr _indexOffset;
        #ifndef MAGNUM_TARGET_GLES2
        UnsignedInt _indexStart, _indexEnd;
        #endif
};

inline MeshView::MeshView(Mesh& original): _original(&original), _count(0), _baseVertex(0), _indexOffset(0)
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
