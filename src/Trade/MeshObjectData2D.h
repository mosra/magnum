#ifndef Magnum_Trade_MeshObjectData2D_h
#define Magnum_Trade_MeshObjectData2D_h
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
 * @brief Class Magnum::Trade::MeshObjectData2D
 */

#include "ObjectData2D.h"

namespace Magnum { namespace Trade {

/**
@brief Two-dimensional mesh object data

Provides access to material information for given mesh instance.
@see MeshObjectData3D
*/
class MeshObjectData2D: public ObjectData2D {
    MeshObjectData2D(const MeshObjectData2D& other) = delete;
    MeshObjectData2D(MeshObjectData2D&& other) = delete;
    MeshObjectData2D& operator=(const MeshObjectData2D& other) = delete;
    MeshObjectData2D& operator=(MeshObjectData2D&& other) = delete;

    public:
        /**
         * @brief Constructor
         * @param children          Child objects
         * @param transformation    Transformation (relative to parent)
         * @param instance          Instance ID
         * @param material          Material ID
         *
         * Creates object with mesh instance type.
         */
        inline MeshObjectData2D(const std::vector<UnsignedInt>& children, const Matrix4& transformation, UnsignedInt instance, UnsignedInt material): ObjectData2D(children, transformation, InstanceType::Mesh, instance), _material(material) {}

        /** @brief Material ID */
        inline UnsignedInt material() const { return _material; }

    private:
        UnsignedInt _material;
};

}}

#endif
