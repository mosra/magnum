#ifndef Magnum_Trade_SceneData_h
#define Magnum_Trade_SceneData_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
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
 * @brief Class @ref Magnum::Trade::SceneData, @ref Magnum::Trade::SceneFieldData, enum @ref Magnum::Trade::SceneMappingType, @ref Magnum::Trade::SceneField, @ref Magnum::Trade::SceneFieldType, @ref Magnum::Trade::SceneFieldFlag, enum set @ref Magnum::Trade::SceneFieldFlags, function @ref Magnum::Trade::sceneMappingTypeSize(), @ref Magnum::Trade::sceneMappingTypeAlignment(), @ref Magnum::Trade::sceneFieldTypeSize(), @ref Magnum::Trade::sceneFieldTypeAlignment(), @ref Magnum::Trade::isSceneFieldCustom(), @ref Magnum::Trade::sceneFieldCustom()
 */

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Utility/Macros.h> /* CORRADE_UNUSED */

#include "Magnum/Trade/Data.h"
#include "Magnum/Trade/Trade.h"
#include "Magnum/Trade/visibility.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Utility/StlForwardVector.h>
#endif

namespace Magnum { namespace Trade {

/**
@brief Scene object mapping type
@m_since_latest

Type used for mapping fields to corresponding objects. Unlike
@ref SceneFieldType that is different for different fields, the object mapping
type is the same for all fields, and is guaranteed to be large enough to fit
@ref SceneData::mappingBound() objects.
@see @ref SceneData::mappingType(), @ref sceneMappingTypeSize(),
    @ref sceneMappingTypeAlignment()
*/
enum class SceneMappingType: UnsignedByte {
    /* Zero used for an invalid value */

    UnsignedByte = 1,   /**< @relativeref{Magnum,UnsignedByte} */
    UnsignedShort,      /**< @relativeref{Magnum,UnsignedShort} */
    UnsignedInt,        /**< @relativeref{Magnum,UnsignedInt} */
    UnsignedLong        /**< @relativeref{Magnum,UnsignedLong} */

    /* Bits 3-6 used to store `SceneFieldType::String* << 3` in
       SceneFieldData::_mappingTypeStringType */
};

namespace Implementation {

enum: UnsignedByte {
    SceneMappingTypeMask = 0x07, /* covers SceneMappingType values */
    SceneMappingStringTypeMask = 0xf8  /* covers `SceneFieldType::String* << 3` */
};

}

/**
@debugoperatorenum{SceneMappingType}
@m_since_latest
*/
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, SceneMappingType value);

/**
@brief Size of given scene object mapping type
@m_since_latest

@see @ref sceneMappingTypeAlignment()
*/
MAGNUM_TRADE_EXPORT UnsignedInt sceneMappingTypeSize(SceneMappingType type);

/**
@brief Alignment of given scene object mapping type
@m_since_latest

Returns the same value as @ref sceneMappingTypeSize().
*/
MAGNUM_TRADE_EXPORT UnsignedInt sceneMappingTypeAlignment(SceneMappingType type);

namespace Implementation {
    enum: UnsignedInt { SceneFieldCustom = 0x80000000u };
}

/**
@brief Scene field name
@m_since_latest

See @ref SceneData for more information.

Apart from the builtin field names it's possible to have custom ones, which use
the upper half of the enum range. While it's unlikely to have billions of
custom fields, the enum intentionally reserves a full 31-bit range to avoid the
need to remap field identifiers coming from 3rd party ECS frameworks, for
example.

Custom fields are detected via @ref isSceneFieldCustom() and can be converted
to and from a numeric identifier using @ref sceneFieldCustom(SceneField) and
@ref sceneFieldCustom(UnsignedInt). Unlike the builtin ones, these can be of
any type. An importer that exposes custom fields then may also provide a string
mapping using @ref AbstractImporter::sceneFieldForName() and
@ref AbstractImporter::sceneFieldName(); conversely a scene converter
supporting custom scene fields can have the string mapping specified via
@ref AbstractSceneConverter::setSceneFieldName(). See documentation of a
particular importer and scene converter for details.
@see @ref SceneFieldData, @ref SceneFieldType
*/
enum class SceneField: UnsignedInt {
    /* Zero used for an invalid value */

    /**
     * Parent object. Type is usually @ref SceneFieldType::Int, but can be also
     * any of @relativeref{SceneFieldType,Byte},
     * @relativeref{SceneFieldType,Short} or a
     * @relativeref{SceneFieldType,Long}. A value of @cpp -1 @ce means there's
     * no parent. An object should have only one parent, altough this isn't
     * enforced in any way, and which of the duplicate fields gets used is not
     * defined.
     * @see @ref SceneData::parentsAsArray(), @ref SceneData::parentFor(),
     *      @ref SceneData::childrenFor()
     */
    Parent = 1,

    /**
     * Transformation. Type is usually @ref SceneFieldType::Matrix3x3 for 2D
     * and @ref SceneFieldType::Matrix4x4 for 3D, but can be also any of
     * @relativeref{SceneFieldType,Matrix3x3d},
     * @relativeref{SceneFieldType,Matrix3x2} or
     * @relativeref{SceneFieldType,Matrix3x2d} (with the bottom row implicitly
     * assumed to be @f$ \begin{pmatrix} 0 & 0 & 1 \end{pmatrix} @f$),
     * @relativeref{SceneFieldType,DualComplex} or
     * @relativeref{SceneFieldType,DualComplexd} for 2D and
     * @relativeref{SceneFieldType,Matrix4x4d},
     * @relativeref{SceneFieldType,Matrix4x3} or
     * @relativeref{SceneFieldType,Matrix4x3d} (with the bottom row implicitly
     * assumed to be @f$ \begin{pmatrix} 0 & 0 & 0 & 1 \end{pmatrix} @f$),
     * @relativeref{SceneFieldType,DualQuaternion} or
     * @relativeref{SceneFieldType,DualQuaterniond} for 3D. An object should
     * have only one transformation, altough this isn't enforced in any way,
     * and which of the duplicate fields gets used is not defined.
     *
     * The transformation can be also represented by separate
     * @ref SceneField::Translation, @ref SceneField::Rotation and
     * @ref SceneField::Scaling fields. All present transformation-related
     * fields are expected to have the same dimensionality --- either all 2D or
     * all 3D. If both @ref SceneField::Transformation and TRS fields are
     * specified, it's expected that all objects that have TRS fields have a
     * combined transformation field as well, and
     * @ref SceneData::transformations2DAsArray() /
     * @ref SceneData::transformations3DAsArray() then takes into account only
     * the combined transformation field. TRS fields can however be specified
     * only for a subset of transformed objects, useful for example when only
     * certain objects have these properties animated.
     * @see @ref SceneData::is2D(), @ref SceneData::is3D(),
     *      @ref SceneData::transformations2DAsArray(),
     *      @ref SceneData::transformations3DAsArray(),
     *      @ref SceneData::transformation2DFor(),
     *      @ref SceneData::transformation3DFor()
     */
    Transformation,

    /**
     * Translation. Type is usually @ref SceneFieldType::Vector2 for 2D and
     * @ref SceneFieldType::Vector3 for 3D, but can be also any of
     * @relativeref{SceneFieldType,Vector2d} for 2D and
     * @relativeref{SceneFieldType,Vector3d} for 3D. An object should
     * have only one translation, altough this isn't enforced in any way,
     * and which of the duplicate fields gets used is not defined.
     *
     * The translation field usually is (but doesn't have to be) complemented
     * by a @ref SceneField::Rotation and @ref SceneField::Scaling, which, if
     * present, are expected to all share the same object mapping view and have
     * the same dimensionality, either all 2D or all 3D. The TRS
     * components can either completely replace @ref SceneField::Transformation
     * or be provided just for a subset of it --- see its documentation for
     * details.
     * @see @ref SceneData::is2D(), @ref SceneData::is3D(),
     *      @ref SceneData::transformations2DAsArray(),
     *      @ref SceneData::transformations3DAsArray(),
     *      @ref SceneData::transformation2DFor(),
     *      @ref SceneData::transformation3DFor(),
     *      @ref SceneData::translationsRotationsScalings2DAsArray(),
     *      @ref SceneData::translationsRotationsScalings3DAsArray(),
     *      @ref SceneData::translationRotationScaling2DFor(),
     *      @ref SceneData::translationRotationScaling3DFor()
     */
    Translation,

    /**
     * Rotation. Type is usually @ref SceneFieldType::Complex for 2D and
     * @ref SceneFieldType::Quaternion for 3D, but can be also any of
     * @relativeref{SceneFieldType,Complexd} for 2D and
     * @relativeref{SceneFieldType,Quaterniond} for 3D. An object should have
     * only one rotation, altough this isn't enforced in any way, and which of
     * the duplicate fields gets used is not defined.
     *
     * The rotation field usually is (but doesn't have to be) complemented by a
     * @ref SceneField::Translation and @ref SceneField::Scaling, which, if
     * present, are expected to all share the same object mapping view and have
     * the same dimensionality, either all 2D or all 3D. The TRS
     * components can either completely replace @ref SceneField::Transformation
     * or be provided just for a subset of it --- see its documentation for
     * details.
     * @see @ref SceneData::is2D(), @ref SceneData::is3D(),
     *      @ref SceneData::transformations2DAsArray(),
     *      @ref SceneData::transformations3DAsArray(),
     *      @ref SceneData::transformation2DFor(),
     *      @ref SceneData::transformation3DFor(),
     *      @ref SceneData::translationsRotationsScalings2DAsArray(),
     *      @ref SceneData::translationsRotationsScalings3DAsArray(),
     *      @ref SceneData::translationRotationScaling2DFor(),
     *      @ref SceneData::translationRotationScaling3DFor()
     */
    Rotation,

    /**
     * Scaling. Type is usually @ref SceneFieldType::Vector2 for 2D and
     * @ref SceneFieldType::Vector3 for 3D, but can be also any of
     * @relativeref{SceneFieldType,Vector2d} for 2D and
     * @relativeref{SceneFieldType,Vector3d} for 3D. An object should
     * have only one scaling, altough this isn't enforced in any way, and which
     * of the duplicate fields gets used is not defined.
     *
     * The scaling field usually is (but doesn't have to be) complemented by a
     * @ref SceneField::Translation and @ref SceneField::Rotation, which, if
     * present, are expected to all share the same object mapping view and have
     * the same dimensionality, either all 2D or all 3D. The TRS
     * components can either completely replace @ref SceneField::Transformation
     * or be provided just for a subset of it --- see its documentation for
     * details.
     * @see @ref SceneData::is2D(), @ref SceneData::is3D(),
     *      @ref SceneData::transformations2DAsArray(),
     *      @ref SceneData::transformations3DAsArray(),
     *      @ref SceneData::transformation2DFor(),
     *      @ref SceneData::transformation3DFor(),
     *      @ref SceneData::translationsRotationsScalings2DAsArray(),
     *      @ref SceneData::translationsRotationsScalings3DAsArray(),
     *      @ref SceneData::translationRotationScaling2DFor(),
     *      @ref SceneData::translationRotationScaling3DFor()
     */
    Scaling,

    /**
     * ID of a mesh associated with this object, corresponding to the ID passed
     * to @ref AbstractImporter::mesh(). Type is usually
     * @ref SceneFieldType::UnsignedInt, but can be also any of
     * @relativeref{SceneFieldType,UnsignedByte} or
     * @relativeref{SceneFieldType,UnsignedShort}. An object can have multiple
     * meshes associated.
     *
     * Usually complemented with a @ref SceneField::MeshMaterial, although not
     * required. If present, both should share the same object mapping view.
     * Objects with multiple meshes then have the Nth mesh associated with the
     * Nth material.
     * @see @ref SceneData::meshesMaterialsAsArray(),
     *      @ref SceneData::meshesMaterialsFor()
     */
    Mesh,

    /**
     * ID of a material for a @ref SceneField::Mesh, corresponding to the ID
     * passed to @ref AbstractImporter::material() or @cpp -1 @ce if the mesh
     * has no material associated. Type is usually @ref SceneFieldType::Int,
     * but can be also any of @relativeref{SceneFieldType,Byte} or
     * @relativeref{SceneFieldType,Short}. Expected to share the
     * object mapping view with @ref SceneField::Mesh.
     * @see @ref SceneData::meshesMaterialsAsArray(),
     *      @ref SceneData::meshesMaterialsFor()
     */
    MeshMaterial,

    /**
     * ID of a light associated with this object, corresponding to the ID
     * passed to @ref AbstractImporter::light(). Type is usually
     * @ref SceneFieldType::UnsignedInt, but can be also any of
     * @relativeref{SceneFieldType,UnsignedByte} or
     * @relativeref{SceneFieldType,UnsignedShort}. An object can have multiple
     * lights associated.
     * @see @ref SceneData::lightsAsArray(), @ref SceneData::lightsFor()
     */
    Light,

    /**
     * ID of a camera associated with this object, corresponding to the ID
     * passed to @ref AbstractImporter::camera(). Type is usually
     * @ref SceneFieldType::UnsignedInt, but can be also any of
     * @relativeref{SceneFieldType,UnsignedByte} or
     * @relativeref{SceneFieldType,UnsignedShort}. An object can have multiple
     * cameras associated.
     * @see @ref SceneData::camerasAsArray(), @ref SceneData::camerasFor()
     */
    Camera,

    /**
     * ID of a skin associated with this object, corresponding to the ID
     * passed to @ref AbstractImporter::skin2D() or
     * @ref AbstractImporter::skin3D(), depending on whether the scene has a 2D
     * or 3D transformation. Type is usually @ref SceneFieldType::UnsignedInt,
     * but can be also any of @relativeref{SceneFieldType,UnsignedByte} or
     * @relativeref{SceneFieldType,UnsignedShort}. An object can have multiple
     * skins associated.
     * @see @ref SceneData::is2D(), @ref SceneData::is3D(),
     *      @ref SceneData::skinsAsArray(), @ref SceneData::skinsFor()
     */
    Skin,

    /**
     * Importer state for given object, per-object counterpart to
     * scene-specific @ref SceneData::importerState(). Type is usually
     * @ref SceneFieldType::Pointer but can be also
     * @ref SceneFieldType::MutablePointer. An object should have only one
     * importer state, altough this isn't enforced in any way, and which of the
     * duplicate fields gets used is not defined.
     * @see @ref SceneData::importerStateAsArray(),
     *      @ref SceneData::importerStateFor()
     */
    ImporterState
};

/**
@debugoperatorenum{SceneField}
@m_since_latest
*/
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, SceneField value);

/**
@brief Whether a scene field is custom
@m_since_latest

Returns @cpp true @ce if @p name has a value in the upper 31 bits of the enum
range, @cpp false @ce otherwise.
@see @ref sceneFieldCustom(UnsignedInt), @ref sceneFieldCustom(SceneField),
    @ref AbstractImporter::sceneFieldName()
*/
constexpr bool isSceneFieldCustom(SceneField name) {
    return UnsignedInt(name) >= Implementation::SceneFieldCustom;
}

/**
@brief Create a custom scene field
@m_since_latest

Returns a custom scene field with index @p id. The index is expected to fit
into 31 bits. Use @ref sceneFieldCustom(SceneField) to get the index back.
*/
/* Constexpr so it's usable for creating compile-time SceneFieldData
   instances */
constexpr SceneField sceneFieldCustom(UnsignedInt id) {
    return CORRADE_CONSTEXPR_ASSERT(id < Implementation::SceneFieldCustom,
        "Trade::sceneFieldCustom(): index" << id << "too large"),
        SceneField(Implementation::SceneFieldCustom + id);
}

/**
@brief Get index of a custom scene field
@m_since_latest

Inverse to @ref sceneFieldCustom(UnsignedInt). Expects that the field is
custom.
@see @ref isSceneFieldCustom(), @ref AbstractImporter::sceneFieldName()
*/
constexpr UnsignedInt sceneFieldCustom(SceneField name) {
    return CORRADE_CONSTEXPR_ASSERT(isSceneFieldCustom(name),
        "Trade::sceneFieldCustom():" << name << "is not custom"),
        UnsignedInt(name) - Implementation::SceneFieldCustom;
}

/**
@brief Scene field type
@m_since_latest

A type in which a @ref SceneField is stored. See @ref SceneData for more
information.
@see @ref SceneFieldData, @ref sceneFieldTypeSize(),
    @ref sceneFieldTypeAlignment()
*/
enum class SceneFieldType: UnsignedShort {
    /* Zero used for an invalid value */

    /* 1-12 used by String* types defined below as they need to fit into 4 bits
       to be stored in a single byte together with SceneMappingType */

    /**
     * A single bit or an array of bits. Use @ref SceneData::fieldBits() or
     * @relativeref{SceneData,fieldBitArrays()} for convenient access.
     */
    Bit = 13,

    Float,          /**< @relativeref{Magnum,Float} */
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

    /**
     * @cpp const void* @ce, type is not preserved. For convenience it's
     * possible to retrieve the value by calling
     * @ref SceneData::field() "field<const T*>()" with an arbitrary `T` but
     * the user has to ensure the type is correct.
     */
    Pointer,

    /**
     * @cpp void* @ce, type is not preserved. For convenience it's possible to
     * retrieve the value by calling @ref SceneData::field() "field<T*>()" with
     * an arbitrary `T` but the user has to ensure the type is correct.
     */
    MutablePointer,

    /* String types defined at the end because of exquisite complexity */

    /**
     * 32-bit string offsets with implicit sizes. Use
     * @ref SceneData::fieldStrings() for convenient access.
     *
     * Internally, the first string starts at @ref SceneData::fieldStringData(),
     * second string starts at @relativeref{SceneData,fieldStringData()} plus
     * @ref SceneData::field() "field<UnsignedInt>()[0]", etc. String sizes are
     * implicitly the distance between two successive offsets or
     * @cpp field<UnsignedInt>()[0] @ce in case of the first string; if
     * @ref SceneFieldFlag::NullTerminatedString is set the distance includes
     * the null terminator.
     *
     * The `StringOffset*` types are useful mainly for cases where each string
     * is unique, for strings with many duplicates
     * @ref SceneFieldType::StringRange32 "SceneFieldType::StringRange*" or
     * @ref SceneFieldType::StringRangeNullTerminated32 "StringRangeNullTerminated32*"
     * may be a more space-efficient option.
     */
    StringOffset32 = 3,

    /**
     * 8-bit string offsets with implicit sizes. Use
     * @ref SceneData::fieldStrings() for convenient access.
     *
     * The internal layout is similar to @ref SceneFieldType::StringOffset32
     * except that @relativeref{Magnum,UnsignedByte} is used as the type, see
     * its documentation for more information.
     */
    StringOffset8 = 1,

    /**
     * 16-bit string offsets with implicit sizes. Use
     * @ref SceneData::fieldStrings() for convenient access.
     *
     * The internal layout is similar to @ref SceneFieldType::StringOffset32
     * except that @relativeref{Magnum,UnsignedShort} is used as the type, see
     * its documentation for more information.
     */
    StringOffset16 = 2,

    /**
     * 64-bit string offsets with implicit sizes. Use
     * @ref SceneData::fieldStrings() for convenient access.
     *
     * The internal layout is similar to @ref SceneFieldType::StringOffset32
     * except that @relativeref{Magnum,UnsignedLong} is used as the type, see
     * its documentation for more information.
     */
    StringOffset64 = 4,

    /**
     * @relativeref{Corrade,Containers::Pair} of 32-bit string offsets and
     * sizes. Use @ref SceneData::fieldStrings() for convenient access.
     *
     * Internally, string `i` starts at @ref SceneData::fieldStringData() plus
     * @ref SceneData::field() "field<Containers::Pair<UnsignedInt, UnsignedInt>>()[i].first()"
     * and has a size of @cpp field<Containers::Pair<UnsignedInt, UnsignedInt>>()[i].second() @ce.
     *
     * The main use case for `StringRange*` types is to be able to reference
     * the same string from multiple field entries without having to duplicate
     * it. For strings without duplicates
     * @ref SceneFieldType::StringOffset32 "SceneFieldType::StringOffset*" may
     * be a more space-efficient option, as the size is implicit.
     * Alternatively, @ref SceneFieldType::StringRangeNullTerminated32 "StringRangeNullTerminated32*"
     * has the same space requirements as
     * @ref SceneFieldType::StringOffset32 "StringOffset*" with
     * @ref SceneFieldFlag::NullTerminatedString set and allows deduplication,
     * however at the cost of a @ref std::strlen() call on every access.
     */
    StringRange32 = 7,

    /**
     * @relativeref{Corrade,Containers::Pair} of 8-bit string offsets and
     * sizes. Use @ref SceneData::fieldStrings() for convenient access.
     *
     * The internal layout is similar to @ref SceneFieldType::StringRange32
     * except that @relativeref{Magnum,UnsignedByte} is used for the pair
     * types, see its documentation for more information.
     */
    StringRange8 = 5,

    /**
     * @relativeref{Corrade,Containers::Pair} of 16-bit string offsets and
     * sizes. Use @ref SceneData::fieldStrings() for convenient access.
     *
     * The internal layout is similar to @ref SceneFieldType::StringRange32
     * except that @relativeref{Magnum,UnsignedShort} is used for the pair
     * types, see its documentation for more information.
     */
    StringRange16 = 6,

    /**
     * @relativeref{Corrade,Containers::Pair} of 64-bit string offsets and
     * sizes. Use @ref SceneData::fieldStrings() for convenient access.
     *
     * The internal layout is similar to @ref SceneFieldType::StringRange32
     * except that @relativeref{Magnum,UnsignedLong} is used for the pair
     * types, see its documentation for more information.
     */
    StringRange64 = 8,

    /**
     * 32-bit offsets to null-terminated strings. Use
     * @ref SceneData::fieldStrings() for convenient access.
     *
     * Compared to @ref SceneFieldType::StringRange32 stores just the offset,
     * the size is calculated on-the-fly with @ref std::strlen().
     *
     * Internally, string `i` starts at @ref SceneData::fieldStringData() plus
     * @ref SceneData::field() "field<UnsignedInt>()[i]", size is implicitly
     * until the first @cpp '\0' @ce byte. See
     * @ref SceneFieldType::StringRange32 for use case recommendations.
     */
    StringRangeNullTerminated32 = 11,

    /**
     * 8-bit offsets to null-terminated strings. Use
     * @ref SceneData::fieldStrings() for convenient access.
     *
     * The internal layout is similar to
     * @ref SceneFieldType::StringRangeNullTerminated32 except that
     * @relativeref{Magnum,UnsignedByte} is used as the type, see its
     * documentation for more information.
     */
    StringRangeNullTerminated8 = 9,

    /**
     * 16-bit offsets to null-terminated strings. Use
     * @ref SceneData::fieldStrings() for convenient access.
     *
     * The internal layout is similar to
     * @ref SceneFieldType::StringRangeNullTerminated32 except that
     * @relativeref{Magnum,UnsignedShort} is used as the type, see its
     * documentation for more information.
     */
    StringRangeNullTerminated16 = 10,

    /**
     * 32-bit offsets to null-terminated strings. Use
     * @ref SceneData::fieldStrings() for convenient access.
     *
     * The internal layout is similar to
     * @ref SceneFieldType::StringRangeNullTerminated32 except that
     * @relativeref{Magnum,UnsignedLong} is used as the type, see its
     * documentation for more information.
     */
    StringRangeNullTerminated64 = 12,
};

/**
@debugoperatorenum{SceneFieldType}
@m_since_latest
*/
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, SceneFieldType value);

/**
@brief Size of given scene field type
@m_since_latest

Expects that @p type isn't @ref SceneFieldType::Bit, for which the size isn't
representable in bytes.
@see @ref sceneFieldTypeAlignment()
*/
MAGNUM_TRADE_EXPORT UnsignedInt sceneFieldTypeSize(SceneFieldType type);

/**
@brief Alignment of given scene field type
@m_since_latest

Expects that @p type isn't @ref SceneFieldType::Bit, for which the alignment
isn't representable in bytes.
@see @ref sceneFieldTypeSize()
*/
MAGNUM_TRADE_EXPORT UnsignedInt sceneFieldTypeAlignment(SceneFieldType type);

/**
@brief Scene field flag
@m_since_latest

@see @ref SceneFieldFlags, @ref SceneFieldData, @ref SceneFieldData::flags(),
    @ref SceneData::fieldFlags()
*/
enum class SceneFieldFlag: UnsignedByte {
    /**
     * The field is offset-only, i.e. doesn't contain the data views directly
     * but referes to unspecified external data. Set implicitly by
     * the @ref SceneFieldData::SceneFieldData(SceneField, std::size_t, SceneMappingType, std::size_t, std::ptrdiff_t, SceneFieldType, std::size_t, std::ptrdiff_t, UnsignedShort, SceneFieldFlags)
     * constructor, can't be used for any other constructor.
     * @see @ref SceneFieldData::mappingData(Containers::ArrayView<const void>) const,
     *      @ref SceneFieldData::fieldData(Containers::ArrayView<const void>) const
     */
    OffsetOnly = 1 << 0,

    /**
     * The field has an ordered object mapping, i.e. a monotonically increasing
     * sequence. Object IDs in fields marked with this flag can be looked up
     * with an @f$ \mathcal{O}(\log{} n) @f$ complexity, gaps and duplicates
     * are possible.
     *
     * Note that validity of the object mapping data isn't checked in any way
     * and if the data doesn't correspond to rules of the flag, queries such
     * as @ref SceneData::findFieldObjectOffset() may return a wrong value.
     *
     * If a field has neither this nor the @ref SceneFieldFlag::ImplicitMapping
     * flag, it's assumed to be unordered, with an
     * @f$ \mathcal{O}(n) @f$ lookup complexity.
     */
    OrderedMapping = 1 << 1,

    /**
     * The field has an implicit object mapping, i.e. a contiguous sequence
     * from 0 up to size of the field. A superset of
     * @ref SceneFieldFlag::OrderedMapping. Object IDs in fields marked with
     * this flag can be looked up with an @f$ \mathcal{O}(1) @f$ complexity,
     * but the field is restricted to exactly one value for each object.
     *
     * Note that validity of the object mapping data isn't checked in any way
     * and if the data doesn't correspond to rules of the flag, queries such
     * as @ref SceneData::findFieldObjectOffset() may return a wrong value.
     *
     * If a field has neither this nor the @ref SceneFieldFlag::OrderedMapping
     * flag, it's assumed to be unordered, with an
     * @f$ \mathcal{O}(n) @f$ lookup complexity.
     */
    ImplicitMapping = (1 << 2)|OrderedMapping,

    /**
     * The field may have multiple entries for the same object. Meant to be
     * used as a hint to distinguish between fields that are expected to have
     * at most one entry for an object and fields that can have multiple
     * entries for an object but sometimes have just one.
     *
     * Note that presence of this flag isn't enforced in any way, fields
     * without this flag may still have multiple entries for the same object.
     *
     * Can't be set for @ref SceneField::Parent,
     * @relativeref{SceneField,Transformation},
     * @relativeref{SceneField,Translation}, @relativeref{SceneField,Rotation}
     * or @relativeref{SceneField,Scaling}; however mapping uniqueness for
     * those fields isn't enforced in any way either.
     */
    MultiEntry = 1 << 4,

    /**
     * The string field is null-terminated, i.e. string views returned from
     * @ref SceneData::fieldStrings() will always have
     * @relativeref{Corrade,Containers::StringViewFlag::NullTerminated} set.
     * Internally it means that the distance between successive
     * @ref SceneFieldType::StringOffset32 "SceneFieldType::StringOffset*"
     * entries includes the null terminator;
     * @ref SceneFieldType::StringRange32 "SceneFieldType::StringRange*" size
     * is excluding the null terminator but assumes it's present right after;
     * for @ref SceneFieldType::StringRangeNullTerminated32 "SceneFieldType,StringRangeNullTerminated*"
     * it's set implicitly as that's the default behavior.
     *
     * Can only be set for string @ref SceneFieldType.
     */
    NullTerminatedString = 1 << 3
};

/**
@debugoperatorenum{SceneFieldFlag}
@m_since_latest
*/
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, SceneFieldFlag value);

/**
@brief Scene field flags
@m_since_latest

@see @ref SceneFieldData::flags(), @ref SceneData::fieldFlags()
*/
typedef Containers::EnumSet<SceneFieldFlag> SceneFieldFlags;

CORRADE_ENUMSET_OPERATORS(SceneFieldFlags)

/**
@debugoperatorenum{SceneFieldFlags}
@m_since_latest
*/
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, SceneFieldFlags value);

/**
@brief Scene field data
@m_since_latest

Convenience type for populating @ref SceneData, see
@ref Trade-SceneData-populating "its documentation" for an introduction.
Additionally usable in various @ref SceneTools algorithms such as
@ref SceneTools::combineFields(Trade::SceneMappingType, UnsignedLong, Containers::ArrayView<const Trade::SceneFieldData>).

@section Trade-SceneFieldData-usage Usage

The most straightforward usage is constructing an instance from a
@ref SceneField and a strided view for the field data and object mapping. The
@ref SceneMappingType and @ref SceneFieldType get inferred from the view types:

@snippet Trade.cpp SceneFieldData-usage

Alternatively, you can pass typeless @cpp const void @ce or 2D views and supply
@ref SceneMappingType and @ref SceneFieldType explicitly.

@subsection Trade-SceneFieldData-usage-offset-only Offset-only field data

If the actual field / mapping data location is not known yet, the instance can
be created as "offset-only" using @ref SceneFieldData(SceneField, std::size_t, SceneMappingType, std::size_t, std::ptrdiff_t, SceneFieldType, std::size_t, std::ptrdiff_t, UnsignedShort, SceneFieldFlags)
and related constructor overloads, meaning the actual views get created only
later when passed to a @ref SceneData instance with a concrete data array. This
is useful mainly to avoid pointer patching during data serialization, but also
for example when the data layout is static (and thus can be defined at compile
time), but the actual data is allocated / populated at runtime.

@snippet Trade.cpp SceneFieldData-usage-offset-only

See @ref Trade-SceneData-populating-non-owned "the corresponding SceneData documentation"
for a complete usage example. Offset-only fields are marked with
@ref SceneFieldFlag::OffsetOnly in @ref flags(). Note that @ref SceneTools
algorithms generally don't accept offset-only @ref SceneFieldData instances
except when passed through a @ref SceneData, as for a standalone offset-only
@ref SceneFieldData it's impossible to know what data it points to.

@subsection Trade-SceneFieldData-usage-object-mapping Ordered and implicit object mapping

If you can guarantee the object mapping field is monotonically non-decreasing,
it's recommended to annotate it with @ref SceneFieldFlag::OrderedMapping. This
makes certain convenience APIs such as @ref SceneData::findFieldObjectOffset()
or e.g. @relativeref{SceneData,transformation3DFor()} perform the lookup in
@f$ \mathcal{O}(\log{} n) @f$ instead of @f$ \mathcal{O}(n) @f$. Data consuming
algorithms on the application side can then also adapt based on what flags are
present in @ref SceneData::fieldFlags().

In some cases the object mapping is even implicit, i.e. the first entry of the
field specifying data for object @cpp 0 @ce, second entry for object
@cpp 1 @ce, third for object @cpp 2 @ce and so on. You can annotate such fields
with @ref SceneFieldFlag::ImplicitMapping, which is a superset of
@relativeref{SceneFieldFlag,OrderedMapping}.

@subsection Trade-SceneFieldData-usage-bits Bit fields

Bit fields have dedicated constructors taking a
@relativeref{Corrade,Containers::StridedBitArrayView1D} or
@relativeref{Corrade,Containers::StridedBitArrayView2D}, and because the type
is always @ref SceneFieldType::Bit, it's omitted. For offset-only bit fields
there's a @ref SceneFieldData(SceneField, std::size_t, SceneMappingType, std::size_t, std::ptrdiff_t, std::size_t, std::size_t, std::ptrdiff_t, UnsignedShort, SceneFieldFlags)
constructor that omits @ref SceneFieldType as well, but contains an additional
bit offset parameter.

@subsection Trade-SceneFieldData-usage-strings String fields

String fields have to be constructed using dedicated constructors that
additionally take a @cpp const char* @ce base string pointer, and because a
particular type can correspond to more than one @ref SceneFieldType (such as
@ref SceneFieldType::StringOffset32 and
@ref SceneFieldType::StringRangeNullTerminated32 being both represented with an
@relativeref{Magnum,UnsignedInt}), the type has to be specified explicitly:

@snippet Trade.cpp SceneFieldData-usage-strings

Offset-only constructors have it similar, containing an extra base string
offset. Due to packing in the internal layout, string fields can't be arrays.
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
        constexpr explicit SceneFieldData() noexcept: _size{}, _name{}, _flags{}, _mappingTypeStringType{}, _mappingStride{}, _mappingData{}, _field{}, _fieldData{} {}

        /**
         * @brief Construct from type-erased views
         * @param name              Field name
         * @param mappingType       Object mapping type
         * @param mappingData       Object mapping data
         * @param fieldType         Field type. @ref SceneFieldType::Bit and
         *      `SceneFieldType::String*` values are not allowed here.
         * @param fieldData         Field data
         * @param fieldArraySize    Field array size. Use @cpp 0 @ce for
         *      non-array fields.
         * @param flags             Field flags.
         *      @ref SceneFieldFlag::OffsetOnly and
         *      @ref SceneFieldFlag::NullTerminatedString is not allowed here.
         *
         * Expects that @p mappingData and @p fieldData have the same size; and
         * for builtin fields that @p fieldType corresponds to @p name and
         * @p fieldArraySize is zero.
         */
        constexpr explicit SceneFieldData(SceneField name, SceneMappingType mappingType, const Containers::StridedArrayView1D<const void>& mappingData, SceneFieldType fieldType, const Containers::StridedArrayView1D<const void>& fieldData, UnsignedShort fieldArraySize = 0, SceneFieldFlags flags = {}) noexcept;

        /** @overload */
        constexpr explicit SceneFieldData(SceneField name, SceneMappingType mappingType, const Containers::StridedArrayView1D<const void>& mappingData, SceneFieldType fieldType, const Containers::StridedArrayView1D<const void>& fieldData, SceneFieldFlags flags) noexcept: SceneFieldData{name, mappingType, mappingData, fieldType, fieldData, 0, flags} {}

        /**
         * @brief Construct from 2D type-erased views
         * @param name              Field name
         * @param mappingData       Object mapping data
         * @param fieldType         Field type. @ref SceneFieldType::Bit and
         *      `SceneFieldType::String*` values are not allowed here.
         * @param fieldData         Field data
         * @param fieldArraySize    Field array size. Use @cpp 0 @ce for
         *      non-array fields.
         * @param flags             Field flags.
         *      @ref SceneFieldFlag::OffsetOnly and
         *      @ref SceneFieldFlag::NullTerminatedString is not allowed here.
         *
         * Expects that @p mappingData and @p fieldData have the same size in
         * the first dimension; that the second dimension of @p mappingData is
         * contiguous and its size is either 1, 2, 4 or 8, corresponding to one
         * of the @ref SceneMappingType values; that the second dimension of
         * @p fieldData is contiguous and its size matches @p fieldType and
         * @p fieldArraySize; and that for builtin fields @p fieldType
         * corresponds to @p name and @p fieldArraySize is zero.
         */
        explicit SceneFieldData(SceneField name, const Containers::StridedArrayView2D<const char>& mappingData, SceneFieldType fieldType, const Containers::StridedArrayView2D<const char>& fieldData, UnsignedShort fieldArraySize = 0, SceneFieldFlags flags = {}) noexcept;

        /** @overload */
        explicit SceneFieldData(SceneField name, const Containers::StridedArrayView2D<const char>& mappingData, SceneFieldType fieldType, const Containers::StridedArrayView2D<const char>& fieldData, SceneFieldFlags flags) noexcept: SceneFieldData{name, mappingData, fieldType, fieldData, 0, flags} {}

        /**
         * @brief Constructor
         * @param name          Field name
         * @param mappingData   Object mapping data
         * @param fieldData     Field data
         * @param flags         Field flags. @ref SceneFieldFlag::OffsetOnly
         *      and @ref SceneFieldFlag::NullTerminatedString is not allowed
         *      here.
         *
         * Detects @ref SceneMappingType based on @p T and @ref SceneFieldType
         * based on @p U and calls @ref SceneFieldData(SceneField, SceneMappingType, const Containers::StridedArrayView1D<const void>&, SceneFieldType, const Containers::StridedArrayView1D<const void>&, UnsignedShort, SceneFieldFlags).
         * For all types known by Magnum, the detected @ref SceneFieldType is
         * of the same name as the type (so e.g. @relativeref{Magnum,Vector3ui}
         * gets recognized as @ref SceneFieldType::Vector3ui).
         */
        template<class T, class U> constexpr explicit SceneFieldData(SceneField name, const Containers::StridedArrayView1D<T>& mappingData, const Containers::StridedArrayView1D<U>& fieldData, SceneFieldFlags flags = {}) noexcept;

        /** @overload */
        template<class T, class U> constexpr explicit SceneFieldData(SceneField name, const Containers::StridedArrayView1D<T>& mappingData, const Containers::ArrayView<U>& fieldData, SceneFieldFlags flags = {}) noexcept: SceneFieldData{name, mappingData, Containers::stridedArrayView(fieldData), flags} {}

        /** @overload */
        template<class T, class U> constexpr explicit SceneFieldData(SceneField name, const Containers::ArrayView<T>& mappingData, const Containers::StridedArrayView1D<U>& fieldData, SceneFieldFlags flags = {}) noexcept: SceneFieldData{name, Containers::stridedArrayView(mappingData), fieldData, flags} {}

        /** @overload */
        template<class T, class U> constexpr explicit SceneFieldData(SceneField name, const Containers::ArrayView<T>& mappingData, const Containers::ArrayView<U>& fieldData, SceneFieldFlags flags = {}) noexcept: SceneFieldData{name, Containers::stridedArrayView(mappingData), Containers::stridedArrayView(fieldData), flags} {}

        /**
         * @brief Construct an array field
         * @param name          Field name
         * @param mappingData   Object mapping data
         * @param fieldData     Field data
         * @param flags         Field flags. @ref SceneFieldFlag::OffsetOnly
         *      and @ref SceneFieldFlag::NullTerminatedString is not allowed
         *      here.
         *
         * Detects @ref SceneMappingType based on @p T and @ref SceneFieldType
         * based on @p U and calls @ref SceneFieldData(SceneField, SceneMappingType, const Containers::StridedArrayView1D<const void>&, SceneFieldType, const Containers::StridedArrayView1D<const void>&, UnsignedShort, SceneFieldFlags)
         * with the @p fieldData second dimension size passed to
         * @p fieldArraySize. Expects that the second dimension of @p fieldData
         * is contiguous. At the moment only custom fields can be arrays, which
         * means this constructor can't be used with a builtin @p name. See
         * @ref SceneFieldData(SceneField, const Containers::StridedArrayView1D<T>&, const Containers::StridedArrayView1D<U>&, SceneFieldFlags)
         * for details about @ref SceneMappingType and @ref SceneFieldType
         * detection.
         */
        template<class T, class U> constexpr explicit SceneFieldData(SceneField name, const Containers::StridedArrayView1D<T>& mappingData, const Containers::StridedArrayView2D<U>& fieldData, SceneFieldFlags flags = {}) noexcept;

        /** @overload */
        template<class T, class U> constexpr explicit SceneFieldData(SceneField name, const Containers::ArrayView<T>& mappingData, const Containers::StridedArrayView2D<U>& fieldData, SceneFieldFlags flags = {}) noexcept: SceneFieldData{name, Containers::stridedArrayView(mappingData), fieldData, flags} {}

        /**
         * @brief Construct a bit field with a type-erased mapping view
         * @param name          Field name
         * @param mappingType   Object mapping type
         * @param mappingData   Object mapping data
         * @param fieldData     Field data
         * @param flags         Field flags. @ref SceneFieldFlag::OffsetOnly
         *      and @ref SceneFieldFlag::NullTerminatedString is not allowed
         *      here.
         *
         * Field type is implicitly @ref SceneFieldType::Bit and array size is
         * @cpp 0 @ce. Expects that @p mappingData and @p fieldData have the
         * same size. At the moment only custom fields can be bits, which means
         * this constructor can't be used with a builtin @p name.
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        constexpr explicit SceneFieldData(SceneField name, SceneMappingType mappingType, const Containers::StridedArrayView1D<const void>& mappingData, const Containers::StridedBitArrayView1D& fieldData, SceneFieldFlags flags = {}) noexcept;
        #else
        /* Has to be a template to avoid having to include StridedBitArrayView
           here -- compared to StridedArrayView it's used rather rarely */
        template<class T> constexpr explicit SceneFieldData(SceneField name, SceneMappingType mappingType, const Containers::StridedArrayView1D<const void>& mappingData, const Containers::BasicStridedBitArrayView<1, T>& fieldData, SceneFieldFlags flags = {}) noexcept;
        /* Uhh, and because of the template we need this overload as well */
        template<class T> constexpr explicit SceneFieldData(SceneField name, SceneMappingType mappingType, const Containers::StridedArrayView1D<const void>& mappingData, Containers::BasicBitArrayView<T> fieldData, SceneFieldFlags flags = {}) noexcept: SceneFieldData{name, mappingType, mappingData, Containers::BasicStridedBitArrayView<1, T>{fieldData}, flags} {}
        #endif

        /**
         * @brief Construct a bit field with a 2D type-erased mapping view
         * @param name          Field name
         * @param mappingData   Object mapping data
         * @param fieldData     Field data
         * @param flags         Field flags. @ref SceneFieldFlag::OffsetOnly
         *      and @ref SceneFieldFlag::NullTerminatedString is not allowed
         *      here.
         *
         * Field type is implicitly @ref SceneFieldType::Bit and array size is
         * @cpp 0 @ce. Expects that @p mappingData and @p fieldData have the
         * same size in the first dimension and that the second dimension of
         * @p mappingData is contiguous and its size is either 1, 2, 4 or 8,
         * corresponding to one of the @ref SceneMappingType values. At the
         * moment only custom fields can be bits, which means this constructor
         * can't be used with a builtin @p name.
         */
        explicit SceneFieldData(SceneField name, const Containers::StridedArrayView2D<const char>& mappingData, const Containers::StridedBitArrayView1D& fieldData, SceneFieldFlags flags = {}) noexcept;

        /**
         * @brief Construct a bit field
         * @param name          Field name
         * @param mappingData   Object mapping data
         * @param fieldData     Field data
         * @param flags         Field flags. @ref SceneFieldFlag::OffsetOnly
         *      and @ref SceneFieldFlag::NullTerminatedString is not allowed
         *      here.
         *
         * Detects @ref SceneMappingType based on @p T and calls @ref SceneFieldData(SceneField, SceneMappingType, const Containers::StridedArrayView1D<const void>&, const Containers::StridedBitArrayView1D&, SceneFieldFlags).
         */
        template<class T> constexpr explicit SceneFieldData(SceneField name, const Containers::StridedArrayView1D<T>& mappingData, const Containers::StridedBitArrayView1D& fieldData, SceneFieldFlags flags = {}) noexcept;

        /** @overload */
        template<class T> constexpr explicit SceneFieldData(SceneField name, const Containers::ArrayView<T>& mappingData, const Containers::StridedBitArrayView1D& fieldData, SceneFieldFlags flags = {}) noexcept: SceneFieldData{name, stridedArrayView(mappingData), fieldData, flags} {}

        /**
         * @brief Construct an array bit field with a type-erased mapping view
         * @param name          Field name
         * @param mappingType   Object mapping type
         * @param mappingData   Object mapping data
         * @param fieldData     Field data
         * @param flags         Field flags. @ref SceneFieldFlag::OffsetOnly
         *      and @ref SceneFieldFlag::NullTerminatedString is not allowed
         *      here.
         *
         * Field type is implicitly @ref SceneFieldType::Bit. Expects that
         * @p mappingData and @p fieldData have the same size in the first
         * dimension and that the second dimension of @p fieldData is
         * contiguous. At the moment only custom fields can be bits, which
         * means this constructor can't be used with a builtin @p name.
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        constexpr explicit SceneFieldData(SceneField name, SceneMappingType mappingType, const Containers::StridedArrayView1D<const void>& mappingData, const Containers::StridedBitArrayView2D& fieldData, SceneFieldFlags flags = {}) noexcept;
        #else
        /* Has to be a template to avoid having to include StridedBitArrayView
           here -- compared to StridedArrayView it's used rather rarely */
        template<class T> constexpr explicit SceneFieldData(SceneField name, SceneMappingType mappingType, const Containers::StridedArrayView1D<const void>& mappingData, const Containers::BasicStridedBitArrayView<2, T>& fieldData, SceneFieldFlags flags = {}) noexcept;
        #endif

        /**
         * @brief Construct an array bit field with a 2D type-erased mapping view
         * @param name          Field name
         * @param mappingData   Object mapping data
         * @param fieldData     Field data
         * @param flags         Field flags. @ref SceneFieldFlag::OffsetOnly
         *      and @ref SceneFieldFlag::NullTerminatedString is not allowed
         *      here.
         *
         * Field type is implicitly @ref SceneFieldType::Bit and array size is
         * @cpp 0 @ce. Expects that @p mappingData and @p fieldData have the
         * same size in the first dimension, that the second dimension of
         * @p mappingData is contiguous and its size is either 1, 2, 4 or 8,
         * corresponding to one of the @ref SceneMappingType values, and that
         * the second dimension of @p fieldData is contiguous. At the moment
         * only custom fields can be bits, which means this constructor
         * can't be used with a builtin @p name.
         */
        explicit SceneFieldData(SceneField name, const Containers::StridedArrayView2D<const char>& mappingData, const Containers::StridedBitArrayView2D& fieldData, SceneFieldFlags flags = {}) noexcept;

        /**
         * @brief Construct an array bit field
         * @param name          Field name
         * @param mappingData   Object mapping data
         * @param fieldData     Field data
         * @param flags         Field flags. @ref SceneFieldFlag::OffsetOnly
         *      and @ref SceneFieldFlag::NullTerminatedString is not allowed
         *      here.
         *
         * Detects @ref SceneMappingType based on @p T and calls @ref SceneFieldData(SceneField, SceneMappingType, const Containers::StridedArrayView1D<const void>&, const Containers::StridedBitArrayView2D&, SceneFieldFlags).
         */
        template<class T> constexpr explicit SceneFieldData(SceneField name, const Containers::StridedArrayView1D<T>& mappingData, const Containers::StridedBitArrayView2D& fieldData, SceneFieldFlags flags = {}) noexcept;

        /** @overload */
        template<class T> constexpr explicit SceneFieldData(SceneField name, const Containers::ArrayView<T>& mappingData, const Containers::StridedBitArrayView2D& fieldData, SceneFieldFlags flags = {}) noexcept: SceneFieldData{name, stridedArrayView(mappingData), fieldData, flags} {}

        /**
         * @brief Construct a string field from type-erased views
         * @param name          Field name
         * @param mappingType   Object mapping type
         * @param mappingData   Object mapping data
         * @param stringData    String to which the field offset or range data
         *      are relative to
         * @param fieldType     Field type. Only `SceneFieldType::String*`
         *      values are allowed here.
         * @param fieldData     Field data
         * @param flags         Field flags. @ref SceneFieldFlag::OffsetOnly is
         *      not allowed here. @ref SceneFieldFlag::NullTerminatedString is
         *      set implicitly for
         *      @ref SceneFieldType::StringRangeNullTerminated8,
         *      @relativeref{SceneFieldType,StringRangeNullTerminated16},
         *      @relativeref{SceneFieldType,StringRangeNullTerminated32} and
         *      @relativeref{SceneFieldType,StringRangeNullTerminated64}.
         *
         * Expects that @p mappingData and @p fieldData have the same size. At
         * the moment only custom fields can be strings, which means this
         * constructor can't be used with a builtin @p name.
         */
        /* While a StringView could provide the range assertions with more
           context inside the SceneFieldData constructor, the main range
           checking happens in the SceneField constructor, at which point the
           size would be gone anyway as SceneFieldData can store only the begin
           pointer inside. Using it would also mean we'd need to include its
           full definition in this header.

           Not constexpr because internally we store the 48-bit
           `stringData - fieldData` offset, which is not possible to do in a
           constexpr context. Only the offset-only constructor is usable for
           creating constexpr string fields. */
        explicit SceneFieldData(SceneField name, SceneMappingType mappingType, const Containers::StridedArrayView1D<const void>& mappingData, const char* stringData, SceneFieldType fieldType, const Containers::StridedArrayView1D<const void>& fieldData, SceneFieldFlags flags = {}) noexcept;

        /**
         * @brief Construct a string field from 2D type-erased views
         * @param name          Field name
         * @param mappingData   Object mapping data
         * @param stringData    String to which the field offset or range data
         *      are relative to
         * @param fieldType     Field type. Only `SceneFieldType::String*`
         *      values are allowed here.
         * @param fieldData     Field data
         * @param flags         Field flags. @ref SceneFieldFlag::OffsetOnly is
         *      not allowed here. @ref SceneFieldFlag::NullTerminatedString is
         *      set implicitly for
         *      @ref SceneFieldType::StringRangeNullTerminated8,
         *      @relativeref{SceneFieldType,StringRangeNullTerminated16},
         *      @relativeref{SceneFieldType,StringRangeNullTerminated32} and
         *      @relativeref{SceneFieldType,StringRangeNullTerminated64}.
         *
         * Expects that @p mappingData and @p fieldData have the same size in
         * the first dimension; that the second dimension of @p mappingData is
         * contiguous and its size is either 1, 2, 4 or 8, corresponding to one
         * of the @ref SceneMappingType values and that the second dimension of
         * @p fieldData is contiguous and its size matches @p fieldType. At the
         * moment only custom fields can be strings, which means this
         * constructor can't be used with a builtin @p name.
         */
        /* See above for why const char* is used instead of StringView */
        explicit SceneFieldData(SceneField name, const Containers::StridedArrayView2D<const char>& mappingData, const char* stringData, SceneFieldType fieldType, const Containers::StridedArrayView2D<const char>& fieldData, SceneFieldFlags flags = {}) noexcept;

        /**
         * @brief Construct a string field
         * @param name          Field name
         * @param mappingData   Object mapping data
         * @param stringData    String to which the field offset or range data
         *      are relative to
         * @param fieldType     Field type. Only `SceneFieldType::String*`
         *      values are allowed here.
         * @param fieldData     Field data
         * @param flags         Field flags. @ref SceneFieldFlag::OffsetOnly is
         *      not allowed here. @ref SceneFieldFlag::NullTerminatedString is
         *      set implicitly for
         *      @ref SceneFieldType::StringRangeNullTerminated8,
         *      @relativeref{SceneFieldType,StringRangeNullTerminated16},
         *      @relativeref{SceneFieldType,StringRangeNullTerminated32} and
         *      @relativeref{SceneFieldType,StringRangeNullTerminated64}.
         */
        /* See above for why const char* is used instead of StringView and why
           this function is not constexpr */
        template<class T> explicit SceneFieldData(SceneField name, const Containers::StridedArrayView1D<T>& mappingData, const char* stringData, SceneFieldType fieldType, const Containers::StridedArrayView1D<const void>& fieldData, SceneFieldFlags flags = {}) noexcept;

        /** @overload */
        template<class T> explicit SceneFieldData(SceneField name, const Containers::ArrayView<T>& mappingData, const char* stringData, SceneFieldType fieldType, const Containers::StridedArrayView1D<const void>& fieldData, SceneFieldFlags flags = {}) noexcept: SceneFieldData{name, Containers::stridedArrayView(mappingData), stringData, fieldType, fieldData, flags} {}

        /**
         * @brief Construct an offset-only field
         * @param name              Field name
         * @param size              Number of entries
         * @param mappingType       Object mapping type
         * @param mappingOffset     Object mapping data offset
         * @param mappingStride     Object mapping data stride
         * @param fieldType         Field type
         * @param fieldOffset       Field data offset
         * @param fieldStride       Field data stride
         * @param fieldArraySize    Field array size. Use @cpp 0 @ce for
         *      non-array fields.
         * @param flags             Field flags.
         *      @ref SceneFieldFlag::OffsetOnly is set implicitly.
         *      @ref SceneFieldFlag::NullTerminatedString is not allowed here.
         *
         * Instances created this way refer to offsets in unspecified
         * external scene data instead of containing the data views directly.
         * Useful when the location of the scene data array is not known at
         * field construction time. Expects that for builtin fields
         * @p fieldType corresponds to @p name and @p fieldArraySize is zero.
         *
         * Note that due to the @cpp constexpr @ce nature of this constructor,
         * no @p mappingType checks against @p mappingStride or
         * @p fieldType / @p fieldArraySize checks against @p fieldStride can
         * be done. You're encouraged to use the @ref SceneFieldData(SceneField, SceneMappingType, const Containers::StridedArrayView1D<const void>&, SceneFieldType, const Containers::StridedArrayView1D<const void>&, UnsignedShort, SceneFieldFlags)
         * constructor if you want additional safeguards.
         *
         * @ref SceneFieldType::Bit and `SceneFieldType::String*` values are
         * not allowed in @p fieldType. For offset-only bit fields use the
         * @ref SceneFieldData(SceneField, std::size_t, SceneMappingType, std::size_t, std::ptrdiff_t, std::size_t, std::size_t, std::ptrdiff_t, UnsignedShort, SceneFieldFlags)
         * constructor instead, for offset-only string fields use
         * @ref SceneFieldData(SceneField, std::size_t, SceneMappingType, std::size_t, std::ptrdiff_t, std::size_t, SceneFieldType, std::size_t, std::ptrdiff_t, SceneFieldFlags)
         * instead.
         * @see @ref flags(), @ref fieldArraySize() const,
         *      @ref mappingData(Containers::ArrayView<const void>) const,
         *      @ref fieldData(Containers::ArrayView<const void>) const
         */
        explicit constexpr SceneFieldData(SceneField name, std::size_t size, SceneMappingType mappingType, std::size_t mappingOffset, std::ptrdiff_t mappingStride, SceneFieldType fieldType, std::size_t fieldOffset, std::ptrdiff_t fieldStride, UnsignedShort fieldArraySize = 0, SceneFieldFlags flags = {}) noexcept;

        /** @overload */
        explicit constexpr SceneFieldData(SceneField name, std::size_t size, SceneMappingType mappingType, std::size_t mappingOffset, std::ptrdiff_t mappingStride, SceneFieldType fieldType, std::size_t fieldOffset, std::ptrdiff_t fieldStride, SceneFieldFlags flags) noexcept: SceneFieldData{name, size, mappingType, mappingOffset, mappingStride, fieldType, fieldOffset, fieldStride, 0, flags} {}

        /**
         * @brief Construct an offset-only bit field
         * @param name              Field name
         * @param size              Number of entries
         * @param mappingType       Object mapping type
         * @param mappingOffset     Object mapping data offset
         * @param mappingStride     Object mapping data stride
         * @param fieldOffset       Field data offset in bytes
         * @param fieldBitOffset    Field data bit offset
         * @param fieldStride       Field data stride *in bits*
         * @param fieldArraySize    Field array size. Use @cpp 0 @ce for
         *      non-array fields.
         * @param flags             Field flags.
         *      @ref SceneFieldFlag::OffsetOnly is set implicitly.
         *      @ref SceneFieldFlag::NullTerminatedString is not allowed here.
         *
         * Instances created this way refer to offsets in unspecified
         * external scene data instead of containing the data views directly.
         * Useful when the location of the scene data array is not known at
         * field construction time. At the moment only custom fields can be
         * bits, which means this constructor can't be used with a builtin
         * @p name. Expects that @p fieldBitOffset is less than @cpp 8 @ce, for
         * consistency with @ref Corrade::Containers::BasicStridedBitArrayView "Containers::StridedBitArrayView"
         * also expects that @p size fits into 29 bits on 32-bit platforms and
         * into 61 bits on 64-bit platforms.
         *
         * Note that due to the @cpp constexpr @ce nature of this constructor,
         * no @p mappingType checks against @p mappingStride can be done.
         * You're encouraged to use the @ref SceneFieldData(SceneField, SceneMappingType, const Containers::StridedArrayView1D<const void>&, const Containers::StridedBitArrayView1D&, SceneFieldFlags)
         * or @ref SceneFieldData(SceneField, SceneMappingType, const Containers::StridedArrayView1D<const void>&, const Containers::StridedBitArrayView2D&, SceneFieldFlags)
         * constructors if you want additional safeguards.
         * @see @ref SceneFieldData(SceneField, std::size_t, SceneMappingType, std::size_t, std::ptrdiff_t, SceneFieldType, std::size_t, std::ptrdiff_t, UnsignedShort, SceneFieldFlags),
         *      @ref flags(),
         *      @ref mappingData(Containers::ArrayView<const void>) const,
         *      @ref fieldData(Containers::ArrayView<const void>) const
         */
        explicit constexpr SceneFieldData(SceneField name, std::size_t size, SceneMappingType mappingType, std::size_t mappingOffset, std::ptrdiff_t mappingStride, std::size_t fieldOffset, std::size_t fieldBitOffset, std::ptrdiff_t fieldStride, UnsignedShort fieldArraySize = 0, SceneFieldFlags flags = {}) noexcept;

        /** @overload */
        explicit constexpr SceneFieldData(SceneField name, std::size_t size, SceneMappingType mappingType, std::size_t mappingOffset, std::ptrdiff_t mappingStride, std::size_t fieldOffset, std::size_t fieldBitOffset, std::ptrdiff_t fieldStride, SceneFieldFlags flags) noexcept: SceneFieldData{name, size, mappingType, mappingOffset, mappingStride, fieldOffset, fieldBitOffset, fieldStride, 0, flags} {}

        /**
         * @brief Construct an offset-only string field
         * @param name              Field name
         * @param size              Number of entries
         * @param mappingType       Object mapping type
         * @param mappingOffset     Object mapping data offset
         * @param mappingStride     Object mapping data stride
         * @param stringOffset      String data offset to which the field
         *      offset or range data are relative to
         * @param fieldType         Field type. Only `SceneFieldType::String*`
         *      values are allowed here.
         * @param fieldOffset       Field data offset
         * @param fieldStride       Field data stride
         * @param flags             Field flags.
         *      @ref SceneFieldFlag::OffsetOnly is set implicitly.
         *      @ref SceneFieldFlag::NullTerminatedString is set implicitly for
         *      @ref SceneFieldType::StringRangeNullTerminated8,
         *      @relativeref{SceneFieldType,StringRangeNullTerminated16},
         *      @relativeref{SceneFieldType,StringRangeNullTerminated32} and
         *      @relativeref{SceneFieldType,StringRangeNullTerminated64}.
         *
         * Instances created this way refer to offsets in unspecified
         * external scene data instead of containing the data views directly.
         * Useful when the location of the scene data array is not known at
         * field construction time. At the moment only custom fields can be
         * strings, which means this constructor can't be used with a builtin
         * @p name.
         *
         * Note that due to the @cpp constexpr @ce nature of this constructor,
         * no @p mappingType checks against @p mappingStride or
         * @p fieldType checks against @p fieldStride can be done. You're
         * encouraged to use the @ref SceneFieldData(SceneField, SceneMappingType, const Containers::StridedArrayView1D<const void>&, SceneFieldType, const Containers::StridedArrayView1D<const void>&, UnsignedShort, SceneFieldFlags)
         * constructor if you want additional safeguards.
         * @see @ref SceneFieldData(SceneField, std::size_t, SceneMappingType, std::size_t, std::ptrdiff_t, SceneFieldType, std::size_t, std::ptrdiff_t, UnsignedShort, SceneFieldFlags),
         *      @ref flags(),
         *      @ref mappingData(Containers::ArrayView<const void>) const,
         *      @ref fieldData(Containers::ArrayView<const void>) const
         */
        explicit constexpr SceneFieldData(SceneField name, std::size_t size, SceneMappingType mappingType, std::size_t mappingOffset, std::ptrdiff_t mappingStride, std::size_t stringOffset, SceneFieldType fieldType, std::size_t fieldOffset, std::ptrdiff_t fieldStride, SceneFieldFlags flags = {}) noexcept;

        /** @brief Field flags */
        constexpr SceneFieldFlags flags() const { return _flags; }

        /** @brief Field name */
        constexpr SceneField name() const { return _name; }

        /** @brief Number of entries */
        constexpr std::size_t size() const { return _size; }

        /** @brief Object mapping type */
        constexpr SceneMappingType mappingType() const {
            return SceneMappingType(_mappingTypeStringType & Implementation::SceneMappingTypeMask);
        }

        /**
         * @brief Type-erased object mapping data
         *
         * Expects that the field does not have @ref SceneFieldFlag::OffsetOnly
         * set, in that case use the @ref mappingData(Containers::ArrayView<const void>) const
         * overload instead.
         * @see @ref flags()
         */
        Containers::StridedArrayView1D<const void> mappingData() const;

        /**
         * @brief Type-erased object mapping data for an offset-only field
         *
         * If the field does not have @ref SceneFieldFlag::OffsetOnly set, the
         * @p data parameter is ignored.
         * @see @ref flags(), @ref mappingData() const
         */
        Containers::StridedArrayView1D<const void> mappingData(Containers::ArrayView<const void> data) const;

        /** @brief Field type */
        SceneFieldType fieldType() const {
            return _mappingTypeStringType & Implementation::SceneMappingStringTypeMask ?
                SceneFieldType((_mappingTypeStringType & Implementation::SceneMappingStringTypeMask) >> 3) : _field.data.type;
        }

        /** @brief Field array size */
        UnsignedShort fieldArraySize() const {
            return _mappingTypeStringType & Implementation::SceneMappingStringTypeMask ?
                0 : _field.data.arraySize;
        }

        /**
         * @brief Type-erased field data
         *
         * Expects that the field is not @ref SceneFieldType::Bit and does not
         * have @ref SceneFieldFlag::OffsetOnly set. For bit fields use
         * @ref fieldBitData() instead, for offset-only fields use the
         * @ref fieldData(Containers::ArrayView<const void>) const overload
         * instead.
         * @see @ref fieldType(), @ref flags()
         */
        Containers::StridedArrayView1D<const void> fieldData() const;

        /**
         * @brief Type-erased field data for an offset-only field
         *
         * Expects that the field is not @ref SceneFieldType::Bit, in that case
         * use @ref fieldBitData(Containers::ArrayView<const void>) const
         * instead. If the field does not have @ref SceneFieldFlag::OffsetOnly
         * set, the @p data parameter is ignored.
         * @see @ref fieldType(), @ref flags(), @ref fieldData() const
         */
        Containers::StridedArrayView1D<const void> fieldData(Containers::ArrayView<const void> data) const;

        /**
         * @brief Bit field data
         *
         * Expects that the field is @ref SceneFieldType::Bit and does not have
         * @ref SceneFieldFlag::OffsetOnly set. For non-bit fields use
         * @ref fieldData() instead, for offset-only bit fields use the
         * @ref fieldBitData(Containers::ArrayView<const void>) const overload
         * instead. The returned view is 2D with the second dimension being
         * always @cpp 1 @ce for non-array fields. The second dimension is
         * always contiguous.
         * @see @ref fieldType(), @ref flags()
         */
        Containers::StridedBitArrayView2D fieldBitData() const;

        /**
         * @brief Bit field data for an offset-only field
         *
         * Expects that the field is @ref SceneFieldType::Bit, otherwise use
         * @ref fieldData(Containers::ArrayView<const void>) const instead. If
         * the field does not have @ref SceneFieldFlag::OffsetOnly set, the
         * @p data parameter is ignored. The returned view is 2D with the
         * second dimension being always @cpp 1 @ce for non-array fields. The
         * second dimension is always contiguous.
         * @see @ref fieldType(), @ref flags(), @ref fieldBitData() const
         */
        Containers::StridedBitArrayView2D fieldBitData(Containers::ArrayView<const void> data) const;

        /**
         * @brief Base data pointer for a string field
         *
         * Offsets and ranges returned from @ref fieldData() are relative to
         * this pointer. Can be only called on
         * @ref SceneFieldType::StringOffset32 "SceneFieldType::StringOffset*",
         * @ref SceneFieldType::StringRange32 "StringRange*"
         * and @ref SceneFieldType::StringRangeNullTerminated32 "StringRangeNullTerminated*"
         * fields.
         *
         * Expects that the field does not have @ref SceneFieldFlag::OffsetOnly
         * set, in that case use the @ref stringData(Containers::ArrayView<const void>) const
         * overload instead.
         * @see @ref flags()
         */
        const char* stringData() const;

        /**
         * @brief Base data pointer for an offset-only string field
         *
         * Offsets and ranges returned from @ref SceneData::field() are
         * relative to this pointer. Can be only called on
         * @ref SceneFieldType::StringOffset32 "SceneFieldType::StringOffset*",
         * @ref SceneFieldType::StringRange32 "StringRange*"
         * and @ref SceneFieldType::StringRangeNullTerminated32 "StringRangeNullTerminated*"
         * fields.
         *
         * If the field does not have @ref SceneFieldFlag::OffsetOnly set, the
         * @p data parameter is ignored.
         * @see @ref flags(), @ref fieldData() const
         */
        const char* stringData(Containers::ArrayView<const void> data) const;

    private:
        friend SceneData;

        /* Delegated to from all StridedBitArrayView constructors, contains
           assertions common for all variants */
        constexpr explicit SceneFieldData(SceneField name, SceneMappingType mappingType, const Containers::StridedArrayView1D<const void>& mappingData, const void* fieldData, UnsignedByte fieldBitOffset, std::size_t fieldSize, std::ptrdiff_t fieldStride, UnsignedShort fieldArraySize, SceneFieldFlags flags) noexcept;
        /* Delegated to from all StridedBitArrayView constructors with a 2D
           mapping view, contains common SceneMappingType handling */
        explicit SceneFieldData(SceneField name, const Containers::StridedArrayView2D<const char>& mappingData, const void* fieldData, UnsignedByte fieldBitOffset, std::size_t fieldSize, std::ptrdiff_t fieldStride, UnsignedShort fieldArraySize, SceneFieldFlags flags) noexcept;

        union Data {
            /* FFS C++ why this doesn't JUST WORK goddamit?! It's already past
               the End Of Times AND YET this piece of complex shit can't do the
               obvious! */
            constexpr Data(const void* pointer = nullptr): pointer{pointer} {}
            constexpr Data(std::size_t offset): offset{offset} {}

            const void* pointer;
            std::size_t offset;
        };

        /** @todo to make more room for extra properties inside Field or to
            support array string fields, this could be size (6 bytes) + array
            size (2 bytes) together, similarly to how string data offset is
            packed below */
        UnsignedLong _size;
        SceneField _name;
        SceneFieldFlags _flags;
        /* Contains SceneMappingType in the lower 3 bits. If the
           next 4 bits are non-zero, they encode one of the
           `SceneFieldType::String* << 3` values. 1 remaining bit unused. */
        UnsignedByte _mappingTypeStringType;
        Short _mappingStride;
        Data _mappingData;

        /* Contains either of the two following layouts depending on whether
           _mappingTypeStringType marks this as a string field. In that case
           the type is stored in _mappingTypeStringType already and the
           remaining 6 bytes store a *signed* string data offset:

            -   if SceneFieldFlag::OffsetOnly is not set, relative to
                _fieldData.pointer
            -   if OffsetOnly is set, absolute

           The 6 bytes allow addressing up to ±128 TB of data, which should be
           plenty, making it relative to _fieldData.pointer accounts for cases
           where an absolute pointer itself wouldn't fit into 48 bits.

            0            2            4            6       7    8 LE
            +------------+------------+------------+------+-----+
            |            |    type    | array size | bOff |     |
            +   stride   +------------+------------+------+-----+
            |            |          string data offset          |
            +------------+--------------------------------------+
            8            6            4            2            0 BE */
        union Field {
            /* C++, if you wouldn't be stupid, these constructors wouldn't be
               needed. I just want to initialize one or the other union
               field! */
            constexpr explicit Field(): data{} {}
            constexpr explicit Field(Short stride, SceneFieldType type, UnsignedShort arraySize, UnsignedByte bitOffset = 0): data{stride, type, arraySize, bitOffset} {}
            constexpr explicit Field(Short stride, Long offset): strideOffset{(UnsignedLong(stride) & 0xffff)|((UnsignedLong(offset) & 0xffffffffffffull) << 16)} {}

            struct {
                Short stride;
                SceneFieldType type;
                UnsignedShort arraySize;
                UnsignedByte bitOffset; /* for FieldType::Bit, 5 bits unused */
                /* 1 byte unused */
            } data;
            UnsignedLong strideOffset; /* Upper 48 bits on LE, lower 48 on BE */
        } _field;
        Data _fieldData;
};

/** @relatesalso SceneFieldData
@brief Create a non-owning array of @ref SceneFieldData items
@m_since_latest

Useful when you have the field definitions statically defined (for example when
the data themselves are already defined at compile time) and don't want to
allocate just to pass those to @ref SceneData.
*/
Containers::Array<SceneFieldData> MAGNUM_TRADE_EXPORT sceneFieldDataNonOwningArray(Containers::ArrayView<const SceneFieldData> view);

/**
@brief Scene data

Contains scene node hierarchy, transformations, resource assignment as well as
any other data associated with the scene. Populated instances of this class are
returned from @ref AbstractImporter::scene() as well as used in various
@ref SceneTools algorithms. Like with other @ref Trade types, the internal
representation is fixed upon construction and allows only optional in-place
modification of the data itself, but not of the overall structure.

@section Trade-SceneData-representation Data representation and terminology

@m_div{m-container-inflate m-col-l-9 m-left-l}
@htmlinclude scenedata-tree.svg
@m_enddiv

The usual mental image of a scene is a tree hierarchy with varying amount of
data attached to each node, like shown in the first diagram. The @ref SceneData
however decouples the hierarchy from the data and stores everything in linear
arrays, like in the second diagram.

This allows for a more efficient storage, as only the actually needed
information is stored. For example, three nodes in the tree have an implicit
transformation, which we can simply omit, or because there might be way less
materials than meshes, their references can be in a smaller type. It's also
more flexible --- having multiple meshes per node is just about having multiple
entries associated with the same node.

@m_div{m-clearfix-l} @m_enddiv

@m_div{m-container-inflate m-col-l-8 m-right-l}
@htmlinclude scenedata-dod.svg
@m_enddiv

From a high-level perspective, the scene data storage can thought of as a set
of *Fields*, with field entries mapped to *Objects*. Scene *Nodes* are a
special case of *Objects*.

An *Object* is an arbitrary numeric identifier, not containing anything on its
own. All objects referenced by a particular scene are contained in a range from
@cpp 0 @ce up to @ref mappingBound() minus one. The range is allowed to be
sparse.

A *Field* is a list of data --- for example transformations, mesh IDs, or
parent objects. The @ref SceneField enum lists all predefined fields together
with possible restrictions and the expected @ref SceneFieldType they're
expected to be in. Custom fields are supported as well. Field entries are
mapped to objects with the same 8-, 16-, 32- or 64-bit type for all fields,
indicated with @ref SceneMappingType. Generally there's a 1:N mapping between
objects and fields (not all objects need to have a transformation, a single
object can reference multiple meshes...), but certain field types expect
various restrictions (such as an object allowed to only have one parent or
transformation).

Finally, scene *Nodes* are *Objects* that have the @ref SceneField::Parent
field associated. An *Object* thus doesn't have to represent just a node in the
hierarchy. For example, a scene can also contain an alternative representation
in the form of an octree, and thus some objects would be nodes and some octree
cells.

@subsection Trade-SceneData-representation-multi-scene Object identifiers and multiple scenes

For a standalone scene, a common case is that the object identifiers form a
contigous range of numbers, and each of the objects has at least one field
assigned.

The @ref AbstractImporter supports files with multiple scenes. All imported
scenes share a single object range, from @cpp 0 @ce to
@ref AbstractImporter::objectCount(). A particular object can be part of any of
the scenes, causing the @ref SceneData::mappingBound() ranges to be sparse ---
a particular scene having certain object IDs that have no fields assigned. This
is something to be aware of when consuming the scene data, that not all objects
identifiers in the mapping range may actually exist.

It's also possible for a single object identifier to be contained in multiple
scenes at the same time --- for example, when two scenes are variants of the
same model, with most data shared but certain textures or colors different.
Another theoretical use case is that an object could identify a building in a
3D scene and a corresponding area on a map in a 2D scene. There's no set of
rules the objects should follow, but such identifier reusal should not be
abused for completely unrelated objects.

@todoc mention handles and how they would affect the basic use below (aaaa!)

@section Trade-SceneData-usage Basic usage

A simple goal could be to populate a @ref SceneGraph with a node hierarchy
and attach drawables for meshes where appropriate. First we check if the scene
is 3D with @ref is3D(), because if it's not, it could mean it's either 2D or
that it has no transformation field altogether, suggesting a need for
specialized handling. It's also of no use for this example if there's no node
hierarchy, or if there are no meshes we could draw.

Then we create the scene instance and an array of pointers that will act as a
map from object identifiers to live objects. The @ref mappingBound() is an
upper bound to all object identifiers referenced by the scene, but as mentioned
above, not all of them may be actual nodes so we don't allocate actual scene
graph object instances for them yet. Alternatively, for very sparse ranges, a hashmap could be also used here.

@snippet Trade.cpp SceneData-usage1

<b></b>

@m_class{m-noindent}

Next we go through objects that have an associated parent using
@ref parentsAsArray(). Those are the actual nodes we want, so we allocate a
scene graph object for each ...

@snippet Trade.cpp SceneData-usage2

@m_class{m-noindent}

<b></b>

... and then we assign a proper parent, or add it directly to the scene if the
parent is @cpp -1 @ce. We do this in a separate pass to ensure the parent
object is already allocated by the time we pass it to
@ref SceneGraph::Object::setParent() --- generally there's no guarantee that a
parent appears in the field before its children.

@snippet Trade.cpp SceneData-usage3

With the hierarchy done, we assign transformations. The transformation field
can be present for only a subset of the nodes, with the rest implicitly having
an indentity transformation, but it can also be present for objects that aren't
nodes, so we only set it for objects present in our hierarchy. The
@ref transformations3DAsArray() function also conveniently converts separate
transformation / rotation / scaling fields into a matrix for us, if the scene
contains only those.

@snippet Trade.cpp SceneData-usage4

Finally, assuming there's a `Drawable` class derived from
@ref SceneGraph::Drawable that accepts a mesh and material ID (retrieving them
subsequently from @ref AbstractImporter::mesh() /
@relativeref{AbstractImporter,material()}, for example), the process of
assigning actual meshes to corresponding scene nodes is just another
@cpp for @ce loop over @ref meshesMaterialsAsArray():

@snippet Trade.cpp SceneData-usage5

<b></b>

@m_class{m-note m-success}

@par
    The full process of importing a scene including meshes, materials and
    textures is shown in the @ref examples-viewer example.

@section Trade-SceneData-usage-advanced Advanced usage

The @ref parentsAsArray(), ... functions shown above always return a
newly-allocated @relativeref{Corrade,Containers::Array} instance in a
well-defined canonical type. While that's convenient and fine at a smaller
scale, it may prove problematic with huge scenes. Or maybe the internal
representation is already optimized for best processing efficiency and the
convenience functions would ruin that. The @ref SceneData class thus provides
access directly to the stored object mapping and field data using the
@ref mapping() and @ref field() accessors, together with specialized
@ref fieldBits() for accessing bit fields and @ref fieldStrings() for accessing
string fields.

However, since each @ref SceneField can be in a variety of types, you're
expected to either check that the type is indeed what you expect using
@ref fieldType(SceneField) const, or at least check with documentation of the
corresponding importer. For example, because glTF files represent the scene
in a textual form, @ref GltfImporter will always parse the data into canonical
32-bit types. With that assumption, the above snippet that used
@ref transformations3DAsArray() can be rewritten to a zero-copy form like this:

@snippet Trade.cpp SceneData-usage-advanced

@section Trade-SceneData-usage-per-object Per-object access

While the designated way to access scene data is by iterating through the field
and object arrays, it's also possible to directly look at fields for a
particular object without having to do a lookup on your own and with simplified
error handling. The @ref parentFor(), @ref childrenFor(),
@ref transformation3DFor(), @ref meshesMaterialsFor() and other functions
return either an @relativeref{Corrade,Containers::Optional} or an
@relativeref{Corrade,Containers::Array} depending on whether there's expected
just one occurence of the field or more, returning an empty optional or array
if the field is not present in the scene or if the object was not found in the
field array.

For example, together with an @ref AbstractImporter instance the scene comes
from, the following snippet lists meshes and material names that are associated
with a "Chair" object, assuming such object exists:

@snippet Trade.cpp SceneData-per-object

The actual object ID lookup is done by @ref findFieldObjectOffset() and
depending on what @ref SceneFieldFlags are present for given field, it can be
done in constant, logarithmic or, worst case, linear time. As such, for general
scene representations these are suited mainly for introspection and debugging
purposes and retrieving field data for many objects is better achieved by
accessing the field data directly.

@section Trade-SceneData-usage-mutable Mutable data access

The interfaces implicitly provide @cpp const @ce views on the contained object
and field data through the @ref data(), @ref mapping() and @ref field()
accessors. This is done because in general case the data can also refer to a
memory-mapped file or constant memory. In cases when it's desirable to modify
the data in-place, there's the @ref mutableData(), @ref mutableMapping() and
@ref mutableField() set of functions. To use these, you need to check that
the data are mutable using @ref dataFlags() first. The following snippet
updates all transformations with the live state of a scene imported earlier,
for example in order to bake in a certain animation state:

@snippet Trade.cpp SceneData-usage-mutable

@section Trade-SceneData-populating Populating an instance

The actual data in a @ref SceneData instance are represented as a single block
of contiguous memory, which all object and field views point to. This is
easiest to achieve with an @relativeref{Corrade,Containers::ArrayTuple}. In the
example below, all objects have a parent and a transformation field, which are
stored together in a @cpp struct @ce, while a subset of them has a mesh and a
material assigned, which are stored in separate arrays. And because the scene
is small, we save space by using just 16-bit indices for everything.

@snippet Trade.cpp SceneData-populating

Note that the above layout is just an example, you're free to choose any
representation that matches your use case best, with fields interleaved
together or not. See also the @ref SceneFieldData class documentation for
additional ways how to specify and annotate the data.

@subsection Trade-SceneData-populating-non-owned Non-owned instances and static data layouts

In some cases you may want the @ref SceneData instance to only refer to
external data without taking ownership, for example with a memory-mapped file,
global data etc. For that, instead of moving in an
@relativeref{Corrade,Containers::Array}, pass @ref DataFlags describing data
mutability and ownership together with an
@relativeref{Corrade,Containers::ArrayView} to the
@ref SceneData(SceneMappingType, UnsignedLong, DataFlags, Containers::ArrayView<const void>, Containers::Array<SceneFieldData>&&, const void*)
constructor:

@snippet Trade.cpp SceneData-populating-non-owned

The @ref SceneFieldData list is still implicitly allocated in the above case,
but it can also be defined externally and referenced via
@ref sceneFieldDataNonOwningArray() instead if desired. Finally, if the data
layout is constant but the actual data is allocated / populated at runtime, the
@ref SceneFieldData instances can be defined in a global array as offset-only
and then subsequently referenced from a @ref SceneData with a concrete data
array:

@snippet Trade.cpp SceneData-populating-offset-only

See also the @ref Trade-SceneFieldData-usage-offset-only "corresponding SceneFieldData documentation for offset-only fields".

@subsection Trade-SceneData-populating-custom Custom scene fields and non-node objects

Let's say that, in addition to the node hierarchy from above, our scene
contains also a precomputed [camera-space light culling grid](https://wickedengine.net/2018/01/10/optimizing-tile-based-light-culling/),
where each cell of the grid contains a list of lights that affect given area of
the screen. And we want to add it into the @ref SceneData for verification with
external tools.

For simplicity let's assume we have a 32x24 grid and the shader we have can
work with up to 8 lights. So there will be a fixed-size array for each of those
cells, and we save calculated frustums for inspection as well. For the new data
we allocate object IDs from a range after `nodeCount`, and copy in the actual
data.

@snippet Trade.cpp SceneData-populating-custom1

Then, similarly as with @ref MeshData, the scene can have custom fields as
well, created with @ref sceneFieldCustom(). We create one for the cell light
reference array and one for the cell frustum and then use them to annotate
the views allocated above. Note that we also increased the total object count
to include the light culling grid cells as well.

@snippet Trade.cpp SceneData-populating-custom2

Later, the fields can be retrieved back using the same custom identifiers.
The light references are actually a 2D array (8 lights for each cell), so a
@cpp [] @ce needs to be used:

@snippet Trade.cpp SceneData-populating-custom-retrieve

@subsection Trade-SceneData-populating-strings String fields

Besides fixed-size types and their arrays, the @ref SceneData class is capable
of storing strings. A string field consists of a base pointer and a list
containing offsets or ranges relative to that base pointer. Such separation
allows more efficient storage compared to storing a full pointer (or a pair of
a pointer and size) for every field, as it's possible to choose a smaller
offset type if the referenced strings fit into a 8- or 16-bit range.

To cover different use cases, there's multiple ways how to store the string
references:

-   @ref SceneFieldType::StringOffset32 and its 8-, 16- and 64-bit variants
    store a running offset into the string array. For example, offsets
    @cpp {3, 11, 23} @ce would mean the first string is from byte 0 to 3,
    second is from byte 3 to 11 and third from byte 11 to 23. This storage type
    is most efficient if the strings are unique for each entry --- such as
    various names or external data filenames.
-   @ref SceneFieldType::StringRange32 and its 8-, 16- and 64-bit variants
    store a pair of offset and size. For example, ranges
    @cpp {{11, 5}, {4, 7}, {11, 5}} @ce would mean the first and third string
    is from byte 11 to 16 and the second string is from byte 4 to 11. This
    storage type is thus most efficient when there's many duplicates --- such
    as various object tags or categories.
-   @ref SceneFieldType::StringRangeNullTerminated32 and its 8-, 16- and 64-bit
    variants are similar to @ref SceneFieldType::StringRange32, but stores just
    the offset and size is implicitly until the next @cpp '\0' @ce byte. Thus
    it trades a slightly better space efficiency for the cost of a runtime
    @ref std::strlen() call on every access.

String fields can also have @ref SceneFieldFlag::NullTerminatedString set, in
which case the returned @relativeref{Corrade,Containers::StringView} instances
will have @relativeref{Corrade,Containers::StringViewFlag::NullTerminated} set,
which may be useful for example to avoid an allocation when passing filenames
to OS APIs in @relativeref{Corrade,Utility::Path::read()}. The null terminators
of course have to be stored in the data itself, see a particular
@ref SceneFieldType for information about how it affects the field encoding.

The following example shows populating a @ref SceneData with a "tag" string
field, stored as null-terminated 8-bit string array ranges. In other words ---
assuming there's enough stored data --- the space efficiency is the same as if
a just a numeric value of an 8-bit @cpp enum @ce would be stored, but here it
includes human-readable string names.

@snippet Trade.cpp SceneData-populating-strings

While there's many options how to store the string, retrieving of any string
@ref SceneFieldType can be conveniently done using @ref fieldStrings():

@snippet Trade.cpp SceneData-populating-strings-retrieve
*/
class MAGNUM_TRADE_EXPORT SceneData {
    public:
        /**
         * @brief Construct scene data
         * @param mappingType   Object mapping type
         * @param mappingBound  Upper bound on object mapping indices in the
         *      scene
         * @param data          Data for all fields and object mappings
         * @param fields        Description of all scene field data
         * @param importerState Importer-specific state
         * @m_since_latest
         *
         * The @p mappingType is expected to be large enough to index
         * @p mappingBound objects. The @p fields are expected to reference
         * (sparse) sub-ranges of @p data, each having an unique
         * @ref SceneField, and @ref SceneMappingType equal to @p mappingType.
         * Particular fields can have additional restrictions, see
         * documentation of @ref SceneField values for more information.
         *
         * The @ref dataFlags() are implicitly set to a combination of
         * @ref DataFlag::Owned and @ref DataFlag::Mutable. For non-owned data
         * use the @ref SceneData(SceneMappingType, UnsignedLong, DataFlags, Containers::ArrayView<const void>, Containers::Array<SceneFieldData>&&, const void*)
         * constructor or its variants instead.
         */
        explicit SceneData(SceneMappingType mappingType, UnsignedLong mappingBound, Containers::Array<char>&& data, Containers::Array<SceneFieldData>&& fields, const void* importerState = nullptr) noexcept;

        /**
         * @overload
         * @m_since_latest
         */
        /* Not noexcept because allocation happens inside */
        explicit SceneData(SceneMappingType mappingType, UnsignedLong mappingBound, Containers::Array<char>&& data, std::initializer_list<SceneFieldData> fields, const void* importerState = nullptr);

        /**
         * @brief Construct non-owned scene data
         * @param mappingType   Object mapping type
         * @param mappingBound  Upper bound on object mapping indices in the
         *      scene
         * @param dataFlags     Data flags
         * @param data          View on data for all fields and object mappings
         * @param fields        Description of all scene field data
         * @param importerState Importer-specific state
         * @m_since_latest
         *
         * Compared to @ref SceneData(SceneMappingType, UnsignedLong, Containers::Array<char>&&, Containers::Array<SceneFieldData>&&, const void*)
         * creates an instance that doesn't own the passed data. The
         * @p dataFlags parameter can contain @ref DataFlag::Mutable to
         * indicate the external data can be modified, and is expected to *not*
         * have @ref DataFlag::Owned set.
         */
        explicit SceneData(SceneMappingType mappingType, UnsignedLong mappingBound, DataFlags dataFlags, Containers::ArrayView<const void> data, Containers::Array<SceneFieldData>&& fields, const void* importerState = nullptr) noexcept;

        /**
         * @overload
         * @m_since_latest
         */
        /* Not noexcept because allocation happens inside */
        explicit SceneData(SceneMappingType mappingType, UnsignedLong mappingBound, DataFlags dataFlags, Containers::ArrayView<const void> data, std::initializer_list<SceneFieldData> fields, const void* importerState = nullptr);

        #ifdef MAGNUM_BUILD_DEPRECATED
         /**
         * @brief Constructor
         * @param children2D        Two-dimensional child objects
         * @param children3D        Three-dimensional child objects
         * @param importerState     Importer-specific state
         * @m_deprecated_since_latest Use @ref SceneData(SceneMappingType, UnsignedLong, Containers::Array<char>&&, Containers::Array<SceneFieldData>&&, const void*)
         *      instead.
         */
        explicit CORRADE_DEPRECATED("use SceneData(SceneMappingType, UnsignedLong, Containers::Array<char>&&, Containers::Array<SceneFieldData>&&, const void*) instead") SceneData(std::vector<UnsignedInt> children2D, std::vector<UnsignedInt> children3D, const void* importerState = nullptr);
        #endif

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
         * @see @ref releaseData(), @ref mutableData(), @ref mutableMapping(),
         *      @ref mutableField()
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
         * @brief Type used for object mapping
         * @m_since_latest
         *
         * Type returned from @ref mapping() and @ref mutableMapping(). It's
         * the same for all fields and is guaranteed to be large enough to fit
         * @ref mappingBound() objects.
         */
        SceneMappingType mappingType() const { return _mappingType; }

        /**
         * @brief Object mapping bound
         * @m_since_latest
         *
         * Upper bound on object mapping indices of all fields in the scene.
         * Note that an object can have a certain field associated with
         * it multiple times with different values (for example an object
         * having multiple meshes), and thus a field size can be larger than
         * @ref mappingBound() --- see @ref fieldSizeBound() for an upper
         * bound for all field sizes.
         * @see @ref fieldCount(), @ref fieldSize()
         */
        UnsignedLong mappingBound() const { return _mappingBound; }

        /**
         * @brief Field count
         * @m_since_latest
         *
         * Count of different fields contained in the scene, or @cpp 0 @ce for
         * a scene with no fields. Each @ref SceneField can be present only
         * once, however an object can have a certain field associated with it
         * multiple times with different values (for example an object having
         * multiple meshes).
         */
        UnsignedInt fieldCount() const { return _fields.size(); }

        /**
         * @brief Field size bound
         * @m_since_latest
         *
         * A maximum of all @ref fieldSize() or @cpp 0 @ce for a scene with no
         * fields. Note that an object can have a certain field associated with
         * it multiple times with different values (for example an object
         * having multiple meshes), and thus a field size can be larger than
         * @ref mappingBound().
         */
        std::size_t fieldSizeBound() const;

        /**
         * @brief Raw field metadata
         * @m_since_latest
         *
         * Returns the raw data that are used as a base for all `field*()`
         * accessors, or @cpp nullptr @ce if the scene has no fields. In most
         * cases you don't want to access those directly, but rather use the
         * @ref mapping(), @ref field(), @ref fieldName(), @ref fieldType(),
         * @ref fieldSize() and @ref fieldArraySize() accessors. Compared to
         * those and to @ref fieldData(UnsignedInt) const, the
         * @ref SceneFieldData instances returned by this function may have
         * different data pointers, and some of them might have
         * @ref SceneFieldFlag::OffsetOnly set --- use this function only if
         * you *really* know what are you doing.
         * @see @ref SceneFieldData::flags()
         */
        Containers::ArrayView<const SceneFieldData> fieldData() const & { return _fields; }

        /**
         * @brief Taking a view to a r-value instance is not allowed
         * @m_since_latest
         */
        Containers::ArrayView<const SceneFieldData> fieldData() const && = delete;

        /**
         * @brief Raw field data
         * @m_since_latest
         *
         * Returns the raw data that are used as a base for all `field*()`
         * accessors. In most cases you don't want to access those directly,
         * but rather use the @ref mapping(), @ref field(), @ref fieldName(),
         * @ref fieldType(), @ref fieldSize() and @ref fieldArraySize()
         * accessors. This is also the reason why there's no overload taking a
         * @ref SceneField, unlike the other accessors.
         *
         * Unlike with @ref fieldData() and @ref releaseFieldData(), returned
         * instances are guaranteed to always have an absolute data pointer
         * (i.e., @ref SceneFieldData::flags() never having
         * @ref SceneFieldFlag::OffsetOnly set). The @p id is expected to be
         * smaller than @ref fieldCount().
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
         * @brief Field flags
         * @m_since_latest
         *
         * The @p id is expected to be smaller than @ref fieldCount().
         * @see @ref findFieldObjectOffset(UnsignedInt, UnsignedLong, std::size_t) const
         */
        SceneFieldFlags fieldFlags(UnsignedInt id) const;

        /**
         * @brief Field type
         * @m_since_latest
         *
         * The @p id is expected to be smaller than @ref fieldCount(). You can
         * also use @ref fieldType(SceneField) const to directly get a type of
         * given named field.
         * @see @ref fieldName(), @ref mappingType()
         */
        SceneFieldType fieldType(UnsignedInt id) const;

        /**
         * @brief Number of entries in a field
         * @m_since_latest
         *
         * Size of the view returned by @ref mapping() / @ref mutableMapping()
         * and @ref field() / @ref mutableField() for given @p id. Since an
         * object can have multiple entries of the same field (for example
         * multiple meshes associated with an object), the size doesn't
         * necessarily match the number of objects having given field.
         *
         * The @p id is expected to be smaller than @ref fieldCount(). You can
         * also use @ref fieldSize(SceneField) const to directly get a size of
         * given named field.
         */
        std::size_t fieldSize(UnsignedInt id) const;

        /**
         * @brief Field array size
         * @m_since_latest
         *
         * In case given field is an array (the euqivalent of e.g.
         * @cpp int[30] @ce), returns array size, otherwise returns @cpp 0 @ce.
         * At the moment only custom fields can be arrays, no builtin
         * @ref SceneField is an array field. Additionally, fields with
         * @ref SceneFieldType::StringOffset32 "SceneFieldType::StringOffset*",
         * @ref SceneFieldType::StringRange32 "SceneFieldType::StringRange*" or
         * @ref SceneFieldType::StringRangeNullTerminated32 "SceneFieldType::StringRangeNullTerminated*"
         * can't be arrays, for them the function always returns @cpp 0 @ce.
         *
         * Note that this is different from the count of entries for given
         * field, which is exposed through @ref fieldSize(). See
         * @ref Trade-SceneData-populating-custom for an example.
         *
         * The @p id is expected to be smaller than @ref fieldCount(). You can
         * also use @ref fieldArraySize(SceneField) const to directly get a
         * type of given named field.
         */
        UnsignedShort fieldArraySize(UnsignedInt id) const;

        /**
         * @brief Whether the scene is two-dimensional
         * @m_since_latest
         *
         * Returns @cpp true @ce if the present
         * @ref SceneField::Transformation,
         * @relativeref{SceneField,Translation},
         * @relativeref{SceneField,Rotation} and
         * @relativeref{SceneField,Scaling} fields have a 2D type,
         * @cpp false @ce otherwise.
         *
         * If there's no transformation-related field, the scene is treated as
         * neither 2D nor 3D and both @ref is2D() and @ref is3D() return
         * @cpp false @ce. On the other hand, a scene can't be both 2D and 3D.
         * @see @ref hasField()
         */
        bool is2D() const { return _dimensions == 2; }

        /**
         * @brief Whether the scene is three-dimensional
         * @m_since_latest
         *
         * Returns @cpp true @ce if the present
         * @ref SceneField::Transformation,
         * @relativeref{SceneField,Translation},
         * @relativeref{SceneField,Rotation} and
         * @relativeref{SceneField,Scaling} fields have a 3D type,
         * @cpp false @ce otherwise.
         *
         * If there's no transformation-related field, the scene is treated as
         * neither 2D nor 3D and both @ref is2D() and @ref is3D() return
         * @cpp false @ce. On the other hand, a scene can't be both 2D and 3D.
         * @see @ref hasField()
         */
        bool is3D() const { return _dimensions == 3; }

        /**
         * @brief Find an absolute ID of a named field
         * @m_since_latest
         *
         * If @p name doesn't exist, returns
         * @relativeref{Corrade,Containers::NullOpt}. The lookup is done in an
         * @f$ \mathcal{O}(n) @f$ complexity with @f$ n @f$ being the field
         * count.
         * @see @ref hasField(), @ref fieldId()
         */
        Containers::Optional<UnsignedInt> findFieldId(SceneField name) const;

        /**
         * @brief Absolute ID of a named field
         * @m_since_latest
         *
         * Like @ref findFieldId(), but the @p name is expected to exist.
         * @see @ref hasField(), @ref fieldName(UnsignedInt) const
         */
        UnsignedInt fieldId(SceneField name) const;

        /**
         * @brief Whether the scene has given field
         * @m_since_latest
         *
         * @see @ref is2D(), @ref is3D(), @ref findFieldId()
         */
        bool hasField(SceneField name) const;

        /**
         * @brief Find offset of an object in given field
         * @m_since_latest
         *
         * If @p object isn't present in @p fieldId starting at @p offset,
         * returns @relativeref{Corrade,Containers::NullOpt}. The @p fieldId is
         * expected to be smaller than @ref fieldCount(), @p object smaller
         * than @ref mappingBound() and @p offset not larger than
         * @ref fieldSize(UnsignedInt) const.
         *
         * If the field has @ref SceneFieldFlag::ImplicitMapping, the lookup is
         * done in an @f$ \mathcal{O}(1) @f$ complexity. Otherwise, if the
         * field has @ref SceneFieldFlag::OrderedMapping, the lookup is done in
         * an @f$ \mathcal{O}(\log{} n) @f$ complexity with @f$ n @f$ being the
         * size of the field. Otherwise, the lookup is done in an
         * @f$ \mathcal{O}(n) @f$ complexity.
         *
         * You can also use @ref findFieldObjectOffset(SceneField, UnsignedLong, std::size_t) const
         * to directly find offset of an object in given named field.
         * @see @ref hasFieldObject(UnsignedInt, UnsignedLong) const,
         *      @ref fieldObjectOffset(UnsignedInt, UnsignedLong, std::size_t) const
         */
        Containers::Optional<std::size_t> findFieldObjectOffset(UnsignedInt fieldId, UnsignedLong object, std::size_t offset = 0) const;

        /**
         * @brief Find offset of an object in given named field
         * @m_since_latest
         *
         * If @p object isn't present in @p fieldName starting at @p offset,
         * returns @relativeref{Corrade,Containers::NullOpt}. The @p fieldName
         * is expected to exist, @p object is expected to be smaller than
         * @ref mappingBound() and @p offset not be larger than
         * @ref fieldSize(SceneField) const.
         *
         * If the field has @ref SceneFieldFlag::ImplicitMapping, the lookup is
         * done in an @f$ \mathcal{O}(m) @f$ complexity with @f$ m @f$ being
         * the field count. Otherwise, if the field has
         * @ref SceneFieldFlag::OrderedMapping, the lookup is done in an
         * @f$ \mathcal{O}(m + \log{} n) @f$ complexity with @f$ m @f$ being
         * the field count and @f$ n @f$ the size of the field. Otherwise, the
         * lookup is done in an @f$ \mathcal{O}(m + n) @f$ complexity.
         *
         * @see @ref hasField(), @ref hasFieldObject(SceneField, UnsignedLong) const,
         *      @ref fieldObjectOffset(SceneField, UnsignedLong, std::size_t) const
         */
        Containers::Optional<std::size_t> findFieldObjectOffset(SceneField fieldName, UnsignedLong object, std::size_t offset = 0) const;

        /**
         * @brief Offset of an object in given field
         * @m_since_latest
         *
         * Like @ref findFieldObjectOffset(UnsignedInt, UnsignedLong, std::size_t) const,
         * but @p object is additionally expected to be present in @p fieldId
         * starting at @p offset.
         *
         * You can also use @ref fieldObjectOffset(SceneField, UnsignedLong, std::size_t) const
         * to directly get offset of an object in given named field.
         */
        std::size_t fieldObjectOffset(UnsignedInt fieldId, UnsignedLong object, std::size_t offset = 0) const;

        /**
         * @brief Offset of an object in given named field
         * @m_since_latest
         *
         * Like @ref findFieldObjectOffset(SceneField, UnsignedLong, std::size_t) const,
         * but @p object is additionally expected to be present in @p fieldName
         * starting at @p offset.
         */
        std::size_t fieldObjectOffset(SceneField fieldName, UnsignedLong object, std::size_t offset = 0) const;

        /**
         * @brief Whether a scene field has given object
         * @m_since_latest
         *
         * The @p fieldId is expected to be smaller than @ref fieldCount() and
         * @p object smaller than @ref mappingBound().
         *
         * You can also use @ref hasFieldObject(SceneField, UnsignedLong) const
         * to directly query object presence in given named field.
         */
        bool hasFieldObject(UnsignedInt fieldId, UnsignedLong object) const;

        /**
         * @brief Whether a named scene field has given object
         * @m_since_latest
         *
         * The @p fieldName is expected to exist and @p object is expected to
         * be smaller than @ref mappingBound().
         * @see @ref hasField()
         */
        bool hasFieldObject(SceneField fieldName, UnsignedLong object) const;

        /**
         * @brief Flags of a named field
         * @m_since_latest
         *
         * The @p name is expected to exist.
         * @see @ref findFieldObjectOffset(SceneField, UnsignedLong, std::size_t) const
         */
        SceneFieldFlags fieldFlags(SceneField name) const;

        /**
         * @brief Type of a named field
         * @m_since_latest
         *
         * The @p name is expected to exist.
         * @see @ref hasField(), @ref fieldType(UnsignedInt) const
         */
        SceneFieldType fieldType(SceneField name) const;

        /**
         * @brief Number of entries in a named field
         * @m_since_latest
         *
         * The @p name is expected to exist.
         * @see @ref hasField(), @ref fieldSize(UnsignedInt) const
         */
        std::size_t fieldSize(SceneField name) const;

        /**
         * @brief Array size of a named field
         * @m_since_latest
         *
         * The @p name is expected to exist. Note that this is different from
         * the count of entries for given field, which is exposed through
         * @ref fieldSize(SceneField) const. See @ref fieldArraySize(UnsignedInt) const
         * for more information.
         * @see @ref hasField()
         */
        UnsignedShort fieldArraySize(SceneField name) const;

        /**
         * @brief Object mapping data for given field
         * @m_since_latest
         *
         * The @p fieldId is expected to be smaller than @ref fieldCount(). The
         * second dimension represents the actual data type (its size is equal
         * to @ref SceneMappingType size) and is guaranteed to be contiguous.
         * Use the templated overload below to get the mapping in a concrete
         * type. You can also use @ref mapping(SceneField) const to directly
         * get object mapping data for given named field.
         * @see @ref mutableMapping(UnsignedInt),
         *      @ref Corrade::Containers::StridedArrayView::isContiguous(),
         *      @ref sceneMappingTypeSize()
         */
        Containers::StridedArrayView2D<const char> mapping(UnsignedInt fieldId) const;

        /**
         * @brief Mutable object mapping data for given field
         * @m_since_latest
         *
         * Like @ref mapping(UnsignedInt) const, but returns a mutable view.
         * Expects that the scene is mutable.
         * @see @ref dataFlags()
         */
        Containers::StridedArrayView2D<char> mutableMapping(UnsignedInt fieldId);

        /**
         * @brief Object mapping for given field
         * @m_since_latest
         *
         * The @p fieldId is expected to be smaller than @ref fieldCount() and
         * @p T is expected to correspond to @ref mappingType().
         *
         * You can also use the non-templated @ref mappingAsArray() accessor
         * (or the combined @ref parentsAsArray(),
         * @ref transformations2DAsArray(), @ref transformations3DAsArray(),
         * @ref translationsRotationsScalings2DAsArray(),
         * @ref translationsRotationsScalings3DAsArray(),
         * @ref meshesMaterialsAsArray(), @ref lightsAsArray(),
         * @ref camerasAsArray(), @ref skinsAsArray(),
         * @ref importerStateAsArray() accessors) to get the object mapping
         * converted to the usual type, but note that these operations involve
         * extra allocation and data conversion.
         * @see @ref mutableMapping(UnsignedInt)
         */
        template<class T> Containers::StridedArrayView1D<const T> mapping(UnsignedInt fieldId) const;

        /**
         * @brief Mutable object mapping for given field
         * @m_since_latest
         *
         * Like @ref mapping(UnsignedInt) const, but returns a mutable view.
         * Expects that the scene is mutable.
         * @see @ref dataFlags()
         */
        template<class T> Containers::StridedArrayView1D<T> mutableMapping(UnsignedInt fieldId);

        /**
         * @brief Object mapping data for given named field
         * @m_since_latest
         *
         * The @p fieldName is expected to exist. See
         * @ref mapping(UnsignedInt) const for more information. Use the
         * templated overload below to get the object mapping in a concrete
         * type.
         * @see @ref hasField(), @ref mutableMapping(SceneField)
         */
        Containers::StridedArrayView2D<const char> mapping(SceneField fieldName) const;

        /**
         * @brief Mutable object mapping data for given named field
         * @m_since_latest
         *
         * Like @ref mapping(SceneField) const, but returns a mutable view.
         * Expects that the scene is mutable.
         * @see @ref dataFlags()
         */
        Containers::StridedArrayView2D<char> mutableMapping(SceneField fieldName);

        /**
         * @brief Object mapping for given named field
         * @m_since_latest
         *
         * The @p fieldName is expected to exist and @p T is expected to
         * correspond to @ref mappingType().
         *
         * You can also use the non-templated @ref mappingAsArray() accessor
         * (or the combined @ref parentsAsArray(),
         * @ref transformations2DAsArray(), @ref transformations3DAsArray(),
         * @ref translationsRotationsScalings2DAsArray(),
         * @ref translationsRotationsScalings3DAsArray(),
         * @ref meshesMaterialsAsArray(), @ref lightsAsArray(),
         * @ref camerasAsArray(), @ref skinsAsArray(),
         * @ref importerStateAsArray() accessors) to get the object mapping
         * converted to the usual type, but note that these operations involve
         * extra allocation and data conversion.
         * @see @ref hasField(), @ref mapping(UnsignedInt) const,
         *      @ref mutableMapping(SceneField)
         */
        template<class T> Containers::StridedArrayView1D<const T> mapping(SceneField fieldName) const;

        /**
         * @brief Mutable object mapping for given named field
         * @m_since_latest
         *
         * Like @ref mapping(SceneField) const, but returns a mutable view.
         * Expects that the scene is mutable.
         * @see @ref dataFlags()
         */
        template<class T> Containers::StridedArrayView1D<T> mutableMapping(SceneField fieldName);

        /**
         * @brief Data for given field
         * @m_since_latest
         *
         * The @p id is expected to be smaller than @ref fieldCount(). The
         * second dimension represents the actual data type (its size is equal
         * to @ref SceneFieldType size, possibly multiplied by array size) and
         * is guaranteed to be contiguous. Use the templated overload below to
         * get the field in a concrete type. You can also use
         * @ref field(SceneField) const to directly get data for given named
         * field.
         *
         * For @ref SceneFieldType::Bit use @ref fieldBits() or
         * @ref fieldBitArrays() instead.
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
         * For @ref SceneFieldType::Bit use @ref fieldBits() or
         * @ref fieldBitArrays() instead.
         *
         * You can also use the non-templated @ref parentsAsArray(),
         * @ref transformations2DAsArray(), @ref transformations3DAsArray(),
         * @ref translationsRotationsScalings2DAsArray(),
         * @ref translationsRotationsScalings3DAsArray(),
         * @ref meshesMaterialsAsArray(), @ref lightsAsArray(),
         * @ref camerasAsArray(), @ref skinsAsArray(),
         * @ref importerStateAsArray() accessors to get common fields converted
         * to usual types, but note that these operations involve extra
         * allocation and data conversion.
         * @see @ref field(SceneField) const, @ref mutableField(UnsignedInt),
         *      @ref fieldArraySize()
         */
        template<class T
            #ifndef DOXYGEN_GENERATING_OUTPUT
            , typename std::enable_if<!std::is_array<T>::value, int>::type = 0
            #endif
        > Containers::StridedArrayView1D<const T> field(UnsignedInt id) const;

        /**
         * @brief Data for given array field in a concrete type
         * @m_since_latest
         *
         * Same as above, except that it works with array fields as well ---
         * you're expected to select this overload by passing @cpp T[] @ce
         * instead of @cpp T @ce. The second dimension is guaranteed to be
         * contiguous and have the same size as reported by
         * @ref fieldArraySize(UnsignedInt) const for given field. For
         * non-array fields the second dimension has a size of @cpp 1 @ce.
         */
        template<class T
            #ifndef DOXYGEN_GENERATING_OUTPUT
            , typename std::enable_if<std::is_array<T>::value, int>::type = 0
            #endif
        > Containers::StridedArrayView2D<const typename std::remove_extent<T>::type> field(UnsignedInt id) const;

        /**
         * @brief Mutable data for given field in a concrete type
         * @m_since_latest
         *
         * Like @ref field(UnsignedInt) const, but returns a mutable view.
         * Expects that the scene is mutable.
         * @see @ref dataFlags()
         */
        template<class T
            #ifndef DOXYGEN_GENERATING_OUTPUT
            , typename std::enable_if<!std::is_array<T>::value, int>::type = 0
            #endif
        > Containers::StridedArrayView1D<T> mutableField(UnsignedInt id);

        /**
         * @brief Mutable data for given array field in a concrete type
         * @m_since_latest
         *
         * Same as above, except that it works with array fields as well ---
         * you're expected to select this overload by passing @cpp T[] @ce
         * instead of @cpp T @ce. The second dimension is guaranteed to be
         * contiguous and have the same size as reported by
         * @ref fieldArraySize(UnsignedInt) const for given field. For
         * non-array fields the second dimension has a size of @cpp 1 @ce.
         */
        template<class T
            #ifndef DOXYGEN_GENERATING_OUTPUT
            , typename std::enable_if<std::is_array<T>::value, int>::type = 0
            #endif
        > Containers::StridedArrayView2D<typename std::remove_extent<T>::type> mutableField(UnsignedInt id);

        /**
         * @brief Data for given named field
         * @m_since_latest
         *
         * The @p name is expected to exist. See @ref field(UnsignedInt) const
         * for more information. Use the templated overload below to get the
         * field in a concrete type.
         * @see @ref hasField(), @ref mutableField(SceneField)
         */
        Containers::StridedArrayView2D<const char> field(SceneField name) const;

        /**
         * @brief Mutable data for given named field
         * @m_since_latest
         *
         * Like @ref field(SceneField) const, but returns a mutable view.
         * Expects that the scene is mutable.
         * @see @ref dataFlags()
         */
        Containers::StridedArrayView2D<char> mutableField(SceneField name);

        /**
         * @brief Data for given named field in a concrete type
         * @m_since_latest
         *
         * The @p name is expected to exist and @p T is expected to correspond
         * to @ref fieldType(SceneField) const. The field is also expected to
         * not be an array, in that case you need to use the overload below by
         * using @cpp T[] @ce instead of @cpp T @ce.
         *
         * You can also use the non-templated @ref parentsAsArray(),
         * @ref transformations2DAsArray(), @ref transformations3DAsArray(),
         * @ref translationsRotationsScalings2DAsArray(),
         * @ref translationsRotationsScalings3DAsArray(),
         * @ref meshesMaterialsAsArray(), @ref lightsAsArray(),
         * @ref camerasAsArray(), @ref skinsAsArray(),
         * @ref importerStateAsArray() accessors to get common fields converted
         * to usual types, but note that these operations involve extra
         * allocation and data conversion.
         * @see @ref hasField(), @ref field(UnsignedInt) const,
         *      @ref mutableField(SceneField)
         */
        template<class T
            #ifndef DOXYGEN_GENERATING_OUTPUT
            , typename std::enable_if<!std::is_array<T>::value, int>::type = 0
            #endif
        > Containers::StridedArrayView1D<const T> field(SceneField name) const;

        /**
         * @brief Data for given named array field in a concrete type
         * @m_since_latest
         *
         * Same as above, except that it works with array fields as well ---
         * you're expected to select this overload by passing @cpp T[] @ce
         * instead of @cpp T @ce. The second dimension is guaranteed to be
         * contiguous and have the same size as reported by
         * @ref fieldArraySize(SceneField) const for given field. For non-array
         * fields the second dimension has a size of @cpp 1 @ce.
         */
        template<class T
            #ifndef DOXYGEN_GENERATING_OUTPUT
            , typename std::enable_if<std::is_array<T>::value, int>::type = 0
            #endif
        > Containers::StridedArrayView2D<const typename std::remove_extent<T>::type> field(SceneField name) const;

        /**
         * @brief Mutable data for given named field
         * @m_since_latest
         *
         * Like @ref field(SceneField) const, but returns a mutable view.
         * Expects that the scene is mutable.
         * @see @ref dataFlags()
         */
        template<class T
            #ifndef DOXYGEN_GENERATING_OUTPUT
            , typename std::enable_if<!std::is_array<T>::value, int>::type = 0
            #endif
        > Containers::StridedArrayView1D<T> mutableField(SceneField name);

        /**
         * @brief Mutable data for given named array field in a concrete type
         * @m_since_latest
         *
         * Same as above, except that it works with array fields as well ---
         * you're expected to select this overload by passing @cpp T[] @ce
         * instead of @cpp T @ce. The second dimension is guaranteed to be
         * contiguous and have the same size as reported by
         * @ref fieldArraySize(SceneField) const for given field. For non-array
         * fields the second dimension has a size of @cpp 1 @ce.
         */
        template<class T
            #ifndef DOXYGEN_GENERATING_OUTPUT
            , typename std::enable_if<std::is_array<T>::value, int>::type = 0
            #endif
        > Containers::StridedArrayView2D<typename std::remove_extent<T>::type> mutableField(SceneField name);

        /**
         * @brief Contents of given bit field
         * @m_since_latest
         *
         * Expects that @p id is smaller than @ref fieldCount() const and that
         * the field is @ref SceneFieldType::Bit. The field is also expected to
         * not be an array, in that case you need to use
         * @ref fieldBitArrays(UnsignedInt) const instead.
         * @see @ref fieldType(UnsignedInt) const,
         *      @ref fieldArraySize(UnsignedInt) const
         */
        Containers::StridedBitArrayView1D fieldBits(UnsignedInt id) const;

        /**
         * @brief Contents of given array bit field
         * @m_since_latest
         *
         * Same as above, except that it works with array fields as well. The
         * second dimension is guaranteed to be contiguous and have the same
         * size as reported by @ref fieldArraySize(UnsignedInt) const for given
         * field. For non-array fields the second dimension has a size of
         * @cpp 1 @ce.
         */
        Containers::StridedBitArrayView2D fieldBitArrays(UnsignedInt id) const;

        /**
         * @brief Mutable contents of given bit field
         * @m_since_latest
         *
         * Like @ref fieldBits(UnsignedInt) const, but returns a mutable view.
         * Expects that the scene is mutable.
         * @see @ref dataFlags()
         */
        Containers::MutableStridedBitArrayView1D mutableFieldBits(UnsignedInt id);

        /**
         * @brief Mutable contents of given array bit field
         * @m_since_latest
         *
         * Same as above, except that it works with array fields as well. The
         * second dimension is guaranteed to be contiguous and have the same
         * size as reported by @ref fieldArraySize(UnsignedInt) const for given
         * field. For non-array fields the second dimension has a size of
         * @cpp 1 @ce.
         */
        Containers::MutableStridedBitArrayView2D mutableFieldBitArrays(UnsignedInt id);

        /**
         * @brief Contents of given named bit field
         * @m_since_latest
         *
         * Expects that @p name exists and that the field is
         * @ref SceneFieldType::Bit. The field is also expected to not be an
         * array, in that case you need to use
         * @ref fieldBitArrays(SceneField) const instead.
         * @see @ref hasField(), @ref fieldType(SceneField) const,
         *      @ref fieldArraySize(SceneField) const
         */
        Containers::StridedBitArrayView1D fieldBits(SceneField name) const;

        /**
         * @brief Contents of given array bit field
         * @m_since_latest
         *
         * Same as above, except that it works with array fields as well. The
         * second dimension is guaranteed to be contiguous and have the same
         * size as reported by @ref fieldArraySize(SceneField) const for given
         * field. For non-array fields the second dimension has a size of
         * @cpp 1 @ce.
         */
        Containers::StridedBitArrayView2D fieldBitArrays(SceneField name) const;

        /**
         * @brief Mutable contents of given bit field
         * @m_since_latest
         *
         * Like @ref fieldBits(SceneField) const, but returns a mutable view.
         * Expects that the scene is mutable.
         * @see @ref dataFlags()
         */
        Containers::MutableStridedBitArrayView1D mutableFieldBits(SceneField name);

        /**
         * @brief Mutable contents of given array bit field
         * @m_since_latest
         *
         * Same as above, except that it works with array fields as well. The
         * second dimension is guaranteed to be contiguous and have the same
         * size as reported by @ref fieldArraySize(SceneField) const for given
         * field. For non-array fields the second dimension has a size of
         * @cpp 1 @ce.
         */
        Containers::MutableStridedBitArrayView2D mutableFieldBitArrays(SceneField name);

        /**
         * @brief Base data pointer for given string field
         * @m_since_latest
         *
         * Raw string offsets and ranges returned from @ref field() are
         * relative to this pointer. For more convenient access use
         * @ref fieldStrings() instead. Expects that @p id is smaller than
         * @ref fieldCount() const and that the field is
         * @ref SceneFieldType::StringOffset32 "SceneFieldType::StringOffset*",
         * @ref SceneFieldType::StringRange32 "SceneFieldType::StringRange*" or
         * @ref SceneFieldType::StringRangeNullTerminated32 "SceneFieldType::StringRangeNullTerminated*".
         * You can also use @ref fieldStringData(SceneField) const to directly
         * get a data pointer for given named string field.
         * @see @ref fieldType(UnsignedInt) const
         */
        const char* fieldStringData(UnsignedInt id) const;

        /**
         * @brief Base data pointer for given named string field
         * @m_since_latest
         *
         * Expects that @p name exists and is
         * @ref SceneFieldType::StringOffset32 "SceneFieldType::StringOffset*",
         * @ref SceneFieldType::StringRange32 "SceneFieldType::StringRange*" or
         * @ref SceneFieldType::StringRangeNullTerminated32 "SceneFieldType::StringRangeNullTerminated*". See
         * @ref fieldStringData(UnsignedInt) const for more information.
         * @see @ref hasField()
         */
        const char* fieldStringData(SceneField name) const;

        /**
         * @brief Contents of given string field
         * @m_since_latest
         *
         * The returned views point to strings owned by this instance. If the
         * field has @ref SceneFieldFlag::NullTerminatedString set, the
         * returned views all have @relativeref{Corrade,Containers::StringViewFlag::NullTerminated}
         * set. Expects that @p id is smaller than @ref fieldCount() const and
         * that the field is
         * @ref SceneFieldType::StringOffset32 "SceneFieldType::StringOffset*",
         * @ref SceneFieldType::StringRange32 "SceneFieldType::StringRange*" or
         * @ref SceneFieldType::StringRangeNullTerminated32 "SceneFieldType::StringRangeNullTerminated*". You can also use
         * @ref fieldStrings(SceneField) const to directly get contents of
         * given named string field.
         *
         * The raw string data can be accessed using @ref fieldStringData() and
         * @ref field(). See a particular @ref SceneFieldType for information
         * about how to interpret the data.
         */
        Containers::StringIterable fieldStrings(UnsignedInt id) const;

        /**
         * @brief Contents of given string field
         * @m_since_latest
         *
         * Expects that @p name exists and that the field is
         * @ref SceneFieldType::StringOffset32 "SceneFieldType::StringOffset*",
         * @ref SceneFieldType::StringRange32 "SceneFieldType::StringRange*",
         * @ref SceneFieldType::StringRangeNullTerminated32 "SceneFieldType::StringRangeNullTerminated*". See
         * @ref fieldStrings(UnsignedInt) const for more information.
         * @see @ref hasField()
         */
        Containers::StringIterable fieldStrings(SceneField name) const;

        /**
         * @brief Object mapping for given field as 32-bit integers
         * @m_since_latest
         *
         * Convenience alternative to the templated
         * @ref mapping(UnsignedInt) const that converts the field from an
         * arbitrary underlying type and returns it in a newly-allocated array.
         * The @p fieldId is expected to be smaller than @ref fieldCount().
         *
         * Note that, for common fields, you can also use the
         * @ref parentsAsArray(), @ref transformations2DAsArray(),
         * @ref transformations3DAsArray(),
         * @ref translationsRotationsScalings2DAsArray(),
         * @ref translationsRotationsScalings3DAsArray(),
         * @ref meshesMaterialsAsArray(), @ref lightsAsArray(),
         * @ref camerasAsArray(), @ref skinsAsArray(),
         * @ref importerStateAsArray() accessors, which give out the object
         * mapping together with the field data.
         * @see @ref mappingInto(UnsignedInt, const Containers::StridedArrayView1D<UnsignedInt>&) const
         */
        Containers::Array<UnsignedInt> mappingAsArray(UnsignedInt fieldId) const;

        /**
         * @brief Object mapping for given field as 32-bit integers into a pre-allocated view
         * @m_since_latest
         *
         * Like @ref mappingAsArray(UnsignedInt) const, but puts the result
         * into @p destination instead of allocating a new array. Expects that
         * @p destination is sized to contain exactly all data.
         *
         * Note that, for common fields, you can also use the
         * @ref parentsInto(), @ref transformations2DInto(),
         * @ref transformations3DInto(),
         * @ref translationsRotationsScalings2DInto(),
         * @ref translationsRotationsScalings3DInto(),
         * @ref meshesMaterialsInto(), @ref lightsInto(), @ref camerasInto(),
         * @ref skinsInto(), @ref importerStateInto() accessors, which can give
         * out the object mapping together with the field data.
         *
         * @see @ref fieldSize(UnsignedInt) const
         */
        void mappingInto(UnsignedInt fieldId, const Containers::StridedArrayView1D<UnsignedInt>& destination) const;

        /**
         * @brief A subrange of object mapping for given field as 32-bit integers into a pre-allocated view
         * @m_since_latest
         *
         * Compared to @ref mappingInto(UnsignedInt, const Containers::StridedArrayView1D<UnsignedInt>&) const
         * extracts only a subrange of the object mapping defined by @p offset
         * and size of the @p destination view, returning the count of items
         * actually extracted. The @p offset is expected to not be larger than
         * the field size.
         *
         * Note that, for common fields, you can also use the
         * @ref parentsInto(), @ref transformations2DInto(),
         * @ref transformations3DInto(),
         * @ref translationsRotationsScalings2DInto(),
         * @ref translationsRotationsScalings3DInto(),
         * @ref meshesMaterialsInto(), @ref lightsInto(), @ref camerasInto(),
         * @ref skinsInto(), @ref importerStateInto() accessors, which can give
         * out the object mapping together with the field data.
         *
         * @see @ref fieldSize(UnsignedInt) const,
         *      @ref fieldObjectOffset(UnsignedInt, UnsignedLong, std::size_t) const
         */
        std::size_t mappingInto(UnsignedInt fieldId, std::size_t offset, const Containers::StridedArrayView1D<UnsignedInt>& destination) const;

        /**
         * @brief Object mapping for given named field as 32-bit integers
         * @m_since_latest
         *
         * Convenience alternative to the templated
         * @ref mapping(SceneField) const that converts the field from an
         * arbitrary underlying type and returns it in a newly-allocated array.
         * The @p fieldName is expected to exist.
         *
         * Note that, for common fields, you can also use the
         * @ref parentsAsArray(), @ref transformations2DAsArray(),
         * @ref transformations3DAsArray(),
         * @ref translationsRotationsScalings2DAsArray(),
         * @ref translationsRotationsScalings3DAsArray(),
         * @ref meshesMaterialsAsArray(), @ref lightsAsArray(),
         * @ref camerasAsArray(), @ref skinsAsArray(),
         * @ref importerStateAsArray() accessors, which give out the object
         * mapping together with the field data.
         * @see @ref mappingInto(SceneField, const Containers::StridedArrayView1D<UnsignedInt>&) const,
         *      @ref hasField()
         */
        Containers::Array<UnsignedInt> mappingAsArray(SceneField fieldName) const;

        /**
         * @brief Object mapping for given named field as 32-bit integers into a pre-allocated view
         * @m_since_latest
         *
         * Like @ref mappingAsArray(SceneField) const, but puts the result into
         * @p destination instead of allocating a new array. Expects that
         * @p destination is sized to contain exactly all data.
         *
         * Note that, for common fields, you can also use the
         * @ref parentsInto(), @ref transformations2DInto(),
         * @ref transformations3DInto(),
         * @ref translationsRotationsScalings2DInto(),
         * @ref translationsRotationsScalings3DInto(),
         * @ref meshesMaterialsInto(), @ref lightsInto(), @ref camerasInto(),
         * @ref skinsInto(), @ref importerStateInto() accessors, which can give
         * out the object mapping together with the field data.
         *
         * @see @ref fieldSize(SceneField) const
         */
        void mappingInto(SceneField fieldName, const Containers::StridedArrayView1D<UnsignedInt>& destination) const;

        /**
         * @brief A subrange of object mapping for given named field as 32-bit integers into a pre-allocated view
         * @m_since_latest
         *
         * Compared to @ref mappingInto(SceneField, const Containers::StridedArrayView1D<UnsignedInt>&) const
         * extracts only a subrange of the object mapping defined by @p offset
         * and size of the @p destination view, returning the count of items
         * actually extracted. The @p offset is expected to not be larger than
         * the field size.
         *
         * Note that, for common fields, you can also use the
         * @ref parentsInto(), @ref transformations2DInto(),
         * @ref transformations3DInto(),
         * @ref translationsRotationsScalings2DInto(),
         * @ref translationsRotationsScalings3DInto(),
         * @ref meshesMaterialsInto(), @ref lightsInto(), @ref camerasInto(),
         * @ref skinsInto(), @ref importerStateInto() accessors, which can give
         * out the object mapping together with the field data.
         *
         * @see @ref fieldSize(SceneField) const,
         *      @ref fieldObjectOffset(SceneField, UnsignedLong, std::size_t) const
         */
        std::size_t mappingInto(SceneField fieldName, std::size_t offset, const Containers::StridedArrayView1D<UnsignedInt>& destination) const;

        /**
         * @brief Parent indices as 32-bit integers
         * @return Pairs of (object mapping, parent ID)
         * @m_since_latest
         *
         * Convenience alternative to @ref mapping(SceneField) const together
         * with @ref field(SceneField) const with @ref SceneField::Parent as
         * the argument. Converts the object mapping and the field from
         * arbitrary underlying types and returns them in a newly-allocated
         * array. The field is expected to exist.
         * @see @ref parentsInto(), @ref hasField(), @ref parentFor(),
         *      @ref childrenFor()
         */
        Containers::Array<Containers::Pair<UnsignedInt, Int>> parentsAsArray() const;

        /**
         * @brief Parent indices as 32-bit integers into a pre-allocated view
         * @m_since_latest
         *
         * Like @ref parentsAsArray(), but puts the result into
         * @p mappingDestination and @p fieldDestination instead of allocating
         * a new array. Expects that each view is either @cpp nullptr @ce or
         * sized to contain exactly all data. If @p fieldDestination is
         * @cpp nullptr @ce, the effect is the same as calling
         * @ref mappingInto() with @ref SceneField::Parent.
         * @see @ref fieldSize(SceneField) const
         */
        void parentsInto(const Containers::StridedArrayView1D<UnsignedInt>& mappingDestination, const Containers::StridedArrayView1D<Int>& fieldDestination) const;

        /**
         * @brief A subrange of parent indices as 32-bit integers into a pre-allocated view
         * @m_since_latest
         *
         * Compared to @ref parentsInto(const Containers::StridedArrayView1D<UnsignedInt>&, const Containers::StridedArrayView1D<Int>&) const
         * extracts only a subrange of the field defined by @p offset and size
         * of the views, returning the count of items actually extracted. The
         * @p offset is expected to not be larger than the field size, views
         * that are not @cpp nullptr @ce are expected to have the same size.
         * @see @ref fieldSize(SceneField) const,
         *      @ref fieldObjectOffset(SceneField, UnsignedLong, std::size_t) const
         */
        std::size_t parentsInto(std::size_t offset, const Containers::StridedArrayView1D<UnsignedInt>& mappingDestination, const Containers::StridedArrayView1D<Int>& fieldDestination) const;

        /**
         * @brief Transformation field size
         * @m_since_latest
         *
         * Returns the size of the @ref SceneField::Transformation field, or,
         * if not present, of any of the @ref SceneField::Translation,
         * @ref SceneField::Rotation and @ref SceneField::Scaling fields that's
         * present. Expects that at least one transformation field is present.
         * @see @ref is2D(), @ref is3D(), @ref fieldSize(SceneField) const
         */
        std::size_t transformationFieldSize() const;

        /**
         * @brief 2D transformations as 3x3 float matrices
         * @return Pairs of (object mapping, transformation)
         * @m_since_latest
         *
         * Convenience alternative to @ref mapping(SceneField) const together
         * with @ref field(SceneField) const with
         * @ref SceneField::Transformation as the argument, or, if not present,
         * to a matrix created out of a subset of the
         * @ref SceneField::Translation, @ref SceneField::Rotation and
         * @ref SceneField::Scaling fields that's present. Converts the object
         * mapping and the fields from arbitrary underlying types and returns
         * them in a newly-allocated array. At least one of the fields is
         * expected to exist and they are expected to have a type corresponding
         * to 2D, otherwise you're supposed to use
         * @ref transformations3DAsArray().
         * @see @ref is2D(), @ref transformations2DInto(), @ref hasField(),
         *      @ref fieldType(SceneField) const, @ref transformation2DFor()
         */
        Containers::Array<Containers::Pair<UnsignedInt, Matrix3>> transformations2DAsArray() const;

        /**
         * @brief 2D transformations as 3x3 float matrices into a pre-allocated view
         * @m_since_latest
         *
         * Like @ref transformations2DAsArray(), but puts the result into
         * @p mappingDestination and @p fieldDestination instead of allocating
         * a new array. Expects that each view is either @cpp nullptr @ce or
         * sized to contain exactly all data. If @p fieldDestination is
         * @cpp nullptr @ce, the effect is the same as calling
         * @ref mappingInto() with the first of the
         * @ref SceneField::Transformation, @ref SceneField::Translation,
         * @ref SceneField::Rotation and @ref SceneField::Scaling fields that's
         * present.
         * @see @ref transformationFieldSize()
         */
        void transformations2DInto(const Containers::StridedArrayView1D<UnsignedInt>& mappingDestination, const Containers::StridedArrayView1D<Matrix3>& fieldDestination) const;

        /**
         * @brief A subrange of 2D transformations as 3x3 float matrices into a pre-allocated view
         * @m_since_latest
         *
         * Compared to @ref transformations2DInto(const Containers::StridedArrayView1D<UnsignedInt>&, const Containers::StridedArrayView1D<Matrix3>&) const
         * extracts only a subrange of the field defined by @p offset and size
         * of the views, returning the count of items actually extracted. The
         * @p offset is expected to not be larger than the field size, views
         * that are not @cpp nullptr @ce are expected to have the same size.
         * @see @ref transformationFieldSize(),
         *      @ref fieldObjectOffset(SceneField, UnsignedLong, std::size_t) const
         */
        std::size_t transformations2DInto(std::size_t offset, const Containers::StridedArrayView1D<UnsignedInt>& mappingDestination, const Containers::StridedArrayView1D<Matrix3>& fieldDestination) const;

        /**
         * @brief 2D transformations as float translation, rotation and scaling components
         * @return Pairs of (object mapping, (translation, rotation, scaling))
         * @m_since_latest
         *
         * Convenience alternative to @ref mapping(SceneField) const together
         * with @ref field(SceneField) const with @ref SceneField::Translation,
         * @ref SceneField::Rotation and @ref SceneField::Scaling as the
         * arguments, as these are required to share the same object mapping.
         * Converts the object mapping and the fields from arbitrary underlying
         * types and returns them in a newly-allocated array. At least one of
         * the fields is expected to exist and they are expected to have a type
         * corresponding to 2D, otherwise you're supposed to use
         * @ref translationsRotationsScalings3DAsArray(). If the
         * @ref SceneField::Translation field isn't present, the first returned
         * value is a zero vector. If the @relativeref{SceneField,Rotation}
         * field isn't present, the second value is an identity rotation. If
         * the @relativeref{SceneField,Scaling} field isn't present, the third
         * value is an identity scaling (@cpp 1.0f @ce in both dimensions).
         * @see @ref is2D(), @ref translationsRotationsScalings2DInto(),
         *      @ref hasField(), @ref fieldType(SceneField) const,
         *      @ref translationRotationScaling2DFor()
         */
        Containers::Array<Containers::Pair<UnsignedInt, Containers::Triple<Vector2, Complex, Vector2>>> translationsRotationsScalings2DAsArray() const;

        /**
         * @brief 2D transformations as float translation, rotation and scaling components into a pre-allocated view
         * @m_since_latest
         *
         * Like @ref translationsRotationsScalings2DAsArray(), but puts the
         * result into @p mappingDestination, @p translationDestination,
         * @p rotationDestination and @p scalingDestination instead of
         * allocating a new array. Expects that each view is either
         * @cpp nullptr @ce or sized to contain exactly all data. If
         * @p translationDestination, @p rotationDestination and
         * @p scalingDestination are all @cpp nullptr @ce, the effect is the
         * same as calling @ref mappingInto() with one of the
         * @ref SceneField::Translation, @ref SceneField::Rotation and
         * @ref SceneField::Scaling fields that's present.
         * @see @ref transformationFieldSize()
         */
        void translationsRotationsScalings2DInto(const Containers::StridedArrayView1D<UnsignedInt>& mappingDestination, const Containers::StridedArrayView1D<Vector2>& translationDestination, const Containers::StridedArrayView1D<Complex>& rotationDestination, const Containers::StridedArrayView1D<Vector2>& scalingDestination) const;

        /**
         * @brief A subrange of 2D transformations as float translation, rotation and scaling components into a pre-allocated view
         * @m_since_latest
         *
         * Compared to @ref translationsRotationsScalings2DInto(const Containers::StridedArrayView1D<UnsignedInt>&, const Containers::StridedArrayView1D<Vector2>&, const Containers::StridedArrayView1D<Complex>&, const Containers::StridedArrayView1D<Vector2>&) const
         * extracts only a subrange of the field defined by @p offset and size
         * of the views, returning the count of items actually extracted. The
         * @p offset is expected to not be larger than the field size, views
         * that are not @cpp nullptr @ce are expected to have the same size.
         * @see @ref transformationFieldSize(),
         *      @ref fieldObjectOffset(SceneField, UnsignedLong, std::size_t) const
         */
        std::size_t translationsRotationsScalings2DInto(std::size_t offset, const Containers::StridedArrayView1D<UnsignedInt>& mappingDestination, const Containers::StridedArrayView1D<Vector2>& translationDestination, const Containers::StridedArrayView1D<Complex>& rotationDestination, const Containers::StridedArrayView1D<Vector2>& scalingDestination) const;

        /**
         * @brief 3D transformations as 4x4 float matrices
         * @return Pairs of (object mapping, transformation)
         * @m_since_latest
         *
         * Convenience alternative to @ref mapping(SceneField) const together
         * with @ref field(SceneField) const with
         * @ref SceneField::Transformation as the argument, or, if not present,
         * to a matrix created out of a subset of the
         * @ref SceneField::Translation, @ref SceneField::Rotation and
         * @ref SceneField::Scaling fields that's present. Converts the object
         * mapping and the fields from arbitrary underlying types and returns
         * them in a newly-allocated array. At least one of the fields is
         * expected to exist and they are expected to have a type corresponding
         * to 3D, otherwise you're supposed to use
         * @ref transformations2DAsArray().
         * @see @ref is3D(), @ref transformations3DInto(), @ref hasField(),
         *      @ref fieldType(SceneField) const, @ref transformation3DFor()
         */
        Containers::Array<Containers::Pair<UnsignedInt, Matrix4>> transformations3DAsArray() const;

        /**
         * @brief 3D transformations as 4x4 float matrices into a pre-allocated view
         * @m_since_latest
         *
         * Like @ref transformations3DAsArray(), but puts the result into
         * @p mappingDestination and @p fieldDestination instead of allocating
         * a new array. Expects that the two views are either @cpp nullptr @ce
         * or sized to contain exactly all data. If @p fieldDestination is
         * @cpp nullptr @ce, the effect is the same as calling
         * @ref mappingInto() with the first of the
         * @ref SceneField::Transformation, @ref SceneField::Translation,
         * @ref SceneField::Rotation and @ref SceneField::Scaling fields that's
         * present.
         * @see @ref transformationFieldSize()
         */
        void transformations3DInto(const Containers::StridedArrayView1D<UnsignedInt>& mappingDestination, const Containers::StridedArrayView1D<Matrix4>& destination) const;

        /**
         * @brief A subrange of 3D transformations as 4x4 float matrices into a pre-allocated view
         * @m_since_latest
         *
         * Compared to @ref transformations3DInto(const Containers::StridedArrayView1D<UnsignedInt>&, const Containers::StridedArrayView1D<Matrix4>&) const
         * extracts only a subrange of the field defined by @p offset and size
         * of the views, returning the count of items actually extracted. The
         * @p offset is expected to not be larger than the field size, views
         * that are not @cpp nullptr @ce are expected to have the same size.
         * @see @ref transformationFieldSize(),
         *      @ref fieldObjectOffset(SceneField, UnsignedLong, std::size_t) const
         */
        std::size_t transformations3DInto(std::size_t offset, const Containers::StridedArrayView1D<UnsignedInt>& mappingDestination, const Containers::StridedArrayView1D<Matrix4>& destination) const;

        /**
         * @brief 3D transformations as float translation, rotation and scaling components
         * @return Pairs of (object mapping, (translation, rotation, scaling))
         * @m_since_latest
         *
         * Convenience alternative to @ref mapping(SceneField) const together
         * with @ref field(SceneField) const with @ref SceneField::Translation,
         * @ref SceneField::Rotation and @ref SceneField::Scaling as the
         * arguments, as these are required to share the same object mapping.
         * Converts the object mapping and the fields from arbitrary underlying
         * types and returns them in a newly-allocated array. At least one of
         * the fields is expected to exist and they are expected to have a type
         * corresponding to 3D, otherwise you're supposed to use
         * @ref translationsRotationsScalings2DAsArray(). If the
         * @ref SceneField::Translation field isn't present, the first returned
         * value is a zero vector. If the @relativeref{SceneField,Rotation}
         * field isn't present, the second value is an identity rotation. If
         * the @relativeref{SceneField,Scaling} field isn't present, the third
         * value is an identity scaling (@cpp 1.0f @ce in all dimensions).
         * @see @ref is3D(), @ref translationsRotationsScalings3DInto(),
         *      @ref hasField(), @ref fieldType(SceneField) const,
         *      @ref translationRotationScaling3DFor()
         */
        Containers::Array<Containers::Pair<UnsignedInt, Containers::Triple<Vector3, Quaternion, Vector3>>> translationsRotationsScalings3DAsArray() const;

        /**
         * @brief 3D transformations as float translation, rotation and scaling components into a pre-allocated view
         * @m_since_latest
         *
         * Like @ref translationsRotationsScalings3DAsArray(), but puts the
         * result into @p mappingDestination, @p translationDestination,
         * @p rotationDestination and @p scalingDestination instead of
         * allocating a new array. Expects that each view is either
         * @cpp nullptr @ce or sized to contain exactly all data. If
         * @p translationDestination, @p rotationDestination and
         * @p scalingDestination are all @cpp nullptr @ce, the effect is the
         * same as calling @ref mappingInto() with one of the
         * @ref SceneField::Translation, @ref SceneField::Rotation and
         * @ref SceneField::Scaling fields that's present.
         * @see @ref transformationFieldSize()
         */
        void translationsRotationsScalings3DInto(const Containers::StridedArrayView1D<UnsignedInt>& mappingDestination, const Containers::StridedArrayView1D<Vector3>& translationDestination, const Containers::StridedArrayView1D<Quaternion>& rotationDestination, const Containers::StridedArrayView1D<Vector3>& scalingDestination) const;

        /**
         * @brief A subrange of 3D transformations as float translation, rotation and scaling components into a pre-allocated view
         * @m_since_latest
         *
         * Compared to @ref translationsRotationsScalings3DInto(const Containers::StridedArrayView1D<UnsignedInt>&, const Containers::StridedArrayView1D<Vector3>&, const Containers::StridedArrayView1D<Quaternion>&, const Containers::StridedArrayView1D<Vector3>&) const
         * extracts only a subrange of the field defined by @p offset and size
         * of the views, returning the count of items actually extracted. The
         * @p offset is expected to not be larger than the field size, views
         * that are not @cpp nullptr @ce are expected to have the same size.
         * @see @ref transformationFieldSize(),
         *      @ref fieldObjectOffset(SceneField, UnsignedLong, std::size_t) const
         */
        std::size_t translationsRotationsScalings3DInto(std::size_t offset, const Containers::StridedArrayView1D<UnsignedInt>& mappingDestination, const Containers::StridedArrayView1D<Vector3>& translationDestination, const Containers::StridedArrayView1D<Quaternion>& rotationDestination, const Containers::StridedArrayView1D<Vector3>& scalingDestination) const;

        /**
         * @brief Mesh and material IDs as 32-bit integers
         * @return Pairs of (object mapping, (mesh ID, material ID))
         * @m_since_latest
         *
         * Convenience alternative to @ref mapping(SceneField) const together
         * with @ref field(SceneField) const with @ref SceneField::Mesh and
         * @ref SceneField::MeshMaterial as the argument, as the two are
         * required to share the same object mapping. Converts the object
         * mapping and the fields from arbitrary underlying types and returns
         * them in a newly-allocated array. The @ref SceneField::Mesh field is
         * expected to exist, if @ref SceneField::MeshMaterial isn't present,
         * the second returned values are all @cpp -1 @ce.
         * @see @ref meshesMaterialsInto(), @ref hasField(),
         *      @ref meshesMaterialsFor()
         */
        Containers::Array<Containers::Pair<UnsignedInt, Containers::Pair<UnsignedInt, Int>>> meshesMaterialsAsArray() const;

        /**
         * @brief Mesh and material IDs as 32-bit integers into a pre-allocated view
         * @m_since_latest
         *
         * Like @ref meshesMaterialsAsArray(), but puts the results into
         * @p mappingDestination, @p meshDestination and
         * @p meshMaterialDestination instead of allocating a new array.
         * Expects that each view is either @cpp nullptr @ce or sized to
         * contain exactly all data. If @p meshDestination and
         * @p meshMaterialDestination are both @cpp nullptr @ce, the effect is
         * the same as calling @ref mappingInto() with @ref SceneField::Mesh.
         * @see @ref fieldSize(SceneField) const
         */
        void meshesMaterialsInto(const Containers::StridedArrayView1D<UnsignedInt>& mappingDestination, const Containers::StridedArrayView1D<UnsignedInt>& meshDestination, const Containers::StridedArrayView1D<Int>& meshMaterialDestination) const;

        /**
         * @brief A subrange of mesh and material IDs as 32-bit integers into a pre-allocated view
         * @m_since_latest
         *
         * Compared to @ref meshesMaterialsInto(const Containers::StridedArrayView1D<UnsignedInt>&, const Containers::StridedArrayView1D<UnsignedInt>&, const Containers::StridedArrayView1D<Int>&) const
         * extracts only a subrange of the field defined by @p offset and size
         * of the views, returning the count of items actually extracted. The
         * @p offset is expected to not be larger than the field size, views
         * that are not @cpp nullptr @ce are expected to have the same size.
         * @see @ref fieldSize(SceneField) const,
         *      @ref fieldObjectOffset(SceneField, UnsignedLong, std::size_t) const
         */
        std::size_t meshesMaterialsInto(std::size_t offset, const Containers::StridedArrayView1D<UnsignedInt>& mappingDestination, const Containers::StridedArrayView1D<UnsignedInt>& meshDestination, const Containers::StridedArrayView1D<Int>& meshMaterialsDestination) const;

        /**
         * @brief Light IDs as 32-bit integers
         * @return Pairs of (object mapping, light ID)
         * @m_since_latest
         *
         * Convenience alternative to @ref mapping(SceneField) const together
         * with @ref field(SceneField) const with @ref SceneField::Light as the
         * argument. Converts the object mapping and the field from arbitrary
         * underlying types and returns them in a newly-allocated array. The
         * field is expected to exist.
         * @see @ref lightsInto(), @ref hasField(), @ref lightsFor()
         */
        Containers::Array<Containers::Pair<UnsignedInt, UnsignedInt>> lightsAsArray() const;

        /**
         * @brief Light IDs as 32-bit integers into a pre-allocated view
         * @m_since_latest
         *
         * Like @ref lightsAsArray(), but puts the result into
         * @p mappingDestination and @p fieldDestination instead of allocating
         * a new array. Expects that each view is either @cpp nullptr @ce or
         * sized to contain exactly all data. If @p fieldDestination is
         * @cpp nullptr @ce, the effect is the same as calling
         * @ref lightsInto() with @ref SceneField::Light.
         * @see @ref fieldSize(SceneField) const
         */
        void lightsInto(const Containers::StridedArrayView1D<UnsignedInt>& mappingDestination, const Containers::StridedArrayView1D<UnsignedInt>& fieldDestination) const;

        /**
         * @brief A subrange of light IDs as 32-bit integers into a pre-allocated view
         * @m_since_latest
         *
         * Compared to @ref lightsInto(const Containers::StridedArrayView1D<UnsignedInt>&, const Containers::StridedArrayView1D<UnsignedInt>&) const
         * extracts only a subrange of the field defined by @p offset and size
         * of the views, returning the count of items actually extracted. The
         * @p offset is expected to not be larger than the field size, views
         * that are not @cpp nullptr @ce are expected to have the same size.
         * @see @ref fieldSize(SceneField) const,
         *      @ref fieldObjectOffset(SceneField, UnsignedLong, std::size_t) const
         */
        std::size_t lightsInto(std::size_t offset, const Containers::StridedArrayView1D<UnsignedInt>& mappingDestination, const Containers::StridedArrayView1D<UnsignedInt>& fieldDestination) const;

        /**
         * @brief Camera IDs as 32-bit integers
         * @return Pairs of (object mapping, camera ID)
         * @m_since_latest
         *
         * Convenience alternative to @ref mapping(SceneField) const together
         * with @ref field(SceneField) const with @ref SceneField::Camera as
         * the argument. Converts the object mapping and the field from
         * arbitrary underlying types and returns them in a newly-allocated
         * array. The field is expected to exist.
         * @see @ref camerasInto(), @ref hasField(), @ref camerasFor()
         */
        Containers::Array<Containers::Pair<UnsignedInt, UnsignedInt>> camerasAsArray() const;

        /**
         * @brief Camera IDs as 32-bit integers into a pre-allocated view
         * @m_since_latest
         *
         * Like @ref camerasAsArray(), but puts the result into
         * @p mappingDestination and @p fieldDestination instead of allocating
         * a new array. Expects that each view is either @cpp nullptr @ce or
         * sized to contain exactly all data. If @p fieldDestination is
         * @cpp nullptr @ce, the effect is the same as calling
         * @ref mappingInto() with @ref SceneField::Camera.
         * @see @ref fieldSize(SceneField) const
         */
        void camerasInto(const Containers::StridedArrayView1D<UnsignedInt>& mappingDestination, const Containers::StridedArrayView1D<UnsignedInt>& fieldDestination) const;

        /**
         * @brief A subrange of camera IDs as 32-bit integers into a pre-allocated view
         * @m_since_latest
         *
         * Compared to @ref camerasInto(const Containers::StridedArrayView1D<UnsignedInt>&, const Containers::StridedArrayView1D<UnsignedInt>&) const
         * extracts only a subrange of the field defined by @p offset and size
         * of the views, returning the count of items actually extracted. The
         * @p offset is expected to not be larger than the field size, views
         * that are not @cpp nullptr @ce are expected to have the same size.
         * @see @ref fieldSize(SceneField) const,
         *      @ref fieldObjectOffset(SceneField, UnsignedLong, std::size_t) const
         */
        std::size_t camerasInto(std::size_t offset, const Containers::StridedArrayView1D<UnsignedInt>& mappingDestination, const Containers::StridedArrayView1D<UnsignedInt>& fieldDestination) const;

        /**
         * @brief Skin IDs as 32-bit integers
         * @return Pairs of (object mapping, skin ID)
         * @m_since_latest
         *
         * Convenience alternative to @ref mapping(SceneField) const together
         * with @ref field(SceneField) const with @ref SceneField::Skin as the
         * argument. Converts the object mapping and the field from arbitrary
         * underlying types and returns them in a newly-allocated array. The
         * field is expected to exist.
         * @see @ref skinsInto(), @ref hasField(), @ref skinsFor()
         */
        Containers::Array<Containers::Pair<UnsignedInt, UnsignedInt>> skinsAsArray() const;

        /**
         * @brief Skin IDs as 32-bit integers into a pre-allocated view
         * @m_since_latest
         *
         * Like @ref skinsAsArray(), but puts the result into
         * @p mappingDestination and @p fieldDestination instead of allocating a
         * new array. Expects that each view is either @cpp nullptr @ce or
         * sized to contain exactly all data. If @p fieldDestination is
         * @cpp nullptr @ce, the effect is the same as calling
         * @ref mappingInto() with @ref SceneField::Skin.
         * @see @ref fieldSize(SceneField) const
         */
        void skinsInto(const Containers::StridedArrayView1D<UnsignedInt>& mappingDestination, const Containers::StridedArrayView1D<UnsignedInt>& fieldDestination) const;

        /**
         * @brief A subrange of skin IDs as 32-bit integers into a pre-allocated view
         * @m_since_latest
         *
         * Compared to @ref skinsInto(const Containers::StridedArrayView1D<UnsignedInt>&, const Containers::StridedArrayView1D<UnsignedInt>&) const
         * extracts only a subrange of the field defined by @p offset and size
         * of the views, returning the count of items actually extracted. The
         * @p offset is expected to not be larger than the field size, views
         * that are not @cpp nullptr @ce are expected to have the same size.
         * @see @ref fieldSize(SceneField) const,
         *      @ref fieldObjectOffset(SceneField, UnsignedLong, std::size_t) const
         */
        std::size_t skinsInto(std::size_t offset, const Containers::StridedArrayView1D<UnsignedInt>& mappingDestination, const Containers::StridedArrayView1D<UnsignedInt>& fieldDestination) const;

        /**
         * @brief Per-object importer state as `void` pointers
         * @return Pairs of (object mapping, importer state)
         * @m_since_latest
         *
         * Convenience alternative to @ref mapping(SceneField) const together
         * with @ref field(SceneField) const with @ref SceneField::ImporterState
         * as the argument. Converts the object mapping and the field from
         * arbitrary underlying types and returns them in a newly-allocated
         * array. The field is expected to exist.
         *
         * This is different from @ref importerState(), which returns importer
         * state for the scene itself, not particular objects.
         * @see @ref importerStateInto(), @ref hasField(), @ref importerStateFor()
         */
        Containers::Array<Containers::Pair<UnsignedInt, const void*>> importerStateAsArray() const;

        /**
         * @brief Per-object importer state as `void` pointers into a pre-allocated view
         * @m_since_latest
         *
         * Like @ref importerStateAsArray(), but puts the result into
         * @p mappingDestination and @p fieldDestination instead of allocating
         * a new array. Expects that each view is either @cpp nullptr @ce or
         * sized to contain exactly all data. If @p fieldDestination is
         * @cpp nullptr @ce, the effect is the same as calling
         * @ref mappingInto() with @ref SceneField::ImporterState.
         * @see @ref fieldSize(SceneField) const
         */
        void importerStateInto(const Containers::StridedArrayView1D<UnsignedInt>& mappingDestination, const Containers::StridedArrayView1D<const void*>& fieldDestination) const;

        /**
         * @brief A subrange of per-object importer state as `void` pointers into a pre-allocated view
         * @m_since_latest
         *
         * Compared to @ref importerStateInto(const Containers::StridedArrayView1D<UnsignedInt>&, const Containers::StridedArrayView1D<const void*>&) const
         * extracts only a subrange of the field defined by @p offset and size
         * of the views, returning the count of items actually extracted. The
         * @p offset is expected to not be larger than the field size, views
         * that are not @cpp nullptr @ce are expected to have the same size.
         * @see @ref fieldSize(SceneField) const,
         *      @ref fieldObjectOffset(SceneField, UnsignedLong, std::size_t) const
         */
        std::size_t importerStateInto(std::size_t offset, const Containers::StridedArrayView1D<UnsignedInt>& mappingDestination, const Containers::StridedArrayView1D<const void*>& fieldDestination) const;

        /**
         * @brief Parent for given object
         * @m_since_latest
         *
         * Looks up the @ref SceneField::Parent field for @p object
         * equivalently to @ref findFieldObjectOffset(SceneField, UnsignedLong, std::size_t) const
         * and then converts the field from an arbitrary underlying type the
         * same way as @ref parentsAsArray(). See the lookup function
         * documentation for operation complexity --- for retrieving parent
         * info for many objects it's recommended to access the field data
         * directly.
         *
         * If the @ref SceneField::Parent field is not present or if there's no
         * parent for @p object, returns @relativeref{Corrade,Containers::NullOpt}.
         * If @p object is top-level, returns @cpp -1 @ce.
         *
         * The @p object is expected to be less than @ref mappingBound().
         * @see @ref childrenFor()
         */
        Containers::Optional<Long> parentFor(UnsignedLong object) const;

        /**
         * @brief Children for given object
         * @m_since_latest
         *
         * Looks up @p object in the object mapping array for
         * @ref SceneField::Parent equivalently to @ref findFieldObjectOffset(SceneField, UnsignedLong, std::size_t) const,
         * converts the fields from an arbitrary underlying type the same way
         * as @ref parentsAsArray(), returning a list of all object IDs that
         * have it listed as the parent. See the lookup function documentation
         * for operation complexity --- for retrieving parent/child info for
         * many objects it's recommended to access the field data directly.
         *
         * If the @ref SceneField::Parent field doesn't exist or there are no
         * objects which would have @p object listed as their parent, returns
         * an empty array. Pass @cpp -1 @ce to get a list of top-level objects.
         *
         * The @p object is expected to be less than @ref mappingBound().
         * @see @ref parentFor()
         */
        Containers::Array<UnsignedLong> childrenFor(Long object) const;

        /**
         * @brief 2D transformation for given object
         * @m_since_latest
         *
         * Looks up the @ref SceneField::Transformation field for @p object
         * equivalently to @ref findFieldObjectOffset(SceneField, UnsignedLong, std::size_t) const
         * or combines it from a @ref SceneField::Translation,
         * @relativeref{SceneField,Rotation} and
         * @relativeref{SceneField,Scaling}, converting the fields from
         * arbitrary underlying types the same way as
         * @ref transformations2DAsArray(). See the lookup function
         * documentation for operation complexity --- for retrieving
         * transformation info for many objects it's recommended to access the
         * field data directly.
         *
         * If neither @ref SceneField::Transformation nor any of
         * @ref SceneField::Translation, @relativeref{SceneField,Rotation} or
         * @relativeref{SceneField,Scaling} is present, the fields represent a
         * 3D transformation or there's no transformation for @p object,
         * returns @relativeref{Corrade,Containers::NullOpt}.
         *
         * The @p object is expected to be less than @ref mappingBound().
         * @see @ref translationRotationScaling2DFor()
         */
        Containers::Optional<Matrix3> transformation2DFor(UnsignedLong object) const;

        /**
         * @brief 2D translation, rotation and scaling for given object
         * @m_since_latest
         *
         * Looks up the @ref SceneField::Translation,
         * @relativeref{SceneField,Rotation} and
         * @relativeref{SceneField,Scaling} fields for @p object equivalently
         * to @ref findFieldObjectOffset(SceneField, UnsignedLong, std::size_t) const
         * and then converts the fields from arbitrary underlying types the
         * same way as @ref translationsRotationsScalings2DAsArray(). See the
         * lookup function documentation for operation complexity --- for
         * retrieving transformation info for many objects it's recommended to
         * access the field data directly.
         *
         * If the @ref SceneField::Translation field isn't present, the first
         * returned value is a zero vector. If the
         * @relativeref{SceneField,Rotation} field isn't present, the second
         * value is an identity rotation. If the @relativeref{SceneField,Scaling}
         * field isn't present, the third value is an identity scaling
         * (@cpp 1.0f @ce in both dimensions). If neither of those fields is
         * present, if any of the fields represents a 3D transformation or if
         * there's no transformation for @p object, returns
         * @relativeref{Corrade,Containers::NullOpt}.
         *
         * The @p object is expected to be less than @ref mappingBound().
         * @see @ref transformation2DFor()
         */
        Containers::Optional<Containers::Triple<Vector2, Complex, Vector2>> translationRotationScaling2DFor(UnsignedLong object) const;

        /**
         * @brief 3D transformation for given object
         * @m_since_latest
         *
         * Looks up the @ref SceneField::Transformation field for @p object
         * equivalently to @ref findFieldObjectOffset(SceneField, UnsignedLong, std::size_t) const
         * or combines it from a @ref SceneField::Translation,
         * @relativeref{SceneField,Rotation} and
         * @relativeref{SceneField,Scaling}, converting the fields from
         * arbitrary underlying types the same way as
         * @ref transformations3DAsArray(). See the lookup function
         * documentation for operation complexity --- for retrieving
         * transformation info for many objects it's recommended to access the
         * field data directly.
         *
         * If neither @ref SceneField::Transformation nor any of
         * @ref SceneField::Translation, @relativeref{SceneField,Rotation} or
         * @relativeref{SceneField,Scaling} is present, the fields represent a
         * 2D transformation or there's no transformation for @p object,
         * returns @relativeref{Corrade,Containers::NullOpt}.
         *
         * The @p object is expected to be less than @ref mappingBound().
         * @see @ref translationRotationScaling3DFor()
         */
        Containers::Optional<Matrix4> transformation3DFor(UnsignedLong object) const;

        /**
         * @brief 3D translation, rotation and scaling for given object
         * @m_since_latest
         *
         * Looks up the @ref SceneField::Translation,
         * @relativeref{SceneField,Rotation} and
         * @relativeref{SceneField,Scaling} fields for @p object equivalently
         * to @ref findFieldObjectOffset(SceneField, UnsignedLong, std::size_t) const
         * and then converts the fields from arbitrary underlying types the
         * same way as @ref translationsRotationsScalings2DAsArray(). See the
         * lookup function documentation for operation complexity --- for
         * retrieving transformation info for many objects it's recommended to
         * access the field data directly.
         *
         * If the @ref SceneField::Translation field isn't present, the first
         * returned value is a zero vector. If the
         * @relativeref{SceneField,Rotation} field isn't present, the second
         * value is an identity rotation. If the @relativeref{SceneField,Scaling}
         * field isn't present, the third value is an identity scaling
         * (@cpp 1.0f @ce in all dimensions). If neither of those fields is
         * present, if any of the fields represents a 2D transformation or if
         * there's no transformation for @p object, returns
         * @relativeref{Corrade,Containers::NullOpt}.
         *
         * The @p object is expected to be less than @ref mappingBound().
         * @see @ref transformation3DFor()
         */
        Containers::Optional<Containers::Triple<Vector3, Quaternion, Vector3>> translationRotationScaling3DFor(UnsignedLong object) const;

        /**
         * @brief Meshes and materials for given object
         * @m_since_latest
         *
         * Looks up all @ref SceneField::Mesh and @ref SceneField::MeshMaterial
         * @relativeref{SceneField,Scaling} fields for @p object
         * equivalently to @ref findFieldObjectOffset(SceneField, UnsignedLong, std::size_t) const
         * and then converts the field from an arbitrary underlying type the
         * same way as @ref meshesMaterialsAsArray(). See the lookup function
         * documentation for operation complexity --- for retrieving mesh and
         * material info for many objects it's recommended to access the field
         * data directly.
         *
         * If the @ref SceneField::MeshMaterial field is not present, the
         * second returned value is always @cpp -1 @ce. If
         * @ref SceneField::Mesh is not present or if there's no mesh for
         * @p object, returns an empty array.
         *
         * The @p object is expected to be less than @ref mappingBound().
         */
        Containers::Array<Containers::Pair<UnsignedInt, Int>> meshesMaterialsFor(UnsignedLong object) const;

        /**
         * @brief Lights for given object
         * @m_since_latest
         *
         * Looks up all @ref SceneField::Light fields for @p object
         * equivalently to @ref findFieldObjectOffset(SceneField, UnsignedLong, std::size_t) const
         * and then converts the field from an arbitrary underlying type the
         * same way as @ref lightsAsArray(). See the lookup function
         * documentation for operation complexity --- for retrieving light info
         * for many objects it's recommended to access the field data directly.
         *
         * If the @ref SceneField::Light field is not present or if there's no
         * light for @p object, returns an empty array.
         *
         * The @p object is expected to be less than @ref mappingBound().
         */
        Containers::Array<UnsignedInt> lightsFor(UnsignedLong object) const;

        /**
         * @brief Cameras for given object
         * @m_since_latest
         *
         * Looks up all @ref SceneField::Camera fields for @p object
         * equivalently to @ref findFieldObjectOffset(SceneField, UnsignedLong, std::size_t) const
         * and then converts the field from an arbitrary underlying type the
         * same way as @ref camerasAsArray(). See the lookup function
         * documentation for operation complexity --- for retrieving camera
         * info for many objects it's recommended to access the field data
         * directly.
         *
         * If the @ref SceneField::Camera field is not present or if there's no
         * camera for @p object, returns an empty array.
         *
         * The @p object is expected to be less than @ref mappingBound().
         */
        Containers::Array<UnsignedInt> camerasFor(UnsignedLong object) const;

        /**
         * @brief Skins for given object
         * @m_since_latest
         *
         * Looks up all @ref SceneField::Skin fields for @p object
         * equivalently to @ref findFieldObjectOffset(SceneField, UnsignedLong, std::size_t) const
         * and then converts the field from an arbitrary underlying type the
         * same way as @ref skinsAsArray(). See the lookup function
         * documentation for operation complexity --- for retrieving skin info
         * for many objects it's recommended to access the field data directly.
         *
         * If the @ref SceneField::Skin field is not present or if there's no
         * skin for @p object, returns an empty array.
         *
         * The @p object is expected to be less than @ref mappingBound().
         */
        Containers::Array<UnsignedInt> skinsFor(UnsignedLong object) const;

        /**
         * @brief Importer state for given object
         * @m_since_latest
         *
         * Looks up the @ref SceneField::ImporterState field for @p object
         * equivalently to @ref findFieldObjectOffset(SceneField, UnsignedLong, std::size_t) const
         * and then converts the field from an arbitrary underlying type the
         * same way as @ref importerStateAsArray(). See the lookup function
         * documentation for operation complexity --- for retrieving importer
         * state info for many objects it's recommended to access the field
         * data directly.
         *
         * If the @ref SceneField::ImporterState field is not present or if
         * there's no importer state for @p object, returns
         * @relativeref{Corrade,Containers::NullOpt}.
         *
         * The @p object is expected to be less than @ref mappingBound().
         */
        Containers::Optional<const void*> importerStateFor(UnsignedLong object) const;

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Two-dimensional root scene objects
         * @m_deprecated_since_latest Use @ref childrenFor() with `-1` passed
         *      as the @p object argument.
         */
        CORRADE_DEPRECATED("use childrenFor() instead") std::vector<UnsignedInt> children2D() const;

        /**
         * @brief Three-dimensional root scene objects
         * @m_deprecated_since_latest Use @ref childrenFor() with `-1` passed
         *      as the @p object argument.
         */
        CORRADE_DEPRECATED("use childrenFor() instead") std::vector<UnsignedInt> children3D() const;
        #endif

        /**
         * @brief Release field data storage
         * @m_since_latest
         *
         * Releases the ownership of the field data array and resets internal
         * field-related state to default. The scene then behaves like if it
         * has no fields (but it can still have non-empty data). Note that the
         * returned array has a custom no-op deleter when the data are not
         * owned by the scene, and while the returned array type is mutable,
         * the actual memory might be not. Additionally, the returned
         * @ref SceneFieldData instances may have different data pointers and
         * sizes than what's returned by the @ref field() and
         * @ref fieldData(UnsignedInt) const accessors as some of them might
         * have @ref SceneFieldFlag::OffsetOnly --- use this function only if
         * you *really* know what are you doing.
         * @see @ref fieldData(), @ref SceneFieldData::flags()
         */
        Containers::Array<SceneFieldData> releaseFieldData();

        /**
         * @brief Release data storage
         * @m_since_latest
         *
         * Releases the ownership of the data array. Note that the returned
         * array has a custom no-op deleter when the data are not owned by the
         * scene, and while the returned array type is mutable, the actual
         * memory might be not.
         *
         * @attention Querying any field after calling @ref releaseData() has
         *      undefined behavior and might lead to crashes. This is done
         *      intentionally in order to simplify the interaction between this
         *      function and @ref releaseFieldData().
         * @see @ref data(), @ref dataFlags()
         */
        Containers::Array<char> releaseData();

        /**
         * @brief Importer-specific state
         *
         * Scene-specific importer state. For object-specific importer state
         * look for the @ref SceneField::ImporterState field or access it via
         * @ref importerStateAsArray(), @ref importerStateFor() and related
         * convenience functions. See @ref AbstractImporter::importerState()
         * for general information about importer state pointers.
         */
        const void* importerState() const { return _importerState; }

    private:
        /* For custom deleter checks. Not done in the constructors here because
           the restriction is pointless when used outside of plugin
           implementations. */
        friend AbstractImporter;

        /* Internal helper without the extra overhead from Optional, returns
           ~UnsignedInt{} on failure */
        UnsignedInt findFieldIdInternal(SceneField name) const;

        /* Returns the offset at which `object` is for field at index `id`, or
           the end offset if the object is not found. The returned offset can
           be then passed to fieldData{Mapping,Field}ViewInternal(). */
        MAGNUM_TRADE_LOCAL std::size_t findFieldObjectOffsetInternal(const SceneFieldData& field, UnsignedLong object, std::size_t offset) const;

        /* Like mapping() / field(), but returning just a 1D view, sliced from
           offset to offset + size. The parameterless overloads are equal to
           offset = 0 and size = field.size(). */
        MAGNUM_TRADE_LOCAL Containers::StridedArrayView1D<const void> fieldDataMappingViewInternal(const SceneFieldData& field, std::size_t offset, std::size_t size) const;
        MAGNUM_TRADE_LOCAL Containers::StridedArrayView1D<const void> fieldDataMappingViewInternal(const SceneFieldData& field) const;
        MAGNUM_TRADE_LOCAL Containers::StridedArrayView1D<const void> fieldDataFieldViewInternal(const SceneFieldData& field, std::size_t offset, std::size_t size) const;
        MAGNUM_TRADE_LOCAL Containers::StridedArrayView1D<const void> fieldDataFieldViewInternal(const SceneFieldData& field) const;
        /* Assertion-less helper for fieldStringData() and fieldStrings() */
        MAGNUM_TRADE_LOCAL const char* fieldDataStringDataInternal(const SceneFieldData& field) const;

        #ifndef CORRADE_NO_ASSERT
        template<class T> bool checkFieldTypeCompatibility(const SceneFieldData& field, const char* prefix) const;
        #endif

        MAGNUM_TRADE_LOCAL void mappingIntoInternal(UnsignedInt fieldId, std::size_t offset, const Containers::StridedArrayView1D<UnsignedInt>& destination) const;
        MAGNUM_TRADE_LOCAL void parentsIntoInternal(UnsignedInt fieldId, std::size_t offset, const Containers::StridedArrayView1D<Int>& destination) const;
        MAGNUM_TRADE_LOCAL UnsignedInt findTransformationFields(UnsignedInt& transformationFieldId, UnsignedInt& translationFieldId, UnsignedInt& rotationFieldId, UnsignedInt& scalingFieldId) const;
        MAGNUM_TRADE_LOCAL UnsignedInt findTranslationRotationScalingFields(UnsignedInt& translationFieldId, UnsignedInt& rotationFieldId, UnsignedInt& scalingFieldId) const;
        MAGNUM_TRADE_LOCAL void transformations2DIntoInternal(UnsignedInt transformationFieldId, UnsignedInt translationFieldId, UnsignedInt rotationFieldId, UnsignedInt scalingFieldId, std::size_t offset, const Containers::StridedArrayView1D<Matrix3>& destination) const;
        MAGNUM_TRADE_LOCAL void translationsRotationsScalings2DIntoInternal(UnsignedInt translationFieldId, UnsignedInt rotationFieldId, UnsignedInt scalingFieldId, std::size_t offset, const Containers::StridedArrayView1D<Vector2>& translationDestination, const Containers::StridedArrayView1D<Complex>& rotationDestination, const Containers::StridedArrayView1D<Vector2>& scalingDestination) const;
        MAGNUM_TRADE_LOCAL void transformations3DIntoInternal(UnsignedInt transformationFieldId, UnsignedInt translationFieldId, UnsignedInt rotationFieldId, UnsignedInt scalingFieldId, std::size_t offset, const Containers::StridedArrayView1D<Matrix4>& destination) const;
        MAGNUM_TRADE_LOCAL void translationsRotationsScalings3DIntoInternal(UnsignedInt translationFieldId, UnsignedInt rotationFieldId, UnsignedInt scalingFieldId, std::size_t offset, const Containers::StridedArrayView1D<Vector3>& translationDestination, const Containers::StridedArrayView1D<Quaternion>& rotationDestination, const Containers::StridedArrayView1D<Vector3>& scalingDestination) const;
        MAGNUM_TRADE_LOCAL void unsignedIndexFieldIntoInternal(const UnsignedInt fieldId, std::size_t offset, const Containers::StridedArrayView1D<UnsignedInt>& destination) const;
        MAGNUM_TRADE_LOCAL void indexFieldIntoInternal(const UnsignedInt fieldId, std::size_t offset, const Containers::StridedArrayView1D<Int>& destination) const;
        MAGNUM_TRADE_LOCAL Containers::Array<Containers::Pair<UnsignedInt, UnsignedInt>> unsignedIndexFieldAsArrayInternal(const UnsignedInt fieldId) const;
        MAGNUM_TRADE_LOCAL void meshesMaterialsIntoInternal(UnsignedInt fieldId, std::size_t offset, const Containers::StridedArrayView1D<UnsignedInt>& meshDestination, const Containers::StridedArrayView1D<Int>& meshMaterialDestination) const;
        MAGNUM_TRADE_LOCAL void importerStateIntoInternal(const UnsignedInt fieldId, std::size_t offset, const Containers::StridedArrayView1D<const void*>& destination) const;

        DataFlags _dataFlags;
        SceneMappingType _mappingType;
        UnsignedByte _dimensions;
        /* 1/5 bytes free */
        UnsignedLong _mappingBound;
        const void* _importerState;
        Containers::Array<SceneFieldData> _fields;
        Containers::Array<char> _data;
};

namespace Implementation {
    /* Making this a struct because there can't be partial specializations for
       a function (which we may need for pointers, matrix/vector subclasses
       etc) */
    template<class T> struct SceneFieldTypeFor {
        static_assert(sizeof(T) == 0, "unsupported field type");
    };
    #ifndef DOXYGEN_GENERATING_OUTPUT
    #define _cn(name, ...) template<> struct SceneFieldTypeFor<__VA_ARGS__> { \
        constexpr static SceneFieldType type() {                            \
            return SceneFieldType::name;                                    \
        }                                                                   \
    };
    #define _c(type_) _cn(type_, type_)
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
    _c(Range1Dh)
    _c(Range1Dd)
    _c(Range1Di)
    _c(Range2D)
    _c(Range2Dh)
    _c(Range2Dd)
    _c(Range2Di)
    _c(Range3D)
    _c(Range3Dh)
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
    _cn(StringRange8, Containers::Pair<UnsignedByte, UnsignedByte>)
    _cn(StringRange16, Containers::Pair<UnsignedShort, UnsignedShort>)
    _cn(StringRange32, Containers::Pair<UnsignedInt, UnsignedInt>)
    _cn(StringRange64, Containers::Pair<UnsignedLong, UnsignedLong>)
    #undef _c
    #undef _cn
    #endif
    /** @todo this doesn't handle RectangleMatrix<size, size, T> and Vector<size, T> at the moment, do we need those? */
    template<class T> struct SceneFieldTypeFor<Math::Color3<T>>: SceneFieldTypeFor<Math::Vector3<T>> {};
    template<class T> struct SceneFieldTypeFor<Math::Color4<T>>: SceneFieldTypeFor<Math::Vector4<T>> {};
    template<class T> struct SceneFieldTypeFor<Math::Matrix3<T>>: SceneFieldTypeFor<Math::Matrix3x3<T>> {};
    template<class T> struct SceneFieldTypeFor<Math::Matrix4<T>>: SceneFieldTypeFor<Math::Matrix4x4<T>> {};
    template<class T> struct SceneFieldTypeFor<const T*> {
        constexpr static SceneFieldType type() {
            return SceneFieldType::Pointer;
        }
    };
    template<class T> struct SceneFieldTypeFor<T*> {
        constexpr static SceneFieldType type() {
            return SceneFieldType::MutablePointer;
        }
    };

    /* Used by field<T>(). Delegates to SceneFieldTypeFor by default, types
       that can work with multiple SceneFieldType values have a
       specialization. */
    template<class T> struct SceneFieldTypeTraits {
        constexpr static bool isCompatible(SceneFieldType type) {
            return type == SceneFieldTypeFor<T>::type();
        }
    };
    template<> struct SceneFieldTypeTraits<UnsignedByte> {
        constexpr static bool isCompatible(SceneFieldType type) {
            return type == SceneFieldType::UnsignedByte ||
                   type == SceneFieldType::StringOffset8 ||
                   type == SceneFieldType::StringRangeNullTerminated8;
        }
    };
    template<> struct SceneFieldTypeTraits<UnsignedShort> {
        constexpr static bool isCompatible(SceneFieldType type) {
            return type == SceneFieldType::UnsignedShort ||
                   type == SceneFieldType::StringOffset16 ||
                   type == SceneFieldType::StringRangeNullTerminated16;
        }
    };
    template<> struct SceneFieldTypeTraits<UnsignedInt> {
        constexpr static bool isCompatible(SceneFieldType type) {
            return type == SceneFieldType::UnsignedInt ||
                   type == SceneFieldType::StringOffset32 ||
                   type == SceneFieldType::StringRangeNullTerminated32;
        }
    };
    template<> struct SceneFieldTypeTraits<UnsignedLong> {
        constexpr static bool isCompatible(SceneFieldType type) {
            return type == SceneFieldType::UnsignedLong ||
                   type == SceneFieldType::StringOffset64 ||
                   type == SceneFieldType::StringRangeNullTerminated64;
        }
    };

    template<class T> constexpr SceneMappingType sceneMappingTypeFor() {
        static_assert(sizeof(T) == 0, "unsupported mapping type");
        return {};
    }
    #ifndef DOXYGEN_GENERATING_OUTPUT
    #define _c(type) \
        template<> constexpr SceneMappingType sceneMappingTypeFor<type>() { return SceneMappingType::type; }
    _c(UnsignedByte)
    _c(UnsignedShort)
    _c(UnsignedInt)
    _c(UnsignedLong)
    #undef _c
    #endif

    constexpr bool isSceneFieldTypeCompatibleWithField(SceneField name, SceneFieldType type) {
        return
            /* Named fields are restricted so we can decode them */
            (name == SceneField::Parent &&
                (type == SceneFieldType::Byte ||
                 type == SceneFieldType::Short ||
                 type == SceneFieldType::Int ||
                 type == SceneFieldType::Long)) ||
            (name == SceneField::Transformation &&
                (type == SceneFieldType::Matrix3x3 ||
                 type == SceneFieldType::Matrix3x3d ||
                 type == SceneFieldType::Matrix4x4 ||
                 type == SceneFieldType::Matrix4x4d ||
                 type == SceneFieldType::Matrix3x2 ||
                 type == SceneFieldType::Matrix3x2d ||
                 type == SceneFieldType::Matrix4x3 ||
                 type == SceneFieldType::Matrix4x3d ||
                 type == SceneFieldType::DualComplex ||
                 type == SceneFieldType::DualComplexd ||
                 type == SceneFieldType::DualQuaternion ||
                 type == SceneFieldType::DualQuaterniond)) ||
           ((name == SceneField::Translation ||
             name == SceneField::Scaling) &&
                (type == SceneFieldType::Vector2 ||
                 type == SceneFieldType::Vector2d ||
                 type == SceneFieldType::Vector3 ||
                 type == SceneFieldType::Vector3d)) ||
            (name == SceneField::Rotation &&
                (type == SceneFieldType::Complex ||
                 type == SceneFieldType::Complexd ||
                 type == SceneFieldType::Quaternion ||
                 type == SceneFieldType::Quaterniond)) ||
           ((name == SceneField::Mesh ||
             name == SceneField::Light ||
             name == SceneField::Camera ||
             name == SceneField::Skin) &&
                (type == SceneFieldType::UnsignedByte ||
                 type == SceneFieldType::UnsignedShort ||
                 type == SceneFieldType::UnsignedInt)) ||
            (name == SceneField::MeshMaterial &&
                (type == SceneFieldType::Byte ||
                 type == SceneFieldType::Short ||
                 type == SceneFieldType::Int)) ||
            (name == SceneField::ImporterState &&
                (type == SceneFieldType::Pointer ||
                 type == SceneFieldType::MutablePointer)) ||
            /* Custom fields can be anything */
            isSceneFieldCustom(name);
    }

    constexpr bool isSceneFieldArrayAllowed(SceneField name) {
        return isSceneFieldCustom(name);
    }

    constexpr bool isSceneFieldTypeString(SceneFieldType type) {
        return
            type == SceneFieldType::StringOffset8 ||
            type == SceneFieldType::StringOffset16 ||
            type == SceneFieldType::StringOffset32 ||
            type == SceneFieldType::StringOffset64 ||
            type == SceneFieldType::StringRange8 ||
            type == SceneFieldType::StringRange16 ||
            type == SceneFieldType::StringRange32 ||
            type == SceneFieldType::StringRange64 ||
            type == SceneFieldType::StringRangeNullTerminated8 ||
            type == SceneFieldType::StringRangeNullTerminated16 ||
            type == SceneFieldType::StringRangeNullTerminated32 ||
            type == SceneFieldType::StringRangeNullTerminated64;
    }

    constexpr SceneFieldFlags implicitSceneFieldFlagsFor(SceneFieldType type) {
        return
            type == SceneFieldType::StringRangeNullTerminated8 ||
            type == SceneFieldType::StringRangeNullTerminated16 ||
            type == SceneFieldType::StringRangeNullTerminated32 ||
            type == SceneFieldType::StringRangeNullTerminated64 ?
            SceneFieldFlag::NullTerminatedString : SceneFieldFlags{};
    }

    constexpr SceneFieldFlags disallowedSceneFieldFlagsFor(SceneField name) {
        return
            name == SceneField::Parent ||
            name == SceneField::Transformation ||
            name == SceneField::Translation ||
            name == SceneField::Rotation ||
            name == SceneField::Scaling ?
            SceneFieldFlag::MultiEntry : SceneFieldFlags{};
    }
}

constexpr SceneFieldData::SceneFieldData(const SceneField name, const SceneMappingType mappingType, const Containers::StridedArrayView1D<const void>& mappingData, const SceneFieldType fieldType, const Containers::StridedArrayView1D<const void>& fieldData, const UnsignedShort fieldArraySize, const SceneFieldFlags flags) noexcept:
    _size{(CORRADE_CONSTEXPR_ASSERT(mappingData.size() == fieldData.size(),
        "Trade::SceneFieldData: expected" << name << "mapping and field view to have the same size but got" << mappingData.size() << "and" << fieldData.size()), mappingData.size())},
    _name{(CORRADE_CONSTEXPR_ASSERT(Implementation::isSceneFieldTypeCompatibleWithField(name, fieldType),
        "Trade::SceneFieldData:" << fieldType << "is not a valid type for" << name), name)},
    _flags{(CORRADE_CONSTEXPR_ASSERT(!(flags & (SceneFieldFlag::OffsetOnly|SceneFieldFlag::NullTerminatedString|Implementation::disallowedSceneFieldFlagsFor(name))),
        "Trade::SceneFieldData: can't pass" << (flags & (SceneFieldFlag::OffsetOnly|SceneFieldFlag::NullTerminatedString|Implementation::disallowedSceneFieldFlagsFor(name))) << "for a" << name << "view of" << fieldType), flags)},
    /* Can't use {} because GCC 4.8 then says "warning: parameter 'mappingType'
       set but not used" */
    _mappingTypeStringType(UnsignedByte(mappingType)),
    _mappingStride{(CORRADE_CONSTEXPR_ASSERT(mappingData.stride() >= -32768 && mappingData.stride() <= 32767,
        "Trade::SceneFieldData: expected mapping view stride to fit into 16 bits but got" << mappingData.stride()), Short(mappingData.stride()))},
    _mappingData{mappingData.data()},
    _field{
        (CORRADE_CONSTEXPR_ASSERT(fieldData.stride() >= -32768 && fieldData.stride() <= 32767,
            "Trade::SceneFieldData: expected field view stride to fit into 16 bits but got" << fieldData.stride()), Short(fieldData.stride())),
        (CORRADE_CONSTEXPR_ASSERT(!Implementation::isSceneFieldTypeString(fieldType),
            "Trade::SceneFieldData: use a string constructor for" << fieldType),
         CORRADE_CONSTEXPR_ASSERT(fieldType != SceneFieldType::Bit,
            "Trade::SceneFieldData: use a bit constructor for" << fieldType), fieldType),
        (CORRADE_CONSTEXPR_ASSERT(!fieldArraySize || Implementation::isSceneFieldArrayAllowed(name),
            "Trade::SceneFieldData:" << name << "can't be an array field"), fieldArraySize)},
    _fieldData{fieldData.data()} {}

template<class T, class U> constexpr SceneFieldData::SceneFieldData(const SceneField name, const Containers::StridedArrayView1D<T>& mappingData, const Containers::StridedArrayView1D<U>& fieldData, const SceneFieldFlags flags) noexcept: SceneFieldData{name, Implementation::sceneMappingTypeFor<typename std::remove_const<T>::type>(), mappingData, Implementation::SceneFieldTypeFor<typename std::remove_const<U>::type>::type(), fieldData, 0, flags} {}

template<class T, class U> constexpr SceneFieldData::SceneFieldData(const SceneField name, const Containers::StridedArrayView1D<T>& mappingData, const Containers::StridedArrayView2D<U>& fieldData, const SceneFieldFlags flags) noexcept: SceneFieldData{
    name,
    Implementation::sceneMappingTypeFor<typename std::remove_const<T>::type>(),
    mappingData,
    Implementation::SceneFieldTypeFor<typename std::remove_const<U>::type>::type(),
    Containers::StridedArrayView1D<const void>{{fieldData.data(), ~std::size_t{}}, fieldData.size()[0], fieldData.stride()[0]},
    /* Not using isContiguous<1>() as that's not constexpr */
    (CORRADE_CONSTEXPR_ASSERT(fieldData.stride()[1] == sizeof(U), "Trade::SceneFieldData: second field view dimension is not contiguous"), UnsignedShort(fieldData.size()[1])),
    flags
} {}

/* Assumes that fieldDataBitOffset and fieldDataSize is already bounds-checked,
   either by the StridedBitArrayView itself or by the offset-only constructor;
   and fieldArraySize as well by the constructor taking the 2D bit view */
constexpr SceneFieldData::SceneFieldData(const SceneField name, const SceneMappingType mappingType, const Containers::StridedArrayView1D<const void>& mappingData, const void* const fieldData, const UnsignedByte fieldBitOffset, CORRADE_UNUSED const std::size_t fieldSize, const std::ptrdiff_t fieldStride, const UnsignedShort fieldArraySize, const SceneFieldFlags flags) noexcept:
    _size{(CORRADE_CONSTEXPR_ASSERT(mappingData.size() == fieldSize,
        "Trade::SceneFieldData: expected" << name << "mapping and field view to have the same size but got" << mappingData.size() << "and" << fieldSize), mappingData.size())},
    _name{(CORRADE_CONSTEXPR_ASSERT(Implementation::isSceneFieldTypeCompatibleWithField(name, SceneFieldType::Bit),
        "Trade::SceneFieldData:" << SceneFieldType::Bit << "is not a valid type for" << name), name)},
    _flags{(CORRADE_CONSTEXPR_ASSERT(!(flags & (SceneFieldFlag::OffsetOnly|SceneFieldFlag::NullTerminatedString|Implementation::disallowedSceneFieldFlagsFor(name))),
        "Trade::SceneFieldData: can't pass" << (flags & (SceneFieldFlag::OffsetOnly|SceneFieldFlag::NullTerminatedString|Implementation::disallowedSceneFieldFlagsFor(name))) << "for a" << name << "view of" << SceneFieldType::Bit), flags)},
    /* Can't use {} because GCC 4.8 then says "warning: parameter 'mappingType'
       set but not used" */
    _mappingTypeStringType(UnsignedByte(mappingType)),
    _mappingStride{(CORRADE_CONSTEXPR_ASSERT(mappingData.stride() >= -32768 && mappingData.stride() <= 32767,
        "Trade::SceneFieldData: expected mapping view stride to fit into 16 bits but got" << mappingData.stride()), Short(mappingData.stride()))},
    _mappingData{mappingData.data()},
    _field{
        (CORRADE_CONSTEXPR_ASSERT(fieldStride >= -32768 && fieldStride <= 32767,
            "Trade::SceneFieldData: expected field view stride to fit into 16 bits but got" << fieldStride), Short(fieldStride)),
        SceneFieldType::Bit, fieldArraySize, fieldBitOffset},
    _fieldData{fieldData} {}

#ifndef DOXYGEN_GENERATING_OUTPUT
template<class T> constexpr SceneFieldData::SceneFieldData(const SceneField name, const SceneMappingType mappingType, const Containers::StridedArrayView1D<const void>& mappingData, const Containers::BasicStridedBitArrayView<1, T>& fieldData, const SceneFieldFlags flags) noexcept: SceneFieldData{name, mappingType, mappingData, fieldData.data(), UnsignedByte(fieldData.offset()), fieldData.size(), fieldData.stride(), 0, flags} {}
#endif

template<class T> constexpr SceneFieldData::SceneFieldData(const SceneField name, const Containers::StridedArrayView1D<T>& mappingData, const Containers::StridedBitArrayView1D& fieldData, const SceneFieldFlags flags) noexcept: SceneFieldData{name, Implementation::sceneMappingTypeFor<typename std::remove_const<T>::type>(), mappingData, fieldData, flags} {}

#ifndef DOXYGEN_GENERATING_OUTPUT
template<class T> constexpr SceneFieldData::SceneFieldData(const SceneField name, const SceneMappingType mappingType, const Containers::StridedArrayView1D<const void>& mappingData, const Containers::BasicStridedBitArrayView<2, T>& fieldData, const SceneFieldFlags flags) noexcept: SceneFieldData{name, mappingType, mappingData, fieldData.data(), UnsignedByte(fieldData.offset()), fieldData.size()[0],
    (CORRADE_CONSTEXPR_ASSERT(fieldData.stride()[1] == 1,
        "Trade::SceneFieldData: second field view dimension is not contiguous"), fieldData.stride()[0]),
    /* There's no need to check that the array size fits into 16 bits, as the
       stronger requirement is that the signed stride fits into 16 bits */
    UnsignedShort(fieldData.size()[1]), flags} {}
#endif

template<class T> constexpr SceneFieldData::SceneFieldData(const SceneField name, const Containers::StridedArrayView1D<T>& mappingData, const Containers::StridedBitArrayView2D& fieldData, const SceneFieldFlags flags) noexcept: SceneFieldData{name, Implementation::sceneMappingTypeFor<typename std::remove_const<T>::type>(), mappingData, fieldData, flags} {}

template<class T> inline SceneFieldData::SceneFieldData(const SceneField name, const Containers::StridedArrayView1D<T>& mappingData, const char* stringData, SceneFieldType fieldType, const Containers::StridedArrayView1D<const void>& fieldData, const SceneFieldFlags flags) noexcept: SceneFieldData{name, Implementation::sceneMappingTypeFor<typename std::remove_const<T>::type>(), mappingData, stringData, fieldType, fieldData, flags} {}

constexpr SceneFieldData::SceneFieldData(const SceneField name, const std::size_t size, const SceneMappingType mappingType, const std::size_t mappingOffset, const std::ptrdiff_t mappingStride, const SceneFieldType fieldType, const std::size_t fieldOffset, const std::ptrdiff_t fieldStride, const UnsignedShort fieldArraySize, const SceneFieldFlags flags) noexcept:
    _size{size},
    _name{(CORRADE_CONSTEXPR_ASSERT(Implementation::isSceneFieldTypeCompatibleWithField(name, fieldType),
        "Trade::SceneFieldData:" << fieldType << "is not a valid type for" << name), name)},
    _flags{(CORRADE_CONSTEXPR_ASSERT(!(flags & (SceneFieldFlag::NullTerminatedString|Implementation::disallowedSceneFieldFlagsFor(name))),
        "Trade::SceneFieldData: can't pass" << (flags & (SceneFieldFlag::NullTerminatedString|Implementation::disallowedSceneFieldFlagsFor(name))) << "for" << name << "of" << fieldType), flags|SceneFieldFlag::OffsetOnly)},
    /* Can't use {} because GCC 4.8 then says "warning: parameter 'mappingType'
       set but not used" */
    _mappingTypeStringType(UnsignedByte(mappingType)),
    _mappingStride{(CORRADE_CONSTEXPR_ASSERT(mappingStride >= -32768 && mappingStride <= 32767,
        "Trade::SceneFieldData: expected mapping view stride to fit into 16 bits but got" << mappingStride), Short(mappingStride))},
    _mappingData{mappingOffset},
    _field{
        (CORRADE_CONSTEXPR_ASSERT(fieldStride >= -32768 && fieldStride <= 32767,
            "Trade::SceneFieldData: expected field view stride to fit into 16 bits but got" << fieldStride), Short(fieldStride)),
        (CORRADE_CONSTEXPR_ASSERT(!Implementation::isSceneFieldTypeString(fieldType),
            "Trade::SceneFieldData: use a string constructor for" << fieldType),
         CORRADE_CONSTEXPR_ASSERT(fieldType != SceneFieldType::Bit,
            "Trade::SceneFieldData: use a bit constructor for" << fieldType), fieldType),
        (CORRADE_CONSTEXPR_ASSERT(!fieldArraySize || Implementation::isSceneFieldArrayAllowed(name),
            "Trade::SceneFieldData:" << name << "can't be an array field"), fieldArraySize)},
    _fieldData{fieldOffset} {}

constexpr SceneFieldData::SceneFieldData(const SceneField name, const std::size_t size, const SceneMappingType mappingType, const std::size_t mappingOffset, const std::ptrdiff_t mappingStride, const std::size_t fieldOffset, const std::size_t fieldBitOffset, const std::ptrdiff_t fieldStride, const UnsignedShort fieldArraySize, const SceneFieldFlags flags) noexcept:
    _size{(CORRADE_CONSTEXPR_DEBUG_ASSERT(size < std::size_t{1} << (sizeof(std::size_t)*8 - 3),
        "Trade::SceneFieldData: size expected to be smaller than 2^" << Debug::nospace << (sizeof(std::size_t)*8 - 3) << "bits, got" << size), size)},
    _name{(CORRADE_CONSTEXPR_ASSERT(Implementation::isSceneFieldTypeCompatibleWithField(name, SceneFieldType::Bit),
        "Trade::SceneFieldData:" << SceneFieldType::Bit << "is not a valid type for" << name), name)},
    _flags{(CORRADE_CONSTEXPR_ASSERT(!(flags & (SceneFieldFlag::NullTerminatedString|Implementation::disallowedSceneFieldFlagsFor(name))),
        "Trade::SceneFieldData: can't pass" << (flags & (SceneFieldFlag::NullTerminatedString|Implementation::disallowedSceneFieldFlagsFor(name))) << "for" << name << "of" << SceneFieldType::Bit), flags|SceneFieldFlag::OffsetOnly)},
    /* Can't use {} because GCC 4.8 then says "warning: parameter 'mappingType'
       set but not used" */
    _mappingTypeStringType(UnsignedByte(mappingType)),
    _mappingStride{(CORRADE_CONSTEXPR_ASSERT(mappingStride >= -32768 && mappingStride <= 32767,
        "Trade::SceneFieldData: expected mapping view stride to fit into 16 bits but got" << mappingStride), Short(mappingStride))},
    _mappingData{mappingOffset},
    _field{
        (CORRADE_CONSTEXPR_ASSERT(fieldStride >= -32768 && fieldStride <= 32767,
            "Trade::SceneFieldData: expected field view stride to fit into 16 bits but got" << fieldStride), Short(fieldStride)),
        SceneFieldType::Bit, fieldArraySize,
        (CORRADE_CONSTEXPR_DEBUG_ASSERT(fieldBitOffset < 8,
            "Trade::SceneFieldData: bit offset expected to be smaller than 8, got" << fieldBitOffset), UnsignedByte(fieldBitOffset))},
    _fieldData{fieldOffset} {}

constexpr SceneFieldData::SceneFieldData(const SceneField name, const std::size_t size, const SceneMappingType mappingType, const std::size_t mappingOffset, const std::ptrdiff_t mappingStride, const std::size_t stringOffset, const SceneFieldType fieldType, const std::size_t fieldOffset, const std::ptrdiff_t fieldStride, const SceneFieldFlags flags) noexcept:
    _size{size},
    _name{(CORRADE_CONSTEXPR_ASSERT(Implementation::isSceneFieldTypeCompatibleWithField(name, fieldType),
        "Trade::SceneFieldData:" << fieldType << "is not a valid type for" << name), name)},
    /** @todo check allowed flags once disallowedSceneFieldFlagsFor() includes
        string fields too */
    _flags{flags|SceneFieldFlag::OffsetOnly|Implementation::implicitSceneFieldFlagsFor(fieldType)},
    _mappingTypeStringType{(CORRADE_CONSTEXPR_ASSERT(Implementation::isSceneFieldTypeString(fieldType),
        "Trade::SceneFieldData: can't use a string constructor for" << fieldType), UnsignedByte(UnsignedByte(mappingType)|(UnsignedShort(fieldType) << 3)))},
    _mappingStride{(CORRADE_CONSTEXPR_ASSERT(mappingStride >= -32768 && mappingStride <= 32767,
        "Trade::SceneFieldData: expected mapping view stride to fit into 16 bits but got" << mappingStride), Short(mappingStride))},
    _mappingData{mappingOffset},
    _field{
        (CORRADE_CONSTEXPR_ASSERT(fieldStride >= -32768 && fieldStride <= 32767,
            "Trade::SceneFieldData: expected field view stride to fit into 16 bits but got" << fieldStride), Short(fieldStride)),
        (
            #ifndef CORRADE_TARGET_32BIT
            /* 47 because the distance is signed */
            CORRADE_CONSTEXPR_ASSERT(stringOffset < (1ull << 47),
                "Trade::SceneFieldData: expected string data offset to fit into 48 bits but got" << stringOffset),
            #endif
        Long(stringOffset))},
    _fieldData{fieldOffset} {}

template<class T> Containers::StridedArrayView1D<const T> SceneData::mapping(const UnsignedInt fieldId) const {
    Containers::StridedArrayView2D<const char> data = mapping(fieldId);
    #ifdef CORRADE_GRACEFUL_ASSERT /* Sigh. Brittle. Better idea? */
    if(!data.stride()[1]) return {};
    #endif
    CORRADE_ASSERT(Implementation::sceneMappingTypeFor<T>() == _mappingType,
        "Trade::SceneData::mapping(): mapping is" << _mappingType << "but requested" << Implementation::sceneMappingTypeFor<T>(), {});
    return Containers::arrayCast<1, const T>(data);
}

template<class T> Containers::StridedArrayView1D<T> SceneData::mutableMapping(const UnsignedInt fieldId) {
    Containers::StridedArrayView2D<char> data = mutableMapping(fieldId);
    #ifdef CORRADE_GRACEFUL_ASSERT /* Sigh. Brittle. Better idea? */
    if(!data.stride()[1]) return {};
    #endif
    CORRADE_ASSERT(Implementation::sceneMappingTypeFor<T>() == _mappingType,
        "Trade::SceneData::mutableMapping(): mapping is" << _mappingType << "but requested" << Implementation::sceneMappingTypeFor<T>(), {});
    return Containers::arrayCast<1, T>(data);
}

template<class T> Containers::StridedArrayView1D<const T> SceneData::mapping(const SceneField fieldName) const {
    Containers::StridedArrayView2D<const char> data = mapping(fieldName);
    #ifdef CORRADE_GRACEFUL_ASSERT /* Sigh. Brittle. Better idea? */
    if(!data.stride()[1]) return {};
    #endif
    CORRADE_ASSERT(Implementation::sceneMappingTypeFor<T>() == _mappingType,
        "Trade::SceneData::mapping(): mapping is" << _mappingType << "but requested" << Implementation::sceneMappingTypeFor<T>(), {});
    return Containers::arrayCast<1, const T>(data);
}

template<class T> Containers::StridedArrayView1D<T> SceneData::mutableMapping(const SceneField fieldName) {
    Containers::StridedArrayView2D<char> data = mutableMapping(fieldName);
    #ifdef CORRADE_GRACEFUL_ASSERT /* Sigh. Brittle. Better idea? */
    if(!data.stride()[1]) return {};
    #endif
    CORRADE_ASSERT(Implementation::sceneMappingTypeFor<T>() == _mappingType,
        "Trade::SceneData::mutableMapping(): mapping is" << _mappingType << "but requested" << Implementation::sceneMappingTypeFor<T>(), {});
    return Containers::arrayCast<1, T>(data);
}

#ifndef CORRADE_NO_ASSERT
template<class T> bool SceneData::checkFieldTypeCompatibility(const SceneFieldData& field, const char*
    #ifndef CORRADE_STANDARD_ASSERT
    const prefix
    #endif
) const {
    CORRADE_ASSERT(Implementation::SceneFieldTypeTraits<typename std::remove_extent<T>::type>::isCompatible(field.fieldType()),
        prefix << field._name << "is" << field.fieldType() << "but requested a type equivalent to" << Implementation::SceneFieldTypeFor<typename std::remove_extent<T>::type>::type(), false);
    CORRADE_ASSERT(!field.fieldArraySize() || std::is_array<T>::value,
        prefix << field._name << "is an array field, use T[] to access it", false);
    return true;
}
#endif

#ifndef DOXYGEN_GENERATING_OUTPUT
template<class T, typename std::enable_if<!std::is_array<T>::value, int>::type> Containers::StridedArrayView1D<const T> SceneData::field(const UnsignedInt id) const {
    Containers::StridedArrayView2D<const char> data = field(id);
    #ifdef CORRADE_GRACEFUL_ASSERT /* Sigh. Brittle. Better idea? */
    if(!data.stride()[1]) return {};
    #endif
    #ifndef CORRADE_NO_ASSERT
    if(!checkFieldTypeCompatibility<T>(_fields[id], "Trade::SceneData::field():")) return {};
    #endif
    return Containers::arrayCast<1, const T>(data);
}

template<class T, typename std::enable_if<std::is_array<T>::value, int>::type> Containers::StridedArrayView2D<const typename std::remove_extent<T>::type> SceneData::field(const UnsignedInt id) const {
    Containers::StridedArrayView2D<const char> data = field(id);
    #ifdef CORRADE_GRACEFUL_ASSERT /* Sigh. Brittle. Better idea? */
    if(!data.stride()[1]) return {};
    #endif
    #ifndef CORRADE_NO_ASSERT
    if(!checkFieldTypeCompatibility<T>(_fields[id], "Trade::SceneData::field():")) return {};
    #endif
    return Containers::arrayCast<2, const typename std::remove_extent<T>::type>(data);
}

template<class T, typename std::enable_if<!std::is_array<T>::value, int>::type> Containers::StridedArrayView1D<T> SceneData::mutableField(const UnsignedInt id) {
    Containers::StridedArrayView2D<char> data = mutableField(id);
    #ifdef CORRADE_GRACEFUL_ASSERT /* Sigh. Brittle. Better idea? */
    if(!data.stride()[1]) return {};
    #endif
    #ifndef CORRADE_NO_ASSERT
    if(!checkFieldTypeCompatibility<T>(_fields[id], "Trade::SceneData::mutableField():")) return {};
    #endif
    return Containers::arrayCast<1, T>(data);
}

template<class T, typename std::enable_if<std::is_array<T>::value, int>::type> Containers::StridedArrayView2D<typename std::remove_extent<T>::type> SceneData::mutableField(const UnsignedInt id) {
    Containers::StridedArrayView2D<char> data = mutableField(id);
    #ifdef CORRADE_GRACEFUL_ASSERT /* Sigh. Brittle. Better idea? */
    if(!data.stride()[1]) return {};
    #endif
    #ifndef CORRADE_NO_ASSERT
    if(!checkFieldTypeCompatibility<T>(_fields[id], "Trade::SceneData::mutableField():")) return {};
    #endif
    return Containers::arrayCast<2, typename std::remove_extent<T>::type>(data);
}

template<class T, typename std::enable_if<!std::is_array<T>::value, int>::type> Containers::StridedArrayView1D<const T> SceneData::field(const SceneField name) const {
    const UnsignedInt fieldId = findFieldIdInternal(name);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{},
        "Trade::SceneData::field(): field" << name << "not found", {});
    Containers::StridedArrayView2D<const char> data = field(fieldId);
    #ifdef CORRADE_GRACEFUL_ASSERT /* Sigh. Brittle. Better idea? */
    if(!data.stride()[1]) return {};
    #endif
    #ifndef CORRADE_NO_ASSERT
    if(!checkFieldTypeCompatibility<T>(_fields[fieldId], "Trade::SceneData::field():")) return {};
    #endif
    return Containers::arrayCast<1, const T>(data);
}

template<class T, typename std::enable_if<std::is_array<T>::value, int>::type> Containers::StridedArrayView2D<const typename std::remove_extent<T>::type> SceneData::field(const SceneField name) const {
    const UnsignedInt fieldId = findFieldIdInternal(name);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{},
        "Trade::SceneData::field(): field" << name << "not found", {});
    Containers::StridedArrayView2D<const char> data = field(fieldId);
    #ifdef CORRADE_GRACEFUL_ASSERT /* Sigh. Brittle. Better idea? */
    if(!data.stride()[1]) return {};
    #endif
    #ifndef CORRADE_NO_ASSERT
    if(!checkFieldTypeCompatibility<T>(_fields[fieldId], "Trade::SceneData::field():")) return {};
    #endif
    return Containers::arrayCast<2, const typename std::remove_extent<T>::type>(data);
}

template<class T, typename std::enable_if<!std::is_array<T>::value, int>::type> Containers::StridedArrayView1D<T> SceneData::mutableField(const SceneField name) {
    const UnsignedInt fieldId = findFieldIdInternal(name);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{},
        "Trade::SceneData::mutableField(): field" << name << "not found", {});
    Containers::StridedArrayView2D<char> data = mutableField(fieldId);
    #ifdef CORRADE_GRACEFUL_ASSERT /* Sigh. Brittle. Better idea? */
    if(!data.stride()[1]) return {};
    #endif
    #ifndef CORRADE_NO_ASSERT
    if(!checkFieldTypeCompatibility<T>(_fields[fieldId], "Trade::SceneData::mutableField():")) return {};
    #endif
    return Containers::arrayCast<1, T>(data);
}

template<class T, typename std::enable_if<std::is_array<T>::value, int>::type> Containers::StridedArrayView2D<typename std::remove_extent<T>::type> SceneData::mutableField(const SceneField name) {
    const UnsignedInt fieldId = findFieldIdInternal(name);
    CORRADE_ASSERT(fieldId != ~UnsignedInt{},
        "Trade::SceneData::mutableField(): field" << name << "not found", {});
    Containers::StridedArrayView2D<char> data = mutableField(fieldId);
    #ifdef CORRADE_GRACEFUL_ASSERT /* Sigh. Brittle. Better idea? */
    if(!data.stride()[1]) return {};
    #endif
    #ifndef CORRADE_NO_ASSERT
    if(!checkFieldTypeCompatibility<T>(_fields[fieldId], "Trade::SceneData::mutableField():")) return {};
    #endif
    return Containers::arrayCast<2, typename std::remove_extent<T>::type>(data);
}
#endif

}}

#endif
