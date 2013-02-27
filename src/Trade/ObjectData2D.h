#ifndef Magnum_Trade_ObjectData2D_h
#define Magnum_Trade_ObjectData2D_h
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
 * @brief Class Magnum::Trade::ObjectData2D
 */

#include <vector>

#include "Math/Matrix3.h"
#include "Magnum.h"

namespace Magnum { namespace Trade {

/**
@brief Two-dimensional object data

Provides access to object transformation and hierarchy. See also
MeshObjectData2D, which is specialized for objects with mesh instance type.
@see ObjectData3D
*/
class ObjectData2D {
    ObjectData2D(const ObjectData2D& other) = delete;
    ObjectData2D(ObjectData2D&& other) = delete;
    ObjectData2D& operator=(const ObjectData2D& other) = delete;
    ObjectData2D& operator=(ObjectData2D&& other) = delete;

    public:
        /** @brief Instance type */
        enum class InstanceType {
            Camera,     /**< Camera instance (see CameraData) */
            Mesh,       /**< Three-dimensional mesh instance (see MeshData2D) */
            Empty       /**< Empty */
        };

        /**
         * @brief Constructor
         * @param children          Child objects
         * @param transformation    Transformation (relative to parent)
         * @param instanceType      Instance type
         * @param instanceId        Instance ID
         */
        inline ObjectData2D(const std::vector<UnsignedInt>& children, const Matrix3& transformation, InstanceType instanceType, UnsignedInt instanceId): _children(children), _transformation(transformation), _instanceType(instanceType), _instanceId(instanceId) {}

        /**
         * @brief Constructor for empty instance
         * @param children          Child objects
         * @param transformation    Transformation (relative to parent)
         */
        inline ObjectData2D(const std::vector<UnsignedInt>& children, const Matrix3& transformation): _children(children), _transformation(transformation), _instanceType(InstanceType::Empty), _instanceId(-1) {}

        /** @brief Destructor */
        inline virtual ~ObjectData2D() {}

        /** @brief Child objects */
        inline std::vector<UnsignedInt>& children() { return _children; }

        /** @brief Transformation (relative to parent) */
        inline Matrix3 transformation() const { return _transformation; }

        /**
         * @brief Instance type
         * @return Type of instance held by this object
         *
         * If the instance is of type InstanceType::Mesh, the instance can be
         * casted to MeshObjectData2D and provide more information.
         */
        inline InstanceType instanceType() const { return _instanceType; }

        /**
         * @brief Instance ID
         * @return ID of given camera / light / mesh etc., specified by
         *      instanceType()
         */
        inline Int instanceId() const { return _instanceId; }

    private:
        std::vector<UnsignedInt> _children;
        Matrix3 _transformation;
        InstanceType _instanceType;
        Int _instanceId;
};

/** @debugoperator{Magnum::Trade::ObjectData2D} */
Debug MAGNUM_EXPORT operator<<(Debug debug, ObjectData2D::InstanceType value);

}}

#endif
