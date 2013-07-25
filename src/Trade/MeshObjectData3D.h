#ifndef Magnum_Trade_MeshObjectData3D_h
#define Magnum_Trade_MeshObjectData3D_h
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
 * @brief Class Magnum::Trade::MeshObjectData3D
 */

#include "ObjectData3D.h"

namespace Magnum { namespace Trade {

/**
@brief Three-dimensional mesh object data

Provides access to material information for given mesh instance.
@see MeshObjectData2D
*/
class MAGNUM_EXPORT MeshObjectData3D: public ObjectData3D {
    public:
        /**
         * @brief Constructor
         * @param children          Child objects
         * @param transformation    Transformation (relative to parent)
         * @param instance          Instance ID
         * @param material          Material ID
         * @param textureCoordinate2DBindings 2D texture coordinate bindings
         *
         * Creates object with mesh instance type.
         */
        explicit MeshObjectData3D(std::vector<UnsignedInt> children, const Matrix4& transformation, UnsignedInt instance, UnsignedInt material, std::vector<std::pair<UnsignedInt, UnsignedInt>> textureCoordinate2DBindings);

        /** @brief Copying is not allowed */
        MeshObjectData3D(const MeshObjectData3D&) = delete;

        /** @brief Move constructor */
        MeshObjectData3D(MeshObjectData3D&&);

        ~MeshObjectData3D();

        /** @brief Copying is not allowed */
        MeshObjectData3D& operator=(const MeshObjectData3D&) = delete;

        /** @brief Move assignment */
        MeshObjectData3D& operator=(MeshObjectData3D&&);

        /** @brief Material ID */
        UnsignedInt material() const { return _material; }

        /**
         * @brief 2D texture coordinate bindings
         *
         * First item of the pair is 2D texture coodinate ID, second is
         * material texture ID (e.g. @ref PhongMaterialData "PhongMaterialData::DiffuseTextureID").
         */
        std::vector<std::pair<UnsignedInt, UnsignedInt>>& textureCoordinate2DBindings() {
            return _textureCoordinate2DBindings;
        }

        /** @overload */
        const std::vector<std::pair<UnsignedInt, UnsignedInt>>& textureCoordinate2DBindings() const {
            return _textureCoordinate2DBindings;
        }

    private:
        UnsignedInt _material;
        std::vector<std::pair<UnsignedInt, UnsignedInt>> _textureCoordinate2DBindings;
};

}}

#endif
