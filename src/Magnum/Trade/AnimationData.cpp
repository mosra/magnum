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

#include "AnimationData.h"

#include <Corrade/Utility/Debug.h>

#include "Magnum/Math/Vector4.h"
#include "Magnum/Math/Quaternion.h"
#include "Magnum/Trade/Implementation/arrayUtilities.h"

namespace Magnum { namespace Trade {

AnimationData::AnimationData(Containers::Array<char>&& data, Containers::Array<AnimationTrackData>&& tracks, const Range1D& duration, const void* importerState) noexcept: _dataFlags{DataFlag::Owned|DataFlag::Mutable}, _duration{duration}, _data{std::move(data)}, _tracks{std::move(tracks)}, _importerState{importerState} {}

AnimationData::AnimationData(Containers::Array<char>&& data, std::initializer_list<AnimationTrackData> tracks, const Range1D& duration, const void* importerState): AnimationData{std::move(data), Implementation::initializerListToArrayWithDefaultDeleter(tracks), duration, importerState} {}

AnimationData::AnimationData(const DataFlags dataFlags, const Containers::ArrayView<const void> data, Containers::Array<AnimationTrackData>&& tracks, const Range1D& duration, const void* importerState) noexcept: AnimationData{Containers::Array<char>{const_cast<char*>(static_cast<const char*>(data.data())), data.size(), Implementation::nonOwnedArrayDeleter}, std::move(tracks), duration, importerState} {
    CORRADE_ASSERT(!(dataFlags & DataFlag::Owned),
        "Trade::AnimationData: can't construct a non-owned instance with" << dataFlags, );
    _dataFlags = dataFlags;
}

AnimationData::AnimationData(const DataFlags dataFlags, const Containers::ArrayView<const void> data, std::initializer_list<AnimationTrackData> tracks, const Range1D& duration, const void* importerState): AnimationData{dataFlags, data, Implementation::initializerListToArrayWithDefaultDeleter(tracks), duration, importerState} {}

AnimationData::AnimationData(Containers::Array<char>&& data, Containers::Array<AnimationTrackData>&& tracks, const void* importerState) noexcept: _dataFlags{DataFlag::Owned|DataFlag::Mutable}, _data{std::move(data)}, _tracks{std::move(tracks)}, _importerState{importerState} {
    if(!_tracks.empty()) {
        /* Reset duration to duration of the first track so it properly support
           cases where tracks don't start at 0 */
        _duration = _tracks.front()._view.duration();
        for(std::size_t i = 1; i != _tracks.size(); ++i)
            _duration = Math::join(_duration, _tracks[i]._view.duration());
    }
}

AnimationData::AnimationData(Containers::Array<char>&& data, std::initializer_list<AnimationTrackData> tracks, const void* importerState): AnimationData{std::move(data), Implementation::initializerListToArrayWithDefaultDeleter(tracks), importerState} {}

AnimationData::AnimationData(const DataFlags dataFlags, const Containers::ArrayView<const void> data, Containers::Array<AnimationTrackData>&& tracks, const void* importerState) noexcept: AnimationData{Containers::Array<char>{const_cast<char*>(static_cast<const char*>(data.data())), data.size(), Implementation::nonOwnedArrayDeleter}, std::move(tracks), importerState} {
    CORRADE_ASSERT(!(dataFlags & DataFlag::Owned),
        "Trade::AnimationData: can't construct a non-owned instance with" << dataFlags, );
    _dataFlags = dataFlags;
}

AnimationData::AnimationData(const DataFlags dataFlags, const Containers::ArrayView<const void> data, std::initializer_list<AnimationTrackData> tracks, const void* importerState): AnimationData{dataFlags, data, Implementation::initializerListToArrayWithDefaultDeleter(tracks), importerState} {}

AnimationData::~AnimationData() = default;

AnimationData::AnimationData(AnimationData&&) noexcept = default;

AnimationData& AnimationData::operator=(AnimationData&&) noexcept = default;

Containers::ArrayView<char> AnimationData::mutableData() & {
    CORRADE_ASSERT(_dataFlags & DataFlag::Mutable,
        "Trade::AnimationData::mutableData(): the animation is not mutable", {});
    return _data;
}

AnimationTrackType AnimationData::trackType(UnsignedInt id) const {
    CORRADE_ASSERT(id < _tracks.size(), "Trade::AnimationData::trackType(): index out of range", {});
    return _tracks[id]._type;
}

AnimationTrackType AnimationData::trackResultType(UnsignedInt id) const {
    CORRADE_ASSERT(id < _tracks.size(), "Trade::AnimationData::trackResultType(): index out of range", {});
    return _tracks[id]._resultType;
}

AnimationTrackTargetType AnimationData::trackTargetType(UnsignedInt id) const {
    CORRADE_ASSERT(id < _tracks.size(), "Trade::AnimationData::trackTargetType(): index out of range", {});
    return _tracks[id]._targetType;
}

UnsignedInt AnimationData::trackTarget(UnsignedInt id) const {
    CORRADE_ASSERT(id < _tracks.size(), "Trade::AnimationData::trackTarget(): index out of range", {});
    return _tracks[id]._target;
}

const Animation::TrackViewStorage<const Float>& AnimationData::track(UnsignedInt id) const {
    CORRADE_ASSERT(id < _tracks.size(), "Trade::AnimationData::track(): index out of range", _tracks[id]._view);
    return _tracks[id]._view;
}

const Animation::TrackViewStorage<Float>& AnimationData::mutableTrack(UnsignedInt id) {
    CORRADE_ASSERT(_dataFlags & DataFlag::Mutable,
        "Trade::AnimationData::mutableTrack(): the animation is not mutable", reinterpret_cast<const Animation::TrackViewStorage<Float>&>(_tracks[id]._view));
    CORRADE_ASSERT(id < _tracks.size(), "Trade::AnimationData::track(): index out of range", reinterpret_cast<const Animation::TrackViewStorage<Float>&>(_tracks[id]._view));
    return reinterpret_cast<const Animation::TrackViewStorage<Float>&>(_tracks[id]._view);
}

Containers::Array<char> AnimationData::release() {
    _tracks = nullptr;
    return std::move(_data);
}

template<class V, class R> auto animationInterpolatorFor(Animation::Interpolation interpolation) -> R(*)(const V&, const V&, Float) {
    return Animation::interpolatorFor<V, R>(interpolation);
}

#ifndef DOXYGEN_GENERATING_OUTPUT
template MAGNUM_TRADE_EXPORT auto animationInterpolatorFor<bool, bool>(Animation::Interpolation) -> bool(*)(const bool&, const bool&, Float);
template MAGNUM_TRADE_EXPORT auto animationInterpolatorFor<Float, Float>(Animation::Interpolation) -> Float(*)(const Float&, const Float&, Float);
template MAGNUM_TRADE_EXPORT auto animationInterpolatorFor<UnsignedInt, UnsignedInt>(Animation::Interpolation) -> UnsignedInt(*)(const UnsignedInt&, const UnsignedInt&, Float);
template MAGNUM_TRADE_EXPORT auto animationInterpolatorFor<Int, Int>(Animation::Interpolation) -> Int(*)(const Int&, const Int&, Float);
template MAGNUM_TRADE_EXPORT auto animationInterpolatorFor<Math::BoolVector<2>, Math::BoolVector<2>>(Animation::Interpolation) -> Math::BoolVector<2>(*)(const Math::BoolVector<2>&, const Math::BoolVector<2>&, Float);
template MAGNUM_TRADE_EXPORT auto animationInterpolatorFor<Math::BoolVector<3>, Math::BoolVector<3>>(Animation::Interpolation) -> Math::BoolVector<3>(*)(const Math::BoolVector<3>&, const Math::BoolVector<3>&, Float);
template MAGNUM_TRADE_EXPORT auto animationInterpolatorFor<Math::BoolVector<4>, Math::BoolVector<4>>(Animation::Interpolation) -> Math::BoolVector<4>(*)(const Math::BoolVector<4>&, const Math::BoolVector<4>&, Float);
template MAGNUM_TRADE_EXPORT auto animationInterpolatorFor<Vector2, Vector2>(Animation::Interpolation) -> Vector2(*)(const Vector2&, const Vector2&, Float);
template MAGNUM_TRADE_EXPORT auto animationInterpolatorFor<Vector2i, Vector2i>(Animation::Interpolation) -> Vector2i(*)(const Vector2i&, const Vector2i&, Float);
template MAGNUM_TRADE_EXPORT auto animationInterpolatorFor<Vector2ui, Vector2ui>(Animation::Interpolation) -> Vector2ui(*)(const Vector2ui&, const Vector2ui&, Float);
template MAGNUM_TRADE_EXPORT auto animationInterpolatorFor<Vector3, Vector3>(Animation::Interpolation) -> Vector3(*)(const Vector3&, const Vector3&, Float);
template MAGNUM_TRADE_EXPORT auto animationInterpolatorFor<Vector3i, Vector3i>(Animation::Interpolation) -> Vector3i(*)(const Vector3i&, const Vector3i&, Float);
template MAGNUM_TRADE_EXPORT auto animationInterpolatorFor<Vector3ui, Vector3ui>(Animation::Interpolation) -> Vector3ui(*)(const Vector3ui&, const Vector3ui&, Float);
template MAGNUM_TRADE_EXPORT auto animationInterpolatorFor<Vector4, Vector4>(Animation::Interpolation) -> Vector4(*)(const Vector4&, const Vector4&, Float);
template MAGNUM_TRADE_EXPORT auto animationInterpolatorFor<Vector4d, Vector4d>(Animation::Interpolation) -> Vector4d(*)(const Vector4d&, const Vector4d&, Float);
template MAGNUM_TRADE_EXPORT auto animationInterpolatorFor<Vector4i, Vector4i>(Animation::Interpolation) -> Vector4i(*)(const Vector4i&, const Vector4i&, Float);
template MAGNUM_TRADE_EXPORT auto animationInterpolatorFor<Vector4ui, Vector4ui>(Animation::Interpolation) -> Vector4ui(*)(const Vector4ui&, const Vector4ui&, Float);
template MAGNUM_TRADE_EXPORT auto animationInterpolatorFor<Complex, Complex>(Animation::Interpolation) -> Complex(*)(const Complex&, const Complex&, Float);
template MAGNUM_TRADE_EXPORT auto animationInterpolatorFor<Quaternion, Quaternion>(Animation::Interpolation) -> Quaternion(*)(const Quaternion&, const Quaternion&, Float);
template MAGNUM_TRADE_EXPORT auto animationInterpolatorFor<DualQuaternion, DualQuaternion>(Animation::Interpolation) -> DualQuaternion(*)(const DualQuaternion&, const DualQuaternion&, Float);
template MAGNUM_TRADE_EXPORT auto animationInterpolatorFor<CubicHermite2D, Math::Vector2<Float>>(Animation::Interpolation) -> Math::Vector2<Float>(*)(const CubicHermite2D&, const CubicHermite2D&, Float);
template MAGNUM_TRADE_EXPORT auto animationInterpolatorFor<CubicHermite3D, Math::Vector3<Float>>(Animation::Interpolation) -> Math::Vector3<Float>(*)(const CubicHermite3D&, const CubicHermite3D&, Float);
template MAGNUM_TRADE_EXPORT auto animationInterpolatorFor<CubicHermiteComplex, Complex>(Animation::Interpolation) -> Complex(*)(const CubicHermiteComplex&, const CubicHermiteComplex&, Float);
template MAGNUM_TRADE_EXPORT auto animationInterpolatorFor<CubicHermiteQuaternion, Quaternion>(Animation::Interpolation) -> Quaternion(*)(const CubicHermiteQuaternion&, const CubicHermiteQuaternion&, Float);

Debug& operator<<(Debug& debug, const AnimationTrackType value) {
    debug << "Trade::AnimationTrackType" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case AnimationTrackType::value: return debug << "::" #value;
        _c(Bool)
        _c(Float)
        _c(UnsignedInt)
        _c(Int)
        _c(BoolVector2)
        _c(BoolVector3)
        _c(BoolVector4)
        _c(Vector2)
        _c(Vector2ui)
        _c(Vector2i)
        _c(Vector3)
        _c(Vector3ui)
        _c(Vector3i)
        _c(Vector4)
        _c(Vector4ui)
        _c(Vector4i)
        _c(Complex)
        _c(Quaternion)
        _c(DualQuaternion)
        _c(CubicHermite1D)
        _c(CubicHermite2D)
        _c(CubicHermite3D)
        _c(CubicHermiteComplex)
        _c(CubicHermiteQuaternion)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedByte(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const AnimationTrackTargetType value) {
    if(UnsignedByte(value) >= UnsignedByte(AnimationTrackTargetType::Custom))
        return debug << "Trade::AnimationTrackTargetType::Custom(" << Debug::nospace << UnsignedByte(value) << Debug::nospace << ")";

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case AnimationTrackTargetType::value: return debug << "Trade::AnimationTrackTargetType::" #value;
        _c(Translation2D)
        _c(Translation3D)
        _c(Rotation2D)
        _c(Rotation3D)
        _c(Scaling2D)
        _c(Scaling3D)
        #undef _c
        /* LCOV_EXCL_STOP */

        /* To silence compiler warning about unhandled values */
        case AnimationTrackTargetType::Custom: CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
    }

    return debug << "Trade::AnimationTrackTargetType(" << Debug::nospace << reinterpret_cast<void*>(UnsignedByte(value)) << Debug::nospace << ")";
}
#endif

}}
