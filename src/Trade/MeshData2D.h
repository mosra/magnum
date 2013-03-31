#ifndef Magnum_Trade_MeshData2D_h
#define Magnum_Trade_MeshData2D_h
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
 * @brief Class Magnum::Trade::MeshData2D
 */

#include <string>

#include "Mesh.h"

namespace Magnum { namespace Trade {

/**
@brief Two-dimensional mesh data

Provides access to mesh data and additional information, such as primitive
type.
@see MeshData3D
*/
class MAGNUM_EXPORT MeshData2D {
    MeshData2D(const MeshData2D&) = delete;
    MeshData2D& operator=(const MeshData2D&) = delete;

    public:
        /**
         * @brief Constructor
         * @param primitive         Primitive
         * @param indices           Array with indices or 0, if this is not
         *      indexed mesh
         * @param positions         Array with vertex positions. At least one
         *      position array should be present.
         * @param textureCoords2D   Array with two-dimensional texture
         *      coordinate arrays or empty array
         */
        explicit MeshData2D(Mesh::Primitive primitive, std::vector<UnsignedInt>* indices, std::vector<std::vector<Vector2>*> positions, std::vector<std::vector<Vector2>*> textureCoords2D);

        /** @brief Move constructor */
        MeshData2D(MeshData2D&&);

        /** @brief Destructor */
        ~MeshData2D();

        /** @brief Move assignment */
        MeshData2D& operator=(MeshData2D&&);

        /** @brief Primitive */
        inline Mesh::Primitive primitive() const { return _primitive; }

        /**
         * @brief Indices
         * @return Indices or nullptr if the mesh is not indexed.
         */
        inline std::vector<UnsignedInt>* indices() { return _indices; }
        inline const std::vector<UnsignedInt>* indices() const { return _indices; } /**< @overload */

        /** @brief Count of vertex position arrays */
        inline UnsignedInt positionArrayCount() const { return _positions.size(); }

        /**
         * @brief Positions
         * @param id    ID of position data array
         * @return Positions or nullptr if there is no vertex array with given
         *      ID.
         */
        inline std::vector<Vector2>* positions(UnsignedInt id) { return _positions[id]; }
        inline const std::vector<Vector2>* positions(UnsignedInt id) const { return _positions[id]; } /**< @overload */

        /** @brief Count of 2D texture coordinate arrays */
        inline UnsignedInt textureCoords2DArrayCount() const { return _textureCoords2D.size(); }

        /**
         * @brief 2D texture coordinates
         * @param id    ID of texture coordinates array
         * @return %Texture coordinates or nullptr if there is no texture
         *      coordinates array with given ID.
         */
        inline std::vector<Vector2>* textureCoords2D(UnsignedInt id) { return _textureCoords2D[id]; }
        inline const std::vector<Vector2>* textureCoords2D(UnsignedInt id) const { return _textureCoords2D[id]; } /**< @overload */

    private:
        Mesh::Primitive _primitive;
        std::vector<UnsignedInt>* _indices;
        std::vector<std::vector<Vector2>*> _positions;
        std::vector<std::vector<Vector2>*> _textureCoords2D;
};

}}

#endif
