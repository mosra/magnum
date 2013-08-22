#ifndef Magnum_MeshView_h
#define Magnum_MeshView_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class Magnum::MeshView
 */

#include "Magnum.h"
#include "OpenGL.h"
#include "magnumVisibility.h"

namespace Magnum {

/**
@brief %Mesh view

Allows different interpretation of given @ref Mesh data via different vertex or
index count and offset. It is then possible to reuse one mesh buffer
configuration for different views. %Mesh primitive, index type, attribute
bindings and attached buffers are reused from original mesh.

The same rules as in Mesh apply, i.e. if the view has non-zero index count, it
is treated as indexed mesh, otherwise it is treated as non-indexed mesh. If
both index and vertex count is zero, the view is treated as empty and no draw
commands are issued when calling draw().

You must ensure that the original mesh remains available for whole view
lifetime.
@todo Might cause issues when there are more data than just indices in index
    buffer (wrongly computed offset)
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
         * @brief Set vertex range
         * @param first     First vertex
         * @param count     Vertex count
         * @return Reference to self (for method chaining)
         *
         * Default is zero @p offset and zero @p count. If index range is
         * non-zero, vertex range is ignored, see main class documentation for
         * more information.
         */
        MeshView& setVertexRange(Int first, Int count) {
            _firstVertex = first;
            _vertexCount = count;
            return *this;
        }

        /**
         * @brief Set index range
         * @param first     First index
         * @param count     Index count
         * @param start     Minimum array index contained in the buffer
         * @param end       Maximum array index contained in the buffer
         * @return Reference to self (for method chaining)
         *
         * Specifying `0` for both @p start and @p end behaves the same as
         * @ref setIndexRange(Int, Int). On OpenGL ES 2.0 this function behaves
         * always as @ref  setIndexRange(Int, Int), as this functionality is
         * not available there.
         */
        MeshView& setIndexRange(Int first, Int count, UnsignedInt start, UnsignedInt end);

        /**
         * @brief Set index range
         * @param first     First index
         * @param count     Index count
         * @return Reference to self (for method chaining)
         *
         * Prefer to use @ref setIndexRange(Int, Int, UnsignedInt, UnsignedInt)
         * for better performance.
         */
        MeshView& setIndexRange(Int first, Int count) {
            return setIndexRange(first, count, 0, 0);
        }

        /**
         * @brief Draw the mesh
         *
         * See @ref Mesh::draw() for more information.
         */
        void draw();

    private:
        Mesh* _original;

        Int _firstVertex, _vertexCount, _indexCount;
        GLintptr _indexOffset;
        #ifndef MAGNUM_TARGET_GLES2
        UnsignedInt _indexStart, _indexEnd;
        #endif
};

inline MeshView::MeshView(Mesh& original): _original(&original), _firstVertex(0), _vertexCount(0), _indexCount(0), _indexOffset(0)
    #ifndef MAGNUM_TARGET_GLES2
    , _indexStart(0), _indexEnd(0)
    #endif
    {}

}

#endif
