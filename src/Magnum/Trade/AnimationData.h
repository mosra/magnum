#ifndef Magnum_Trade_AnimationData_h
#define Magnum_Trade_AnimationData_h
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
 * @brief Class @ref Magnum::Trade::AnimationTrackData, @ref Magnum::Trade::AnimationData, enum @ref Magnum::Trade::AnimationTrackType, @ref Magnum::Trade::AnimationTrackTarget, function @ref Magnum::Trade::animationInterpolatorFor()
 */

#include "Magnum/Magnum.h"
#include "Magnum/Math/Math.h"
#include "Magnum/Animation/Track.h"
#include "Magnum/Trade/Data.h"
#include "Magnum/Trade/Trade.h"
#include "Magnum/Trade/visibility.h"

namespace Magnum { namespace Trade {

/**
@brief Type of animation track data

A type in which track data for given @ref AnimationTrackTarget is stored, See
@ref AnimationData for more information.
@see @ref AnimationData, @ref animationTrackTypeSize(),
    @ref animationTrackTypeAlignment()
@experimental
*/
enum class AnimationTrackType: UnsignedByte {
    /* Zero used for an invalid value */

    Bool = 1,           /**< @cpp bool @ce <b></b> */
    Float,              /**< @ref Magnum::Float "Float" */
    UnsignedInt,        /**< @ref Magnum::UnsignedInt "UnsignedInt" */
    Int,                /**< @ref Magnum::Int "Int" */

    /**
     * @relativeref{Magnum,BitVector2}
     * @m_since_latest
     */
    BitVector2,

    #ifdef MAGNUM_BUILD_DEPRECATED
    /**
     * @relativeref{Magnum,BitVector2}
     * @m_deprecated_since_latest Use @ref AnimationTrackType::BitVector2
     *      instead.
     */
    BoolVector2 CORRADE_DEPRECATED_ENUM("use BitVector2 instead") = BitVector2,
    #endif

    /**
     * @relativeref{Magnum,BitVector3}
     * @m_since_latest
     */
    BitVector3,

    #ifdef MAGNUM_BUILD_DEPRECATED
    /**
     * @relativeref{Magnum,BitVector3}
     * @m_deprecated_since_latest Use @ref AnimationTrackType::BitVector3
     *      instead.
     */
    BoolVector3 CORRADE_DEPRECATED_ENUM("use BitVector3 instead") = BitVector3,
    #endif

    /**
     * @relativeref{Magnum,BitVector4}
     * @m_since_latest
     */
    BitVector4,

    #ifdef MAGNUM_BUILD_DEPRECATED
    /**
     * @relativeref{Magnum,BitVector4}
     * @m_deprecated_since_latest Use @ref AnimationTrackType::BitVector4
     *      instead.
     */
    BoolVector4 CORRADE_DEPRECATED_ENUM("use BitVector4 instead") = BitVector4,
    #endif

    /**
     * @ref Magnum::Vector2 "Vector2". Usually used for
     * @ref AnimationTrackTarget::Translation2D and
     * @ref AnimationTrackTarget::Scaling2D.
     */
    Vector2,

    Vector2ui,          /**< @ref Magnum::Vector2ui "Vector2ui" */
    Vector2i,           /**< @ref Magnum::Vector2i "Vector2i" */

    /**
     * @ref Magnum::Vector3 "Vector3". Usually used for
     * @ref AnimationTrackTarget::Translation3D and
     * @ref AnimationTrackTarget::Scaling3D.
     */
    Vector3,

    Vector3ui,          /**< @ref Magnum::Vector3ui "Vector3ui" */
    Vector3i,           /**< @ref Magnum::Vector3i "Vector3i" */
    Vector4,            /**< @ref Magnum::Vector4 "Vector4" */
    Vector4ui,          /**< @ref Magnum::Vector4ui "Vector4ui" */
    Vector4i,           /**< @ref Magnum::Vector4i "Vector4i" */

    /**
     * @ref Magnum::Complex "Complex". Usually used for
     * @ref AnimationTrackTarget::Rotation2D.
     */
    Complex,

    /**
     * @ref Magnum::Quaternion "Quaternion". Usually used for
     * @ref AnimationTrackTarget::Rotation3D.
     */
    Quaternion,

    DualQuaternion,     /**< @ref Magnum::DualQuaternion "DualQuaternion" */
    CubicHermite1D,     /**< @ref Magnum::CubicHermite1D "CubicHermite1D" */

    /**
     * @ref Magnum::CubicHermite2D "CubicHermite2D". Usually used for
     * spline-interpolated @ref AnimationTrackTarget::Translation2D and
     * @ref AnimationTrackTarget::Scaling2D.
     */
    CubicHermite2D,

    /**
     * @ref Magnum::CubicHermite3D "CubicHermite3D". Usually used for
     * spline-interpolated @ref AnimationTrackTarget::Translation3D and
     * @ref AnimationTrackTarget::Scaling3D.
     */
    CubicHermite3D,

    /**
     * @ref Magnum::CubicHermiteComplex "CubicHermiteComplex". Usually used for
     * spline-interpolated @ref AnimationTrackTarget::Rotation2D.
     */
    CubicHermiteComplex,

    /**
     * @ref Magnum::CubicHermiteQuaternion "CubicHermiteQuaternion". Usually
     * used for spline-interpolated @ref AnimationTrackTarget::Rotation3D.
     */
    CubicHermiteQuaternion
};

/** @debugoperatorenum{AnimationTrackType} */
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, AnimationTrackType value);

/**
@brief Size of given animation track data type
@m_since_latest

@see @ref animationTrackTypeAlignment()
*/
MAGNUM_TRADE_EXPORT UnsignedInt animationTrackTypeSize(AnimationTrackType type);

/**
@brief Alignment of given animation track data type
@m_since_latest

@see @ref animationTrackTypeSize()
*/
MAGNUM_TRADE_EXPORT UnsignedInt animationTrackTypeAlignment(AnimationTrackType type);

namespace Implementation {
    enum: UnsignedShort { AnimationTrackTargetCustom = 32768 };
}

/**
@brief Target of an animation track

See @ref AnimationData for more information.

Apart from builtin target types it's possible to have custom ones, which use
the upper half of the enum range. Those are detected via
@ref isAnimationTrackTargetCustom() and can be converted to and from a
numeric identifier using @ref animationTrackTargetCustom(AnimationTrackTarget)
and @ref animationTrackTargetCustom(UnsignedShort). Unlike the builtin ones,
these can be of any type and @ref AnimationData::trackTarget() might or might
not point to an existing object. An importer that exposes custom types then may
also provide a string mapping using
@ref AbstractImporter::animationTrackTargetForName() and
@ref AbstractImporter::animationTrackTargetName(). See documentation of a
particular importer for details.
@see @ref AnimationTrackData
@experimental
*/
enum class AnimationTrackTarget: UnsignedShort {
    /* Zero used for an invalid value */

    /**
     * Modifies 2D object translation. Type is usually
     * @ref Magnum::Vector2 "Vector2" or
     * @ref Magnum::CubicHermite2D "CubicHermite2D" for spline-interpolated
     * data.
     *
     * @see @ref AnimationTrackType::Vector2,
     *      @ref AnimationTrackType::CubicHermite2D,
     *      @ref SceneField::Translation, @ref SceneData::is2D()
     */
    Translation2D = 1,

    /**
     * Modifies 3D object translation. Type is usually
     * @ref Magnum::Vector3 "Vector3" or
     * @ref Magnum::CubicHermite3D "CubicHermite3D" for spline-interpolated
     * data.
     *
     * @see @ref AnimationTrackType::Vector3,
     *      @ref AnimationTrackType::CubicHermite3D,
     *      @ref SceneField::Translation, @ref SceneData::is3D()
     */
    Translation3D,

    /**
     * Modifies 2D object rotation. Type is usually
     * @ref Magnum::Complex "Complex" or
     * @ref Magnum::CubicHermiteComplex "CubicHermiteComplex" for
     * spline-interpolated data.
     *
     * @see @ref AnimationTrackType::Complex,
     *      @ref AnimationTrackType::CubicHermiteComplex,
     *      @ref SceneField::Rotation. @ref SceneData::is2D()
     */
    Rotation2D,

    /**
     * Modifies 3D object rotation. Type is usually
     * @ref Magnum::Quaternion "Quaternion" or
     * @ref Magnum::CubicHermiteQuaternion "CubicHermiteQuaternion" for
     * spline-interpolated data.
     *
     * @see @ref AnimationTrackType::Quaternion,
     *      @ref AnimationTrackType::CubicHermiteQuaternion,
     *      @ref SceneField::Rotation, @ref SceneData::is3D()
     */
    Rotation3D,

    /**
     * Modifies 2D object scaling. Type is usually
     * @ref Magnum::Vector2 "Vector2" or
     * @ref Magnum::CubicHermite2D "CubicHermite2D" for spline-interpolated
     * data.
     *
     * @see @ref AnimationTrackType::Vector2,
     *      @ref AnimationTrackType::CubicHermite2D,
     *      @ref SceneField::Scaling, @ref SceneData::is2D()
     */
    Scaling2D,

    /**
     * Modifies 3D object scaling. Type is usually
     * @ref Magnum::Vector3 "Vector3" or
     * @ref Magnum::CubicHermite3D "CubicHermite3D" for spline-interpolated
     * data.
     *
     * @see @ref AnimationTrackType::Vector3,
     *      @ref AnimationTrackType::CubicHermite3D,
     *      @ref SceneField::Scaling, @ref SceneData::is3D()
     */
    Scaling3D,

    #ifdef MAGNUM_BUILD_DEPRECATED
    /**
     * This and all higher values are for importer-specific targets.
     *
     * @m_deprecated_since_latest Use @ref animationTrackTargetCustom() and
     *      @ref isAnimationTrackTargetCustom() instead.
     */
    Custom CORRADE_DEPRECATED_ENUM("use animationTrackTargetCustom() instead") = Implementation::AnimationTrackTargetCustom
    #endif
};

#ifdef MAGNUM_BUILD_DEPRECATED
/**
@brief @copybrief AnimationTrackTarget
@m_deprecated_since_latest Use @ref AnimationTrackTarget instead.
*/
typedef CORRADE_DEPRECATED("use AnimationTrackTarget instead") AnimationTrackTarget AnimationTrackTargetType;
#endif

/** @debugoperatorenum{AnimationTrackTarget} */
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, AnimationTrackTarget value);

/**
@brief Whether a target for an animation track is custom
@m_since_latest

Returns @cpp true @ce if @p name has a value in the upper 15 bits of the enum
range, @cpp false @ce otherwise.
@see @ref animationTrackTargetCustom(UnsignedShort),
    @ref animationTrackTargetCustom(AnimationTrackTarget),
    @ref AbstractImporter::animationTrackTargetName()
*/
constexpr bool isAnimationTrackTargetCustom(AnimationTrackTarget name) {
    return UnsignedShort(name) >= Implementation::AnimationTrackTargetCustom;
}

/**
@brief Create a custom target for an animation track
@m_since_latest

Returns a custom animation track target with index @p id. The index is expected
to fit into 15 bits. Use
@ref animationTrackTargetCustom(AnimationTrackTarget) to get the index
back.
*/
/* Constexpr so it's usable for creating compile-time AnimationData
   instances */
constexpr AnimationTrackTarget animationTrackTargetCustom(UnsignedShort id) {
    return CORRADE_CONSTEXPR_ASSERT(id < Implementation::AnimationTrackTargetCustom,
        "Trade::animationTrackTargetCustom(): index" << id << "too large"),
        AnimationTrackTarget(Implementation::AnimationTrackTargetCustom + id);
}

/**
@brief Get index of a custom target for an animation track
@m_since_latest

Inverse to @ref animationTrackTargetCustom(UnsignedShort). Expects that the
type is custom.
@see @ref isAnimationTrackTargetCustom(),
    @ref AbstractImporter::animationTrackTargetName()
*/
constexpr UnsignedShort animationTrackTargetCustom(AnimationTrackTarget name) {
    return CORRADE_CONSTEXPR_ASSERT(isAnimationTrackTargetCustom(name),
        "Trade::animationTrackTargetCustom():" << name << "is not custom"),
        UnsignedShort(name) - Implementation::AnimationTrackTargetCustom;
}

/**
@brief Animation track data

Convenience type for populating @ref AnimationData. Has no accessors, as the
data are then accessible through @ref AnimationData APIs.
@experimental
*/
class MAGNUM_TRADE_EXPORT AnimationTrackData {
    public:
        /**
         * @brief Default constructor
         *
         * Leaves contents at unspecified values. Provided as a convenience for
         * initialization of the track array for @ref AnimationData, expected
         * to be replaced with concrete values later.
         */
        explicit AnimationTrackData() noexcept: _type{}, _resultType{}, _targetName{}, _interpolation{}, _before{}, _after{}, _target{}, _size{}, _keysStride{}, _valuesStride{}, _keysData{}, _valuesData{}, _interpolator{} {}

        /**
         * @brief Type-erased constructor with generic interpolation behavior
         * @param targetName    Track target name
         * @param target        Track target ID
         * @param type          Value type
         * @param resultType    Result type
         * @param keys          Frame keys
         * @param values        Frame values
         * @param interpolation Interpolation behavior
         * @param before        Extrapolation behavior before
         * @param after         Extrapolation behavior after
         * @m_since_latest
         *
         * The keyframe data are assumed to be stored in sorted order. It's not
         * an error to have two successive keyframes with the same frame value.
         * Expects that @p keys and @p values strides both fit into signed
         * 16-bit values, that they both have the same size and that keyframe
         * count fits into 32 bits.
         *
         * The interpolator function is picked implicitly for given
         * @p interpolation, @p type and @p resultType and the combination is
         * expected to make sense. Use
         * @ref AnimationTrackData(AnimationTrackTarget, UnsignedLong, AnimationTrackType, AnimationTrackType, const Containers::StridedArrayView1D<const Float>&, const Containers::StridedArrayView1D<const void>&, Animation::Interpolation, void(*)(), Animation::Extrapolation, Animation::Extrapolation)
         * to supply it explicitly.
         */
        explicit AnimationTrackData(AnimationTrackTarget targetName, UnsignedLong target, AnimationTrackType type, AnimationTrackType resultType, const Containers::StridedArrayView1D<const Float>& keys, const Containers::StridedArrayView1D<const void>& values, Animation::Interpolation interpolation, Animation::Extrapolation before, Animation::Extrapolation after) noexcept;

        /** @overload
         * @m_since_latest
         *
         * Equivalent to calling @ref AnimationTrackData(AnimationTrackTarget, UnsignedLong, AnimationTrackType, AnimationTrackType, const Containers::StridedArrayView1D<const Float>&, const Containers::StridedArrayView1D<const void>&, Animation::Interpolation, Animation::Extrapolation, Animation::Extrapolation)
         * with both @p type and @p resultType set to @p type.
         */
        explicit AnimationTrackData(AnimationTrackTarget targetName, UnsignedLong target, AnimationTrackType type, const Containers::StridedArrayView1D<const Float>& keys, const Containers::StridedArrayView1D<const void>& values, Animation::Interpolation interpolation, Animation::Extrapolation before, Animation::Extrapolation after) noexcept: AnimationTrackData{targetName, target, type, type, keys, values, interpolation, before, after} {}

        /** @overload
         * @m_since_latest
         *
         * Equivalent to calling @ref AnimationTrackData(AnimationTrackTarget, UnsignedLong, AnimationTrackType, AnimationTrackType, const Containers::StridedArrayView1D<const Float>&, const Containers::StridedArrayView1D<const void>&, Animation::Interpolation, Animation::Extrapolation, Animation::Extrapolation)
         * with both @p before and @p after set to @p extrapolation.
         */
        explicit AnimationTrackData(AnimationTrackTarget targetName, UnsignedLong target, AnimationTrackType type, AnimationTrackType resultType, const Containers::StridedArrayView1D<const Float>& keys, const Containers::StridedArrayView1D<const void>& values, Animation::Interpolation interpolation, Animation::Extrapolation extrapolation = Animation::Extrapolation::Constant) noexcept: AnimationTrackData{targetName, target, type, resultType, keys, values, interpolation, extrapolation, extrapolation} {}

        /** @overload
         * @m_since_latest
         *
         * Equivalent to calling @ref AnimationTrackData(AnimationTrackTarget, UnsignedLong, AnimationTrackType, AnimationTrackType, const Containers::StridedArrayView1D<const Float>&, const Containers::StridedArrayView1D<const void>&, Animation::Interpolation, Animation::Extrapolation, Animation::Extrapolation)
         * with both @p type and @p resultType set to @p type, and both
         * @p before and @p after set to @p extrapolation.
         */
        explicit AnimationTrackData(AnimationTrackTarget targetName, UnsignedLong target, AnimationTrackType type, const Containers::StridedArrayView1D<const Float>& keys, const Containers::StridedArrayView1D<const void>& values, Animation::Interpolation interpolation, Animation::Extrapolation extrapolation = Animation::Extrapolation::Constant) noexcept: AnimationTrackData{targetName, target, type, keys, values, interpolation, extrapolation, extrapolation} {}

        /**
         * @brief Type-erased constructor with both generic and custom interpolator
         * @param targetName    Track target name
         * @param target        Track target ID
         * @param type          Value type
         * @param resultType    Result type
         * @param keys          Frame keys
         * @param values        Frame values
         * @param interpolation Interpolation behavior
         * @param interpolator  Interpolator function
         * @param before        Extrapolation behavior before
         * @param after         Extrapolation behavior after
         * @m_since_latest
         *
         * Compared to @ref AnimationTrackData(AnimationTrackTarget, UnsignedLong, AnimationTrackType, AnimationTrackType, const Containers::StridedArrayView1D<const Float>&, const Containers::StridedArrayView1D<const void>&, Animation::Interpolation, Animation::Extrapolation, Animation::Extrapolation)
         * which picks an interpolator function based on the value of
         * @p interpolation, @p type and @p resultType, here it's taken
         * explicitly. Even though it accepts the function cast to a
         * @cpp void(*)() @ce, it's expected to be of a right type for @p type
         * and @p resultType.
         */
        explicit AnimationTrackData(AnimationTrackTarget targetName, UnsignedLong target, AnimationTrackType type, AnimationTrackType resultType, const Containers::StridedArrayView1D<const Float>& keys, const Containers::StridedArrayView1D<const void>& values, Animation::Interpolation interpolation, void(*interpolator)(), Animation::Extrapolation before, Animation::Extrapolation after) noexcept;

        /** @overload
         * @m_since_latest
         *
         * Equivalent to calling @ref AnimationTrackData(AnimationTrackTarget, UnsignedLong, AnimationTrackType, AnimationTrackType, const Containers::StridedArrayView1D<const Float>&, const Containers::StridedArrayView1D<const void>&, Animation::Interpolation, void(*)(), Animation::Extrapolation, Animation::Extrapolation)
         * with both @p type and @p resultType set to @p type.
         */
        explicit AnimationTrackData(AnimationTrackTarget targetName, UnsignedLong target, AnimationTrackType type, const Containers::StridedArrayView1D<const Float>& keys, const Containers::StridedArrayView1D<const void>& values, Animation::Interpolation interpolation, void(*interpolator)(), Animation::Extrapolation before, Animation::Extrapolation after) noexcept: AnimationTrackData{targetName, target, type, type, keys, values, interpolation, interpolator, before, after} {}

        /** @overload
         * @m_since_latest
         *
         * Equivalent to calling @ref AnimationTrackData(AnimationTrackTarget, UnsignedLong, AnimationTrackType, AnimationTrackType, const Containers::StridedArrayView1D<const Float>&, const Containers::StridedArrayView1D<const void>&, Animation::Interpolation, void(*)(), Animation::Extrapolation, Animation::Extrapolation)
         * with both @p before and @p after set to @p extrapolation.
         */
        explicit AnimationTrackData(AnimationTrackTarget targetName, UnsignedLong target, AnimationTrackType type, AnimationTrackType resultType, const Containers::StridedArrayView1D<const Float>& keys, const Containers::StridedArrayView1D<const void>& values, Animation::Interpolation interpolation, void(*interpolator)(), Animation::Extrapolation extrapolation = Animation::Extrapolation::Constant) noexcept: AnimationTrackData{targetName, target, type, resultType, keys, values, interpolation, interpolator, extrapolation, extrapolation} {}

        /** @overload
         * @m_since_latest
         *
         * Equivalent to calling @ref AnimationTrackData(AnimationTrackTarget, UnsignedLong, AnimationTrackType, AnimationTrackType, const Containers::StridedArrayView1D<const Float>&, const Containers::StridedArrayView1D<const void>&, Animation::Interpolation, void(*)(), Animation::Extrapolation, Animation::Extrapolation)
         * with both @p type and @p resultType set to @p type, and both
         * @p before and @p after set to @p extrapolation.
         */
        explicit AnimationTrackData(AnimationTrackTarget targetName, UnsignedLong target, AnimationTrackType type, const Containers::StridedArrayView1D<const Float>& keys, const Containers::StridedArrayView1D<const void>& values, Animation::Interpolation interpolation, void(*interpolator)(), Animation::Extrapolation extrapolation = Animation::Extrapolation::Constant) noexcept: AnimationTrackData{targetName, target, type, keys, values, interpolation, interpolator, extrapolation, extrapolation} {}

        /**
         * @brief Type-erased constructor with custom interpolator
         * @param targetName    Track target name
         * @param target        Track target ID
         * @param type          Value type
         * @param resultType    Result type
         * @param keys          Frame keys
         * @param values        Frame values
         * @param interpolator  Interpolator function
         * @param before        Extrapolation behavior before
         * @param after         Extrapolation behavior after
         * @m_since_latest
         *
         * Calls @ref AnimationTrackData(AnimationTrackTarget, UnsignedLong, AnimationTrackType, AnimationTrackType, const Containers::StridedArrayView1D<const Float>&, const Containers::StridedArrayView1D<const void>&, Animation::Interpolation, void(*)(), Animation::Extrapolation, Animation::Extrapolation)
         * with @p interpolation set to @ref Animation::Interpolation::Custom.
         */
        explicit AnimationTrackData(AnimationTrackTarget targetName, UnsignedLong target, AnimationTrackType type, AnimationTrackType resultType, const Containers::StridedArrayView1D<const Float>& keys, const Containers::StridedArrayView1D<const void>& values, void(*interpolator)(), Animation::Extrapolation before, Animation::Extrapolation after) noexcept: AnimationTrackData{targetName, target, type, resultType, keys, values, Animation::Interpolation::Custom, interpolator, before, after} {}

        /** @overload
         * @m_since_latest
         *
         * Equivalent to calling @ref AnimationTrackData(AnimationTrackTarget, UnsignedLong, AnimationTrackType, AnimationTrackType, const Containers::StridedArrayView1D<const Float>&, const Containers::StridedArrayView1D<const void>&, void(*)(), Extrapolation, Extrapolation)
         * with both @p type and @p resultType set to @p type.
         */
        explicit AnimationTrackData(AnimationTrackTarget targetName, UnsignedLong target, AnimationTrackType type, const Containers::StridedArrayView1D<const Float>& keys, const Containers::StridedArrayView1D<const void>& values, void(*interpolator)(), Animation::Extrapolation before, Animation::Extrapolation after) noexcept: AnimationTrackData{targetName, target, type, type, keys, values, interpolator, before, after} {}

        /** @overload
         * @m_since_latest
         *
         * Equivalent to calling @ref AnimationTrackData(AnimationTrackTarget, UnsignedLong, AnimationTrackType, AnimationTrackType, const Containers::StridedArrayView1D<const Float>&, const Containers::StridedArrayView1D<const void>&, void(*)(), Extrapolation, Extrapolation)
         * with both @p before and @p after set to @p extrapolation.
         */
        explicit AnimationTrackData(AnimationTrackTarget targetName, UnsignedLong target, AnimationTrackType type, AnimationTrackType resultType, const Containers::StridedArrayView1D<const Float>& keys, const Containers::StridedArrayView1D<const void>& values, void(*interpolator)(), Animation::Extrapolation extrapolation = Animation::Extrapolation::Constant) noexcept: AnimationTrackData{targetName, target, type, resultType, keys, values, interpolator, extrapolation, extrapolation} {}

        /** @overload
         * @m_since_latest
         *
         * Equivalent to calling @ref AnimationTrackData(AnimationTrackTarget, UnsignedLong, AnimationTrackType, AnimationTrackType, const Containers::StridedArrayView1D<const Float>&, const Containers::StridedArrayView1D<const void>&, void(*)(), Extrapolation, Extrapolation)
         * with both @p type and @p resultType set to @p type, and both
         * @p before and @p after set to @p extrapolation.
         */
        explicit AnimationTrackData(AnimationTrackTarget targetName, UnsignedLong target, AnimationTrackType type, const Containers::StridedArrayView1D<const Float>& keys, const Containers::StridedArrayView1D<const void>& values, void(*interpolator)(), Animation::Extrapolation extrapolation = Animation::Extrapolation::Constant) noexcept: AnimationTrackData{targetName, target, type, keys, values, interpolator, extrapolation, extrapolation} {}

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Type-erased constructor
         * @param type          Value type
         * @param resultType    Result type
         * @param targetName    Track target name
         * @param target        Track target ID
         * @param view          Type-erased @ref Animation::TrackView instance
         *
         * Expects that @p view key and value strides both fit into signed
         * 16-bit values and that keyframe count fits into 32 bits.
         *
         * @m_deprecated_since_latest Use either the typed
         *      @ref AnimationTrackData(AnimationTrackTarget, UnsignedLong, const Animation::TrackView<const Float, const V, R>&)
         *      constructor or the type-erased
         *      @ref AnimationTrackData(AnimationTrackTarget, UnsignedLong, AnimationTrackType, AnimationTrackType, const Containers::StridedArrayView1D<const Float>&, const Containers::StridedArrayView1D<const void>&, Animation::Interpolation, Animation::Extrapolation, Animation::Extrapolation)
         *      etc. constructors instead.
         */
        explicit CORRADE_DEPRECATED("use either the typed TrackView constructor or the type-erased constructors taking key/value views directly") AnimationTrackData(AnimationTrackType type, AnimationTrackType resultType, AnimationTrackTarget targetName, UnsignedLong target, const Animation::TrackViewStorage<const Float>& view) noexcept: AnimationTrackData{targetName, target, type, resultType, view.keys(), view.values(), view.interpolation(), view.interpolator(), view.before(), view.after()} {}

        /** @overload
         *
         * Equivalent to the above with @p type used as both value type and
         * result type.
         *
         * @m_deprecated_since_latest Use either the typed
         *      @ref AnimationTrackData(AnimationTrackTarget, UnsignedLong, const Animation::TrackView<const Float, const V, R>&)
         *      constructor or the type-erased
         *      @ref AnimationTrackData(AnimationTrackTarget, UnsignedLong, AnimationTrackType, const Containers::StridedArrayView1D<const Float>&, const Containers::StridedArrayView1D<const void>&, Animation::Interpolation, Animation::Extrapolation, Animation::Extrapolation)
         *      etc. constructors instead.
         */
        explicit CORRADE_DEPRECATED("use either the typed TrackView constructor or the type-erased constructors taking key/value views directly") AnimationTrackData(AnimationTrackType type, AnimationTrackTarget targetName, UnsignedLong target, const Animation::TrackViewStorage<const Float>& view) noexcept: AnimationTrackData{targetName, target, type, view.keys(), view.values(), view.interpolation(), view.interpolator(), view.before(), view.after()} {}
        #endif

        /**
         * @brief Construct with generic interpolation behavior
         * @param targetName    Track target name
         * @param target        Track target ID
         * @param keys          Frame keys
         * @param values        Frame values
         * @param interpolation Interpolation behavior
         * @param before        Extrapolation behavior before
         * @param after         Extrapolation behavior after
         * @m_since_latest
         *
         * Detects @ref AnimationTrackType from @p values and delegates to
         * @ref AnimationTrackData(AnimationTrackTarget, UnsignedLong, AnimationTrackType, AnimationTrackType, const Containers::StridedArrayView1D<const Float>&, const Containers::StridedArrayView1D<const void>&, Animation::Interpolation, Animation::Extrapolation, Animation::Extrapolation).
         */
        template<class V, class R = Animation::ResultOf<V>> AnimationTrackData(AnimationTrackTarget targetName, UnsignedLong target, const Containers::StridedArrayView1D<const Float>& keys, const Containers::StridedArrayView1D<V>& values, Animation::Interpolation interpolation, Animation::Extrapolation before, Animation::Extrapolation after) noexcept;

        /** @overload
         * @m_since_latest
         *
         * Equivalent to calling @ref AnimationTrackData(AnimationTrackTarget, UnsignedLong, const Containers::StridedArrayView1D<const Float>&, const Containers::StridedArrayView1D<V>&, Animation::Interpolation, Animation::Extrapolation, Animation::Extrapolation)
         * with both @p before and @p after set to @p extrapolation.
         */
        template<class V, class R = Animation::ResultOf<V>> AnimationTrackData(AnimationTrackTarget targetName, UnsignedLong target, const Containers::StridedArrayView1D<const Float>& keys, const Containers::StridedArrayView1D<V>& values, Animation::Interpolation interpolation, Animation::Extrapolation extrapolation = Animation::Extrapolation::Constant) noexcept: AnimationTrackData{targetName, target, keys, values, interpolation, extrapolation, extrapolation} {}

        /**
         * @brief Construct with both generic and custom interpolator
         * @param targetName    Track target name
         * @param target        Track target ID
         * @param keys          Frame keys
         * @param values        Frame values
         * @param interpolation Interpolation behavior
         * @param interpolator  Interpolator function
         * @param before        Extrapolation behavior before
         * @param after         Extrapolation behavior after
         * @m_since_latest
         *
         * Detects @ref AnimationTrackType from @p values and delegates to
         * @ref AnimationTrackData(AnimationTrackTarget, UnsignedLong, AnimationTrackType, AnimationTrackType, const Containers::StridedArrayView1D<const Float>&, const Containers::StridedArrayView1D<const void>&, Animation::Interpolation, void(*)(), Animation::Extrapolation, Animation::Extrapolation).
         */
        template<class V, class R = Animation::ResultOf<V>> AnimationTrackData(AnimationTrackTarget targetName, UnsignedLong target, const Containers::StridedArrayView1D<const Float>& keys, const Containers::StridedArrayView1D<V>& values, Animation::Interpolation interpolation, R(*interpolator)(const typename std::remove_const<V>::type&, const typename std::remove_const<V>::type&, Float), Animation::Extrapolation before, Animation::Extrapolation after) noexcept;

        /** @overload
         * @m_since_latest
         *
         * Equivalent to calling @ref AnimationTrackData(AnimationTrackTarget, UnsignedLong, const Containers::StridedArrayView1D<const Float>&, const Containers::StridedArrayView1D<const V>&, Animation::Interpolation, R(*)(const V&, const V&, Float), Animation::Extrapolation, Animation::Extrapolation)
         * with both @p before and @p after set to @p extrapolation.
         */
        template<class V, class R = Animation::ResultOf<V>> AnimationTrackData(AnimationTrackTarget targetName, UnsignedLong target, const Containers::StridedArrayView1D<const Float>& keys, const Containers::StridedArrayView1D<V>& values, Animation::Interpolation interpolation, R(*interpolator)(const typename std::remove_const<V>::type&, const typename std::remove_const<V>::type&, Float), Animation::Extrapolation extrapolation = Animation::Extrapolation::Constant) noexcept: AnimationTrackData{targetName, target, keys, values, interpolation, interpolator, extrapolation, extrapolation} {}

        /**
         * @brief Construct with custom interpolator
         * @param targetName    Track target name
         * @param target        Track target ID
         * @param keys          Frame keys
         * @param values        Frame values
         * @param interpolator  Interpolator function
         * @param before        Extrapolation behavior before
         * @param after         Extrapolation behavior after
         * @m_since_latest
         *
         * Calls @ref AnimationTrackData(AnimationTrackTarget, UnsignedLong, AnimationTrackType, AnimationTrackType, const Containers::StridedArrayView1D<const Float>&, const Containers::StridedArrayView1D<const V>&, Interpolation, R(*)(const V&, const V&, Float), Extrapolation, Extrapolation)
         * with @p interpolation set to @ref Animation::Interpolation::Custom.
         */
        template<class V, class R = Animation::ResultOf<V>> AnimationTrackData(AnimationTrackTarget targetName, UnsignedLong target, const Containers::StridedArrayView1D<const Float>& keys, const Containers::StridedArrayView1D<V>& values, R(*interpolator)(const typename std::remove_const<V>::type&, const typename std::remove_const<V>::type&, Float), Animation::Extrapolation before, Animation::Extrapolation after) noexcept: AnimationTrackData{targetName, target, keys, values, Animation::Interpolation::Custom, interpolator, before, after} {}

        /** @overload
         * @m_since_latest
         *
         * Equivalent to calling @ref AnimationTrackData(AnimationTrackTarget, UnsignedLong, const Containers::StridedArrayView1D<const Float>&, const Containers::StridedArrayView1D<const V>&, R(*)(const V&, const V&, Float), Extrapolation, Extrapolation)
         * with both @p before and @p after set to @p extrapolation.
         */
        template<class V, class R = Animation::ResultOf<V>> AnimationTrackData(AnimationTrackTarget targetName, UnsignedLong target, const Containers::StridedArrayView1D<const Float>& keys, const Containers::StridedArrayView1D<V>& values, R(*interpolator)(const typename std::remove_const<V>::type&, const typename std::remove_const<V>::type&, Float), Animation::Extrapolation extrapolation = Animation::Extrapolation::Constant) noexcept: AnimationTrackData{targetName, target, keys, values, interpolator, extrapolation, extrapolation} {}

        /**
         * @brief Construct from a track view
         * @param targetName    Track target name
         * @param target        Track target ID
         * @param view          Track view
         * @m_since{2020,06}
         *
         * Delegates to @ref AnimationTrackData(AnimationTrackTarget, UnsignedLong, AnimationTrackType, AnimationTrackType, const Containers::StridedArrayView1D<const Float>&, const Containers::StridedArrayView1D<const V>&, Animation::Interpolation, R(*)(const V&, const V&, Float), Animation::Extrapolation, Animation::Extrapolation).
         */
        template<class V, class R = Animation::ResultOf<V>> explicit AnimationTrackData(AnimationTrackTarget targetName, UnsignedLong target, const Animation::TrackView<const Float, const V, R>& view) noexcept: AnimationTrackData{targetName, target, view.keys(), view.values(), view.interpolation(), view.interpolator(), view.before(), view.after()} {}

        /**
         * @brief Value type
         * @m_since_latest
         */
        AnimationTrackType type() const { return _type; }

        /**
         * @brief Result type
         * @m_since_latest
         */
        AnimationTrackType resultType() const { return _resultType; }

        /**
         * @brief Track target name
         * @m_since_latest
         */
        AnimationTrackTarget targetName() const { return _targetName; }

        /**
         * @brief Track target ID
         * @m_since_latest
         */
        UnsignedLong target() const { return _target; }

        /**
         * @brief Type-erased @ref Animation::TrackView instance
         * @m_since_latest
         */
        Animation::TrackViewStorage<const Float> track() const;

    private:
        friend AnimationData;

        AnimationTrackType _type, _resultType;
        AnimationTrackTarget _targetName;
        Animation::Interpolation _interpolation;
        Animation::Extrapolation _before, _after;
        /* 1 byte padding */
        UnsignedLong _target;
        UnsignedInt _size;
        Short _keysStride;
        Short _valuesStride;
        const void* _keysData;
        const void* _valuesData;
        void(*_interpolator)();
};

/**
@brief Animation clip data

Provides access to animation data and track properties of given clip. Populated
instances of this class are returned from @ref AbstractImporter::animation()
and can be passed to @ref AbstractSceneConverter::add(const AnimationData&, Containers::StringView).
Like with other @ref Trade types, the internal representation is fixed upon
construction and allows only optional in-place modification of the data itself,
but not of the overall structure.

Typical usage is feeding all the tracks directly to @ref Animation::Player. For
every track, you need to query its concrete type and then feed the resulting
@ref Animation::TrackView of correct type to @ref Animation::Player::add(), for
example.

Note that this class owns the animation track data and the tracks are only
views on it. In order to be able to destroy the @ref AnimationData instance and
keep using the animations later, you need to take ownership of the data using
@ref release().

In the following snippet all animated positions are stored in an array. The
array is then updated during calls to @ref Animation::Player::advance().

@snippet Trade.cpp AnimationData-usage

It's also possible to directly update object transformations using callbacks,
among other things. See documentation of the @ref Animation::Player class for
more information.

@section Trade-AnimationData-usage-mutable Mutable data access

The interfaces implicitly provide @cpp const @ce views on the contained
keyframe data through the @ref data() and @ref track() accessors. This is done
because in general case the data can also refer to a memory-mapped file or
constant memory. In cases when it's desirable to modify the data in-place,
there's the @ref mutableData() and @ref mutableTrack() set of functions. To use
these, you need to check that the data are mutable using @ref dataFlags()
first. The following snippet inverts the Y coordinate of a translation
animation:

@snippet Trade.cpp AnimationData-usage-mutable

@experimental
*/
class MAGNUM_TRADE_EXPORT AnimationData {
    public:
        /**
         * @brief Construct an animation data
         * @param data          Buffer containing all keyframe data for this
         *      animation clip
         * @param tracks        Track data
         * @param importerState Importer-specific state
         *
         * Each item of @p track should have an @ref Animation::TrackView
         * instance pointing its key/value views to @p data. The @ref duration()
         * is automatically calculated from durations of all tracks.
         *
         * The @ref dataFlags() are implicitly set to a combination of
         * @ref DataFlag::Owned and @ref DataFlag::Mutable. For non-owned data
         * use the @ref AnimationData(DataFlags,  Containers::ArrayView<const void>, Containers::Array<AnimationTrackData>&&, const void*)
         * constructor instead.
         */
        explicit AnimationData(Containers::Array<char>&& data, Containers::Array<AnimationTrackData>&& tracks, const void* importerState = nullptr) noexcept;

        /**
         * @overload
         * @m_since{2020,06}
         */
        /* Not noexcept because allocation happens inside */
        explicit AnimationData(Containers::Array<char>&& data, std::initializer_list<AnimationTrackData> tracks, const void* importerState = nullptr);

        /**
         * @brief Construct a non-owned animation data
         * @param dataFlags     Data flags
         * @param data          View on a buffer containing all keyframe data
         *      for this animation clip
         * @param tracks        Track data
         * @param importerState Importer-specific state
         * @m_since{2020,06}
         *
         * Compared to @ref AnimationData(Containers::Array<char>&&, Containers::Array<AnimationTrackData>&&, const void*)
         * creates an instance that doesn't own the passed data. The
         * @p dataFlags parameter can contain @ref DataFlag::Mutable to
         * indicate the external data can be modified, and is expected to *not*
         * have @ref DataFlag::Owned set.
         */
        explicit AnimationData(DataFlags dataFlags, Containers::ArrayView<const void> data, Containers::Array<AnimationTrackData>&& tracks, const void* importerState = nullptr) noexcept;

        /**
         * @overload
         * @m_since{2020,06}
         */
        /* Not noexcept because allocation happens inside */
        explicit AnimationData(DataFlags dataFlags, Containers::ArrayView<const void> data, std::initializer_list<AnimationTrackData> tracks, const void* importerState = nullptr);

        /**
         * @brief Construct an animation data with explicit duration
         * @param data          Buffer containing all keyframe data for this
         *      animation clip
         * @param tracks        Track data
         * @param duration      Animation track duration
         * @param importerState Importer-specific state
         *
         * Each item of @p track should have an @ref Animation::TrackView
         * instance pointing its key/value views to @p data.
         *
         * The @ref dataFlags() are implicitly set to a combination of
         * @ref DataFlag::Owned and @ref DataFlag::Mutable. For non-owned data
         * use the @ref AnimationData(DataFlags,  Containers::ArrayView<const void>, Containers::Array<AnimationTrackData>&&, const Range1D&, const void*)
         * constructor instead.
         */
        explicit AnimationData(Containers::Array<char>&& data, Containers::Array<AnimationTrackData>&& tracks, const Range1D& duration, const void* importerState = nullptr) noexcept;

        /**
         * @overload
         * @m_since{2020,06}
         */
        /* Not noexcept because allocation happens inside */
        explicit AnimationData(Containers::Array<char>&& data, std::initializer_list<AnimationTrackData> tracks, const Range1D& duration, const void* importerState = nullptr);

        /**
         * @brief Construct a non-owned animation data with explicit duration
         * @param dataFlags     Data flags
         * @param data          View on a buffer containing all keyframe data
         *      for this animation clip
         * @param tracks        Track data
         * @param duration      Animation track duration
         * @param importerState Importer-specific state
         * @m_since{2020,06}
         *
         * Compared to @ref AnimationData(Containers::Array<char>&&, Containers::Array<AnimationTrackData>&&, const Range1D&, const void*)
         * creates an instance that doesn't own the passed data. The
         * @p dataFlags parameter can contain @ref DataFlag::Mutable to
         * indicate the external data can be modified, and is expected to *not*
         * have @ref DataFlag::Owned set.
         */
        explicit AnimationData(DataFlags dataFlags, Containers::ArrayView<const void> data, Containers::Array<AnimationTrackData>&& tracks, const Range1D& duration, const void* importerState = nullptr) noexcept;

        /**
         * @overload
         * @m_since{2020,06}
         */
        /* Not noexcept because allocation happens inside */
        explicit AnimationData(DataFlags dataFlags, Containers::ArrayView<const void> data, std::initializer_list<AnimationTrackData> tracks, const Range1D& duration, const void* importerState = nullptr);

        ~AnimationData();

        /** @brief Copying is not allowed */
        AnimationData(const AnimationData&) = delete;

        /** @brief Move constructor */
        AnimationData(AnimationData&&) noexcept;

        /** @brief Copying is not allowed */
        AnimationData& operator=(const AnimationData&) = delete;

        /** @brief Move assignment */
        AnimationData& operator=(AnimationData&&) noexcept;

        /**
         * @brief Data flags
         * @m_since{2020,06}
         *
         * @see @ref release(), @ref mutableData(), @ref mutableTrack()
         */
        DataFlags dataFlags() const { return _dataFlags; }

        /**
         * @brief Raw data
         *
         * Contains data for all tracks contained in this clip.
         * @see @ref release(), @ref mutableData()
         */
        Containers::ArrayView<const char> data() const & { return _data; }

        /** @brief Taking a view to a r-value instance is not allowed */
        Containers::ArrayView<const char> data() const && = delete;

        /**
         * @brief Mutable raw data
         * @m_since{2020,06}
         *
         * Like @ref data(), but returns a non-const view. Expects that the
         * animation is mutable.
         * @see @ref dataFlags()
         */
        Containers::ArrayView<char> mutableData() &;

        /**
         * @brief Taking a view to a r-value instance is not allowed
         * @m_since{2020,06}
         */
        Containers::ArrayView<char> mutableData() && = delete;

        /** @brief Duration */
        Range1D duration() const { return _duration; }

        /** @brief Track count */
        UnsignedInt trackCount() const { return _tracks.size(); }

        /**
         * @brief Track value type
         * @param id        Track index
         *
         * Data types are usually closely related to @ref trackTargetName(),
         * see @ref AnimationTrackTarget documentation for more information.
         * @see @ref trackCount()
         */
        AnimationTrackType trackType(UnsignedInt id) const;

        /**
         * @brief Track result type
         * @param id        Track index
         *
         * In case track values are packed, track result type is different from
         * @ref trackType(). Data types are usually closely related to
         * @ref trackTargetName(), see @ref AnimationTrackTarget documentation
         * for more information.
         * @see @ref trackCount()
         */
        AnimationTrackType trackResultType(UnsignedInt id) const;

        /**
         * @brief Track target name
         * @param id        Track index
         *
         * Particular animation targets usually correspond to a common
         * @ref trackType(), see @ref AnimationTrackTarget documentation for
         * more information.
         * @see @ref trackCount()
         */
        AnimationTrackTarget trackTargetName(UnsignedInt id) const;

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief @copybrief trackTargetName()
         * @m_deprecated_since_latest Use @ref trackTargetName() instead.
         */
        CORRADE_DEPRECATED("use trackTargetName() instead") AnimationTrackTarget trackTargetType(UnsignedInt id) const {
            return trackTargetName(id);
        }
        #endif

        /**
         * @brief Track target ID
         * @param id        Track index
         *
         * For @ref trackTargetName() with
         * @ref AnimationTrackTarget::Translation2D,
         * @ref AnimationTrackTarget::Translation3D,
         * @ref AnimationTrackTarget::Rotation2D,
         * @ref AnimationTrackTarget::Rotation3D,
         * @ref AnimationTrackTarget::Scaling2D,
         * @ref AnimationTrackTarget::Scaling3D specifies object which
         * property is modified.
         * @see @ref trackCount(), @ref AbstractImporter::scene()
         */
        UnsignedLong trackTarget(UnsignedInt id) const;

        /**
         * @brief Track data storage
         *
         * Returns the untyped base of a @ref Animation::TrackView, which
         * allows access only to some track properties. Use the templated and
         * checked version below to access a concrete @ref Animation::TrackView
         * type.
         */
        Animation::TrackViewStorage<const Float> track(UnsignedInt id) const;

        /**
         * @brief Mutable track data storage
         * @m_since{2020,06}
         *
         * Like @ref track(), but returns a mutable view. Expects that the
         * animation is mutable.
         * @see @ref dataFlags()
         */
        Animation::TrackViewStorage<Float> mutableTrack(UnsignedInt id);

        /**
         * @brief Track data
         * @tparam V    Track value type
         * @tparam R    Track result type
         *
         * Expects that requested types are correct for given @ref trackType()
         * and @ref trackResultType(). Note that the returned view is onto
         * @ref data(), meaning you either need to ensure that the
         * @cpp AnimationData @ce instance stays in scope for as long as you
         * use the view or you need to release the data array using
         * @ref release() and manage its lifetime yourself.
         */
        template<class V, class R = Animation::ResultOf<V>> Animation::TrackView<const Float, const V, R> track(UnsignedInt id) const;

        /**
         * @brief Mutable track data
         * @m_since{2020,06}
         *
         * Like @ref track(), but returns a mutable view. Expects that the
         * animation is mutable.
         * @see @ref dataFlags()
         */
        template<class V, class R = Animation::ResultOf<V>> Animation::TrackView<Float, V, R> mutableTrack(UnsignedInt id);

        /**
         * @brief Release data storage
         *
         * Releases the ownership of the data array and resets internal state
         * to default. The animation then behaves like it's empty. Note that
         * the returned array has a custom no-op deleter when the data are not
         * owned by the animation, and while the returned array type is
         * mutable, the actual memory might be not.
         * @see @ref data(), @ref dataFlags()
         */
        Containers::Array<char> release();

        /**
         * @brief Importer-specific state
         *
         * See @ref AbstractImporter::importerState() for more information.
         */
        const void* importerState() const { return _importerState; }

    private:
        /* For custom deleter checks. Not done in the constructors here because
           the restriction is pointless when used outside of plugin
           implementations. */
        friend AbstractImporter;

        DataFlags _dataFlags;
        Range1D _duration;
        Containers::Array<char> _data;
        Containers::Array<AnimationTrackData> _tracks;
        const void* _importerState;
};

/** @relatesalso AnimationData
@brief Animation interpolator function for given interpolation behavior

To be used from importer plugins --- wrapper around @ref Animation::interpolatorFor(),
guaranteeing that the returned function pointer is not instantiated inside the
plugin binary to avoid dangling function pointers on plugin unload.
@see @ref AnimationData
@experimental
*/
template<class V, class R = Animation::ResultOf<V>> MAGNUM_TRADE_EXPORT auto animationInterpolatorFor(Animation::Interpolation interpolation) -> R(*)(const V&, const V&, Float);

namespace Implementation {
    /* LCOV_EXCL_START */
    template<class> constexpr AnimationTrackType animationTypeFor();
    template<> constexpr AnimationTrackType animationTypeFor<bool>() { return AnimationTrackType::Bool; }
    template<> constexpr AnimationTrackType animationTypeFor<Float>() { return AnimationTrackType::Float; }
    template<> constexpr AnimationTrackType animationTypeFor<UnsignedInt>() { return AnimationTrackType::UnsignedInt; }
    template<> constexpr AnimationTrackType animationTypeFor<Int>() { return AnimationTrackType::Int; }

    template<> constexpr AnimationTrackType animationTypeFor<Math::BitVector<2>>() { return AnimationTrackType::BitVector2; }
    template<> constexpr AnimationTrackType animationTypeFor<Math::BitVector<3>>() { return AnimationTrackType::BitVector3; }
    template<> constexpr AnimationTrackType animationTypeFor<Math::BitVector<4>>() { return AnimationTrackType::BitVector4; }

    template<> constexpr AnimationTrackType animationTypeFor<Vector2>() { return AnimationTrackType::Vector2; }
    template<> constexpr AnimationTrackType animationTypeFor<Vector3>() { return AnimationTrackType::Vector3; }
    template<> constexpr AnimationTrackType animationTypeFor<Vector4>() { return AnimationTrackType::Vector4; }
    template<> constexpr AnimationTrackType animationTypeFor<Math::Vector<2, Float>>() { return AnimationTrackType::Vector2; }
    template<> constexpr AnimationTrackType animationTypeFor<Math::Vector<3, Float>>() { return AnimationTrackType::Vector3; }
    template<> constexpr AnimationTrackType animationTypeFor<Math::Vector<4, Float>>() { return AnimationTrackType::Vector4; }

    template<> constexpr AnimationTrackType animationTypeFor<Color3>() { return AnimationTrackType::Vector3; }
    template<> constexpr AnimationTrackType animationTypeFor<Color4>() { return AnimationTrackType::Vector4; }

    template<> constexpr AnimationTrackType animationTypeFor<Vector2ui>() { return AnimationTrackType::Vector2ui; }
    template<> constexpr AnimationTrackType animationTypeFor<Vector3ui>() { return AnimationTrackType::Vector3ui; }
    template<> constexpr AnimationTrackType animationTypeFor<Vector4ui>() { return AnimationTrackType::Vector4ui; }
    template<> constexpr AnimationTrackType animationTypeFor<Math::Vector<2, UnsignedInt>>() { return AnimationTrackType::Vector2ui; }
    template<> constexpr AnimationTrackType animationTypeFor<Math::Vector<3, UnsignedInt>>() { return AnimationTrackType::Vector3ui; }
    template<> constexpr AnimationTrackType animationTypeFor<Math::Vector<4, UnsignedInt>>() { return AnimationTrackType::Vector4ui; }

    template<> constexpr AnimationTrackType animationTypeFor<Vector2i>() { return AnimationTrackType::Vector2i; }
    template<> constexpr AnimationTrackType animationTypeFor<Vector3i>() { return AnimationTrackType::Vector3i; }
    template<> constexpr AnimationTrackType animationTypeFor<Vector4i>() { return AnimationTrackType::Vector4i; }
    template<> constexpr AnimationTrackType animationTypeFor<Math::Vector<2, Int>>() { return AnimationTrackType::Vector2i; }
    template<> constexpr AnimationTrackType animationTypeFor<Math::Vector<3, Int>>() { return AnimationTrackType::Vector3i; }
    template<> constexpr AnimationTrackType animationTypeFor<Math::Vector<4, Int>>() { return AnimationTrackType::Vector4i; }

    template<> constexpr AnimationTrackType animationTypeFor<Complex>() { return AnimationTrackType::Complex; }
    template<> constexpr AnimationTrackType animationTypeFor<Quaternion>() { return AnimationTrackType::Quaternion; }
    template<> constexpr AnimationTrackType animationTypeFor<DualQuaternion>() { return AnimationTrackType::DualQuaternion; }

    template<> constexpr AnimationTrackType animationTypeFor<CubicHermite1D>() { return AnimationTrackType::CubicHermite1D; }
    template<> constexpr AnimationTrackType animationTypeFor<CubicHermite2D>() { return AnimationTrackType::CubicHermite2D; }
    template<> constexpr AnimationTrackType animationTypeFor<CubicHermite3D>() { return AnimationTrackType::CubicHermite3D; }
    template<> constexpr AnimationTrackType animationTypeFor<CubicHermiteComplex>() { return AnimationTrackType::CubicHermiteComplex; }
    template<> constexpr AnimationTrackType animationTypeFor<CubicHermiteQuaternion>() { return AnimationTrackType::CubicHermiteQuaternion; }
    /* LCOV_EXCL_STOP */
}

template<class V, class R> inline AnimationTrackData::AnimationTrackData(const AnimationTrackTarget targetName, const UnsignedLong target, const Containers::StridedArrayView1D<const Float>& keys, const Containers::StridedArrayView1D<V>& values, const Animation::Interpolation interpolation, const Animation::Extrapolation before, const Animation::Extrapolation after) noexcept: AnimationTrackData{targetName, target, Implementation::animationTypeFor<typename std::remove_const<V>::type>(), Implementation::animationTypeFor<R>(), keys, values, interpolation, before, after} {}

template<class V, class R> inline AnimationTrackData::AnimationTrackData(const AnimationTrackTarget targetName, const UnsignedLong target, const Containers::StridedArrayView1D<const Float>& keys, const Containers::StridedArrayView1D<V>& values, const Animation::Interpolation interpolation, R(*interpolator)(const typename std::remove_const<V>::type&, const typename std::remove_const<V>::type&, Float), const Animation::Extrapolation before, const Animation::Extrapolation after) noexcept: AnimationTrackData{targetName, target, Implementation::animationTypeFor<typename std::remove_const<V>::type>(), Implementation::animationTypeFor<R>(), keys, values, interpolation, reinterpret_cast<void(*)()>(interpolator), before, after} {}

template<class V, class R> Animation::TrackView<const Float, const V, R> AnimationData::track(UnsignedInt id) const {
    const Animation::TrackViewStorage<const Float> storage = track(id);
    CORRADE_ASSERT(Implementation::animationTypeFor<V>() == _tracks[id]._type, "Trade::AnimationData::track(): improper type requested for" << _tracks[id]._type, (static_cast<const Animation::TrackView<const Float, const V, R>&>(storage)));
    CORRADE_ASSERT(Implementation::animationTypeFor<R>() == _tracks[id]._resultType, "Trade::AnimationData::track(): improper result type requested for" << _tracks[id]._resultType, (static_cast<const Animation::TrackView<const Float, const V, R>&>(storage)));
    return static_cast<const Animation::TrackView<const Float, const V, R>&>(storage);
}

template<class V, class R> Animation::TrackView<Float, V, R> AnimationData::mutableTrack(UnsignedInt id) {
    const Animation::TrackViewStorage<Float> storage = mutableTrack(id);
    CORRADE_ASSERT(Implementation::animationTypeFor<V>() == _tracks[id]._type, "Trade::AnimationData::mutableTrack(): improper type requested for" << _tracks[id]._type, (static_cast<const Animation::TrackView<Float, V, R>&>(storage)));
    CORRADE_ASSERT(Implementation::animationTypeFor<R>() == _tracks[id]._resultType, "Trade::AnimationData::mutableTrack(): improper result type requested for" << _tracks[id]._resultType, (static_cast<const Animation::TrackView<Float, V, R>&>(storage)));
    return static_cast<const Animation::TrackView<Float, V, R>&>(storage);
}

}}

#endif
