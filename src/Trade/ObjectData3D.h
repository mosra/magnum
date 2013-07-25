#ifndef Magnum_Trade_ObjectData3D_h
#define Magnum_Trade_ObjectData3D_h
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
class MAGNUM_EXPORT ObjectData3D {
    public:
        /** @brief Instance type */
        enum class InstanceType: UnsignedByte {
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
        explicit ObjectData3D(std::vector<UnsignedInt> children, const Matrix4& transformation, InstanceType instanceType, UnsignedInt instanceId);

        /**
         * @brief Constructor for empty instance
         * @param children          Child objects
         * @param transformation    Transformation (relative to parent)
         */
        explicit ObjectData3D(std::vector<UnsignedInt> children, const Matrix4& transformation);

        /** @brief Copying is not allowed */
        ObjectData3D(const ObjectData3D&) = delete;

        /** @brief Move constructor */
        ObjectData3D(ObjectData3D&&);

        /** @brief Destructor */
        virtual ~ObjectData3D();

        /** @brief Copying is not allowed */
        ObjectData3D& operator=(const ObjectData3D&) = delete;

        /** @brief Move assignment */
        ObjectData3D& operator=(ObjectData3D&&);

        /** @brief Child objects */
        std::vector<UnsignedInt>& children() { return _children; }

        /** @brief Transformation (relative to parent) */
        Matrix4 transformation() const { return _transformation; }

        /**
         * @brief Instance type
         * @return Type of instance held by this object
         *
         * If the instance is of type InstanceType::Mesh, the instance can be
         * casted to MeshObjectData3D and provide more information.
         */
        InstanceType instanceType() const { return _instanceType; }

        /**
         * @brief Instance ID
         * @return ID of given camera / light / mesh etc., specified by
         *      instanceType()
         */
        Int instanceId() const { return _instanceId; }

    private:
        std::vector<UnsignedInt> _children;
        Matrix4 _transformation;
        InstanceType _instanceType;
        Int _instanceId;
};

/** @debugoperator{Magnum::Trade::ObjectData3D} */
Debug MAGNUM_EXPORT operator<<(Debug debug, ObjectData3D::InstanceType value);

}}

#endif
