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

#include "Math/Matrix4.h"

struct Mat4 {
    float a[16];
};

namespace Magnum { namespace Math {

namespace Implementation {

template<> struct RectangularMatrixConverter<4, 4, float, Mat4> {
    inline constexpr static RectangularMatrix<4, 4, Float> from(const Mat4& other) {
        return RectangularMatrix<4, 4, Float>(
            Vector<4, Float>(other.a[0], other.a[1], other.a[2], other.a[3]),
            Vector<4, Float>(other.a[4], other.a[5], other.a[6], other.a[7]),
            Vector<4, Float>(other.a[8], other.a[9], other.a[10], other.a[11]),
            Vector<4, Float>(other.a[12], other.a[13], other.a[14], other.a[15]));
    }

    inline constexpr static Mat4 to(const RectangularMatrix<4, 4, Float>& other) {
        return Mat4{{other[0][0], other[0][1], other[0][2], other[0][3],
                     other[1][0], other[1][1], other[1][2], other[1][3],
                     other[2][0], other[2][1], other[2][2], other[2][3],
                     other[3][0], other[3][1], other[3][2], other[3][3]}};
    }
};

}

namespace Test {

class Matrix4Test: public Corrade::TestSuite::Tester {
    public:
        Matrix4Test();

        void construct();
        void constructIdentity();
        void constructZero();
        void constructConversion();
        void constructCopy();

        void convert();

        void isRigidTransformation();

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
        void invertedRigid();
        void transform();

        void debug();
        void configuration();
};

typedef Math::Deg<Float> Deg;
typedef Math::Rad<Float> Rad;
typedef Math::Matrix4<Float> Matrix4;
typedef Math::Matrix4<Int> Matrix4i;
typedef Math::Matrix<3, Float> Matrix3;
typedef Math::Vector3<Float> Vector3;

Matrix4Test::Matrix4Test() {
    addTests({&Matrix4Test::construct,
              &Matrix4Test::constructIdentity,
              &Matrix4Test::constructZero,
              &Matrix4Test::constructConversion,
              &Matrix4Test::constructCopy,

              &Matrix4Test::convert,

              &Matrix4Test::isRigidTransformation,

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
              &Matrix4Test::invertedRigid,
              &Matrix4Test::transform,

              &Matrix4Test::debug,
              &Matrix4Test::configuration});
}

void Matrix4Test::construct() {
    constexpr Matrix4 a({3.0f,  5.0f, 8.0f, -3.0f},
                        {4.5f,  4.0f, 7.0f,  2.0f},
                        {1.0f,  2.0f, 3.0f, -1.0f},
                        {7.9f, -1.0f, 8.0f, -1.5f});
    CORRADE_COMPARE(a, Matrix4({3.0f,  5.0f, 8.0f, -3.0f},
                               {4.5f,  4.0f, 7.0f,  2.0f},
                               {1.0f,  2.0f, 3.0f, -1.0f},
                               {7.9f, -1.0f, 8.0f, -1.5f}));
}

void Matrix4Test::constructIdentity() {
    constexpr Matrix4 identity;
    constexpr Matrix4 identity2(Matrix4::Identity);
    constexpr Matrix4 identity3(Matrix4::Identity, 4.0f);

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

void Matrix4Test::constructZero() {
    /* Zero constructor */
    constexpr Matrix4 a(Matrix4::Zero);
    CORRADE_COMPARE(a, Matrix4({0.0f, 0.0f, 0.0f, 0.0f},
                               {0.0f, 0.0f, 0.0f, 0.0f},
                               {0.0f, 0.0f, 0.0f, 0.0f},
                               {0.0f, 0.0f, 0.0f, 0.0f}));
}

void Matrix4Test::constructConversion() {
    constexpr Matrix4 a({3.0f,  5.0f, 8.0f, -3.0f},
                        {4.5f,  4.0f, 7.0f,  2.0f},
                        {1.0f,  2.0f, 3.0f, -1.0f},
                        {7.9f, -1.0f, 8.0f, -1.5f});
    #ifndef CORRADE_GCC46_COMPATIBILITY
    constexpr Matrix4i b(a);
    #else
    Matrix4i b(a); /* Not constexpr under GCC < 4.7 */
    #endif
    CORRADE_COMPARE(b, Matrix4i({3,  5, 8, -3},
                                {4,  4, 7,  2},
                                {1,  2, 3, -1},
                                {7, -1, 8, -1}));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Matrix4, Matrix4i>::value));
}

void Matrix4Test::constructCopy() {
    constexpr Matrix4 a({3.0f,  5.0f, 8.0f, -3.0f},
                        {4.5f,  4.0f, 7.0f,  2.0f},
                        {1.0f,  2.0f, 3.0f, -1.0f},
                        {7.9f, -1.0f, 8.0f, -1.5f});
    constexpr Matrix4 b(a);
    CORRADE_COMPARE(b, Matrix4({3.0f,  5.0f, 8.0f, -3.0f},
                               {4.5f,  4.0f, 7.0f,  2.0f},
                               {1.0f,  2.0f, 3.0f, -1.0f},
                               {7.9f, -1.0f, 8.0f, -1.5f}));
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

    #ifndef CORRADE_GCC46_COMPATIBILITY
    constexpr /* Not constexpr under GCC < 4.7 */
    #endif
    Mat4 d(b);
    for(std::size_t i = 0; i != 16; ++i)
        CORRADE_COMPARE(d.a[i], a.a[i]);

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Mat4, Matrix4>::value));
    CORRADE_VERIFY(!(std::is_convertible<Matrix4, Mat4>::value));
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
    CORRADE_COMPARE(Matrix4::rotation(Rad(Math::Constants<Float>::pi()/7), Vector3::xAxis()), matrix);
    CORRADE_COMPARE(Matrix4::rotationX(Rad(Math::Constants<Float>::pi()/7)), matrix);
}

void Matrix4Test::rotationY() {
    Matrix4 matrix({0.90096887f, 0.0f, -0.43388374f, 0.0f},
                   {       0.0f, 1.0f,         0.0f, 0.0f},
                   {0.43388374f, 0.0f,  0.90096887f, 0.0f},
                   {       0.0f, 0.0f,         0.0f, 1.0f});
    CORRADE_COMPARE(Matrix4::rotation(Rad(Math::Constants<Float>::pi()/7), Vector3::yAxis()), matrix);
    CORRADE_COMPARE(Matrix4::rotationY(Rad(Math::Constants<Float>::pi()/7)), matrix);
}

void Matrix4Test::rotationZ() {
    Matrix4 matrix({ 0.90096887f, 0.43388374f, 0.0f, 0.0f},
                   {-0.43388374f, 0.90096887f, 0.0f, 0.0f},
                   {        0.0f,        0.0f, 1.0f, 0.0f},
                   {        0.0f,        0.0f, 0.0f, 1.0f});
    CORRADE_COMPARE(Matrix4::rotation(Rad(Math::Constants<Float>::pi()/7), Vector3::zAxis()), matrix);
    CORRADE_COMPARE(Matrix4::rotationZ(Rad(Math::Constants<Float>::pi()/7)), matrix);
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
    constexpr Matrix3 rotationScaling(Vector3(3.0f,  5.0f, 8.0f),
                                      Vector3(4.0f,  4.0f, 7.0f),
                                      Vector3(7.0f, -1.0f, 8.0f));
    constexpr Vector3 translation(9.0f, 4.0f, 5.0f);
    constexpr Matrix4 a = Matrix4::from(rotationScaling, translation);

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
    constexpr Matrix3 b = a.rotationScaling();

    CORRADE_COMPARE(b, Matrix3(Vector3(3.0f, 5.0f, 8.0f),
                               Vector3(4.0f, 4.0f, 7.0f),
                               Vector3(7.0f, -1.0f, 8.0f)));
}

void Matrix4Test::rotationPart() {
    Matrix4 rotation = Matrix4::rotation(Deg(-74.0f), Vector3(-1.0f, 2.0f, 2.0f).normalized());
    Matrix3 expectedRotationPart(Vector3( 0.35612214f,  -0.80181062f, 0.47987163f),
                                 Vector3( 0.47987163f,   0.59757638f,  0.6423595f),
                                 Vector3(-0.80181062f, 0.0015183985f, 0.59757638f));

    /* For rotation and translation this is the same as rotationScaling() */
    Matrix4 rotationTranslation = rotation*Matrix4::translation({2.0f, 5.0f, -3.0f});
    Matrix3 rotationTranslationPart = rotationTranslation.rotation();
    CORRADE_COMPARE(rotationTranslationPart, rotationTranslation.rotationScaling());
    CORRADE_COMPARE(rotationTranslationPart, expectedRotationPart);

    /* Test uniform scaling */
    Matrix4 rotationScaling = rotation*Matrix4::scaling(Vector3(9.0f));
    Matrix3 rotationScalingPart = rotationScaling.rotation();
    CORRADE_COMPARE(rotationScalingPart.determinant(), 1.0f);
    CORRADE_COMPARE(rotationScalingPart*rotationScalingPart.transposed(), Matrix3());
    CORRADE_COMPARE(rotationScalingPart, expectedRotationPart);

    /* Fails on non-uniform scaling */
    {
        CORRADE_EXPECT_FAIL("Assertion on uniform scaling is not implemented yet.");
        std::ostringstream o;
        Error::setOutput(&o);
        Matrix4 rotationScaling2 = rotation*Matrix4::scaling(Vector3::yScale(3.5f));
        CORRADE_COMPARE(o.str(), "Math::Matrix4::rotation(): the matrix doesn't have uniform scaling\n");
        CORRADE_COMPARE(rotationScaling2, Matrix4(Matrix4::Zero));
    }
}

void Matrix4Test::vectorParts() {
    constexpr Matrix4 a({-1.0f,  0.0f,  0.0f, 0.0f},
                        { 0.0f, 12.0f,  0.0f, 0.0f},
                        { 0.0f,  0.0f, 35.0f, 0.0f},
                        {-5.0f, 12.0f,  0.5f, 1.0f});
    constexpr Vector3 right = a.right();
    constexpr Vector3 up = a.up();
    constexpr Vector3 backward = a.backward();
    constexpr Vector3 translation = a.translation();

    CORRADE_COMPARE(right, Vector3::xAxis(-1.0f));
    CORRADE_COMPARE(up, Vector3::yAxis(12.0f));
    CORRADE_COMPARE(backward, Vector3::zAxis(35.0f));
    CORRADE_COMPARE(translation, Vector3(-5.0f, 12.0f, 0.5f));
}

void Matrix4Test::invertedRigid() {
    Matrix4 actual = Matrix4::rotation(Deg(-74.0f), Vector3(-1.0f, 0.5f, 2.0f).normalized())*
                     Matrix4::reflection(Vector3(0.5f, -2.0f, 2.0f).normalized())*
                     Matrix4::translation({1.0f, 2.0f, -3.0f});
    Matrix4 expected = Matrix4::translation({-1.0f, -2.0f, 3.0f})*
                       Matrix4::reflection(Vector3(0.5f, -2.0f, 2.0f).normalized())*
                       Matrix4::rotation(Deg(74.0f), Vector3(-1.0f, 0.5f, 2.0f).normalized());

    std::ostringstream o;
    Error::setOutput(&o);
    (2*actual).invertedRigid();
    CORRADE_COMPARE(o.str(), "Math::Matrix4::invertedRigid(): the matrix doesn't represent rigid transformation\n");

    CORRADE_COMPARE(actual.invertedRigid(), expected);
    CORRADE_COMPARE(actual.invertedRigid(), actual.inverted());
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
