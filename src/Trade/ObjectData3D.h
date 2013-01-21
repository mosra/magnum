#ifndef Magnum_Trade_ObjectData3D_h
#define Magnum_Trade_ObjectData3D_h
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
 * @brief Class Magnum::Trade::ObjectData3D
 */

#include <vector>

#include "Math/Matrix4.h"
#include "Magnum.h"

namespace Magnum { namespace Trade {

/**
@brief Three-dimensional object data

Provides access to object transformation and hierarchy. See also
MeshObjectData3D, which is specialized for objects with mesh instance type.
@see ObjectData2D
*/
class ObjectData3D {
    ObjectData3D(const ObjectData3D& other) = delete;
    ObjectData3D(ObjectData3D&& other) = delete;
    ObjectData3D& operator=(const ObjectData3D& other) = delete;
    ObjectData3D& operator=(ObjectData3D&& other) = delete;

    public:
        /** @brief Instance type */
        enum class InstanceType {
            Camera,     /**< Camera instance (see CameraData) */
            Light,      /**< Light instance (see LightData) */
            Mesh,       /**< Three-dimensional mesh instance (see MeshData3D) */
            Empty       /**< Empty */
        };

        /**
         * @brief Constructor
         * @param children          Child objects
         * @param transformation    Transformation (relative to parent)
         * @param instanceType      Instance type
         * @param instanceId        Instance ID
         */
        inline ObjectData3D(const std::vector<std::uint32_t>& children, const Matrix4& transformation, InstanceType instanceType, std::uint32_t instanceId): _children(children), _transformation(transformation), _instanceType(instanceType), _instanceId(instanceId) {}

        /**
         * @brief Constructor for empty instance
         * @param children          Child objects
         * @param transformation    Transformation (relative to parent)
         */
        inline ObjectData3D(const std::vector<std::uint32_t>& children, const Matrix4& transformation): _children(children), _transformation(transformation), _instanceType(InstanceType::Empty), _instanceId(-1) {}

        /** @brief Destructor */
        inline virtual ~ObjectData3D() {}

        /** @brief Child objects */
        inline std::vector<std::uint32_t>& children() { return _children; }

        /** @brief Transformation (relative to parent) */
        inline Matrix4 transformation() const { return _transformation; }

        /**
         * @brief Instance type
         * @return Type of instance held by this object
         *
         * If the instance is of type InstanceType::Mesh, the instance can be
         * casted to MeshObjectData3D and provide more information.
         */
        inline InstanceType instanceType() const { return _instanceType; }

        /**
         * @brief Instance ID
         * @return ID of given camera / light / mesh etc., specified by
         *      instanceType()
         */
        inline std::int32_t instanceId() const { return _instanceId; }

    private:
        std::vector<std::uint32_t> _children;
        Matrix4 _transformation;
        InstanceType _instanceType;
        std::int32_t _instanceId;
};

/** @debugoperator{Magnum::Trade::ObjectData3D} */
Debug MAGNUM_EXPORT operator<<(Debug debug, ObjectData3D::InstanceType value);

}}

#endif
