#ifndef Magnum_Trade_ObjectData2D_h
#define Magnum_Trade_ObjectData2D_h
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
 * @brief Class @ref Magnum::Trade::ObjectData2D, enum @ref Magnum::Trade::ObjectInstanceType2D
 */

#include <vector>

#include "Magnum/Magnum.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Complex.h"
#include "Magnum/Trade/visibility.h"

namespace Magnum { namespace Trade {

/**
@brief Type of instance held by given 2D object

@see @ref ObjectData2D::instanceType()
*/
enum class ObjectInstanceType2D: UnsignedByte {
    Camera,     /**< Camera instance (see @ref CameraData) */

    /**
     * Mesh instance. The data can be cast to @ref MeshObjectData2D to provide
     * more information.
     */
    Mesh,

    Empty       /**< Empty */
};

/**
@brief 2D object flag

@see @ref ObjectFlags2D, @ref ObjectData2D::flags()
*/
enum class ObjectFlag2D: UnsignedByte {
    /**
     * The object provides separate translation / rotation / scaling
     * properties. The @ref ObjectData2D::transformation() matrix returns them
     * combined, but it's possible to access particular parts of the
     * transformation using @ref ObjectData2D::translation(),
     * @ref ObjectData2D::rotation() and @ref ObjectData2D::scaling().
     */
    HasTranslationRotationScaling = 1 << 0
};

/**
@brief 2D object flags

@see @ref ObjectData2D::flags()
*/
typedef Containers::EnumSet<ObjectFlag2D> ObjectFlags2D;

CORRADE_ENUMSET_OPERATORS(ObjectFlags2D)

/**
@brief Two-dimensional object data

Provides access to object transformation and hierarchy.
@see @ref AbstractImporter::object2D(), @ref MeshObjectData2D,
    @ref ObjectData3D
*/
class MAGNUM_TRADE_EXPORT ObjectData2D {
    public:
        /**
         * @brief Construct with combined transformation
         * @param children          Child objects
         * @param transformation    Transformation (relative to parent)
         * @param instanceType      Instance type
         * @param instance          Instance ID
         * @param importerState     Importer-specific state
         */
        explicit ObjectData2D(std::vector<UnsignedInt> children, const Matrix3& transformation, ObjectInstanceType2D instanceType, UnsignedInt instance, const void* importerState = nullptr);

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
        explicit ObjectData2D(std::vector<UnsignedInt> children, const Vector2& translation, const Complex& rotation, const Vector2& scaling, ObjectInstanceType2D instanceType, UnsignedInt instance, const void* importerState = nullptr);

        /**
         * @brief Construct empty instance with combined transformation
         * @param children          Child objects
         * @param transformation    Transformation (relative to parent)
         * @param importerState     Importer-specific state
         */
        explicit ObjectData2D(std::vector<UnsignedInt> children, const Matrix3& transformation, const void* importerState = nullptr);

        /**
         * @brief Construct empty instance with separate transformations
         * @param children          Child objects
         * @param translation       Translation (relative to parent)
         * @param rotation          Rotation (relative to parent)
         * @param scaling           Scaling (relative to parent)
         * @param importerState     Importer-specific state
         */
        explicit ObjectData2D(std::vector<UnsignedInt> children, const Vector2& translation, const Complex& rotation, const Vector2& scaling, const void* importerState = nullptr);

        /** @brief Copying is not allowed */
        ObjectData2D(const ObjectData2D&) = delete;

        /** @brief Move constructor */
        ObjectData2D(ObjectData2D&&)
            /* GCC 4.9.0 (the one from Android NDK) thinks this does not match
               the implicit signature so it can't be defaulted. Works on 4.8,
               5.0 and everywhere else, so I don't bother. */
            #if !defined(__GNUC__) || __GNUC__*100 + __GNUC_MINOR__ != 409
            noexcept
            #endif
            ;

        /** @brief Destructor */
        virtual ~ObjectData2D();

        /** @brief Copying is not allowed */
        ObjectData2D& operator=(const ObjectData2D&) = delete;

        /** @brief Move assignment */
        ObjectData2D& operator=(ObjectData2D&&)
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
        ObjectFlags2D flags() const { return _flags; }

        /**
         * @brief Translation (relative to parent)
         *
         * Available only if @ref ObjectFlag2D::HasTranslationRotationScaling
         * is set, use @ref transformation() otherwise. Applied as last in the
         * final transformation, see @ref transformation() for more
         * information.
         * @see @ref flags(), @ref rotation(), @ref scaling()
         */
        Vector2 translation() const;

        /**
         * @brief Rotation (relative to parent)
         *
         * Available only if @ref ObjectFlag2D::HasTranslationRotationScaling
         * is set, use @ref transformation() otherwise. Applied second in the
         * final transformation, see @ref transformation() for more
         * information.
         * @see @ref flags(), @ref translation(), @ref scaling()
         */
        Complex rotation() const;

        /**
         * @brief Scaling (relative to parent)
         *
         * Available only if @ref ObjectFlag2D::HasTranslationRotationScaling
         * is set, use @ref transformation() otherwise. Applied as first in the
         * final transformation, see @ref transformation() for more
         * information.
         * @see @ref flags(), @ref translation(), @ref rotation()
         */
        Vector2 scaling() const;

        /**
         * @brief Transformation (relative to parent)
         *
         * If @ref ObjectFlag2D::HasTranslationRotationScaling is not set,
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
         * @snippet MagnumTrade.cpp ObjectData2D-transformation
         *
         * @see @ref flags()
         */
        Matrix3 transformation() const;

        /**
         * @brief Instance type
         *
         * @see @ref instance()
         */
        ObjectInstanceType2D instanceType() const { return _instanceType; }

        /**
         * @brief Instance ID
         *
         * Returns ID of given camera / light / mesh etc., specified by
         * @ref instanceType(). If @ref instanceType() is
         * @ref ObjectInstanceType2D::Empty, this function returns `-1`.
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
            Transformation(const Matrix3& matrix): matrix{matrix} {}
            Transformation(const Vector2& translation, const Complex& rotation, const Vector2& scaling): trs{translation, rotation, scaling} {}
            ~Transformation() {}

            Matrix3 matrix;
            struct {
                Vector2 translation;
                Complex rotation;
                Vector2 scaling;
            } trs;
        } _transformation;
        ObjectInstanceType2D _instanceType;
        ObjectFlags2D _flags;
        Int _instance;
        const void* _importerState;
};

/** @debugoperatorenum{ObjectInstanceType2D} */
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, ObjectInstanceType2D value);

/** @debugoperatorenum{ObjectFlag2D} */
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, ObjectFlag2D value);

/** @debugoperatorenum{ObjectFlags2D} */
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, ObjectFlags2D value);

}}

#endif
