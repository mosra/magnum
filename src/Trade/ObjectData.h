#ifndef Magnum_Trade_ObjectData_h
#define Magnum_Trade_ObjectData_h
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
 * @brief Class Magnum::Trade::ObjectData
 */

#include "Math/Matrix4.h"
#include "Magnum.h"

namespace Magnum { namespace Trade {

/**
@brief %Object data

Provides access to object transformation and hierarchy. See also
MeshObjectData, which is specialized for objects with mesh instance type.
*/
class ObjectData {
    ObjectData(const ObjectData& other) = delete;
    ObjectData(ObjectData&& other) = delete;
    ObjectData& operator=(const ObjectData& other) = delete;
    ObjectData& operator=(ObjectData&& other) = delete;

    public:
        /** @brief Instance type */
        enum class InstanceType {
            Camera,     /**< Camera instance (see CameraData) */
            Light,      /**< Light instance (see LightData) */
            Mesh,       /**< Mesh instance (see MeshData) */
            Empty       /**< Empty */
        };

        /**
         * @brief Constructor
         * @param name              Object name
         * @param children          Child objects
         * @param transformation    Transformation (relative to parent)
         * @param instanceType      Instance type
         * @param instanceId        Instance ID
         */
        inline ObjectData(const std::string& name, const std::vector<unsigned int>& children, const Matrix4& transformation, InstanceType instanceType, unsigned int instanceId): _name(name), _children(children), _transformation(transformation), _instanceType(instanceType), _instanceId(instanceId) {}

        /**
         * @brief Constructor for empty instance
         * @param name              Object name
         * @param children          Child objects
         * @param transformation    Transformation (relative to parent)
         */
        inline ObjectData(const std::string& name, const std::vector<unsigned int>& children, const Matrix4& transformation): _name(name), _children(children), _transformation(transformation), _instanceType(InstanceType::Empty), _instanceId(-1) {}

        /** @brief Destructor */
        inline virtual ~ObjectData() {}

        /** @brief %Object name */
        inline std::string name() const { return _name; }

        /** @brief Child objects */
        inline std::vector<unsigned int>& children() { return _children; }

        /** @brief Transformation (relative to parent) */
        inline Matrix4 transformation() const { return _transformation; }

        /**
         * @brief Instance type
         * @return Type of instance held by this object
         *
         * If the instance is of type InstanceType::Mesh, the instance can be
         * casted to MeshObjectData and provide more information.
         */
        inline InstanceType instanceType() const { return _instanceType; }

        /**
         * @brief Instance ID
         * @return ID of given camera / light / mesh etc., specified by
         *      instanceType()
         */
        inline int instanceId() const { return _instanceId; }

    private:
        std::string _name;
        std::vector<unsigned int> _children;
        Matrix4 _transformation;
        InstanceType _instanceType;
        int _instanceId;
};

}}

#endif
