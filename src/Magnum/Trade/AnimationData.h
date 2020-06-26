#ifndef Magnum_Trade_AnimationData_h
#define Magnum_Trade_AnimationData_h
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
 * @brief Class @ref Magnum::Trade::AnimationTrackData, @ref Magnum::Trade::AnimationData, enum @ref Magnum::Trade::AnimationTrackType, @ref Magnum::Trade::AnimationTrackTargetType, function @ref Magnum::Trade::animationInterpolatorFor()
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
     * @ref AnimationTrackTargetType::Translation2D and
     * @ref AnimationTrackTargetType::Scaling2D.
     */
    Vector2,

    Vector2ui,          /**< @ref Magnum::Vector2ui "Vector2ui" */
    Vector2i,           /**< @ref Magnum::Vector2i "Vector2i" */

    /**
     * @ref Magnum::Vector3 "Vector3". Usually used for
     * @ref AnimationTrackTargetType::Translation3D and
     * @ref AnimationTrackTargetType::Scaling3D.
     */
    Vector3,

    Vector3ui,          /**< @ref Magnum::Vector3ui "Vector3ui" */
    Vector3i,           /**< @ref Magnum::Vector3i "Vector3i" */
    Vector4,            /**< @ref Magnum::Vector4 "Vector4" */
    Vector4ui,          /**< @ref Magnum::Vector4ui "Vector4ui" */
    Vector4i,           /**< @ref Magnum::Vector4i "Vector4i" */

    /**
     * @ref Magnum::Complex "Complex". Usually used for
     * @ref AnimationTrackTargetType::Rotation2D.
     */
    Complex,

    /**
     * @ref Magnum::Quaternion "Quaternion". Usually used for
     * @ref AnimationTrackTargetType::Rotation3D.
     */
    Quaternion,

    DualQuaternion,     /**< @ref Magnum::DualQuaternion "DualQuaternion" */
    CubicHermite1D,     /**< @ref Magnum::CubicHermite1D "CubicHermite1D" */

    /**
     * @ref Magnum::CubicHermite2D "CubicHermite2D". Usually used for
     * spline-interpolated @ref AnimationTrackTargetType::Translation2D and
     * @ref AnimationTrackTargetType::Scaling2D.
     */
    CubicHermite2D,

    /**
     * @ref Magnum::CubicHermite3D "CubicHermite3D". Usually used for
     * spline-interpolated @ref AnimationTrackTargetType::Translation3D and
     * @ref AnimationTrackTargetType::Scaling3D.
     */
    CubicHermite3D,

    /**
     * @ref Magnum::CubicHermiteComplex "CubicHermiteComplex". Usually used for
     * spline-interpolated @ref AnimationTrackTargetType::Rotation2D.
     */
    CubicHermiteComplex,

    /**
     * @ref Magnum::CubicHermiteQuaternion "CubicHermiteQuaternion". Usually
     * used for spline-interpolated @ref AnimationTrackTargetType::Rotation3D.
     */
    CubicHermiteQuaternion
};

/** @debugoperatorenum{AnimationTrackType} */
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, AnimationTrackType value);

/**
@brief Target of an animation track

@see @ref AnimationData
@experimental
*/
enum class AnimationTrackTargetType: UnsignedByte {
    /**
     * Modifies 2D object translation. Type is usually
     * @ref Magnum::Vector2 "Vector2" or
     * @ref Magnum::CubicHermite2D "CubicHermite2D" for spline-interpolated
     * data.
     *
     * @see @ref AnimationTrackType::Vector2,
     *      @ref AnimationTrackType::CubicHermite2D,
     *      @ref ObjectData2D::translation()
     */
    Translation2D,

    /**
     * Modifies 3D object translation. Type is usually
     * @ref Magnum::Vector3 "Vector3" or
     * @ref Magnum::CubicHermite3D "CubicHermite3D" for spline-interpolated
     * data.
     *
     * @see @ref AnimationTrackType::Vector3,
     *      @ref AnimationTrackType::CubicHermite3D,
     *      @ref ObjectData3D::translation()
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
     *      @ref ObjectData2D::rotation()
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
     *      @ref ObjectData3D::rotation()
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
     *      @ref ObjectData2D::scaling()
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
     *      @ref ObjectData3D::scaling()
     */
    Scaling3D,

    /**
     * This and all higher values are for importer-specific targets. Can be of
     * any type, @ref AnimationData::trackTarget() might or might not point to
     * an existing object. See documentation of a particular importer for
     * details.
     */
    Custom = 128
};

/** @debugoperatorenum{AnimationTrackTargetType} */
MAGNUM_TRADE_EXPORT Debug& operator<<(Debug& debug, AnimationTrackTargetType value);

/**
@brief Animation track data

Convenience type for populating @ref AnimationData. Has no accessors, as the
data are then accessible through @ref AnimationData APIs.
@experimental
*/
class AnimationTrackData {
    public:
        /**
         * @brief Default constructor
         *
         * Leaves contents at unspecified values. Provided as a convenience for
         * initialization of the track array for @ref AnimationData, expected
         * to be replaced with concrete values later.
         */
        explicit AnimationTrackData() noexcept: _type{}, _resultType{}, _targetType{}, _target{}, _view{} {}

        /**
         * @brief Type-erased constructor
         * @param type          Value type
         * @param resultType    Result type
         * @param targetType    Track target type
         * @param target        Track target
         * @param view          Type-erased @ref Animation::TrackView instance
         */
        explicit AnimationTrackData(AnimationTrackType type, AnimationTrackType resultType, AnimationTrackTargetType targetType, UnsignedInt target, Animation::TrackViewStorage<const Float> view) noexcept: _type{type}, _resultType{resultType}, _targetType{targetType}, _target{target}, _view{view} {}

        /** @overload
         *
         * Equivalent to the above with @p type used as both value type and
         * result type.
         */
        explicit AnimationTrackData(AnimationTrackType type, AnimationTrackTargetType targetType, UnsignedInt target, Animation::TrackViewStorage<const Float> view) noexcept: _type{type}, _resultType{type}, _targetType{targetType}, _target{target}, _view{view} {}

        /**
         * @brief Constructor
         * @param targetType    Track target type
         * @param target        Track target
         * @param view          @ref Animation::TrackView instance
         * @m_since{2020,06}
         *
         * Detects @ref AnimationTrackType from @p view type and delegates to
         * @ref AnimationTrackData(AnimationTrackType, AnimationTrackType, AnimationTrackTargetType, UnsignedInt, Animation::TrackViewStorage<const Float>).
         */
        template<class V, class R> explicit AnimationTrackData(AnimationTrackTargetType targetType, UnsignedInt target, Animation::TrackView<const Float, const V, R> view) noexcept;

    private:
        friend AnimationData;

        AnimationTrackType _type, _resultType;
        AnimationTrackTargetType _targetType;
        UnsignedInt _target;
        Animation::TrackViewStorage<const Float> _view;
};

/**
@brief Animation clip data

Provides access to animation data and track properties of given clip. The
instance is commonly returned from @ref AbstractImporter::animation() and a
typical usage is feeding all the tracks directly to @ref Animation::Player.
For every track, you need to query its concrete type and then feed the
resulting @ref Animation::TrackView of correct type to
@ref Animation::Player::add(), for example.

Note that this class owns the animation track data and the tracks are only
views on it. In order to be able to destroy the @ref AnimationData instance and
keep using the animations later, you need to take ownership of the data using
@ref release().

In the following snippet all animated positions are stored in an array. The
array is then updated during calls to @ref Animation::Player::advance().

@snippet MagnumTrade.cpp AnimationData-usage

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

@snippet MagnumTrade.cpp AnimationData-usage-mutable

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
         * Data types are usually closely related to @ref trackTargetType(),
         * see @ref AnimationTrackTargetType documentation for more
         * information.
         * @see @ref trackCount()
         */
        AnimationTrackType trackType(UnsignedInt id) const;

        /**
         * @brief Track result type
         * @param id        Track index
         *
         * In case track values are packed, track result type is different from
         * @ref trackType(). Data types are usually closely related to
         * @ref trackTargetType(), see @ref AnimationTrackTargetType
         * documentation for more information.
         * @see @ref trackCount()
         */
        AnimationTrackType trackResultType(UnsignedInt id) const;

        /**
         * @brief Track target type
         * @param id        Track index
         *
         * Particular animation targets usually correspond to a common
         * @ref trackType(), see @ref AnimationTrackTargetType documentation
         * for more information.
         * @see @ref trackCount()
         */
        AnimationTrackTargetType trackTargetType(UnsignedInt id) const;

        /**
         * @brief Track target
         * @param id        Track index
         *
         * For @ref trackTargetType() with
         * @ref AnimationTrackTargetType::Translation2D,
         * @ref AnimationTrackTargetType::Translation3D,
         * @ref AnimationTrackTargetType::Rotation2D,
         * @ref AnimationTrackTargetType::Rotation3D,
         * @ref AnimationTrackTargetType::Scaling2D,
         * @ref AnimationTrackTargetType::Scaling3D specifies object which
         * property is modified.
         * @see @ref trackCount(), @ref AbstractImporter::object2D(),
         *      @ref AbstractImporter::object3D()
         */
        UnsignedInt trackTarget(UnsignedInt id) const;

        /**
         * @brief Track data storage
         *
         * Returns the untyped base of a @ref Animation::TrackView, which
         * allows access only to some track properties. Use the templated and
         * checked version below to access a concrete @ref Animation::TrackView
         * type.
         */
        const Animation::TrackViewStorage<const Float>& track(UnsignedInt id) const;

        /**
         * @brief Mutable track data storage
         * @m_since{2020,06}
         *
         * Like @ref track(), but returns a mutable view. Expects that the
         * animation is mutable.
         * @see @ref dataFlags()
         */
        const Animation::TrackViewStorage<Float>& mutableTrack(UnsignedInt id);

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
        template<class V, class R = Animation::ResultOf<V>> const Animation::TrackView<const Float, const V, R>& track(UnsignedInt id) const;

        /**
         * @brief Mutable track data
         * @m_since{2020,06}
         *
         * Like @ref track(), but returns a mutable view. Expects that the
         * animation is mutable.
         * @see @ref dataFlags()
         */
        template<class V, class R = Animation::ResultOf<V>> const Animation::TrackView<Float, V, R>& mutableTrack(UnsignedInt id);

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

    template<> constexpr AnimationTrackType animationTypeFor<Math::BoolVector<2>>() { return AnimationTrackType::BoolVector2; }
    template<> constexpr AnimationTrackType animationTypeFor<Math::BoolVector<3>>() { return AnimationTrackType::BoolVector3; }
    template<> constexpr AnimationTrackType animationTypeFor<Math::BoolVector<4>>() { return AnimationTrackType::BoolVector4; }

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

template<class V, class R> inline AnimationTrackData::AnimationTrackData(AnimationTrackTargetType targetType, UnsignedInt target, Animation::TrackView<const Float, const V, R> view) noexcept: AnimationTrackData{Implementation::animationTypeFor<V>(), Implementation::animationTypeFor<R>(), targetType, target, view} {}

template<class V, class R> const Animation::TrackView<const Float, const V, R>& AnimationData::track(UnsignedInt id) const {
    const Animation::TrackViewStorage<const Float>& storage = track(id);
    CORRADE_ASSERT(Implementation::animationTypeFor<V>() == _tracks[id]._type, "Trade::AnimationData::track(): improper type requested for" << _tracks[id]._type, (static_cast<const Animation::TrackView<const Float, const V, R>&>(storage)));
    CORRADE_ASSERT(Implementation::animationTypeFor<R>() == _tracks[id]._resultType, "Trade::AnimationData::track(): improper result type requested for" << _tracks[id]._resultType, (static_cast<const Animation::TrackView<const Float, const V, R>&>(storage)));
    return static_cast<const Animation::TrackView<const Float, const V, R>&>(storage);
}

template<class V, class R> const Animation::TrackView<Float, V, R>& AnimationData::mutableTrack(UnsignedInt id) {
    const Animation::TrackViewStorage<Float>& storage = mutableTrack(id);
    CORRADE_ASSERT(Implementation::animationTypeFor<V>() == _tracks[id]._type, "Trade::AnimationData::mutableTrack(): improper type requested for" << _tracks[id]._type, (static_cast<const Animation::TrackView<Float, V, R>&>(storage)));
    CORRADE_ASSERT(Implementation::animationTypeFor<R>() == _tracks[id]._resultType, "Trade::AnimationData::mutableTrack(): improper result type requested for" << _tracks[id]._resultType, (static_cast<const Animation::TrackView<Float, V, R>&>(storage)));
    return static_cast<const Animation::TrackView<Float, V, R>&>(storage);
}

}}

#endif
