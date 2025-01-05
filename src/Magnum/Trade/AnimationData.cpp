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

#include "AnimationData.h"

#include <Corrade/Utility/Debug.h>

#include "Magnum/Math/Vector4.h"
#include "Magnum/Math/Quaternion.h"
#include "Magnum/Trade/Implementation/arrayUtilities.h"

namespace Magnum { namespace Trade {

Debug& operator<<(Debug& debug, const AnimationTrackType value) {
    const bool packed = debug.immediateFlags() >= Debug::Flag::Packed;

    if(!packed)
        debug << "Trade::AnimationTrackType" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case AnimationTrackType::value: return debug << (packed ? "" : "::") << Debug::nospace << #value;
        _c(Bool)
        _c(Float)
        _c(UnsignedInt)
        _c(Int)
        _c(BitVector2)
        _c(BitVector3)
        _c(BitVector4)
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

    return debug << (packed ? "" : "(") << Debug::nospace << Debug::hex << UnsignedByte(value) << Debug::nospace << (packed ? "" : ")");
}

UnsignedInt animationTrackTypeSize(const AnimationTrackType type) {
    #ifdef CORRADE_TARGET_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic error "-Wswitch"
    #endif
    switch(type) {
        case AnimationTrackType::Bool:
        case AnimationTrackType::BitVector2:
        case AnimationTrackType::BitVector3:
        case AnimationTrackType::BitVector4:
            return 1;
        case AnimationTrackType::Float:
        case AnimationTrackType::UnsignedInt:
        case AnimationTrackType::Int:
            return 4;
        case AnimationTrackType::Vector2:
        case AnimationTrackType::Vector2ui:
        case AnimationTrackType::Vector2i:
        case AnimationTrackType::Complex:
            return 8;
        case AnimationTrackType::Vector3:
        case AnimationTrackType::Vector3ui:
        case AnimationTrackType::Vector3i:
        case AnimationTrackType::CubicHermite1D:
            return 12;
        case AnimationTrackType::Vector4:
        case AnimationTrackType::Vector4ui:
        case AnimationTrackType::Vector4i:
        case AnimationTrackType::Quaternion:
            return 16;
        case AnimationTrackType::CubicHermite2D:
        case AnimationTrackType::CubicHermiteComplex:
            return 24;
        case AnimationTrackType::DualQuaternion:
            return 32;
        case AnimationTrackType::CubicHermite3D:
            return 36;
        case AnimationTrackType::CubicHermiteQuaternion:
            return 48;
    }
    #ifdef CORRADE_TARGET_GCC
    #pragma GCC diagnostic pop
    #endif

    CORRADE_ASSERT_UNREACHABLE("Trade::animationTrackTypeSize(): invalid type" << type, {});
}

UnsignedInt animationTrackTypeAlignment(const AnimationTrackType type) {
    #ifdef CORRADE_TARGET_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic error "-Wswitch"
    #endif
    switch(type) {
        case AnimationTrackType::Bool:
        case AnimationTrackType::BitVector2:
        case AnimationTrackType::BitVector3:
        case AnimationTrackType::BitVector4:
            return 1;
        case AnimationTrackType::Float:
        case AnimationTrackType::UnsignedInt:
        case AnimationTrackType::Int:
        case AnimationTrackType::Vector2:
        case AnimationTrackType::Vector2ui:
        case AnimationTrackType::Vector2i:
        case AnimationTrackType::Complex:
        case AnimationTrackType::Vector3:
        case AnimationTrackType::Vector3ui:
        case AnimationTrackType::Vector3i:
        case AnimationTrackType::CubicHermite1D:
        case AnimationTrackType::Vector4:
        case AnimationTrackType::Vector4ui:
        case AnimationTrackType::Vector4i:
        case AnimationTrackType::Quaternion:
        case AnimationTrackType::CubicHermite2D:
        case AnimationTrackType::CubicHermiteComplex:
        case AnimationTrackType::DualQuaternion:
        case AnimationTrackType::CubicHermite3D:
        case AnimationTrackType::CubicHermiteQuaternion:
            return 4;
    }
    #ifdef CORRADE_TARGET_GCC
    #pragma GCC diagnostic pop
    #endif

    CORRADE_ASSERT_UNREACHABLE("Trade::animationTrackTypeAlignment(): invalid type" << type, {});
}

Debug& operator<<(Debug& debug, const AnimationTrackTarget value) {
    const bool packed = debug.immediateFlags() >= Debug::Flag::Packed;

    if(!packed)
        debug << "Trade::AnimationTrackTarget" << Debug::nospace;

    if(isAnimationTrackTargetCustom(value))
        return debug << (packed ? "Custom(" : "::Custom(") << Debug::nospace << animationTrackTargetCustom(value) << Debug::nospace << ")";

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case AnimationTrackTarget::value: return debug << (packed ? "" : "::") << Debug::nospace << #value;
        _c(Translation2D)
        _c(Translation3D)
        _c(Rotation2D)
        _c(Rotation3D)
        _c(Scaling2D)
        _c(Scaling3D)
        #undef _c
        /* LCOV_EXCL_STOP */

        #ifdef MAGNUM_BUILD_DEPRECATED
        CORRADE_IGNORE_DEPRECATED_PUSH
        /* To silence compiler warnings about unhandled values */
        case AnimationTrackTarget::Custom: CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
        CORRADE_IGNORE_DEPRECATED_POP
        #endif
    }

    return debug << (packed ? "" : "(") << Debug::nospace << Debug::hex << UnsignedShort(value) << Debug::nospace << (packed ? "" : ")");
}

namespace {

auto animationInterpolatorFor(const Animation::Interpolation interpolation, const AnimationTrackType type, const AnimationTrackType resultType) -> void(*)() {
    switch(type) {
        /* LCOV_EXCL_START */
        #define _ct(value, type_)                                           \
            case AnimationTrackType::value:                                 \
                if(type == resultType)                                      \
                    return reinterpret_cast<void(*)()>(Trade::animationInterpolatorFor<type_, type_>(interpolation)); \
                break;
        #define _c(type_) _ct(type_, type_)
        _ct(Bool, bool)
        _c(Float)
        _c(UnsignedInt)
        _c(Int)
        _c(BitVector2)
        _c(BitVector3)
        _c(BitVector4)
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
        #undef _c
        #undef _ct
        /* LCOV_EXCL_STOP */

        /* LCOV_EXCL_START */
        #define _cr(type_, resultType_)                                     \
            case AnimationTrackType::type_:                                 \
                if(resultType == AnimationTrackType::resultType_)           \
                    return reinterpret_cast<void(*)()>(Trade::animationInterpolatorFor<type_, resultType_>(interpolation)); \
                break;
        _cr(CubicHermite1D, Float)
        _cr(CubicHermite2D, Vector2)
        _cr(CubicHermite3D, Vector3)
        _cr(CubicHermiteComplex, Complex)
        _cr(CubicHermiteQuaternion, Quaternion)
        #undef _cr
        /* LCOV_EXCL_STOP */
    }

    /** @todo this doesn't print the types when e.g. a spline interpolation is
        requested for bool, how to fix? */
    CORRADE_ASSERT_UNREACHABLE("Trade::AnimationTrackData: can't deduce interpolator function for" << type << Debug::nospace << "," << resultType << "and" << interpolation, {});
}

}

AnimationTrackData::AnimationTrackData(const AnimationTrackTarget targetName, const UnsignedLong target, const AnimationTrackType type, const AnimationTrackType resultType, const Containers::StridedArrayView1D<const Float>& keys, const Containers::StridedArrayView1D<const void>& values, const Animation::Interpolation interpolation, void(*const interpolator)(), const Animation::Extrapolation before, const Animation::Extrapolation after) noexcept: _type{type}, _resultType{resultType}, _targetName{targetName}, _interpolation{interpolation}, _before{before}, _after{after}, _target{target}, _size{UnsignedInt(keys.size())}, _keysStride{Short(keys.stride())}, _valuesStride{Short(values.stride())}, _keysData{keys.data()}, _valuesData{values.data()}, _interpolator{interpolator} {
    CORRADE_ASSERT(keys.size() == values.size(),
        "Trade::AnimationTrackData: expected key and value view to have the same size but got" << keys.size() << "and" << values.size(), );
    #ifndef CORRADE_TARGET_32BIT
    CORRADE_ASSERT(keys.size() <= 0xffffffffu,
        "Trade::AnimationTrackData: expected keyframe count to fit into 32 bits but got" << keys.size(), );
    #endif
    CORRADE_ASSERT(keys.stride() >= -32768 && keys.stride() <= 32767,
        "Trade::AnimationTrackData: expected key stride to fit into 16 bits but got" << keys.stride(), );
    CORRADE_ASSERT(values.stride() >= -32768 && values.stride() <= 32767,
        "Trade::AnimationTrackData: expected value stride to fit into 16 bits but got" << values.stride(), );
}


AnimationTrackData::AnimationTrackData(const AnimationTrackTarget targetName, const UnsignedLong target, const AnimationTrackType type, const AnimationTrackType resultType, const Containers::StridedArrayView1D<const Float>& keys, const Containers::StridedArrayView1D<const void>& values, const Animation::Interpolation interpolation, const Animation::Extrapolation before, const Animation::Extrapolation after) noexcept: AnimationTrackData{targetName, target, type, resultType, keys, values, interpolation, animationInterpolatorFor(interpolation, type, resultType), before, after} {}

Animation::TrackViewStorage<const Float> AnimationTrackData::track() const {
    return Animation::TrackViewStorage<const Float>{
        /* We're sure the views are correct, circumventing the asserts */
        Containers::StridedArrayView1D<const Float>{{nullptr, ~std::size_t{}}, static_cast<const Float*>(_keysData), _size, _keysStride},
        Containers::StridedArrayView1D<const char>{{nullptr, ~std::size_t{}}, static_cast<const char*>(_valuesData), _size, _valuesStride},
        _interpolation, _interpolator, _before, _after};
}

AnimationData::AnimationData(Containers::Array<char>&& data, Containers::Array<AnimationTrackData>&& tracks, const Range1D& duration, const void* importerState) noexcept: _dataFlags{DataFlag::Owned|DataFlag::Mutable}, _duration{duration}, _data{Utility::move(data)}, _tracks{Utility::move(tracks)}, _importerState{importerState} {}

AnimationData::AnimationData(Containers::Array<char>&& data, std::initializer_list<AnimationTrackData> tracks, const Range1D& duration, const void* importerState): AnimationData{Utility::move(data), Implementation::initializerListToArrayWithDefaultDeleter(tracks), duration, importerState} {}

AnimationData::AnimationData(const DataFlags dataFlags, const Containers::ArrayView<const void> data, Containers::Array<AnimationTrackData>&& tracks, const Range1D& duration, const void* importerState) noexcept: AnimationData{Containers::Array<char>{const_cast<char*>(static_cast<const char*>(data.data())), data.size(), Implementation::nonOwnedArrayDeleter}, Utility::move(tracks), duration, importerState} {
    CORRADE_ASSERT(!(dataFlags & DataFlag::Owned),
        "Trade::AnimationData: can't construct a non-owned instance with" << dataFlags, );
    _dataFlags = dataFlags;
}

AnimationData::AnimationData(const DataFlags dataFlags, const Containers::ArrayView<const void> data, std::initializer_list<AnimationTrackData> tracks, const Range1D& duration, const void* importerState): AnimationData{dataFlags, data, Implementation::initializerListToArrayWithDefaultDeleter(tracks), duration, importerState} {}

AnimationData::AnimationData(Containers::Array<char>&& data, Containers::Array<AnimationTrackData>&& tracks, const void* importerState) noexcept: _dataFlags{DataFlag::Owned|DataFlag::Mutable}, _data{Utility::move(data)}, _tracks{Utility::move(tracks)}, _importerState{importerState} {
    if(!_tracks.isEmpty()) {
        const auto duration = [](const AnimationTrackData& track) {
            if(!track._size) return Range1D{};

            /* We're sure the view is correct, circumventing the assert */
            Containers::StridedArrayView1D<const Float> keys{{nullptr, ~std::size_t{}}, static_cast<const Float*>(track._keysData), track._size, track._keysStride};
            return Range1D{keys.front(), keys.back()};
        };

        /* Reset duration to duration of the first track so it properly support
           cases where tracks don't start at 0 */
        _duration = duration(_tracks.front());
        for(std::size_t i = 1; i != _tracks.size(); ++i)
            _duration = Math::join(_duration, duration(_tracks[i]));
    }
}

AnimationData::AnimationData(Containers::Array<char>&& data, std::initializer_list<AnimationTrackData> tracks, const void* importerState): AnimationData{Utility::move(data), Implementation::initializerListToArrayWithDefaultDeleter(tracks), importerState} {}

AnimationData::AnimationData(const DataFlags dataFlags, const Containers::ArrayView<const void> data, Containers::Array<AnimationTrackData>&& tracks, const void* importerState) noexcept: AnimationData{Containers::Array<char>{const_cast<char*>(static_cast<const char*>(data.data())), data.size(), Implementation::nonOwnedArrayDeleter}, Utility::move(tracks), importerState} {
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
    CORRADE_ASSERT(id < _tracks.size(),
        "Trade::AnimationData::trackType(): index" << id << "out of range for" << _tracks.size() << "tracks", {});
    return _tracks[id]._type;
}

AnimationTrackType AnimationData::trackResultType(UnsignedInt id) const {
    CORRADE_ASSERT(id < _tracks.size(),
        "Trade::AnimationData::trackResultType(): index" << id << "out of range for" << _tracks.size() << "tracks", {});
    return _tracks[id]._resultType;
}

AnimationTrackTarget AnimationData::trackTargetName(UnsignedInt id) const {
    CORRADE_ASSERT(id < _tracks.size(),
        "Trade::AnimationData::trackTargetName(): index" << id << "out of range for" << _tracks.size() << "tracks", {});
    return _tracks[id]._targetName;
}

UnsignedLong AnimationData::trackTarget(UnsignedInt id) const {
    CORRADE_ASSERT(id < _tracks.size(),
        "Trade::AnimationData::trackTarget(): index" << id << "out of range for" << _tracks.size() << "tracks", {});
    return _tracks[id]._target;
}

Animation::TrackViewStorage<const Float> AnimationData::track(UnsignedInt id) const {
    CORRADE_ASSERT(id < _tracks.size(),
        "Trade::AnimationData::track(): index" << id << "out of range for" << _tracks.size() << "tracks", _tracks[0].track());
    return _tracks[id].track();
}

Animation::TrackViewStorage<Float> AnimationData::mutableTrack(UnsignedInt id) {
    /* EW those casts! */
    CORRADE_ASSERT(_dataFlags & DataFlag::Mutable,
        "Trade::AnimationData::mutableTrack(): the animation is not mutable",
        reinterpret_cast<const Animation::TrackViewStorage<Float>&>(static_cast<const Animation::TrackViewStorage<const Float>&>(_tracks[0].track())));
    CORRADE_ASSERT(id < _tracks.size(),
        "Trade::AnimationData::mutableTrack(): index" << id << "out of range for" << _tracks.size() << "tracks",
        reinterpret_cast<const Animation::TrackViewStorage<Float>&>(static_cast<const Animation::TrackViewStorage<const Float>&>(_tracks[0].track())));
    return reinterpret_cast<const Animation::TrackViewStorage<Float>&>(static_cast<const Animation::TrackViewStorage<const Float>&>(_tracks[id].track()));
}

Containers::Array<char> AnimationData::release() {
    _tracks = nullptr;
    return Utility::move(_data);
}

template<class V, class R> auto animationInterpolatorFor(Animation::Interpolation interpolation) -> R(*)(const V&, const V&, Float) {
    return Animation::interpolatorFor<V, R>(interpolation);
}

template MAGNUM_TRADE_EXPORT auto animationInterpolatorFor<bool, bool>(Animation::Interpolation) -> bool(*)(const bool&, const bool&, Float);
template MAGNUM_TRADE_EXPORT auto animationInterpolatorFor<Float, Float>(Animation::Interpolation) -> Float(*)(const Float&, const Float&, Float);
template MAGNUM_TRADE_EXPORT auto animationInterpolatorFor<UnsignedInt, UnsignedInt>(Animation::Interpolation) -> UnsignedInt(*)(const UnsignedInt&, const UnsignedInt&, Float);
template MAGNUM_TRADE_EXPORT auto animationInterpolatorFor<Int, Int>(Animation::Interpolation) -> Int(*)(const Int&, const Int&, Float);
template MAGNUM_TRADE_EXPORT auto animationInterpolatorFor<Math::BitVector<2>, Math::BitVector<2>>(Animation::Interpolation) -> Math::BitVector<2>(*)(const Math::BitVector<2>&, const Math::BitVector<2>&, Float);
template MAGNUM_TRADE_EXPORT auto animationInterpolatorFor<Math::BitVector<3>, Math::BitVector<3>>(Animation::Interpolation) -> Math::BitVector<3>(*)(const Math::BitVector<3>&, const Math::BitVector<3>&, Float);
template MAGNUM_TRADE_EXPORT auto animationInterpolatorFor<Math::BitVector<4>, Math::BitVector<4>>(Animation::Interpolation) -> Math::BitVector<4>(*)(const Math::BitVector<4>&, const Math::BitVector<4>&, Float);
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

}}
