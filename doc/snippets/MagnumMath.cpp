/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
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

#include "Magnum/Magnum.h"
#include "Magnum/Math/DualComplex.h"
#include "Magnum/Math/DualQuaternion.h"
#include "Magnum/Math/Algorithms/GramSchmidt.h"

using namespace Magnum;
using namespace Magnum::Math::Literals;

int main() {
{
/* [transformations-rotation2D] */
auto a = Matrix3::rotation(23.0_degf);
auto b = Complex::rotation(Rad(Constants::piHalf()));
auto c = DualComplex::rotation(-1.57_radf);
/* [transformations-rotation2D] */
static_cast<void>(a);
static_cast<void>(b);
static_cast<void>(c);
}

{
Rad angle;
/* [transformations-rotation3D] */
auto a = Quaternion::rotation(60.0_degf, Vector3::xAxis());
auto b = DualQuaternion::rotation(-1.0_degf, Vector3(1.0f, 0.5f, 3.0f).normalized());
auto c = Matrix4::rotationZ(angle);
/* [transformations-rotation3D] */
static_cast<void>(a);
static_cast<void>(b);
static_cast<void>(c);
}

{
/* [transformations-translation2D] */
auto a = Matrix3::translation(Vector2::xAxis(-5.0f));
auto b = DualComplex::translation({-1.0f, 0.5f});
/* [transformations-translation2D] */
static_cast<void>(a);
static_cast<void>(b);
}

{
Vector3 vector;
/* [transformations-translation3D] */
auto a = Matrix4::translation(vector);
auto b = DualQuaternion::translation(Vector3::zAxis(1.3f));
/* [transformations-translation3D] */
static_cast<void>(a);
static_cast<void>(b);
}

{
/* [transformations-scaling] */
auto a = Matrix3::scaling(Vector2::xScale(2.0f));
auto b = Matrix4::scaling({2.0f, -2.0f, 1.5f});
auto c = Matrix4::scaling(Vector3(10.0f));
/* [transformations-scaling] */
static_cast<void>(a);
static_cast<void>(b);
static_cast<void>(c);
}

{
Vector3 axis;
/* [transformations-reflection] */
auto a = Matrix3::reflection(Vector2::yAxis());
auto b = Matrix4::reflection(axis.normalized());
/* [transformations-reflection] */
static_cast<void>(a);
static_cast<void>(b);
}

{
/* [transformations-projection] */
auto a = Matrix3::projection({4.0f, 3.0f});
auto b = Matrix4::orthographicProjection({4.0f, 3.0f}, 0.001f, 100.0f);
auto c = Matrix4::perspectiveProjection(35.0_degf, 1.333f, 0.001f, 100.0f);
/* [transformations-projection] */
static_cast<void>(a);
static_cast<void>(b);
static_cast<void>(c);
}

{
/* [transformations-composing] */
auto a = DualComplex::translation(Vector2::yAxis(2.0f))*
         DualComplex::rotation(25.0_degf);
auto b = Matrix4::translation(Vector3::yAxis(5.0f))*
         Matrix4::rotationY(25.0_degf);
/* [transformations-composing] */
static_cast<void>(a);
static_cast<void>(b);
}

{
/* [transformations-transform2D] */
auto transformation = Matrix3::rotation(-30.0_degf)*Matrix3::scaling(Vector2(3.0f));
Vector2 transformed = transformation.transformVector({1.5f, -7.9f});
/* [transformations-transform2D] */
static_cast<void>(transformed);
}

{
/* [transformations-transform3D] */
auto transformation = DualQuaternion::rotation(-30.0_degf, Vector3::xAxis())*
                      DualQuaternion::translation(Vector3::yAxis(3.0f));
Vector3 transformed = transformation.transformPointNormalized({1.5f, 3.0f, -7.9f});
/* [transformations-transform3D] */
static_cast<void>(transformed);
}

{
/* [transformations-properties] */
Matrix4 transformation;
Matrix3x3 rotationScaling = transformation.rotationScaling();
Vector3 up = transformation.up();
Vector3 right = transformation.right();

Matrix3 b;
Matrix2x2 rotation = b.rotation();
Float xTranslation = b.translation().x();
/* [transformations-properties] */

/* [transformations-recreate] */
Matrix3 c = Matrix3::from(rotation, {1.0f, 3.0f});
/* [transformations-recreate] */
static_cast<void>(rotationScaling);
static_cast<void>(up);
static_cast<void>(right);
static_cast<void>(xTranslation);
static_cast<void>(c);
}

{
/* [transformations-properties-complex-quat] */
DualComplex a;
Rad rotationAngle = a.rotation().angle();
Vector2 translation = a.translation();

Quaternion b;
Vector3 rotationAxis = b.axis();
/* [transformations-properties-complex-quat] */
static_cast<void>(rotationAngle);
static_cast<void>(translation);
static_cast<void>(rotationAxis);
}

{
/* [transformations-properties-complex-quat-to-matrix] */
Quaternion a;
auto rotation = Matrix4::from(a.toMatrix(), {});

DualComplex b;
Matrix3 transformation = b.toMatrix();
/* [transformations-properties-complex-quat-to-matrix] */
static_cast<void>(rotation);
static_cast<void>(transformation);
}

{
/* [transformations-properties-complex-quat-from-matrix] */
Matrix3 rotation;
auto a = Complex::fromMatrix(rotation.rotationScaling());

Matrix4 transformation;
auto b = DualQuaternion::fromMatrix(transformation);
/* [transformations-properties-complex-quat-from-matrix] */
static_cast<void>(a);
static_cast<void>(b);
}

{
/* [transformations-normalization-matrix] */
Matrix4 transformation;
Math::Algorithms::gramSchmidtOrthonormalizeInPlace(transformation);
/* [transformations-normalization-matrix] */
}

{
/* [transformations-normalization-quat] */
DualQuaternion transformation;
transformation = transformation.normalized();
/* [transformations-normalization-quat] */
}

}
