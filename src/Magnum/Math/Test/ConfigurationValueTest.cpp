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

#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/Configuration.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Math/ConfigurationValue.h"

namespace Magnum { namespace Math { namespace Test { namespace {

struct ConfigurationValueTest: Corrade::TestSuite::Tester {
    explicit ConfigurationValueTest();

    void deg();
    void rad();

    void vector();
    void vector2();
    void vector3();
    void vector4();
    void color();

    void rectangularMatrix();
    void matrix();
    void matrix3();
    void matrix4();

    void range();

    void complex();
    void dualComplex();
    void quaternion();
    void dualQuaternion();

    void bezier();
};

ConfigurationValueTest::ConfigurationValueTest() {
    addTests({&ConfigurationValueTest::deg,
              &ConfigurationValueTest::rad,

              &ConfigurationValueTest::vector,
              &ConfigurationValueTest::vector2,
              &ConfigurationValueTest::vector3,
              &ConfigurationValueTest::vector4,
              &ConfigurationValueTest::color,

              &ConfigurationValueTest::rectangularMatrix,
              &ConfigurationValueTest::matrix,
              &ConfigurationValueTest::matrix3,
              &ConfigurationValueTest::matrix4,

              &ConfigurationValueTest::range,

              &ConfigurationValueTest::complex,
              &ConfigurationValueTest::dualComplex,
              &ConfigurationValueTest::quaternion,
              &ConfigurationValueTest::dualQuaternion,

              &ConfigurationValueTest::bezier});
}

void ConfigurationValueTest::deg() {
    typedef Math::Deg<Float> Deg;

    Corrade::Utility::Configuration c;

    Deg angle{25.3f};
    std::string value("25.3");

    c.setValue("angle", angle);
    CORRADE_COMPARE(c.value("angle"), value);
    CORRADE_COMPARE(c.value<Deg>("angle"), angle);
}

void ConfigurationValueTest::rad() {
    typedef Math::Rad<Float> Rad;

    Corrade::Utility::Configuration c;

    Rad angle{3.14159f};
    std::string value("3.14159");

    c.setValue("angle", angle);
    CORRADE_COMPARE(c.value("angle"), value);
    CORRADE_COMPARE(c.value<Rad>("angle"), angle);
}

void ConfigurationValueTest::vector() {
    typedef Vector<4, Float> Vector4;

    Corrade::Utility::Configuration c;

    Vector4 vec(3.0f, 3.125f, 9.0f, 9.55f);
    std::string value("3 3.125 9 9.55");

    c.setValue("vector", vec);
    CORRADE_COMPARE(c.value("vector"), value);
    CORRADE_COMPARE(c.value<Vector4>("vector"), vec);

    /* Underflow */
    c.setValue("underflow", "2.1 8.9");
    CORRADE_COMPARE(c.value<Vector4>("underflow"), (Vector4{2.1f, 8.9f, 0.0f, 0.0f}));

    /* Overflow */
    c.setValue("overflow", "2 1 8 9 16 33");
    CORRADE_COMPARE(c.value<Vector4>("overflow"), (Vector4{2.0f, 1.0f, 8.0f, 9.0f}));
}

void ConfigurationValueTest::vector2() {
    typedef Math::Vector2<Float> Vector2;

    Corrade::Utility::Configuration c;

    Vector2 vec(3.125f, 9.0f);
    std::string value("3.125 9");

    c.setValue("vector", vec);
    CORRADE_COMPARE(c.value("vector"), value);
    CORRADE_COMPARE(c.value<Vector2>("vector"), vec);
}

void ConfigurationValueTest::vector3() {
    typedef Math::Vector3<Float> Vector3;

    Corrade::Utility::Configuration c;

    Vector3 vec(3.0f, 3.125f, 9.55f);
    std::string value("3 3.125 9.55");

    c.setValue("vector", vec);
    CORRADE_COMPARE(c.value("vector"), value);
    CORRADE_COMPARE(c.value<Vector3>("vector"), vec);
}

void ConfigurationValueTest::vector4() {
    typedef Math::Vector4<Float> Vector4;

    Corrade::Utility::Configuration c;

    Vector4 vec(3.0f, 3.125f, 9.0f, 9.55f);
    std::string value("3 3.125 9 9.55");

    c.setValue("vector", vec);
    CORRADE_COMPARE(c.value("vector"), value);
    CORRADE_COMPARE(c.value<Vector4>("vector"), vec);
}

void ConfigurationValueTest::color() {
    typedef Math::Color3<Float> Color3;
    typedef Math::Color4<Float> Color4;

    Corrade::Utility::Configuration c;

    Color3 color3(0.5f, 0.75f, 1.0f);
    std::string value3("0.5 0.75 1");

    c.setValue("color3", color3);
    CORRADE_COMPARE(c.value("color3"), value3);
    CORRADE_COMPARE(c.value<Color3>("color3"), color3);

    Color4 color4(0.5f, 0.75f, 0.0f, 1.0f);
    std::string value4("0.5 0.75 0 1");

    c.setValue("color4", color4);
    CORRADE_COMPARE(c.value("color4"), value4);
    CORRADE_COMPARE(c.value<Color4>("color4"), color4);
}

void ConfigurationValueTest::rectangularMatrix() {
    typedef Math::Vector4<Float> Vector4;
    typedef Math::Matrix3x4<Float> Matrix3x4;

    Matrix3x4 m(Vector4(3.0f,  5.0f, 8.0f,   4.0f),
                Vector4(4.0f,  4.0f, 7.0f, 3.125f),
                Vector4(7.0f, -1.0f, 8.0f,  9.55f));
    std::string value("3 4 7 5 4 -1 8 7 8 4 3.125 9.55");

    Corrade::Utility::Configuration c;
    c.setValue<Matrix3x4>("matrix", m);

    CORRADE_COMPARE(c.value("matrix"), value);
    CORRADE_COMPARE(c.value<Matrix3x4>("matrix"), m);

    /* Underflow */
    c.setValue("underflow", "2.1 8.9 1.3 1 5 7 1.5");
    CORRADE_COMPARE(c.value<Matrix3x4>("underflow"), (Matrix3x4{
        Vector4{2.1f, 1.0f, 1.5f, 0.0f},
        Vector4{8.9f, 5.0f, 0.0f, 0.0f},
        Vector4{1.3f, 7.0f, 0.0f, 0.0f}}));

    /* Overflow */
    c.setValue("overflow", "2 1 8 9 1 3 1 5 7 1 6 3 3 1.5 23 17");
    CORRADE_COMPARE(c.value<Matrix3x4>("overflow"), (Matrix3x4{
        Vector4{2.0f, 9.0f, 1.0f, 1.0f},
        Vector4{1.0f, 1.0f, 5.0f, 6.0f},
        Vector4{8.0f, 3.0f, 7.0f, 3.0f}}));
}

void ConfigurationValueTest::matrix() {
    typedef Math::Vector4<Float> Vector4;
    typedef Math::Matrix4x4<Float> Matrix4x4;

    Corrade::Utility::Configuration c;

    Matrix4x4 m(Vector4(3.0f,  5.0f, 8.0f,   4.0f),
                Vector4(4.0f,  4.0f, 7.0f, 3.125f),
                Vector4(7.0f, -1.0f, 8.0f,   0.0f),
                Vector4(9.0f,  4.0f, 5.0f,  9.55f));
    std::string value("3 4 7 9 5 4 -1 4 8 7 8 5 4 3.125 0 9.55");

    c.setValue("matrix", m);
    CORRADE_COMPARE(c.value("matrix"), value);
    CORRADE_COMPARE(c.value<Matrix4x4>("matrix"), m);
}

void ConfigurationValueTest::matrix3() {
    typedef Math::Matrix3<Float> Matrix3;

    Corrade::Utility::Configuration c;

    Matrix3 m({5.0f, 8.0f,   4.0f},
              {4.0f, 7.0f, 3.125f},
              {4.0f, 5.0f,  9.55f});
    std::string value("5 4 4 8 7 5 4 3.125 9.55");

    c.setValue("matrix", m);
    CORRADE_COMPARE(c.value("matrix"), value);
    CORRADE_COMPARE(c.value<Matrix3>("matrix"), m);
}

void ConfigurationValueTest::matrix4() {
    typedef Math::Matrix4<Float> Matrix4;

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

void ConfigurationValueTest::complex() {
    typedef Math::Complex<Float> Complex;

    Corrade::Utility::Configuration c;

    Complex x{3.0f, 3.125f};
    std::string value{"3 3.125"};

    c.setValue("complex", x);
    CORRADE_COMPARE(c.value("complex"), value);
    CORRADE_COMPARE(c.value<Complex>("complex"), x);

    /* Underflow */
    c.setValue("underflow", "2.1");
    CORRADE_COMPARE(c.value<Complex>("underflow"), (Complex{2.1f, 0.0f}));

    /* Overflow */
    c.setValue("overflow", "2 9 16 33");
    CORRADE_COMPARE(c.value<Complex>("overflow"), (Complex{2.0f, 9.0f}));
}

void ConfigurationValueTest::dualComplex() {
    typedef Math::DualComplex<Float> DualComplex;

    Corrade::Utility::Configuration c;

    DualComplex a{{3.0f, 3.125f}, {9.0f, 9.55f}};
    std::string value("3 3.125 9 9.55");

    c.setValue("dualcomplex", a);
    CORRADE_COMPARE(c.value("dualcomplex"), value);
    CORRADE_COMPARE(c.value<DualComplex>("dualcomplex"), a);

    /* Underflow */
    c.setValue("underflow", "2.1 8.9");
    CORRADE_COMPARE(c.value<DualComplex>("underflow"), (DualComplex{{2.1f, 8.9f}, {0.0f, 0.0f}}));

    /* Overflow */
    c.setValue("overflow", "2 1 8 9 16 33");
    CORRADE_COMPARE(c.value<DualComplex>("overflow"), (DualComplex{{2.0f, 1.0f}, {8.0f, 9.0f}}));
}

void ConfigurationValueTest::quaternion() {
    typedef Math::Quaternion<Float> Quaternion;

    Corrade::Utility::Configuration c;

    Quaternion q{{3.0f, 3.125f, 9.0f}, 9.55f};
    std::string value{"3 3.125 9 9.55"};

    c.setValue("quat", q);
    CORRADE_COMPARE(c.value("quat"), value);
    CORRADE_COMPARE(c.value<Quaternion>("quat"), q);

    /* Underflow */
    c.setValue("underflow", "2.1 8.9");
    CORRADE_COMPARE(c.value<Quaternion>("underflow"), (Quaternion{{2.1f, 8.9f, 0.0f}, 0.0f}));

    /* Overflow */
    c.setValue("overflow", "2 1 8 9 16 33");
    CORRADE_COMPARE(c.value<Quaternion>("overflow"), (Quaternion{{2.0f, 1.0f, 8.0f}, 9.0f}));
}

void ConfigurationValueTest::dualQuaternion() {
    typedef Math::DualQuaternion<Float> DualQuaternion;

    Corrade::Utility::Configuration c;

    DualQuaternion a{{{3.0f, 3.125f, 9.0f}, 9.55f}, {{-1.2f, 0.3f, 1.1f}, 92.05f}};
    std::string value{"3 3.125 9 9.55 -1.2 0.3 1.1 92.05"};

    c.setValue("dualquat", a);
    CORRADE_COMPARE(c.value("dualquat"), value);
    CORRADE_COMPARE(c.value<DualQuaternion>("dualquat"), a);

    /* Underflow */
    c.setValue("underflow", "2.1 8.9");
    CORRADE_COMPARE(c.value<DualQuaternion>("underflow"), (DualQuaternion{{{2.1f, 8.9f, 0.0f}, 0.0f}, {{0.0f, 0.0f, 0.0f}, 0.0f}}));

    /* Overflow */
    c.setValue("overflow", "2 1 8 9 16 33 -1 5 2 10");
    CORRADE_COMPARE(c.value<DualQuaternion>("overflow"), (DualQuaternion{{{2.0f, 1.0f, 8.0f}, 9.0f}, {{16.0f, 33.0f, -1.0f}, 5.0f}}));
}

void ConfigurationValueTest::range() {
    typedef Math::Range2D<Float> Range2D;

    Corrade::Utility::Configuration c;

    Range2D rect({3.0f, 3.125f}, {9.0f, 9.55f});
    std::string value("3 3.125 9 9.55");

    c.setValue("rectangle", rect);
    CORRADE_COMPARE(c.value("rectangle"), value);
    CORRADE_COMPARE(c.value<Range2D>("rectangle"), rect);
}

void ConfigurationValueTest::bezier() {
    typedef Math::Vector2<Float> Vector2;
    typedef Math::CubicBezier2D<Float> CubicBezier2D;

    Corrade::Utility::Configuration c;

    CubicBezier2D bezier{Vector2{0.0f, 1.0f}, Vector2{1.5f, -0.3f}, Vector2{2.1f, 0.5f}, Vector2{0.0f, 2.0f}};
    std::string value("0 1 1.5 -0.3 2.1 0.5 0 2");

    c.setValue("bezier", bezier);
    CORRADE_COMPARE(c.value("bezier"), value);
    CORRADE_COMPARE(c.value<CubicBezier2D>("bezier"), bezier);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::ConfigurationValueTest)
