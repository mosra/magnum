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

#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/StrictWeakOrdering.h"

struct Mat3 {
    float a[9];
};

namespace Magnum { namespace Math {

namespace Implementation {

template<> struct RectangularMatrixConverter<3, 3, float, Mat3> {
    constexpr static RectangularMatrix<3, 3, Float> from(const Mat3& other) {
        return RectangularMatrix<3, 3, Float>(
            Vector<3, Float>(other.a[0], other.a[1], other.a[2]),
            Vector<3, Float>(other.a[3], other.a[4], other.a[5]),
            Vector<3, Float>(other.a[6], other.a[7], other.a[8]));
    }

    constexpr static Mat3 to(const RectangularMatrix<3, 3, Float>& other) {
        return Mat3{{other[0][0], other[0][1], other[0][2],
                     other[1][0], other[1][1], other[1][2],
                     other[2][0], other[2][1], other[2][2]}};
    }
};

}

namespace Test { namespace {

struct Matrix3Test: Corrade::TestSuite::Tester {
    explicit Matrix3Test();

    void construct();
    void constructIdentity();
    void constructZero();
    void constructNoInit();
    void constructOneValue();
    void constructConversion();
    void constructFromDifferentSize();
    void constructCopy();
    void convert();

    void isRigidTransformation();

    void translation();
    void scaling();
    void rotation();
    void reflection();
    void reflectionNotNormalized();
    void reflectionIsScaling();
    void shearingX();
    void shearingY();
    void projection();

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
    void vectorParts();
    void invertedRigid();
    void invertedRigidNotRigid();
    void transform();

    void strictWeakOrdering();

    void debug();
};

typedef Math::Deg<Float> Deg;
typedef Math::Matrix2x2<Float> Matrix2x2;
typedef Math::Matrix3<Float> Matrix3;
typedef Math::Matrix3<Int> Matrix3i;
typedef Math::Vector2<Float> Vector2;
typedef Math::Vector3<Float> Vector3;

Matrix3Test::Matrix3Test() {
    addTests({&Matrix3Test::construct,
              &Matrix3Test::constructIdentity,
              &Matrix3Test::constructZero,
              &Matrix3Test::constructNoInit,
              &Matrix3Test::constructOneValue,
              &Matrix3Test::constructConversion,
              &Matrix3Test::constructFromDifferentSize,
              &Matrix3Test::constructCopy,
              &Matrix3Test::convert,

              &Matrix3Test::isRigidTransformation,

              &Matrix3Test::translation,
              &Matrix3Test::scaling,
              &Matrix3Test::rotation,
              &Matrix3Test::reflection,
              &Matrix3Test::reflectionNotNormalized,
              &Matrix3Test::reflectionIsScaling,
              &Matrix3Test::shearingX,
              &Matrix3Test::shearingY,
              &Matrix3Test::projection,

              &Matrix3Test::fromParts,
              &Matrix3Test::rotationScalingPart,
              &Matrix3Test::rotationShearPart,
              &Matrix3Test::rotationPart,
              &Matrix3Test::rotationPartNotOrthogonal,
              &Matrix3Test::rotationNormalizedPart,
              &Matrix3Test::rotationNormalizedPartNotOrthogonal,
              &Matrix3Test::scalingPart,
              &Matrix3Test::rotationScalingPartNegative,
              &Matrix3Test::uniformScalingPart,
              &Matrix3Test::uniformScalingPartNotUniform,
              &Matrix3Test::vectorParts,
              &Matrix3Test::invertedRigid,
              &Matrix3Test::invertedRigidNotRigid,
              &Matrix3Test::transform,

              &Matrix3Test::strictWeakOrdering,

              &Matrix3Test::debug});
}

using namespace Literals;

void Matrix3Test::construct() {
    constexpr Matrix3 a = {{3.0f,  5.0f, 8.0f},
                           {4.5f,  4.0f, 7.0f},
                           {7.9f, -1.0f, 8.0f}};
    CORRADE_COMPARE(a, Matrix3({3.0f,  5.0f, 8.0f},
                               {4.5f,  4.0f, 7.0f},
                               {7.9f, -1.0f, 8.0f}));

    CORRADE_VERIFY((std::is_nothrow_constructible<Matrix3, Vector3, Vector3, Vector3>::value));
}

void Matrix3Test::constructIdentity() {
    constexpr Matrix3 identity;
    constexpr Matrix3 identity2{IdentityInit};
    constexpr Matrix3 identity3{IdentityInit, 4.0f};

    Matrix3 identityExpected({1.0f, 0.0f, 0.0f},
                             {0.0f, 1.0f, 0.0f},
                             {0.0f, 0.0f, 1.0f});

    Matrix3 identity3Expected({4.0f, 0.0f, 0.0f},
                              {0.0f, 4.0f, 0.0f},
                              {0.0f, 0.0f, 4.0f});

    CORRADE_COMPARE(identity, identityExpected);
    CORRADE_COMPARE(identity2, identityExpected);
    CORRADE_COMPARE(identity3, identity3Expected);

    CORRADE_VERIFY(std::is_nothrow_default_constructible<Matrix3>::value);
    CORRADE_VERIFY((std::is_nothrow_constructible<Matrix3, IdentityInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<IdentityInitT, Matrix3>::value));
}

void Matrix3Test::constructZero() {
    constexpr Matrix3 a{ZeroInit};
    CORRADE_COMPARE(a, Matrix3({0.0f, 0.0f, 0.0f},
                               {0.0f, 0.0f, 0.0f},
                               {0.0f, 0.0f, 0.0f}));

    CORRADE_VERIFY((std::is_nothrow_constructible<Matrix3, ZeroInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<ZeroInitT, Matrix3>::value));
}

void Matrix3Test::constructNoInit() {
    Matrix3 a{{3.0f,  5.0f, 8.0f},
              {4.5f,  4.0f, 7.0f},
              {7.9f, -1.0f, 8.0f}};
    new(&a) Matrix3{Magnum::NoInit};
    {
        #if defined(__GNUC__) && __GNUC__*100 + __GNUC_MINOR__ >= 601 && __OPTIMIZE__
        CORRADE_EXPECT_FAIL("GCC 6.1+ misoptimizes and overwrites the value.");
        #endif
        CORRADE_COMPARE(a, Matrix3({3.0f,  5.0f, 8.0f},
                                   {4.5f,  4.0f, 7.0f},
                                   {7.9f, -1.0f, 8.0f}));
    }

    CORRADE_VERIFY((std::is_nothrow_constructible<Matrix3, Magnum::NoInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Magnum::NoInitT, Matrix3>::value));
}

void Matrix3Test::constructOneValue() {
    constexpr Matrix3 a{1.5f};
    CORRADE_COMPARE(a, (Matrix3{Vector3{1.5f, 1.5f, 1.5f},
                                Vector3{1.5f, 1.5f, 1.5f},
                                Vector3{1.5f, 1.5f, 1.5f}}));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Float, Matrix3>::value));

    CORRADE_VERIFY((std::is_nothrow_constructible<Matrix3, Float>::value));
}

void Matrix3Test::constructConversion() {
    constexpr Matrix3 a({3.0f,  5.0f, 8.0f},
                        {4.5f,  4.0f, 7.0f},
                        {7.9f, -1.0f, 8.0f});
    constexpr Matrix3i b(a);
    CORRADE_COMPARE(b, Matrix3i({3,  5, 8},
                                {4,  4, 7},
                                {7, -1, 8}));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Matrix3, Matrix3i>::value));

    CORRADE_VERIFY((std::is_nothrow_constructible<Matrix3, Matrix3i>::value));
}

void Matrix3Test::constructFromDifferentSize() {
    constexpr Matrix3 a{{3.0f,  5.0f, 8.0f},
                        {4.5f,  4.0f, 7.0f},
                        {1.0f,  2.0f, 3.0f}};
    constexpr Matrix2x2 b{Vector2{3.0f,  5.0f},
                          Vector2{4.5f,  4.0f}};
    constexpr Matrix3 c{{3.0f, 5.0f, 0.0f},
                        {4.5f, 4.0f, 0.0f},
                        {0.0f, 0.0f, 1.0f}};

    constexpr Matrix3 larger{b};
    CORRADE_COMPARE(larger, c);
    CORRADE_COMPARE(Matrix3{b}, c);

    constexpr Matrix2x2 smaller{a};
    CORRADE_COMPARE(smaller, b);
    CORRADE_COMPARE(Matrix2x2{a}, b);
}

void Matrix3Test::constructCopy() {
    constexpr RectangularMatrix<3, 3, Float> a(Vector<3, Float>(3.0f,  5.0f, 8.0f),
                                               Vector<3, Float>(4.5f,  4.0f, 7.0f),
                                               Vector<3, Float>(7.9f, -1.0f, 8.0f));
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* Why can't be copy constexpr? */
    constexpr
    #endif
    Matrix3 b(a);
    CORRADE_COMPARE(b, Matrix3({3.0f,  5.0f, 8.0f},
                               {4.5f,  4.0f, 7.0f},
                               {7.9f, -1.0f, 8.0f}));

    CORRADE_VERIFY(std::is_nothrow_copy_constructible<Matrix3>::value);
    CORRADE_VERIFY(std::is_nothrow_copy_assignable<Matrix3>::value);
}

void Matrix3Test::convert() {
    constexpr Mat3 a{{1.5f,  2.0f, -3.5f,
                      2.0f, -3.1f,  0.4f,
                      9.5f, -1.5f,  0.1f}};
    constexpr Matrix3 b(Vector3(1.5f, 2.0f, -3.5f),
                        Vector3(2.0f, -3.1f,  0.4f),
                        Vector3(9.5f, -1.5f,  0.1f));

    constexpr Matrix3 c(b);
    CORRADE_COMPARE(c, b);

    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* Why can't be conversion constexpr? */
    constexpr
    #endif
    Mat3 d(b);
    for(std::size_t i = 0; i != 9; ++i)
        CORRADE_COMPARE(d.a[0], a.a[0]);

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Mat3, Matrix3>::value));
    CORRADE_VERIFY(!(std::is_convertible<Matrix3, Mat3>::value));
}

void Matrix3Test::isRigidTransformation() {
    CORRADE_VERIFY(!Matrix3({1.0f, 0.0f, 0.0f},
                            {0.1f, 1.0f, 0.0f},
                            {5.0f, 4.0f, 1.0f}).isRigidTransformation());
    CORRADE_VERIFY(!Matrix3({1.0f, 0.0f, 0.0f},
                            {0.0f, 1.0f, 1.0f},
                            {5.0f, 4.0f, 0.0f}).isRigidTransformation());
    CORRADE_VERIFY(Matrix3({1.0f, 0.0f, 0.0f},
                           {0.0f, 1.0f, 0.0f},
                           {5.0f, 4.0f, 1.0f}).isRigidTransformation());
}

void Matrix3Test::translation() {
    constexpr Matrix3 a = Matrix3::translation({3.0f, 1.0f});
    CORRADE_COMPARE(a, Matrix3({1.0f, 0.0f, 0.0f},
                               {0.0f, 1.0f, 0.0f},
                               {3.0f, 1.0f, 1.0f}));
}

void Matrix3Test::scaling() {
    constexpr Matrix3 a = Matrix3::scaling({3.0f, 1.5f});
    CORRADE_COMPARE(a, Matrix3({3.0f, 0.0f, 0.0f},
                               {0.0f, 1.5f, 0.0f},
                               {0.0f, 0.0f, 1.0f}));
}

void Matrix3Test::rotation() {
    Matrix3 matrix({ 0.965926f, 0.258819f, 0.0f},
                   {-0.258819f, 0.965926f, 0.0f},
                   {      0.0f,      0.0f, 1.0f});

    CORRADE_COMPARE(Matrix3::rotation(15.0_degf), matrix);
}

void Matrix3Test::reflection() {
    Vector2 normal(-1.0f, 2.0f);
    Matrix3 actual = Matrix3::reflection(normal.normalized());
    Matrix3 expected({0.6f,  0.8f, 0.0f},
                     {0.8f, -0.6f, 0.0f},
                     {0.0f,  0.0f, 1.0f});

    CORRADE_COMPARE(actual*actual, Matrix3());
    CORRADE_COMPARE(actual.transformVector(normal), -normal);
    CORRADE_COMPARE(actual, expected);
}

void Matrix3Test::reflectionNotNormalized() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    Matrix3::reflection({-1.0f, 2.0f});
    CORRADE_COMPARE(out.str(), "Math::Matrix3::reflection(): normal Vector(-1, 2) is not normalized\n");
}

void Matrix3Test::reflectionIsScaling() {
    CORRADE_COMPARE(Matrix3::reflection(Vector2::yAxis()), Matrix3::scaling(Vector2::yScale(-1.0f)));
}

void Matrix3Test::shearingX() {
    constexpr Matrix3 a = Matrix3::shearingX(3.0f);
    CORRADE_COMPARE(a, Matrix3({1.0f, 0.0f, 0.0f},
                               {3.0f, 1.0f, 0.0f},
                               {0.0f, 0.0f, 1.0f}));
    CORRADE_COMPARE(a.transformPoint(Vector2(1.0f)), Vector2(4.0f, 1.0f));
}

void Matrix3Test::shearingY() {
    constexpr Matrix3 a = Matrix3::shearingY(3.0f);
    CORRADE_COMPARE(a, Matrix3({1.0f, 3.0f, 0.0f},
                               {0.0f, 1.0f, 0.0f},
                               {0.0f, 0.0f, 1.0f}));
    CORRADE_COMPARE(a.transformPoint(Vector2(1.0f)), Vector2(1.0f, 4.0f));
}

void Matrix3Test::projection() {
    Matrix3 expected({2.0f/4.0f,      0.0f, 0.0f},
                     {     0.0f, 2.0f/3.0f, 0.0f},
                     {     0.0f,      0.0f, 1.0f});

    CORRADE_COMPARE(Matrix3::projection({4.0f, 3.0f}), expected);
}

void Matrix3Test::fromParts() {
    #ifndef CORRADE_MSVC2017_COMPATIBILITY
    constexpr Matrix2x2 rotationScaling(Vector2(3.0f, 5.0f),
                                        Vector2(4.0f, 4.0f));
    #else
    /* https://developercommunity.visualstudio.com/content/problem/259204/1572-regression-ice-in-constexpr-code-involving-de.html */
    constexpr Matrix2x2 rotationScaling{Vector<2, Float>{3.0f, 5.0f},
                                        Vector<2, Float>{4.0f, 4.0f}};
    #endif
    constexpr Vector2 translation(7.0f, -1.0f);
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* Probably because copy is not constexpr */
    constexpr
    #endif
    Matrix3 a = Matrix3::from(rotationScaling, translation);

    CORRADE_COMPARE(a, Matrix3({3.0f,  5.0f, 0.0f},
                               {4.0f,  4.0f, 0.0f},
                               {7.0f, -1.0f, 1.0f}));
}

void Matrix3Test::rotationScalingPart() {
    constexpr Matrix3 a({3.0f,  5.0f, 8.0f},
                        {4.0f,  4.0f, 7.0f},
                        {7.0f, -1.0f, 8.0f});
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* Probably because copy is not constexpr */
    constexpr
    #endif
    Matrix2x2 b = a.rotationScaling();

    CORRADE_COMPARE(b, Matrix2x2(Vector2(3.0f, 5.0f),
                                 Vector2(4.0f, 4.0f)));
}

void Matrix3Test::rotationShearPart() {
    Matrix3 rotation = Matrix3::rotation(15.0_degf);
    Matrix3 rotationScalingTranslation = rotation*
        Matrix3::scaling(Vector2::yScale(3.5f))*
        Matrix3::translation({2.0f, -3.0f});

    Matrix2x2 a = rotationScalingTranslation.rotationShear();
    CORRADE_COMPARE(a, rotation.rotationScaling());
    CORRADE_COMPARE(a, (Matrix2x2{Vector2{ 0.965926f, 0.258819f},
                                  Vector2{-0.258819f, 0.965926f}}));
}

void Matrix3Test::rotationPart() {
    Matrix3 rotation = Matrix3::rotation(15.0_degf);
    Matrix2x2 expectedRotationPart{Vector2{ 0.965926f, 0.258819f},
                                   Vector2{-0.258819f, 0.965926f}};

    /* For rotation and translation this is the same as rotationScaling() */
    Matrix3 rotationTranslation = rotation*Matrix3::translation({2.0f, 5.0f});
    Matrix2x2 rotationTranslationPart = rotationTranslation.rotation();
    CORRADE_COMPARE(rotationTranslationPart, rotationTranslation.rotationScaling());
    CORRADE_COMPARE(rotationTranslationPart, expectedRotationPart);

    /* Uniform scaling */
    Matrix3 rotationUniformScalingTranslation = rotation*
        Matrix3::scaling(Vector2(3.0f))*
        Matrix3::translation({2.0f, -3.0f});
    Matrix2x2 rotationUniformScalingPart = rotationUniformScalingTranslation.rotation();
    CORRADE_COMPARE(rotationUniformScalingPart.determinant(), 1.0f);
    CORRADE_COMPARE(rotationUniformScalingPart*rotationUniformScalingPart.transposed(), Matrix2x2());
    CORRADE_COMPARE(rotationUniformScalingPart, expectedRotationPart);

    /* Non-uniform scaling */
    Matrix3 rotationScalingTranslation = rotation*
        Matrix3::scaling(Vector2::yScale(3.5f))*
        Matrix3::translation({2.0f, -3.0f});
    Matrix2x2 rotationScalingPart = rotationScalingTranslation.rotation();
    CORRADE_COMPARE(rotationScalingPart.determinant(), 1.0f);
    CORRADE_COMPARE(rotationScalingPart*rotationScalingPart.transposed(), Matrix2x2());
    CORRADE_COMPARE(rotationScalingPart, expectedRotationPart);
}

void Matrix3Test::rotationPartNotOrthogonal() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    /* Test both non-orthogonality and "unnormalizable" scaling */
    Matrix3::shearingX(1.5f).rotation();
    Matrix3::scaling(Vector2::yScale(0.0f)).rotation();

    #if defined(CORRADE_TARGET_APPLE) || (defined(CORRADE_TARGET_WINDOWS) && defined(__MINGW32__)) || defined(CORRADE_TARGET_ANDROID)
    CORRADE_COMPARE(out.str(),
        "Math::Matrix3::rotation(): the normalized rotation part is not orthogonal:\n"
        "Matrix(1, 0.83205,\n"
        "       0, 0.5547)\n"
        "Math::Matrix3::rotation(): the normalized rotation part is not orthogonal:\n"
        "Matrix(1, nan,\n"
        "       0, nan)\n");
    #elif defined(CORRADE_TARGET_WINDOWS) && defined(_MSC_VER)
    CORRADE_COMPARE(out.str(),
        "Math::Matrix3::rotation(): the normalized rotation part is not orthogonal:\n"
        "Matrix(1, 0.83205,\n"
        "       0, 0.5547)\n"
        "Math::Matrix3::rotation(): the normalized rotation part is not orthogonal:\n"
        "Matrix(1, -nan(ind),\n"
        "       0, -nan(ind))\n");
    #else
    CORRADE_COMPARE(out.str(),
        "Math::Matrix3::rotation(): the normalized rotation part is not orthogonal:\n"
        "Matrix(1, 0.83205,\n"
        "       0, 0.5547)\n"
        "Math::Matrix3::rotation(): the normalized rotation part is not orthogonal:\n"
        "Matrix(1, -nan,\n"
        "       0, -nan)\n");
    #endif
}

void Matrix3Test::rotationNormalizedPart() {
    Matrix3 a({ 0.965926f, 0.258819f, 1.0f},
              {-0.258819f, 0.965926f, 3.0f},
              {      0.0f,      0.0f, 1.0f});
    CORRADE_COMPARE(a.rotationNormalized(),
        (Matrix2x2{Vector2{ 0.965926f, 0.258819f},
                   Vector2{-0.258819f, 0.965926f}}));
}

void Matrix3Test::rotationNormalizedPartNotOrthogonal() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    Matrix3 a({1.0f,  0.0f, 8.0f},
              {1.0f,  0.1f, 7.0f},
              {7.0f, -1.0f, 8.0f});
    a.rotationNormalized();

    CORRADE_COMPARE(out.str(),
        "Math::Matrix3::rotationNormalized(): the rotation part is not orthogonal:\n"
        "Matrix(1, 1,\n"
        "       0, 0.1)\n");
}

void Matrix3Test::scalingPart() {
    Matrix3 translationRotationScaling =
        Matrix3::translation({2.0f, -3.0f})*
        Matrix3::rotation(15.0_degf)*
        Matrix3::scaling({0.5f, 3.5f});

    CORRADE_COMPARE(translationRotationScaling.scaling(), (Vector2{0.5f, 3.5f}));
    CORRADE_COMPARE(translationRotationScaling.scalingSquared(), (Vector2{0.25f, 12.25f}));
}

void Matrix3Test::rotationScalingPartNegative() {
    /* Large angle */
    Matrix3 largeAngle =
        Matrix3::rotation(215.0_degf)*
        Matrix3::scaling({0.5f, 3.5f});
    CORRADE_COMPARE(Matrix3::from(largeAngle.rotation(), {}),
        Matrix3::rotation(215.0_degf));
    CORRADE_COMPARE(largeAngle.scaling(), (Vector2{0.5f, 3.5f}));
    /* The parts should combine back to the same matrix */
    CORRADE_COMPARE(
        Matrix3::from(largeAngle.rotation(), {})*
        Matrix3::scaling(largeAngle.scaling()),
        largeAngle);

    /* The sign gets contained in the rotation */
    Matrix3 negativeScaling =
        Matrix3::rotation(15.0_degf)*
        Matrix3::scaling({0.5f, -3.5f});
    CORRADE_COMPARE(Matrix3::from(negativeScaling.rotation(), {}),
        Matrix3::rotation(15.0_degf)*
        Matrix3::scaling(Vector2::yScale(-1)));
    CORRADE_COMPARE(negativeScaling.scaling(), (Vector2{0.5f, 3.5f}));
    /* The parts should combine back to the same matrix */
    CORRADE_COMPARE(
        Matrix3::from(negativeScaling.rotation(), {})*
        Matrix3::scaling(negativeScaling.scaling()),
        negativeScaling);
}

void Matrix3Test::uniformScalingPart() {
    const Matrix3 rotation = Matrix3::rotation(-74.0_degf);

    CORRADE_COMPARE((rotation*Matrix3::scaling(Vector2(3.0f))).uniformScaling(), 3.0f);
}

void Matrix3Test::uniformScalingPartNotUniform() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    Matrix3::scaling(Vector2::yScale(3.0f)).uniformScaling();
    CORRADE_COMPARE(out.str(),
        "Math::Matrix3::uniformScaling(): the matrix doesn't have uniform scaling:\n"
        "Matrix(1, 0,\n"
        "       0, 3)\n");
}

void Matrix3Test::vectorParts() {
    constexpr Matrix3 a({15.0f,  0.0f, 0.0f},
                        { 0.0f, -3.0f, 0.0f},
                        {-5.0f, 12.0f, 1.0f});
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* Probably because copy is not constexpr */
    constexpr
    #endif
    Vector2 right = a.right();
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* Probably because copy is not constexpr */
    constexpr
    #endif
    Vector2 up = a.up();
    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* Probably because copy is not constexpr */
    constexpr
    #endif
    Vector2 translation = a.translation();

    CORRADE_COMPARE(right, Vector2::xAxis(15.0f));
    CORRADE_COMPARE(up, Vector2::yAxis(-3.0f));
    CORRADE_COMPARE(translation, Vector2(-5.0f, 12.0f));
}

void Matrix3Test::invertedRigid() {
    Matrix3 actual = Matrix3::rotation(-74.0_degf)*
                     Matrix3::reflection(Vector2(0.5f, -2.0f).normalized())*
                     Matrix3::translation({2.0f, -3.0f});
    Matrix3 expected = Matrix3::translation({-2.0f, 3.0f})*
                       Matrix3::reflection(Vector2(0.5f, -2.0f).normalized())*
                       Matrix3::rotation(74.0_degf);

    CORRADE_COMPARE(actual.invertedRigid(), expected);
    CORRADE_COMPARE(actual.invertedRigid(), actual.inverted());
}

void Matrix3Test::invertedRigidNotRigid() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    (Matrix3::rotation(60.0_degf)*2.0f).invertedRigid();

    CORRADE_COMPARE(out.str(),
        "Math::Matrix3::invertedRigid(): the matrix doesn't represent a rigid transformation:\n"
        "Matrix(1, -1.73205, 0,\n"
        "       1.73205, 1, 0,\n"
        "       0, 0, 2)\n");
}

void Matrix3Test::transform() {
    Matrix3 a = Matrix3::translation({1.0f, -5.0f})*Matrix3::rotation(90.0_degf);
    Vector2 v(1.0f, -2.0f);

    CORRADE_COMPARE(a.transformVector(v), Vector2(2.0f, 1.0f));
    CORRADE_COMPARE(a.transformPoint(v), Vector2(3.0f, -4.0f));
}

void Matrix3Test::strictWeakOrdering() {
    StrictWeakOrdering o;
    const Matrix3 a(Vector3{1.0f, 1.0f, 2.0f}, Vector3{5.0f, 5.0f, 5.0f}, Vector3{3.0f, 1.0f, 4.0f});
    const Matrix3 b(Vector3{2.0f, 1.0f, 3.0f}, Vector3{5.0f, 5.0f, 5.0f}, Vector3{4.0f, 1.0f, 5.0f});
    const Matrix3 c(Vector3{1.0f, 1.0f, 2.0f}, Vector3{5.0f, 5.0f, 5.0f}, Vector3{3.0f, 1.0f, 5.0f});

    CORRADE_VERIFY( o(a, b));
    CORRADE_VERIFY(!o(b, a));
    CORRADE_VERIFY( o(a, c));
    CORRADE_VERIFY(!o(c, a));
    CORRADE_VERIFY( o(c, b));
    CORRADE_VERIFY(!o(b, c));

    CORRADE_VERIFY(!o(a, a));
}

void Matrix3Test::debug() {
    Matrix3 m({3.0f,  5.0f, 8.0f},
              {4.0f,  4.0f, 7.0f},
              {7.0f, -1.0f, 8.0f});

    std::ostringstream o;
    Debug(&o) << m;
    CORRADE_COMPARE(o.str(), "Matrix(3, 4, 7,\n"
                             "       5, 4, -1,\n"
                             "       8, 7, 8)\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::Matrix3Test)
