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

#include <Corrade/Containers/String.h>
#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Math/CubicHermite.h"
#include "Magnum/Math/DualQuaternion.h"
#include "Magnum/Trade/AnimationData.h"

namespace Magnum { namespace Trade { namespace Test { namespace {

struct AnimationDataTest: TestSuite::Tester {
    explicit AnimationDataTest();

    void trackTypeSizeAlignment();
    void trackTypeSizeAlignmentInvalid();
    void debugTrackType();
    void debugTrackTypePacked();

    void customTrackTarget();
    void customTrackTargetTooLarge();
    void customTrackTargetNotCustom();
    void debugTrackTarget();
    void debugTrackTargetPacked();

    void constructTrackDefault();
    void constructTrack();
    void constructTrackTypeErased();
    void constructTrackTypeErasedImplicitResultType();
    void constructTrackExplicitInterpolator();
    void constructTrackExplicitInterpolatorTypeErased();
    void constructTrackExplicitInterpolatorTypeErasedImplicitResultType();
    void constructTrackCustomInterpolator();
    void constructTrackCustomInterpolatorTypeErased();
    void constructTrackCustomInterpolatorTypeErasedImplicitResultType();
    void constructTrackFromView();
    #ifdef MAGNUM_BUILD_DEPRECATED
    void constructTrackDeprecated();
    void constructTrackDeprecatedImplicitResultType();
    #endif
    void constructTrackInconsitentViewSize();
    void constructTrackUnknownInterpolator();
    #ifndef CORRADE_TARGET_32BIT
    void constructTrackWrongSize();
    #endif
    void constructTrackWrongStride();

    void construct();
    void constructNotOwned();
    void constructImplicitDuration();
    void constructImplicitDurationEmpty();
    void constructImplicitDurationNotOwned();
    void constructNotOwnedFlagOwned();
    void constructImplicitDurationNotOwnedFlagOwned();

    void constructCopy();
    void constructMove();

    void mutableAccessNotAllowed();

    void trackCustomResultType();
    void trackWrongIndex();
    void trackWrongType();
    void trackWrongResultType();

    void release();
};

struct {
    const char* name;
    DataFlags dataFlags;
} NotOwnedData[] {
    {"", {}},
    {"mutable", DataFlag::Mutable},
};

AnimationDataTest::AnimationDataTest() {
    addTests({&AnimationDataTest::trackTypeSizeAlignment,
              &AnimationDataTest::trackTypeSizeAlignmentInvalid,
              &AnimationDataTest::debugTrackType,
              &AnimationDataTest::debugTrackTypePacked,

              &AnimationDataTest::customTrackTarget,
              &AnimationDataTest::customTrackTargetTooLarge,
              &AnimationDataTest::customTrackTargetNotCustom,
              &AnimationDataTest::debugTrackTarget,
              &AnimationDataTest::debugTrackTargetPacked,

              &AnimationDataTest::constructTrackDefault,
              &AnimationDataTest::constructTrack,
              &AnimationDataTest::constructTrackTypeErased,
              &AnimationDataTest::constructTrackTypeErasedImplicitResultType,
              &AnimationDataTest::constructTrackExplicitInterpolator,
              &AnimationDataTest::constructTrackExplicitInterpolatorTypeErased,
              &AnimationDataTest::constructTrackExplicitInterpolatorTypeErasedImplicitResultType,
              &AnimationDataTest::constructTrackCustomInterpolator,
              &AnimationDataTest::constructTrackCustomInterpolatorTypeErased,
              &AnimationDataTest::constructTrackCustomInterpolatorTypeErasedImplicitResultType,
              &AnimationDataTest::constructTrackFromView,
              #ifdef MAGNUM_BUILD_DEPRECATED
              &AnimationDataTest::constructTrackDeprecated,
              &AnimationDataTest::constructTrackDeprecatedImplicitResultType,
              #endif
              &AnimationDataTest::constructTrackInconsitentViewSize,
              &AnimationDataTest::constructTrackUnknownInterpolator,
              #ifndef CORRADE_TARGET_32BIT
              &AnimationDataTest::constructTrackWrongSize,
              #endif
              &AnimationDataTest::constructTrackWrongStride,

              &AnimationDataTest::construct,
              &AnimationDataTest::constructImplicitDuration,
              &AnimationDataTest::constructImplicitDurationEmpty});

    addInstancedTests({&AnimationDataTest::constructNotOwned,
                       &AnimationDataTest::constructImplicitDurationNotOwned},
        Containers::arraySize(NotOwnedData));

    addTests({&AnimationDataTest::constructNotOwnedFlagOwned,
              &AnimationDataTest::constructImplicitDurationNotOwnedFlagOwned,

              &AnimationDataTest::constructCopy,
              &AnimationDataTest::constructMove,

              &AnimationDataTest::mutableAccessNotAllowed,

              &AnimationDataTest::trackCustomResultType,
              &AnimationDataTest::trackWrongIndex,
              &AnimationDataTest::trackWrongType,
              &AnimationDataTest::trackWrongResultType,

              &AnimationDataTest::release});
}

using namespace Math::Literals;

void AnimationDataTest::trackTypeSizeAlignment() {
    /* Obvious cases */
    CORRADE_COMPARE(animationTrackTypeSize(AnimationTrackType::BitVector3), sizeof(BitVector3));
    CORRADE_COMPARE(animationTrackTypeSize(AnimationTrackType::UnsignedInt), sizeof(UnsignedInt));
    CORRADE_COMPARE(animationTrackTypeSize(AnimationTrackType::Vector2), sizeof(Vector2));
    CORRADE_COMPARE(animationTrackTypeSize(AnimationTrackType::Vector4i), sizeof(Vector4i));
    CORRADE_COMPARE(animationTrackTypeSize(AnimationTrackType::DualQuaternion), sizeof(DualQuaternion));
    /* Possibly non-obvious */
    CORRADE_COMPARE(animationTrackTypeSize(AnimationTrackType::CubicHermite1D), sizeof(CubicHermite1D));
    CORRADE_COMPARE(animationTrackTypeSize(AnimationTrackType::CubicHermiteComplex), sizeof(CubicHermiteComplex));
    CORRADE_COMPARE(animationTrackTypeSize(AnimationTrackType::CubicHermite3D), sizeof(CubicHermite3D));
    CORRADE_COMPARE(animationTrackTypeSize(AnimationTrackType::CubicHermiteQuaternion), sizeof(CubicHermiteQuaternion));

    /* Alignment is 4 for most types, except for bit-sized ones */
    CORRADE_COMPARE(animationTrackTypeAlignment(AnimationTrackType::BitVector4), 1);
    CORRADE_COMPARE(animationTrackTypeAlignment(AnimationTrackType::Float), alignof(Float));
    CORRADE_COMPARE(animationTrackTypeAlignment(AnimationTrackType::CubicHermiteQuaternion), alignof(CubicHermiteQuaternion));
}

void AnimationDataTest::trackTypeSizeAlignmentInvalid() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    animationTrackTypeSize(AnimationTrackType{});
    animationTrackTypeAlignment(AnimationTrackType{});
    animationTrackTypeSize(AnimationTrackType(0x73));
    animationTrackTypeAlignment(AnimationTrackType(0x73));
    CORRADE_COMPARE(out,
        "Trade::animationTrackTypeSize(): invalid type Trade::AnimationTrackType(0x0)\n"
        "Trade::animationTrackTypeAlignment(): invalid type Trade::AnimationTrackType(0x0)\n"
        "Trade::animationTrackTypeSize(): invalid type Trade::AnimationTrackType(0x73)\n"
        "Trade::animationTrackTypeAlignment(): invalid type Trade::AnimationTrackType(0x73)\n");
}

void AnimationDataTest::debugTrackType() {
    Containers::String out;

    Debug{&out} << AnimationTrackType::DualQuaternion << AnimationTrackType(0xde);
    CORRADE_COMPARE(out, "Trade::AnimationTrackType::DualQuaternion Trade::AnimationTrackType(0xde)\n");
}

void AnimationDataTest::debugTrackTypePacked() {
    Containers::String out;
    /* Second is not packed, the first should not make any flags persistent */
    Debug{&out} << Debug::packed << AnimationTrackType::DualQuaternion << Debug::packed << AnimationTrackType(0xde) << AnimationTrackType::Float;
    CORRADE_COMPARE(out, "DualQuaternion 0xde Trade::AnimationTrackType::Float\n");
}

void AnimationDataTest::customTrackTarget() {
    CORRADE_VERIFY(!isAnimationTrackTargetCustom(AnimationTrackTarget::Rotation3D));
    CORRADE_VERIFY(!isAnimationTrackTargetCustom(AnimationTrackTarget(32767)));
    CORRADE_VERIFY(isAnimationTrackTargetCustom(AnimationTrackTarget(Implementation::AnimationTrackTargetCustom)));
    CORRADE_VERIFY(isAnimationTrackTargetCustom(AnimationTrackTarget(65535)));

    CORRADE_COMPARE(UnsignedShort(animationTrackTargetCustom(0)), 32768);
    CORRADE_COMPARE(UnsignedShort(animationTrackTargetCustom(8290)), 41058);
    CORRADE_COMPARE(UnsignedShort(animationTrackTargetCustom(32767)), 65535);

    CORRADE_COMPARE(animationTrackTargetCustom(AnimationTrackTarget(Implementation::AnimationTrackTargetCustom)), 0);
    CORRADE_COMPARE(animationTrackTargetCustom(AnimationTrackTarget(41058)), 8290);
    CORRADE_COMPARE(animationTrackTargetCustom(AnimationTrackTarget(65535)), 32767);

    constexpr bool is = isAnimationTrackTargetCustom(AnimationTrackTarget(41058));
    CORRADE_VERIFY(is);
    constexpr AnimationTrackTarget a = animationTrackTargetCustom(8290);
    CORRADE_COMPARE(UnsignedShort(a), 41058);
    constexpr UnsignedShort b = animationTrackTargetCustom(a);
    CORRADE_COMPARE(b, 8290);
}

void AnimationDataTest::customTrackTargetTooLarge() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    animationTrackTargetCustom(32768);
    CORRADE_COMPARE(out, "Trade::animationTrackTargetCustom(): index 32768 too large\n");
}

void AnimationDataTest::customTrackTargetNotCustom() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    animationTrackTargetCustom(AnimationTrackTarget::Translation2D);
    CORRADE_COMPARE(out, "Trade::animationTrackTargetCustom(): Trade::AnimationTrackTarget::Translation2D is not custom\n");
}

void AnimationDataTest::debugTrackTarget() {
    Containers::String out;

    Debug{&out} << AnimationTrackTarget::Rotation3D << animationTrackTargetCustom(9) << AnimationTrackTarget(0x4242);
    CORRADE_COMPARE(out, "Trade::AnimationTrackTarget::Rotation3D Trade::AnimationTrackTarget::Custom(9) Trade::AnimationTrackTarget(0x4242)\n");
}

void AnimationDataTest::debugTrackTargetPacked() {
    Containers::String out;
    /* Last is not packed, ones before should not make any flags persistent */
    Debug{&out} << Debug::packed << AnimationTrackTarget::Rotation3D << Debug::packed << animationTrackTargetCustom(120) << Debug::packed << AnimationTrackTarget(0x4242) << AnimationTrackType::Float;
    CORRADE_COMPARE(out, "Rotation3D Custom(120) 0x4242 Trade::AnimationTrackType::Float\n");
}

void AnimationDataTest::constructTrackDefault() {
    AnimationTrackData data;
    CORRADE_COMPARE(data.type(), AnimationTrackType{});
    CORRADE_COMPARE(data.resultType(), AnimationTrackType{});
    CORRADE_COMPARE(data.targetName(), AnimationTrackTarget{});
    CORRADE_COMPARE(data.target(), 0);

    CORRADE_COMPARE(data.track().interpolation(), Animation::Interpolation::Constant);
    CORRADE_COMPARE(data.track().before(), Animation::Extrapolation::Extrapolated);
    CORRADE_COMPARE(data.track().after(), Animation::Extrapolation::Extrapolated);
    CORRADE_COMPARE(data.track().size(), 0);
    CORRADE_COMPARE(data.track().interpolator(), nullptr);
    CORRADE_COMPARE(data.track().keys().data(), nullptr);
    CORRADE_COMPARE(data.track().values().data(), nullptr);
}

void AnimationDataTest::constructTrack() {
    struct Keyframe {
        Float time;
        CubicHermite2D value;
    } keyframes[3]; /* {} makes GCC 4.8 crash */

    AnimationTrackData data{
         AnimationTrackTarget::Scaling2D, 42,
         Containers::stridedArrayView(keyframes).slice(&Keyframe::time),
         Containers::stridedArrayView(keyframes).slice(&Keyframe::value),
         Animation::Interpolation::Linear,
         Animation::Extrapolation::Constant};
    CORRADE_COMPARE(data.targetName(), AnimationTrackTarget::Scaling2D);
    CORRADE_COMPARE(data.target(), 42);
    CORRADE_COMPARE(data.type(), AnimationTrackType::CubicHermite2D);
    CORRADE_COMPARE(data.resultType(), AnimationTrackType::Vector2);
    CORRADE_COMPARE(data.track().keys().data(), &keyframes[0].time);
    CORRADE_COMPARE(data.track().values().data(), &keyframes[0].value);
    CORRADE_COMPARE(data.track().interpolation(), Animation::Interpolation::Linear);
    CORRADE_COMPARE(data.track().interpolator(), reinterpret_cast<void(*)()>(animationInterpolatorFor<CubicHermite2D>(Animation::Interpolation::Linear)));
    CORRADE_COMPARE(data.track().before(), Animation::Extrapolation::Constant);
    CORRADE_COMPARE(data.track().after(), Animation::Extrapolation::Constant);
}

void AnimationDataTest::constructTrackTypeErased() {
    struct Keyframe {
        Float time;
        CubicHermite3D value;
    } keyframes[3]; /* {} makes GCC 4.8 crash */

    AnimationTrackData data{
         AnimationTrackTarget::Translation3D, 42,
         AnimationTrackType::CubicHermite3D,
         AnimationTrackType::Vector3,
         Containers::stridedArrayView(keyframes).slice(&Keyframe::time),
         Containers::stridedArrayView(keyframes).slice(&Keyframe::value),
         Animation::Interpolation::Spline,
         Animation::Extrapolation::DefaultConstructed};
    CORRADE_COMPARE(data.targetName(), AnimationTrackTarget::Translation3D);
    CORRADE_COMPARE(data.target(), 42);
    CORRADE_COMPARE(data.type(), AnimationTrackType::CubicHermite3D);
    CORRADE_COMPARE(data.resultType(), AnimationTrackType::Vector3);
    CORRADE_COMPARE(data.track().keys().data(), &keyframes[0].time);
    CORRADE_COMPARE(data.track().values().data(), &keyframes[0].value);
    CORRADE_COMPARE(data.track().interpolation(), Animation::Interpolation::Spline);
    CORRADE_COMPARE(data.track().interpolator(), reinterpret_cast<void(*)()>(animationInterpolatorFor<CubicHermite3D>(Animation::Interpolation::Spline)));
    CORRADE_COMPARE(data.track().before(), Animation::Extrapolation::DefaultConstructed);
    CORRADE_COMPARE(data.track().after(), Animation::Extrapolation::DefaultConstructed);
}

void AnimationDataTest::constructTrackTypeErasedImplicitResultType() {
    struct Keyframe {
        Float time;
        Quaternion value;
    } keyframes[3]; /* {} makes GCC 4.8 crash */

    AnimationTrackData data{
         AnimationTrackTarget::Rotation3D, 42,
         AnimationTrackType::Quaternion,
         Containers::stridedArrayView(keyframes).slice(&Keyframe::time),
         Containers::stridedArrayView(keyframes).slice(&Keyframe::value),
         Animation::Interpolation::Linear,
         Animation::Extrapolation::DefaultConstructed};
    CORRADE_COMPARE(data.targetName(), AnimationTrackTarget::Rotation3D);
    CORRADE_COMPARE(data.target(), 42);
    CORRADE_COMPARE(data.type(), AnimationTrackType::Quaternion);
    CORRADE_COMPARE(data.resultType(), AnimationTrackType::Quaternion);
    CORRADE_COMPARE(data.track().keys().data(), &keyframes[0].time);
    CORRADE_COMPARE(data.track().values().data(), &keyframes[0].value);
    CORRADE_COMPARE(data.track().interpolation(), Animation::Interpolation::Linear);
    CORRADE_COMPARE(data.track().interpolator(), reinterpret_cast<void(*)()>(animationInterpolatorFor<Quaternion>(Animation::Interpolation::Linear)));
    CORRADE_COMPARE(data.track().before(), Animation::Extrapolation::DefaultConstructed);
    CORRADE_COMPARE(data.track().after(), Animation::Extrapolation::DefaultConstructed);
}

void AnimationDataTest::constructTrackExplicitInterpolator() {
    struct Keyframe {
        Float time;
        CubicHermite2D value;
    } keyframes[3]; /* {} makes GCC 4.8 crash */

    AnimationTrackData data{
         AnimationTrackTarget::Scaling2D, 42,
         Containers::stridedArrayView(keyframes).slice(&Keyframe::time),
         Containers::stridedArrayView(keyframes).slice(&Keyframe::value),
         Animation::Interpolation::Spline,
         Math::splerp,
         Animation::Extrapolation::Constant};
    CORRADE_COMPARE(data.targetName(), AnimationTrackTarget::Scaling2D);
    CORRADE_COMPARE(data.target(), 42);
    CORRADE_COMPARE(data.type(), AnimationTrackType::CubicHermite2D);
    CORRADE_COMPARE(data.resultType(), AnimationTrackType::Vector2);
    CORRADE_COMPARE(data.track().keys().data(), &keyframes[0].time);
    CORRADE_COMPARE(data.track().values().data(), &keyframes[0].value);
    CORRADE_COMPARE(data.track().interpolation(), Animation::Interpolation::Spline);
    CORRADE_COMPARE(data.track().interpolator(), reinterpret_cast<void(*)()>(static_cast<Vector2(*)(const CubicHermite2D&, const CubicHermite2D&, Float)>(Math::splerp)));
    CORRADE_COMPARE(data.track().before(), Animation::Extrapolation::Constant);
    CORRADE_COMPARE(data.track().after(), Animation::Extrapolation::Constant);
}

void AnimationDataTest::constructTrackExplicitInterpolatorTypeErased() {
    struct Keyframe {
        Float time;
        CubicHermite2D value;
    } keyframes[3]; /* {} makes GCC 4.8 crash */

    auto interpolator = reinterpret_cast<void(*)()>(static_cast<Vector2(*)(const CubicHermite2D&, const CubicHermite2D&, Float)>(Math::splerp));

    AnimationTrackData data{
         AnimationTrackTarget::Scaling2D, 42,
         AnimationTrackType::CubicHermite2D,
         AnimationTrackType::Vector2,
         Containers::stridedArrayView(keyframes).slice(&Keyframe::time),
         Containers::stridedArrayView(keyframes).slice(&Keyframe::value),
         Animation::Interpolation::Spline,
         interpolator,
         Animation::Extrapolation::Constant};
    CORRADE_COMPARE(data.targetName(), AnimationTrackTarget::Scaling2D);
    CORRADE_COMPARE(data.target(), 42);
    CORRADE_COMPARE(data.type(), AnimationTrackType::CubicHermite2D);
    CORRADE_COMPARE(data.resultType(), AnimationTrackType::Vector2);
    CORRADE_COMPARE(data.track().keys().data(), &keyframes[0].time);
    CORRADE_COMPARE(data.track().values().data(), &keyframes[0].value);
    CORRADE_COMPARE(data.track().interpolation(), Animation::Interpolation::Spline);
    CORRADE_COMPARE(data.track().interpolator(), interpolator);
    CORRADE_COMPARE(data.track().before(), Animation::Extrapolation::Constant);
    CORRADE_COMPARE(data.track().after(), Animation::Extrapolation::Constant);
}

void AnimationDataTest::constructTrackExplicitInterpolatorTypeErasedImplicitResultType() {
    struct Keyframe {
        Float time;
        Vector3 value;
    } keyframes[3]; /* {} makes GCC 4.8 crash */

    auto interpolator = reinterpret_cast<void(*)()>(static_cast<Vector3(*)(const Vector3&, const Vector3&, Float)>(Math::lerp));

    AnimationTrackData data{
         AnimationTrackTarget::Translation3D, 42,
         AnimationTrackType::Vector3,
         Containers::stridedArrayView(keyframes).slice(&Keyframe::time),
         Containers::stridedArrayView(keyframes).slice(&Keyframe::value),
         Animation::Interpolation::Linear,
         interpolator,
         Animation::Extrapolation::DefaultConstructed};
    CORRADE_COMPARE(data.targetName(), AnimationTrackTarget::Translation3D);
    CORRADE_COMPARE(data.target(), 42);
    CORRADE_COMPARE(data.type(), AnimationTrackType::Vector3);
    CORRADE_COMPARE(data.resultType(), AnimationTrackType::Vector3);
    CORRADE_COMPARE(data.track().keys().data(), &keyframes[0].time);
    CORRADE_COMPARE(data.track().values().data(), &keyframes[0].value);
    CORRADE_COMPARE(data.track().interpolation(), Animation::Interpolation::Linear);
    CORRADE_COMPARE(data.track().interpolator(), interpolator);
    CORRADE_COMPARE(data.track().before(), Animation::Extrapolation::DefaultConstructed);
    CORRADE_COMPARE(data.track().after(), Animation::Extrapolation::DefaultConstructed);
}

void AnimationDataTest::constructTrackCustomInterpolator() {
    struct Keyframe {
        Float time;
        CubicHermite2D value;
    } keyframes[3]; /* {} makes GCC 4.8 crash */

    AnimationTrackData data{
         AnimationTrackTarget::Scaling2D, 42,
         Containers::stridedArrayView(keyframes).slice(&Keyframe::time),
         Containers::stridedArrayView(keyframes).slice(&Keyframe::value),
         Math::splerp,
         Animation::Extrapolation::Constant};
    CORRADE_COMPARE(data.targetName(), AnimationTrackTarget::Scaling2D);
    CORRADE_COMPARE(data.target(), 42);
    CORRADE_COMPARE(data.type(), AnimationTrackType::CubicHermite2D);
    CORRADE_COMPARE(data.resultType(), AnimationTrackType::Vector2);
    CORRADE_COMPARE(data.track().keys().data(), &keyframes[0].time);
    CORRADE_COMPARE(data.track().values().data(), &keyframes[0].value);
    CORRADE_COMPARE(data.track().interpolation(), Animation::Interpolation::Custom);
    CORRADE_COMPARE(data.track().interpolator(), reinterpret_cast<void(*)()>(static_cast<Vector2(*)(const CubicHermite2D&, const CubicHermite2D&, Float)>(Math::splerp)));
    CORRADE_COMPARE(data.track().before(), Animation::Extrapolation::Constant);
    CORRADE_COMPARE(data.track().after(), Animation::Extrapolation::Constant);
}

void AnimationDataTest::constructTrackCustomInterpolatorTypeErased() {
    struct Keyframe {
        Float time;
        CubicHermite2D value;
    } keyframes[3]; /* {} makes GCC 4.8 crash */

    auto interpolator = reinterpret_cast<void(*)()>(static_cast<Vector2(*)(const CubicHermite2D&, const CubicHermite2D&, Float)>(Math::splerp));

    AnimationTrackData data{
         AnimationTrackTarget::Scaling2D, 42,
         AnimationTrackType::CubicHermite2D,
         AnimationTrackType::Vector2,
         Containers::stridedArrayView(keyframes).slice(&Keyframe::time),
         Containers::stridedArrayView(keyframes).slice(&Keyframe::value),
         interpolator,
         Animation::Extrapolation::Constant};
    CORRADE_COMPARE(data.targetName(), AnimationTrackTarget::Scaling2D);
    CORRADE_COMPARE(data.target(), 42);
    CORRADE_COMPARE(data.type(), AnimationTrackType::CubicHermite2D);
    CORRADE_COMPARE(data.resultType(), AnimationTrackType::Vector2);
    CORRADE_COMPARE(data.track().keys().data(), &keyframes[0].time);
    CORRADE_COMPARE(data.track().values().data(), &keyframes[0].value);
    CORRADE_COMPARE(data.track().interpolation(), Animation::Interpolation::Custom);
    CORRADE_COMPARE(data.track().interpolator(), interpolator);
    CORRADE_COMPARE(data.track().before(), Animation::Extrapolation::Constant);
    CORRADE_COMPARE(data.track().after(), Animation::Extrapolation::Constant);
}

void AnimationDataTest::constructTrackCustomInterpolatorTypeErasedImplicitResultType() {
    struct Keyframe {
        Float time;
        Vector3 value;
    } keyframes[3]; /* {} makes GCC 4.8 crash */

    auto interpolator = reinterpret_cast<void(*)()>(static_cast<Vector3(*)(const Vector3&, const Vector3&, Float)>(Math::lerp));

    AnimationTrackData data{
         AnimationTrackTarget::Translation3D, 42,
         AnimationTrackType::Vector3,
         Containers::stridedArrayView(keyframes).slice(&Keyframe::time),
         Containers::stridedArrayView(keyframes).slice(&Keyframe::value),
         interpolator,
         Animation::Extrapolation::DefaultConstructed};
    CORRADE_COMPARE(data.targetName(), AnimationTrackTarget::Translation3D);
    CORRADE_COMPARE(data.target(), 42);
    CORRADE_COMPARE(data.type(), AnimationTrackType::Vector3);
    CORRADE_COMPARE(data.resultType(), AnimationTrackType::Vector3);
    CORRADE_COMPARE(data.track().keys().data(), &keyframes[0].time);
    CORRADE_COMPARE(data.track().values().data(), &keyframes[0].value);
    CORRADE_COMPARE(data.track().interpolation(), Animation::Interpolation::Custom);
    CORRADE_COMPARE(data.track().interpolator(), interpolator);
    CORRADE_COMPARE(data.track().before(), Animation::Extrapolation::DefaultConstructed);
    CORRADE_COMPARE(data.track().after(), Animation::Extrapolation::DefaultConstructed);
}

void AnimationDataTest::constructTrackFromView() {
    AnimationTrackData data{
         AnimationTrackTarget::Translation3D, 42,
         Animation::TrackView<const Float, const CubicHermite3D>{
            nullptr,
            Animation::Interpolation::Linear,
            animationInterpolatorFor<CubicHermite3D>(Animation::Interpolation::Linear)}};
    CORRADE_COMPARE(data.type(), AnimationTrackType::CubicHermite3D);
    CORRADE_COMPARE(data.resultType(), AnimationTrackType::Vector3);
    CORRADE_COMPARE(data.targetName(), AnimationTrackTarget::Translation3D);
    CORRADE_COMPARE(data.target(), 42);
    CORRADE_COMPARE(data.track().interpolation(), Animation::Interpolation::Linear);
}

#ifdef MAGNUM_BUILD_DEPRECATED
void AnimationDataTest::constructTrackDeprecated() {
    CORRADE_IGNORE_DEPRECATED_PUSH
    AnimationTrackData data{
         AnimationTrackType::CubicHermite3D,
         AnimationTrackType::Vector3,
         AnimationTrackTarget::Translation3D, 42,
         Animation::TrackView<const Float, const CubicHermite3D>{
            nullptr,
            Animation::Interpolation::Linear,
            animationInterpolatorFor<CubicHermite3D>(Animation::Interpolation::Linear)}};
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(data.type(), AnimationTrackType::CubicHermite3D);
    CORRADE_COMPARE(data.resultType(), AnimationTrackType::Vector3);
    CORRADE_COMPARE(data.targetName(), AnimationTrackTarget::Translation3D);
    CORRADE_COMPARE(data.target(), 42);
    CORRADE_COMPARE(data.track().interpolation(), Animation::Interpolation::Linear);
}

void AnimationDataTest::constructTrackDeprecatedImplicitResultType() {
    CORRADE_IGNORE_DEPRECATED_PUSH
    AnimationTrackData data{
         AnimationTrackType::Vector3,
         AnimationTrackTarget::Translation3D, 42,
         Animation::TrackView<const Float, const Vector3>{
            nullptr,
            Animation::Interpolation::Linear,
            animationInterpolatorFor<Vector3>(Animation::Interpolation::Linear)}};
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE(data.type(), AnimationTrackType::Vector3);
    CORRADE_COMPARE(data.resultType(), AnimationTrackType::Vector3);
    CORRADE_COMPARE(data.targetName(), AnimationTrackTarget::Translation3D);
    CORRADE_COMPARE(data.target(), 42);
    CORRADE_COMPARE(data.track().interpolation(), Animation::Interpolation::Linear);
}
#endif

void AnimationDataTest::constructTrackInconsitentViewSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Float time[2];
    Vector2 value[3];
    Containers::String out;
    Error redirectError{&out};
    AnimationTrackData{
         AnimationTrackTarget::Rotation3D, 42,
         Containers::stridedArrayView(time),
         Containers::stridedArrayView(value),
         Animation::Interpolation::Linear};
    CORRADE_COMPARE(out, "Trade::AnimationTrackData: expected key and value view to have the same size but got 2 and 3\n");
}

void AnimationDataTest::constructTrackUnknownInterpolator() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    AnimationTrackData{
         AnimationTrackTarget::Rotation3D, 42,
         AnimationTrackType::CubicHermite1D,
         AnimationTrackType::Vector2,
         nullptr,
         nullptr,
         Animation::Interpolation::Linear};
    AnimationTrackData{
         AnimationTrackTarget::Rotation3D, 42,
         AnimationTrackType::Vector3,
         nullptr,
         nullptr,
         Animation::Interpolation::Spline};
    CORRADE_COMPARE(out,
        "Trade::AnimationTrackData: can't deduce interpolator function for Trade::AnimationTrackType::CubicHermite1D, Trade::AnimationTrackType::Vector2 and Animation::Interpolation::Linear\n"
        /* This assertion is from the delegated-to interpolationFor(), which
           unfortunately doesn't print the types */
        "Animation::interpolatorFor(): can't deduce interpolator function for Animation::Interpolation::Spline\n");
}

#ifndef CORRADE_TARGET_32BIT
void AnimationDataTest::constructTrackWrongSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    /* This should be fine */
    AnimationTrackData{AnimationTrackTarget::Rotation3D, 16, Animation::TrackView<const Float, const Quaternion>{
        {nullptr, 0xffffffffu},
        {nullptr, 0xffffffffu},
        Animation::Interpolation::Constant
    }};

    Containers::String out;
    Error redirectError{&out};
    AnimationTrackData{AnimationTrackTarget::Rotation3D, 16, Animation::TrackView<const Float, const Quaternion>{
        {nullptr, 0x100000000ull},
        {nullptr, 0x100000000ull},
        Animation::Interpolation::Constant
    }};
    CORRADE_COMPARE(out,
        "Trade::AnimationTrackData: expected keyframe count to fit into 32 bits but got 4294967296\n");
}
#endif

void AnimationDataTest::constructTrackWrongStride() {
    CORRADE_SKIP_IF_NO_ASSERT();

    char toomuch[2*(32768 + sizeof(Vector2))];

    /* These should be fine */
    AnimationTrackData{AnimationTrackTarget::Scaling2D, 1, Animation::TrackView<const Float, const Vector2>{
        Containers::StridedArrayView1D<Float>{Containers::arrayCast<Float>(toomuch), 2, 32767},
        Containers::StridedArrayView1D<Vector2>{Containers::arrayCast<Vector2>(toomuch), 2, 32768}.flipped<0>(),
        Animation::Interpolation::Constant
    }};
    AnimationTrackData{AnimationTrackTarget::Scaling2D, 1, Animation::TrackView<const Float, const Vector2>{
        Containers::StridedArrayView1D<Float>{Containers::arrayCast<Float>(toomuch), 2, 32768}.flipped<0>(),
        Containers::StridedArrayView1D<Vector2>{Containers::arrayCast<Vector2>(toomuch), 2, 32767},
        Animation::Interpolation::Constant
    }};

    Containers::String out;
    Error redirectError{&out};
    AnimationTrackData{AnimationTrackTarget::Scaling2D, 1, Animation::TrackView<const Float, const Vector2>{
        Containers::StridedArrayView1D<Float>{Containers::arrayCast<Float>(toomuch), 2, 32768},
        Containers::StridedArrayView1D<Vector2>{Containers::arrayCast<Vector2>(toomuch), 2, 32767},
        Animation::Interpolation::Constant
    }};
    AnimationTrackData{AnimationTrackTarget::Scaling2D, 1, Animation::TrackView<const Float, const Vector2>{
        Containers::StridedArrayView1D<Float>{Containers::arrayCast<Float>(toomuch), 2, 32769}.flipped<0>(),
        Containers::StridedArrayView1D<Vector2>{Containers::arrayCast<Vector2>(toomuch), 2, 32767},
        Animation::Interpolation::Constant
    }};
    AnimationTrackData{AnimationTrackTarget::Scaling2D, 1, Animation::TrackView<const Float, const Vector2>{
        Containers::StridedArrayView1D<Float>{Containers::arrayCast<Float>(toomuch), 2, 32767},
        Containers::StridedArrayView1D<Vector2>{Containers::arrayCast<Vector2>(toomuch), 2, 32768},
        Animation::Interpolation::Constant
    }};
    AnimationTrackData{AnimationTrackTarget::Scaling2D, 1, Animation::TrackView<const Float, const Vector2>{
        Containers::StridedArrayView1D<Float>{Containers::arrayCast<Float>(toomuch), 2, 32767},
        Containers::StridedArrayView1D<Vector2>{Containers::arrayCast<Vector2>(toomuch), 2, 32769}.flipped<0>(),
        Animation::Interpolation::Constant
    }};
    CORRADE_COMPARE(out,
        "Trade::AnimationTrackData: expected key stride to fit into 16 bits but got 32768\n"
        "Trade::AnimationTrackData: expected key stride to fit into 16 bits but got -32769\n"
        "Trade::AnimationTrackData: expected value stride to fit into 16 bits but got 32768\n"
        "Trade::AnimationTrackData: expected value stride to fit into 16 bits but got -32769\n");
}

void AnimationDataTest::construct() {
    struct Data {
        Float time;
        Vector3 position;
        Quaternion rotation;
    };
    Containers::Array<char> buffer{sizeof(Data)*3};
    auto view = Containers::arrayCast<Data>(buffer);
    view[0] = {0.0f, {3.0f, 1.0f, 0.1f}, Quaternion::rotation(45.0_degf, Vector3::yAxis())};
    view[1] = {5.0f, {0.3f, 0.6f, 1.0f}, Quaternion::rotation(20.0_degf, Vector3::yAxis())};
    view[2] = {7.5f, {1.0f, 0.3f, 2.1f}, Quaternion{}};

    const int state = 5;
    AnimationData data{Utility::move(buffer), {
        AnimationTrackData{AnimationTrackTarget::Translation3D, 42,
            Animation::TrackView<const Float, const Vector3>{
                {view, &view[0].time, view.size(), sizeof(Data)},
                {view, &view[0].position, view.size(), sizeof(Data)},
                Animation::Interpolation::Constant,
                animationInterpolatorFor<Vector3>(Animation::Interpolation::Constant)}},
        AnimationTrackData{AnimationTrackTarget::Rotation3D, 1337,
            Animation::TrackView<const Float, const Quaternion>{
                {view, &view[0].time, view.size(), sizeof(Data)},
                {view, &view[0].rotation, view.size(), sizeof(Data)},
                Animation::Interpolation::Linear,
                animationInterpolatorFor<Quaternion>(Animation::Interpolation::Linear)}}
        }, {-1.0f, 7.0f}, &state};

    CORRADE_COMPARE(data.dataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_COMPARE(data.duration(), (Range1D{-1.0f, 7.0f}));
    CORRADE_COMPARE(static_cast<const void*>(data.data().data()), view.data());
    CORRADE_COMPARE(static_cast<void*>(data.mutableData().data()), view.data());
    CORRADE_COMPARE(data.trackCount(), 2);
    CORRADE_COMPARE(data.importerState(), &state);

    {
        CORRADE_COMPARE(data.trackType(0), AnimationTrackType::Vector3);
        CORRADE_COMPARE(data.trackResultType(0), AnimationTrackType::Vector3);
        CORRADE_COMPARE(data.trackTargetName(0), AnimationTrackTarget::Translation3D);
        CORRADE_COMPARE(data.trackTarget(0), 42);

        Animation::TrackView<const Float, const Vector3> track = data.track<Vector3>(0);
        CORRADE_COMPARE(track.keys().size(), 3);
        CORRADE_COMPARE(track.values().size(), 3);
        CORRADE_COMPARE(track.interpolation(), Animation::Interpolation::Constant);
        CORRADE_COMPARE(track.at(2.5f), (Vector3{3.0f, 1.0f, 0.1f}));

        Animation::TrackView<Float, Vector3> mutableTrack = data.mutableTrack<Vector3>(0);
        CORRADE_COMPARE(mutableTrack.keys().size(), 3);
        CORRADE_COMPARE(mutableTrack.values().size(), 3);
        CORRADE_COMPARE(mutableTrack.interpolation(), Animation::Interpolation::Constant);
        CORRADE_COMPARE(mutableTrack.at(2.5f), (Vector3{3.0f, 1.0f, 0.1f}));
    } {
        CORRADE_COMPARE(data.trackType(1), AnimationTrackType::Quaternion);
        CORRADE_COMPARE(data.trackResultType(1), AnimationTrackType::Quaternion);
        CORRADE_COMPARE(data.trackTargetName(1), AnimationTrackTarget::Rotation3D);
        CORRADE_COMPARE(data.trackTarget(1), 1337);

        Animation::TrackView<const Float, const Quaternion> track = data.track<Quaternion>(1);
        CORRADE_COMPARE(track.keys().size(), 3);
        CORRADE_COMPARE(track.values().size(), 3);
        CORRADE_COMPARE(track.interpolation(), Animation::Interpolation::Linear);
        CORRADE_COMPARE(track.at(2.5f), Quaternion::rotation(32.5_degf, Vector3::yAxis()));

        /* Testing the mutable track just once is enough */
    }
}

void AnimationDataTest::constructImplicitDuration() {
    struct Data {
        Float time;
        bool value;
    };
    Containers::Array<char> buffer{sizeof(Data)*4};
    auto view = Containers::arrayCast<Data>(buffer);
    view[0] = {1.0f, true};
    view[1] = {5.0f, false};
    view[2] = {3.0f, true};
    view[3] = {7.0f, false};

    const int state = 5;
    AnimationData data{Utility::move(buffer), {
        AnimationTrackData{animationTrackTargetCustom(1), 0,
            Animation::TrackView<const Float, const bool>{
                {view, &view[0].time, 2, sizeof(Data)},
                {view, &view[0].value, 2, sizeof(Data)},
                Animation::Interpolation::Constant}},
        AnimationTrackData{animationTrackTargetCustom(2), 1,
            Animation::TrackView<const Float, const bool>{
                {view, &view[2].time, 2, sizeof(Data)},
                {view, &view[2].value, 2, sizeof(Data)},
                Animation::Interpolation::Linear}}
        }, &state};

    CORRADE_COMPARE(data.dataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_COMPARE(data.duration(), (Range1D{1.0f, 7.0f}));
    CORRADE_COMPARE(data.trackCount(), 2);
    CORRADE_COMPARE(data.importerState(), &state);
    {
        CORRADE_COMPARE(data.trackType(0), AnimationTrackType::Bool);
        CORRADE_COMPARE(data.trackResultType(0), AnimationTrackType::Bool);
        CORRADE_COMPARE(data.trackTargetName(0), animationTrackTargetCustom(1));
        CORRADE_COMPARE(data.trackTarget(0), 0);

        Animation::TrackView<const Float, const bool> track = data.track<bool>(0);
        CORRADE_COMPARE(track.duration(), (Range1D{1.0f, 5.0f}));
        CORRADE_COMPARE(track.keys().size(), 2);
        CORRADE_COMPARE(track.values().size(), 2);
        CORRADE_COMPARE(track.interpolation(), Animation::Interpolation::Constant);
        CORRADE_COMPARE(track.at(6.0f), false);

        Animation::TrackView<Float, bool> mutableTrack = data.mutableTrack<bool>(0);
        CORRADE_COMPARE(mutableTrack.duration(), (Range1D{1.0f, 5.0f}));
        CORRADE_COMPARE(mutableTrack.keys().size(), 2);
        CORRADE_COMPARE(mutableTrack.values().size(), 2);
        CORRADE_COMPARE(mutableTrack.interpolation(), Animation::Interpolation::Constant);
        CORRADE_COMPARE(mutableTrack.at(6.0f), false);
    } {
        CORRADE_COMPARE(data.trackType(1), AnimationTrackType::Bool);
        CORRADE_COMPARE(data.trackResultType(1), AnimationTrackType::Bool);
        CORRADE_COMPARE(data.trackTargetName(1), animationTrackTargetCustom(2));
        CORRADE_COMPARE(data.trackTarget(1), 1);

        Animation::TrackView<const Float, const bool> track = data.track<bool>(1);
        CORRADE_COMPARE(track.duration(), (Range1D{3.0f, 7.0f}));
        CORRADE_COMPARE(track.keys().size(), 2);
        CORRADE_COMPARE(track.values().size(), 2);
        CORRADE_COMPARE(track.interpolation(), Animation::Interpolation::Linear);
        CORRADE_COMPARE(track.at(4.5f), true);

        /* Testing the mutable track just once is enough */
    }
}

void AnimationDataTest::constructImplicitDurationEmpty() {
    AnimationData data{nullptr, nullptr};
    CORRADE_COMPARE(data.duration(), Range1D{});
}

void AnimationDataTest::constructNotOwned() {
    auto&& instanceData = NotOwnedData[testCaseInstanceId()];
    setTestCaseDescription(instanceData.name);

    std::pair<Float, Vector3> keyframes[] {
        {0.0f, {3.0f, 1.0f, 0.1f}},
        {5.0f, {0.3f, 0.6f, 1.0f}}
    };

    const int state = 5;
    AnimationData data{instanceData.dataFlags, keyframes, {
        AnimationTrackData{AnimationTrackTarget::Translation3D, 42,
            Animation::TrackView<const Float, const Vector3>{
                keyframes,
                Animation::Interpolation::Constant,
                animationInterpolatorFor<Vector3>(Animation::Interpolation::Constant)}}
        }, {-1.0f, 7.0f}, &state};

    CORRADE_COMPARE(data.dataFlags(), instanceData.dataFlags);
    CORRADE_COMPARE(data.duration(), (Range1D{-1.0f, 7.0f}));
    CORRADE_COMPARE(static_cast<const void*>(data.data().data()), keyframes);
    if(instanceData.dataFlags & DataFlag::Mutable)
        CORRADE_COMPARE(static_cast<const void*>(data.mutableData().data()), keyframes);
    CORRADE_COMPARE(data.trackCount(), 1);
    CORRADE_COMPARE(data.importerState(), &state);

    {
        CORRADE_COMPARE(data.trackType(0), AnimationTrackType::Vector3);
        CORRADE_COMPARE(data.trackResultType(0), AnimationTrackType::Vector3);
        CORRADE_COMPARE(data.trackTargetName(0), AnimationTrackTarget::Translation3D);
        CORRADE_COMPARE(data.trackTarget(0), 42);

        Animation::TrackView<const Float, const Vector3> track = data.track<Vector3>(0);
        CORRADE_COMPARE(track.keys().size(), 2);
        CORRADE_COMPARE(track.values().size(), 2);
        CORRADE_COMPARE(track.interpolation(), Animation::Interpolation::Constant);
        CORRADE_COMPARE(track.at(2.5f), (Vector3{3.0f, 1.0f, 0.1f}));

        if(instanceData.dataFlags & DataFlag::Mutable) {
            Animation::TrackView<Float, Vector3> mutableTrack = data.mutableTrack<Vector3>(0);
            CORRADE_COMPARE(mutableTrack.keys().size(), 2);
            CORRADE_COMPARE(mutableTrack.values().size(), 2);
            CORRADE_COMPARE(mutableTrack.interpolation(), Animation::Interpolation::Constant);
            CORRADE_COMPARE(mutableTrack.at(2.5f), (Vector3{3.0f, 1.0f, 0.1f}));
        }
    }
}

void AnimationDataTest::constructImplicitDurationNotOwned() {
    auto&& instanceData = NotOwnedData[testCaseInstanceId()];
    setTestCaseDescription(instanceData.name);

    std::pair<Float, bool> keyframes[] {
        {1.0f, true},
        {5.0f, false}
    };

    const int state = 5;
    AnimationData data{instanceData.dataFlags, keyframes, {
        AnimationTrackData{animationTrackTargetCustom(1), 0,
            Animation::TrackView<const Float, const bool>{keyframes, Animation::Interpolation::Constant}},
        }, &state};

    CORRADE_COMPARE(data.dataFlags(), instanceData.dataFlags);
    CORRADE_COMPARE(data.duration(), (Range1D{1.0f, 5.0f}));
    CORRADE_COMPARE(static_cast<const void*>(data.data().data()), keyframes);
    if(instanceData.dataFlags & DataFlag::Mutable)
        CORRADE_COMPARE(static_cast<const void*>(data.mutableData().data()), keyframes);
    CORRADE_COMPARE(data.trackCount(), 1);
    CORRADE_COMPARE(data.importerState(), &state);

    {
        CORRADE_COMPARE(data.trackType(0), AnimationTrackType::Bool);
        CORRADE_COMPARE(data.trackResultType(0), AnimationTrackType::Bool);
        CORRADE_COMPARE(data.trackTargetName(0), animationTrackTargetCustom(1));
        CORRADE_COMPARE(data.trackTarget(0), 0);

        Animation::TrackView<const Float, const bool> track = data.track<bool>(0);
        CORRADE_COMPARE(track.duration(), (Range1D{1.0f, 5.0f}));
        CORRADE_COMPARE(track.keys().size(), 2);
        CORRADE_COMPARE(track.values().size(), 2);
        CORRADE_COMPARE(track.interpolation(), Animation::Interpolation::Constant);
        CORRADE_COMPARE(track.at(3.0f), true);

        if(instanceData.dataFlags & DataFlag::Mutable) {
            Animation::TrackView<Float, bool> mutableTrack = data.mutableTrack<bool>(0);
            CORRADE_COMPARE(mutableTrack.duration(), (Range1D{1.0f, 5.0f}));
            CORRADE_COMPARE(mutableTrack.keys().size(), 2);
            CORRADE_COMPARE(mutableTrack.values().size(), 2);
            CORRADE_COMPARE(mutableTrack.interpolation(), Animation::Interpolation::Constant);
            CORRADE_COMPARE(mutableTrack.at(3.0f), true);
        }
    }
}

void AnimationDataTest::constructNotOwnedFlagOwned() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    AnimationData data{DataFlag::Owned, nullptr, {}, {-1.0f, 7.0f}};
    CORRADE_COMPARE(out,
        "Trade::AnimationData: can't construct a non-owned instance with Trade::DataFlag::Owned\n");
}

void AnimationDataTest::constructImplicitDurationNotOwnedFlagOwned() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    AnimationData data{DataFlag::Owned, nullptr, {}};
    CORRADE_COMPARE(out,
        "Trade::AnimationData: can't construct a non-owned instance with Trade::DataFlag::Owned\n");
}

void AnimationDataTest::constructCopy() {
    CORRADE_VERIFY(!std::is_copy_constructible<AnimationData>{});
    CORRADE_VERIFY(!std::is_copy_assignable<AnimationData>{});
}

void AnimationDataTest::constructMove() {
    /* Ugh and here we go again! */
    struct Data {
        Float time;
        Vector3 position;
        Quaternion rotation;
    };
    Containers::Array<char> buffer{sizeof(Data)*3};
    auto view = Containers::arrayCast<Data>(buffer);
    view[0] = {0.0f, {3.0f, 1.0f, 0.1f}, Quaternion::rotation(45.0_degf, Vector3::yAxis())};
    view[1] = {5.0f, {0.3f, 0.6f, 1.0f}, Quaternion::rotation(20.0_degf, Vector3::yAxis())};
    view[2] = {7.5f, {1.0f, 0.3f, 2.1f}, Quaternion{}};

    const int state = 5;
    AnimationData a{Utility::move(buffer), {
        AnimationTrackData{AnimationTrackTarget::Translation3D, 42,
            Animation::TrackView<const Float, const Vector3>{
                {view, &view[0].time, view.size(), sizeof(Data)},
                {view, &view[0].position, view.size(), sizeof(Data)},
                Animation::Interpolation::Constant,
                animationInterpolatorFor<Vector3>(Animation::Interpolation::Constant)}},
        AnimationTrackData{AnimationTrackTarget::Rotation3D, 1337,
            Animation::TrackView<const Float, const Quaternion>{
                {view, &view[0].time, view.size(), sizeof(Data)},
                {view, &view[0].rotation, view.size(), sizeof(Data)},
                Animation::Interpolation::Linear,
                animationInterpolatorFor<Quaternion>(Animation::Interpolation::Linear)}}
        }, {-1.0f, 7.0f}, &state};

    AnimationData b{Utility::move(a)};

    CORRADE_COMPARE(b.duration(), (Range1D{-1.0f, 7.0f}));
    CORRADE_COMPARE(b.data().size(), sizeof(Data)*3);
    CORRADE_COMPARE(b.trackCount(), 2);
    CORRADE_COMPARE(b.importerState(), &state);

    {
        CORRADE_COMPARE(b.trackType(0), AnimationTrackType::Vector3);
        CORRADE_COMPARE(b.trackResultType(0), AnimationTrackType::Vector3);
        CORRADE_COMPARE(b.trackTargetName(0), AnimationTrackTarget::Translation3D);
        CORRADE_COMPARE(b.trackTarget(0), 42);

        Animation::TrackView<const Float, const Vector3> track = b.track<Vector3>(0);
        CORRADE_COMPARE(track.keys().size(), 3);
        CORRADE_COMPARE(track.values().size(), 3);
        CORRADE_COMPARE(track.interpolation(), Animation::Interpolation::Constant);
        CORRADE_COMPARE(track.at(2.5f), (Vector3{3.0f, 1.0f, 0.1f}));
    } {
        CORRADE_COMPARE(b.trackType(1), AnimationTrackType::Quaternion);
        CORRADE_COMPARE(b.trackResultType(1), AnimationTrackType::Quaternion);
        CORRADE_COMPARE(b.trackTargetName(1), AnimationTrackTarget::Rotation3D);
        CORRADE_COMPARE(b.trackTarget(1), 1337);

        Animation::TrackView<const Float, const Quaternion> track = b.track<Quaternion>(1);
        CORRADE_COMPARE(track.keys().size(), 3);
        CORRADE_COMPARE(track.values().size(), 3);
        CORRADE_COMPARE(track.interpolation(), Animation::Interpolation::Linear);
        CORRADE_COMPARE(track.at(2.5f), Quaternion::rotation(32.5_degf, Vector3::yAxis()));
    }

    int other;
    AnimationData c{nullptr, nullptr, &other};
    c = Utility::move(b);

    CORRADE_COMPARE(c.duration(), (Range1D{-1.0f, 7.0f}));
    CORRADE_COMPARE(c.data().size(), sizeof(Data)*3);
    CORRADE_COMPARE(c.trackCount(), 2);
    CORRADE_COMPARE(c.importerState(), &state);

    {
        CORRADE_COMPARE(c.trackType(0), AnimationTrackType::Vector3);
        CORRADE_COMPARE(c.trackResultType(0), AnimationTrackType::Vector3);
        CORRADE_COMPARE(c.trackTargetName(0), AnimationTrackTarget::Translation3D);
        CORRADE_COMPARE(c.trackTarget(0), 42);

        Animation::TrackView<const Float, const Vector3> track = c.track<Vector3>(0);
        CORRADE_COMPARE(track.keys().size(), 3);
        CORRADE_COMPARE(track.values().size(), 3);
        CORRADE_COMPARE(track.interpolation(), Animation::Interpolation::Constant);
        CORRADE_COMPARE(track.at(2.5f), (Vector3{3.0f, 1.0f, 0.1f}));
    } {
        CORRADE_COMPARE(c.trackType(1), AnimationTrackType::Quaternion);
        CORRADE_COMPARE(c.trackResultType(1), AnimationTrackType::Quaternion);
        CORRADE_COMPARE(c.trackTargetName(1), AnimationTrackTarget::Rotation3D);
        CORRADE_COMPARE(c.trackTarget(1), 1337);

        Animation::TrackView<const Float, const Quaternion> track = c.track<Quaternion>(1);
        CORRADE_COMPARE(track.keys().size(), 3);
        CORRADE_COMPARE(track.values().size(), 3);
        CORRADE_COMPARE(track.interpolation(), Animation::Interpolation::Linear);
        CORRADE_COMPARE(track.at(2.5f), Quaternion::rotation(32.5_degf, Vector3::yAxis()));
    }

    CORRADE_VERIFY(std::is_nothrow_move_constructible<AnimationData>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<AnimationData>::value);
}

void AnimationDataTest::mutableAccessNotAllowed() {
    CORRADE_SKIP_IF_NO_ASSERT();

    const std::pair<Float, bool> keyframes[] {
        {1.0f, true},
        {5.0f, false}
    };

    AnimationData data{{}, keyframes, {
        AnimationTrackData{animationTrackTargetCustom(1), 0,
            Animation::TrackView<const Float, const bool>{keyframes, Animation::Interpolation::Constant}},
        }};
    CORRADE_COMPARE(data.dataFlags(), DataFlags{});

    Containers::String out;
    Error redirectError{&out};
    data.mutableData();
    data.mutableTrack(0);
    data.mutableTrack<bool>(0);
    CORRADE_COMPARE(out,
        "Trade::AnimationData::mutableData(): the animation is not mutable\n"
        "Trade::AnimationData::mutableTrack(): the animation is not mutable\n"
        "Trade::AnimationData::mutableTrack(): the animation is not mutable\n");
}

void AnimationDataTest::trackCustomResultType() {
    using namespace Math::Literals;

    struct Data {
        Float time;
        Vector3i position;
    };
    Containers::Array<char> buffer{sizeof(Data)*3};
    auto view = Containers::arrayCast<Data>(buffer);
    view[0] = {0.0f, {300, 100, 10}};
    view[1] = {5.0f, {30, 60, 100}};

    AnimationData data{Utility::move(buffer), {
        AnimationTrackData{AnimationTrackTarget::Scaling3D, 0,
            Animation::TrackView<const Float, const Vector3i, Vector3>{
                {view, &view[0].time, view.size(), sizeof(Data)},
                {view, &view[0].position, view.size(), sizeof(Data)},
                [](const Vector3i& a, const Vector3i& b, Float t) -> Vector3 {
                    return Math::lerp(Vector3{a}*0.01f, Vector3{b}*0.01f, t);
                }}}
        }};

    CORRADE_COMPARE((data.track<Vector3i, Vector3>(0).at(2.5f)), (Vector3{1.65f, 0.8f, 0.55f}));
}

void AnimationDataTest::trackWrongIndex() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};

    AnimationData data{nullptr, {
        AnimationTrackData{AnimationTrackTarget::Scaling3D, 0, AnimationTrackType::Vector3, nullptr, nullptr, Animation::Interpolation::Constant}
    }};
    data.trackType(1);
    data.trackResultType(1);
    data.trackTargetName(1);
    data.trackTarget(1);
    data.track(1);
    data.mutableTrack(1);

    CORRADE_COMPARE(out,
        "Trade::AnimationData::trackType(): index 1 out of range for 1 tracks\n"
        "Trade::AnimationData::trackResultType(): index 1 out of range for 1 tracks\n"
        "Trade::AnimationData::trackTargetName(): index 1 out of range for 1 tracks\n"
        "Trade::AnimationData::trackTarget(): index 1 out of range for 1 tracks\n"
        "Trade::AnimationData::track(): index 1 out of range for 1 tracks\n"
        "Trade::AnimationData::mutableTrack(): index 1 out of range for 1 tracks\n");
}

void AnimationDataTest::trackWrongType() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};

    AnimationData data{nullptr, {
        AnimationTrackData{AnimationTrackTarget::Scaling3D, 0, AnimationTrackType::Vector3i, AnimationTrackType::Vector3, nullptr, nullptr, Animation::Interpolation::Constant, nullptr}
    }};

    data.track<Vector3>(0);

    CORRADE_COMPARE(out, "Trade::AnimationData::track(): improper type requested for Trade::AnimationTrackType::Vector3i\n");
}

void AnimationDataTest::trackWrongResultType() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};

    AnimationData data{nullptr, {
        AnimationTrackData{AnimationTrackTarget::Scaling3D, 0, AnimationTrackType::Vector3i, AnimationTrackType::Vector3, nullptr, nullptr, Animation::Interpolation::Constant, nullptr}
    }};

    data.track<Vector3i, Vector2>(0);

    CORRADE_COMPARE(out, "Trade::AnimationData::track(): improper result type requested for Trade::AnimationTrackType::Vector3\n");
}

void AnimationDataTest::release() {
    const std::pair<Float, bool> keyframes[] {
        {1.0f, true},
        {5.0f, false}
    };

    AnimationData data{{}, keyframes, {
        AnimationTrackData{animationTrackTargetCustom(1), 0,
            Animation::TrackView<const Float, const bool>{keyframes, Animation::Interpolation::Constant}},
        }};
    CORRADE_COMPARE(data.trackCount(), 1);

    Containers::Array<char> released = data.release();
    CORRADE_COMPARE(data.data(), static_cast<const void*>(nullptr));
    CORRADE_COMPARE(data.trackCount(), 0);
    CORRADE_COMPARE(static_cast<const void*>(released.data()), keyframes);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::AnimationDataTest)
