/*
    Magnum::Math
        — a graphics-focused vector math library

    https://doc.magnum.graphics/magnum/namespaceMagnum_1_1Math.html
    https://doc.magnum.graphics/magnum/namespaceMagnum_1_1EigenIntegration.html
    https://doc.magnum.graphics/magnum/namespaceMagnum_1_1GlmIntegration.html

    Depends on CorradePair.h.

    This is a single-header library generated from the Magnum project. With the
    goal being easy integration, it's deliberately free of all comments to keep
    the file size small. More info, changelogs and full docs here:

    -   Project homepage — https://magnum.graphics/magnum/
    -   Documentation — https://doc.magnum.graphics/
    -   GitHub project page — https://github.com/mosra/magnum
    -   GitHub Singles repository — https://github.com/mosra/magnum-singles

    The library has a separate non-inline implementation part, enable it *just
    once* like this:

        #define MAGNUM_MATH_IMPLEMENTATION
        #include <MagnumMath.hpp>

    If you need the deinlined symbols to be exported from a shared library,
    `#define MAGNUM_EXPORT` as appropriate. The STL compatibility bits for time
    types are included as well --- opt-in with
    `#define MAGNUM_MATH_STL_COMPATIBILITY` before including the file. Contents
    of the GlmIntegration and EigenIntegration libraries can be opted in by
    specifying either `#define MAGNUM_MATH_GLM_INTEGRATION` or
    `#define MAGNUM_MATH_EIGEN_INTEGRATION` before including the file.
    Including it multiple times with different macros defined works as well.

    v2020.06-3125-g632a2 (2025-01-07)
    -   New Nanoseconds and Seconds types along with _nsec, _usec, _msec and
        _sec literals and opt-in std::chrono compatibility
    -   Literals are now in inline AngleLiterals, ColorLiterals, HalfLiterals
        and TimeLiterals subnamespaces to allow for more fine-grained inclusion
    -   Most const Vector APIs are now constexpr as well
    -   The _rgbf and _rgbaf literals are now constexpr
    -   Vector and [Rectangular]Matrix instances can are now constexpr
        constructible directly from plain C arrays of matching sizes
    -   Added Quaternion::rotation() from two vectors
    -   Added Quaternion::xyzw() and wxyz() for conversion to a Vector4
    -   Bezier APIs now use the leaf Vector2 / Vector3 types instead of the
        base Vector type
    -   New Matrix2x1, Matrix3x1, Matrix4x1, Matrix2x1d, Matrix3x1d and
        Matrix4x1d typedefs for single-row matrices
    -   Removed dependency on <utility> in favor of CorradePair.h, which
        reduces the preprocessed size by about 500 lines, and enables constexpr
        usage in various new places
    -   Updated Eigen integration to work with MSVC 2022 17.10+
    v2020.06-2544-g3e435 (2023-09-11)
    -   Fixes to the Utility::swap() helper to avoid ambiguity with std::swap()
    v2020.06-2502-gfa079385b (2023-08-28)
    -   New Range1Dui, Range2Dui, Range3Dui, Degh, Radh, Range1Dh, Range2Dh and
        Range3Dh typedefs
    -   New binomialCoefficient(), popcount() and fmod() APIs
    -   Added r() and g() accessors to Vector2 and rg() to Vector3
    -   New Color3::fromLinearRgbInt(), toLinearRgbInt() and
        Color4::fromLinearRgbaInt(), toLinearRgbaInt() for converting a color
        from/to a packed 24-/32-bit representation without a sRGB conversion;
        integer-taking fromSrgb() and fromSrgbAlpha() is now renamed to
        fromSrgbInt() and fromSrgbAlphaInt() for consistency
    -   Added an off-center Matrix3::projection() and
        Matrix4::orthographicProjection() overloads
    -   New Matrix4::orthographicProjectionNear(), orthographicProjectionFar(),
        perspectiveProjectionNear(), perspectiveProjectionFar() accessors
    -   Added Quaternion::reflection() and reflectVector() APIs which perform
        a reflection with a quaternion instead of a rotation
    -   Ability to create a DualQuaternion from a rotation quaternion and a
        translation vector
    -   angle() for Quaternion is now called halfAngle() because that's what it
        returns, angle() will be eventually reintroduced again but returning
        the correct value
    -   Convenience Distance::pointPoint() and pointPointSquared(),
        Intersection::pointCircle() and pointSphere() APIs as a more
        self-documenting way of using (a - b).length() or dot()
    -   New Intersection::rayRange() API
    -   Conversion between Eigen::AlignedBox and Range
    -   Added unary operator+ to all math classes
    -   Matrices can now created from matrices of different sizes with a custom
        value on new diagonal elements
    -   data() accessors of all classes now return sized array references
        instead of pointers
    -   Fixed Matrix4::normalMatrix() to behave correctly in presence of a
        reflection
    -   BoolVector is renamed to BitVector and has new set() and reset() APIs
    -   64-bit integers and long doubles are no longer compiled away on
        Emscripten
    -   Fixed QuadraticBezier2Dd, QuadraticBezier3Dd, CubicBezier2Dd and
        CubicBezier3Dd typedefs to be actually doubles
    -   Compatibility with C++20 which removes the <ciso646> header
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
// {{includes}}

#if (!defined(CORRADE_ASSERT) || !defined(CORRADE_CONSTEXPR_ASSERT) || !defined(CORRADE_INTERNAL_ASSERT_OUTPUT) || !defined(CORRADE_INTERNAL_ASSERT_UNREACHABLE)) && !defined(NDEBUG)
#include <cassert>
#endif

/* Combined copyrights because the tool isn't able to merge those on its own:

    Copyright © 2016, 2018, 2020 Jonathan Hale <squareys@googlemail.com>

*/

/* We're taking stuff from integration as well */
#pragma ACME path ../../../magnum-integration/src
#pragma ACME revision magnum-integration/src echo "$(git describe --long --match 'v*' --abbrev=4) ($(date -d @$(git log -1 --format=%at) +%Y-%m-%d))"

/* CORRADE_DEBUG_ASSERT, CORRADE_CONSTEXPR_DEBUG_ASSERT,
   CORRADE_INTERNAL_DEBUG_ASSERT_OUTPUT and
   CORRADE_INTERNAL_DEBUG_ASSERT_UNREACHABLE are used, wrapping the #include
   <cassert> above. When enabling additional asserts, be sure to update them
   above as well-- without the _DEBUG variants, as they just delegate to the
   non-debug version of the macro. */
#include "singles/assert.h"
#pragma ACME forget CORRADE_ASSERT
#pragma ACME forget CORRADE_DEBUG_ASSERT
#pragma ACME forget CORRADE_CONSTEXPR_ASSERT
#pragma ACME forget CORRADE_CONSTEXPR_DEBUG_ASSERT
#pragma ACME forget CORRADE_INTERNAL_ASSERT_OUTPUT
#pragma ACME forget CORRADE_INTERNAL_DEBUG_ASSERT_OUTPUT
#pragma ACME forget CORRADE_INTERNAL_ASSERT_UNREACHABLE
#pragma ACME forget CORRADE_INTERNAL_DEBUG_ASSERT_UNREACHABLE

/* Disable strict weak ordering helpers as they're non-essential. The
   StrictWeakOrdering.h header isn't included either. */
#pragma ACME enable MAGNUM_NO_MATH_STRICT_WEAK_ORDERING

/* We don't need everything from configure.h here */
#pragma ACME enable Corrade_configure_h
#pragma ACME enable Magnum_configure_h
#if defined(_MSC_VER) && _MSC_VER < 1920
#define CORRADE_MSVC2017_COMPATIBILITY
#endif
#if defined(_MSC_VER) && _MSC_VER < 1910
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

#ifdef __GNUC__
#define CORRADE_TARGET_GCC
#endif
#ifdef __clang__
#define CORRADE_TARGET_CLANG
#endif
#if defined(__clang__) && defined(_MSC_VER)
#define CORRADE_TARGET_CLANG_CL
#endif
#ifdef _MSC_VER
#define CORRADE_TARGET_MSVC
#endif
/* Needed by TypeTraits::equal() */
#if defined(CORRADE_TARGET_MSVC) || (defined(CORRADE_TARGET_ANDROID) && !__LP64__) || defined(CORRADE_TARGET_EMSCRIPTEN) || (defined(CORRADE_TARGET_APPLE) && !defined(CORRADE_TARGET_IOS) && defined(CORRADE_TARGET_ARM))
#define CORRADE_LONG_DOUBLE_SAME_AS_DOUBLE
#endif

/* Our own subset of visibility macros */
#pragma ACME enable Magnum_visibility_h
#pragma ACME enable Corrade_Utility_VisibilityMacros_h
#ifndef MAGNUM_EXPORT
#define MAGNUM_EXPORT
#endif

/* CorradePair.h is a dependency */
#pragma ACME noexpand CorradePair.h
#pragma ACME enable Corrade_Containers_Pair_h
#include "CorradePair.h"

/* A semi-verbatim copy of Utility/StlMath.h because otherwise the includes
   don't stay in the correct place. */
#ifndef Corrade_Utility_StlMath_h
#define Corrade_Utility_StlMath_h
#ifdef _GLIBCXX_USE_STD_SPEC_FUNCS
#undef _GLIBCXX_USE_STD_SPEC_FUNCS
#define _GLIBCXX_USE_STD_SPEC_FUNCS 0
#endif
#include <cmath>
#endif
#pragma ACME enable Corrade_Utility_StlMath_h
/* Our own subset of Containers.h and Magnum.h, need just CORRADE_HAS_TYPE from
   Utility/TypeTraits.h */
#pragma ACME enable Corrade_Containers_Containers_h
#pragma ACME enable Corrade_Utility_TypeTraits_h
#pragma ACME enable Magnum_Magnum_h
#include "Magnum/Math/Math.h"
#ifndef MagnumMath_hpp
#define MagnumMath_hpp

#define CORRADE_HAS_TYPE(className, ...)                                    \
template<class U> class className {                                         \
    template<class T> static char get(T&&, __VA_ARGS__* = nullptr);         \
    static short get(...);                                                  \
    public:                                                                 \
        enum: bool { value = sizeof(get(std::declval<U>())) == sizeof(char) }; \
}

namespace Magnum {

using namespace Corrade;

typedef Math::Half Half;
typedef Math::BitVector<2> BitVector2;
typedef Math::BitVector<3> BitVector3;
typedef Math::BitVector<4> BitVector4;
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
typedef Math::Matrix2x1<Float> Matrix2x1;
typedef Math::Matrix2x2<Float> Matrix2x2;
typedef Math::Matrix2x3<Float> Matrix2x3;
typedef Math::Matrix2x4<Float> Matrix2x4;
typedef Math::Matrix3x1<Float> Matrix3x1;
typedef Math::Matrix3x2<Float> Matrix3x2;
typedef Math::Matrix3x3<Float> Matrix3x3;
typedef Math::Matrix3x4<Float> Matrix3x4;
typedef Math::Matrix4x1<Float> Matrix4x1;
typedef Math::Matrix4x2<Float> Matrix4x2;
typedef Math::Matrix4x3<Float> Matrix4x3;
typedef Math::Matrix4x4<Float> Matrix4x4;
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
typedef Math::Range1D<UnsignedInt> Range1Dui;
typedef Math::Range2D<UnsignedInt> Range2Dui;
typedef Math::Range3D<UnsignedInt> Range3Dui;
typedef Math::Range1D<Int> Range1Di;
typedef Math::Range2D<Int> Range2Di;
typedef Math::Range3D<Int> Range3Di;
typedef Math::Frustum<Float> Frustum;
typedef Math::Nanoseconds<Long> Nanoseconds;
typedef Math::Seconds<Float> Seconds;
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
typedef Math::Deg<Half> Degh;
typedef Math::Rad<Half> Radh;
typedef Math::Range1D<Half> Range1Dh;
typedef Math::Range2D<Half> Range2Dh;
typedef Math::Range3D<Half> Range3Dh;
typedef Math::Vector2<Double> Vector2d;
typedef Math::Vector3<Double> Vector3d;
typedef Math::Vector4<Double> Vector4d;
typedef Math::Matrix3<Double> Matrix3d;
typedef Math::Matrix4<Double> Matrix4d;
typedef Math::Matrix2x1<Double> Matrix2x1d;
typedef Math::Matrix2x2<Double> Matrix2x2d;
typedef Math::Matrix2x3<Double> Matrix2x3d;
typedef Math::Matrix2x4<Double> Matrix2x4d;
typedef Math::Matrix3x1<Double> Matrix3x1d;
typedef Math::Matrix3x2<Double> Matrix3x2d;
typedef Math::Matrix3x3<Double> Matrix3x3d;
typedef Math::Matrix3x4<Double> Matrix3x4d;
typedef Math::Matrix4x1<Double> Matrix4x1d;
typedef Math::Matrix4x2<Double> Matrix4x2d;
typedef Math::Matrix4x3<Double> Matrix4x3d;
typedef Math::Matrix4x4<Double> Matrix4x4d;
typedef Math::QuadraticBezier2D<Double> QuadraticBezier2Dd;
typedef Math::QuadraticBezier3D<Double> QuadraticBezier3Dd;
typedef Math::CubicBezier2D<Double> CubicBezier2Dd;
typedef Math::CubicBezier3D<Double> CubicBezier3Dd;
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
#include "Magnum/Math/BitVector.h"
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
#include "Magnum/Math/Swizzle.h"
#include "Magnum/Math/Tags.h"
#include "Magnum/Math/Time.h"
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
//#include "Magnum/Math/Algorithms/Svd.h" // TODO needs Optional and Triple
#ifdef MAGNUM_MATH_STL_COMPATIBILITY
// {{includes}}
#include "Magnum/Math/TimeStl.h"
#endif
#ifdef MAGNUM_MATH_GLM_INTEGRATION
// {{includes}}
#include "Magnum/GlmIntegration/Integration.h"
#include "Magnum/GlmIntegration/GtcIntegration.h"
#include "Magnum/GlmIntegration/GtxIntegration.h"
#endif
#ifdef MAGNUM_MATH_EIGEN_INTEGRATION
// {{includes}}
#include "Magnum/EigenIntegration/Integration.h"
// TODO: DynamicMatrixIntegration (separate library because of StridedArrayView)
#include "Magnum/EigenIntegration/GeometryIntegration.h"
#endif
#ifdef MAGNUM_MATH_IMPLEMENTATION
// {{ includes }}
#include "Magnum/Math/Functions.cpp"
#include "Magnum/Math/Packing.cpp"
/* Time.cpp is just debug output operators, not included */
#endif
