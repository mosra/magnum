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

#include <new>
#include <Corrade/Containers/String.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Numeric.h>

#include "Magnum/Math/Matrix4.h"
#include "Magnum/Math/StrictWeakOrdering.h"

struct Mat4 {
    float a[16];
};

namespace Magnum { namespace Math {

namespace Implementation {

template<> struct RectangularMatrixConverter<4, 4, float, Mat4> {
    constexpr static RectangularMatrix<4, 4, Float> from(const Mat4& other) {
        return RectangularMatrix<4, 4, Float>(
            Vector<4, Float>(other.a[0], other.a[1], other.a[2], other.a[3]),
            Vector<4, Float>(other.a[4], other.a[5], other.a[6], other.a[7]),
            Vector<4, Float>(other.a[8], other.a[9], other.a[10], other.a[11]),
            Vector<4, Float>(other.a[12], other.a[13], other.a[14], other.a[15]));
    }

    constexpr static Mat4 to(const RectangularMatrix<4, 4, Float>& other) {
        return Mat4{{other[0][0], other[0][1], other[0][2], other[0][3],
                     other[1][0], other[1][1], other[1][2], other[1][3],
                     other[2][0], other[2][1], other[2][2], other[2][3],
                     other[3][0], other[3][1], other[3][2], other[3][3]}};
    }
};

}

namespace Test { namespace {

struct Matrix4Test: TestSuite::Tester {
    explicit Matrix4Test();

    void construct();
    void constructIdentity();
    void constructZero();
    void constructNoInit();
    void constructOneValue();
    void constructArray();
    void constructArrayRvalue();
    void constructConversion();
    void constructFromDifferentSize();
    void constructCopy();
    void convert();

    void isRigidTransformation();

    void translation();
    void scaling();
    void rotation();
    void rotationNotNormalized();
    void rotationX();
    void rotationY();
    void rotationZ();
    void reflection();
    void reflectionNotNormalized();
    void reflectionIsScaling();
    void shearingXY();
    void shearingXZ();
    void shearingYZ();
    void orthographicProjection();
    void orthographicProjectionOffCenter();
    void perspectiveProjection();
    void perspectiveProjectionInfiniteFar();
    void perspectiveProjectionFov();
    void perspectiveProjectionFovInfiniteFar();
    void perspectiveProjectionOffCenter();
    void perspectiveProjectionOffCenterInfiniteFar();
    void lookAt();

    void fromParts();
    void rotationScalingPart();
    void rotationShearPart();
    void rotationPart();
    void rotationPartNotOrthogonal();
    void rotationNormalizedPart();
    void rotationNormalizedPartNotOrthogonal();
    void scalingPart();
    void rotationScalingPartNegative();
    void uniformScalingPart();
    void uniformScalingPartNotUniform();
    void normalMatrixPart();
    void vectorParts();
    void orthographicProjectionParts();
    void perspectiveProjectionParts();
    void perspectiveProjectionInfiniteFarParts();
    void invertedRigid();
    void invertedRigidNotRigid();
    void transform();
    void transformProjection();

    void strictWeakOrdering();

    void debug();
};

/* What's a typedef and not a using differs from the typedefs in root Magnum
   namespace, or is not present there at all */
using Magnum::Deg;
using Magnum::Rad;
using Magnum::Matrix2x2;
using Magnum::Matrix2x3;
using Magnum::Matrix3x3;
using Magnum::Matrix4;
typedef Math::Matrix4<Int> Matrix4i;
using Magnum::Vector2;
using Magnum::Vector3;
using Magnum::Vector4;
using Magnum::Constants;

Matrix4Test::Matrix4Test() {
    addTests({&Matrix4Test::construct,
              &Matrix4Test::constructIdentity,
              &Matrix4Test::constructZero,
              &Matrix4Test::constructNoInit,
              &Matrix4Test::constructOneValue,
              &Matrix4Test::constructArray,
              &Matrix4Test::constructArrayRvalue,
              &Matrix4Test::constructConversion,
              &Matrix4Test::constructFromDifferentSize,
              &Matrix4Test::constructCopy,
              &Matrix4Test::convert,

              &Matrix4Test::isRigidTransformation,

              &Matrix4Test::translation,
              &Matrix4Test::scaling,
              &Matrix4Test::rotation,
              &Matrix4Test::rotationNotNormalized,
              &Matrix4Test::rotationX,
              &Matrix4Test::rotationY,
              &Matrix4Test::rotationZ,
              &Matrix4Test::reflection,
              &Matrix4Test::reflectionNotNormalized,
              &Matrix4Test::reflectionIsScaling,
              &Matrix4Test::shearingXY,
              &Matrix4Test::shearingXZ,
              &Matrix4Test::shearingYZ,
              &Matrix4Test::orthographicProjection,
              &Matrix4Test::orthographicProjectionOffCenter,
              &Matrix4Test::perspectiveProjection,
              &Matrix4Test::perspectiveProjectionInfiniteFar,
              &Matrix4Test::perspectiveProjectionFov,
              &Matrix4Test::perspectiveProjectionFovInfiniteFar,
              &Matrix4Test::perspectiveProjectionOffCenter,
              &Matrix4Test::perspectiveProjectionOffCenterInfiniteFar,
              &Matrix4Test::lookAt,

              &Matrix4Test::fromParts,
              &Matrix4Test::rotationScalingPart,
              &Matrix4Test::rotationShearPart,
              &Matrix4Test::rotationPart,
              &Matrix4Test::rotationPartNotOrthogonal,
              &Matrix4Test::rotationNormalizedPart,
              &Matrix4Test::rotationNormalizedPartNotOrthogonal,
              &Matrix4Test::scalingPart,
              &Matrix4Test::rotationScalingPartNegative,
              &Matrix4Test::uniformScalingPart,
              &Matrix4Test::uniformScalingPartNotUniform,
              &Matrix4Test::normalMatrixPart,
              &Matrix4Test::vectorParts,
              &Matrix4Test::orthographicProjectionParts,
              &Matrix4Test::perspectiveProjectionParts,
              &Matrix4Test::perspectiveProjectionInfiniteFarParts,
              &Matrix4Test::invertedRigid,
              &Matrix4Test::invertedRigidNotRigid,
              &Matrix4Test::transform,
              &Matrix4Test::transformProjection,

              &Matrix4Test::strictWeakOrdering,

              &Matrix4Test::debug});
}

using namespace Literals;

void Matrix4Test::construct() {
    constexpr Matrix4 a = {{3.0f,  5.0f, 8.0f, -3.0f},
                           {4.5f,  4.0f, 7.0f,  2.0f},
                           {1.0f,  2.0f, 3.0f, -1.0f},
                           {7.9f, -1.0f, 8.0f, -1.5f}};
    CORRADE_COMPARE(a, Matrix4({3.0f,  5.0f, 8.0f, -3.0f},
                               {4.5f,  4.0f, 7.0f,  2.0f},
                               {1.0f,  2.0f, 3.0f, -1.0f},
                               {7.9f, -1.0f, 8.0f, -1.5f}));

    CORRADE_VERIFY(std::is_nothrow_constructible<Matrix4, Vector4, Vector4, Vector4, Vector4>::value);
}

void Matrix4Test::constructIdentity() {
    constexpr Matrix4 identity;
    constexpr Matrix4 identity2{IdentityInit};
    constexpr Matrix4 identity3{IdentityInit, 4.0f};

    Matrix4 identityExpected({1.0f, 0.0f, 0.0f, 0.0f},
                             {0.0f, 1.0f, 0.0f, 0.0f},
                             {0.0f, 0.0f, 1.0f, 0.0f},
                             {0.0f, 0.0f, 0.0f, 1.0f});

    Matrix4 identity3Expected({4.0f, 0.0f, 0.0f, 0.0f},
                              {0.0f, 4.0f, 0.0f, 0.0f},
                              {0.0f, 0.0f, 4.0f, 0.0f},
                              {0.0f, 0.0f, 0.0f, 4.0f});

    CORRADE_COMPARE(identity, identityExpected);
    CORRADE_COMPARE(identity2, identityExpected);
    CORRADE_COMPARE(identity3, identity3Expected);

    CORRADE_VERIFY(std::is_nothrow_default_constructible<Matrix4>::value);
    CORRADE_VERIFY(std::is_nothrow_constructible<Matrix4, IdentityInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<IdentityInitT, Matrix4>::value);
}

void Matrix4Test::constructZero() {
    constexpr Matrix4 a{ZeroInit};
    CORRADE_COMPARE(a, Matrix4({0.0f, 0.0f, 0.0f, 0.0f},
                               {0.0f, 0.0f, 0.0f, 0.0f},
                               {0.0f, 0.0f, 0.0f, 0.0f},
                               {0.0f, 0.0f, 0.0f, 0.0f}));

    CORRADE_VERIFY(std::is_nothrow_constructible<Matrix4, ZeroInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<ZeroInitT, Matrix4>::value);
}

void Matrix4Test::constructNoInit() {
    Matrix4 a = {{3.0f,  5.0f, 8.0f, -3.0f},
                 {4.5f,  4.0f, 7.0f,  2.0f},
                 {1.0f,  2.0f, 3.0f, -1.0f},
                 {7.9f, -1.0f, 8.0f, -1.5f}};
    new(&a) Matrix4{Magnum::NoInit};
    {
        /* Explicitly check we're not on Clang because certain Clang-based IDEs
           inherit __GNUC__ if GCC is used instead of leaving it at 4 like
           Clang itself does */
        #if defined(CORRADE_TARGET_GCC) && !defined(CORRADE_TARGET_CLANG) && __GNUC__*100 + __GNUC_MINOR__ >= 601 && __OPTIMIZE__
        CORRADE_EXPECT_FAIL("GCC 6.1+ misoptimizes and overwrites the value.");
        #endif
        CORRADE_COMPARE(a, Matrix4({3.0f,  5.0f, 8.0f, -3.0f},
                                   {4.5f,  4.0f, 7.0f,  2.0f},
                                   {1.0f,  2.0f, 3.0f, -1.0f},
                                   {7.9f, -1.0f, 8.0f, -1.5f}));
    }

    CORRADE_VERIFY(std::is_nothrow_constructible<Matrix4, Magnum::NoInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<Magnum::NoInitT, Matrix4>::value);
}

void Matrix4Test::constructOneValue() {
    constexpr Matrix4 a{1.5f};
    CORRADE_COMPARE(a, (Matrix4{{1.5f, 1.5f, 1.5f, 1.5f},
                                {1.5f, 1.5f, 1.5f, 1.5f},
                                {1.5f, 1.5f, 1.5f, 1.5f},
                                {1.5f, 1.5f, 1.5f, 1.5f}}));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!std::is_convertible<Float, Matrix4>::value);

    CORRADE_VERIFY(std::is_nothrow_constructible<Matrix4, Float>::value);
}

void Matrix4Test::constructArray() {
    float data[4][4]{
        {3.0f,  5.0f, 8.0f, -3.0f},
        {4.5f,  4.0f, 7.0f,  2.0f},
        {1.0f,  2.0f, 3.0f, -1.0f},
        {7.9f, -1.0f, 8.0f, -1.5f}
    };
    Matrix4 a{data};
    CORRADE_COMPARE(a, (Matrix4{{3.0f,  5.0f, 8.0f, -3.0f},
                                {4.5f,  4.0f, 7.0f,  2.0f},
                                {1.0f,  2.0f, 3.0f, -1.0f},
                                {7.9f, -1.0f, 8.0f, -1.5f}}));

    constexpr float cdata[4][4]{
        {3.0f,  5.0f, 8.0f, -3.0f},
        {4.5f,  4.0f, 7.0f,  2.0f},
        {1.0f,  2.0f, 3.0f, -1.0f},
        {7.9f, -1.0f, 8.0f, -1.5f}
    };
    constexpr Matrix4 ca{cdata};
    CORRADE_COMPARE(ca, (Matrix4{{3.0f,  5.0f, 8.0f, -3.0f},
                                 {4.5f,  4.0f, 7.0f,  2.0f},
                                 {1.0f,  2.0f, 3.0f, -1.0f},
                                 {7.9f, -1.0f, 8.0f, -1.5f}}));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!std::is_convertible<float[4][4], Matrix4>::value);

    CORRADE_VERIFY(std::is_nothrow_constructible<Matrix4, float[4][4]>::value);

    /* See RectangularMatrixTest::constructArray() for details */
    #if 0
    float data43[4][3]{
        {3.0f,  5.0f, 8.0f},
        {4.5f,  4.0f, 7.0f},
        {1.0f,  2.0f, 3.0f},
        {7.9f, -1.0f, 8.0f}
    };
    float data45[4][5]{
        {3.0f,  5.0f, 8.0f, -3.0f, 0.3f},
        {4.5f,  4.0f, 7.0f,  2.0f, 0.3f},
        {1.0f,  2.0f, 3.0f, -1.0f, 0.3f},
        {7.9f, -1.0f, 8.0f, -1.5f, 0.3f}
    };
    float data14[1][4]{
        {3.0f, 5.0f, 8.0f, -3.0f},
    };
    float data54[5][4]{
        {3.0f, 5.0f, 8.0f, -3.0f},
        {4.5f, 4.0f, 7.0f,  2.0f},
        {4.5f, 4.0f, 7.0f,  2.0f},
        {4.5f, 4.0f, 7.0f,  2.0f},
        {3.0f, 7.0f, 0.0f,  1.0f}
    };
    Matrix4 b{data43};
    Matrix4 c{data45};
    Matrix4 d{data14};
    Matrix4 e{data54};
    #endif
}

void Matrix4Test::constructArrayRvalue() {
    /* In this case the constructor already has concrete types, so the array
       constructor doesn't really add anything to it */
    Matrix4 a{{{3.0f,  5.0f, 8.0f, -3.0f},
               {4.5f,  4.0f, 7.0f,  2.0f},
               {1.0f,  2.0f, 3.0f, -1.0f},
               {7.9f, -1.0f, 8.0f, -1.5f}}};
    CORRADE_COMPARE(a, (Matrix4{{3.0f,  5.0f, 8.0f, -3.0f},
                                {4.5f,  4.0f, 7.0f,  2.0f},
                                {1.0f,  2.0f, 3.0f, -1.0f},
                                {7.9f, -1.0f, 8.0f, -1.5f}}));

    /* Unfortunately on MSVC (even 2022) this leads to
        error C2131: expression did not evaluate to a constant
        note: failure was caused by out of range index 3; allowed range is 0 <= index < 2
       and similarly in other tests. Not sure where that comes from, for Vector
       this all works, constructArray() above works, and the GCC 4.8 workaround
       with fixed size doesn't help here. */
    #ifndef CORRADE_TARGET_MSVC
    constexpr Matrix4 ca{{{3.0f,  5.0f, 8.0f, -3.0f},
                          {4.5f,  4.0f, 7.0f,  2.0f},
                          {1.0f,  2.0f, 3.0f, -1.0f},
                          {7.9f, -1.0f, 8.0f, -1.5f}}};
    CORRADE_COMPARE(ca, (Matrix4{{3.0f,  5.0f, 8.0f, -3.0f},
                                 {4.5f,  4.0f, 7.0f,  2.0f},
                                 {1.0f,  2.0f, 3.0f, -1.0f},
                                 {7.9f, -1.0f, 8.0f, -1.5f}}));
    #endif

    /* See RectangularMatrixTest::constructArrayRvalue() for details */
    #if 0
    Matrix4 c{{{3.0f,  5.0f, 8.0f, -3.0f, 0.3f},
               {4.5f,  4.0f, 7.0f,  2.0f, 0.3f},
               {1.0f,  2.0f, 3.0f, -1.0f, 0.3f},
               {7.9f, -1.0f, 8.0f, -1.5f, 0.3f}}};
    Matrix4 e{{{3.0f, 5.0f, 8.0f, -3.0f},
               {4.5f, 4.0f, 7.0f,  2.0f},
               {4.5f, 4.0f, 7.0f,  2.0f},
               {4.5f, 4.0f, 7.0f,  2.0f},
               {3.0f, 7.0f, 0.0f,  1.0f}}};
    #endif
    #if 0 || (defined(CORRADE_TARGET_GCC) && !defined(CORRADE_TARGET_CLANG) && __GNUC__ < 5)
    CORRADE_WARN("Creating a Matrix from a smaller array isn't an error on GCC 4.8.");
    Matrix4 b{{{3.0f,  5.0f, 8.0f},
               {4.5f,  4.0f, 7.0f},
               {1.0f,  2.0f, 3.0f},
               {7.9f, -1.0f, 8.0f}}};
    Matrix4 d{{{3.0f, 5.0f, 8.0f, -3.0f}}};
    #endif
}

void Matrix4Test::constructConversion() {
    constexpr Matrix4 a({3.0f,  5.0f, 8.0f, -3.0f},
                        {4.5f,  4.0f, 7.0f,  2.0f},
                        {1.0f,  2.0f, 3.0f, -1.0f},
                        {7.9f, -1.0f, 8.0f, -1.5f});
    constexpr Matrix4i b(a);
    CORRADE_COMPARE(b, Matrix4i({3,  5, 8, -3},
                                {4,  4, 7,  2},
                                {1,  2, 3, -1},
                                {7, -1, 8, -1}));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!std::is_convertible<Matrix4, Matrix4i>::value);

    CORRADE_VERIFY(std::is_nothrow_constructible<Matrix4, Matrix4i>::value);
}

void Matrix4Test::constructFromDifferentSize() {
    /* Tested thoroughly in RectangularMatrixTest, verify just that the args
       are propagated correctly (cols, rows, zero/identity, identity value) */
    constexpr Matrix2x3 a{Vector3{3.0f,  5.0f, 8.0f},
                          Vector3{4.5f,  4.0f, 7.0f}};
    constexpr Matrix4 expectedIdentity{{3.0f, 5.0f, 8.0f, 0.0f},
                                       {4.5f, 4.0f, 7.0f, 0.0f},
                                       {0.0f, 0.0f, 0.5f, 0.0f},
                                       {0.0f, 0.0f, 0.0f, 0.5f}};
    constexpr Matrix4 expectedZero{{3.0f, 5.0f, 8.0f, 0.0f},
                                   {4.5f, 4.0f, 7.0f, 0.0f},
                                   {0.0f, 0.0f, 0.0f, 0.0f},
                                   {0.0f, 0.0f, 0.0f, 0.0f}};

    constexpr Matrix4 identity1{a, 0.5f};
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* It ICEs, haha */
    constexpr
    #endif
    Matrix4 identity2{IdentityInit, a, 0.5f};
    CORRADE_COMPARE(identity1, expectedIdentity);
    CORRADE_COMPARE(identity2, expectedIdentity);
    CORRADE_COMPARE((Matrix4{a, 0.5f}), expectedIdentity);
    CORRADE_COMPARE((Matrix4{IdentityInit, a, 0.5f}), expectedIdentity);

    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* It ICEs, haha */
    constexpr
    #endif
    Matrix4 zero{ZeroInit, a};
    CORRADE_COMPARE(zero, expectedZero);
    CORRADE_COMPARE((Matrix4{ZeroInit, a}), expectedZero);
}

void Matrix4Test::constructCopy() {
    constexpr Matrix<4, Float> a(Vector<4, Float>(3.0f,  5.0f, 8.0f, -3.0f),
                                 Vector<4, Float>(4.5f,  4.0f, 7.0f,  2.0f),
                                 Vector<4, Float>(1.0f,  2.0f, 3.0f, -1.0f),
                                 Vector<4, Float>(7.9f, -1.0f, 8.0f, -1.5f));
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* Why can't be copy constexpr? */
    constexpr
    #endif
    Matrix4 b(a);
    CORRADE_COMPARE(b, Matrix4({3.0f,  5.0f, 8.0f, -3.0f},
                               {4.5f,  4.0f, 7.0f,  2.0f},
                               {1.0f,  2.0f, 3.0f, -1.0f},
                               {7.9f, -1.0f, 8.0f, -1.5f}));

    #ifndef CORRADE_NO_STD_IS_TRIVIALLY_TRAITS
    CORRADE_VERIFY(std::is_trivially_copy_constructible<Matrix4>::value);
    CORRADE_VERIFY(std::is_trivially_copy_assignable<Matrix4>::value);
    #endif
    CORRADE_VERIFY(std::is_nothrow_copy_constructible<Matrix4>::value);
    CORRADE_VERIFY(std::is_nothrow_copy_assignable<Matrix4>::value);
}

void Matrix4Test::convert() {
    constexpr Mat4 a{{3.0f,  5.0f, 8.0f, -3.0f,
                      4.5f,  4.0f, 7.0f,  2.0f,
                      1.0f,  2.0f, 3.0f, -1.0f,
                      7.9f, -1.0f, 8.0f, -1.5f}};
    constexpr Matrix4 b({3.0f,  5.0f, 8.0f, -3.0f},
                        {4.5f,  4.0f, 7.0f,  2.0f},
                        {1.0f,  2.0f, 3.0f, -1.0f},
                        {7.9f, -1.0f, 8.0f, -1.5f});

    constexpr Matrix4 c(b);
    CORRADE_COMPARE(c, b);

    /* https://developercommunity.visualstudio.com/t/MSVC-1933-fails-to-compile-valid-code-u/10185268 */
    #if defined(CORRADE_TARGET_MSVC) && CORRADE_CXX_STANDARD >= 202002L
    constexpr auto d = Mat4(b);
    #else
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* Why can't be conversion constexpr? */
    constexpr
    #endif
    Mat4 d(b);
    #endif
    for(std::size_t i = 0; i != 16; ++i)
        CORRADE_COMPARE(d.a[i], a.a[i]);

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!std::is_convertible<Mat4, Matrix4>::value);
    CORRADE_VERIFY(!std::is_convertible<Matrix4, Mat4>::value);
}

void Matrix4Test::isRigidTransformation() {
    CORRADE_VERIFY(!Matrix4({1.0f, 0.0f, 0.0f, 0.0f},
                            {0.0f, 1.0f, 0.0f, 0.0f},
                            {0.0f, 0.1f, 1.0f, 0.0f},
                            {5.0f, 4.0f, 0.5f, 1.0f}).isRigidTransformation());
    CORRADE_VERIFY(!Matrix4({1.0f, 0.0f, 0.0f, 0.0f},
                            {0.1f, 1.0f, 0.0f, 0.0f},
                            {0.0f, 0.0f, 1.0f, 1.0f},
                            {5.0f, 4.0f, 0.5f, 0.0f}).isRigidTransformation());
    CORRADE_VERIFY(Matrix4({1.0f, 0.0f, 0.0f, 0.0f},
                           {0.0f, 1.0f, 0.0f, 0.0f},
                           {0.0f, 0.0f, 1.0f, 0.0f},
                           {5.0f, 4.0f, 0.5f, 1.0f}).isRigidTransformation());
}

void Matrix4Test::translation() {
    constexpr Matrix4 a = Matrix4::translation({3.0f, 1.0f, 2.0f});
    CORRADE_COMPARE(a, Matrix4({1.0f, 0.0f, 0.0f, 0.0f},
                               {0.0f, 1.0f, 0.0f, 0.0f},
                               {0.0f, 0.0f, 1.0f, 0.0f},
                               {3.0f, 1.0f, 2.0f, 1.0f}));
}

void Matrix4Test::scaling() {
    constexpr Matrix4 a = Matrix4::scaling({3.0f, 1.5f, 2.0f});
    CORRADE_COMPARE(a, Matrix4({3.0f, 0.0f, 0.0f, 0.0f},
                               {0.0f, 1.5f, 0.0f, 0.0f},
                               {0.0f, 0.0f, 2.0f, 0.0f},
                               {0.0f, 0.0f, 0.0f, 1.0f}));
}

void Matrix4Test::rotation() {
    Matrix4 matrix({ 0.35612202f, -0.80181062f, 0.47987163f, 0.0f},
                   { 0.47987163f,  0.59757626f,  0.6423596f, 0.0f},
                   {-0.80181062f,  0.00151846f, 0.59757626f, 0.0f},
                   {        0.0f,         0.0f,        0.0f, 1.0f});
    CORRADE_COMPARE(Matrix4::rotation(-74.0_degf, Vector3(-1.0f, 2.0f, 2.0f).normalized()), matrix);
}

void Matrix4Test::rotationNotNormalized() {
    CORRADE_SKIP_IF_NO_DEBUG_ASSERT();

    Containers::String out;
    Error redirectError{&out};

    Matrix4::rotation(-74.0_degf, {-1.0f, 2.0f, 2.0f});
    CORRADE_COMPARE(out, "Math::Matrix4::rotation(): axis Vector(-1, 2, 2) is not normalized\n");
}

void Matrix4Test::rotationX() {
    Matrix4 matrix({1.0f,         0.0f,        0.0f, 0.0f},
                   {0.0f,  0.90096887f, 0.43388374f, 0.0f},
                   {0.0f, -0.43388374f, 0.90096887f, 0.0f},
                   {0.0f,         0.0f,        0.0f, 1.0f});
    CORRADE_COMPARE(Matrix4::rotation(Rad(Constants::pi()/7), Vector3::xAxis()), matrix);
    CORRADE_COMPARE(Matrix4::rotationX(Rad(Constants::pi()/7)), matrix);
}

void Matrix4Test::rotationY() {
    Matrix4 matrix({0.90096887f, 0.0f, -0.43388374f, 0.0f},
                   {       0.0f, 1.0f,         0.0f, 0.0f},
                   {0.43388374f, 0.0f,  0.90096887f, 0.0f},
                   {       0.0f, 0.0f,         0.0f, 1.0f});
    CORRADE_COMPARE(Matrix4::rotation(Rad(Constants::pi()/7), Vector3::yAxis()), matrix);
    CORRADE_COMPARE(Matrix4::rotationY(Rad(Constants::pi()/7)), matrix);
}

void Matrix4Test::rotationZ() {
    Matrix4 matrix({ 0.90096887f, 0.43388374f, 0.0f, 0.0f},
                   {-0.43388374f, 0.90096887f, 0.0f, 0.0f},
                   {        0.0f,        0.0f, 1.0f, 0.0f},
                   {        0.0f,        0.0f, 0.0f, 1.0f});
    CORRADE_COMPARE(Matrix4::rotation(Rad(Constants::pi()/7), Vector3::zAxis()), matrix);
    CORRADE_COMPARE(Matrix4::rotationZ(Rad(Constants::pi()/7)), matrix);
}

void Matrix4Test::reflection() {
    Vector3 normal(-1.0f, 2.0f, 2.0f);
    Matrix4 actual = Matrix4::reflection(normal.normalized());
    Matrix4 expected({0.777778f,  0.444444f,  0.444444f, 0.0f},
                     {0.444444f,  0.111111f, -0.888889f, 0.0f},
                     {0.444444f, -0.888889f,  0.111111f, 0.0f},
                     {     0.0f,       0.0f,       0.0f, 1.0f});

    CORRADE_COMPARE(actual*actual, Matrix4());
    CORRADE_COMPARE(actual.transformVector(normal), -normal);
    CORRADE_COMPARE(actual, expected);
}

void Matrix4Test::reflectionNotNormalized() {
    CORRADE_SKIP_IF_NO_DEBUG_ASSERT();

    Containers::String out;
    Error redirectError{&out};

    Matrix4::reflection({-1.0f, 2.0f, 2.0f});
    CORRADE_COMPARE(out, "Math::Matrix4::reflection(): normal Vector(-1, 2, 2) is not normalized\n");
}

void Matrix4Test::reflectionIsScaling() {
    CORRADE_COMPARE(Matrix4::reflection(Vector3::yAxis()), Matrix4::scaling(Vector3::yScale(-1.0f)));
}

void Matrix4Test::shearingXY() {
    constexpr Matrix4 a = Matrix4::shearingXY(3.0f, -5.0f);
    CORRADE_COMPARE(a, Matrix4({1.0f,  0.0f, 0.0f, 0.0f},
                               {0.0f,  1.0f, 0.0f, 0.0f},
                               {3.0f, -5.0f, 1.0f, 0.0f},
                               {0.0f,  0.0f, 0.0f, 1.0f}));
    CORRADE_COMPARE(a.transformPoint(Vector3(1.0f)), Vector3(4.0f, -4.0f, 1.0f));
}

void Matrix4Test::shearingXZ() {
    constexpr Matrix4 a = Matrix4::shearingXZ(3.0f, -5.0f);
    CORRADE_COMPARE(a, Matrix4({1.0f, 0.0f,  0.0f, 0.0f},
                               {3.0f, 1.0f, -5.0f, 0.0f},
                               {0.0f, 0.0f,  1.0f, 0.0f},
                               {0.0f, 0.0f,  0.0f, 1.0f}));
    CORRADE_COMPARE(a.transformPoint(Vector3(1.0f)), Vector3(4.0f, 1.0f, -4.0f));
}

void Matrix4Test::shearingYZ() {
    constexpr Matrix4 a = Matrix4::shearingYZ(3.0f, -5.0f);
    CORRADE_COMPARE(a, Matrix4({1.0f, 3.0f, -5.0f, 0.0f},
                               {0.0f, 1.0f,  0.0f, 0.0f},
                               {0.0f, 0.0f,  1.0f, 0.0f},
                               {0.0f, 0.0f,  0.0f, 1.0f}));
    CORRADE_COMPARE(a.transformPoint(Vector3(1.0f)), Vector3(1.0f, 4.0f, -4.0f));
}

void Matrix4Test::orthographicProjection() {
    Matrix4 expected({0.4f, 0.0f,   0.0f, 0.0f},
                     {0.0f, 0.5f,   0.0f, 0.0f},
                     {0.0f, 0.0f, -0.25f, 0.0f},
                     {0.0f, 0.0f, -1.25f, 1.0f});
    Matrix4 actual = Matrix4::orthographicProjection({5.0f, 4.0f}, 1.0f, 9.0f);
    CORRADE_COMPARE(actual, expected);

    /* NDC is left-handed, so point on near plane should be -1, far +1 */
    CORRADE_COMPARE(actual.transformPoint({0.0f, 0.0f, -1.0f}), Vector3(0.0f, 0.0f, -1.0f));
    CORRADE_COMPARE(actual.transformPoint({0.0f, 0.0f, -9.0f}), Vector3(0.0f, 0.0f, +1.0f));
}

void Matrix4Test::orthographicProjectionOffCenter() {
    Matrix4 expected({0.4f,  0.0f,   0.0f, 0.0f},
                     {0.0f,  0.5f,   0.0f, 0.0f},
                     {0.0f,  0.0f, -0.25f, 0.0f},
                     {0.4f, 0.25f, -1.25f, 1.0f});
    /* Shifted by (-1, -0.5) compared to the orthographicProjection() test */
    Matrix4 actual = Matrix4::orthographicProjection({-3.5f, -2.5f}, {1.5f, 1.5f}, 1.0f, 9.0f);
    CORRADE_COMPARE(actual, expected);

    /* NDC is left-handed, so point on the near plane top right corner should
       be (1, 1, -1), and a point on the far plane bottom left corner should be
       (-1, -1, 1) */
    CORRADE_COMPARE(actual.transformPoint({1.5f, 1.5f, -1.0f}), (Vector3{1.0f, 1.0f, -1.0f}));
    CORRADE_COMPARE(actual.transformPoint({-3.5f, -2.5f, -9.0f}), (Vector3{-1.0f, -1.0f, +1.0f}));
}

void Matrix4Test::perspectiveProjection() {
    Matrix4 expected({4.0f,      0.0f,         0.0f,  0.0f},
                     {0.0f, 7.111111f,         0.0f,  0.0f},
                     {0.0f,      0.0f,  -1.9411764f, -1.0f},
                     {0.0f,      0.0f, -94.1176452f,  0.0f});
    Matrix4 actual = Matrix4::perspectiveProjection({16.0f, 9.0f}, 32.0f, 100.0f);
    CORRADE_COMPARE(actual, expected);

    /* NDC is left-handed, so point on near plane should be -1, far +1 */
    CORRADE_COMPARE(actual.transformPoint({0.0f, 0.0f, -32.0f}), Vector3(0.0f, 0.0f, -1.0f));
    CORRADE_COMPARE(actual.transformPoint({0.0f, 0.0f, -100.0f}), Vector3(0.0f, 0.0f, +1.0f));

    /* The version with bottom/left/top/right should give the same result if
       it's centered */
    CORRADE_COMPARE(Matrix4::perspectiveProjection({-8.0f, -4.5f}, {8.0f, 4.5f}, 32.0f, 100.0f), expected);
}

void Matrix4Test::perspectiveProjectionInfiniteFar() {
    Matrix4 expected({4.0f,      0.0f,   0.0f,  0.0f},
                     {0.0f, 7.111111f,   0.0f,  0.0f},
                     {0.0f,      0.0f,  -1.0f, -1.0f},
                     {0.0f,      0.0f, -64.0f,  0.0f});
    Matrix4 actual = Matrix4::perspectiveProjection({16.0f, 9.0f}, 32.0f, Constants::inf());
    CORRADE_COMPARE(actual, expected);

    /* NDC is left-handed, so point on near plane should be -1 and a *vector*
       in direction of far plane +1 */
    CORRADE_COMPARE(actual.transformPoint({0.0f, 0.0f, -32.0f}), Vector3(0.0f, 0.0f, -1.0f));
    CORRADE_COMPARE(actual.transformVector({0.0f, 0.0f, -1.0f}), Vector3(0.0f, 0.0f, +1.0f));

    /* The version with bottom/left/top/right should give the same result if
       it's centered */
    CORRADE_COMPARE(Matrix4::perspectiveProjection({-8.0f, -4.5f}, {8.0f, 4.5f}, 32.0f, Constants::inf()), expected);
}

void Matrix4Test::perspectiveProjectionFov() {
    Matrix4 expected({4.1652994f,      0.0f,         0.0f,  0.0f},
                     {      0.0f, 9.788454f,         0.0f,  0.0f},
                     {      0.0f,      0.0f,  -1.9411764f, -1.0f},
                     {      0.0f,      0.0f, -94.1176452f,  0.0f});
    CORRADE_COMPARE(Matrix4::perspectiveProjection(27.0_degf, 2.35f, 32.0f, 100.0f), expected);
}

void Matrix4Test::perspectiveProjectionFovInfiniteFar() {
    Matrix4 expected({4.1652994f,      0.0f,   0.0f,  0.0f},
                     {      0.0f, 9.788454f,   0.0f,  0.0f},
                     {      0.0f,      0.0f,  -1.0f, -1.0f},
                     {      0.0f,      0.0f, -64.0f,  0.0f});
    CORRADE_COMPARE(Matrix4::perspectiveProjection(27.0_degf, 2.35f, 32.0f, Constants::inf()), expected);
}

void Matrix4Test::perspectiveProjectionOffCenter() {
    Matrix4 expected({   4.0f,        0.0f,         0.0f,  0.0f},
                     {   0.0f,   7.111111f,         0.0f,  0.0f},
                     {-0.125f, -0.1111111f,  -1.9411764f, -1.0f},
                     {   0.0f,        0.0f, -94.1176452f,  0.0f});
    /* Shifted by (-1, -0.5) compared to the perspectiveProjection() test */
    Matrix4 actual = Matrix4::perspectiveProjection({-9.0f, -5.0f}, {7.0f, 4.0f}, 32.0f, 100.0f);
    CORRADE_COMPARE(actual, expected);

    /* NDC is left-handed, so point on the near plane top right corner should
       be (1, 1, -1), and a point in the center on the far plane roughly (0, 0,
       +1) due to the "off-centerness" */
    CORRADE_COMPARE(actual.transformPoint({7.0f, 4.0f, -32.0f}), Vector3(1.0f, 1.0f, -1.0f));
    CORRADE_COMPARE(actual.transformPoint({0.0f, 0.0f, -100.0f}), Vector3(0.125f, 0.1111111f, +1.0f));
}

void Matrix4Test::perspectiveProjectionOffCenterInfiniteFar() {
    Matrix4 expected({   4.0f,        0.0f,   0.0f,  0.0f},
                     {   0.0f,   7.111111f,   0.0f,  0.0f},
                     {-0.125f, -0.1111111f,  -1.0f, -1.0f},
                     {   0.0f,        0.0f, -64.0f,  0.0f});
    /* Shifted by (-1, -0.5) compared to perspectiveProjectionInfiniteFar() */
    Matrix4 actual = Matrix4::perspectiveProjection({-9.0f, -5.0f}, {7.0f, 4.0f}, 32.0f, Constants::inf());
    CORRADE_COMPARE(actual, expected);

    /* NDC is left-handed, so point on the near plane bottom left corner should
       be (1, 1, -1) and a *vector* in the direction of the far plane roughly
       (0, 0, +1) due to the "off-centerness" */
    CORRADE_COMPARE(actual.transformPoint({-9.0f, -5.0f, -32.0f}), Vector3(-1.0f, -1.0f, -1.0f));
    CORRADE_COMPARE(actual.transformVector({0.0f, 0.0f, -1.0f}), Vector3(0.125f, 0.1111111f, +1.0f));
}

void Matrix4Test::lookAt() {
    Vector3 translation{5.3f, -8.9f, -10.0f};
    Vector3 target{19.0f, 29.3f, 0.0f};
    Matrix4 a = Matrix4::lookAt(translation, target, Vector3::xAxis());

    /* It's just a translation and rotation */
    CORRADE_VERIFY(a.isRigidTransformation());

    /* The matrix should translate to the position */
    CORRADE_COMPARE(a.translation(), translation);

    /* Forward vector should point in direction of the target */
    CORRADE_COMPARE(dot(-a.backward(), (target - translation).normalized()), 1.0f);

    /* Up vector should be in the same direction as X axis */
    CORRADE_COMPARE_AS(dot(Vector3::xAxis(), a.up()), 0.0f, TestSuite::Compare::Greater);

    /* Just to be sure */
    CORRADE_COMPARE(a, Matrix4({     0.0f,  0.253247f,  -0.967402f, 0.0f},
                               {0.944754f, -0.317095f, -0.0830092f, 0.0f},
                               {-0.32778f, -0.913957f,  -0.239256f, 0.0f},
                               {     5.3f,      -8.9f,      -10.0f, 1.0f}));
}

void Matrix4Test::fromParts() {
    #ifndef CORRADE_MSVC2017_COMPATIBILITY
    constexpr Matrix3x3 rotationScaling(Vector3(3.0f,  5.0f, 8.0f),
                                        Vector3(4.0f,  4.0f, 7.0f),
                                        Vector3(7.0f, -1.0f, 8.0f));
    #else
    /* https://developercommunity.visualstudio.com/content/problem/259204/1572-regression-ice-in-constexpr-code-involving-de.html */
    constexpr Matrix3x3 rotationScaling{Vector<3, Float>{3.0f,  5.0f, 8.0f},
                                        Vector<3, Float>{4.0f,  4.0f, 7.0f},
                                        Vector<3, Float>{7.0f, -1.0f, 8.0f}};
    #endif
    constexpr Vector3 translation(9.0f, 4.0f, 5.0f);
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* Probably because copy is not constexpr */
    constexpr
    #endif
    Matrix4 a = Matrix4::from(rotationScaling, translation);

    CORRADE_COMPARE(a, Matrix4({3.0f,  5.0f, 8.0f, 0.0f},
                               {4.0f,  4.0f, 7.0f, 0.0f},
                               {7.0f, -1.0f, 8.0f, 0.0f},
                               {9.0f,  4.0f, 5.0f, 1.0f}));
}

void Matrix4Test::rotationScalingPart() {
    constexpr Matrix4 a({3.0f,  5.0f, 8.0f, 4.0f},
                        {4.0f,  4.0f, 7.0f, 3.0f},
                        {7.0f, -1.0f, 8.0f, 0.0f},
                        {9.0f,  4.0f, 5.0f, 9.0f});
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* Probably because copy is not constexpr */
    constexpr
    #endif
    Matrix3x3 b = a.rotationScaling();

    CORRADE_COMPARE(b, Matrix3x3(Vector3(3.0f, 5.0f, 8.0f),
                                 Vector3(4.0f, 4.0f, 7.0f),
                                 Vector3(7.0f, -1.0f, 8.0f)));
}

void Matrix4Test::rotationShearPart() {
    Matrix4 rotation = Matrix4::rotation(-74.0_degf, Vector3{-1.0f, 2.0f, 2.0f}.normalized());
    Matrix4 rotationScalingTranslation = rotation*
        Matrix4::scaling(Vector3::yScale(3.5f))*
        Matrix4::translation({2.0f, 5.0f, -3.0f});

    Matrix3x3 a = rotationScalingTranslation.rotationShear();
    CORRADE_COMPARE(a, rotation.rotationScaling());
    CORRADE_COMPARE(a, (Matrix3x3{
        Vector3{ 0.35612206f, -0.80181074f, 0.47987169f},
        Vector3{ 0.47987163f,  0.59757626f, 0.64235962f},
        Vector3{-0.80181062f,  0.00151846f, 0.59757626f}}));
}

void Matrix4Test::rotationPart() {
    Matrix4 rotation = Matrix4::rotation(-74.0_degf, Vector3{-1.0f, 2.0f, 2.0f}.normalized());
    Matrix3x3 expectedRotationPart{
        Vector3( 0.35612206f, -0.80181074f, 0.47987169f),
        Vector3( 0.47987163f,  0.59757626f, 0.64235962f),
        Vector3(-0.80181062f,  0.00151846f, 0.59757626f)};

    /* For rotation and translation this is the same as rotationScaling() */
    Matrix4 rotationTranslation = rotation*
        Matrix4::translation({2.0f, 5.0f, -3.0f});
    Matrix3x3 rotationTranslationPart = rotationTranslation.rotation();
    CORRADE_COMPARE(rotationTranslationPart, rotationTranslation.rotationScaling());
    CORRADE_COMPARE(rotationTranslationPart, expectedRotationPart);

    /* Uniform scaling */
    Matrix4 rotationUniformScalingTranslation = rotation*
        Matrix4::scaling(Vector3(3.0f))*
        Matrix4::translation({2.0f, 5.0f, -3.0f});
    Matrix3x3 rotationUniformScalingPart = rotationUniformScalingTranslation.rotation();
    CORRADE_COMPARE(rotationUniformScalingPart.determinant(), 1.0f);
    CORRADE_COMPARE(rotationUniformScalingPart*rotationUniformScalingPart.transposed(), Matrix3x3());
    CORRADE_COMPARE(rotationUniformScalingPart, expectedRotationPart);

    /* Non-uniform scaling */
    Matrix4 rotationScalingTranslation = rotation*
        Matrix4::scaling(Vector3::yScale(3.5f))*
        Matrix4::translation({2.0f, 5.0f, -3.0f});
    Matrix3x3 rotationScalingPart = rotationScalingTranslation.rotation();
    CORRADE_COMPARE(rotationScalingPart.determinant(), 1.0f);
    CORRADE_COMPARE(rotationScalingPart*rotationScalingPart.transposed(), Matrix3x3());
    CORRADE_COMPARE(rotationScalingPart, expectedRotationPart);
}

void Matrix4Test::rotationPartNotOrthogonal() {
    CORRADE_SKIP_IF_NO_DEBUG_ASSERT();

    Containers::String out;
    Error redirectError{&out};

    /* Test both non-orthogonality and "unnormalizable" scaling */
    Matrix4::shearingXY(1.5f, 0.0f).rotation();
    Matrix4::scaling(Vector3::yScale(0.0f)).rotation();

    #ifdef CORRADE_TARGET_MSVC
    CORRADE_COMPARE(out,
        "Math::Matrix4::rotation(): the normalized rotation part is not orthogonal:\n"
        "Matrix(1, 0, 0.83205,\n"
        "       0, 1, 0,\n"
        "       0, 0, 0.5547)\n"
        "Math::Matrix4::rotation(): the normalized rotation part is not orthogonal:\n"
        "Matrix(1, -nan(ind), 0,\n"
        "       0, -nan(ind), 0,\n"
        "       0, -nan(ind), 1)\n");

    /* Linux, Emscripten, Android. Somehow Androids randomly differ between
       printing positive and negative NaNs. Apple platforms and MinGW32 don't
       print signed NaNs, but it doesn't make sense to have yet another branch
       for those so they're handled here as well. */
    #else
    constexpr const char* expectedPositive =
        "Math::Matrix4::rotation(): the normalized rotation part is not orthogonal:\n"
        "Matrix(1, 0, 0.83205,\n"
        "       0, 1, 0,\n"
        "       0, 0, 0.5547)\n"
        "Math::Matrix4::rotation(): the normalized rotation part is not orthogonal:\n"
        "Matrix(1, nan, 0,\n"
        "       0, nan, 0,\n"
        "       0, nan, 1)\n";
    if(out == expectedPositive)
        CORRADE_COMPARE(out, expectedPositive);
    else CORRADE_COMPARE(out,
        "Math::Matrix4::rotation(): the normalized rotation part is not orthogonal:\n"
        "Matrix(1, 0, 0.83205,\n"
        "       0, 1, 0,\n"
        "       0, 0, 0.5547)\n"
        "Math::Matrix4::rotation(): the normalized rotation part is not orthogonal:\n"
        "Matrix(1, -nan, 0,\n"
        "       0, -nan, 0,\n"
        "       0, -nan, 1)\n");
    #endif
}

void Matrix4Test::rotationNormalizedPart() {
    Matrix4 a({ 0.35612214f,  -0.80181062f, 0.47987163f, 1.0f},
              { 0.47987163f,   0.59757638f,  0.6423595f, 3.0f},
              {-0.80181062f, 0.0015183985f, 0.59757638f, 4.0f},
              {        0.0f,          0.0f,        0.0f, 1.0f});
    CORRADE_COMPARE(a.rotationNormalized(),
        (Matrix3x3{Vector3{ 0.35612214f,  -0.80181062f, 0.47987163f},
                   Vector3{ 0.47987163f,   0.59757638f,  0.6423595f},
                   Vector3{-0.80181062f, 0.0015183985f, 0.59757638f}}));
}

void Matrix4Test::rotationNormalizedPartNotOrthogonal() {
    CORRADE_SKIP_IF_NO_DEBUG_ASSERT();

    Containers::String out;
    Error redirectError{&out};

    Matrix4 a({0.0f,  0.0f, 1.0f, 4.0f},
              {1.0f,  0.0f, 0.0f, 3.0f},
              {0.0f, -1.0f, 0.1f, 0.0f},
              {9.0f,  4.0f, 5.0f, 9.0f});
    a.rotationNormalized();
    CORRADE_COMPARE(out,
        "Math::Matrix4::rotationNormalized(): the rotation part is not orthogonal:\n"
        "Matrix(0, 1, 0,\n"
        "       0, 0, -1,\n"
        "       1, 0, 0.1)\n");
}

void Matrix4Test::scalingPart() {
    Matrix4 translationRotationScaling =
        Matrix4::translation({2.0f, 5.0f, -3.0f})*
        Matrix4::rotation(-74.0_degf, Vector3(-1.0f, 2.0f, 2.0f).normalized())*
        Matrix4::scaling({0.5f, 3.5f, 1.2f});

    CORRADE_COMPARE(translationRotationScaling.scaling(), (Vector3{0.5f, 3.5f, 1.2f}));
    CORRADE_COMPARE(translationRotationScaling.scalingSquared(), (Vector3{0.25f, 12.25f, 1.44f}));
}

void Matrix4Test::rotationScalingPartNegative() {
    /* Large angle */
    Matrix4 largeAngle =
        Matrix4::rotationY(215.0_degf)*
        Matrix4::scaling({0.5f, 3.5f, 1.2f});
    CORRADE_COMPARE(Matrix4::from(largeAngle.rotation(), {}),
        Matrix4::rotationY(215.0_degf));
    CORRADE_COMPARE(largeAngle.scaling(), (Vector3{0.5f, 3.5f, 1.2f}));
    /* The parts should combine back to the same matrix */
    CORRADE_COMPARE(
        Matrix4::from(largeAngle.rotation(), {})*
        Matrix4::scaling(largeAngle.scaling()),
        largeAngle);

    /* The sign gets contained in the rotation */
    Matrix4 negativeScaling =
        Matrix4::rotationY(15.0_degf)*
        Matrix4::scaling({0.5f, -3.5f, 1.2f});
    CORRADE_COMPARE(Matrix4::from(negativeScaling.rotation(), {}),
        Matrix4::rotationY(15.0_degf)*
        Matrix4::scaling(Vector3::yScale(-1)));
    CORRADE_COMPARE(negativeScaling.scaling(), (Vector3{0.5f, 3.5f, 1.2f}));
    /* The parts should combine back to the same matrix */
    CORRADE_COMPARE(
        Matrix4::from(negativeScaling.rotation(), {})*
        Matrix4::scaling(negativeScaling.scaling()),
        negativeScaling);
}

void Matrix4Test::uniformScalingPart() {
    const Matrix4 rotation = Matrix4::rotation(-74.0_degf, Vector3(-1.0f, 2.0f, 2.0f).normalized());

    CORRADE_COMPARE((rotation*Matrix4::scaling(Vector3(3.0f))).uniformScaling(), 3.0f);
}

void Matrix4Test::uniformScalingPartNotUniform() {
    CORRADE_SKIP_IF_NO_DEBUG_ASSERT();

    Containers::String out;
    Error redirectError{&out}; Matrix4::scaling(Vector3::yScale(3.0f)).uniformScaling();
    CORRADE_COMPARE(out, "Math::Matrix4::uniformScaling(): the matrix doesn't have uniform scaling:\n"
        "Matrix(1, 0, 0,\n"
        "       0, 3, 0,\n"
        "       0, 0, 1)\n");
}

namespace {

/* FFS. https://gcc.gnu.org/bugzilla/show_bug.cgi?id=60491 */
#ifdef minor
#undef minor
#endif

/* Code from https://github.com/graphitemaster/normals_revisited for historical
   records. This is how normalMatrix() was implemented originally as the
   article says transpose inverse "is such an accepted practice that nearly
   every single graphics programming resource mentions and encourages it. The
   problem is it's wrong."

   The problem is that what the article says is right is actually *wrong* for
   majority of use cases here. The proof there works with normals calculated
   via a cross product from three vertices in a CCW winding. That's alright,
   but then it flips the vertices along X, and calculates a cross product the
   same way again, however this time it's a CW winding and so the cross product
   is oriented *the wrong way*. Which the proposed cofactor method then fixes
   by applying the determinant sign once again with

    m.determinant()*m.inverted().transposed()

   which expands to

    m.determinant()*m.cofactor().transposed().transposed()/m.determinant

   and thus, after everything cancels out, just `m.cofactor()`. Luckily, as the
   normals have to be normalized per-pixel in the shader, it doesn't matter
   that the proposed matrix arbitrarily scales them, and this means one less
   determinant calculation.

   However, *in practice*, the normals are always calculated from an
   untransformed mesh and the shader gets them from a vertex attribute, it
   doesn't calculate anything from vertex positions where it would depend on
   winding. Which means, when the positions get reflected and triangle winding
   changes, it doesn't affect the normals in any way. And thus the
   `m.inverted().transposed()` is correct, while the cofactor-based method
   flips the normal to a wrong direction.

   Even though it's useless at this point, the test verifies against this code
   to ensure the same issue isn't introduced again at some point in the
   future. The cofactor() is also kept in the codebase as Matrix::comatrix()
   in case it'd be useful in other cases. */

float minor(const float* m, int r0, int r1, int r2, int c0, int c1, int c2) {
  return m[4*r0+c0] * (m[4*r1+c1] * m[4*r2+c2] - m[4*r2+c1] * m[4*r1+c2]) -
         m[4*r0+c1] * (m[4*r1+c0] * m[4*r2+c2] - m[4*r2+c0] * m[4*r1+c2]) +
         m[4*r0+c2] * (m[4*r1+c0] * m[4*r2+c1] - m[4*r2+c0] * m[4*r1+c1]);
}

void cofactor(const float* src, float* dst) {
  dst[ 0] =  minor(src, 1, 2, 3, 1, 2, 3);
  dst[ 1] = -minor(src, 1, 2, 3, 0, 2, 3);
  dst[ 2] =  minor(src, 1, 2, 3, 0, 1, 3);
  dst[ 3] = -minor(src, 1, 2, 3, 0, 1, 2);
  dst[ 4] = -minor(src, 0, 2, 3, 1, 2, 3);
  dst[ 5] =  minor(src, 0, 2, 3, 0, 2, 3);
  dst[ 6] = -minor(src, 0, 2, 3, 0, 1, 3);
  dst[ 7] =  minor(src, 0, 2, 3, 0, 1, 2);
  dst[ 8] =  minor(src, 0, 1, 3, 1, 2, 3);
  dst[ 9] = -minor(src, 0, 1, 3, 0, 2, 3);
  dst[10] =  minor(src, 0, 1, 3, 0, 1, 3);
  dst[11] = -minor(src, 0, 1, 3, 0, 1, 2);
  dst[12] = -minor(src, 0, 1, 2, 1, 2, 3);
  dst[13] =  minor(src, 0, 1, 2, 0, 2, 3);
  dst[14] = -minor(src, 0, 1, 2, 0, 1, 3);
  dst[15] =  minor(src, 0, 1, 2, 0, 1, 2);
}

Matrix4 cofactorGroundTruth(const Matrix4& src) {
    Matrix4 out;
    cofactor(src.data(), out.data());
    return out;
}

}

void Matrix4Test::normalMatrixPart() {
    /* Comparing normalized matrices -- we care only about orientation, not
       scaling as that's renormalized in the shader anyway */
    auto unit = [](const Matrix3x3& a) {
        return Matrix3x3{a[0].normalized(),
                         a[1].normalized(),
                         a[2].normalized()};
    };

    /* For just a rotation, normalMatrix is the same as the upper-left part */
    auto a = Matrix4::rotationY(35.0_degf);
    CORRADE_COMPARE(a.normalMatrix(), a.rotationScaling());
    CORRADE_COMPARE(a.normalMatrix(), a.rotationScaling().inverted().transposed());
    /* And same as the cofactor-based method */
    CORRADE_COMPARE(a.normalMatrix(), cofactorGroundTruth(a).rotationScaling());
    CORRADE_COMPARE(a.normalMatrix(), a.rotationScaling().comatrix());

    /* For rotation + uniform scaling, normalMatrix is the same as the
       normalized upper-left part */
    auto b = Matrix4::rotationZ(35.0_degf)*Matrix4::scaling(Vector3{3.5f});
    CORRADE_COMPARE(unit(b.normalMatrix()), unit(b.rotation()));
    /* And same as the cofactor-based method, except scale */
    CORRADE_COMPARE(unit(b.normalMatrix()), unit(cofactorGroundTruth(b).rotationScaling()));
    CORRADE_COMPARE(unit(b.normalMatrix()), unit(b.rotationScaling().comatrix()));

    /* Rotation and non-uniform scaling (= shear) needs inverse transpose to
       be correct */
    auto c = Matrix4::rotationX(35.0_degf)*Matrix4::scaling({0.3f, 1.1f, 3.5f});
    CORRADE_COMPARE(unit(c.normalMatrix()), unit(c.rotationScaling().inverted().transposed()));
    /* And again same as the cofactor-based method except scale */
    CORRADE_COMPARE(unit(c.normalMatrix()), unit(cofactorGroundTruth(c).rotationScaling()));
    CORRADE_COMPARE(unit(c.normalMatrix()), unit(c.rotationScaling().comatrix()));

    /* Reflection (or scaling by -1) needs inverse transpose as well */
    auto d = Matrix4::rotationZ(35.0_degf)*Matrix4::reflection(Vector3{1.0f/Constants::sqrt3()});
    CORRADE_COMPARE(unit(d.normalMatrix()), unit(d.rotationScaling().inverted().transposed()));
    /* The cofactor-based method gives back a different (wrong) sign */
    CORRADE_COMPARE(unit(d.normalMatrix()), -unit(cofactorGroundTruth(d).rotationScaling()));
    CORRADE_COMPARE(unit(d.normalMatrix()), -unit(d.rotationScaling().comatrix()));
}

void Matrix4Test::vectorParts() {
    constexpr Matrix4 a({-1.0f,  0.0f,  0.0f, 0.0f},
                        { 0.0f, 12.0f,  0.0f, 0.0f},
                        { 0.0f,  0.0f, 35.0f, 0.0f},
                        {-5.0f, 12.0f,  0.5f, 1.0f});
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* Probably because copy is not constexpr */
    constexpr
    #endif
    Vector3 right = a.right();
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* Probably because copy is not constexpr */
    constexpr
    #endif
    Vector3 up = a.up();
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* Probably because copy is not constexpr */
    constexpr
    #endif
    Vector3 backward = a.backward();
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* Probably because copy is not constexpr */
    constexpr
    #endif
    Vector3 translation = a.translation();

    CORRADE_COMPARE(right, Vector3::xAxis(-1.0f));
    CORRADE_COMPARE(up, Vector3::yAxis(12.0f));
    CORRADE_COMPARE(backward, Vector3::zAxis(35.0f));
    CORRADE_COMPARE(translation, Vector3(-5.0f, 12.0f, 0.5f));
}

void Matrix4Test::orthographicProjectionParts() {
    Matrix4 projection = Matrix4::orthographicProjection({5.0f, 4.0f}, 1.0f, 9.0f);
    CORRADE_COMPARE(projection.orthographicProjectionNear(), 1.0f);
    CORRADE_COMPARE(projection.orthographicProjectionFar(), 9.0f);
}

void Matrix4Test::perspectiveProjectionParts() {
    Matrix4 projection = Matrix4::perspectiveProjection({16.0f, 9.0f}, 32.0f, 100.0f);
    CORRADE_COMPARE(projection.perspectiveProjectionNear(), 32.0f);
    CORRADE_COMPARE(projection.perspectiveProjectionFar(), 100.0f);
}

void Matrix4Test::perspectiveProjectionInfiniteFarParts() {
    Matrix4 projection = Matrix4::perspectiveProjection({16.0f, 9.0f}, 32.0f, Constants::inf());
    CORRADE_COMPARE(projection.perspectiveProjectionNear(), 32.0f);
    CORRADE_COMPARE(projection.perspectiveProjectionFar(), Constants::inf());
}

void Matrix4Test::invertedRigid() {
    Matrix4 actual = Matrix4::rotation(-74.0_degf, Vector3(-1.0f, 0.5f, 2.0f).normalized())*
                     Matrix4::reflection(Vector3(0.5f, -2.0f, 2.0f).normalized())*
                     Matrix4::translation({1.0f, 2.0f, -3.0f});
    Matrix4 expected = Matrix4::translation({-1.0f, -2.0f, 3.0f})*
                       Matrix4::reflection(Vector3(0.5f, -2.0f, 2.0f).normalized())*
                       Matrix4::rotation(74.0_degf, Vector3(-1.0f, 0.5f, 2.0f).normalized());


    CORRADE_COMPARE(actual.invertedRigid(), expected);
    CORRADE_COMPARE(actual.invertedRigid(), actual.inverted());
}

void Matrix4Test::invertedRigidNotRigid() {
    CORRADE_SKIP_IF_NO_DEBUG_ASSERT();

    Containers::String out;
    Error redirectError{&out};

    (Matrix4::rotationX(-60.0_degf)*2.0f).invertedRigid();
    CORRADE_COMPARE(out,
        "Math::Matrix4::invertedRigid(): the matrix doesn't represent a rigid transformation:\n"
        "Matrix(2, 0, 0, 0,\n"
        "       0, 1, 1.73205, 0,\n"
        "       0, -1.73205, 1, 0,\n"
        "       0, 0, 0, 2)\n");
}

void Matrix4Test::transform() {
    Matrix4 a = Matrix4::translation({1.0f, -5.0f, 3.5f})*Matrix4::rotation(90.0_degf, Vector3::zAxis());
    Vector3 v(1.0f, -2.0f, 5.5f);

    CORRADE_COMPARE(a.transformVector(v), Vector3(2.0f, 1.0f, 5.5f));
    CORRADE_COMPARE(a.transformPoint(v), Vector3(3.0f, -4.0f, 9.0f));
}

void Matrix4Test::transformProjection() {
    Matrix4 a = Matrix4::perspectiveProjection({2.0f, 2.0f}, 1.0f, 100.0f);
    Vector3 v{0.0f, 0.0f, -100.0f};

    CORRADE_COMPARE(a.transformPoint(v), Vector3(0.0f, 0.0f, 1.0f));
}

void Matrix4Test::strictWeakOrdering() {
    StrictWeakOrdering o;
    const Matrix4 a(Vector4{1.0f, 1.0f, 2.0f, 2.0f}, Vector4{5.0f, 5.0f, 6.0f, 5.0f}, Vector4{5.0f, 5.0f, 6.0f, 5.0f}, Vector4{3.0f, 1.0f, 2.0f, 4.0f});
    const Matrix4 b(Vector4{2.0f, 1.0f, 2.0f, 3.0f}, Vector4{5.0f, 5.0f, 6.0f, 5.0f}, Vector4{5.0f, 5.0f, 6.0f, 5.0f}, Vector4{4.0f, 1.0f, 2.0f, 5.0f});
    const Matrix4 c(Vector4{1.0f, 1.0f, 2.0f, 2.0f}, Vector4{5.0f, 5.0f, 6.0f, 5.0f}, Vector4{5.0f, 5.0f, 6.0f, 5.0f}, Vector4{3.0f, 1.0f, 2.0f, 5.0f});

    CORRADE_VERIFY( o(a, b));
    CORRADE_VERIFY(!o(b, a));
    CORRADE_VERIFY( o(a, c));
    CORRADE_VERIFY(!o(c, a));
    CORRADE_VERIFY( o(c, b));
    CORRADE_VERIFY(!o(b, c));

    CORRADE_VERIFY(!o(a, a));
}

void Matrix4Test::debug() {
    Matrix4 m({3.0f,  5.0f, 8.0f, 4.0f},
              {4.0f,  4.0f, 7.0f, 3.0f},
              {7.0f, -1.0f, 8.0f, 0.0f},
              {9.0f,  4.0f, 5.0f, 9.0f});

    Containers::String out;
    Debug{&out} << m;
    CORRADE_COMPARE(out, "Matrix(3, 4, 7, 9,\n"
                             "       5, 4, -1, 4,\n"
                             "       8, 7, 8, 5,\n"
                             "       4, 3, 0, 9)\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::Matrix4Test)
