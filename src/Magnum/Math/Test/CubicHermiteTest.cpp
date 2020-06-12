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

#include <sstream>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Math/Bezier.h"
#include "Magnum/Math/CubicHermite.h"
#include "Magnum/Math/Functions.h"
#include "Magnum/Math/Vector2.h"
#include "Magnum/Math/StrictWeakOrdering.h"

namespace Magnum { namespace Math { namespace Test { namespace {

struct CubicHermiteTest: Corrade::TestSuite::Tester {
    explicit CubicHermiteTest();

    void constructScalar();
    void constructVector();
    void constructComplex();
    void constructQuaternion();

    void constructDefaultScalar();
    void constructDefaultVector();
    void constructDefaultComplex();
    void constructDefaultQuaternion();

    void constructZeroScalar();
    void constructZeroVector();
    void constructZeroComplex();
    void constructZeroQuaternion();

    void constructIdentityScalar();
    void constructIdentityVector();
    void constructIdentityComplex();
    void constructIdentityQuaternion();

    void constructNoInitScalar();
    void constructNoInitVector();
    void constructNoInitComplex();
    void constructNoInitQuaternion();

    void constructConversionScalar();
    void constructConversionVector();
    void constructConversionComplex();
    void constructConversionQuaternion();

    void constructFromBezier();

    void constructCopyScalar();
    void constructCopyVector();
    void constructCopyComplex();
    void constructCopyQuaternion();

    void dataScalar();
    void dataVector();
    void dataComplex();
    void dataQuaternion();

    void compareScalar();
    void compareVector();
    void compareComplex();
    void compareQuaternion();

    void selectScalar();
    void selectVector();
    void selectComplex();
    void selectQuaternion();

    void lerpScalar();
    void lerpVector();
    void lerpComplex();
    void lerpComplexNotNormalized();
    void lerpQuaternion();
    void lerpQuaternionNotNormalized();
    void lerpQuaternionShortestPath();
    void lerpQuaternionShortestPathNotNormalized();

    void slerpComplex();
    void slerpComplexNotNormalized();
    void slerpQuaternion();
    void slerpQuaternionNotNormalized();
    void slerpQuaternionShortestPath();
    void slerpQuaternionShortestPathNotNormalized();

    void splerpScalar();
    void splerpVector();
    void splerpVectorFromBezier();
    void splerpComplex();
    void splerpComplexNotNormalized();
    void splerpQuaternion();
    void splerpQuaternionNotNormalized();

    void strictWeakOrdering();

    void debugScalar();
    void debugVector();
    void debugComplex();
    void debugQuaternion();
};

CubicHermiteTest::CubicHermiteTest() {
    addTests({&CubicHermiteTest::constructScalar,
              &CubicHermiteTest::constructVector,
              &CubicHermiteTest::constructComplex,
              &CubicHermiteTest::constructQuaternion,

              &CubicHermiteTest::constructDefaultScalar,
              &CubicHermiteTest::constructDefaultVector,
              &CubicHermiteTest::constructDefaultComplex,
              &CubicHermiteTest::constructDefaultQuaternion,

              &CubicHermiteTest::constructZeroScalar,
              &CubicHermiteTest::constructZeroVector,
              &CubicHermiteTest::constructZeroComplex,
              &CubicHermiteTest::constructZeroQuaternion,

              &CubicHermiteTest::constructIdentityScalar,
              &CubicHermiteTest::constructIdentityVector,
              &CubicHermiteTest::constructIdentityComplex,
              &CubicHermiteTest::constructIdentityQuaternion,

              &CubicHermiteTest::constructNoInitScalar,
              &CubicHermiteTest::constructNoInitVector,
              &CubicHermiteTest::constructNoInitComplex,
              &CubicHermiteTest::constructNoInitQuaternion,

              &CubicHermiteTest::constructConversionScalar,
              &CubicHermiteTest::constructConversionVector,
              &CubicHermiteTest::constructConversionComplex,
              &CubicHermiteTest::constructConversionQuaternion,

              &CubicHermiteTest::constructFromBezier,

              &CubicHermiteTest::constructCopyScalar,
              &CubicHermiteTest::constructCopyVector,
              &CubicHermiteTest::constructCopyComplex,
              &CubicHermiteTest::constructCopyQuaternion,

              &CubicHermiteTest::dataScalar,
              &CubicHermiteTest::dataVector,
              &CubicHermiteTest::dataComplex,
              &CubicHermiteTest::dataQuaternion,

              &CubicHermiteTest::compareScalar,
              &CubicHermiteTest::compareVector,
              &CubicHermiteTest::compareComplex,
              &CubicHermiteTest::compareQuaternion,

              &CubicHermiteTest::selectScalar,
              &CubicHermiteTest::selectVector,
              &CubicHermiteTest::selectComplex,
              &CubicHermiteTest::selectQuaternion,

              &CubicHermiteTest::lerpScalar,
              &CubicHermiteTest::lerpVector,
              &CubicHermiteTest::lerpComplex,
              &CubicHermiteTest::lerpComplexNotNormalized,
              &CubicHermiteTest::lerpQuaternion,
              &CubicHermiteTest::lerpQuaternionNotNormalized,
              &CubicHermiteTest::lerpQuaternionShortestPath,
              &CubicHermiteTest::lerpQuaternionShortestPathNotNormalized,

              &CubicHermiteTest::slerpComplex,
              &CubicHermiteTest::slerpComplexNotNormalized,
              &CubicHermiteTest::slerpQuaternion,
              &CubicHermiteTest::slerpQuaternionNotNormalized,
              &CubicHermiteTest::slerpQuaternionShortestPath,
              &CubicHermiteTest::slerpQuaternionShortestPathNotNormalized,

              &CubicHermiteTest::splerpScalar,
              &CubicHermiteTest::splerpVector,
              &CubicHermiteTest::splerpVectorFromBezier,
              &CubicHermiteTest::splerpComplex,
              &CubicHermiteTest::splerpComplexNotNormalized,
              &CubicHermiteTest::splerpQuaternion,
              &CubicHermiteTest::splerpQuaternionNotNormalized,

              &CubicHermiteTest::strictWeakOrdering,

              &CubicHermiteTest::debugScalar,
              &CubicHermiteTest::debugVector,
              &CubicHermiteTest::debugComplex,
              &CubicHermiteTest::debugQuaternion});
}

using namespace Math::Literals;

typedef Math::Vector2<Float> Vector2;
typedef Math::Vector3<Float> Vector3;
typedef Math::Complex<Float> Complex;
typedef Math::Quaternion<Float> Quaternion;
typedef Math::CubicBezier2D<Float> CubicBezier2D;
typedef Math::CubicHermite1D<Float> CubicHermite1D;
typedef Math::CubicHermite2D<Float> CubicHermite2D;
typedef Math::CubicHermiteComplex<Float> CubicHermiteComplex;
typedef Math::CubicHermiteQuaternion<Float> CubicHermiteQuaternion;

void CubicHermiteTest::constructScalar() {
    constexpr CubicHermite1D a{2.0f, -2.0f, -0.5f};
    CubicHermite1D b{2.0f, -2.0f, -0.5f};

    CORRADE_COMPARE(a.inTangent(), 2.0f);
    CORRADE_COMPARE(b.inTangent(), 2.0f);
    CORRADE_COMPARE(a.point(), -2.0f);
    CORRADE_COMPARE(b.point(), -2.0f);
    CORRADE_COMPARE(a.outTangent(), -0.5f);
    CORRADE_COMPARE(b.outTangent(), -0.5f);

    CORRADE_VERIFY((std::is_nothrow_constructible<CubicHermite1D, Float, Float, Float>::value));
}

void CubicHermiteTest::constructVector() {
    constexpr CubicHermite2D a{{1.0f, 2.0f}, {1.5f, -2.0f}, {3.0f, -0.5f}};
    CubicHermite2D b{{1.0f, 2.0f}, {1.5f, -2.0f}, {3.0f, -0.5f}};

    CORRADE_COMPARE(a.inTangent(), (Vector2{1.0f, 2.0f}));
    CORRADE_COMPARE(b.inTangent(), (Vector2{1.0f, 2.0f}));
    CORRADE_COMPARE(a.point(), (Vector2{1.5f, -2.0f}));
    CORRADE_COMPARE(b.point(), (Vector2{1.5f, -2.0f}));
    CORRADE_COMPARE(a.outTangent(), (Vector2{3.0f, -0.5f}));
    CORRADE_COMPARE(b.outTangent(), (Vector2{3.0f, -0.5f}));

    CORRADE_VERIFY((std::is_nothrow_constructible<CubicHermite2D, Vector2, Vector2, Vector2>::value));
}

void CubicHermiteTest::constructComplex() {
    constexpr CubicHermiteComplex a{{1.0f, 2.0f}, {1.5f, -2.0f}, {3.0f, -0.5f}};
    CubicHermiteComplex b{{1.0f, 2.0f}, {1.5f, -2.0f}, {3.0f, -0.5f}};

    CORRADE_COMPARE(a.inTangent(), (Complex{1.0f, 2.0f}));
    CORRADE_COMPARE(b.inTangent(), (Complex{1.0f, 2.0f}));
    CORRADE_COMPARE(a.point(), (Complex{1.5f, -2.0f}));
    CORRADE_COMPARE(b.point(), (Complex{1.5f, -2.0f}));
    CORRADE_COMPARE(a.outTangent(), (Complex{3.0f, -0.5f}));
    CORRADE_COMPARE(b.outTangent(), (Complex{3.0f, -0.5f}));

    CORRADE_VERIFY((std::is_nothrow_constructible<CubicHermiteComplex, Complex, Complex, Complex>::value));
}

void CubicHermiteTest::constructQuaternion() {
    constexpr CubicHermiteQuaternion a{
        {{1.0f, 2.0f, -1.0f}, 3.0f},
        {{1.5f, -2.0f, 0.1f}, 1.1f},
        {{3.0f, -0.5f, 1.2f}, 0.3f}};
    CubicHermiteQuaternion b{
        {{1.0f, 2.0f, -1.0f}, 3.0f},
        {{1.5f, -2.0f, 0.1f}, 1.1f},
        {{3.0f, -0.5f, 1.2f}, 0.3f}};

    CORRADE_COMPARE(a.inTangent(), (Quaternion{{1.0f, 2.0f, -1.0f}, 3.0f}));
    CORRADE_COMPARE(b.inTangent(), (Quaternion{{1.0f, 2.0f, -1.0f}, 3.0f}));
    CORRADE_COMPARE(a.point(), (Quaternion{{1.5f, -2.0f, 0.1f}, 1.1f}));
    CORRADE_COMPARE(b.point(), (Quaternion{{1.5f, -2.0f, 0.1f}, 1.1f}));
    CORRADE_COMPARE(a.outTangent(), (Quaternion{{3.0f, -0.5f, 1.2f}, 0.3f}));
    CORRADE_COMPARE(b.outTangent(), (Quaternion{{3.0f, -0.5f, 1.2f}, 0.3f}));

    CORRADE_VERIFY((std::is_nothrow_constructible<CubicHermiteQuaternion, Quaternion, Quaternion, Quaternion>::value));
}

void CubicHermiteTest::constructDefaultScalar() {
    constexpr CubicHermite1D a;
    CubicHermite1D b;

    /* Equivalent to ZeroInit constructor */
    CORRADE_COMPARE(a, (CubicHermite1D{0.0f, 0.0f, 0.0f}));
    CORRADE_COMPARE(b, (CubicHermite1D{0.0f, 0.0f, 0.0f}));

    CORRADE_VERIFY((std::is_nothrow_default_constructible<CubicHermite1D>::value));
}

void CubicHermiteTest::constructDefaultVector() {
    constexpr CubicHermite2D a;
    CubicHermite2D b;

    /* Equivalent to ZeroInit constructor */
    CORRADE_COMPARE(a, (CubicHermite2D{{0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}}));
    CORRADE_COMPARE(b, (CubicHermite2D{{0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}}));

    CORRADE_VERIFY((std::is_nothrow_default_constructible<CubicHermite2D>::value));
}

void CubicHermiteTest::constructDefaultComplex() {
    constexpr CubicHermiteComplex a;
    CubicHermiteComplex b;

    /* Equivalent to IdentityInit constructor */
    CORRADE_COMPARE(a, (CubicHermiteComplex{{0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f}}));
    CORRADE_COMPARE(b, (CubicHermiteComplex{{0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f}}));

    CORRADE_VERIFY((std::is_nothrow_default_constructible<CubicHermiteComplex>::value));
}

void CubicHermiteTest::constructDefaultQuaternion() {
    constexpr CubicHermiteQuaternion a;
    CubicHermiteQuaternion b;

    /* Equivalent to IdentityInit constructor */
    CORRADE_COMPARE(a, (CubicHermiteQuaternion{
        {{0.0f, 0.0f, 0.0f}, 0.0f},
        {{0.0f, 0.0f, 0.0f}, 1.0f},
        {{0.0f, 0.0f, 0.0f}, 0.0f}}));
    CORRADE_COMPARE(b, (CubicHermiteQuaternion{
        {{0.0f, 0.0f, 0.0f}, 0.0f},
        {{0.0f, 0.0f, 0.0f}, 1.0f},
        {{0.0f, 0.0f, 0.0f}, 0.0f}}));

    CORRADE_VERIFY((std::is_nothrow_default_constructible<CubicHermiteQuaternion>::value));
}

void CubicHermiteTest::constructZeroScalar() {
    constexpr CubicHermite1D a{ZeroInit};
    CubicHermite1D b{ZeroInit};

    CORRADE_COMPARE(a, (CubicHermite1D{0.0f, 0.0f, 0.0f}));
    CORRADE_COMPARE(b, (CubicHermite1D{0.0f, 0.0f, 0.0f}));

    CORRADE_VERIFY((std::is_nothrow_constructible<CubicHermite1D, ZeroInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<ZeroInitT, CubicHermite1D>::value));
}

void CubicHermiteTest::constructZeroVector() {
    constexpr CubicHermite2D a{ZeroInit};
    CubicHermite2D b{ZeroInit};

    CORRADE_COMPARE(a, (CubicHermite2D{{0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}}));
    CORRADE_COMPARE(b, (CubicHermite2D{{0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}}));

    CORRADE_VERIFY((std::is_nothrow_constructible<CubicHermite2D, ZeroInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<ZeroInitT, CubicHermite2D>::value));
}

void CubicHermiteTest::constructZeroComplex() {
    constexpr CubicHermiteComplex a{ZeroInit};
    CubicHermiteComplex b{ZeroInit};

    CORRADE_COMPARE(a, (CubicHermiteComplex{{0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}}));
    CORRADE_COMPARE(b, (CubicHermiteComplex{{0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}}));

    CORRADE_VERIFY((std::is_nothrow_constructible<CubicHermiteComplex, ZeroInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<ZeroInitT, CubicHermiteComplex>::value));
}

void CubicHermiteTest::constructZeroQuaternion() {
    constexpr CubicHermiteQuaternion a{ZeroInit};
    CubicHermiteQuaternion b{ZeroInit};

    CORRADE_COMPARE(a, (CubicHermiteQuaternion{
        {{0.0f, 0.0f, 0.0f}, 0.0f},
        {{0.0f, 0.0f, 0.0f}, 0.0f},
        {{0.0f, 0.0f, 0.0f}, 0.0f}}));
    CORRADE_COMPARE(b, (CubicHermiteQuaternion{
        {{0.0f, 0.0f, 0.0f}, 0.0f},
        {{0.0f, 0.0f, 0.0f}, 0.0f},
        {{0.0f, 0.0f, 0.0f}, 0.0f}}));

    CORRADE_VERIFY((std::is_nothrow_constructible<CubicHermiteQuaternion, ZeroInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<ZeroInitT, CubicHermiteQuaternion>::value));
}

void CubicHermiteTest::constructIdentityScalar() {
    CORRADE_VERIFY(!(std::is_constructible<CubicHermite1D, IdentityInitT>::value));
}

void CubicHermiteTest::constructIdentityVector() {
    CORRADE_VERIFY(!(std::is_constructible<CubicHermite2D, IdentityInitT>::value));
}

void CubicHermiteTest::constructIdentityComplex() {
    constexpr CubicHermiteComplex a{IdentityInit};
    CubicHermiteComplex b{IdentityInit};

    CORRADE_COMPARE(a, (CubicHermiteComplex{{0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f}}));
    CORRADE_COMPARE(b, (CubicHermiteComplex{{0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f}}));

    CORRADE_VERIFY((std::is_nothrow_constructible<CubicHermiteComplex, IdentityInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<IdentityInitT, CubicHermiteComplex>::value));
}

void CubicHermiteTest::constructIdentityQuaternion() {
    constexpr CubicHermiteQuaternion a{IdentityInit};
    CubicHermiteQuaternion b{IdentityInit};

    CORRADE_COMPARE(a, (CubicHermiteQuaternion{
        {{0.0f, 0.0f, 0.0f}, 0.0f},
        {{0.0f, 0.0f, 0.0f}, 1.0f},
        {{0.0f, 0.0f, 0.0f}, 0.0f}}));
    CORRADE_COMPARE(b, (CubicHermiteQuaternion{
        {{0.0f, 0.0f, 0.0f}, 0.0f},
        {{0.0f, 0.0f, 0.0f}, 1.0f},
        {{0.0f, 0.0f, 0.0f}, 0.0f}}));

    CORRADE_VERIFY((std::is_nothrow_constructible<CubicHermiteComplex, IdentityInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<IdentityInitT, CubicHermiteQuaternion>::value));
}

void CubicHermiteTest::constructNoInitScalar() {
    CubicHermite1D spline{2.0f, -2.0f, -0.5f};
    new(&spline) CubicHermite1D{Magnum::NoInit};
    {
        #if defined(__GNUC__) && __GNUC__*100 + __GNUC_MINOR__ >= 601 && __OPTIMIZE__
        CORRADE_EXPECT_FAIL("GCC 6.1+ misoptimizes and overwrites the value.");
        #endif
        CORRADE_COMPARE(spline, (CubicHermite1D{2.0f, -2.0f, -0.5f}));
    }

    CORRADE_VERIFY((std::is_nothrow_constructible<CubicHermite1D, Magnum::NoInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Magnum::NoInitT, CubicHermite1D>::value));
}

void CubicHermiteTest::constructNoInitVector() {
    CubicHermite2D spline{{1.0f, 2.0f}, {1.5f, -2.0f}, {3.0f, -0.5f}};
    new(&spline) CubicHermite2D{Magnum::NoInit};
    {
        #if defined(__GNUC__) && __GNUC__*100 + __GNUC_MINOR__ >= 601 && __OPTIMIZE__
        CORRADE_EXPECT_FAIL("GCC 6.1+ misoptimizes and overwrites the value.");
        #endif
        CORRADE_COMPARE(spline, (CubicHermite2D{{1.0f, 2.0f}, {1.5f, -2.0f}, {3.0f, -0.5f}}));
    }

    CORRADE_VERIFY((std::is_nothrow_constructible<CubicHermite2D, Magnum::NoInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Magnum::NoInitT, CubicHermite2D>::value));
}

void CubicHermiteTest::constructNoInitComplex() {
    CubicHermiteComplex spline{{1.0f, 2.0f}, {1.5f, -2.0f}, {3.0f, -0.5f}};
    new(&spline) CubicHermiteComplex{Magnum::NoInit};
    {
        #if defined(__GNUC__) && __GNUC__*100 + __GNUC_MINOR__ >= 601 && __OPTIMIZE__
        CORRADE_EXPECT_FAIL("GCC 6.1+ misoptimizes and overwrites the value.");
        #endif
        CORRADE_COMPARE(spline, (CubicHermiteComplex{{1.0f, 2.0f}, {1.5f, -2.0f}, {3.0f, -0.5f}}));
    }

    CORRADE_VERIFY((std::is_nothrow_constructible<CubicHermiteComplex, Magnum::NoInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Magnum::NoInitT, CubicHermiteComplex>::value));
}

void CubicHermiteTest::constructNoInitQuaternion() {
    CubicHermiteQuaternion spline{
        {{1.0f, 2.0f, -1.0f}, 3.0f},
        {{1.5f, -2.0f, 0.1f}, 1.1f},
        {{3.0f, -0.5f, 1.2f}, 0.3f}};
    new(&spline) CubicHermiteQuaternion{Magnum::NoInit};
    {
        #if defined(__GNUC__) && __GNUC__*100 + __GNUC_MINOR__ >= 601 && __OPTIMIZE__
        CORRADE_EXPECT_FAIL("GCC 6.1+ misoptimizes and overwrites the value.");
        #endif
        CORRADE_COMPARE(spline, (CubicHermiteQuaternion{
            {{1.0f, 2.0f, -1.0f}, 3.0f},
            {{1.5f, -2.0f, 0.1f}, 1.1f},
            {{3.0f, -0.5f, 1.2f}, 0.3f}}));
    }

    CORRADE_VERIFY((std::is_nothrow_constructible<CubicHermiteQuaternion, Magnum::NoInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Magnum::NoInitT, CubicHermiteQuaternion>::value));
}

void CubicHermiteTest::constructConversionScalar() {
    typedef Math::CubicHermite1D<Int> CubicHermite1Di;

    constexpr CubicHermite1D a{2.0f, -2.0f, -0.5f};
    constexpr CubicHermite1Di b{a};
    CubicHermite1Di c{a};
    CORRADE_COMPARE(b, (CubicHermite1Di{2, -2, 0}));
    CORRADE_COMPARE(c, (CubicHermite1Di{2, -2, 0}));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<CubicHermite1D, CubicHermite1Di>::value));

    CORRADE_VERIFY((std::is_nothrow_constructible<CubicHermite1D, CubicHermite1Di>::value));
}

void CubicHermiteTest::constructConversionVector() {
    typedef Math::CubicHermite2D<Int> CubicHermite2Di;

    constexpr CubicHermite2D a{{1.0f, 2.0f}, {1.5f, -2.0f}, {3.0f, -0.5f}};
    constexpr CubicHermite2Di b{a};
    CubicHermite2Di c{a};
    CORRADE_COMPARE(b, (CubicHermite2Di{{1, 2}, {1, -2}, {3, 0}}));
    CORRADE_COMPARE(c, (CubicHermite2Di{{1, 2}, {1, -2}, {3, 0}}));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<CubicHermite2D, CubicHermite2Di>::value));

    CORRADE_VERIFY((std::is_nothrow_constructible<CubicHermite2D, CubicHermite2Di>::value));
}

void CubicHermiteTest::constructConversionComplex() {
    typedef Math::CubicHermiteComplex<Int> CubicHermiteComplexi;

    constexpr CubicHermiteComplex a{{1.0f, 2.0f}, {1.5f, -2.0f}, {3.0f, -0.5f}};
    constexpr CubicHermiteComplexi b{a};
    CubicHermiteComplexi c{a};
    CORRADE_COMPARE(b, (CubicHermiteComplexi{{1, 2}, {1, -2}, {3, 0}}));
    CORRADE_COMPARE(c, (CubicHermiteComplexi{{1, 2}, {1, -2}, {3, 0}}));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<CubicHermiteComplex, CubicHermiteComplexi>::value));

    CORRADE_VERIFY((std::is_nothrow_constructible<CubicHermiteComplex, CubicHermiteComplexi>::value));
}

void CubicHermiteTest::constructConversionQuaternion() {
    typedef Math::CubicHermiteQuaternion<Int> CubicHermiteQuaternioni;

    constexpr CubicHermiteQuaternion a{
        {{1.0f, 2.0f, -1.0f}, 3.0f},
        {{1.5f, -2.0f, 0.1f}, 1.1f},
        {{3.0f, -0.5f, 1.2f}, 0.3f}};
    constexpr CubicHermiteQuaternioni b{a};
    CubicHermiteQuaternioni c{a};
    CORRADE_COMPARE(b, (CubicHermiteQuaternioni{
        {{1, 2, -1}, 3},
        {{1, -2, 0}, 1},
        {{3, 0, 1}, 0}}));
    CORRADE_COMPARE(c, (CubicHermiteQuaternioni{
        {{1, 2, -1}, 3},
        {{1, -2, 0}, 1},
        {{3, 0, 1}, 0}}));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<CubicHermiteQuaternion, CubicHermiteQuaternioni>::value));

    CORRADE_VERIFY((std::is_nothrow_constructible<CubicHermiteQuaternion, CubicHermiteQuaternioni>::value));
}

void CubicHermiteTest::constructFromBezier() {
    /* Taken from BezierTest::valueCubic() -- we're testing the same values
       also in splerpVectorFromBezier(). See
       BezierTest::constructFromCubicHermite() for the inverse. */
    CubicBezier2D bezier{Vector2{0.0f, 0.0f}, Vector2{10.0f, 15.0f}, Vector2{20.0f, 4.0f}, Vector2{5.0f, -20.0f}};
    auto a = CubicHermite2D::fromBezier({Vector2{}, Vector2{}, Vector2{}, bezier[0]}, bezier);
    auto b = CubicHermite2D::fromBezier(bezier, {bezier[3], Vector2{}, Vector2{}, Vector2{}});

    CORRADE_COMPARE(a.point(), bezier[0]);
    CORRADE_COMPARE(a.outTangent(), (Vector2{30.0f, 45.0f}));
    CORRADE_COMPARE(b.inTangent(), (Vector2{-45, -72}));
    CORRADE_COMPARE(b.point(), bezier[3]);
}

void CubicHermiteTest::constructCopyScalar() {
    constexpr CubicHermite1D a{2.0f, -2.0f, -0.5f};
    constexpr CubicHermite1D b{a};

    CORRADE_COMPARE(b, (CubicHermite1D{2.0f, -2.0f, -0.5f}));

    CORRADE_VERIFY(std::is_nothrow_copy_constructible<CubicHermite1D>::value);
    CORRADE_VERIFY(std::is_nothrow_copy_assignable<CubicHermite1D>::value);
}

void CubicHermiteTest::constructCopyVector() {
    constexpr CubicHermite2D a{{1.0f, 2.0f}, {1.5f, -2.0f}, {3.0f, -0.5f}};
    constexpr CubicHermite2D b{a};

    CORRADE_COMPARE(b, (CubicHermite2D{{1.0f, 2.0f}, {1.5f, -2.0f}, {3.0f, -0.5f}}));

    CORRADE_VERIFY(std::is_nothrow_copy_constructible<CubicHermite2D>::value);
    CORRADE_VERIFY(std::is_nothrow_copy_assignable<CubicHermite2D>::value);
}

void CubicHermiteTest::constructCopyComplex() {
    constexpr CubicHermiteComplex a{{1.0f, 2.0f}, {1.5f, -2.0f}, {3.0f, -0.5f}};
    constexpr CubicHermiteComplex b{a};

    CORRADE_COMPARE(b, (CubicHermiteComplex{{1.0f, 2.0f}, {1.5f, -2.0f}, {3.0f, -0.5f}}));

    CORRADE_VERIFY(std::is_nothrow_copy_constructible<CubicHermiteComplex>::value);
    CORRADE_VERIFY(std::is_nothrow_copy_assignable<CubicHermiteComplex>::value);
}

void CubicHermiteTest::constructCopyQuaternion() {
    constexpr CubicHermiteQuaternion a{
        {{1.0f, 2.0f, -1.0f}, 3.0f},
        {{1.5f, -2.0f, 0.1f}, 1.1f},
        {{3.0f, -0.5f, 1.2f}, 0.3f}};
    constexpr CubicHermiteQuaternion b{a};

    CORRADE_COMPARE(b, (CubicHermiteQuaternion{
        {{1.0f, 2.0f, -1.0f}, 3.0f},
        {{1.5f, -2.0f, 0.1f}, 1.1f},
        {{3.0f, -0.5f, 1.2f}, 0.3f}}));

    CORRADE_VERIFY(std::is_nothrow_copy_constructible<CubicHermiteComplex>::value);
    CORRADE_VERIFY(std::is_nothrow_copy_assignable<CubicHermiteComplex>::value);
}

void CubicHermiteTest::dataScalar() {
    constexpr CubicHermite1D ca{2.0f, -2.0f, -0.5f};
    constexpr Float inTangent = ca.inTangent();
    constexpr Float point = ca.point();
    constexpr Float outTangent = ca.outTangent();
    CORRADE_COMPARE(inTangent, 2.0f);
    CORRADE_COMPARE(point, -2.0f);
    CORRADE_COMPARE(outTangent, -0.5f);

    CubicHermite1D a{2.0f, -2.0f, -0.5f};
    a.inTangent() = 3.0f;
    a.point() = 1.0f;
    a.outTangent() = 2.1f;
    CORRADE_COMPARE(a, (CubicHermite1D{3.0f, 1.0f, 2.1f}));

    constexpr Float b = *ca.data();
    Float c = a.data()[2];
    CORRADE_COMPARE(b, 2.0f);
    CORRADE_COMPARE(c, 2.1f);
}

void CubicHermiteTest::dataVector() {
    constexpr CubicHermite2D ca{{1.0f, 2.0f}, {1.5f, -2.0f}, {3.0f, -0.5f}};
    constexpr Vector2 inTangent = ca.inTangent();
    constexpr Vector2 point = ca.point();
    constexpr Vector2 outTangent = ca.outTangent();
    CORRADE_COMPARE(inTangent, (Vector2{1.0f, 2.0f}));
    CORRADE_COMPARE(point, (Vector2{1.5f, -2.0f}));
    CORRADE_COMPARE(outTangent, (Vector2{3.0f, -0.5f}));

    CubicHermite2D a{{1.0f, 2.0f}, {1.5f, -2.0f}, {3.0f, -0.5f}};
    a.inTangent().y() = 3.0f;
    a.point().x() = 1.0f;
    a.outTangent().y() = 2.0f;
    CORRADE_COMPARE(a, (CubicHermite2D{{1.0f, 3.0f}, {1.0f, -2.0f}, {3.0f, 2.0f}}));

    constexpr Vector2 b = *ca.data();
    Vector2 c = a.data()[2];
    CORRADE_COMPARE(b, (Vector2{1.0f, 2.0f}));
    CORRADE_COMPARE(c, (Vector2{3.0f, 2.0f}));
}

void CubicHermiteTest::dataComplex() {
    constexpr CubicHermiteComplex ca{{1.0f, 2.0f}, {1.5f, -2.0f}, {3.0f, -0.5f}};
    constexpr Complex inTangent = ca.inTangent();
    constexpr Complex point = ca.point();
    constexpr Complex outTangent = ca.outTangent();
    CORRADE_COMPARE(inTangent, (Complex{1.0f, 2.0f}));
    CORRADE_COMPARE(point, (Complex{1.5f, -2.0f}));
    CORRADE_COMPARE(outTangent, (Complex{3.0f, -0.5f}));

    CubicHermiteComplex a{{1.0f, 2.0f}, {1.5f, -2.0f}, {3.0f, -0.5f}};
    a.inTangent().imaginary() = 3.0f;
    a.point().real() = 1.0f;
    a.outTangent().imaginary() = 2.0f;
    CORRADE_COMPARE(a, (CubicHermiteComplex{{1.0f, 3.0f}, {1.0f, -2.0f}, {3.0f, 2.0f}}));

    constexpr Complex b = *ca.data();
    Complex c = a.data()[2];
    CORRADE_COMPARE(b, (Complex{1.0f, 2.0f}));
    CORRADE_COMPARE(c, (Complex{3.0f, 2.0f}));
}

void CubicHermiteTest::dataQuaternion() {
    constexpr CubicHermiteQuaternion ca{
        {{1.0f, 2.0f, -1.0f}, 3.0f},
        {{1.5f, -2.0f, 0.1f}, 1.1f},
        {{3.0f, -0.5f, 1.2f}, 0.3f}};
    constexpr Quaternion inTangent = ca.inTangent();
    constexpr Quaternion point = ca.point();
    constexpr Quaternion outTangent = ca.outTangent();

    CORRADE_COMPARE(inTangent, (Quaternion{{1.0f, 2.0f, -1.0f}, 3.0f}));
    CORRADE_COMPARE(point, (Quaternion{{1.5f, -2.0f, 0.1f}, 1.1f}));
    CORRADE_COMPARE(outTangent, (Quaternion{{3.0f, -0.5f, 1.2f}, 0.3f}));

    CubicHermiteQuaternion a{
        {{1.0f, 2.0f, -1.0f}, 3.0f},
        {{1.5f, -2.0f, 0.1f}, 1.1f},
        {{3.0f, -0.5f, 1.2f}, 0.3f}};
    a.inTangent().vector().y() = 3.0f;
    a.point().scalar() = 1.0f;
    a.outTangent().vector().z() = 2.0f;
    CORRADE_COMPARE(a, (CubicHermiteQuaternion{
        {{1.0f, 3.0f, -1.0f}, 3.0f},
        {{1.5f, -2.0f, 0.1f}, 1.0f},
        {{3.0f, -0.5f, 2.0f}, 0.3f}}));

    constexpr Quaternion b = *ca.data();
    Quaternion c = a.data()[2];
    CORRADE_COMPARE(b, (Quaternion{{1.0f, 2.0f, -1.0f}, 3.0f}));
    CORRADE_COMPARE(c, (Quaternion{{3.0f, -0.5f, 2.0f}, 0.3f}));
}

void CubicHermiteTest::compareScalar() {
    CORRADE_VERIFY((CubicHermite1D{3.0f, 1.0f, 2.0f} == CubicHermite1D{3.0f, 1.0f + TypeTraits<Float>::epsilon()/2, 2.0f}));
    CORRADE_VERIFY((CubicHermite1D{3.0f, 1.0f, 2.0f} != CubicHermite1D{3.0f + TypeTraits<Float>::epsilon()*6, 1.0f, 2.0f}));
}

void CubicHermiteTest::compareVector() {
    CORRADE_VERIFY((CubicHermite2D{{1.0f, 3.0f}, {1.0f, -2.0f}, {3.0f, 2.0f}}) == (CubicHermite2D{{1.0f, 3.0f}, {1.0f, -2.0f}, {3.0f, 2.0f + TypeTraits<Float>::epsilon()/2}}));
    CORRADE_VERIFY((CubicHermite2D{{1.0f, 3.0f}, {1.0f, -2.0f}, {3.0f, 2.0f}}) != (CubicHermite2D{{1.0f + TypeTraits<Float>::epsilon()*2, 3.0f}, {1.0f, -2.0f}, {3.0f, 2.0f}}));
}

void CubicHermiteTest::compareComplex() {
    CORRADE_VERIFY((CubicHermiteComplex{{1.0f, 3.0f}, {1.0f, -2.0f}, {3.0f, 2.0f}}) == (CubicHermiteComplex{{1.0f, 3.0f}, {1.0f, -2.0f}, {3.0f, 2.0f + TypeTraits<Float>::epsilon()/2}}));
    CORRADE_VERIFY((CubicHermiteComplex{{1.0f, 3.0f}, {1.0f, -2.0f}, {3.0f, 2.0f}}) != (CubicHermiteComplex{{1.0f + TypeTraits<Float>::epsilon()*2, 3.0f}, {1.0f, -2.0f}, {3.0f, 2.0f}}));
}

void CubicHermiteTest::compareQuaternion() {
    CORRADE_VERIFY((CubicHermiteQuaternion{
        {{1.0f, 3.0f, -1.0f}, 3.0f},
        {{1.5f, -2.0f, 0.1f}, 1.0f},
        {{3.0f, -0.5f, 2.0f}, 0.3f}}) == (CubicHermiteQuaternion{
        {{1.0f, 3.0f, -1.0f}, 3.0f},
        {{1.5f, -2.0f, 0.1f}, 1.0f + TypeTraits<Float>::epsilon()/2},
        {{3.0f, -0.5f, 2.0f}, 0.3f}}));
    CORRADE_VERIFY((CubicHermiteQuaternion{
        {{1.0f, 3.0f, -1.0f}, 3.0f},
        {{1.5f, -2.0f, 0.1f}, 1.0f},
        {{3.0f, -0.5f, 2.0f}, 0.3f}}) != (CubicHermiteQuaternion{
        {{1.0f + TypeTraits<Float>::epsilon()*2, 3.0f, -1.0f}, 3.0f},
        {{1.5f, -2.0f, 0.1f}, 1.0f},
        {{3.0f, -0.5f, 2.0f}, 0.3f}}));
}

void CubicHermiteTest::selectScalar() {
    CubicHermite1D a{2.0f, 3.0f, -1.0f};
    CubicHermite1D b{5.0f, -2.0f, 1.5f};

    CORRADE_COMPARE(Math::select(a, b, 0.0f), 3.0f);
    CORRADE_COMPARE(Math::select(a, b, 0.8f), 3.0f);
    CORRADE_COMPARE(Math::select(a, b, 1.0f), -2.0f);
}

void CubicHermiteTest::selectVector() {
    CubicHermite2D a{{2.0f, 1.5f}, {3.0f, 0.1f}, {-1.0f, 0.0f}};
    CubicHermite2D b{{5.0f, 0.3f}, {-2.0f, 1.1f}, {1.5f, 0.3f}};

    CORRADE_COMPARE(Math::select(a, b, 0.0f), (Vector2{3.0f, 0.1f}));
    CORRADE_COMPARE(Math::select(a, b, 0.8f), (Vector2{3.0f, 0.1f}));
    CORRADE_COMPARE(Math::select(a, b, 1.0f), (Vector2{-2.0f, 1.1f}));
}

void CubicHermiteTest::selectComplex() {
    CubicHermiteComplex a{{2.0f, 1.5f}, {3.0f, 0.1f}, {-1.0f, 0.0f}};
    CubicHermiteComplex b{{5.0f, 0.3f}, {-2.0f, 1.1f}, {1.5f, 0.3f}};

    CORRADE_COMPARE(Math::select(a, b, 0.0f), (Complex{3.0f, 0.1f}));
    CORRADE_COMPARE(Math::select(a, b, 0.8f), (Complex{3.0f, 0.1f}));
    CORRADE_COMPARE(Math::select(a, b, 1.0f), (Complex{-2.0f, 1.1f}));
}

void CubicHermiteTest::selectQuaternion() {
    CubicHermiteQuaternion a{
        {{2.0f, 1.5f, 0.3f}, 1.1f},
        {{3.0f, 0.1f, 2.3f}, 0.7f},
        {{-1.0f, 0.0f, 0.3f}, 0.4f}};
    CubicHermiteQuaternion b{
        {{5.0f, 0.3f, 1.1f}, 0.5f},
        {{-2.0f, 1.1f, 1.0f}, 1.3f},
        {{1.5f, 0.3f, 17.0f}, -7.0f}};

    CORRADE_COMPARE(Math::select(a, b, 0.0f), a.point());
    CORRADE_COMPARE(Math::select(a, b, 0.8f), a.point());
    CORRADE_COMPARE(Math::select(a, b, 1.0f), b.point());
}

void CubicHermiteTest::lerpScalar() {
    CubicHermite1D a{2.0f, 3.0f, -1.0f};
    CubicHermite1D b{5.0f, -2.0f, 1.5f};

    CORRADE_COMPARE(Math::lerp(a, b, 0.0f), 3.0f);
    CORRADE_COMPARE(Math::lerp(a, b, 1.0f), -2.0f);

    CORRADE_COMPARE(Math::lerp(a, b, 0.35f), 1.25f);
    CORRADE_COMPARE(Math::lerp(a.point(), b.point(), 0.35f), 1.25f);

    CORRADE_COMPARE(Math::lerp(a, b, 0.8f), -1.0f);
    CORRADE_COMPARE(Math::lerp(a.point(), b.point(), 0.8f), -1.0f);
}

void CubicHermiteTest::lerpVector() {
    CubicHermite2D a{{2.0f, 1.5f}, {3.0f, 0.1f}, {-1.0f, 0.0f}};
    CubicHermite2D b{{5.0f, 0.3f}, {-2.0f, 1.1f}, {1.5f, 0.3f}};

    CORRADE_COMPARE(Math::lerp(a, b, 0.0f), a.point());
    CORRADE_COMPARE(Math::lerp(a, b, 1.0f), b.point());

    Vector2 expected035{1.25f, 0.45f};
    CORRADE_COMPARE(Math::lerp(a, b, 0.35f), expected035);
    CORRADE_COMPARE(Math::lerp(a.point(), b.point(), 0.35f), expected035);

    Vector2 expected08{-1.0f, 0.9f};
    CORRADE_COMPARE(Math::lerp(a, b, 0.8f), expected08);
    CORRADE_COMPARE(Math::lerp(a.point(), b.point(), 0.8f), expected08);
}

void CubicHermiteTest::lerpComplex() {
    CubicHermiteComplex a{{2.0f, 1.5f}, {0.999445f, 0.0333148f}, {-1.0f, 0.0f}};
    CubicHermiteComplex b{{5.0f, 0.3f}, {-0.876216f, 0.481919f}, {1.5f, 0.3f}};

    CORRADE_COMPARE(Math::lerp(a, b, 0.0f), a.point());
    CORRADE_COMPARE(Math::lerp(a, b, 1.0f), b.point());

    Complex expected035{0.874384f, 0.485235f};
    CORRADE_COMPARE(Math::lerp(a, b, 0.35f), expected035);
    CORRADE_COMPARE(Math::lerp(a.point(), b.point(), 0.35f), expected035);
    CORRADE_VERIFY(Math::lerp(a, b, 0.35f).isNormalized());

    Complex expected08{-0.78747f, 0.616353f};
    CORRADE_COMPARE(Math::lerp(a, b, 0.8f), expected08);
    CORRADE_COMPARE(Math::lerp(a.point(), b.point(), 0.8f), expected08);
    CORRADE_VERIFY(Math::lerp(a, b, 0.8f).isNormalized());
}

void CubicHermiteTest::lerpComplexNotNormalized() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    /* This one should not assert as the default constructor should create
       identity point */
    CORRADE_COMPARE(Math::lerp(CubicHermiteComplex{}, {}, 0.3f), Complex{});

    /* These will, tho */
    CubicHermiteComplex a{{}, Complex{}*2.0f, {}};
    Math::lerp({}, a, 0.3f);
    Math::lerp(a, {}, 0.3f);
    CORRADE_COMPARE(out.str(),
        "Math::lerp(): complex numbers Complex(1, 0) and Complex(2, 0) are not normalized\n"
        "Math::lerp(): complex numbers Complex(2, 0) and Complex(1, 0) are not normalized\n");
}

void CubicHermiteTest::lerpQuaternion() {
    CubicHermiteQuaternion a{
        {{2.0f, 1.5f, 0.3f}, 1.1f},
        {{0.780076f, 0.0260025f, 0.598059f}, 0.182018f},
        {{-1.0f, 0.0f, 0.3f}, 0.4f}};
    CubicHermiteQuaternion b{
        {{5.0f, 0.3f, 1.1f}, 0.5f},
        {{-0.711568f, 0.391362f, 0.355784f}, 0.462519f},
        {{1.5f, 0.3f, 17.0f}, -7.0f}};

    CORRADE_COMPARE(Math::lerp(a, b, 0.0f), a.point());
    CORRADE_COMPARE(Math::lerp(a, b, 1.0f), b.point());

    Quaternion expected035{{0.392449f, 0.234067f, 0.780733f}, 0.426207f};
    CORRADE_COMPARE(Math::lerp(a, b, 0.35f), expected035);
    CORRADE_COMPARE(Math::lerp(a.point(), b.point(), 0.35f), expected035);
    CORRADE_VERIFY(Math::lerp(a, b, 0.35f).isNormalized());

    Quaternion expected08{{-0.533196f, 0.410685f, 0.521583f}, 0.524396f};
    CORRADE_COMPARE(Math::lerp(a, b, 0.8f), expected08);
    CORRADE_COMPARE(Math::lerp(a.point(), b.point(), 0.8f), expected08);
    CORRADE_VERIFY(Math::lerp(a, b, 0.8f).isNormalized());
}

void CubicHermiteTest::lerpQuaternionNotNormalized() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    /* This one should not assert as the default constructor should create
       identity point */
    Math::lerp(CubicHermiteQuaternion{}, {}, 0.3f);

    /* These will, tho */
    CubicHermiteQuaternion a{{}, Quaternion{}*2.0f, {}};
    Math::lerp({}, a, 0.3f);
    Math::lerp(a, {}, 0.3f);
    CORRADE_COMPARE(out.str(),
        "Math::lerp(): quaternions Quaternion({0, 0, 0}, 1) and Quaternion({0, 0, 0}, 2) are not normalized\n"
        "Math::lerp(): quaternions Quaternion({0, 0, 0}, 2) and Quaternion({0, 0, 0}, 1) are not normalized\n");
}

void CubicHermiteTest::lerpQuaternionShortestPath() {
    /* Values from QuaternionTest::lerpShortestPath() */
    CubicHermiteQuaternion a{
        {{2.0f, 1.5f, 0.3f}, 1.1f},
        Quaternion::rotation(0.0_degf, Vector3::zAxis()),
        {{-1.0f, 0.0f, 0.3f}, 0.4f}};
    CubicHermiteQuaternion b{
        {{5.0f, 0.3f, 1.1f}, 0.5f},
        Quaternion::rotation(225.0_degf, Vector3::zAxis()),
        {{1.5f, 0.3f, 17.0f}, -7.0f}};

    Quaternion lerp = Math::lerp(a, b, 0.25f);
    Quaternion lerpShortestPath = Math::lerpShortestPath(a, b, 0.25f);
    CORRADE_COMPARE(lerp.axis(), Vector3::zAxis());
    CORRADE_COMPARE(lerpShortestPath.axis(), Vector3::zAxis());
    CORRADE_COMPARE(lerp.angle(), 38.8848_degf);
    CORRADE_COMPARE(lerpShortestPath.angle(), 329.448_degf);

    Quaternion expected{{0.0f, 0.0f, 0.26347f}, -0.964667f};
    CORRADE_COMPARE(lerpShortestPath, expected);
    CORRADE_COMPARE(Math::lerpShortestPath(a.point(), b.point(), 0.25f), expected);
}

void CubicHermiteTest::lerpQuaternionShortestPathNotNormalized() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    /* This one should not assert as the default constructor should create
       identity point */
    Math::lerpShortestPath(CubicHermiteQuaternion{}, {}, 0.3f);

    /* These will, tho */
    CubicHermiteQuaternion a{{}, Quaternion{}*2.0f, {}};
    Math::lerpShortestPath({}, a, 0.3f);
    Math::lerpShortestPath(a, {}, 0.3f);
    /* lerpShortestPath() is calling lerp(), so the message is from there */
    CORRADE_COMPARE(out.str(),
        "Math::lerp(): quaternions Quaternion({0, 0, 0}, 1) and Quaternion({0, 0, 0}, 2) are not normalized\n"
        "Math::lerp(): quaternions Quaternion({0, 0, 0}, 2) and Quaternion({0, 0, 0}, 1) are not normalized\n");
}

void CubicHermiteTest::slerpComplex() {
    CubicHermiteComplex a{{2.0f, 1.5f}, {0.999445f, 0.0333148f}, {-1.0f, 0.0f}};
    CubicHermiteComplex b{{5.0f, 0.3f}, {-0.876216f, 0.481919f}, {1.5f, 0.3f}};

    CORRADE_COMPARE(Math::slerp(a, b, 0.0f), a.point());
    CORRADE_COMPARE(Math::slerp(a, b, 1.0f), b.point());

    Complex expected035{0.585564f, 0.810627f};
    CORRADE_COMPARE(Math::slerp(a, b, 0.35f), expected035);
    CORRADE_COMPARE(Math::slerp(a.point(), b.point(), 0.35f), expected035);
    CORRADE_VERIFY(Math::slerp(a, b, 0.35f).isNormalized());

    Complex expected08{-0.520014f, 0.854159f};
    CORRADE_COMPARE(Math::slerp(a, b, 0.8f), expected08);
    CORRADE_COMPARE(Math::slerp(a.point(), b.point(), 0.8f), expected08);
    CORRADE_VERIFY(Math::slerp(a, b, 0.8f).isNormalized());
}

void CubicHermiteTest::slerpComplexNotNormalized() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    /* This one should not assert as the default constructor should create
       identity point */
    CORRADE_COMPARE(Math::slerp(CubicHermiteComplex{}, {}, 0.3f), Complex{});

    /* These will, tho */
    CubicHermiteComplex a{{}, Complex{}*2.0f, {}};
    Math::slerp({}, a, 0.3f);
    Math::slerp(a, {}, 0.3f);
    CORRADE_COMPARE(out.str(),
        "Math::slerp(): complex numbers Complex(1, 0) and Complex(2, 0) are not normalized\n"
        "Math::slerp(): complex numbers Complex(2, 0) and Complex(1, 0) are not normalized\n");
}

void CubicHermiteTest::slerpQuaternion() {
    CubicHermiteQuaternion a{
        {{2.0f, 1.5f, 0.3f}, 1.1f},
        {{0.780076f, 0.0260025f, 0.598059f}, 0.182018f},
        {{-1.0f, 0.0f, 0.3f}, 0.4f}};
    CubicHermiteQuaternion b{
        {{5.0f, 0.3f, 1.1f}, 0.5f},
        {{-0.711568f, 0.391362f, 0.355784f}, 0.462519f},
        {{1.5f, 0.3f, 17.0f}, -7.0f}};

    CORRADE_COMPARE(Math::slerp(a, b, 0.0f), a.point());
    CORRADE_COMPARE(Math::slerp(a, b, 1.0f), b.point());

    Quaternion expected035{{0.308542f, 0.265288f, 0.790272f}, 0.458142f};
    CORRADE_COMPARE(Math::slerp(a, b, 0.35f), expected035);
    CORRADE_COMPARE(Math::slerp(a.point(), b.point(), 0.35f), expected035);
    CORRADE_VERIFY(Math::slerp(a, b, 0.35f).isNormalized());

    Quaternion expected08{{-0.442885f, 0.410928f, 0.584814f}, 0.541279f};
    CORRADE_COMPARE(Math::slerp(a, b, 0.8f), expected08);
    CORRADE_COMPARE(Math::slerp(a.point(), b.point(), 0.8f), expected08);
    CORRADE_VERIFY(Math::slerp(a, b, 0.8f).isNormalized());
}

void CubicHermiteTest::slerpQuaternionNotNormalized() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    /* This one should not assert as the default constructor should create
       identity point */
    Math::slerp(CubicHermiteQuaternion{}, {}, 0.3f);

    /* These will, tho */
    CubicHermiteQuaternion a{{}, Quaternion{}*2.0f, {}};
    Math::slerp({}, a, 0.3f);
    Math::slerp(a, {}, 0.3f);
    CORRADE_COMPARE(out.str(),
        "Math::slerp(): quaternions Quaternion({0, 0, 0}, 1) and Quaternion({0, 0, 0}, 2) are not normalized\n"
        "Math::slerp(): quaternions Quaternion({0, 0, 0}, 2) and Quaternion({0, 0, 0}, 1) are not normalized\n");
}

void CubicHermiteTest::slerpQuaternionShortestPath() {
    /* Values from QuaternionTest::slerpShortestPath() */
    CubicHermiteQuaternion a{
        {{2.0f, 1.5f, 0.3f}, 1.1f},
        Quaternion::rotation(0.0_degf, Vector3::zAxis()),
        {{-1.0f, 0.0f, 0.3f}, 0.4f}};
    CubicHermiteQuaternion b{
        {{5.0f, 0.3f, 1.1f}, 0.5f},
        Quaternion::rotation(225.0_degf, Vector3::zAxis()),
        {{1.5f, 0.3f, 17.0f}, -7.0f}};

    Quaternion slerp = Math::slerp(a, b, 0.25f);
    Quaternion slerpShortestPath = Math::slerpShortestPath(a, b, 0.25f);
    CORRADE_COMPARE(slerp.axis(), Vector3::zAxis());
    CORRADE_COMPARE(slerpShortestPath.axis(), Vector3::zAxis());
    CORRADE_COMPARE(slerp.angle(), 56.25_degf);
    CORRADE_COMPARE(slerpShortestPath.angle(), 326.25_degf);
    CORRADE_COMPARE(slerp, (Quaternion{{0.0f, 0.0f, 0.471397f}, 0.881921f}));

    Quaternion expected{{0.0f, 0.0f, 0.290285f}, -0.95694f};
    CORRADE_COMPARE(slerpShortestPath, expected);
    CORRADE_COMPARE(Math::slerpShortestPath(a.point(), b.point(), 0.25f), expected);
}

void CubicHermiteTest::slerpQuaternionShortestPathNotNormalized() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    /* This one should not assert as the default constructor should create
       identity point */
    Math::slerpShortestPath(CubicHermiteQuaternion{}, {}, 0.3f);

    /* These will, tho */
    CubicHermiteQuaternion a{{}, Quaternion{}*2.0f, {}};
    Math::slerpShortestPath({}, a, 0.3f);
    Math::slerpShortestPath(a, {}, 0.3f);
    CORRADE_COMPARE(out.str(),
        "Math::slerpShortestPath(): quaternions Quaternion({0, 0, 0}, 1) and Quaternion({0, 0, 0}, 2) are not normalized\n"
        "Math::slerpShortestPath(): quaternions Quaternion({0, 0, 0}, 2) and Quaternion({0, 0, 0}, 1) are not normalized\n");
}

void CubicHermiteTest::splerpScalar() {
    CubicHermite1D a{2.0f, 3.0f, -1.0f};
    CubicHermite1D b{5.0f, -2.0f, 1.5f};

    CORRADE_COMPARE(Math::splerp(a, b, 0.0f), 3.0f);
    CORRADE_COMPARE(Math::splerp(a, b, 1.0f), -2.0f);

    CORRADE_COMPARE(Math::splerp(a, b, 0.35f), 1.04525f);
    CORRADE_COMPARE(Math::splerp(a, b, 0.8f), -2.152f);
}

void CubicHermiteTest::splerpVector() {
    CubicHermite2D a{{2.0f, 1.5f}, {3.0f, 0.1f}, {-1.0f, 0.0f}};
    CubicHermite2D b{{5.0f, 0.3f}, {-2.0f, 1.1f}, {1.5f, 0.3f}};

    CORRADE_COMPARE(Math::splerp(a, b, 0.0f), a.point());
    CORRADE_COMPARE(Math::splerp(a, b, 1.0f), b.point());

    CORRADE_COMPARE(Math::splerp(a, b, 0.35f), (Vector2{1.04525f, 0.357862f}));
    CORRADE_COMPARE(Math::splerp(a, b, 0.8f), (Vector2{-2.152f, 0.9576f}));
}

void CubicHermiteTest::splerpVectorFromBezier() {
    /* Taken from BezierTest::valueCubic() */
    CubicBezier2D bezier{Vector2{0.0f, 0.0f}, Vector2{10.0f, 15.0f}, Vector2{20.0f, 4.0f}, Vector2{5.0f, -20.0f}};
    auto a = CubicHermite2D::fromBezier({Vector2{}, Vector2{}, Vector2{}, bezier[0]}, bezier);
    auto b = CubicHermite2D::fromBezier(bezier, {bezier[3], Vector2{}, Vector2{}, Vector2{}});

    CORRADE_COMPARE(bezier.value(0.0f), (Vector2{0.0f, 0.0f}));
    CORRADE_COMPARE(Math::splerp(a, b, 0.0f), (Vector2{0.0f, 0.0f}));

    CORRADE_COMPARE(bezier.value(0.2f), (Vector2{5.8f, 5.984f}));
    CORRADE_COMPARE(Math::splerp(a, b, 0.2f), (Vector2{5.8f, 5.984f}));

    CORRADE_COMPARE(bezier.value(0.5f), (Vector2{11.875f, 4.625f}));
    CORRADE_COMPARE(Math::splerp(a, b, 0.5f), (Vector2{11.875f, 4.625f}));

    CORRADE_COMPARE(bezier.value(1.0f), (Vector2{5.0f, -20.0f}));
    CORRADE_COMPARE(Math::splerp(a, b, 1.0f), (Vector2{5.0f, -20.0f}));
}

void CubicHermiteTest::splerpComplex() {
    CubicHermiteComplex a{{2.0f, 1.5f}, {0.999445f, 0.0333148f}, {-1.0f, 0.0f}};
    CubicHermiteComplex b{{5.0f, 0.3f}, {-0.876216f, 0.481919f}, {1.5f, 0.3f}};

    CORRADE_COMPARE(Math::splerp(a, b, 0.0f), a.point());
    CORRADE_COMPARE(Math::splerp(a, b, 1.0f), b.point());

    CORRADE_COMPARE(Math::splerp(a, b, 0.35f), (Complex{-0.483504f, 0.875342f}));
    CORRADE_VERIFY(Math::splerp(a, b, 0.35f).isNormalized());

    CORRADE_COMPARE(Math::splerp(a, b, 0.8f), (Complex{-0.95958f, 0.281435f}));
    CORRADE_VERIFY(Math::splerp(a, b, 0.8f).isNormalized());
}

void CubicHermiteTest::splerpComplexNotNormalized() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    /* This one should not assert as the default constructor should create
       identity point */
    CORRADE_COMPARE(Math::splerp(CubicHermiteComplex{}, {}, 0.3f), Complex{});

    /* These will, tho */
    CubicHermiteComplex a{{}, Complex{}*2.0f, {}};
    Math::splerp({}, a, 0.3f);
    Math::splerp(a, {}, 0.3f);
    CORRADE_COMPARE(out.str(),
        "Math::splerp(): complex spline points Complex(1, 0) and Complex(2, 0) are not normalized\n"
        "Math::splerp(): complex spline points Complex(2, 0) and Complex(1, 0) are not normalized\n");
}

void CubicHermiteTest::splerpQuaternion() {
    CubicHermiteQuaternion a{
        {{2.0f, 1.5f, 0.3f}, 1.1f},
        {{0.780076f, 0.0260025f, 0.598059f}, 0.182018f},
        {{-1.0f, 0.0f, 0.3f}, 0.4f}};
    CubicHermiteQuaternion b{
        {{5.0f, 0.3f, 1.1f}, 0.5f},
        {{-0.711568f, 0.391362f, 0.355784f}, 0.462519f},
        {{1.5f, 0.3f, 17.0f}, -7.0f}};

    CORRADE_COMPARE(Math::splerp(a, b, 0.0f), a.point());
    CORRADE_COMPARE(Math::splerp(a, b, 1.0f), b.point());

    CORRADE_COMPARE(Math::splerp(a, b, 0.35f), (Quaternion{{-0.309862f, 0.174831f, 0.809747f}, 0.466615f}));
    CORRADE_VERIFY(Math::splerp(a, b, 0.35f).isNormalized());

    CORRADE_COMPARE(Math::splerp(a, b, 0.8f), (Quaternion{{-0.911408f, 0.23368f, 0.185318f}, 0.283524f}));
    CORRADE_VERIFY(Math::splerp(a, b, 0.8f).isNormalized());
}

void CubicHermiteTest::splerpQuaternionNotNormalized() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    /* This one should not assert as the default constructor should create
       identity point */
    Math::splerp(CubicHermiteQuaternion{}, {}, 0.3f);

    /* These will, tho */
    CubicHermiteQuaternion a{{}, Quaternion{}*2.0f, {}};
    Math::splerp({}, a, 0.3f);
    Math::splerp(a, {}, 0.3f);
    CORRADE_COMPARE(out.str(),
        "Math::splerp(): quaternion spline points Quaternion({0, 0, 0}, 1) and Quaternion({0, 0, 0}, 2) are not normalized\n"
        "Math::splerp(): quaternion spline points Quaternion({0, 0, 0}, 2) and Quaternion({0, 0, 0}, 1) are not normalized\n");
}

void CubicHermiteTest::strictWeakOrdering() {
    StrictWeakOrdering o;
    const CubicHermite1D a{1.0f, 2.0f, 3.0f};
    const CubicHermite1D b{2.0f, 3.0f, 4.0f};
    const CubicHermite1D c{1.0f, 2.0f, 4.0f};
    const CubicHermite1D d{1.0f, 5.0f, 4.0f};

    CORRADE_VERIFY( o(a, b));
    CORRADE_VERIFY(!o(b, a));
    CORRADE_VERIFY( o(a, c));
    CORRADE_VERIFY(!o(c, a));
    CORRADE_VERIFY( o(c, b));
    CORRADE_VERIFY(!o(b, c));
    CORRADE_VERIFY( o(a, d));
    CORRADE_VERIFY(!o(d, a));

    CORRADE_VERIFY(!o(a, a));
}

void CubicHermiteTest::debugScalar() {
    std::ostringstream out;
    Debug{&out} << CubicHermite1D{2.0f, 3.0f, -1.0f};
    CORRADE_COMPARE(out.str(), "CubicHermite(2, 3, -1)\n");
}

void CubicHermiteTest::debugVector() {
    std::ostringstream out;
    Debug{&out} << CubicHermite2D{{2.0f, 1.5f}, {3.0f, 0.1f}, {-1.0f, 0.0f}};
    CORRADE_COMPARE(out.str(), "CubicHermite(Vector(2, 1.5), Vector(3, 0.1), Vector(-1, 0))\n");
}

void CubicHermiteTest::debugComplex() {
    std::ostringstream out;
    Debug{&out} << CubicHermiteComplex{{2.0f, 1.5f}, {3.0f, 0.1f}, {-1.0f, 0.0f}};
    CORRADE_COMPARE(out.str(), "CubicHermite(Complex(2, 1.5), Complex(3, 0.1), Complex(-1, 0))\n");
}

void CubicHermiteTest::debugQuaternion() {
    std::ostringstream out;
    Debug{&out} << CubicHermiteQuaternion{
        {{2.0f, 1.5f, 0.3f}, 1.1f},
        {{3.0f, 0.1f, 2.3f}, 0.7f},
        {{-1.0f, 0.0f, 0.3f}, 0.4f}};
    CORRADE_COMPARE(out.str(), "CubicHermite(Quaternion({2, 1.5, 0.3}, 1.1), Quaternion({3, 0.1, 2.3}, 0.7), Quaternion({-1, 0, 0.3}, 0.4))\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::CubicHermiteTest)
