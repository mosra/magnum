#ifndef Magnum_Trade_SceneData_h
#define Magnum_Trade_SceneData_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class @ref Magnum::Trade::SceneData
 */

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/StridedArrayView.h>

#include "Magnum/Trade/Data.h"
#include "Magnum/Trade/Trade.h"
#include "Magnum/Trade/visibility.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Utility/StlForwardVector.h>
#include <Corrade/Utility/Macros.h>
#endif

namespace Magnum { namespace Trade {

#error for an archetype-like ECS there's multiple arrays of a single field, have that controlled globally on a SceneData level with a flag? new SceneIndexType? ahah

/**
@brief Scene field name
@m_since_latest

See @ref SceneData for more information.
@see @ref SceneFieldData, @ref SceneFieldType,
    @ref AbstractImporter::sceneFieldForName(),
    @ref AbstractImporter::sceneFieldName()
*/
enum class SceneField: UnsignedShort {
    /* Zero used for an invalid value */

    /**
     * Parent index. Type is usually @ref SceneFieldType::Int, but can be also
     * any of @relativeref{SceneFieldType,Byte},
     * @relativeref{SceneFieldType,Short} or, rarely, a
     * @relativeref{SceneFieldType,Long}. A value of @cpp -1 @ce means there's
     * no parent. An object should have only one parent, altough this isn't
     * enforced in any way --- only the first found entry for given object is
     * taken into account.
     *
     * Note that the index points to the parent array itself and isn't the
     * actual object index --- the object mapping is stored in the
     * corresponding @ref SceneData::fieldObject() array.
     * @see @ref SceneData::parentAsArray()
     */
    Parent = 1, // TODO rename to ParentTransformation? would no longer work for "query a hand and all the fingers on it"
    // TODO drop the self-referencability, the app should build an accel struct on top?

    // TODO: InheritFrom? BaseObject? where it'd inherit fields from given
    //      obj? not sure how that could be batch-processed, sigh (expanded at
    //       runtime? ugh)
    // TODO have SceneField as a type as well? what would that be useful for

    /**
     * Transformation. Type is usually @ref SceneFieldType::Matrix3x3 for 2D
     * and @ref SceneFieldType::Matrix4x4 for 3D, but can be also any of
     * @relativeref{SceneFieldType,Matrix3x3d},
     * @relativeref{SceneFieldType,DualComplex} or
     * @relativeref{SceneFieldType,DualComplexd} for 2D and
     * @relativeref{SceneFieldType,Matrix4x4d},
     * @relativeref{SceneFieldType,DualQuaternion} or
     * @relativeref{SceneFieldType,DualQuaterniond} for 3D. An object should
     * have only one transformation, altough this isn't enforced in any way ---
     * only the first found entry for given object is taken into account.
     *
     * The transformation can be also represented by separate
     * @ref SceneField::Translation, @ref SceneField::Rotation and
     * @ref SceneField::Scaling fields. These can be either provided in
     * addition to @ref SceneField::Transformation (in which case both are
     * assumed to represent the same transformation) or instead of it (for
     * example only when a subset of objects needs separate TRS components for
     * animation).
     * @see @ref SceneData::transformations2DAsArray(),
     *      @ref SceneData::transformations3DAsArray()
     */
    Transformation,

    /**
     * Translation. Type is usually @ref SceneFieldType::Vector2 for 2D and
     * @ref SceneFieldType::Vector3 for 3D, but can be also any of
     * @relativeref{SceneFieldType,Vector2d} for 2D and
     * @relativeref{SceneFieldType,Vector3d} for 3D. An object should
     * have only one translation, altough this isn't enforced in any way ---
     * only the first found entry for given object is taken into account.
     *
     * The translation field usually is (but doesn't have to be) complemented
     * by a @ref SceneField::Rotation and @ref SceneField::Scaling, which, if
     * present, are expected to all share the same object mapping view. The TRS
     * components can either completely replace @ref SceneField::Transformation
     * or be provided just for a subset of it, in which case both variants are
     * assumed to represent the same transformation, and for each object that
     * has the TRS components there should be a @ref SceneField::Transformation
     * as well.
     * @see @ref SceneData::transformations2DAsArray(),
     *      @ref SceneData::transformations3DAsArray()
     */
    Translation,

    /**
     * Rotation. Type is usually @ref SceneFieldType::Complex for 2D and
     * @ref SceneFieldType::Quaternion for 3D, but can be also any of
     * @relativeref{SceneFieldType,Complexd} for 2D and
     * @relativeref{SceneFieldType,Quaterniond} for 3D. An object should have
     * only one rotation, altough this isn't enforced in any way --- only the
     * first found entry for given object is taken into account.
     *
     * The rotation field usually is (but doesn't have to be) complemented by a
     * @ref SceneField::Translation and @ref SceneField::Scaling, which, if
     * present, are expected to all share the same object mapping view. The TRS
     * components can either completely replace @ref SceneField::Transformation
     * or be provided just for a subset of it, in which case both variants are
     * assumed to represent the same transformation, and for each object that
     * has the TRS components there should be a @ref SceneField::Transformation
     * as well.
     * @see @ref SceneData::transformations2DAsArray(),
     *      @ref SceneData::transformations3DAsArray()
     */
    Rotation,

    /**
     * Scaling. Type is usually @ref SceneFieldType::Vector2 for 2D and
     * @ref SceneFieldType::Vector3 for 3D, but can be also any of
     * @relativeref{SceneFieldType,Vector2d} for 2D and
     * @relativeref{SceneFieldType,Vector3d} for 3D. An object should
     * have only one scaling, altough this isn't enforced in any way ---
     * only the first found entry for given object is taken into account.
     *
     * The scaling field usually is (but doesn't have to be) complemented by a
     * @ref SceneField::Translation and @ref SceneField::Rotation, which, if
     * present, are expected to all share the same object mapping view. The TRS
     * components can either completely replace @ref SceneField::Transformation
     * or be provided just for a subset of it, in which case both variants are
     * assumed to represent the same transformation, and for each object that
     * has the TRS components there should be a @ref SceneField::Transformation
     * as well.
     * @see @ref SceneData::transformations2DAsArray(),
     *      @ref SceneData::transformations3DAsArray()
     */
    Scaling,

    /**
     * ID of a mesh associated with this object, corresponding to the ID passed
     * to @ref AbstractImporter::mesh(). Type is usually
     * @ref SceneFieldType::UnsignedInt, but can be also any of
     * @relativeref{SceneFieldType::UnsignedByte} or
     * @relativeref{SceneFieldType::UnsignedShort}. An object can have multiple
     * meshes associated.
     *
     * Usually complemented with a @ref SceneField::MeshMaterialId, although
     * not required. If present, both should share the same object mapping
     * view. Objects with multiple meshes then have the Nth mesh associated
     * with the Nth material.
     * @see @ref SceneData::meshIdsAsArray()
     */
    MeshId,

    /**
     * ID of a material for a @ref SceneFieldType::MeshId, corresponding to the
     * ID passed to @ref AbstractImporter::material(). Type is usually
     * @ref SceneFieldType::UnsignedInt, but can be also any of
     * @relativeref{SceneFieldType::UnsignedByte} or
     * @relativeref{SceneFieldType::UnsignedShort}. Expected to share the
     * object mapping view with @ref SceneFieldType::MeshMaterialId.
     * @see @ref SceneData::materialIdsAsArray()
     */
    MeshMaterialId,

    /**
     * ID of a light associated with this object, corresponding to the ID
     * passed to @ref AbstractImporter::light(). Type is usually
     * @ref SceneFieldType::UnsignedInt, but can be also any of
     * @relativeref{SceneFieldType::UnsignedByte} or
     * @relativeref{SceneFieldType::UnsignedShort}. An object can have multiple
     * lights associated.
     * @see @ref SceneData::lightIdsAsArray()
     */
    LightId,

    /**
     * ID of a camera associated with this object, corresponding to the ID
     * passed to @ref AbstractImporter::camera(). Type is usually
     * @ref SceneFieldType::UnsignedInt, but can be also any of
     * @relativeref{SceneFieldType::UnsignedByte} or
     * @relativeref{SceneFieldType::UnsignedShort}. An object can have multiple
     * cameras associated.
     * @see @ref SceneData::cameraIdsAsArray()
     */
    CameraId,

    /**
     * ID of an animation associated with this object, corresponding to the ID
     * passed to @ref AbstractImporter::animation(). Type is usually
     * @ref SceneFieldType::UnsignedInt, but can be also any of
     * @relativeref{SceneFieldType::UnsignedByte} or
     * @relativeref{SceneFieldType::UnsignedShort}. An object can have multiple
     * animations associated.
     * @see @ref SceneData::lightIdsAsArray()
     */
    AnimationId,

    /**
     * ID of a skin associated with this object, corresponding to the ID
     * passed to @ref AbstractImporter::skin(). Type is usually
     * @ref SceneFieldType::UnsignedInt, but can be also any of
     * @relativeref{SceneFieldType::UnsignedByte} or
     * @relativeref{SceneFieldType::UnsignedShort}. An object can have multiple
     * skins associated.
     * @see @ref SceneData::skinIdsAsArray()
     */
    SkinId,

//     MeshVertexOffset, // TODO all should have the same object mapping, if present
//     MeshIndexOffset,
//     MeshCount, // TODO ehh?
//     MeshInstanceOffset,
//     MeshInstanceCount,
//
//     InstanceMeshId, // TODO instances? would be cool, eh?

    // TODO texture transform + layer so we don't need 100 materials
    // TODO visible, shadow casting, collidable ... bits


    /**
     * This and all higher values are for importer-specific fields. Can be
     * of any type. See documentation of a particular importer for details.
     * @see @ref isSceneFieldCustom(), @ref sceneFieldCustom(SceneField),
     *      @ref sceneFieldCustom(UnsignedShort)
     */
    Custom = 32768 // TODO more? for user-made entities, flags etc, this might not be enough
};

/**
@debugoperatorenum{SceneField}
@m_since_latest
*/
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, SceneField value);

/**
@brief Whether a scene field is custom
@m_since_latest

Returns @cpp true @ce if @p name has a value larger or equal to
@ref SceneField::Custom, @cpp false @ce otherwise.
@see @ref sceneFieldCustom(UnsignedShort), @ref sceneFieldCustom(SceneField)
*/
constexpr bool isSceneFieldCustom(SceneField name) {
    return UnsignedShort(name) >= UnsignedShort(SceneField::Custom);
}

/**
@brief Create a custom scene field
@m_since_latest

Returns a custom scene field with index @p id. The index is expected to be less
than the value of @ref SceneField::Custom. Use @ref sceneFieldCustom(SceneField)
to get the index back.
*/
/* Constexpr so it's usable for creating compile-time SceneFieldData
   instances */
constexpr SceneField sceneFieldCustom(UnsignedShort id) {
    return CORRADE_CONSTEXPR_ASSERT(id < UnsignedShort(SceneField::Custom),
        "Trade::sceneFieldCustom(): index" << id << "too large"),
        SceneField(UnsignedShort(SceneField::Custom) + id);
}

/**
@brief Get index of a custom scene field
@m_since_latest

Inverse to @ref sceneFieldCustom(UnsignedShort). Expects that the field is
custom.
@see @ref isSceneFieldCustom()
*/
constexpr UnsignedShort sceneFieldCustom(SceneField name) {
    return CORRADE_CONSTEXPR_ASSERT(isSceneFieldCustom(name),
        "Trade::sceneFieldCustom():" << name << "is not custom"),
        UnsignedShort(name) - UnsignedShort(SceneField::Custom);
}

/**
@brief Scene field type
@m_since_latest

A type in which a @ref SceneField is stored. See @ref SceneData for more
information.
@see @ref SceneFieldData, @ref sceneFieldTypeSize()
*/
enum class SceneFieldType: UnsignedShort {
    /* Zero used for an invalid value */

    /* 1 reserved for Bool, which needs [Strided]BoolArray[View] first */
//     Bool = 1, // TODO needs a BoolArray! and also special overloads, sigh
        // also StridedBoolArrayView?!

    Float = 2,      /**< @relativeref{Magnum,Float} */
    Half,           /**< @relativeref{Magnum,Half} */
    Double,         /**< @relativeref{Magnum,Double} */
    UnsignedByte,   /**< @relativeref{Magnum,UnsignedByte} */
    Byte,           /**< @relativeref{Magnum,Byte} */
    UnsignedShort,  /**< @relativeref{Magnum,UnsignedShort} */
    Short,          /**< @relativeref{Magnum,Short} */
    UnsignedInt,    /**< @relativeref{Magnum,UnsignedInt} */
    Int,            /**< @relativeref{Magnum,Int} */
    UnsignedLong,   /**< @relativeref{Magnum,UnsignedLong} */
    Long,           /**< @relativeref{Magnum,Long} */

    Vector2,        /**< @relativeref{Magnum,Vector2} */
    Vector2h,       /**< @relativeref{Magnum,Vector2h} */
    Vector2d,       /**< @relativeref{Magnum,Vector2d} */
    Vector2ub,      /**< @relativeref{Magnum,Vector2ub} */
    Vector2b,       /**< @relativeref{Magnum,Vector2b} */
    Vector2us,      /**< @relativeref{Magnum,Vector2us} */
    Vector2s,       /**< @relativeref{Magnum,Vector2s} */
    Vector2ui,      /**< @relativeref{Magnum,Vector2ui} */
    Vector2i,       /**< @relativeref{Magnum,Vector2i} */

    Vector3,        /**< @relativeref{Magnum,Vector3} */
    Vector3h,       /**< @relativeref{Magnum,Vector3h} */
    Vector3d,       /**< @relativeref{Magnum,Vector3d} */
    Vector3ub,      /**< @relativeref{Magnum,Vector3ub} */
    Vector3b,       /**< @relativeref{Magnum,Vector3b} */
    Vector3us,      /**< @relativeref{Magnum,Vector3us} */
    Vector3s,       /**< @relativeref{Magnum,Vector3s} */
    Vector3ui,      /**< @relativeref{Magnum,Vector3ui} */
    Vector3i,       /**< @relativeref{Magnum,Vector3i} */

    Vector4,        /**< @relativeref{Magnum,Vector4} */
    Vector4h,       /**< @relativeref{Magnum,Vector4h} */
    Vector4d,       /**< @relativeref{Magnum,Vector4d} */
    Vector4ub,      /**< @relativeref{Magnum,Vector4ub} */
    Vector4b,       /**< @relativeref{Magnum,Vector4b} */
    Vector4us,      /**< @relativeref{Magnum,Vector4us} */
    Vector4s,       /**< @relativeref{Magnum,Vector4s} */
    Vector4ui,      /**< @relativeref{Magnum,Vector4ui} */
    Vector4i,       /**< @relativeref{Magnum,Vector4i} */

    Matrix2x2,      /**< @relativeref{Magnum,Matrix2x2} */
    Matrix2x2h,     /**< @relativeref{Magnum,Matrix2x2h} */
    Matrix2x2d,     /**< @relativeref{Magnum,Matrix2x2d} */

    Matrix2x3,      /**< @relativeref{Magnum,Matrix2x3} */
    Matrix2x3h,     /**< @relativeref{Magnum,Matrix2x3h} */
    Matrix2x3d,     /**< @relativeref{Magnum,Matrix2x3d} */

    Matrix2x4,      /**< @relativeref{Magnum,Matrix2x4} */
    Matrix2x4h,     /**< @relativeref{Magnum,Matrix2x4h} */
    Matrix2x4d,     /**< @relativeref{Magnum,Matrix2x4d} */

    Matrix3x2,      /**< @relativeref{Magnum,Matrix3x2} */
    Matrix3x2h,     /**< @relativeref{Magnum,Matrix3x2h} */
    Matrix3x2d,     /**< @relativeref{Magnum,Matrix3x2d} */

    Matrix3x3,      /**< @relativeref{Magnum,Matrix3x3} */
    Matrix3x3h,     /**< @relativeref{Magnum,Matrix3x3h} */
    Matrix3x3d,     /**< @relativeref{Magnum,Matrix3x3d} */

    Matrix3x4,      /**< @relativeref{Magnum,Matrix3x4} */
    Matrix3x4h,     /**< @relativeref{Magnum,Matrix3x4h} */
    Matrix3x4d,     /**< @relativeref{Magnum,Matrix3x4d} */

    Matrix4x2,      /**< @relativeref{Magnum,Matrix4x2} */
    Matrix4x2h,     /**< @relativeref{Magnum,Matrix4x2h} */
    Matrix4x2d,     /**< @relativeref{Magnum,Matrix4x2d} */

    Matrix4x3,      /**< @relativeref{Magnum,Matrix4x3} */
    Matrix4x3h,     /**< @relativeref{Magnum,Matrix4x3h} */
    Matrix4x3d,     /**< @relativeref{Magnum,Matrix4x3d} */

    Matrix4x4,      /**< @relativeref{Magnum,Matrix4x4} */
    Matrix4x4h,     /**< @relativeref{Magnum,Matrix4x4h} */
    Matrix4x4d,     /**< @relativeref{Magnum,Matrix4x4d} */

    Range1D,        /**< @relativeref{Magnum,Range1D} */
    Range1Dh,       /**< @relativeref{Magnum,Range1Dh} */
    Range1Dd,       /**< @relativeref{Magnum,Range1Dd} */
    Range1Di,       /**< @relativeref{Magnum,Range1Di} */

    Range2D,        /**< @relativeref{Magnum,Range2D} */
    Range2Dh,       /**< @relativeref{Magnum,Range2Dh} */
    Range2Dd,       /**< @relativeref{Magnum,Range2Dd} */
    Range2Di,       /**< @relativeref{Magnum,Range2Di} */

    Range3D,        /**< @relativeref{Magnum,Range3D} */
    Range3Dh,       /**< @relativeref{Magnum,Range3Dh} */
    Range3Dd,       /**< @relativeref{Magnum,Range3Dd} */
    Range3Di,       /**< @relativeref{Magnum,Range3Di} */

    Complex,        /**< @relativeref{Magnum,Complex} */
    Complexd,       /**< @relativeref{Magnum,Complexd} */
    DualComplex,    /**< @relativeref{Magnum,DualComplex} */
    DualComplexd,   /**< @relativeref{Magnum,DualComplexd} */

    Quaternion,     /**< @relativeref{Magnum,Quaternion} */
    Quaterniond,    /**< @relativeref{Magnum,Quaterniond} */
    DualQuaternion, /**< @relativeref{Magnum,DualQuaternion} */
    DualQuaterniond,/**< @relativeref{Magnum,DualQuaterniond} */

    Deg,            /**< @relativeref{Magnum,Deg} */
    Degh,           /**< @relativeref{Magnum,Degh} */
    Degd,           /**< @relativeref{Magnum,Degh} */
    Rad,            /**< @relativeref{Magnum,Rad} */
    Radh,           /**< @relativeref{Magnum,Radh} */
    Radd,           /**< @relativeref{Magnum,Radd} */

    // TODO: pointer, like with MaterialData? could be for having real MeshData
    //  LightData / ... references

    // TODO: doc that no need to store any custom types here so it can be just
    // 16 bits

    /* Can be at most 8191 types, as it's stored together with
       SceneFieldIndexType and isOffsetOnly in 16 bits */ // TODO outdated!
};

/**
@debugoperatorenum{SceneFieldType}
@m_since_latest
*/
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, SceneFieldType value);

/**
@brief Size of given scene field type
@m_since_latest
*/
MAGNUM_TRADE_EXPORT UnsignedInt sceneFieldTypeSize(SceneFieldType type);

/**
@brief Scene object index type
@m_since_latest

Type used for associating fields with corresponding objects. Unlike
@ref SceneFieldType that is different for different fields, the index type is
the same for all fields.
@see @ref SceneData::objectIndexType(), @ref sceneIndexTypeSize()
*/
enum class SceneIndexType: UnsignedByte { // TODO SceneObjectType?
    /* Zero used for an invalid value */

    UnsignedByte = 1,   /**< @relativeref{Magnum,UnsignedByte} */
    UnsignedShort,      /**< @relativeref{Magnum,UnsignedShort} */
    UnsignedInt,        /**< @relativeref{Magnum,UnsignedInt} */

    /**
     * @relativeref{Magnum,UnsignedLong}. Meant to be used only in rare cases
     * for *really huge* scenes. If this type is used and
     * @ref SceneData::objectCount() is larger than the max representable
     * 32-bit value, the indices can't be retrieved using
     * @ref fieldObjectAsArray(), but only using an appropriately typed
     * @ref fieldObject().
     */
    UnsignedLong

    // TODO make the largest bit a sparse identifier if given field has a sparse flag
};

/**
@debugoperatorenum{SceneIndexType}
@m_since_latest
*/
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, SceneIndexType value);

// TODO where to have string to object mapping? some serializable way? yep, a
//  designated StringMapData

/**
@brief Size of given scene index type
@m_since_latest
*/
MAGNUM_TRADE_EXPORT UnsignedInt sceneIndexTypeSize(SceneIndexType type);

/**
@brief Scene field data
@m_since_latest

Convenience type for populating @ref SceneData, see its documentation for an
introduction.

@section Trade-SceneFieldData-usage Usage

The most straightforward usage is constructing an instance from a
@ref SceneField and a strided view for the field data and object mapping. The
@ref SceneIndexType and @ref SceneFieldType gets inferred from the view types:

@snippet MagnumTrade.cpp SceneFieldData-usage

Alternatively, you can pass typeless @cpp const void @ce or 2D views and supply
@ref SceneIndexType and @ref SceneFieldType explicitly.

@subsection Trade-SceneFieldData-usage-offset-only Offset-only field data

If the actual field / object data location is not known yet, the instance can
be created as "offset-only", meaning the actual view gets created only later
when passed to a @ref SceneData instance with a concrete data array. This is
useful mainly to avoid pointer patching during data serialization, less so when
the data layout is static (and thus can be defined at compile time), but the
actual data is allocated / populated at runtime:

@snippet MagnumTrade.cpp SceneFieldData-usage-offset-only
*/
class MAGNUM_TRADE_EXPORT SceneFieldData {
    public:
        /**
         * @brief Default constructor
         *
         * Leaves contents at unspecified values. Provided as a convenience for
         * initialization of the field array for @ref SceneData, expected to be
         * replaced with concrete values later.
         */
        constexpr explicit SceneFieldData() noexcept: _countTypeOffsetOnly{}, _name{}, _fieldStride{}, _objectStride{}, _arraySize{}, _fieldData{}, _objectData{} {}

        /**
         * @brief Type-erased constructor
         * @param name      Field name
         * @param type      Field type
         * @param data      Field data
         * @param indexType Object index type
         * @param indexData Object index data
         * @param arraySize Array size. Use @cpp 0 @ce for non-array fields.
         *
         * Expects that @p data and @p indexData have the same size, @p data
         * stride is large enough to fit all @p arraySize items of @p type,
         * @p type corresponds to @p name and @p arraySize is zero for builtin
         * fields.
         */
        explicit SceneFieldData(SceneField name, SceneFieldType type, const Containers::StridedArrayView1D<const void>& data, SceneIndexType indexType, const Containers::StridedArrayView1D<const void>& indexData, UnsignedShort arraySize = 0) noexcept;

        /**
         * @brief Constructor
         * @param name      Field name
         * @param type      Field type
         * @param data      Field data
         * @param indexData Object index data
         * @param arraySize Array size. Use @cpp 0 @ce for non-array fie.
         *
         * Expects that @p data and @p indexData have the same size in the
         * first dimension; the second dimension of @p data is contiguous and
         * its size matches @p type and @p arraSize, that @p type corresponds
         * to @p name and @p arraySize is zero for builtin attributes; that
         * the second dimension of @p indexData is contiguous and its size is
         * either 1, 2, 4 or 8, corresponding to one of the
         * @ref SceneFieldIndexType values.
         */
        explicit SceneFieldData(SceneField name, SceneFieldType type, const Containers::StridedArrayView2D<const char>& data, const Containers::StridedArrayView2D<const char>& indexData, UnsignedShort arraySize = 0) noexcept;

        /**
         * @brief Constructor
         * @param name      Field name
         * @param data      Field data
         * @param indexData Object index data
         *
         * Detects @ref SceneFieldType based on @p T and @ref SceneIndexType
         * based on @p U and calls @ref SceneFieldData(SceneField, SceneFieldType, const Containers::StridedArrayView1D<const void>&, SceneFieldIndexType, const Containers::StridedArrayView1D<const void>&, UnsignedShort).
         * For all types known by Magnum, the detected @ref SceneFieldType is
         * of the same name as the type (so e.g. @relativeref{Magnum,Vector3ui}
         * gets recognized as @ref SceneFieldType::Vector3ui).
         */
        template<class T, class U> constexpr explicit SceneFieldData(SceneField name, const Containers::StridedArrayView1D<T>& data, const Containers::StridedArrayView1D<U>& indexData) noexcept;

        /** @overload */
        template<class T, class U> constexpr explicit SceneFieldData(SceneField name, const Containers::StridedArrayView1D<T>& data, const Containers::ArrayView<U>& indexData) noexcept: SceneFieldData{name, data, Containers::stridedArrayView(indexData)} {}

        /** @overload */
        template<class T, class U> constexpr explicit SceneFieldData(SceneField name, const Containers::ArrayView<T>& data, const Containers::StridedArrayView1D<U>& indexData) noexcept: SceneFieldData{name, Containers::stridedArrayView(data), indexData} {}

        /** @overload */
        template<class T, class U> constexpr explicit SceneFieldData(SceneField name, const Containers::ArrayView<T>& data, const Containers::ArrayView<U>& indexData) noexcept: SceneFieldData{name, Containers::stridedArrayView(data), Containers::stridedArrayView(indexData)} {}

        /**
         * @brief Construct an array field
         * @param name      Field name
         * @param data      Field data
         * @param indexData Object index data
         *
         * Detects @ref SceneFieldType based on @p T and @ref SceneIndexType
         * based on @p U and calls @ref SceneFieldData(SceneField, SceneFieldType, const Containers::StridedArrayView1D<const void>&, SceneFieldIndexType, const Containers::StridedArrayView1D<const void>&, UnsignedShort)
         * with the @p data second dimension size passed to @p arraySize.
         * Expects that the second dimension of @p data is contiguous. At the
         * moment only custom fields can be arrays, which means this function
         * can't be used with a builtin @p name. See @ref MeshAttributeData(SceneField, const Containers::StridedArrayView1D<T>&, const Containers::StridedArrayView1D<U>&)
         * for details about @ref SceneFieldType and @ref SceneIndexType
         * detection.
         */
        template<class T, class U> constexpr explicit SceneFieldData(SceneField name, const Containers::StridedArrayView2D<T>& data, const Containers::StridedArrayView1D<U>& indexData) noexcept;

        /** @overload */
        template<class T, class U> constexpr explicit SceneFieldData(SceneField name, const Containers::StridedArrayView2D<T>& data, const Containers::ArrayView<U>& indexData) noexcept;

        /**
         * @brief Construct an offset-only field
         * @param name          Field name
         * @param count         Object count
         * @param type          Field type
         * @param offset        Field data offset
         * @param stride        Field data stride
         * @param indexOffset   Object index data offset
         * @param indexStride   Object index data stride
         * @param arraySize     Array size. Use @cpp 0 @ce for non-array
         *      fields.
         *
         * Instances created this way refer to offsets in unspecified
         * external scene data instead of containing the data views directly.
         * Useful when the location of the scene data array is not known at
         * field construction time. Expects that @p arraySize is zero for
         * builtin attributes.
         *
         * Note that due to the @cpp constexpr @ce nature of this constructor,
         * no @p type / @p arraySize checks against @p stride can be done.
         * You're encouraged to use the @ref MeshAttributeData(MeshAttribute, VertexFormat, const Containers::StridedArrayView1D<const void>&, UnsignedShort)
         * constructor if you want additional safeguards.
         * @see @ref isOffsetOnly(), @ref arraySize(),
         *      @ref data(Containers::ArrayView<const void>) const
         */
        explicit constexpr SceneFieldData(SceneField name, UnsignedLong count, SceneFieldType type, std::size_t offset, std::ptrdiff_t stride, std::size_t indexOffset, std::ptrdiff_t indexStride, UnsignedShort arraySize = 0) noexcept;

        /**
         * @brief If the field is offset-only
         *
         * Returns @cpp true @ce if the field doesn't contain the data views
         * directly, but instead refers to unspecified external data.
         * @see @ref data(Containers::ArrayView<const void>) const,
         *      @ref indexData(Containers::ArrayView<const void>) const,
         *      @ref MeshAttributeData(SceneField, SceneFieldType, std::size_t, UnsignedInt, std::ptrdiff_t, UnsignedShort)
         */
        constexpr bool isOffsetOnly() const;

        /** @brief Field name */
        constexpr SceneField name() const { return _name; }

        /** @brief Field type */
        constexpr SceneFieldType type() const;

        /** @brief Field array size */
        constexpr UnsignedShort arraySize() const { return _arraySize; }

        /**
         * @brief Type-erased field data
         *
         * Expects that the field is not offset-only, in that case use the
         * @ref data(Containers::ArrayView<const void>) const overload instead.
         * @see @ref isOffsetOnly()
         */
        constexpr Containers::StridedArrayView1D<const void> data() const;

        /**
         * @brief Type-erased field data for an offset-only attribute
         *
         * If the field is not offset-only, the @p sceneData parameter is
         * ignored.
         * @see @ref isOffsetOnly(), @ref data() const
         */
        Containers::StridedArrayView1D<const void> data(Containers::ArrayView<const void> sceneData) const;

        /**
         * @brief Type-erased object index data
         *
         * Expects that the field is not offset-only, in that case use the
         * @ref data(Containers::ArrayView<const void>) const overload instead.
         * @see @ref isOffsetOnly()
         */
        constexpr Containers::StridedArrayView1D<const void> indexData() const;

        /**
         * @brief Type-erased object index data for an offset-only attribute
         *
         * If the field is not offset-only, the @p vertexData parameter is
         * ignored.
         * @see @ref isOffsetOnly(), @ref data() const
         */
        Containers::StridedArrayView1D<const void> indexData(Containers::ArrayView<const void> sceneData) const;

    private:
        /* Count (48) | SceneFieldType (15) | isOffsetOnly (1) */
        // TODO has to store index type so SceneData can verify all are the same ffs
        // TODO should store a bit describing if it's sorted, for perf
        UnsignedLong _countTypeOffsetOnly;
        SceneField _name;
        UnsignedShort _fieldStride;
        UnsignedShort _objectStride;
        UnsignedShort _arraySize;
        // TODO fuck, for bools we need additional 3 bits to store bit position
        // TODO or store that in array size in that case? eugh

        // TODO: use extra 8 bytes? 2 for SceneFieldType, 1 for offset/sorted flags, 1 for bit position, 1 for index type, 3 left
        // TODO also a "sparse" flag

        union Data {
            const void* pointer;
            std::size_t offset;
        };
        Data _fieldData, _objectData; // TODO object data = 0 means ~implicit~? for implicit the data could contain a single monotonic array shared by all
        // TODO oooh implicit could be useful for *huge* bitfields that span all objects, because there it makes no sense to multiply the data size 9x just to specify a range the bits affect
};

/**
@brief Scene data

Contains scene hierarchy, object transformations and association of mesh,
material, camera, light and other resources with particular objects.

@section Trade-SceneData-terminology Terminology and representation

The class stores all data in a single array and all metadata in another. The
internal design is similar to @ref MeshData, except that the contents are not
limited to what the GPU vertex pipeline can understand. The two major concepts
are Objects and Fields.

-   *Objects* in the scene are represented as a contiguous sequence of numeric
    IDs from @cpp 0 @ce up to @ref objectCount() minus one. The object itself
    contains *nothing* by default, not even the transformation, it's just a
    unique index.
-   *Fields* are properties associated with particular objects, such as a
    transformation matrix, a mesh ID or any other from @ref SceneField. Data
    for a particular field are described by a pair of (strided) array views,
    one containing the actual field data (with a type from @ref SceneFieldType)
    and the other describing which object index the field belongs to (all
    indices being of a type from @ref SceneIndexType). The only restriction on
    fields is that there can be at most one array for one particular
    @ref SceneField, apart from that the same field can generally be be
    associated with a particular object index multiple times (such as one
    object having multiple meshes attached) and, conversely, a field doesn't
    have to be defined for each object (so e.g. for a scene with a hundred
    objects the @ref SceneField::LightId can have just three items). The actual
    data layout is also left up to the data producer / importer --- different
    fields can share the same object mapping view, a group of fields that
    applies to a set of objects can (but doesn't have to) be interleaved
    together etc.

@section Trade-SceneData-usage Basic usage

@ref TODO asArray, expected to use those to fill some ECS thingy

@subsection Trade-SceneData-usage-object Per-object access

@ref TODO not recommended but available for debugging purposes

@section Trade-SceneData-usage-advanced Advanced usage

@ref TODO via direct accessors

@section Trade-SceneData-usage-mutable Mutable data access

@ref TODO possibly for storing global animated/simulated state? :O or for
    reordering data

@section Trade-SceneData-populating Populating an instance

@ref TODO ArrayTuple FTW

@snippet MagnumTrade.cpp SceneData-populating

@ref TODO arrays etc? where it was in MeshDatA?

@see @ref AbstractImporter::scene()
*/
class MAGNUM_TRADE_EXPORT SceneData {
    public:

        explicit SceneData(UnsignedLong objectCount, SceneIndexType objectIndexType, Containers::Array<char>&& data, Containers::Array<SceneFieldData>&& fields, const void* importerState = nullptr) noexcept;

        /** @overload */
        /* Not noexcept because allocation happens inside */
        explicit SceneData(UnsignedLong objectCount, SceneIndexType objectIndexType, Containers::Array<char>&& data, std::initializer_list<SceneFieldData> fields, const void* importerState = nullptr);

        explicit SceneData(UnsignedLong objectCount, SceneIndexType objectIndexType, DataFlags dataFlags, Containers::ArrayView<const void> data, Containers::Array<SceneFieldData>&& fields, const void* importerState = nullptr) noexcept;

        /** @overload */
        /* Not noexcept because allocation happens inside */
        explicit SceneData(UnsignedLong objectCount, SceneIndexType objectIndexType, DataFlags dataFlags, Containers::ArrayView<const void> data, std::initializer_list<SceneFieldData> fields, const void* importerState = nullptr);

        /** @brief Copying is not allowed */
        SceneData(const SceneData&) = delete;

        /** @brief Move constructor */
        SceneData(SceneData&&) noexcept;

        ~SceneData();

        /** @brief Copying is not allowed */
        SceneData& operator=(const SceneData&) = delete;

        /** @brief Move assignment */
        SceneData& operator=(SceneData&&) noexcept;

        /**
         * @brief Data flags
         * @m_since_latest
         *
         * @see @ref releaseData(), @ref mutableData(), @ref mutableField(),
         *      @ref mutableFieldObject()
         */
        DataFlags dataFlags() const { return _dataFlags; }

        /**
         * @brief Raw data
         * @m_since_latest
         *
         * Returns @cpp nullptr @ce if the scene has no data.
         */
        Containers::ArrayView<const char> data() const & { return _data; }

        /**
         * @brief Taking a view to a r-value instance is not allowed
         * @m_since_latest
         */
        Containers::ArrayView<const char> data() const && = delete;

        /**
         * @brief Mutable raw data
         * @m_since_latest
         *
         * Like @ref data(), but returns a non-const view. Expects that the
         * scene is mutable.
         * @see @ref dataFlags()
         */
        Containers::ArrayView<char> mutableData() &;

        /**
         * @brief Taking a view to a r-value instance is not allowed
         * @m_since_latest
         */
        Containers::ArrayView<char> mutableData() && = delete;

        /**
         * @brief Raw field metadata
         * @m_since_latest
         *
         * Returns the raw data that are used as a base for all `field*()`
         * accessors, or @cpp nullptr @ce if the scene has no fields. In most
         * cases you don't want to access those directly, but rather use the
         * @ref field(), @ref fieldObject(), @ref fieldName(), @ref fieldType()
         * etc. accessors. Compared to those and to @ref fieldData(UnsignedInt) const,
         * the @ref SceneFieldData instances returned by this function may have
         * different data pointers, and some of them might be offset-only ---
         * use this function only if you *really* know what are you doing.
         * @see @ref SceneFieldData::isOffsetOnly()
         */
        Containers::ArrayView<const SceneFieldData> fieldData() const & { return _fields; }

        /**
         * @brief Taking a view to a r-value instance is not allowed
         * @m_since_latest
         */
        Containers::ArrayView<const SceneFieldData> fieldData() const && = delete;

        /**
         * @brief Field count
         * @m_since_latest
         *
         * Count of different fields contained in the scene, or @cpp 0 @ce for
         * a scene with no fields. Each @ref SceneField can be present only
         * once, however an object can have a certain field associated with it
         * multiple times with different values (for example an object having
         * multiple meshes). See also @ref objectCount() which returns count of
         * actual objects.
         */
        UnsignedInt fieldCount() const { return _fields.size(); }

        /**
         * @brief Raw field data
         * @m_since_latest
         *
         * Returns the raw data that are used as a base for all `field*()`
         * accessors. In most cases you don't want to access those directly,
         * but rather use the @ref field(), @ref fieldObject(),
         * @ref fieldName(), @ref fieldType() etc. accessors.
         *
         * Unlike with @ref fieldData() and @ref releaseFieldData(), returned
         * instances are guaranteed to always have an absolute data pointer
         * (i.e., @ref SceneFieldData::isOffsetOnly() always returning
         * @cpp false @ce). The @p id is expected to be smaller than
         * @ref fieldCount().
         */
        SceneFieldData fieldData(UnsignedInt id) const;

        /**
         * @brief Field name
         * @m_since_latest
         *
         * The @p id is expected to be smaller than @ref fieldCount().
         * @see @ref fieldType(), @ref isSceneFieldCustom(),
         *      @ref AbstractImporter::sceneFieldForName(),
         *      @ref AbstractImporter::sceneFieldName()
         */
        SceneField fieldName(UnsignedInt id) const;

        /**
         * @brief Field type
         * @m_since_latest
         *
         * The @p id is expected to be smaller than @ref fieldCount(). You can
         * also use @ref fieldType(SceneField) to directly get a type of given
         * named field.
         * @see @ref fieldName(), @ref objectIndexType()
         */
        SceneFieldType fieldType(UnsignedInt id) const;

        /**
         * @brief Field array size
         * @m_since_latest
         *
         * In case given field is an array (the euqivalent of e.g.
         * @cpp int[30] @ce), returns array size, otherwise returns @cpp 0 @ce.
         * At the moment only custom fields can be arrays, no builtin
         * @ref SceneField is an array attribute. The @p id is expected to be
         * smaller than @ref fieldCount(). You can also use
         * @ref fieldArraySize(SceneField) to directly get array size of given
         * named attribute.
         *
         * Note that this is different from the count of objects that have
         * given field, which is exposed through @ref fieldCount(). See
         * @ref Trade-SceneData-populating-custom for an example.
         */
        UnsignedShort fieldArraySize(UnsignedInt id) const;

        /**
         * @brief Whether the scene has given field
         * @m_since_latest
         */
        bool hasField(SceneField name) const;

        /**
         * @brief Absolute ID of a named field
         * @m_since_latest
         */
        UnsignedInt fieldId(SceneField name) const;

        /**
         * @brief Type of a named field
         * @m_since_latest
         *
         * @see @ref fieldType(UnsignedInt) const
         */
        SceneFieldType fieldType(SceneField name) const;

        /**
         * @brief Array size of a named field
         * @m_since_latest
         *
         * @see @ref fieldArraySize(UnsignedInt) const
         */
        UnsignedShort fieldArraySize(SceneField name) const;

        /**
         * @brief Total object count
         * @m_since_latest
         *
         * Total number of objects contained in the scene. Use
         * @ref objectCount(SceneField) to get the number of objects associated
         * with given field.
         * @see @ref fieldCount()
         */
        UnsignedLong objectCount() const { return _objectCount; }

        /**
         * @brief Number of objects associated with given field
         * @m_since_latest
         *
         * In other words, size of the view returned by @ref field() /
         * @ref mutableField and @ref fieldObject() / @ref mutableFieldObject().
         * Note that an object can have a certain field associated with it
         * multiple times with different values (for example an object having
         * multiple meshes). The @p id is expected to be smaller than
         * @ref fieldCount().
         */
        UnsignedLong objectCount(UnsignedInt id) const;

        /**
         * @brief Number of objects associated with given field
         * @m_since_latest
         *
         * The @p name is expected to exist.
         * @see @ref hasField(), @ref objectCount(UnsignedInt)
         */
        UnsignedLong objectCount(SceneField name) const; // TODO better name, fieldInstanceCount? fieldElementCount?

        /**
         * @brief Type used for object indexing
         * @m_since_latest
         *
         * Type returned from @ref fieldObject() and @ref mutableFieldObject().
         * For simplicity it's the same for all fields.
         */
        SceneIndexType objectIndexType() const { return _indexType; }

        /**
         * @brief Data for given field
         * @m_since_latest
         *
         * The @p id is expected to be smaller than @ref fieldCount(). The
         * second dimension represents the actual data type (its size is equal
         * to @ref SceneFieldType size, possibly multiplied by array size) and
         * is guaranteed to be contiguous. Use the templated overload below to
         * get the field in a concrete type.
         * @see @ref Corrade::Containers::StridedArrayView::isContiguous(),
         *      @ref sceneFieldTypeSize(), @ref mutableField(UnsignedInt)
         */
        Containers::StridedArrayView2D<const char> field(UnsignedInt id) const;

        /**
         * @brief Mutable data for given field
         * @m_since_latest
         *
         * Like @ref field(UnsignedInt) const, but returns a mutable view.
         * Expects that the scene is mutable.
         * @see @ref dataFlags()
         */
        Containers::StridedArrayView2D<char> mutableField(UnsignedInt id);

        /**
         * @brief Data for given field in a concrete type
         * @m_since_latest
         *
         * The @p id is expected to be smaller than @ref fieldCount() and @p T
         * is expected to correspond to @ref fieldType(UnsignedInt) const. The
         * field is also expected to not be an array, in that case you need to
         * use the overload below by using @cpp T[] @ce instead of @cpp T @ce.
         * You can also use the non-templated @ref parentsAsArray(),
         * @ref transformations2DAsArray(), @ref transformations3DAsArray(),
         * @ref meshIdsAsArray(), @ref materialIdsAsArray(),
         * @ref lightIdsAsArray(), @ref cameraIdsAsArray(),
         * @ref animationIdsAsArray(), @ref skinIdsAsArray() accessors to get
         * common fields converted to usual types, but note that these
         * operations involve extra allocation and data conversion.
         * @see @ref field(SceneField), @ref mutableField(UnsignedInt),
         *      @ref fieldArraySize()
         */
        template<class T, class = typename std::enable_if<!std::is_array<T>::value>::type> Containers::StridedArrayView1D<const T> field(UnsignedInt id) const;

        /**
         * @brief Data for given array field in a concrete type
         * @m_since_latest
         *
         * Same as above, except that it works with array fields instead ---
         * you're expected to select this overload by passing @cpp T[] @ce
         * instead of @cpp T @ce. The second dimension is guaranteed to be
         * contiguous and have the same size as reported by
         * @ref fieldArraySize() for given field.
         */
        template<class T, class = typename std::enable_if<std::is_array<T>::value>::type> Containers::StridedArrayView2D<const T> field(UnsignedInt id) const;

        /**
         * @brief Mutable data for given field in a concrete type
         * @m_since_latest
         *
         * Like @ref field(UnsignedInt), but returns a mutable view. Expects
         * that the scene is mutable.
         * @see @ref dataFlags()
         */
        template<class T, class = typename std::enable_if<!std::is_array<T>::value>::type> Containers::StridedArrayView1D<T> mutableField(UnsignedInt id);

        /**
         * @brief Mutable data for given array field in a concrete type
         * @m_since_latest
         *
         * Same as above, except that it works with array fields instead ---
         * you're expected to select this overload by passing @cpp T[] @ce
         * instead of @cpp T @ce. The second dimension is guaranteed to be
         * contiguous and have the same size as reported by
         * @ref fieldArraySize() for given field.
         */
        template<class T, class = typename std::enable_if<std::is_array<T>::value>::type> Containers::StridedArrayView2D<T> mutableField(UnsignedInt id);

        /**
         * @brief Data for given named field
         * @m_since_latest
         *
         * The @p name is expected to exist. The second dimension represents
         * the actual data type (its size is equal to @ref SceneFieldType size,
         * possibly multiplied by array size) and is guaranteed to be
         * contiguous. Use the templated overload below to get the field in a
         * concrete type.
         * @see @ref hasField(), @ref field(UnsignedInt),
         *      @ref mutableField(SceneField),
         *      @ref Corrade::Containers::StridedArrayView::isContiguous()
         */
        Containers::StridedArrayView2D<const char> field(SceneField name) const;

        /**
         * @brief Mutable data for given named field
         * @m_since_latest
         *
         * Like @ref field(SceneField), but returns a mutable view. Expects
         * that the scene is mutable.
         * @see @ref dataFlags()
         */
        Containers::StridedArrayView2D<char> mutableField(SceneField name);

        /**
         * @brief Data for given named field in a concrete type
         * @m_since_latest
         *
         * The @p name is expected to exist and @p T is expected to correspond
         * to @ref fieldFormat(SceneField). The field is also expected to not
         * be an array, in that case you need to use the overload below by
         * using @cpp T[] @ce instead of @cpp T @ce. You can also use the
         * non-templated @ref parentsAsArray(), @ref transformations2DAsArray(),
         * @ref transformations3DAsArray(), @ref meshIdsAsArray(),
         * @ref materialIdsAsArray(), @ref lightIdsAsArray(),
         * @ref cameraIdsAsArray(), @ref animationIdsAsArray(),
         * @ref skinIdsAsArray() accessors to get common fields converted to
         * usual types, but note that these operations involve extra allocation
         * and data conversion.
         * @see @ref field(UnsignedInt), @ref mutableField(SceneField)
         */
        template<class T, class = typename std::enable_if<!std::is_array<T>::value>::type> Containers::StridedArrayView1D<const T> field(SceneField name) const;

        /**
         * @brief Data for given named array field in a concrete type
         * @m_since_latest
         *
         * Same as above, except that it works with array fields instead ---
         * you're expected to select this overload by passing @cpp T[] @ce
         * instead of @cpp T @ce. The second dimension is guaranteed to be
         * contiguous and have the same size as reported by
         * @ref fieldArraySize() for given field.
         */
        template<class T, class = typename std::enable_if<std::is_array<T>::value>::type> Containers::StridedArrayView2D<const T> field(SceneField name) const;

        /**
         * @brief Mutable data for given named field
         * @m_since_latest
         *
         * Like @ref field(SceneField), but returns a mutable view. Expects
         * that the scene is mutable.
         * @see @ref dataFlags()
         */
        template<class T, class = typename std::enable_if<!std::is_array<T>::value>::type> Containers::StridedArrayView1D<T> mutableField(SceneField name);

        /**
         * @brief Mutable data for given named array field in a concrete type
         * @m_since_latest
         *
         * Same as above, except that it works with array fields instead ---
         * you're expected to select this overload by passing @cpp T[] @ce
         * instead of @cpp T @ce. The second dimension is guaranteed to be
         * contiguous and have the same size as reported by
         * @ref fieldArraySize() for given field.
         */
        template<class T, class = typename std::enable_if<std::is_array<T>::value>::type> Containers::StridedArrayView2D<T> mutableField(SceneField name);

        /**
         * @brief Object index data for given field
         * @m_since_latest
         *
         * The @p id is expected to be smaller than @ref fieldCount(). The
         * second dimension represents the actual data type (its size is equal
         * to @ref SceneIndexType size) and is guaranteed to be contiguous. Use
         * the templated overload below to get the indices in a concrete type.
         * @see @ref mutableFieldObject(UnsignedInt),
         *      @ref Corrade::Containers::StridedArrayView::isContiguous(),
         *      @ref sceneIndexTypeSize()
         */
        Containers::StridedArrayView2D<const char> fieldObject(UnsignedInt id) const; // TODO renmae to fieldObjectMapping

        /**
         * @brief Mutable object index data for given field
         * @m_since_latest
         *
         * Like @ref fieldObject(UnsignedInt), but returns a mutable view.
         * Expects that the scene is mutable.
         * @see @ref dataFlags()
         */
        Containers::StridedArrayView2D<char> mutableFieldObject(UnsignedInt id) const;

        /**
         * @brief Object indices for given field
         * @m_since_latest
         *
         * The @p id is expected to be smaller than @ref fieldCount() and @p T
         * is expected to correspond to @ref objectIndexType().
         * @see @ref mutableFieldObject(UnsignedInt)
         */
        template<class T> Containers::StridedArrayView1D<const T> fieldObject(UnsignedInt id) const;

        /**
         * @brief Mutable object indices for given field
         * @m_since_latest
         *
         * Like @ref fieldObject(UnsignedInt), but returns a mutable view.
         * Expects that the scene is mutable.
         * @see @ref dataFlags()
         */
        template<class T> Containers::StridedArrayView1D<const T> mutableFieldObject(UnsignedInt id) const;

        /**
         * @brief Object index data for given named field
         * @m_since_latest
         *
         * The @p name is expected to exist. The second dimension represents
         * the actual data type (its size is equal to @ref SceneIndexType size)
         * and is guaranteed to be contiguous. Use the templated overload below
         * to get the indices in a concrete type.
         * @see @ref hasField(), @ref fieldObject(UnsignedInt),
         *      @ref mutableFieldObject(SceneField),
         *      @ref Corrade::Containers::StridedArrayView::isContiguous()
         */
        Containers::StridedArrayView2D<const char> fieldObject(SceneField name) const;

        /**
         * @brief Mutable object index data for given named field
         * @m_since_latest
         *
         * Like @ref fieldObject(SceneField), but returns a mutable view.
         * Expects that the scene is mutable.
         * @see @ref dataFlags()
         */
        Containers::StridedArrayView2D<char> mutableFieldObject(SceneField name) const;

        /**
         * @brief Object indices for given named field
         * @m_since_latest
         *
         * The @p name is expected to exist and @p T is expected to correspond
         * to @ref objectIndexType().
         * @see @ref hasField(), @ref fieldObject(UnsignedInt),
         *      @ref mutableFieldObject(UnsignedInt)
         */
        template<class T> Containers::StridedArrayView1D<const T> fieldObject(SceneField name) const;

        /**
         * @brief Mutable object indices for given named field
         * @m_since_latest
         *
         * Like @ref fieldObject(SceneField), but returns a mutable view.
         * Expects that the scene is mutable.
         * @see @ref dataFlags()
         */
        template<class T> Containers::StridedArrayView1D<T> mutableFieldObject(SceneField name);

        // TODO: iterable version? could paper over the differences in index type and the case when the ID is implicit (or... fuck that also?); offset is no more so that doesn't need to be handled
        // TODO or iterable together with some field? hmmmm

        // TODO for all these if they are sparse or if an object is "inactive" what should this do? return less? have a separate bitflag? ughhhhh

        /**
         * @brief Object indices for given field as 32-bit integers
         * @m_since_latest
         *
         * Convenience alternative to the templated
         * @ref fieldObject(UnsignedInt). Converts the index array from
         * arbitrary underlying type and returns it in a newly-allocated array.
         * The @p id is expected to be smaller than @ref fieldCount().
         * @attention In the rare case when @ref objectIndexType() is
         *      @ref SceneIndexType::UnsignedLong and
         *      @ref objectCount() is larger than the max representable 32-bit
         *      value, this function can't be used, only an appropriately typed
         *      @ref fieldObject(UnsignedInt).
         * @see @ref fieldObjectInto(UnsignedInt)
         */
        Containers::Array<UnsignedInt> fieldObjectAsArray(UnsignedInt id) const;

        /**
         * @brief Object indices for given field as 32-bit integers into a pre-allocated view
         * @m_since_latest
         *
         * Like @ref fieldObjectAsArray(UnsignedInt), but puts the result into
         * @p destination instead of allocating a new array. Expects that
         * @p destination is sized to contain exactly all data.
         * @see @ref objectCount(UnsignedInt)
         */
        void fieldObjectInto(UnsignedInt id, Containers::StridedArrayView1D<UnsignedInt> destination) const;

        /**
         * @brief Object indices for given named field as 32-bit integers
         * @m_since_latest
         *
         * Convenience alternative to the templated
         * @ref fieldObject(SceneField). Converts the index array from
         * arbitrary underlying type and returns it in a newly-allocated array.
         * The @p id is expected to be smaller than @ref fieldCount().
         * @attention In the rare case when @ref objectIndexType() is
         *      @ref SceneIndexType::UnsignedLong and
         *      @ref objectCount() is larger than the max representable 32-bit
         *      value, this function can't be used, only an appropriately typed
         *      @ref fieldObject(SceneField).
         * @see @ref fieldObjectInto(SceneField)
         */
        Containers::Array<UnsignedInt> fieldObjectAsArray(SceneField name) const;

        /**
         * @brief Object indices for given named field as 32-bit integers into a pre-allocated view
         * @m_since_latest
         *
         * Like @ref fieldObjectAsArray(SceneField), but puts the result into
         * @p destination instead of allocating a new array. Expects that
         * @p destination is sized to contain exactly all data.
         * @see @ref objectCount(SceneField)
         */
        void fieldObjectInto(SceneField name, Containers::StridedArrayView1D<UnsignedInt> destination) const;

        /**
         * @brief Parent indices as 32-bit integers
         * @m_since_latest
         *
         * Convenience alternative to @ref field(SceneField) with
         * @ref SceneField::Parent as the argument. Converts the parent array
         * from an arbitrary underlying type and returns it in a
         * newly-allocated array.
         * @attention In the rare case when @ref fieldType() is
         *      @ref SceneIndexType::Long and @ref objectCount(SceneField) is
         *      larger than the max representable 32-bit value, this function
         *      can't be used, only an appropriately typed
         *      @ref field(SceneField).
         * @see @ref parentsInto()
         */
        Containers::Array<Int> parentsAsArray() const;

        /**
         * @brief Parent indices as 32-bit indices into a pre-allocated view
         * @m_since_latest
         *
         * Like @ref parentsAsArray(), but puts the result into @p destination
         * instead of allocating a new array. Expects that @p destination is
         * sized to contain exactly all data.
         * @see @ref objectCount(SceneField)
         */
        void parentsInto(Containers::StridedArrayView1D<Int> destination) const;

        /**
         * @brief 2D transformations as 3x3 float matrices
         * @m_since_latest
         *
         * Convenience alternative to @ref field(SceneField) with
         * @ref SceneField::Transformation as the argument, or, if not present,
         * to a matrix created out of the @ref SceneField::Translation,
         * @ref SceneField::Rotation and @ref SceneField::Scaling fields. The
         * fields are expected to have a type corresponding to 2D, otherwise
         * you're supposed to use @ref transformations3DAsArray(). Apart from
         * that restriction, the transformation is converted and composed from
         * an arbitrary underlying type and returned in a newly-allocated
         * array.
         * @see @ref transformations2DInto(), @ref hasField(),
         *      @ref fieldFormat()
         */
        Containers::Array<Matrix3> transformations2DAsArray() const;

        /**
         * @brief 2D transformations as 3x3 float matrices into a pre-allocated view
         * @m_since_latest
         *
         * Like @ref transformations2DAsArray(), but puts the result into
         * @p destination instead of allocating a new array. Expects that
         * @p destination is sized to contain exactly all data.
         * @see @ref objectCount(SceneField)
         */
        void transformations2DInto(Containers::StridedArrayView1D<Matrix3> destination) const;

        /**
         * @brief 3D transformations as 4x4 float matrices
         * @m_since_latest
         *
         * Convenience alternative to @ref field(SceneField) with
         * @ref SceneField::Transformation as the argument, or, if not present,
         * to a matrix created out of the @ref SceneField::Translation,
         * @ref SceneField::Rotation and @ref SceneField::Scaling fields. The
         * fields are expected to have a type corresponding to 3D, otherwise
         * you're supposed to use @ref transformations2DAsArray(). Apart from
         * that restriction, the transformation is converted and composed from
         * an arbitrary underlying type and returned in a newly-allocated
         * array.
         * @see @ref transformations3DInto(), @ref hasField(),
         *      @ref fieldFormat()
         */
        Containers::Array<Matrix4> transformations3DAsArray() const;

        /**
         * @brief 3D transformations as 4x4 float matrices into a pre-allocated view
         * @m_since_latest
         *
         * Like @ref transformations3DAsArray(), but puts the result into
         * @p destination instead of allocating a new array. Expects that
         * @p destination is sized to contain exactly all data.
         * @see @ref objectCount(SceneField)
         */
        void transformations3DInto(Containers::StridedArrayView1D<Matrix4> destination) const;

        /**
         * @brief Mesh IDs as 32-bit integers
         * @m_since_latest
         *
         * Convenience alternative to @ref field(SceneField) with
         * @ref SceneField::MeshId as the argument. Converts the mesh ID array
         * from an arbitrary underlying type and returns it in a
         * newly-allocated array.
         * @see @ref meshIdsInto()
         */
        Containers::Array<UnsignedInt> meshIdsAsArray() const;

        /**
         * @brief Mesh IDs as 32-bit integers into a pre-allocated view
         * @m_since_latest
         *
         * Like @ref meshIdsAsArray(), but puts the result into @p destination
         * instead of allocating a new array. Expects that @p destination is
         * sized to contain exactly all data.
         * @see @ref objectCount(SceneField)
         */
        void meshIdsInto(Containers::StridedArrayView1D<UnsignedInt> destination) const;

        /**
         * @brief Mesh material IDs as 32-bit integers
         * @m_since_latest
         *
         * Convenience alternative to @ref field(SceneField) with
         * @ref SceneField::MeshMaterialId as the argument. Converts the
         * material ID array from an arbitrary underlying type and returns it
         * in a newly-allocated array.
         * @see @ref meshIdsInto()
         */
        Containers::Array<UnsignedInt> meshMaterialIdsAsArray() const;

        /**
         * @brief Mesh material IDs as 32-bit integers into a pre-allocated view
         * @m_since_latest
         *
         * Like @ref materialIdsAsArray(), but puts the result into
         * @p destination instead of allocating a new array. Expects that
         * @p destination is sized to contain exactly all data.
         * @see @ref objectCount(SceneField)
         */
        void meshMaterialIdsInto(Containers::StridedArrayView1D<UnsignedInt> destination) const;

        /**
         * @brief Light IDs as 32-bit integers
         * @m_since_latest
         *
         * Convenience alternative to @ref field(SceneField) with
         * @ref SceneField::LightId as the argument. Converts the light ID
         * array from an arbitrary underlying type and returns it in a
         * newly-allocated array.
         * @see @ref meshIdsInto()
         */
        Containers::Array<UnsignedInt> lightIdsAsArray() const;

        /**
         * @brief Light IDs as 32-bit integers into a pre-allocated view
         * @m_since_latest
         *
         * Like @ref lightIdsAsArray(), but puts the result into @p destination
         * instead of allocating a new array. Expects that @p destination is
         * sized to contain exactly all data.
         * @see @ref objectCount(SceneField)
         */
        void lightIdsInto(Containers::StridedArrayView1D<UnsignedInt> destination) const;

        /**
         * @brief Camera IDs as 32-bit integers
         * @m_since_latest
         *
         * Convenience alternative to @ref field(SceneField) with
         * @ref SceneField::CameraId as the argument. Converts the camera ID
         * array from an arbitrary underlying type and returns it in a
         * newly-allocated array.
         * @see @ref meshIdsInto()
         */
        Containers::Array<UnsignedInt> cameraIdsAsArray() const;

        /**
         * @brief Camera IDs as 32-bit integers into a pre-allocated view
         * @m_since_latest
         *
         * Like @ref cameraIdsAsArray(), but puts the result into
         * @p destination instead of allocating a new array. Expects that
         * @p destination is sized to contain exactly all data.
         * @see @ref objectCount(SceneField)
         */
        void cameraIdsInto(Containers::StridedArrayView1D<UnsignedInt> destination) const;

        /**
         * @brief Skin IDs as 32-bit integers
         * @m_since_latest
         *
         * Convenience alternative to @ref field(SceneField) with
         * @ref SceneField::SkinId as the argument. Converts the skin ID array
         * from an arbitrary underlying type and returns it in a
         * newly-allocated array.
         * @see @ref meshIdsInto()
         */
        Containers::Array<UnsignedInt> skinIdsAsArray() const;

        /**
         * @brief Skin IDs as 32-bit integers into a pre-allocated view
         * @m_since_latest
         *
         * Like @ref skinIdsAsArray(), but puts the result into @p destination
         * instead of allocating a new array. Expects that @p destination is
         * sized to contain exactly all data.
         * @see @ref objectCount(SceneField)
         */
        void skinIdsInto(Containers::StridedArrayView1D<UnsignedInt> destination) const;

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Two-dimensional child objects
         * @m_deprecated_since_latest @ref TODO uhhhh
         */
        CORRADE_DEPRECATED() std::vector<UnsignedInt> children2D() const;

        /**
         * @brief Three-dimensional child objects
         * @m_deprecated_since_latest @ref TODO uhhhh
         */
        CORRADE_DEPRECATED() std::vector<UnsignedInt> children3D() const;
        #endif

        /**
         * @brief Importer-specific state
         *
         * See @ref AbstractImporter::importerState() for more information.
         */
        const void* importerState() const { return _importerState; }

    private:
        DataFlags _dataFlags;
        SceneIndexType _indexType;
        /* 2/6 bytes free */
        UnsignedLong _objectCount;
        const void* _importerState;
        Containers::Array<SceneFieldData> _fields;
        Containers::Array<char> _data;
};

namespace Implementation {

    template<class T> constexpr SceneIndexType sceneIndexTypeFor() {
        static_assert(sizeof(T) == 0, "unsupported index type");
        return {};
    }
    #ifndef DOXYGEN_GENERATING_OUTPUT
    #define _c(type) \
        template<> constexpr SceneIndexType sceneIndexTypeFor<type>() { return SceneIndexType::type; }
    _c(UnsignedByte)
    _c(UnsignedShort)
    _c(UnsignedInt)
    _c(UnsignedLong)
    #undef _c
    #endif

    template<class T> constexpr SceneFieldType sceneFieldTypeFor() {
        static_assert(sizeof(T) == 0, "unsupported field type");
        return {};
    }
    #ifndef DOXYGEN_GENERATING_OUTPUT
    #define _c(type) \
        template<> constexpr SceneFieldType sceneFieldTypeFor<type>() { return SceneFieldType::type; }
    /* Bool needs a special type */
    _c(Float)
    _c(Half)
    _c(Double)
    _c(UnsignedByte)
    _c(Byte)
    _c(UnsignedShort)
    _c(Short)
    _c(UnsignedInt)
    _c(Int)
    _c(UnsignedLong)
    _c(Long)
    _c(Vector2)
    _c(Vector2h)
    _c(Vector2d)
    _c(Vector2ub)
    _c(Vector2b)
    _c(Vector2us)
    _c(Vector2s)
    _c(Vector2ui)
    _c(Vector2i)
    _c(Vector3)
    _c(Vector3h)
    _c(Vector3d)
    _c(Vector3ub)
    _c(Vector3b)
    _c(Vector3us)
    _c(Vector3s)
    _c(Vector3ui)
    _c(Vector3i)
    _c(Vector4)
    _c(Vector4h)
    _c(Vector4d)
    _c(Vector4ub)
    _c(Vector4b)
    _c(Vector4us)
    _c(Vector4s)
    _c(Vector4ui)
    _c(Vector4i)
    _c(Matrix2x2)
    _c(Matrix2x2h)
    _c(Matrix2x2d)
    _c(Matrix2x3)
    _c(Matrix2x3h)
    _c(Matrix2x3d)
    _c(Matrix2x4)
    _c(Matrix2x4h)
    _c(Matrix2x4d)
    _c(Matrix3x2)
    _c(Matrix3x2h)
    _c(Matrix3x2d)
    _c(Matrix3x3)
    _c(Matrix3x3h)
    _c(Matrix3x3d)
    _c(Matrix3x4)
    _c(Matrix3x4h)
    _c(Matrix3x4d)
    _c(Matrix4x2)
    _c(Matrix4x2h)
    _c(Matrix4x2d)
    _c(Matrix4x3)
    _c(Matrix4x3h)
    _c(Matrix4x3d)
    _c(Matrix4x4)
    _c(Matrix4x4h)
    _c(Matrix4x4d)
    _c(Range1D)
    _c(Range1Dd)
    _c(Range1Di)
    _c(Range2D)
    _c(Range2Dd)
    _c(Range2Di)
    _c(Range3D)
    _c(Range3Dd)
    _c(Range3Di)
    _c(Complex)
    _c(Complexd)
    _c(DualComplex)
    _c(DualComplexd)
    _c(Quaternion)
    _c(Quaterniond)
    _c(DualQuaternion)
    _c(DualQuaterniond)
    _c(Deg)
    _c(Degh)
    _c(Degd)
    _c(Rad)
    _c(Radh)
    _c(Radd)
    #undef _c
    #endif
}

}}

#endif
