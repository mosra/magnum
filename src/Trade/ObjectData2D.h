#ifndef Magnum_Trade_ObjectData2D_h
#define Magnum_Trade_ObjectData2D_h
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
