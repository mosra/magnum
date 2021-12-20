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
 * @brief Class @ref Magnum::Trade::SceneData, @ref Magnum::Trade::SceneFieldData, enum @ref Magnum::Trade::SceneMappingType, @ref Magnum::Trade::SceneField, @ref Magnum::Trade::SceneFieldType, @ref Magnum::Trade::SceneFieldFlag, enum set @ref Magnum::Trade::SceneFieldFlags, function @ref Magnum::sceneMappingTypeSize(), @ref Magnum::sceneMappingTypeAlignment(), @ref Magnum::sceneFieldTypeSize(), @ref Magnum::sceneFieldTypeAlignment(), @ref Magnum::Trade::isSceneFieldCustom(), @ref Magnum::sceneFieldCustom()
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
};

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

/**
@brief Scene field name
@m_since_latest

See @ref SceneData for more information.
@see @ref SceneFieldData, @ref SceneFieldType,
    @ref AbstractImporter::sceneFieldForName(),
    @ref AbstractImporter::sceneFieldName()
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
     *
     * This field is allowed to have @ref SceneFieldFlag::TrivialField set,
     * which implies it has @cpp -1 @ce for all values.
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
    ImporterState,

    /**
     * This and all higher values are for importer-specific fields. Can be
     * of any type. See documentation of a particular importer for details.
     *
     * While it's unlikely to have billions of custom fields, the enum
     * intentionally reserves a full 31-bit range to avoid the need to remap
     * field identifiers coming from 3rd party ECS frameworks, for example.
     * @see @ref isSceneFieldCustom(), @ref sceneFieldCustom(SceneField),
     *      @ref sceneFieldCustom(UnsignedInt)
     */
    Custom = 0x80000000u
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
@see @ref sceneFieldCustom(UnsignedInt), @ref sceneFieldCustom(SceneField)
*/
constexpr bool isSceneFieldCustom(SceneField name) {
    return UnsignedInt(name) >= UnsignedInt(SceneField::Custom);
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
constexpr SceneField sceneFieldCustom(UnsignedInt id) {
    return CORRADE_CONSTEXPR_ASSERT(id < UnsignedInt(SceneField::Custom),
        "Trade::sceneFieldCustom(): index" << id << "too large"),
        SceneField(UnsignedInt(SceneField::Custom) + id);
}

/**
@brief Get index of a custom scene field
@m_since_latest

Inverse to @ref sceneFieldCustom(UnsignedInt). Expects that the field is
custom.
@see @ref isSceneFieldCustom()
*/
constexpr UnsignedInt sceneFieldCustom(SceneField name) {
    return CORRADE_CONSTEXPR_ASSERT(isSceneFieldCustom(name),
        "Trade::sceneFieldCustom():" << name << "is not custom"),
        UnsignedInt(name) - UnsignedInt(SceneField::Custom);
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

    /* 1 reserved for Bool (Bit?), which needs [Strided]BitArray[View] first */

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

    /**
     * @cpp const void* @ce, type is not preserved. For convenience it's
     * possible to retrieve the value by calling @cpp field<const T*>() @ce
     * with an arbitrary `T` but the user has to ensure the type is correct.
     */
    Pointer,

    /**
     * @cpp void* @ce, type is not preserved. For convenience it's possible to
     * retrieve the value by calling @cpp field<T*>() @ce with an arbitrary `T`
     * but the user has to ensure the type is correct.
     */
    MutablePointer,
};

/**
@debugoperatorenum{SceneFieldType}
@m_since_latest
*/
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, SceneFieldType value);

/**
@brief Size of given scene field type
@m_since_latest

@see @ref sceneFieldTypeAlignment()
*/
MAGNUM_TRADE_EXPORT UnsignedInt sceneFieldTypeSize(SceneFieldType type);

/**
@brief Alignment of given scene field type
@m_since_latest

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
     * but the field is restricted to exactly one value for each object. If
     * this flag is set, the object mapping view is allowed to be
     * @cpp nullptr @ce.
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
     * The field has a trivial content. Currently allowed only for
     * @ref SceneField::Parent, indicating all entries are @cpp -1 @ce. If this
     * flag is set, the field view is allowed to be @cpp nullptr @ce.
     */
    TrivialField = 1 << 3
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

Convenience type for populating @ref SceneData, see its documentation for an
introduction.

@section Trade-SceneFieldData-usage Usage

The most straightforward usage is constructing an instance from a
@ref SceneField and a strided view for the field data and object mapping. The
@ref SceneMappingType and @ref SceneFieldType get inferred from the view types:

@snippet MagnumTrade.cpp SceneFieldData-usage

Alternatively, you can pass typeless @cpp const void @ce or 2D views and supply
@ref SceneMappingType and @ref SceneFieldType explicitly.

@subsection Trade-SceneFieldData-usage-offset-only Offset-only field data

If the actual field / object data location is not known yet, the instance can
be created as "offset-only", meaning the actual view gets created only later
when passed to a @ref SceneData instance with a concrete data array. This is
useful mainly to avoid pointer patching during data serialization, less so when
the data layout is static (and thus can be defined at compile time), but the
actual data is allocated / populated at runtime:

@snippet MagnumTrade.cpp SceneFieldData-usage-offset-only

Offset-only fields are marked with @ref SceneFieldFlag::OffsetOnly in
@ref flags().

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
@relativeref{SceneFieldFlag,OrderedMapping}. Furthermore, to avoid having to
generate such mapping data, the mapping view can be @cpp nullptr @ce if this
flag is present. The view however still needs to have a size matching the field
data size and the same @ref SceneMappingType as other fields passed to the
@link SceneData @endlink:

@snippet MagnumTrade.cpp SceneFieldData-usage-implicit-mapping

Fields that are both @ref SceneFieldFlag::OffsetOnly and
@ref SceneFieldFlag::ImplicitMapping have their object mapping data always
ignored as it's not possible to know whether the offset points to actual data
or not.

@subsection Trade-SceneFieldData-usage-trivial-field Trivial fields

The @ref SceneField::Parent can be annotated with
@ref SceneFieldFlag::TrivialField, which implies that all nodes are in scene
root. While similar effect could be achieved by repeating a @cpp -1 @ce using
zero stride, the main purpose of this flag is in combination with
@ref SceneFieldFlag::ImplicitMapping --- that way you can indicate that all
objects in the scene are top-level without having to explicitly supply any
field data:

@snippet MagnumTrade.cpp SceneFieldData-usage-trivial-parent
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
        constexpr explicit SceneFieldData() noexcept: _size{}, _name{}, _flags{}, _mappingType{}, _mappingStride{}, _mappingData{}, _fieldType{}, _fieldStride{}, _fieldArraySize{}, _fieldData{} {}

        /**
         * @brief Type-erased constructor
         * @param name              Field name
         * @param mappingType       Object mapping type
         * @param mappingData       Object mapping data
         * @param fieldType         Field type
         * @param fieldData         Field data
         * @param fieldArraySize    Field array size. Use @cpp 0 @ce for
         *      non-array fields.
         * @param flags             Field flags.
         *      @ref SceneFieldFlag::OffsetOnly is not allowed here.
         *
         * Expects that @p mappingData and @p fieldData have the same size,
         * @p fieldType corresponds to @p name and @p fieldArraySize is zero
         * for builtin fields.
         *
         * If @p flags contain @ref SceneFieldFlag::ImplicitMapping, the
         * @p mappingData can be a @cpp nullptr @ce view (although it still has
         * to follow other constraints regarding size and type). While
         * @ref SceneData::mapping() will return it as-is,
         * @relativeref{SceneData,mappingAsArray()} and
         * @relativeref{SceneData,mappingInto()} functions will generate its
         * contents on-the-fly.
         */
        constexpr explicit SceneFieldData(SceneField name, SceneMappingType mappingType, const Containers::StridedArrayView1D<const void>& mappingData, SceneFieldType fieldType, const Containers::StridedArrayView1D<const void>& fieldData, UnsignedShort fieldArraySize = 0, SceneFieldFlags flags = {}) noexcept;

        /** @overload */
        constexpr explicit SceneFieldData(SceneField name, SceneMappingType mappingType, const Containers::StridedArrayView1D<const void>& mappingData, SceneFieldType fieldType, const Containers::StridedArrayView1D<const void>& fieldData, SceneFieldFlags flags) noexcept: SceneFieldData{name, mappingType, mappingData, fieldType, fieldData, 0, flags} {}

        /**
         * @brief Constructor
         * @param name              Field name
         * @param mappingData       Object mapping data
         * @param fieldType         Field type
         * @param fieldData         Field data
         * @param fieldArraySize    Field array size. Use @cpp 0 @ce for
         *      non-array fields.
         * @param flags             Field flags.
         *      @ref SceneFieldFlag::OffsetOnly is not allowed here.
         *
         * Expects that @p mappingData and @p fieldData have the same size in
         * the first dimension, that the second dimension of @p mappingData is
         * contiguous and its size is either 1, 2, 4 or 8, corresponding to one
         * of the @ref SceneMappingType values, that the second dimension of
         * @p fieldData is contiguous and its size matches @p fieldType and
         * @p fieldArraySize and that @p fieldType corresponds to @p name and
         * @p fieldArraySize is zero for builtin attributes.
         *
         * If @p flags contain @ref SceneFieldFlag::ImplicitMapping, the
         * @p mappingData can be a @cpp nullptr @ce view (although it still has
         * to follow other constraints regarding size and type). While
         * @ref SceneData::mapping() will return it as-is,
         * @relativeref{SceneData,mappingAsArray()} and
         * @relativeref{SceneData,mappingInto()} functions will generate its
         * contents on-the-fly.
         */
        explicit SceneFieldData(SceneField name, const Containers::StridedArrayView2D<const char>& mappingData, SceneFieldType fieldType, const Containers::StridedArrayView2D<const char>& fieldData, UnsignedShort fieldArraySize = 0, SceneFieldFlags flags = {}) noexcept;

        /** @overload */
        explicit SceneFieldData(SceneField name, const Containers::StridedArrayView2D<const char>& mappingData, SceneFieldType fieldType, const Containers::StridedArrayView2D<const char>& fieldData, SceneFieldFlags flags) noexcept: SceneFieldData{name, mappingData, fieldType, fieldData, 0, flags} {}

        /**
         * @brief Constructor
         * @param name          Field name
         * @param mappingData   Object mapping data
         * @param fieldData     Field data
         * @param flags         Field flags. @ref SceneFieldFlag::OffsetOnly is
         *      not allowed here.
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
         * @param flags         Field flags. @ref SceneFieldFlag::OffsetOnly is
         *      not allowed here.
         *
         * Detects @ref SceneMappingType based on @p T and @ref SceneFieldType
         * based on @p U and calls @ref SceneFieldData(SceneField, SceneMappingType, const Containers::StridedArrayView1D<const void>&, SceneFieldType, const Containers::StridedArrayView1D<const void>&, UnsignedShort, SceneFieldFlags)
         * with the @p fieldData second dimension size passed to
         * @p fieldArraySize. Expects that the second dimension of @p fieldData
         * is contiguous. At the moment only custom fields can be arrays, which
         * means this function can't be used with a builtin @p name. See
         * @ref SceneFieldData(SceneField, const Containers::StridedArrayView1D<T>&, const Containers::StridedArrayView1D<U>&, SceneFieldFlags)
         * for details about @ref SceneMappingType and @ref SceneFieldType
         * detection.
         */
        template<class T, class U> constexpr explicit SceneFieldData(SceneField name, const Containers::StridedArrayView1D<T>& mappingData, const Containers::StridedArrayView2D<U>& fieldData, SceneFieldFlags flags = {}) noexcept;

        /** @overload */
        template<class T, class U> constexpr explicit SceneFieldData(SceneField name, const Containers::ArrayView<T>& mappingData, const Containers::StridedArrayView2D<U>& fieldData, SceneFieldFlags flags = {}) noexcept: SceneFieldData{name, Containers::stridedArrayView(mappingData), fieldData, flags} {}

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
         *
         * Instances created this way refer to offsets in unspecified
         * external scene data instead of containing the data views directly.
         * Useful when the location of the scene data array is not known at
         * field construction time. Expects that @p fieldType corresponds to
         * @p name and @p fieldArraySize is zero for builtin attributes.
         *
         * Note that due to the @cpp constexpr @ce nature of this constructor,
         * no @p mappingType checks against @p mappingStride or
         * @p fieldType / @p fieldArraySize checks against @p fieldStride can
         * be done. You're encouraged to use the @ref SceneFieldData(SceneField, SceneMappingType, const Containers::StridedArrayView1D<const void>&, SceneFieldType, const Containers::StridedArrayView1D<const void>&, UnsignedShort, SceneFieldFlags)
         * constructor if you want additional safeguards.
         *
         * If @p flags contain @ref SceneFieldFlag::ImplicitMapping, the
         * @p mappingOffset and @p mappingStride fields are ignored and the
         * object mapping is assumed to not be present (however you still have
         * to follow constraints regarding its type). The
         * @ref SceneData::mapping() will then return a @cpp nullptr @ce view,
         * and the @relativeref{SceneData,mappingAsArray()} and
         * @relativeref{SceneData,mappingInto()} functions will generate its
         * contents on-the-fly.
         * @see @ref flags(), @ref fieldArraySize(),
         *      @ref mappingData(Containers::ArrayView<const void>) const,
         *      @ref fieldData(Containers::ArrayView<const void>) const
         */
        explicit constexpr SceneFieldData(SceneField name, std::size_t size, SceneMappingType mappingType, std::size_t mappingOffset, std::ptrdiff_t mappingStride, SceneFieldType fieldType, std::size_t fieldOffset, std::ptrdiff_t fieldStride, UnsignedShort fieldArraySize = 0, SceneFieldFlags flags = {}) noexcept;

        /** @overload */
        explicit constexpr SceneFieldData(SceneField name, std::size_t size, SceneMappingType mappingType, std::size_t mappingOffset, std::ptrdiff_t mappingStride, SceneFieldType fieldType, std::size_t fieldOffset, std::ptrdiff_t fieldStride, SceneFieldFlags flags) noexcept: SceneFieldData{name, size, mappingType, mappingOffset, mappingStride, fieldType, fieldOffset, fieldStride, 0, flags} {}

        /** @brief Field flags */
        constexpr SceneFieldFlags flags() const { return _flags; }

        /** @brief Field name */
        constexpr SceneField name() const { return _name; }

        /** @brief Number of entries */
        constexpr UnsignedLong size() const { return _size; }

        /** @brief Object mapping type */
        constexpr SceneMappingType mappingType() const { return _mappingType; }

        /**
         * @brief Type-erased object mapping data
         *
         * Expects that the field does not have @ref SceneFieldFlag::OffsetOnly
         * set, in that case use the @ref mappingData(Containers::ArrayView<const void>) const
         * overload instead.
         * @see @ref flags()
         */
        constexpr Containers::StridedArrayView1D<const void> mappingData() const {
            return Containers::StridedArrayView1D<const void>{
                /* We're *sure* the view is correct, so faking the view size */
                /** @todo better ideas for the StridedArrayView API? */
                {_mappingData.pointer, ~std::size_t{}}, _size,
                (CORRADE_CONSTEXPR_ASSERT(!(_flags & SceneFieldFlag::OffsetOnly), "Trade::SceneFieldData::mappingData(): the field is offset-only, supply a data array"), _mappingStride)};
        }

        /**
         * @brief Type-erased object mapping data for an offset-only attribute
         *
         * If the field does not have @ref SceneFieldFlag::OffsetOnly set, the
         * @p data parameter is ignored.
         * @see @ref flags(), @ref mappingData() const
         */
        Containers::StridedArrayView1D<const void> mappingData(Containers::ArrayView<const void> data) const {
            return Containers::StridedArrayView1D<const void>{
                /* We're *sure* the view is correct, so faking the view size */
                /** @todo better ideas for the StridedArrayView API? */
                data, _flags & SceneFieldFlag::OffsetOnly ? reinterpret_cast<const char*>(data.data()) + _mappingData.offset : _mappingData.pointer, _size, _mappingStride};
        }

        /** @brief Field type */
        constexpr SceneFieldType fieldType() const { return _fieldType; }

        /** @brief Field array size */
        constexpr UnsignedShort fieldArraySize() const { return _fieldArraySize; }

        /**
         * @brief Type-erased field data
         *
         * Expects that the field does not have @ref SceneFieldFlag::OffsetOnly
         * set, in that case use the @ref fieldData(Containers::ArrayView<const void>) const
         * overload instead.
         * @see @ref flags()
         */
        constexpr Containers::StridedArrayView1D<const void> fieldData() const {
            return Containers::StridedArrayView1D<const void>{
                /* We're *sure* the view is correct, so faking the view size */
                /** @todo better ideas for the StridedArrayView API? */
                {_fieldData.pointer, ~std::size_t{}}, _size,
                (CORRADE_CONSTEXPR_ASSERT(!(_flags & SceneFieldFlag::OffsetOnly), "Trade::SceneFieldData::fieldData(): the field is offset-only, supply a data array"), _fieldStride)};
        }

        /**
         * @brief Type-erased field data for an offset-only attribute
         *
         * If the field does not have @ref SceneFieldFlag::OffsetOnly set, the
         * @p data parameter is ignored.
         * @see @ref flags(), @ref fieldData() const
         */
        Containers::StridedArrayView1D<const void> fieldData(Containers::ArrayView<const void> data) const {
            return Containers::StridedArrayView1D<const void>{
                /* We're *sure* the view is correct, so faking the view size */
                /** @todo better ideas for the StridedArrayView API? */
                data, _flags & SceneFieldFlag::OffsetOnly ? reinterpret_cast<const char*>(data.data()) + _fieldData.offset : _fieldData.pointer, _size, _fieldStride};
        }

    private:
        friend SceneData;

        union Data {
            /* FFS C++ why this doesn't JUST WORK goddamit?! It's already past
               the End Of Times AND YET this piece of complex shit can't do the
               obvious! */
            constexpr Data(const void* pointer = nullptr): pointer{pointer} {}
            constexpr Data(std::size_t offset): offset{offset} {}

            const void* pointer;
            std::size_t offset;
        };

        UnsignedLong _size;
        SceneField _name;
        SceneFieldFlags _flags;
        SceneMappingType _mappingType;
        Short _mappingStride;
        Data _mappingData;

        SceneFieldType _fieldType;
        Short _fieldStride;
        UnsignedShort _fieldArraySize;
        /* 2 bytes free */
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
returned from @ref AbstractImporter::scene().

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

@snippet MagnumTrade.cpp SceneData-usage1

<b></b>

@m_class{m-noindent}

Next we go through objects that have an associated parent using
@ref parentsAsArray(). Those are the actual nodes we want, so we allocate a
scene graph object for each ...

@snippet MagnumTrade.cpp SceneData-usage2

@m_class{m-noindent}

<b></b>

... and then we assign a proper parent, or add it directly to the scene if the
parent is @cpp -1 @ce. We do this in a separate pass to ensure the parent
object is already allocated by the time we pass it to
@ref SceneGraph::Object::setParent() --- generally there's no guarantee that a
parent appears in the field before its children.

@snippet MagnumTrade.cpp SceneData-usage3

With the hierarchy done, we assign transformations. The transformation field
can be present for only a subset of the nodes, with the rest implicitly having
an indentity transformation, but it can also be present for objects that aren't
nodes, so we only set it for objects present in our hierarchy. The
@ref transformations3DAsArray() function also conveniently converts separate
transformation / rotation / scaling fields into a matrix for us, if the scene
contains only those.

@snippet MagnumTrade.cpp SceneData-usage4

Finally, assuming there's a `Drawable` class derived from
@ref SceneGraph::Drawable that accepts a mesh and material ID (retrieving them
subsequently from @ref AbstractImporter::mesh() /
@relativeref{AbstractImporter,material()}, for example), the process of
assigning actual meshes to corresponding scene nodes is just another
@cpp for @ce loop over @ref meshesMaterialsAsArray():

@snippet MagnumTrade.cpp SceneData-usage5

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
@ref mapping() and @ref field() accessors.

However, since each @ref SceneField can be in a variety of types, you're
expected to either check that the type is indeed what you expect using
@ref fieldType(SceneField) const, or at least check with documentation of the
corresponding importer. For example, because glTF files represent the scene
in a textual form, @ref CgltfImporter will always parse the data into canonical
32-bit types. With that assumption, the above snippet that used
@ref transformations3DAsArray() can be rewritten to a zero-copy form like this:

@snippet MagnumTrade.cpp SceneData-usage-advanced

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

@snippet MagnumTrade.cpp SceneData-per-object

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

@snippet MagnumTrade.cpp SceneData-usage-mutable

@section Trade-SceneData-populating Populating an instance

The actual data in a @ref SceneData instance are represented as a single block
of contiguous memory, which all object and field views point to. This is
easiest to achieve with an @relativeref{Corrade,Containers::ArrayTuple}. In the
example below, all objects have a parent and a transformation field, which are
stored together in a @cpp struct @ce, while a subset of them has a mesh and a
material assigned, which are stored in separate arrays. And because the scene
is small, we save space by using just 16-bit indices for everything.

@snippet MagnumTrade.cpp SceneData-populating

Note that the above layout is just an example, you're free to choose any
representation that matches your use case best, with fields interleaved
together or not. See also the @ref SceneFieldData class documentation for
additional ways how to specify and annotate the data.

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

@snippet MagnumTrade.cpp SceneData-populating-custom1

Then, similarly as with @ref MeshData, the scene can have custom fields as
well, created with @ref sceneFieldCustom(). We create one for the cell light
reference array and one for the cell frustum and then use them to annotate
the views allocated above. Note that we also increased the total object count
to include the light culling grid cells as well.

@snippet MagnumTrade.cpp SceneData-populating-custom2

Later, the fields can be retrieved back using the same custom identifiers.
The light references are actually a 2D array (8 lights for each cell), so a
@cpp [] @ce needs to be used:

@snippet MagnumTrade.cpp SceneData-populating-custom-retrieve

@see @ref AbstractImporter::scene()
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
         * @brief Size of given field
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
         * @ref SceneField is an array attribute. Note that this is different
         * from the count of entries for given field, which is exposed through
         * @ref fieldSize(). See @ref Trade-SceneData-populating-custom for an
         * example.
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
         * If @p name doesn't exist, returns @ref Containers::NullOpt. The
         * lookup is done in an @f$ \mathcal{O}(n) @f$ complexity with
         * @f$ n @f$ being the field count.
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
         * @see @ref is2D(), @ref is3D()
         */
        bool hasField(SceneField name) const;

        /**
         * @brief Find offset of an object in given field
         * @m_since_latest
         *
         * If @p object isn't present in @p fieldId starting at @p offset,
         * returns @ref Containers::NullOpt. The @p fieldId is expected to be
         * smaller than @ref fieldCount(), @p object smaller than
         * @ref mappingBound() and @p offset not larger than
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
         * returns @ref Containers::NullOpt. The @p fieldName is expected to
         * exist, @p object is expected to be smaller than @ref mappingBound()
         * and @p offset not be larger than @ref fieldSize(SceneField) const.
         *
         * If the field has @ref SceneFieldFlag::ImplicitMapping, the lookup is
         * done in an @f$ \mathcal{O}(m) @f$ complexity with @f$ m @f$ being
         * the * field count. Otherwise, if the field has
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
         * @brief Field flags
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
         * @brief Number of entries for given named field
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
         * The @p name is expected to exist.
         * @see @ref hasField(), @ref fieldArraySize(UnsignedInt) const
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
         * type.
         *
         * If the field has @ref SceneFieldFlag::ImplicitMapping set and no
         * data was supplied for it or it's @ref SceneFieldFlag::OffsetOnly,
         * the returned view will be correctly sized but @cpp nullptr @ce.
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
         * If the field has @ref SceneFieldFlag::ImplicitMapping set and either
         * no data was supplied for it or it's @ref SceneFieldFlag::OffsetOnly,
         * the returned view will be correctly sized but @cpp nullptr @ce.
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
         * The @p fieldName is expected to exist. The second dimension
         * represents the actual data type (its size is equal to
         * @ref SceneMappingType size) and is guaranteed to be contiguous. Use
         * the templated overload below to get the object mapping in a concrete
         * type.
         *
         * If the field has @ref SceneFieldFlag::ImplicitMapping set and either
         * no data was supplied for it or it's @ref SceneFieldFlag::OffsetOnly,
         * the returned view will be correctly sized but @cpp nullptr @ce.
         * @see @ref hasField(), @ref mapping(UnsignedInt) const,
         *      @ref mutableMapping(SceneField),
         *      @ref Corrade::Containers::StridedArrayView::isContiguous()
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
         * If the field has @ref SceneFieldFlag::ImplicitMapping set and either
         * no data was supplied for it or it's @ref SceneFieldFlag::OffsetOnly,
         * the returned view will be correctly sized but @cpp nullptr @ce.
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
         *      @ref mutableMapping(UnsignedInt)
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
        template<class T, class = typename std::enable_if<!std::is_array<T>::value>::type> Containers::StridedArrayView1D<const T> field(UnsignedInt id) const;

        /**
         * @brief Data for given array field in a concrete type
         * @m_since_latest
         *
         * Same as above, except that it works with array fields instead ---
         * you're expected to select this overload by passing @cpp T[] @ce
         * instead of @cpp T @ce. The second dimension is guaranteed to be
         * contiguous and have the same size as reported by
         * @ref fieldArraySize(UnsignedInt) const for given field.
         */
        template<class T, class = typename std::enable_if<std::is_array<T>::value>::type> Containers::StridedArrayView2D<const typename std::remove_extent<T>::type> field(UnsignedInt id) const;

        /**
         * @brief Mutable data for given field in a concrete type
         * @m_since_latest
         *
         * Like @ref field(UnsignedInt) const, but returns a mutable view.
         * Expects that the scene is mutable.
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
         * @ref fieldArraySize(UnsignedInt) const for given field.
         */
        template<class T, class = typename std::enable_if<std::is_array<T>::value>::type> Containers::StridedArrayView2D<typename std::remove_extent<T>::type> mutableField(UnsignedInt id);

        /**
         * @brief Data for given named field
         * @m_since_latest
         *
         * The @p name is expected to exist. The second dimension represents
         * the actual data type (its size is equal to @ref SceneFieldType size,
         * possibly multiplied by array size) and is guaranteed to be
         * contiguous. Use the templated overload below to get the field in a
         * concrete type.
         * @see @ref hasField(), @ref field(UnsignedInt) const,
         *      @ref mutableField(SceneField),
         *      @ref Corrade::Containers::StridedArrayView::isContiguous()
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
         * @see @ref field(UnsignedInt) const, @ref mutableField(SceneField)
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
         * @ref fieldArraySize(SceneField) const for given field.
         */
        template<class T, class = typename std::enable_if<std::is_array<T>::value>::type> Containers::StridedArrayView2D<const typename std::remove_extent<T>::type> field(SceneField name) const;

        /**
         * @brief Mutable data for given named field
         * @m_since_latest
         *
         * Like @ref field(SceneField) const, but returns a mutable view.
         * Expects that the scene is mutable.
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
         * @ref fieldArraySize(SceneField) const for given field.
         */
        template<class T, class = typename std::enable_if<std::is_array<T>::value>::type> Containers::StridedArrayView2D<typename std::remove_extent<T>::type> mutableField(SceneField name);

        /**
         * @brief Object mapping for given field as 32-bit integers
         * @m_since_latest
         *
         * Convenience alternative to the templated
         * @ref mapping(UnsignedInt) const that converts the field from an
         * arbitrary underlying type and returns it in a newly-allocated array.
         * The @p fieldId is expected to be smaller than @ref fieldCount().
         *
         * If the field has @ref SceneFieldFlag::ImplicitMapping set and either
         * no data was supplied for it or it's @ref SceneFieldFlag::OffsetOnly,
         * the data will be generated on-the-fly.
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
         * parent for @p object, returns @ref Containers::NullOpt. If @p object
         * is top-level, returns @cpp -1 @ce.
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
         * returns @ref Containers::NullOpt.
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
         * @ref Containers::NullOpt.
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
         * returns @ref Containers::NullOpt.
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
         * @ref Containers::NullOpt.
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
         * @ref Containers::NullOpt.
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
         * Releases the ownership of the data array and resets internal
         * field-related state to default. The scene then behaves like it has
         * no fields and no data. If you want to release field data as well,
         * first call @ref releaseFieldData() and then this function.
         *
         * Note that the returned array has a custom no-op deleter when the
         * data are not owned by the scene, and while the returned array type
         * is mutable, the actual memory might be not.
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

        /* Like objects() / field(), but returning just a 1D view, sliced from
           offset to offset + size. The parameterless overloads are equal to
           offset = 0 and size = field.size(). */
        MAGNUM_TRADE_LOCAL Containers::StridedArrayView1D<const void> fieldDataMappingViewInternal(const SceneFieldData& field, std::size_t offset, std::size_t size) const;
        MAGNUM_TRADE_LOCAL Containers::StridedArrayView1D<const void> fieldDataMappingViewInternal(const SceneFieldData& field) const;
        MAGNUM_TRADE_LOCAL Containers::StridedArrayView1D<const void> fieldDataFieldViewInternal(const SceneFieldData& field, std::size_t offset, std::size_t size) const;
        MAGNUM_TRADE_LOCAL Containers::StridedArrayView1D<const void> fieldDataFieldViewInternal(const SceneFieldData& field) const;

        #ifndef CORRADE_NO_ASSERT
        template<class T> bool checkFieldTypeCompatibility(const SceneFieldData& attribute, const char* prefix) const;
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
    #define _c(type_) template<> struct SceneFieldTypeFor<type_> {          \
        constexpr static SceneFieldType type() {                            \
            return SceneFieldType::type_;                                   \
        }                                                                   \
    };
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
    #undef _c
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

    constexpr bool isSceneFieldAllowedTrivial(SceneField name) {
        return name == SceneField::Parent;
    }
}

constexpr SceneFieldData::SceneFieldData(const SceneField name, const SceneMappingType mappingType, const Containers::StridedArrayView1D<const void>& mappingData, const SceneFieldType fieldType, const Containers::StridedArrayView1D<const void>& fieldData, const UnsignedShort fieldArraySize, const SceneFieldFlags flags) noexcept:
    _size{(CORRADE_CONSTEXPR_ASSERT(mappingData.size() == fieldData.size(),
        "Trade::SceneFieldData: expected" << name << "mapping and field view to have the same size but got" << mappingData.size() << "and" << fieldData.size()), mappingData.size())},
    _name{(CORRADE_CONSTEXPR_ASSERT(Implementation::isSceneFieldTypeCompatibleWithField(name, fieldType),
        "Trade::SceneFieldData:" << fieldType << "is not a valid type for" << name), name)},
    _flags{(CORRADE_CONSTEXPR_ASSERT(!(flags & SceneFieldFlag::OffsetOnly),
        "Trade::SceneFieldData: can't pass Trade::SceneFieldFlag::OffsetOnly for a view"),
        CORRADE_CONSTEXPR_ASSERT(!(flags & SceneFieldFlag::TrivialField) || Implementation::isSceneFieldAllowedTrivial(name),
        "Trade::SceneFieldData: can't pass Trade::SceneFieldFlag::TrivialField for" << name),
        flags)},
    _mappingType{mappingType},
    _mappingStride{(CORRADE_CONSTEXPR_ASSERT(mappingData.stride() >= -32768 && mappingData.stride() <= 32767,
        "Trade::SceneFieldData: expected mapping view stride to fit into 16 bits, but got" << mappingData.stride()), Short(mappingData.stride()))},
    _mappingData{mappingData.data()},
    _fieldType{fieldType},
    _fieldStride{(CORRADE_CONSTEXPR_ASSERT(fieldData.stride() >= -32768 && fieldData.stride() <= 32767,
        "Trade::SceneFieldData: expected field view stride to fit into 16 bits, but got" << fieldData.stride()), Short(fieldData.stride()))},
    _fieldArraySize{(CORRADE_CONSTEXPR_ASSERT(!fieldArraySize || Implementation::isSceneFieldArrayAllowed(name),
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

constexpr SceneFieldData::SceneFieldData(const SceneField name, const std::size_t size, const SceneMappingType mappingType, const std::size_t mappingOffset, const std::ptrdiff_t mappingStride, const SceneFieldType fieldType, const std::size_t fieldOffset, const std::ptrdiff_t fieldStride, const UnsignedShort fieldArraySize, const SceneFieldFlags flags) noexcept:
    _size{size},
    _name{(CORRADE_CONSTEXPR_ASSERT(Implementation::isSceneFieldTypeCompatibleWithField(name, fieldType),
        "Trade::SceneFieldData:" << fieldType << "is not a valid type for" << name), name)},
    _flags{(
        CORRADE_CONSTEXPR_ASSERT(!(flags & SceneFieldFlag::TrivialField) || Implementation::isSceneFieldAllowedTrivial(name),
        "Trade::SceneFieldData: can't pass Trade::SceneFieldFlag::TrivialField for" << name),
        flags|SceneFieldFlag::OffsetOnly)},
    _mappingType{mappingType},
    _mappingStride{(CORRADE_CONSTEXPR_ASSERT(mappingStride >= -32768 && mappingStride <= 32767,
        "Trade::SceneFieldData: expected mapping view stride to fit into 16 bits, but got" << mappingStride), Short(mappingStride))},
    _mappingData{mappingOffset},
    _fieldType{fieldType},
    _fieldStride{(CORRADE_CONSTEXPR_ASSERT(fieldStride >= -32768 && fieldStride <= 32767,
        "Trade::SceneFieldData: expected field view stride to fit into 16 bits, but got" << fieldStride), Short(fieldStride))},
    _fieldArraySize{(CORRADE_CONSTEXPR_ASSERT(!fieldArraySize || Implementation::isSceneFieldArrayAllowed(name),
        "Trade::SceneFieldData:" << name << "can't be an array field"), fieldArraySize)},
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
template<class T> bool SceneData::checkFieldTypeCompatibility(const SceneFieldData& field, const char* const prefix) const {
    CORRADE_ASSERT(Implementation::SceneFieldTypeFor<typename std::remove_extent<T>::type>::type() == field._fieldType,
        prefix << field._name << "is" << field._fieldType << "but requested a type equivalent to" << Implementation::SceneFieldTypeFor<typename std::remove_extent<T>::type>::type(), false);
    if(field._fieldArraySize) CORRADE_ASSERT(std::is_array<T>::value,
        prefix << field._name << "is an array field, use T[] to access it", false);
    else CORRADE_ASSERT(!std::is_array<T>::value,
        prefix << field._name << "is not an array field, can't use T[] to access it", false);
    return true;
}
#endif

template<class T, class> Containers::StridedArrayView1D<const T> SceneData::field(const UnsignedInt id) const {
    Containers::StridedArrayView2D<const char> data = field(id);
    #ifdef CORRADE_GRACEFUL_ASSERT /* Sigh. Brittle. Better idea? */
    if(!data.stride()[1]) return {};
    #endif
    #ifndef CORRADE_NO_ASSERT
    if(!checkFieldTypeCompatibility<T>(_fields[id], "Trade::SceneData::field():")) return {};
    #endif
    return Containers::arrayCast<1, const T>(data);
}

template<class T, class> Containers::StridedArrayView2D<const typename std::remove_extent<T>::type> SceneData::field(const UnsignedInt id) const {
    Containers::StridedArrayView2D<const char> data = field(id);
    #ifdef CORRADE_GRACEFUL_ASSERT /* Sigh. Brittle. Better idea? */
    if(!data.stride()[1]) return {};
    #endif
    #ifndef CORRADE_NO_ASSERT
    if(!checkFieldTypeCompatibility<T>(_fields[id], "Trade::SceneData::field():")) return {};
    #endif
    return Containers::arrayCast<2, const typename std::remove_extent<T>::type>(data);
}

template<class T, class> Containers::StridedArrayView1D<T> SceneData::mutableField(const UnsignedInt id) {
    Containers::StridedArrayView2D<char> data = mutableField(id);
    #ifdef CORRADE_GRACEFUL_ASSERT /* Sigh. Brittle. Better idea? */
    if(!data.stride()[1]) return {};
    #endif
    #ifndef CORRADE_NO_ASSERT
    if(!checkFieldTypeCompatibility<T>(_fields[id], "Trade::SceneData::mutableField():")) return {};
    #endif
    return Containers::arrayCast<1, T>(data);
}

template<class T, class> Containers::StridedArrayView2D<typename std::remove_extent<T>::type> SceneData::mutableField(const UnsignedInt id) {
    Containers::StridedArrayView2D<char> data = mutableField(id);
    #ifdef CORRADE_GRACEFUL_ASSERT /* Sigh. Brittle. Better idea? */
    if(!data.stride()[1]) return {};
    #endif
    #ifndef CORRADE_NO_ASSERT
    if(!checkFieldTypeCompatibility<T>(_fields[id], "Trade::SceneData::mutableField():")) return {};
    #endif
    return Containers::arrayCast<2, typename std::remove_extent<T>::type>(data);
}

template<class T, class> Containers::StridedArrayView1D<const T> SceneData::field(const SceneField name) const {
    Containers::StridedArrayView2D<const char> data = field(name);
    #ifdef CORRADE_GRACEFUL_ASSERT /* Sigh. Brittle. Better idea? */
    if(!data.stride()[1]) return {};
    #endif
    #ifndef CORRADE_NO_ASSERT
    if(!checkFieldTypeCompatibility<T>(_fields[findFieldIdInternal(name)], "Trade::SceneData::field():")) return {};
    #endif
    return Containers::arrayCast<1, const T>(data);
}

template<class T, class> Containers::StridedArrayView2D<const typename std::remove_extent<T>::type> SceneData::field(const SceneField name) const {
    Containers::StridedArrayView2D<const char> data = field(name);
    #ifdef CORRADE_GRACEFUL_ASSERT /* Sigh. Brittle. Better idea? */
    if(!data.stride()[1]) return {};
    #endif
    #ifndef CORRADE_NO_ASSERT
    if(!checkFieldTypeCompatibility<T>(_fields[findFieldIdInternal(name)], "Trade::SceneData::field():")) return {};
    #endif
    return Containers::arrayCast<2, const typename std::remove_extent<T>::type>(data);
}

template<class T, class> Containers::StridedArrayView1D<T> SceneData::mutableField(const SceneField name) {
    Containers::StridedArrayView2D<char> data = mutableField(name);
    #ifdef CORRADE_GRACEFUL_ASSERT /* Sigh. Brittle. Better idea? */
    if(!data.stride()[1]) return {};
    #endif
    #ifndef CORRADE_NO_ASSERT
    if(!checkFieldTypeCompatibility<T>(_fields[findFieldIdInternal(name)], "Trade::SceneData::mutableField():")) return {};
    #endif
    return Containers::arrayCast<1, T>(data);
}

template<class T, class> Containers::StridedArrayView2D<typename std::remove_extent<T>::type> SceneData::mutableField(const SceneField name) {
    Containers::StridedArrayView2D<char> data = mutableField(name);
    #ifdef CORRADE_GRACEFUL_ASSERT /* Sigh. Brittle. Better idea? */
    if(!data.stride()[1]) return {};
    #endif
    #ifndef CORRADE_NO_ASSERT
    if(!checkFieldTypeCompatibility<T>(_fields[findFieldIdInternal(name)], "Trade::SceneData::mutableField():")) return {};
    #endif
    return Containers::arrayCast<2, typename std::remove_extent<T>::type>(data);
}

}}

#endif
