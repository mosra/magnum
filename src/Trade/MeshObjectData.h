#ifndef Magnum_Trade_MeshObjectData_h
#define Magnum_Trade_MeshObjectData_h
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
 * @brief Class Magnum::Trade::MeshObjectData
 */

#include "ObjectData.h"

namespace Magnum { namespace Trade {

/**
@brief %Mesh object data

Provides access to material information for given mesh instance.
*/
class MeshObjectData: public ObjectData {
    MeshObjectData(const MeshObjectData& other) = delete;
    MeshObjectData(MeshObjectData&& other) = delete;
    MeshObjectData& operator=(const MeshObjectData& other) = delete;
    MeshObjectData& operator=(MeshObjectData&& other) = delete;

    public:
        /**
         * @brief Constructor
         * @param name              %Mesh object name
         * @param children          Child objects
         * @param transformation    Transformation (relative to parent)
         * @param instance          Instance ID
         * @param material          Material ID
         *
         * Creates object with mesh instance type.
         */
        inline MeshObjectData(const std::string& name, const std::vector<unsigned int>& children, const Matrix4& transformation, unsigned int instance, unsigned int material): ObjectData(name, children, transformation, InstanceType::Mesh, instance), _material(material) {}

        /** @brief Material ID */
        inline unsigned int material() const { return _material; }

    private:
        unsigned int _material;
};

}}

#endif
