#ifndef Magnum_Trade_MeshObjectData2D_h
#define Magnum_Trade_MeshObjectData2D_h
/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
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
