#ifndef Magnum_Trade_MeshData2D_h
#define Magnum_Trade_MeshData2D_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016
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
 * @brief Class @ref Magnum::Trade::MeshData2D
 */

#include <vector>

#include "Magnum/Magnum.h"
#include "Magnum/visibility.h"

namespace Magnum { namespace Trade {

/**
@brief Two-dimensional mesh data

Provides access to mesh data and additional information, such as primitive
type.
@see @ref MeshData3D
*/
class MAGNUM_EXPORT MeshData2D {
    public:
        /**
         * @brief Constructor
         * @param primitive         Primitive
         * @param indices           Index array or empty array, if the mesh is
         *      not indexed
         * @param positions         Position arrays. At least one position
         *      array should be present.
         * @param textureCoords2D   Two-dimensional texture coordinate arrays,
         *      if present
         * @param importerState     Importer-specific state
         */
        explicit MeshData2D(MeshPrimitive primitive, std::vector<UnsignedInt> indices, std::vector<std::vector<Vector2>> positions, std::vector<std::vector<Vector2>> textureCoords2D, const void* importerState = nullptr);

        /** @brief Copying is not allowed */
        MeshData2D(const MeshData2D&) = delete;

        /** @brief Move constructor */
        MeshData2D(MeshData2D&&);

        ~MeshData2D();

        /** @brief Copying is not allowed */
        MeshData2D& operator=(const MeshData2D&) = delete;

        /** @brief Move assignment */
        MeshData2D& operator=(MeshData2D&&);

        /** @brief Primitive */
        MeshPrimitive primitive() const { return _primitive; }

        /** @brief Whether the mesh is indexed */
        bool isIndexed() const { return !_indices.empty(); }

        /**
         * @brief Indices
         *
         * @see @ref isIndexed()
         */
        std::vector<UnsignedInt>& indices();
        const std::vector<UnsignedInt>& indices() const; /**< @overload */

        /**
         * @brief Count of position arrays
         *
         * There is always at least one.
         */
        UnsignedInt positionArrayCount() const { return _positions.size(); }

        /**
         * @brief Positions
         * @param id    Position array ID
         *
         * @see @ref positionArrayCount()
         */
        std::vector<Vector2>& positions(UnsignedInt id);
        const std::vector<Vector2>& positions(UnsignedInt id) const; /**< @overload */

        /** @brief Whether the data contain any 2D texture coordinates */
        bool hasTextureCoords2D() const { return !_textureCoords2D.empty(); }

        /** @brief Count of 2D texture coordinate arrays */
        UnsignedInt textureCoords2DArrayCount() const { return _textureCoords2D.size(); }

        /**
         * @brief 2D texture coordinates
         * @param id    Texture coordinate array ID
         *
         * @see @ref textureCoords2DArrayCount()
         */
        std::vector<Vector2>& textureCoords2D(UnsignedInt id);
        const std::vector<Vector2>& textureCoords2D(UnsignedInt id) const; /**< @overload */

        /**
         * @brief Importer-specific state
         *
         * See @ref AbstractImporter::importerState() for more information.
         */
        const void* importerState() const { return _importerState; }

    private:
        MeshPrimitive _primitive;
        std::vector<UnsignedInt> _indices;
        std::vector<std::vector<Vector2>> _positions;
        std::vector<std::vector<Vector2>> _textureCoords2D;
        const void* _importerState;
};

}}

#endif
