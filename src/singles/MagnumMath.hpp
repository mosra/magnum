/*
    Magnum::Math
        — a graphics-focused vector math library

    https://doc.magnum.graphics/magnum/namespaceMagnum_1_1Math.html
    https://doc.magnum.graphics/magnum/namespaceMagnum_1_1EigenIntegration.html
    https://doc.magnum.graphics/magnum/namespaceMagnum_1_1GlmIntegration.html

    This is a single-header library generated from the Magnum project. With the
    goal being easy integration, it's deliberately free of all comments to keep
    the file size small. More info, changelogs and full docs here:

    -   Project homepage — https://magnum.graphics/magnum/
    -   Documentation — https://doc.magnum.graphics/
    -   GitHub project page — https://github.com/mosra/magnum
    -   GitHub Singles repository — https://github.com/mosra/magnum-singles

    Generated from Corrade {{revision:corrade/src}},
        Magnum {{revision:magnum/src}} and
        Magnum Integration {{revision:magnum-integration/src}}, {{stats:loc}} / {{stats:preprocessed}} LoC
*/

#include "base.h"
// {{includes}}
#if (!defined(CORRADE_ASSERT) || !defined(CORRADE_CONSTEXPR_ASSERT) || !defined(CORRADE_INTERNAL_ASSERT_OUTPUT) || !defined(CORRADE_ASSERT_UNREACHABLE)) && !defined(NDEBUG)
#include <cassert>
#endif

/* We're taking stuff from integration as well */
#pragma ACME path ../../../magnum-integration/src
#pragma ACME revision magnum-integration/src echo "$(git describe --match 'v*') ($(date -d @$(git log -1 --format=%at) +%Y-%m-%d))"

/* Disable asserts that are not used. CORRADE_ASSERT, CORRADE_CONSTEXPR_ASSERT,
   CORRADE_INTERNAL_ASSERT_OUTPUT and CORRADE_ASSERT_UNREACHABLE are used,
   wrapping the #include <cassert> above. When enabling additional asserts, be
   sure to update them above as well. */
#pragma ACME enable CORRADE_ASSERT_OUTPUT
#pragma ACME enable CORRADE_INTERNAL_ASSERT
#pragma ACME enable CORRADE_INTERNAL_CONSTEXPR_ASSERT

/* We don't need anything from configure.h here */
#pragma ACME enable Corrade_configure_h
#pragma ACME enable Magnum_configure_h
#if defined(_MSC_VER) && _MSC_VER <= 1920
#define CORRADE_MSVC2017_COMPATIBILITY
#endif
#if defined(_MSC_VER) && _MSC_VER <= 1910
#define CORRADE_MSVC2015_COMPATIBILITY
#endif
#ifdef _WIN32
#define CORRADE_TARGET_WINDOWS
#endif
#ifdef __EMSCRIPTEN__
#define CORRADE_TARGET_EMSCRIPTEN
#endif
#ifdef __ANDROID__
#define CORRADE_TARGET_ANDROID
#endif

/* Our own subset of visibility macros */
#pragma ACME enable Magnum_visibility_h
#pragma ACME enable Corrade_Utility_VisibilityMacros_h
#ifndef MAGNUM_EXPORT
#define MAGNUM_EXPORT
#endif

/* Our own subset of Containers.h and Magnum.h */
#pragma ACME enable Corrade_Containers_Containers_h
#pragma ACME enable Magnum_Magnum_h
#include "Magnum/Math/Math.h"
#ifndef MagnumMath_hpp
#define MagnumMath_hpp

namespace Magnum {

typedef Math::Half Half;
typedef Math::Vector2<Float> Vector2;
typedef Math::Vector3<Float> Vector3;
typedef Math::Vector4<Float> Vector4;
typedef Math::Vector2<UnsignedInt> Vector2ui;
typedef Math::Vector3<UnsignedInt> Vector3ui;
typedef Math::Vector4<UnsignedInt> Vector4ui;
typedef Math::Vector2<Int> Vector2i;
typedef Math::Vector3<Int> Vector3i;
typedef Math::Vector4<Int> Vector4i;
typedef Math::Color3<Float> Color3;
typedef Math::Color4<Float> Color4;
typedef Math::Color3<UnsignedByte> Color3ub;
typedef Math::Color4<UnsignedByte> Color4ub;
typedef Math::Matrix3<Float> Matrix3;
typedef Math::Matrix4<Float> Matrix4;
typedef Math::Matrix2x2<Float> Matrix2x2;
typedef Math::Matrix3x3<Float> Matrix3x3;
typedef Math::Matrix4x4<Float> Matrix4x4;
typedef Math::Matrix2x3<Float> Matrix2x3;
typedef Math::Matrix3x2<Float> Matrix3x2;
typedef Math::Matrix2x4<Float> Matrix2x4;
typedef Math::Matrix4x2<Float> Matrix4x2;
typedef Math::Matrix3x4<Float> Matrix3x4;
typedef Math::Matrix4x3<Float> Matrix4x3;
typedef Math::QuadraticBezier2D<Float> QuadraticBezier2D;
typedef Math::QuadraticBezier3D<Float> QuadraticBezier3D;
typedef Math::CubicBezier2D<Float> CubicBezier2D;
typedef Math::CubicBezier3D<Float> CubicBezier3D;
typedef Math::CubicHermite1D<Float> CubicHermite1D;
typedef Math::CubicHermite2D<Float> CubicHermite2D;
typedef Math::CubicHermite3D<Float> CubicHermite3D;
typedef Math::CubicHermiteComplex<Float> CubicHermiteComplex;
typedef Math::CubicHermiteQuaternion<Float> CubicHermiteQuaternion;
typedef Math::Complex<Float> Complex;
typedef Math::DualComplex<Float> DualComplex;
typedef Math::Quaternion<Float> Quaternion;
typedef Math::DualQuaternion<Float> DualQuaternion;
typedef Math::Constants<Float> Constants;
typedef Math::Deg<Float> Deg;
typedef Math::Rad<Float> Rad;
typedef Math::Range1D<Float> Range1D;
typedef Math::Range2D<Float> Range2D;
typedef Math::Range3D<Float> Range3D;
typedef Math::Range1D<Int> Range1Di;
typedef Math::Range2D<Int> Range2Di;
typedef Math::Range3D<Int> Range3Di;
typedef Math::Frustum<Float> Frustum;
typedef Math::Vector2<Double> Vector2d;
typedef Math::Vector3<Double> Vector3d;
typedef Math::Vector4<Double> Vector4d;
typedef Math::Matrix3<Double> Matrix3d;
typedef Math::Matrix4<Double> Matrix4d;
typedef Math::Matrix2x2<Double> Matrix2x2d;
typedef Math::Matrix3x3<Double> Matrix3x3d;
typedef Math::Matrix4x4<Double> Matrix4x4d;
typedef Math::Matrix2x3<Double> Matrix2x3d;
typedef Math::Matrix3x2<Double> Matrix3x2d;
typedef Math::Matrix2x4<Double> Matrix2x4d;
typedef Math::Matrix4x2<Double> Matrix4x2d;
typedef Math::Matrix3x4<Double> Matrix3x4d;
typedef Math::Matrix4x3<Double> Matrix4x3d;
typedef Math::QuadraticBezier2D<Float> QuadraticBezier2Dd;
typedef Math::QuadraticBezier3D<Float> QuadraticBezier3Dd;
typedef Math::CubicBezier2D<Float> CubicBezier2Dd;
typedef Math::CubicBezier3D<Float> CubicBezier3Dd;
typedef Math::CubicHermite1D<Double> CubicHermite1Dd;
typedef Math::CubicHermite2D<Double> CubicHermite2Dd;
typedef Math::CubicHermite3D<Double> CubicHermite3Dd;
typedef Math::CubicHermiteComplex<Double> CubicHermiteComplexd;
typedef Math::CubicHermiteQuaternion<Double> CubicHermiteQuaterniond;
typedef Math::Complex<Double> Complexd;
typedef Math::DualComplex<Double> DualComplexd;
typedef Math::Quaternion<Double> Quaterniond;
typedef Math::DualQuaternion<Double> DualQuaterniond;
typedef Math::Constants<Double> Constantsd;
typedef Math::Deg<Double> Degd;
typedef Math::Rad<Double> Radd;
typedef Math::Range1D<Double> Range1Dd;
typedef Math::Range2D<Double> Range2Dd;
typedef Math::Range3D<Double> Range3Dd;
typedef Math::Frustum<Double> Frustumd;

}

#endif
#include "Magnum/Magnum.h"
#include "Magnum/Math/Angle.h"
#include "Magnum/Math/Bezier.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Complex.h"
#include "Magnum/Math/Constants.h"
#include "Magnum/Math/CubicHermite.h"
#include "Magnum/Math/Distance.h"
#include "Magnum/Math/Dual.h"
#include "Magnum/Math/DualComplex.h"
#include "Magnum/Math/DualQuaternion.h"
#include "Magnum/Math/Frustum.h"
#include "Magnum/Math/Functions.h"
#include "Magnum/Math/Half.h"
#include "Magnum/Math/Intersection.h"
#include "Magnum/Math/Matrix.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Matrix4.h"
#include "Magnum/Math/Packing.h"
#include "Magnum/Math/Quaternion.h"
#include "Magnum/Math/Range.h"
#include "Magnum/Math/RectangularMatrix.h"
#include "Magnum/Math/StrictWeakOrdering.h"
#include "Magnum/Math/Swizzle.h"
#include "Magnum/Math/Tags.h"
#include "Magnum/Math/TypeTraits.h"
#include "Magnum/Math/Unit.h"
#include "Magnum/Math/Vector.h"
#include "Magnum/Math/Vector2.h"
#include "Magnum/Math/Vector3.h"
#include "Magnum/Math/Vector4.h"
#include "Magnum/Math/Algorithms/GaussJordan.h"
#include "Magnum/Math/Algorithms/GramSchmidt.h"
#include "Magnum/Math/Algorithms/KahanSum.h"
#include "Magnum/Math/Algorithms/Qr.h"
//#include "Magnum/Math/Algorithms/Svd.h" // TODO: uses <tuple>
#ifdef MAGNUM_MATH_GLM_INTEGRATION
// {{includes}}
#include "Magnum/GlmIntegration/Integration.h"
#include "Magnum/GlmIntegration/GtcIntegration.h"
#include "Magnum/GlmIntegration/GtxIntegration.h"
#endif
#ifdef MAGNUM_MATH_EIGEN_INTEGRATION
// {{includes}}
#include "Magnum/EigenIntegration/Integration.h"
#include "Magnum/EigenIntegration/GeometryIntegration.h"
#endif
#ifdef MAGNUM_MATH_IMPLEMENTATION
// {{ includes }}
#include "Magnum/Math/Functions.cpp"
#include "Magnum/Math/Packing.cpp"
#endif
