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

#include "Math/Matrix4.h"

namespace Magnum { namespace Math { namespace Test {

class Matrix4Test: public Corrade::TestSuite::Tester {
    public:
        Matrix4Test();

        void constructIdentity();

        void translation();
        void scaling();
        void rotation();
        void rotationX();
        void rotationY();
        void rotationZ();
        void reflection();
        void orthographicProjection();
        void perspectiveProjection();
        void perspectiveProjectionFov();
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
typedef Math::Rad<float> Rad;
typedef Math::Matrix4<float> Matrix4;
typedef Math::Matrix<3, float> Matrix3;
typedef Math::Vector3<float> Vector3;

Matrix4Test::Matrix4Test() {
    addTests(&Matrix4Test::constructIdentity,

             &Matrix4Test::translation,
             &Matrix4Test::scaling,
             &Matrix4Test::rotation,
             &Matrix4Test::rotationX,
             &Matrix4Test::rotationY,
             &Matrix4Test::rotationZ,
             &Matrix4Test::reflection,
             &Matrix4Test::orthographicProjection,
             &Matrix4Test::perspectiveProjection,
             &Matrix4Test::perspectiveProjectionFov,
             &Matrix4Test::fromParts,
             &Matrix4Test::rotationScalingPart,
             &Matrix4Test::rotationPart,
             &Matrix4Test::vectorParts,
             &Matrix4Test::invertedEuclidean,
             &Matrix4Test::transform,

             &Matrix4Test::debug,
             &Matrix4Test::configuration);
}

void Matrix4Test::constructIdentity() {
    Matrix4 identity;
    Matrix4 identity2(Matrix4::Identity);
    Matrix4 identity3(Matrix4::Identity, 4.0f);

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
}

void Matrix4Test::translation() {
    Matrix4 matrix({1.0f, 0.0f, 0.0f, 0.0f},
                   {0.0f, 1.0f, 0.0f, 0.0f},
                   {0.0f, 0.0f, 1.0f, 0.0f},
                   {3.0f, 1.0f, 2.0f, 1.0f});

    CORRADE_COMPARE(Matrix4::translation({3.0f, 1.0f, 2.0f}), matrix);
}

void Matrix4Test::scaling() {
    Matrix4 matrix({3.0f, 0.0f, 0.0f, 0.0f},
                   {0.0f, 1.5f, 0.0f, 0.0f},
                   {0.0f, 0.0f, 2.0f, 0.0f},
                   {0.0f, 0.0f, 0.0f, 1.0f});

    CORRADE_COMPARE(Matrix4::scaling({3.0f, 1.5f, 2.0f}), matrix);
}

void Matrix4Test::rotation() {
    std::ostringstream o;
    Error::setOutput(&o);

    CORRADE_COMPARE(Matrix4::rotation(Deg(-74.0f), {-1.0f, 2.0f, 2.0f}), Matrix4());
    CORRADE_COMPARE(o.str(), "Math::Matrix4::rotation(): axis must be normalized\n");

    Matrix4 matrix({ 0.35612214f,  -0.80181062f, 0.47987163f, 0.0f},
                   { 0.47987163f,   0.59757638f,  0.6423595f, 0.0f},
                   {-0.80181062f, 0.0015183985f, 0.59757638f, 0.0f},
                   {        0.0f,          0.0f,        0.0f, 1.0f});
    CORRADE_COMPARE(Matrix4::rotation(Deg(-74.0f), Vector3(-1.0f, 2.0f, 2.0f).normalized()), matrix);
}

void Matrix4Test::rotationX() {
    Matrix4 matrix({1.0f,         0.0f,        0.0f, 0.0f},
                   {0.0f,  0.90096887f, 0.43388374f, 0.0f},
                   {0.0f, -0.43388374f, 0.90096887f, 0.0f},
                   {0.0f,         0.0f,        0.0f, 1.0f});
    CORRADE_COMPARE(Matrix4::rotation(Rad(Math::Constants<float>::pi()/7), Vector3::xAxis()), matrix);
    CORRADE_COMPARE(Matrix4::rotationX(Rad(Math::Constants<float>::pi()/7)), matrix);
}

void Matrix4Test::rotationY() {
    Matrix4 matrix({0.90096887f, 0.0f, -0.43388374f, 0.0f},
                   {       0.0f, 1.0f,         0.0f, 0.0f},
                   {0.43388374f, 0.0f,  0.90096887f, 0.0f},
                   {       0.0f, 0.0f,         0.0f, 1.0f});
    CORRADE_COMPARE(Matrix4::rotation(Rad(Math::Constants<float>::pi()/7), Vector3::yAxis()), matrix);
    CORRADE_COMPARE(Matrix4::rotationY(Rad(Math::Constants<float>::pi()/7)), matrix);
}

void Matrix4Test::rotationZ() {
    Matrix4 matrix({ 0.90096887f, 0.43388374f, 0.0f, 0.0f},
                   {-0.43388374f, 0.90096887f, 0.0f, 0.0f},
                   {        0.0f,        0.0f, 1.0f, 0.0f},
                   {        0.0f,        0.0f, 0.0f, 1.0f});
    CORRADE_COMPARE(Matrix4::rotation(Rad(Math::Constants<float>::pi()/7), Vector3::zAxis()), matrix);
    CORRADE_COMPARE(Matrix4::rotationZ(Rad(Math::Constants<float>::pi()/7)), matrix);
}

void Matrix4Test::reflection() {
    std::ostringstream o;
    Error::setOutput(&o);

    Vector3 normal(-1.0f, 2.0f, 2.0f);

    CORRADE_COMPARE(Matrix4::reflection(normal), Matrix4());
    CORRADE_COMPARE(o.str(), "Math::Matrix4::reflection(): normal must be normalized\n");

    Matrix4 actual = Matrix4::reflection(normal.normalized());
    Matrix4 expected({0.777778f,  0.444444f,  0.444444f, 0.0f},
                     {0.444444f,  0.111111f, -0.888889f, 0.0f},
                     {0.444444f, -0.888889f,  0.111111f, 0.0f},
                     {     0.0f,       0.0f,       0.0f, 1.0f});

    CORRADE_COMPARE(actual*actual, Matrix4());
    CORRADE_COMPARE(actual.transformVector(normal), -normal);
    CORRADE_COMPARE(actual, expected);
}

void Matrix4Test::orthographicProjection() {
    Matrix4 expected({0.4f, 0.0f,   0.0f, 0.0f},
                     {0.0f, 0.5f,   0.0f, 0.0f},
                     {0.0f, 0.0f, -0.25f, 0.0f},
                     {0.0f, 0.0f, -1.25f, 1.0f});
    CORRADE_COMPARE(Matrix4::orthographicProjection({5.0f, 4.0f}, 1, 9), expected);
}

void Matrix4Test::perspectiveProjection() {
    Matrix4 expected({4.0f,      0.0f,         0.0f,  0.0f},
                     {0.0f, 7.111111f,         0.0f,  0.0f},
                     {0.0f,      0.0f,  -1.9411764f, -1.0f},
                     {0.0f,      0.0f, -94.1176452f,  0.0f});
    CORRADE_COMPARE(Matrix4::perspectiveProjection({16.0f, 9.0f}, 32.0f, 100), expected);
}

void Matrix4Test::perspectiveProjectionFov() {
    Matrix4 expected({4.1652994f,      0.0f,         0.0f,  0.0f},
                     {      0.0f, 9.788454f,         0.0f,  0.0f},
                     {      0.0f,      0.0f,  -1.9411764f, -1.0f},
                     {      0.0f,      0.0f, -94.1176452f,  0.0f});
    CORRADE_COMPARE(Matrix4::perspectiveProjection(Deg(27.0f), 2.35f, 32.0f, 100), expected);
}

void Matrix4Test::fromParts() {
    Matrix3 rotationScaling(Vector3(3.0f,  5.0f, 8.0f),
                            Vector3(4.0f,  4.0f, 7.0f),
                            Vector3(7.0f, -1.0f, 8.0f));

    Vector3 translation(9.0f, 4.0f, 5.0f);

    Matrix4 expected({3.0f,  5.0f, 8.0f, 0.0f},
                     {4.0f,  4.0f, 7.0f, 0.0f},
                     {7.0f, -1.0f, 8.0f, 0.0f},
                     {9.0f,  4.0f, 5.0f, 1.0f});
    CORRADE_COMPARE(Matrix4::from(rotationScaling, translation), expected);
}

void Matrix4Test::rotationScalingPart() {
    Matrix4 m({3.0f,  5.0f, 8.0f, 4.0f},
              {4.0f,  4.0f, 7.0f, 3.0f},
              {7.0f, -1.0f, 8.0f, 0.0f},
              {9.0f,  4.0f, 5.0f, 9.0f});

    Matrix3 expected(Vector3(3.0f, 5.0f, 8.0f),
                     Vector3(4.0f, 4.0f, 7.0f),
                     Vector3(7.0f, -1.0f, 8.0f));
    CORRADE_COMPARE(m.rotationScaling(), expected);
}

void Matrix4Test::rotationPart() {
    Matrix3 expectedRotationPart(Vector3( 0.35612214f,  -0.80181062f, 0.47987163f),
                                 Vector3( 0.47987163f,   0.59757638f,  0.6423595f),
                                 Vector3(-0.80181062f, 0.0015183985f, 0.59757638f));

    Matrix4 rotation = Matrix4::rotation(Deg(-74.0f), Vector3(-1.0f, 2.0f, 2.0f).normalized());
    CORRADE_COMPARE(rotation.rotation().determinant(), 1.0f);
    CORRADE_COMPARE(rotation.rotation()*rotation.rotation().transposed(), Matrix3());
    CORRADE_COMPARE(rotation.rotation(), expectedRotationPart);

    Matrix4 rotationTransformed = Matrix4::translation({2.0f, 5.0f, -3.0f})*rotation*Matrix4::scaling(Vector3(9.0f));
    CORRADE_COMPARE(rotationTransformed.rotation().determinant(), 1.0f);
    CORRADE_COMPARE(rotationTransformed.rotation()*rotationTransformed.rotation().transposed(), Matrix3());
    CORRADE_COMPARE(rotationTransformed.rotation(), expectedRotationPart);
}

void Matrix4Test::vectorParts() {
    Matrix4 m({-1.0f,  0.0f,  0.0f, 0.0f},
              { 0.0f, 12.0f,  0.0f, 0.0f},
              { 0.0f,  0.0f, 35.0f, 0.0f},
              {-5.0f, 12.0f,  0.5f, 1.0f});

    CORRADE_COMPARE(m.right(), Vector3::xAxis(-1.0f));
    CORRADE_COMPARE(m.up(), Vector3::yAxis(12.0f));
    CORRADE_COMPARE(m.backward(), Vector3::zAxis(35.0f));
    CORRADE_COMPARE(m.translation(), Vector3(-5.0f, 12.0f, 0.5f));
}

void Matrix4Test::invertedEuclidean() {
    std::ostringstream o;
    Error::setOutput(&o);

    Matrix4 m({3.0f,  5.0f, 8.0f, 4.0f},
              {4.0f,  4.0f, 7.0f, 3.0f},
              {7.0f, -1.0f, 8.0f, 0.0f},
              {9.0f,  4.0f, 5.0f, 9.0f});
    CORRADE_COMPARE(m.invertedEuclidean(), Matrix4());
    CORRADE_COMPARE(o.str(), "Math::Matrix4::invertedEuclidean(): unexpected values on last row\n");

    o.str("");
    CORRADE_COMPARE(Matrix4::scaling(Vector3(2.0f)).invertedEuclidean(), Matrix4());
    CORRADE_COMPARE(o.str(), "Math::Matrix4::invertedEuclidean(): the matrix doesn't represent Euclidean transformation\n");

    Matrix4 actual = Matrix4::rotation(Deg(-74.0f), Vector3(-1.0f, 0.5f, 2.0f).normalized())*
                     Matrix4::reflection(Vector3(0.5f, -2.0f, 2.0f).normalized())*
                     Matrix4::translation({1.0f, 2.0f, -3.0f});
    Matrix4 expected = Matrix4::translation({-1.0f, -2.0f, 3.0f})*
                       Matrix4::reflection(Vector3(0.5f, -2.0f, 2.0f).normalized())*
                       Matrix4::rotation(Deg(74.0f), Vector3(-1.0f, 0.5f, 2.0f).normalized());

    CORRADE_COMPARE(actual.invertedEuclidean(), expected);
    CORRADE_COMPARE(actual.invertedEuclidean(), actual.inverted());
}

void Matrix4Test::transform() {
    Matrix4 a = Matrix4::translation({1.0f, -5.0f, 3.5f})*Matrix4::rotation(Deg(90.0f), Vector3::zAxis());
    Vector3 v(1.0f, -2.0f, 5.5f);

    CORRADE_COMPARE(a.transformVector(v), Vector3(2.0f, 1.0f, 5.5f));
    CORRADE_COMPARE(a.transformPoint(v), Vector3(3.0f, -4.0f, 9.0f));
}

void Matrix4Test::debug() {
    Matrix4 m({3.0f,  5.0f, 8.0f, 4.0f},
              {4.0f,  4.0f, 7.0f, 3.0f},
              {7.0f, -1.0f, 8.0f, 0.0f},
              {9.0f,  4.0f, 5.0f, 9.0f});

    std::ostringstream o;
    Debug(&o) << m;
    CORRADE_COMPARE(o.str(), "Matrix(3, 4, 7, 9,\n"
                             "       5, 4, -1, 4,\n"
                             "       8, 7, 8, 5,\n"
                             "       4, 3, 0, 9)\n");
}

void Matrix4Test::configuration() {
    Corrade::Utility::Configuration c;

    Matrix4 m({3.0f,  5.0f, 8.0f,   4.0f},
              {4.0f,  4.0f, 7.0f, 3.125f},
              {7.0f, -1.0f, 8.0f,   0.0f},
              {9.0f,  4.0f, 5.0f,  9.55f});
    std::string value("3 4 7 9 5 4 -1 4 8 7 8 5 4 3.125 0 9.55");

    c.setValue("matrix", m);
    CORRADE_COMPARE(c.value("matrix"), value);
    CORRADE_COMPARE(c.value<Matrix4>("matrix"), m);
}

}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::Matrix4Test)
