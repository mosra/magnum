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

Provides access to object transformation and hierarchy.
@see MeshObjectData2D, ObjectData3D
*/
class MAGNUM_EXPORT ObjectData2D {
    public:
        /** @brief Type of instance held by this object */
        enum class InstanceType: UnsignedByte {
            Camera,     /**< Camera instance (see CameraData) */

            /**
             * Three-dimensional mesh instance. The data can be cast to
             * MeshObjectData2D to provide more information.
             */
            Mesh,

            Empty       /**< Empty */
        };

        /**
         * @brief Constructor
         * @param children          Child objects
         * @param transformation    Transformation (relative to parent)
         * @param instanceType      Instance type
         * @param instance          Instance ID
         */
        explicit ObjectData2D(std::vector<UnsignedInt> children, const Matrix3& transformation, InstanceType instanceType, UnsignedInt instance);

        /**
         * @brief Constructor for empty instance
         * @param children          Child objects
         * @param transformation    Transformation (relative to parent)
         */
        explicit ObjectData2D(std::vector<UnsignedInt> children, const Matrix3& transformation);

        /** @brief Copying is not allowed */
        ObjectData2D(const ObjectData2D&) = delete;

        /** @brief Move constructor */
        ObjectData2D(ObjectData2D&&);

        /** @brief Destructor */
        virtual ~ObjectData2D();

        /** @brief Copying is not allowed */
        ObjectData2D& operator=(const ObjectData2D&) = delete;

        /** @brief Move assignment */
        ObjectData2D& operator=(ObjectData2D&&);

        /** @brief Child objects */
        std::vector<UnsignedInt>& children() { return _children; }

        /** @brief Transformation (relative to parent) */
        Matrix3 transformation() const { return _transformation; }

        /**
         * @brief Instance type
         *
         * @see instance()
         */
        InstanceType instanceType() const { return _instanceType; }

        /**
         * @brief Instance ID
         * @return ID of given camera / light / mesh etc., specified by
         *      instanceType()
         */
        Int instance() const { return _instance; }

    private:
        std::vector<UnsignedInt> _children;
        Matrix3 _transformation;
        InstanceType _instanceType;
        Int _instance;
};

/** @debugoperator{Magnum::Trade::ObjectData2D} */
Debug MAGNUM_EXPORT operator<<(Debug debug, ObjectData2D::InstanceType value);

}}

#endif
