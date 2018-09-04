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

#include "AnimationData.h"

#include <Corrade/Utility/Debug.h>

#include "Magnum/Math/Vector4.h"
#include "Magnum/Math/Quaternion.h"

namespace Magnum { namespace Trade {

AnimationData::AnimationData(Containers::Array<char>&& data, Containers::Array<AnimationTrackData>&& tracks, const Range1D& duration, const void* importerState) noexcept: _duration{duration}, _data{std::move(data)}, _tracks{std::move(tracks)}, _importerState{importerState} {}

AnimationData::AnimationData(Containers::Array<char>&& data, Containers::Array<AnimationTrackData>&& tracks, const void* importerState) noexcept: _data{std::move(data)}, _tracks{std::move(tracks)}, _importerState{importerState} {
    if(!_tracks.empty()) {
        /* Reset duration to duration of the first track so it properly support
           cases where tracks don't start at 0 */
        _duration = _tracks.front()._view.duration();
        for(std::size_t i = 1; i != _tracks.size(); ++i)
            _duration = Math::join(_duration, _tracks[i]._view.duration());
    }
}

AnimationData::~AnimationData() = default;

AnimationData::AnimationData(AnimationData&&) noexcept = default;

AnimationData& AnimationData::operator=(AnimationData&&) noexcept = default;

AnimationTrackType AnimationData::trackType(UnsignedInt id) const {
    CORRADE_ASSERT(id < _tracks.size(), "Trade::AnimationData::trackType(): index out of range", {});
    return _tracks[id]._type;
}

AnimationTrackType AnimationData::trackResultType(UnsignedInt id) const {
    CORRADE_ASSERT(id < _tracks.size(), "Trade::AnimationData::trackResultType(): index out of range", {});
    return _tracks[id]._resultType;
}

AnimationTrackTarget AnimationData::trackTarget(UnsignedInt id) const {
    CORRADE_ASSERT(id < _tracks.size(), "Trade::AnimationData::trackTarget(): index out of range", {});
    return _tracks[id]._target;
}

UnsignedInt AnimationData::trackTargetId(UnsignedInt id) const {
    CORRADE_ASSERT(id < _tracks.size(), "Trade::AnimationData::trackTargetId(): index out of range", {});
    return _tracks[id]._targetId;
}

const Animation::TrackViewStorage<Float>& AnimationData::track(UnsignedInt id) const {
    CORRADE_ASSERT(id < _tracks.size(), "Trade::AnimationData::track(): index out of range", _tracks[id]._view);
    return _tracks[id]._view;
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
    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case AnimationTrackType::value: return debug << "Trade::AnimationTrackType::" #value;
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

    return debug << "Trade::AnimationTrackType(" << Debug::nospace << reinterpret_cast<void*>(UnsignedByte(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const AnimationTrackTarget value) {
    if(UnsignedByte(value) >= UnsignedByte(AnimationTrackTarget::Custom))
        return debug << "Trade::AnimationTrackTarget::Custom(" << Debug::nospace << UnsignedByte(value) << Debug::nospace << ")";

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case AnimationTrackTarget::value: return debug << "Trade::AnimationTrackTarget::" #value;
        _c(Translation2D)
        _c(Translation3D)
        _c(Rotation2D)
        _c(Rotation3D)
        _c(Scaling2D)
        _c(Scaling3D)
        #undef _c
        /* LCOV_EXCL_STOP */

        /* To silence compiler warning about unhandled values */
        case AnimationTrackTarget::Custom: CORRADE_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
    }

    return debug << "Trade::AnimationTrackTarget(" << Debug::nospace << reinterpret_cast<void*>(UnsignedByte(value)) << Debug::nospace << ")";
}
#endif

}}
