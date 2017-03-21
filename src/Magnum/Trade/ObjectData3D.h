#ifndef Magnum_Trade_ObjectData3D_h
#define Magnum_Trade_ObjectData3D_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
              Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class @ref Magnum::Trade::ObjectData3D, enum @ref Magnum::Trade::ObjectInstanceType3D
 */

#include <vector>

#include "Magnum/Magnum.h"
#include "Magnum/Math/Matrix4.h"

namespace Magnum { namespace Trade {

/**
@brief Type of instance held by given 3D object

@see @ref ObjectData3D::instanceType()
*/
enum class ObjectInstanceType3D: UnsignedByte {
    Camera,     /**< Camera instance (see @ref CameraData) */
    Light,      /**< Light instance (see @ref LightData) */

    /**
     * Mesh instance. The data can be cast to @ref MeshObjectData3D to provide
     * more information.
     */
    Mesh,

    Empty       /**< Empty */
};

/**
@brief Three-dimensional object data

Provides access to object transformation and hierarchy.
@see @ref MeshObjectData3D, @ref ObjectData2D
*/
class MAGNUM_EXPORT ObjectData3D {
    public:
        /**
         * @brief Constructor
         * @param children          Child objects
         * @param transformation    Transformation (relative to parent)
         * @param instanceType      Instance type
         * @param instance          Instance ID
         * @param importerState     Importer-specific state
         */
        explicit ObjectData3D(std::vector<UnsignedInt> children, const Matrix4& transformation, ObjectInstanceType3D instanceType, UnsignedInt instance, const void* importerState = nullptr);

        /**
         * @brief Constructor for empty instance
         * @param children          Child objects
         * @param transformation    Transformation (relative to parent)
         * @param importerState     Importer-specific state
         */
        explicit ObjectData3D(std::vector<UnsignedInt> children, const Matrix4& transformation, const void* importerState = nullptr);

        /** @brief Copying is not allowed */
        ObjectData3D(const ObjectData3D&) = delete;

        /** @brief Move constructor */
        ObjectData3D(ObjectData3D&&)
            /* GCC 4.9.0 (the one from Android NDK) thinks this does not match
               the implicit signature so it can't be defaulted. Works on 4.7,
               5.0 and everywhere else, so I don't bother. */
            #if !defined(__GNUC__) || __GNUC__*100 + __GNUC_MINOR__ != 409
            noexcept
            #endif
            ;

        /** @brief Destructor */
        virtual ~ObjectData3D();

        /** @brief Copying is not allowed */
        ObjectData3D& operator=(const ObjectData3D&) = delete;

        /** @brief Move assignment */
        ObjectData3D& operator=(ObjectData3D&&)
            /* GCC 4.9.0 (the one from Android NDK) thinks this does not match
               the implicit signature so it can't be defaulted. Works on 4.7,
               5.0 and everywhere else, so I don't bother. */
            #if !defined(__GNUC__) || __GNUC__*100 + __GNUC_MINOR__ != 409
            noexcept
            #endif
            ;

        /** @brief Child objects */
        std::vector<UnsignedInt>& children() { return _children; }
        const std::vector<UnsignedInt>& children() const { return _children; } /**< @overload */

        /** @brief Transformation (relative to parent) */
        Matrix4 transformation() const { return _transformation; }

        /**
         * @brief Instance type
         *
         * @see @ref instance()
         */
        ObjectInstanceType3D instanceType() const { return _instanceType; }

        /**
         * @brief Instance ID
         * @return ID of given camera / light / mesh etc., specified by
         *      @ref instanceType()
         */
        Int instance() const { return _instance; }

        /**
         * @brief Importer-specific state
         *
         * See @ref AbstractImporter::importerState() for more information.
         */
        const void* importerState() const { return _importerState; }

    private:
        std::vector<UnsignedInt> _children;
        Matrix4 _transformation;
        ObjectInstanceType3D _instanceType;
        Int _instance;
        const void* _importerState;
};

/** @debugoperatorenum{Magnum::Trade::ObjectInstanceType3D} */
MAGNUM_EXPORT Debug& operator<<(Debug& debug, ObjectInstanceType3D value);

}}

#endif
