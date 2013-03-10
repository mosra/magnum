/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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
#include <TestSuite/Tester.h>
#include <Utility/Configuration.h>

#include "Math/Matrix3.h"

namespace Magnum { namespace Math { namespace Test {

class Matrix3Test: public Corrade::TestSuite::Tester {
    public:
        Matrix3Test();

        void construct();
        void constructIdentity();
        void constructZero();
        void constructConversion();
        void constructCopy();

        void translation();
        void scaling();
        void rotation();
        void reflection();
        void projection();
        void fromParts();
        void rotationScalingPart();
        void rotationPart();
        void vectorParts();
        void invertedEuclidean();
        void transform();

        void debug();
        void configuration();
};

typedef Math::Deg<Float> Deg;
typedef Math::Matrix3<Float> Matrix3;
typedef Math::Matrix3<Int> Matrix3i;
typedef Math::Matrix<2, Float> Matrix2;
typedef Math::Vector2<Float> Vector2;

Matrix3Test::Matrix3Test() {
    addTests({&Matrix3Test::construct,
              &Matrix3Test::constructIdentity,
              &Matrix3Test::constructZero,
              &Matrix3Test::constructConversion,
              &Matrix3Test::constructCopy,

              &Matrix3Test::translation,
              &Matrix3Test::scaling,
              &Matrix3Test::rotation,
              &Matrix3Test::reflection,
              &Matrix3Test::projection,
              &Matrix3Test::fromParts,
              &Matrix3Test::rotationScalingPart,
              &Matrix3Test::rotationPart,
              &Matrix3Test::vectorParts,
              &Matrix3Test::invertedEuclidean,
              &Matrix3Test::transform,

              &Matrix3Test::debug,
              &Matrix3Test::configuration});
}

void Matrix3Test::construct() {
    constexpr Matrix3 a({3.0f,  5.0f, 8.0f},
                        {4.5f,  4.0f, 7.0f},
                        {7.9f, -1.0f, 8.0f});
    CORRADE_COMPARE(a, Matrix3({3.0f,  5.0f, 8.0f},
                               {4.5f,  4.0f, 7.0f},
                               {7.9f, -1.0f, 8.0f}));
}

void Matrix3Test::constructIdentity() {
    constexpr Matrix3 identity;
    constexpr Matrix3 identity2(Matrix3::Identity);
    constexpr Matrix3 identity3(Matrix3::Identity, 4.0f);

    Matrix3 identityExpected({1.0f, 0.0f, 0.0f},
                             {0.0f, 1.0f, 0.0f},
                             {0.0f, 0.0f, 1.0f});

    Matrix3 identity3Expected({4.0f, 0.0f, 0.0f},
                              {0.0f, 4.0f, 0.0f},
                              {0.0f, 0.0f, 4.0f});

    CORRADE_COMPARE(identity, identityExpected);
    CORRADE_COMPARE(identity2, identityExpected);
    CORRADE_COMPARE(identity3, identity3Expected);
}

void Matrix3Test::constructZero() {
    constexpr Matrix3 a(Matrix3::Zero);
    CORRADE_COMPARE(a, Matrix3({0.0f, 0.0f, 0.0f},
                               {0.0f, 0.0f, 0.0f},
                               {0.0f, 0.0f, 0.0f}));
}

void Matrix3Test::constructConversion() {
    constexpr Matrix3 a({3.0f,  5.0f, 8.0f},
                        {4.5f,  4.0f, 7.0f},
                        {7.9f, -1.0f, 8.0f});
    #ifndef CORRADE_GCC46_COMPATIBILITY
    constexpr Matrix3i b(a);
    #else
    Matrix3i b(a); /* Not constexpr under GCC < 4.7 */
    #endif
    CORRADE_COMPARE(b, Matrix3i({3,  5, 8},
                                {4,  4, 7},
                                {7, -1, 8}));
}

void Matrix3Test::constructCopy() {
    constexpr Matrix3 a({3.0f,  5.0f, 8.0f},
                        {4.5f,  4.0f, 7.0f},
                        {7.9f, -1.0f, 8.0f});
    constexpr Matrix3 b(a);
    CORRADE_COMPARE(b, Matrix3({3.0f,  5.0f, 8.0f},
                               {4.5f,  4.0f, 7.0f},
                               {7.9f, -1.0f, 8.0f}));
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

    CORRADE_COMPARE(Matrix3::rotation(Deg(15.0f)), matrix);
}

void Matrix3Test::reflection() {
    std::ostringstream o;
    Error::setOutput(&o);

    Vector2 normal(-1.0f, 2.0f);

    CORRADE_COMPARE(Matrix3::reflection(normal), Matrix3());
    CORRADE_COMPARE(o.str(), "Math::Matrix3::reflection(): normal must be normalized\n");

    Matrix3 actual = Matrix3::reflection(normal.normalized());
    Matrix3 expected({0.6f,  0.8f, 0.0f},
                     {0.8f, -0.6f, 0.0f},
                     {0.0f,  0.0f, 1.0f});

    CORRADE_COMPARE(actual*actual, Matrix3());
    CORRADE_COMPARE(actual.transformVector(normal), -normal);
    CORRADE_COMPARE(actual, expected);
}

void Matrix3Test::projection() {
    Matrix3 expected({2.0f/4.0f,      0.0f, 0.0f},
                     {     0.0f, 2.0f/3.0f, 0.0f},
                     {     0.0f,      0.0f, 1.0f});

    CORRADE_COMPARE(Matrix3::projection({4.0f, 3.0f}), expected);
}

void Matrix3Test::fromParts() {
    constexpr Matrix2 rotationScaling(Vector2(3.0f, 5.0f),
                                      Vector2(4.0f, 4.0f));
    constexpr Vector2 translation(7.0f, -1.0f);
    constexpr Matrix3 a = Matrix3::from(rotationScaling, translation);

    CORRADE_COMPARE(a, Matrix3({3.0f,  5.0f, 0.0f},
                               {4.0f,  4.0f, 0.0f},
                               {7.0f, -1.0f, 1.0f}));
}

void Matrix3Test::rotationScalingPart() {
    constexpr Matrix3 a({3.0f,  5.0f, 8.0f},
                        {4.0f,  4.0f, 7.0f},
                        {7.0f, -1.0f, 8.0f});
    constexpr Matrix2 b = a.rotationScaling();

    CORRADE_COMPARE(b, Matrix2(Vector2(3.0f, 5.0f),
                               Vector2(4.0f, 4.0f)));
}

void Matrix3Test::rotationPart() {
    Matrix3 rotation = Matrix3::rotation(Deg(15.0f));
    Matrix2 expectedRotationPart(Vector2( 0.965926f, 0.258819f),
                                 Vector2(-0.258819f, 0.965926f));

    /* For rotation and translation this is the same as rotationScaling() */
    Matrix3 rotationTranslation = rotation*Matrix3::translation({2.0f, 5.0f});
    Matrix2 rotationTranslationPart = rotationTranslation.rotation();
    CORRADE_COMPARE(rotationTranslationPart, rotationTranslation.rotationScaling());
    CORRADE_COMPARE(rotationTranslationPart, expectedRotationPart);

    /* Test uniform scaling */
    Matrix3 rotationScaling = rotation*Matrix3::scaling(Vector2(9.0f));
    Matrix2 rotationScalingPart = rotationScaling.rotation();
    CORRADE_COMPARE(rotationScalingPart.determinant(), 1.0f);
    CORRADE_COMPARE(rotationScalingPart*rotationScalingPart.transposed(), Matrix2());
    CORRADE_COMPARE(rotationScalingPart, expectedRotationPart);

    /* Fails on non-uniform scaling */
    {
        CORRADE_EXPECT_FAIL("Assertion on uniform scaling is not implemented yet.");
        std::ostringstream o;
        Error::setOutput(&o);
        Matrix3 rotationScaling2 = rotation*Matrix3::scaling(Vector2::yScale(3.5f));
        CORRADE_COMPARE(o.str(), "Math::Matrix3::rotation(): the matrix doesn't have uniform scaling\n");
        CORRADE_COMPARE(rotationScaling2, Matrix3(Matrix3::Zero));
    }
}

void Matrix3Test::vectorParts() {
    constexpr Matrix3 a({15.0f,  0.0f, 0.0f},
                        { 0.0f, -3.0f, 0.0f},
                        {-5.0f, 12.0f, 1.0f});
    constexpr Vector2 right = a.right();
    constexpr Vector2 up = a.up();
    constexpr Vector2 translation = a.translation();

    CORRADE_COMPARE(right, Vector2::xAxis(15.0f));
    CORRADE_COMPARE(up, Vector2::yAxis(-3.0f));
    CORRADE_COMPARE(translation, Vector2(-5.0f, 12.0f));
}

void Matrix3Test::invertedEuclidean() {
    std::ostringstream o;
    Error::setOutput(&o);

    Matrix3 m({3.0f,  5.0f, 8.0f},
              {4.0f,  4.0f, 7.0f},
              {7.0f, -1.0f, 8.0f});
    CORRADE_COMPARE(m.invertedEuclidean(), Matrix3());
    CORRADE_COMPARE(o.str(), "Math::Matrix3::invertedEuclidean(): unexpected values on last row\n");

    o.str({});
    CORRADE_COMPARE(Matrix3::scaling(Vector2(2.0f)).invertedEuclidean(), Matrix3());
    CORRADE_COMPARE(o.str(), "Math::Matrix3::invertedEuclidean(): the matrix doesn't represent Euclidean transformation\n");

    Matrix3 actual = Matrix3::rotation(Deg(-74.0f))*
                     Matrix3::reflection(Vector2(0.5f, -2.0f).normalized())*
                     Matrix3::translation({2.0f, -3.0f});
    Matrix3 expected = Matrix3::translation({-2.0f, 3.0f})*
                       Matrix3::reflection(Vector2(0.5f, -2.0f).normalized())*
                       Matrix3::rotation(Deg(74.0f));

    CORRADE_COMPARE(actual.invertedEuclidean(), expected);
    CORRADE_COMPARE(actual.invertedEuclidean(), actual.inverted());
}

void Matrix3Test::transform() {
    Matrix3 a = Matrix3::translation({1.0f, -5.0f})*Matrix3::rotation(Deg(90.0f));
    Vector2 v(1.0f, -2.0f);

    CORRADE_COMPARE(a.transformVector(v), Vector2(2.0f, 1.0f));
    CORRADE_COMPARE(a.transformPoint(v), Vector2(3.0f, -4.0f));
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

void Matrix3Test::configuration() {
    Corrade::Utility::Configuration c;

    Matrix3 m({5.0f, 8.0f,   4.0f},
              {4.0f, 7.0f, 3.125f},
              {4.0f, 5.0f,  9.55f});
    std::string value("5 4 4 8 7 5 4 3.125 9.55");

    c.setValue("matrix", m);
    CORRADE_COMPARE(c.value("matrix"), value);
    CORRADE_COMPARE(c.value<Matrix3>("matrix"), m);
}

}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::Matrix3Test)
