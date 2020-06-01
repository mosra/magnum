#ifndef Magnum_Trade_ObjectData3D_h
#define Magnum_Trade_ObjectData3D_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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
#include "Magnum/Math/Quaternion.h"
#include "Magnum/Trade/visibility.h"

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
@brief 3D object flag

@see @ref ObjectFlags3D, @ref ObjectData3D::flags()
*/
enum class ObjectFlag3D: UnsignedByte {
    /**
     * The object provides separate translation / rotation / scaling
     * properties. The @ref ObjectData3D::transformation() matrix returns them
     * combined, but it's possible to access particular parts of the
     * transformation using @ref ObjectData3D::translation(),
     * @ref ObjectData3D::rotation() and @ref ObjectData3D::scaling().
     */
    HasTranslationRotationScaling = 1 << 0
};

/**
@brief 3D object flags

@see @ref ObjectData3D::flags()
*/
typedef Containers::EnumSet<ObjectFlag3D> ObjectFlags3D;

CORRADE_ENUMSET_OPERATORS(ObjectFlags3D)

/**
@brief Three-dimensional object data

Provides access to object transformation and hierarchy.
@see @ref AbstractImporter::object3D(), @ref MeshObjectData3D,
    @ref ObjectData2D
*/
class MAGNUM_TRADE_EXPORT ObjectData3D {
    public:
        /**
         * @brief Construct with combined transformation
         * @param children          Child objects
         * @param transformation    Transformation (relative to parent)
         * @param instanceType      Instance type
         * @param instance          Instance ID
         * @param importerState     Importer-specific state
         */
        explicit ObjectData3D(std::vector<UnsignedInt> children, const Matrix4& transformation, ObjectInstanceType3D instanceType, UnsignedInt instance, const void* importerState = nullptr);

        /**
         * @brief Construct with separate transformations
         * @param children          Child objects
         * @param translation       Translation (relative to parent)
         * @param rotation          Rotation (relative to parent)
         * @param scaling           Scaling (relative to parent)
         * @param instanceType      Instance type
         * @param instance          Instance ID
         * @param importerState     Importer-specific state
         */
        explicit ObjectData3D(std::vector<UnsignedInt> children, const Vector3& translation, const Quaternion& rotation, const Vector3& scaling, ObjectInstanceType3D instanceType, UnsignedInt instance, const void* importerState = nullptr);

        /**
         * @brief Construct empty instance with combined transformation
         * @param children          Child objects
         * @param transformation    Transformation (relative to parent)
         * @param importerState     Importer-specific state
         */
        explicit ObjectData3D(std::vector<UnsignedInt> children, const Matrix4& transformation, const void* importerState = nullptr);

        /**
         * @brief Construct empty instance with separate transformations
         * @param children          Child objects
         * @param translation       Translation (relative to parent)
         * @param rotation          Rotation (relative to parent)
         * @param scaling           Scaling (relative to parent)
         * @param importerState     Importer-specific state
         */
        explicit ObjectData3D(std::vector<UnsignedInt> children, const Vector3& translation, const Quaternion& rotation, const Vector3& scaling, const void* importerState = nullptr);

        /** @brief Copying is not allowed */
        ObjectData3D(const ObjectData3D&) = delete;

        /** @brief Move constructor */
        ObjectData3D(ObjectData3D&&)
            /* GCC 4.9.0 (the one from Android NDK) thinks this does not match
               the implicit signature so it can't be defaulted. Works on 4.8,
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
               the implicit signature so it can't be defaulted. Works on 4.8,
               5.0 and everywhere else, so I don't bother. */
            #if !defined(__GNUC__) || __GNUC__*100 + __GNUC_MINOR__ != 409
            noexcept
            #endif
            ;

        /** @brief Child objects */
        std::vector<UnsignedInt>& children() { return _children; }
        const std::vector<UnsignedInt>& children() const { return _children; } /**< @overload */

        /** @brief Flags */
        ObjectFlags3D flags() const { return _flags; }

        /**
         * @brief Translation (relative to parent)
         *
         * Available only if @ref ObjectFlag3D::HasTranslationRotationScaling
         * is set, use @ref transformation() otherwise. Applied as last in the
         * final transformation, see @ref transformation() for more
         * information.
         * @see @ref flags(), @ref rotation(), @ref scaling()
         */
        Vector3 translation() const;

        /**
         * @brief Rotation (relative to parent)
         *
         * Available only if @ref ObjectFlag3D::HasTranslationRotationScaling
         * is set, use @ref transformation() otherwise. Applied second in the
         * final transformation, see @ref transformation() for more
         * information.
         * @see @ref flags(), @ref translation(), @ref scaling()
         */
        Quaternion rotation() const;

        /**
         * @brief Scaling (relative to parent)
         *
         * Available only if @ref ObjectFlag3D::HasTranslationRotationScaling
         * is set, use @ref transformation() otherwise. Applied as first in the
         * final transformation, see @ref transformation() for more
         * information.
         * @see @ref flags(), @ref translation(), @ref rotation()
         */
        Vector3 scaling() const;

        /**
         * @brief Transformation (relative to parent)
         *
         * If @ref ObjectFlag3D::HasTranslationRotationScaling is not set,
         * returns the imported object transformation matrix. Otherwise
         * calculates the final transformation matrix @f$ \boldsymbol{M} @f$
         * from translation, rotation and scaling matrices @f$ \boldsymbol{T} @f$,
         * @f$ \boldsymbol{R} @f$, @f$ \boldsymbol{S} @f$ created from
         * @ref translation(), @ref rotation() and @ref scaling() in the
         * following order: @f[
         *      \boldsymbol{M} = \boldsymbol{T} \boldsymbol{R} \boldsymbol{S}
         * @f]
         *
         * The corresponding code is as follows:
         *
         * @snippet MagnumTrade.cpp ObjectData3D-transformation
         *
         * @see @ref flags()
         */
        Matrix4 transformation() const;

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
        union Transformation {
            Transformation(const Matrix4& matrix): matrix{matrix} {}
            Transformation(const Vector3& translation, const Quaternion& rotation, const Vector3& scaling): trs{translation, rotation, scaling} {}
            ~Transformation() {}

            Matrix4 matrix;
            struct {
                Vector3 translation;
                Quaternion rotation;
                Vector3 scaling;
            } trs;
        } _transformation;
        ObjectInstanceType3D _instanceType;
        ObjectFlags3D _flags;
        Int _instance;
        const void* _importerState;
};

/** @debugoperatorenum{ObjectInstanceType3D} */
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, ObjectInstanceType3D value);

/** @debugoperatorenum{ObjectFlag3D} */
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, ObjectFlag3D value);

/** @debugoperatorenum{ObjectFlags3D} */
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, ObjectFlags3D value);

}}

#endif
