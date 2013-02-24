/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

#include <sstream>
#include <TestSuite/Tester.h>
#include <Utility/Configuration.h>

#include "Math/Matrix3.h"

namespace Magnum { namespace Math { namespace Test {

class Matrix3Test: public Corrade::TestSuite::Tester {
    public:
        Matrix3Test();

        void constructIdentity();

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

typedef Math::Deg<float> Deg;
typedef Math::Matrix3<float> Matrix3;
typedef Math::Matrix<2, float> Matrix2;
typedef Math::Vector2<float> Vector2;

Matrix3Test::Matrix3Test() {
    addTests(&Matrix3Test::constructIdentity,

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
             &Matrix3Test::configuration);
}

void Matrix3Test::constructIdentity() {
    Matrix3 identity;
    Matrix3 identity2(Matrix3::Identity);
    Matrix3 identity3(Matrix3::Identity, 4.0f);

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

void Matrix3Test::translation() {
    Matrix3 matrix({1.0f, 0.0f, 0.0f},
                   {0.0f, 1.0f, 0.0f},
                   {3.0f, 1.0f, 1.0f});

    CORRADE_COMPARE(Matrix3::translation({3.0f, 1.0f}), matrix);
}

void Matrix3Test::scaling() {
    Matrix3 matrix({3.0f, 0.0f, 0.0f},
                   {0.0f, 1.5f, 0.0f},
                   {0.0f, 0.0f, 1.0f});

    CORRADE_COMPARE(Matrix3::scaling({3.0f, 1.5f}), matrix);
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
    Matrix2 rotationScaling(Vector2(3.0f, 5.0f),
                            Vector2(4.0f, 4.0f));

    Vector2 translation(7.0f, -1.0f);

    Matrix3 expected({3.0f,  5.0f, 0.0f},
                     {4.0f,  4.0f, 0.0f},
                     {7.0f, -1.0f, 1.0f});

    CORRADE_COMPARE(Matrix3::from(rotationScaling, translation), expected);
}

void Matrix3Test::rotationScalingPart() {
    Matrix3 m({3.0f,  5.0f, 8.0f},
              {4.0f,  4.0f, 7.0f},
              {7.0f, -1.0f, 8.0f});

    Matrix2 expected(Vector2(3.0f, 5.0f),
                     Vector2(4.0f, 4.0f));

    CORRADE_COMPARE(m.rotationScaling(), expected);
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
    Matrix3 m({15.0f,  0.0f, 0.0f},
              { 0.0f, -3.0f, 0.0f},
              {-5.0f, 12.0f, 1.0f});

    CORRADE_COMPARE(m.right(), Vector2::xAxis(15.0f));
    CORRADE_COMPARE(m.up(), Vector2::yAxis(-3.0f));
    CORRADE_COMPARE(m.translation(), Vector2(-5.0f, 12.0f));
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
