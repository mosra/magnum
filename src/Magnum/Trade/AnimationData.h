#ifndef Magnum_Trade_AnimationData_h
#define Magnum_Trade_AnimationData_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
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
#include "Magnum/Trade/Trade.h"
#include "Magnum/Trade/visibility.h"

namespace Magnum { namespace Trade {

/**
@brief Type of animation track data

@see @ref AnimationData
@experimental
*/
enum class AnimationTrackType: UnsignedByte {
    Bool,               /**< @cpp bool @ce <b></b> */
    Float,              /**< @ref Magnum::Float "Float" */
    UnsignedInt,        /**< @ref Magnum::UnsignedInt "UnsignedInt" */
    Int,                /**< @ref Magnum::Int "Int" */
    BoolVector2,        /**< @ref Math::BoolVector "Math::BoolVector<2>" */
    BoolVector3,        /**< @ref Math::BoolVector "Math::BoolVector<3>" */
    BoolVector4,        /**< @ref Math::BoolVector "Math::BoolVector<4>" */

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
     * @ref Magnum::Quaternion "Quaternion". Usually used for
     * @ref AnimationTrackTarget::Rotation3D.
     */
    Quaternion,

    DualQuaternion      /**< @ref Magnum::DualQuaternion "DualQuaternion" */
};

/** @debugoperatorenum{AnimationTrackType} */
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, AnimationTrackType value);

/**
@brief Target of an animation track

@see @ref AnimationData
@experimental
*/
enum class AnimationTrackTarget: UnsignedByte {
    /**
     * Modifies 2D object translation. Type is usually
     * @ref Magnum::Vector2 "Vector2".
     *
     * @see @ref AnimationTrackType::Vector2, @ref ObjectData2D::translation()
     */
    Translation2D,

    /**
     * Modifies 3D object translation. Type is usually
     * @ref Magnum::Vector3 "Vector3".
     *
     * @see @ref AnimationTrackType::Vector3, @ref ObjectData3D::translation()
     */
    Translation3D,

    /**
     * Modifies 2D object rotation. Type is usually
     * @ref Magnum::Complex "Complex".
     *
     * @see @ref ObjectData2D::rotation()
     */
    Rotation2D,

    /**
     * Modifies 3D object rotation. Type is usually
     * @ref Magnum::Quaternion "Quaternion".
     *
     * @see @ref AnimationTrackType::Quaternion, @ref ObjectData3D::rotation()
     */
    Rotation3D,

    /**
     * Modifies 2D object scaling. Type is usually
     * @ref Magnum::Vector2 "Vector2".
     *
     * @see @ref AnimationTrackType::Vector2, @ref ObjectData2D::scaling()
     */
    Scaling2D,

    /**
     * Modifies 3D object scaling. Type is usually
     * @ref Magnum::Vector3 "Vector3".
     *
     * @see @ref AnimationTrackType::Vector3, @ref ObjectData3D::scaling()
     */
    Scaling3D,

    /**
     * This and all higher values are for importer-specific targets. Can be of
     * any type, @ref AnimationData::trackTargetId() might or might not point
     * to an existing object. See documentation of particular importer for
     * details.
     */
    Custom = 128
};

/** @debugoperatorenum{AnimationTrackTarget} */
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, AnimationTrackTarget value);

/**
@brief Animation track data

Used when constructing @ref AnimationData. Fields can be accessed through its
API.
@experimental
*/
class AnimationTrackData {
    public:
        /**
         * @brief Default constructor
         *
         * Leaves contents at unspecified values. Provided as a convenience for
         * initialization of @ref AnimationData structure, expected to be
         * replaced with concrete values later.
         */
        /*implicit*/ AnimationTrackData() noexcept: _type{}, _resultType{}, _target{}, _targetId{}, _view{} {}

        /**
         * @brief Constructor
         * @param type          Value type
         * @param resultType    Result type
         * @param target        Track target
         * @param targetId      Track target ID
         * @param view          Type-erased @ref Animation::TrackView instance
         */
        /*implicit*/ AnimationTrackData(AnimationTrackType type, AnimationTrackType resultType, AnimationTrackTarget target, UnsignedInt targetId, Animation::TrackViewStorage<Float> view) noexcept: _type{type}, _resultType{resultType}, _target{target}, _targetId{targetId}, _view{view} {}

        /** @overload
         *
         * Equivalent to the above with @p type used as both value type and
         * result type.
         */
        /*implicit*/ AnimationTrackData(AnimationTrackType type, AnimationTrackTarget target, UnsignedInt targetId, Animation::TrackViewStorage<Float> view) noexcept: _type{type}, _resultType{type}, _target{target}, _targetId{targetId}, _view{view} {}

    private:
        friend AnimationData;

        AnimationTrackType _type, _resultType;
        AnimationTrackTarget _target;
        UnsignedInt _targetId;
        Animation::TrackViewStorage<Float> _view;
};

/**
@brief Animation clip data

@see @ref AbstractImporter::animation()
@experimental
*/
class MAGNUM_TRADE_EXPORT AnimationData {
    public:
        /**
         * @brief Construct with implicit duration
         * @param data          Buffer containing all keyframe data for this
         *      animation clip
         * @param tracks        Track data
         * @param importerState Importer-specific state
         *
         * Each item of @p track should have an @ref Animation::TrackView
         * instance pointing its key/value views to @p data. The @ref duration()
         * is automatically calculated from durations of all tracks.
         */
        explicit AnimationData(Containers::Array<char>&& data, Containers::Array<AnimationTrackData>&& tracks, const void* importerState = nullptr) noexcept;

        /**
         * @brief Construct with explicit duration
         * @param data          Buffer containing all keyframe data for this
         *      animation clip
         * @param tracks        Track data
         * @param duration      Animation track duration
         * @param importerState Importer-specific state
         *
         * Each item of @p track should have an @ref Animation::TrackView
         * instance pointing its key/value views to @p data.
         */
        explicit AnimationData(Containers::Array<char>&& data, Containers::Array<AnimationTrackData>&& tracks, const Range1D& duration, const void* importerState = nullptr) noexcept;

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
         * @brief Raw data
         *
         * Contains data for all tracks contained in this clip.
         * @see @ref release()
         */
        Containers::ArrayView<char> data()
            #ifndef CORRADE_GCC47_COMPATIBILITY
            &
            #endif
            { return _data; }
        #ifndef CORRADE_GCC47_COMPATIBILITY
        Containers::ArrayView<char> data() && = delete; /**< @overload */
        #endif

        /** @overload */
        Containers::ArrayView<const char> data() const
            #ifndef CORRADE_GCC47_COMPATIBILITY
            &
            #endif
            { return _data; }
        #ifndef CORRADE_GCC47_COMPATIBILITY
        Containers::ArrayView<const char> data() const && = delete; /**< @overload */
        #endif

        /** @brief Duration */
        Range1D duration() const { return _duration; }

        /** @brief Track count */
        UnsignedInt trackCount() const { return _tracks.size(); }

        /**
         * @brief Track value type
         * @param id        Track index
         *
         * Data types are usually closely related to @ref trackTarget(), see
         * @ref AnimationTrackTarget documentation for more information.
         * @see @ref trackCount()
         */
        AnimationTrackType trackType(UnsignedInt id) const;

        /**
         * @brief Track result type
         * @param id        Track index
         *
         * In case track values are packed, track result type is different from
         * @ref trackType(). Data types are usually closely related to
         * @ref trackTarget(), see @ref AnimationTrackTarget documentation for
         * more information.
         * @see @ref trackCount()
         */
        AnimationTrackType trackResultType(UnsignedInt id) const;

        /**
         * @brief Track target
         * @param id        Track index
         *
         * Particular animation targets usually correspond to a common
         * @ref trackType(), see @ref AnimationTrackTarget documentation for
         * more information.
         * @see @ref trackCount()
         */
        AnimationTrackTarget trackTarget(UnsignedInt id) const;

        /**
         * @brief Track target index
         * @param id        Track index
         *
         * For @ref trackTarget() with @ref AnimationTrackTarget::Translation2D,
         * @ref AnimationTrackTarget::Translation3D,
         * @ref AnimationTrackTarget::Rotation2D,
         * @ref AnimationTrackTarget::Rotation3D,
         * @ref AnimationTrackTarget::Scaling2D,
         * @ref AnimationTrackTarget::Scaling3D specifies object which property
         * is modified.
         * @see @ref trackCount(), @ref AbstractImporter::object2D(),
         *      @ref AbstractImporter::object3D()
         */
        UnsignedInt trackTargetId(UnsignedInt id) const;

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
        template<class V, class R = Animation::ResultOf<V>> Animation::TrackView<Float, V, R> track(UnsignedInt id) const;

        /**
         * @brief Release data storage
         *
         * Releases the ownership of the data array and resets internal state
         * to default.
         * @see @ref data()
         */
        Containers::Array<char> release() { return std::move(_data); }

        /**
         * @brief Importer-specific state
         *
         * See @ref AbstractImporter::importerState() for more information.
         */
        const void* importerState() const { return _importerState; }

    private:
        Range1D _duration;
        Containers::Array<char> _data;
        Containers::Array<AnimationTrackData> _tracks;
        const void* _importerState;
};

/** @relatesalso AnimationData
@brief Animation interpolator function for given interpolation behavior

To be used from importer plugins --- unlike @ref Animation::interpolatorFor()
guarantees that the returned function pointer is not instantiated inside plugin
binary to avoid dangling function pointers on plugin unload. See
@ref Animation::interpolatorFor() for more information.
@see @ref AnimationData
@experimental
*/
template<class V, class R = Animation::ResultOf<V>> MAGNUM_TRADE_EXPORT auto animationInterpolatorFor(Animation::Interpolation interpolation) -> R(*)(const V&, const V&, Float);

#if !defined(CORRADE_NO_ASSERT) || defined(CORRADE_GRACEFUL_ASSERT)
namespace Implementation {
    /* LCOV_EXCL_START */
    template<class T> AnimationTrackType animationTypeFor();
    template<> constexpr AnimationTrackType animationTypeFor<bool>() { return AnimationTrackType::Bool; }
    template<> constexpr AnimationTrackType animationTypeFor<Float>() { return AnimationTrackType::Float; }
    template<> constexpr AnimationTrackType animationTypeFor<UnsignedInt>() { return AnimationTrackType::UnsignedInt; }
    template<> constexpr AnimationTrackType animationTypeFor<Int>() { return AnimationTrackType::Int; }
    template<> constexpr AnimationTrackType animationTypeFor<Math::BoolVector<2>>() { return AnimationTrackType::BoolVector2; }
    template<> constexpr AnimationTrackType animationTypeFor<Math::BoolVector<3>>() { return AnimationTrackType::BoolVector3; }
    template<> constexpr AnimationTrackType animationTypeFor<Math::BoolVector<4>>() { return AnimationTrackType::BoolVector4; }
    template<> constexpr AnimationTrackType animationTypeFor<Vector2>() { return AnimationTrackType::Vector2; }
    template<> constexpr AnimationTrackType animationTypeFor<Vector2ui>() { return AnimationTrackType::Vector2ui; }
    template<> constexpr AnimationTrackType animationTypeFor<Vector2i>() { return AnimationTrackType::Vector2i; }
    template<> constexpr AnimationTrackType animationTypeFor<Vector3>() { return AnimationTrackType::Vector3; }
    template<> constexpr AnimationTrackType animationTypeFor<Vector3ui>() { return AnimationTrackType::Vector3ui; }
    template<> constexpr AnimationTrackType animationTypeFor<Vector3i>() { return AnimationTrackType::Vector3i; }
    template<> constexpr AnimationTrackType animationTypeFor<Vector4>() { return AnimationTrackType::Vector4; }
    template<> constexpr AnimationTrackType animationTypeFor<Vector4ui>() { return AnimationTrackType::Vector4ui; }
    template<> constexpr AnimationTrackType animationTypeFor<Vector4i>() { return AnimationTrackType::Vector4i; }
    template<> constexpr AnimationTrackType animationTypeFor<Quaternion>() { return AnimationTrackType::Quaternion; }
    template<> constexpr AnimationTrackType animationTypeFor<DualQuaternion>() { return AnimationTrackType::DualQuaternion; }
    /* LCOV_EXCL_STOP */
}
#endif

template<class V, class R> Animation::TrackView<Float, V, R> AnimationData::track(UnsignedInt id) const {
    CORRADE_ASSERT(id < _tracks.size(), "Trade::AnimationData::track(): index out of range", (Animation::TrackView<Float, V, R>{}));
    CORRADE_ASSERT(Implementation::animationTypeFor<V>() == _tracks[id]._type, "Trade::AnimationData::track(): improper type requested for" << _tracks[id]._type, (Animation::TrackView<Float, V, R>{}));
    CORRADE_ASSERT(Implementation::animationTypeFor<R>() == _tracks[id]._resultType, "Trade::AnimationData::track(): improper result type requested for" << _tracks[id]._resultType, (Animation::TrackView<Float, V, R>{}));
    return static_cast<const Animation::TrackView<Float, V, R>&>(_tracks[id]._view);
}

}}

#endif
