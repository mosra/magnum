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

#include "Magnum.h"

namespace Magnum { namespace Trade {

/**
@brief %Object data

Provides access to object transformation and hierarchy.
*/
class MAGNUM_EXPORT ObjectData {
    ObjectData(const ObjectData& other) = delete;
    ObjectData(ObjectData&& other) = delete;
    ObjectData& operator=(const ObjectData& other) = delete;
    ObjectData& operator=(ObjectData&& other) = delete;

    public:
        /** @brief Instance type */
        enum class InstanceType {
            Camera,     /**< Camera instance (see CameraData) */
            Light,      /**< Light instance (see LightData) */
            Mesh        /**< Mesh instance (see MeshData) */
        };

        /**
         * @brief Constructor
         * @param children          Child objects
         * @param transformation    Transformation (relative to parent)
         * @param instanceType      Instance type
         * @param instanceId        Instance ID
         */
        inline ObjectData(std::vector<size_t> children, const Matrix4& transformation, InstanceType instanceType, size_t instanceId): _children(children), _transformation(transformation), _instanceType(instanceType), _instanceId(instanceId) {}

        /** @brief Child objects */
        inline std::vector<size_t>& children() { return _children; }

        /** @brief Transformation (relative to parent) */
        inline Matrix4 transformation() const { return _transformation; }

        /**
         * @brief Instance type
         * @return Type of instance held by this object
         */
        inline InstanceType instanceType() const { return _instanceType; }

        /**
         * @brief Instance ID
         * @return ID of given camera / light / mesh etc., specified by
         *      instance()
         */
        inline size_t instanceId() const { return _instanceId; }

    private:
        std::vector<size_t> _children;
        Matrix4 _transformation;
        InstanceType _instanceType;
        size_t _instanceId;
};

}}

#endif
