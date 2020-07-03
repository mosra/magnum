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

    v2020.06-0-gfac6f4da2 (2020-06-27)
    -   Various fixes for Clang-CL compatibility
    -   Expanding the APIs to work with Half and long double types
    -   Magnum::Math::NoInit is now Magnum::NoInit
    -   Minor changes for faster performance of dot() and cross() in Debug
    -   Added reflect() and refract() functions
    -   slerp() / slerpShortestPath() falls back to linear interpolation for
        quaternions that are close together, instead or always returning the
        first
    -   Added Quaternion::toEuler()
    -   Added transformVector() to DualComplex and DualQuaternion to have the
        the same set of APIs as with Matrix3 / Matrix4
    -   Mutable access to Frustum planes
    -   Fixed implicit conversion of std::pair to Range*D
    -   New BoolVector[234], 8-/16-bit and half-float vector and matrix
        convenience typedefs
    v2019.10-0-g8412e8f99 (2019-10-24)
    -   New IsScalar, IsVector, IsIntegral, IsFloatingPoint type traits,
        correct handling of Deg and Rad types in all APIs
    -   Guaranteed NaN handling semantic in min()/max()/minmax() APIs
    -   Using a GCC compiler builtin in sincos()
    -   swizzle() is replaced with gather() and scatter()
    -   Added Matrix::{cofactor,comatrix,adjugate}(), Matrix4::normalMatrix()
    -   New Matrix4::perspectiveProjection() overload taking corner positions
    -   Handling also Eigen::Ref types; EigenIntegration::eigenCast() is now
        just EigenIntegration::cast()
    v2019.01-241-g93686746a (2019-04-03)
    -   Initial release

    Generated from Corrade {{revision:corrade/src}},
        Magnum {{revision:magnum/src}} and
        Magnum Integration {{revision:magnum-integration/src}}, {{stats:loc}} / {{stats:preprocessed}} LoC
*/

#include "base.h"
/* Put the contents of Corrade/Utility/StlMath.h here so it doesn't get
   reordered */
#pragma ACME enable Corrade_Utility_StlMath_h
// {{includes}}
#include <ciso646>
#ifdef _GLIBCXX_USE_STD_SPEC_FUNCS
#undef _GLIBCXX_USE_STD_SPEC_FUNCS
#define _GLIBCXX_USE_STD_SPEC_FUNCS 0
#endif
// {{includes}}
#include <cmath>
#if (!defined(CORRADE_ASSERT) || !defined(CORRADE_CONSTEXPR_ASSERT) || !defined(CORRADE_INTERNAL_ASSERT_OUTPUT) || !defined(CORRADE_INTERNAL_ASSERT_UNREACHABLE)) && !defined(NDEBUG)
#include <cassert>
#endif

/* Combined copyrights because the rool isn't able to merge those on its own:

    Copyright © 2016, 2018, 2020 Jonathan Hale <squareys@googlemail.com>

*/

/* We're taking stuff from integration as well */
#pragma ACME path ../../../magnum-integration/src
#pragma ACME revision magnum-integration/src echo "$(git describe --long --match 'v*') ($(date -d @$(git log -1 --format=%at) +%Y-%m-%d))"

/* Disable asserts that are not used. CORRADE_ASSERT, CORRADE_CONSTEXPR_ASSERT,
   CORRADE_INTERNAL_ASSERT_OUTPUT and CORRADE_INTERNAL_ASSERT_UNREACHABLE are
   used, wrapping the #include <cassert> above. When enabling additional
   asserts, be sure to update them above as well. */
#pragma ACME enable CORRADE_ASSUME
#pragma ACME enable CORRADE_ASSERT_OUTPUT
#pragma ACME enable CORRADE_ASSERT_UNREACHABLE
#pragma ACME enable CORRADE_INTERNAL_ASSERT
#pragma ACME enable CORRADE_INTERNAL_CONSTEXPR_ASSERT

/* We don't need everything from configure.h here */
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

/* CORRADE_TARGET_LIBSTDCXX needed for CORRADE_STD_IS_TRIVIALLY_TRAITS_SUPPORTED,
   and because it's so complex to check for it I can as well pull in the whole
   thing */
#include <ciso646>
#ifdef _LIBCPP_VERSION
#define CORRADE_TARGET_LIBCXX
#elif defined(_CPPLIB_VER)
#define CORRADE_TARGET_DINKUMWARE
#elif defined(__GLIBCXX__)
#define CORRADE_TARGET_LIBSTDCXX
/* GCC's <ciso646> provides the __GLIBCXX__ macro only since 6.1, so on older
   versions we'll try to get it from bits/c++config.h */
#elif defined(__has_include)
    #if __has_include(<bits/c++config.h>)
        #include <bits/c++config.h>
        #ifdef __GLIBCXX__
        #define CORRADE_TARGET_LIBSTDCXX
        #endif
    #endif
/* GCC < 5.0 doesn't have __has_include, so on these versions we'll just assume
   it's libstdc++ as I don't think those versions are used with anything else
   nowadays anyway. Clang reports itself as GCC 4.4, so exclude that one. */
#elif defined(__GNUC__) && !defined(__clang__) && __GNUC__ < 5
#define CORRADE_TARGET_LIBSTDCXX
#else
/* Otherwise no idea. */
#endif

#ifdef _MSC_VER
#define CORRADE_TARGET_MSVC
#endif
#if defined(__clang__) && defined(_MSC_VER)
#define CORRADE_TARGET_CLANG_CL
#endif
/* Needed by TypeTraits::equal() */
#if defined(CORRADE_TARGET_MSVC) || (defined(CORRADE_TARGET_ANDROID) && !__LP64__) || (defined(CORRADE_TARGET_EMSCRIPTEN) && __LDBL_DIG__ == __DBL_DIG__)
#define CORRADE_LONG_DOUBLE_SAME_AS_DOUBLE
#endif
/* CORRADE_TARGET_SSE2 or CORRADE_TARGET_BIG_ENDIAN not used anywhere yet */

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
typedef Math::BoolVector<2> BoolVector2;
typedef Math::BoolVector<3> BoolVector3;
typedef Math::BoolVector<4> BoolVector4;
typedef Math::Vector2<Float> Vector2;
typedef Math::Vector3<Float> Vector3;
typedef Math::Vector4<Float> Vector4;
typedef Math::Vector2<UnsignedByte> Vector2ub;
typedef Math::Vector3<UnsignedByte> Vector3ub;
typedef Math::Vector4<UnsignedByte> Vector4ub;
typedef Math::Vector2<Byte> Vector2b;
typedef Math::Vector3<Byte> Vector3b;
typedef Math::Vector4<Byte> Vector4b;
typedef Math::Vector2<UnsignedShort> Vector2us;
typedef Math::Vector3<UnsignedShort> Vector3us;
typedef Math::Vector4<UnsignedShort> Vector4us;
typedef Math::Vector2<Short> Vector2s;
typedef Math::Vector3<Short> Vector3s;
typedef Math::Vector4<Short> Vector4s;
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
typedef Math::Color3<UnsignedShort> Color3us;
typedef Math::Color4<UnsignedShort> Color4us;
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
typedef Math::Matrix2x2<Byte> Matrix2x2b;
typedef Math::Matrix2x3<Byte> Matrix2x3b;
typedef Math::Matrix2x4<Byte> Matrix2x4b;
typedef Math::Matrix3x2<Byte> Matrix3x2b;
typedef Math::Matrix3x3<Byte> Matrix3x3b;
typedef Math::Matrix3x4<Byte> Matrix3x4b;
typedef Math::Matrix4x2<Byte> Matrix4x2b;
typedef Math::Matrix4x3<Byte> Matrix4x3b;
typedef Math::Matrix4x4<Byte> Matrix4x4b;
typedef Math::Matrix2x2<Short> Matrix2x2s;
typedef Math::Matrix2x3<Short> Matrix2x3s;
typedef Math::Matrix2x4<Short> Matrix2x4s;
typedef Math::Matrix3x2<Short> Matrix3x2s;
typedef Math::Matrix3x3<Short> Matrix3x3s;
typedef Math::Matrix3x4<Short> Matrix3x4s;
typedef Math::Matrix4x2<Short> Matrix4x2s;
typedef Math::Matrix4x3<Short> Matrix4x3s;
typedef Math::Matrix4x4<Short> Matrix4x4s;
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
typedef Math::Vector2<Half> Vector2h;
typedef Math::Vector3<Half> Vector3h;
typedef Math::Vector4<Half> Vector4h;
typedef Math::Color3<Half> Color3h;
typedef Math::Color4<Half> Color4h;
typedef Math::Matrix2x2<Half> Matrix2x2h;
typedef Math::Matrix2x3<Half> Matrix2x3h;
typedef Math::Matrix2x4<Half> Matrix2x4h;
typedef Math::Matrix3x2<Half> Matrix3x2h;
typedef Math::Matrix3x3<Half> Matrix3x3h;
typedef Math::Matrix3x4<Half> Matrix3x4h;
typedef Math::Matrix4x2<Half> Matrix4x2h;
typedef Math::Matrix4x3<Half> Matrix4x3h;
typedef Math::Matrix4x4<Half> Matrix4x4h;
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
// TODO: FunctionsBatch (separate library because of StridedArrayView dep?)
#include "Magnum/Math/Half.h"
#include "Magnum/Math/Intersection.h"
#include "Magnum/Math/Matrix.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Matrix4.h"
#include "Magnum/Math/Packing.h"
// TODO: PackingBatch (separate library because of StridedArrayView dep?)
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
