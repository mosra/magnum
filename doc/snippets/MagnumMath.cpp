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

#include <map>
#include <set>

#include "Magnum/Magnum.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/FunctionsBatch.h"
#include "Magnum/Math/Bezier.h"
#include "Magnum/Math/CubicHermite.h"
#include "Magnum/Math/Distance.h"
#include "Magnum/Math/DualComplex.h"
#include "Magnum/Math/DualQuaternion.h"
#include "Magnum/Math/Frustum.h"
#include "Magnum/Math/Half.h"
#include "Magnum/Math/Range.h"
#include "Magnum/Math/Algorithms/GramSchmidt.h"
#include "Magnum/Math/StrictWeakOrdering.h"
#include "Magnum/Math/Swizzle.h"

using namespace Magnum;
using namespace Magnum::Math::Literals;

int main() {
{
/* [matrix-vector-construct] */
Matrix2x3 a;                    // zero-filled
Vector3i b;                     // zero-filled

Matrix3 identity;               // diagonal set to 1
Matrix3 zero{Math::ZeroInit};   // zero-filled
/* [matrix-vector-construct] */
static_cast<void>(a);
static_cast<void>(b);
static_cast<void>(identity);
static_cast<void>(zero);
}

{
/* [matrix-vector-construct-value] */
Vector3i vec{0, 1, 2};

Matrix3 mat{{0.0f, 1.9f, 2.2f},
            {3.5f, 4.0f, 5.1f},
            {6.0f, 7.3f, 8.0f}};
/* [matrix-vector-construct-value] */
static_cast<void>(vec);
static_cast<void>(mat);
}

{
/* [matrix-vector-construct-diagonal] */
Matrix3 diag{Math::IdentityInit, 2.0f}; // diagonal is 2.0f, zeros elsewhere
Vector3i fill(10);                      // {10, 10, 10}
auto diag2 = Matrix3::fromDiagonal({3.0f, 2.0f, 1.0f});
/* [matrix-vector-construct-diagonal] */
static_cast<void>(diag);
static_cast<void>(fill);
static_cast<void>(diag2);
}

{
/* [matrix-vector-construct-axis] */
auto x = Vector3::xAxis();              // {1.0f, 0.0f, 0.0f}
auto y = Vector2::yAxis(3.0f);          // {0.0f, 3.0f}
auto z = Vector3::zScale(3.0f);         // {1.0f, 1.0f, 3.0f}
/* [matrix-vector-construct-axis] */
static_cast<void>(x);
static_cast<void>(y);
static_cast<void>(z);
}

{
/* [matrix-vector-construct-from] */
Int mat[]{ 2, 4, 6,
           1, 3, 5 };
Math::Matrix2x3<Int>::from(mat) *= 2;   // { 4, 8, 12, 2, 6, 10 }
/* [matrix-vector-construct-from] */
}

{
/* [matrix-vector-construct-color] */
Color4 a = Color3{0.2f, 0.7f, 0.5f};     // {0.2f, 0.7f, 0.5f, 1.0f}
Color4ub b = Color3ub{0x33, 0xb2, 0x7f}; // {0x33, 0xb2, 0x7f, 0xff}
/* [matrix-vector-construct-color] */
static_cast<void>(a);
static_cast<void>(b);
}

{
/* [matrix-vector-construct-color-hue] */
auto green = Color3::green();           // {0.0f, 1.0f, 0.0f}
auto cyan = Color4::cyan(0.5f, 0.95f);  // {0.5f, 1.0f, 1.0f, 0.95f}
auto fadedRed = Color3::fromHsv({219.0_degf, 0.50f, 0.57f});
/* [matrix-vector-construct-color-hue] */
static_cast<void>(green);
static_cast<void>(cyan);
static_cast<void>(fadedRed);
}

{
/* [matrix-vector-construct-color-literal] */
Color3ub a = 0x33b27f_rgb;      // {0x33, 0xb2, 0x7f}
Color4 b = 0x33b27fcc_rgbaf;    // {0.2f, 0.7f, 0.5f, 0.8f}
Color4 c = 0x33b27fcc_srgbaf;   // {0.0331048f, 0.445201f, 0.212231f, 0.8f}
/* [matrix-vector-construct-color-literal] */
static_cast<void>(a);
static_cast<void>(b);
static_cast<void>(c);
}

{
/* [matrix-vector-access] */
Matrix3x2 a;
a[2] /= 2.0f;   // third column (column major indexing, see explanation below)
a[0][1] = 5.3f; // first column, second element

Vector3i b;
b[1] = 1;       // second element
/* [matrix-vector-access] */

/* [matrix-vector-access-row] */
Vector3 c = a.row(1); // second row
/* [matrix-vector-access-row] */
static_cast<void>(c);
}

{
/* [matrix-vector-access-named] */
Vector4i a;
Int x = a.x();
a.y() += 5;

Vector3i xyz = a.xyz();
xyz.xy() *= 5;
/* [matrix-vector-access-named] */
static_cast<void>(x);
}

{
/* [matrix-vector-access-swizzle] */
Vector4i orig{-1, 2, 3, 4};
Vector4i bgra = Math::gather<'b', 'g', 'r', 'a'>(orig); // { 3, 2, -1, 4 }
Math::Vector<6, Int> w10xyz = Math::gather<'w', '1', '0', 'x', 'y', 'z'>(orig);
    // { 4, 1, 0, -1, 2, 3 }

Vector4 vec{1.5f, 3.0f, 0.1f, 1.1f};
Vector2 coords{5.0f, -2.0f};
Math::scatter<'z', 'w'>(vec, coords); // { 1.5, 3.0, 5.0, -2.0 }

/* [matrix-vector-access-swizzle] */
static_cast<void>(bgra);
static_cast<void>(w10xyz);
}

{
/* [matrix-vector-convert] */
Vector3 a{2.2f, 0.25f, -5.1f};
//Vector3i b = a;                   // error, implicit conversion not allowed
auto c = Vector3i{a};               // {2, 0, -5}
auto d = Vector3d{a};               // {2.2, 0.25, -5.1}
/* [matrix-vector-convert] */
static_cast<void>(c);
static_cast<void>(d);
}

{
/* [matrix-vector-convert-pack] */
Color3 a{0.8f, 1.0f, 0.3f};
auto b = Math::pack<Color3ub>(a);   // {204, 255, 76}

Color3ub c{64, 127, 89};
auto d = Math::unpack<Color3>(c);   // {0.251f, 0.498f, 0.349}
/* [matrix-vector-convert-pack] */
static_cast<void>(b);
static_cast<void>(d);
}

{
/* [matrix-vector-operations-vector] */
Vector3 a{1.0f, 2.0f, 3.0f};
Vector3 b = a*5.0f - Vector3{3.0f, -0.5f, -7.5f}; // {5.0f, 9.5f, 7.5f}
Vector3 c = 1.0f/a;                             // {1.0f, 0.5f, 0.333f}
/* [matrix-vector-operations-vector] */
static_cast<void>(b);
static_cast<void>(c);
}

{
/* [matrix-vector-operations-multiply] */
Vector3 a{1.0f, 2.0f, 3.0f};
Vector3 b = a*Vector3{-0.5f, 2.0f, -7.0f};      // {-0.5f, 4.0f, -21.0f}
/* [matrix-vector-operations-multiply] */
static_cast<void>(b);
}

{
/* [matrix-vector-operations-integer] */
Color3ub color{80, 116, 34};
Color3ub lighter = color*1.5f;                  // {120, 174, 51}

Vector3i a{4, 18, -90};
Vector3 multiplier{2.2f, 0.25f, 0.1f};
Vector3i b = a*multiplier;                      // {8, 4, -9}
Vector3 c = Vector3(a)*multiplier;              // {8.0f, 4.5f, -9.0f}
/* [matrix-vector-operations-integer] */
static_cast<void>(lighter);
static_cast<void>(b);
static_cast<void>(c);
}

{
/* [matrix-vector-operations-bitwise] */
Vector2i size{256, 256};
Vector2i mipLevel3Size = size >> 3;             // {32, 32}
/* [matrix-vector-operations-bitwise] */
static_cast<void>(mipLevel3Size);
}

{
/* [matrix-vector-operations-matrix] */
Matrix3x2 a;
Matrix3x2 b;
Matrix3x2 c = a + (-b);

Matrix2x3 d;
Matrix2x2 e = b*d;
Matrix3x3 f = d*b;
/* [matrix-vector-operations-matrix] */
static_cast<void>(c);
static_cast<void>(e);
static_cast<void>(f);
}

{
/* [matrix-vector-operations-multiply-matrix] */
Matrix3x4 a;
Vector3 b;
Vector4 c = a*b;

Math::RectangularMatrix<4, 1, Float> d;
Matrix4x3 e = b*d;
/* [matrix-vector-operations-multiply-matrix] */
static_cast<void>(c);
static_cast<void>(e);
}

{
/* [matrix-vector-operations-componentwise] */
Float a = Vector3{1.5f, 0.3f, 8.0f}.sum();      // 8.8f
Int b = Vector3i{32, -5, 7}.product();          // 1120
/* [matrix-vector-operations-componentwise] */
static_cast<void>(a);
static_cast<void>(b);
}

{
/* [matrix-vector-operations-minmax] */
Vector3i a{-5, 7, 24};
Vector3i b{8, -2, 12};

Vector3i min = Math::min(a, b);                 // {-5, -2, 12}
Int max = a.max();                              // 24
/* [matrix-vector-operations-minmax] */
static_cast<void>(min);
static_cast<void>(max);

/* [matrix-vector-operations-compare] */
BoolVector3 largerOrEqual = a >= b;             // {false, true, true}
bool anySmaller = (a < b).any();                // true
bool allLarger = (a > b).all();                 // false
/* [matrix-vector-operations-compare] */
static_cast<void>(largerOrEqual);
static_cast<void>(anySmaller);
static_cast<void>(allLarger);
}

{
/* [matrix-vector-operations-functions] */
Vector3 a{5.5f, -0.3f, 75.0f};
Vector3 b = Math::round(a);                     // {5.0f,  0.0f, 75.0f}
Vector3 c = Math::abs(a);                       // {5.5f, -0.3f, 75.0f}
Vector3 d = Math::clamp(a, -0.2f, 55.0f);       // {5.5f, -0.2f, 55.0f}
/* [matrix-vector-operations-functions] */
static_cast<void>(b);
static_cast<void>(c);
static_cast<void>(d);
}

{
/* [matrix-vector-operations-functions-componentwise] */
Matrix3x2 mat;
Math::Vector<6, Float> vec = mat.toVector();
// ...
mat = Matrix3x2::fromVector(vec);
/* [matrix-vector-operations-functions-componentwise] */
}

{
Deg value;
/* [matrix-vector-operations-functions-scalar] */
std::pair<Int, Int> minmax = Math::minmax(24, -5);  // -5, 24
Int a = Math::lerp(0, 360, 0.75f);                  // 270
auto b = Math::pack<UnsignedByte>(0.89f);           // 226

Deg c = Math::clamp(value, 25.0_degf, 55.0_degf);
/* [matrix-vector-operations-functions-scalar] */
static_cast<void>(minmax);
static_cast<void>(a);
static_cast<void>(b);
static_cast<void>(c);
}

{
/* [matrix-vector-column-major-template] */
Math::RectangularMatrix<2, 5, Int> mat; // two columns, five rows
/* [matrix-vector-column-major-template] */
static_cast<void>(mat);
}

{
/* [matrix-vector-column-major-construct] */
Math::Matrix3<Int> mat{{0, 1, 2},
                       {3, 4, 5},
                       {6, 7, 8}};      // first column is {0, 1, 2}
/* [matrix-vector-column-major-construct] */

/* [matrix-vector-column-major-access] */
mat[0] *= 2;                            // first column
mat[2][0] = 5;                          // first element of third column
/* [matrix-vector-column-major-access] */
}

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

{
/* [types-literals-colors] */
using namespace Math::Literals;

Color3 a = 0x33b27f_srgbf;      // {0.0331048f, 0.445201f, 0.212231f}
Color4ub b = 0x33b27fcc_rgba;   // {0x33, 0xb2, 0x7f, 0xcc}
/* [types-literals-colors] */
static_cast<void>(a);
static_cast<void>(b);
}

{
/* [types-literals-angles] */
using namespace Math::Literals;

//Deg a = 60.0f         // error, no implicit conversion from Float
Deg a = 60.0_degf;      // okay

Float b = 3.2831853f;
auto tau = Rad{b} + 3.0_radf;
Radd pi = 3.141592653589793_rad;

//Double c = pi;        // error, no implicit conversion to Double
auto c = Double(pi);    // okay
/* [types-literals-angles] */
static_cast<void>(a);
static_cast<void>(tau);
static_cast<void>(c);

/* [types-literals-angle-conversion] */
Rad d = 60.0_degf;      // 1.0471976f
auto e = Degd{pi};      // 180.0

//Rad f = pi;           // error, no implicit conversion of underlying types
auto f = Rad{pi};       // 3.141592654f
/* [types-literals-angle-conversion] */
static_cast<void>(d);
static_cast<void>(e);
static_cast<void>(f);
}

{
/* [types-literals-usage] */
Float a = Math::sin(1.32457_radf);
Complex b = Complex::rotation(60.0_degf);
/* [types-literals-usage] */
static_cast<void>(a);
static_cast<void>(b);
}

{
/* [types-literals-half] */
using namespace Math::Literals;

Half a = 3.5_h;         // 0x4300 internally
/* [types-literals-half] */
static_cast<void>(a);
}

{
bool orthographic = false;
/* [types-literals-init] */
/* These are equivalent */
Vector3 a1;
Vector3 a2{Math::ZeroInit};

/* These too */
Quaternion q1;
Quaternion q2{Math::IdentityInit};

/* Avoid unnecessary initialization if is overwritten anyway */
Matrix4 projection{NoInit};
if(orthographic)
    projection = Matrix4::orthographicProjection({4.0f, 3.0f}, 0.1f, 100.0f);
else
    projection = Matrix4::perspectiveProjection(35.0_degf, 1.33f, 0.1f, 100.0f);
/* [types-literals-init] */
static_cast<void>(a1);
static_cast<void>(a2);
static_cast<void>(q1);
static_cast<void>(q2);
}

{
/* [Deg-usage] */
using namespace Math::Literals;

auto degrees = 60.0_degf;       // type is Deg<Float>
auto radians = 1.047_rad;       // type is Rad<Double>
/* [Deg-usage] */
static_cast<void>(degrees);
static_cast<void>(radians);
}

{
/* [Deg-usage-convert] */
Double foo();

Deg degrees{35.0f};
Radd radians{foo()};
//degrees = 60.0f;              // error, no implicit conversion
/* [Deg-usage-convert] */
static_cast<void>(degrees);
static_cast<void>(radians);
}

{
/* [Deg-usage-operations] */
auto a = 60.0_degf + 17.35_degf;
auto b = -a + 23.0_degf*4;
//auto c = 60.0_degf*45.0_degf; // error, undefined resulting unit
/* [Deg-usage-operations] */
static_cast<void>(b);
}

{
Double foo();
/* [Deg-usage-comparison] */
Rad angle();

Deg x = angle();                // convert to degrees for easier comparison
if(x < 30.0_degf) foo();
//if(x > 1.57_radf) bar();      // error, both need to be of the same type
/* [Deg-usage-comparison] */
}

{
/* [Deg-usage-conversion] */
Float sine(Rad angle);
Float a = sine(60.0_degf);      // the same as sine(1.047_radf)
Degd b = 1.047_rad;             // the same as 60.0_deg
Double c = Double(b);           // 60.0
//Float d = a;                  // error, no implicit conversion
/* [Deg-usage-conversion] */
static_cast<void>(a);
static_cast<void>(c);
}

{
Float sine(Rad angle);
Float a = 60.0f;
Deg b;
/* [Deg-usage-explicit-conversion] */
//sine(a);                      // compilation error
sine(Deg{a});                   // explicitly specifying unit

//std::sin(b);                  // compilation error
std::sin(Float(Rad{b}));        // required explicit conversion hints to user
                                // that this case needs special attention
                                // (i.e., conversion to radians)
/* [Deg-usage-explicit-conversion] */
}

{
/* [_deg] */
using namespace Math::Literals;
Double cos1 = Math::cos(60.0_deg);  // cos1 = 0.5
Double cos2 = Math::cos(1.047_rad); // cos2 = 0.5
/* [_deg] */
static_cast<void>(cos1);
static_cast<void>(cos2);
}

{
/* [_degf] */
using namespace Math::Literals;
Float tan1 = Math::tan(60.0_degf);  // tan1 = 1.732f
Float tan2 = Math::tan(1.047_radf); // tan2 = 1.732f
/* [_degf] */
static_cast<void>(tan1);
static_cast<void>(tan2);
}

{
Vector3 epsilon;
/* [BoolVector-boolean] */
Vector3 a, b;

if(!(b < a - epsilon || a + epsilon < b)) {
    // b is around a
}
/* [BoolVector-boolean] */
}

{
/* [Color3-fromSrgb] */
Vector3ub srgb;
auto rgb = Color3::fromSrgb(srgb);
/* [Color3-fromSrgb] */
static_cast<void>(rgb);
}

{
/* [Color3-fromSrgb-int] */
Color3 a = Color3::fromSrgb(0xff3366);
Color3 b = 0xff3366_srgbf;
/* [Color3-fromSrgb-int] */
static_cast<void>(a);
static_cast<void>(b);
}

{
/* [Color3-unpack] */
Color3ub a{0xff, 0x33, 0x66};
auto bFromSrgb = Color3::fromSrgb(a);       // {1.0f, 0.03311f, 0.1329f}
auto bFromLinear = Math::unpack<Color3>(a); // {1.0f, 0.2f, 0.4f}
/* [Color3-unpack] */
static_cast<void>(bFromLinear);
static_cast<void>(bFromSrgb);
}

{
/* [Color3-toSrgb] */
Color3 color;
Vector3ub srgb = color.toSrgb<UnsignedByte>();
/* [Color3-toSrgb] */
static_cast<void>(srgb);
}

{
/* [Color3-pack] */
Color3 a{1.0f, 0.2f, 0.4f};
auto bSrgb = a.toSrgb<UnsignedByte>();  // {0xff, 0x7c, 0xaa}
auto bLinear = Math::pack<Color3ub>(a); // {0xff, 0x33, 0x66}
/* [Color3-pack] */
static_cast<void>(bLinear);
static_cast<void>(bSrgb);
}

{
/* [Color4-fromSrgbAlpha] */
Vector4ub srgbAlpha;
auto rgba = Color4::fromSrgbAlpha(srgbAlpha);
/* [Color4-fromSrgbAlpha] */
static_cast<void>(rgba);
}

{
/* [Color4-fromSrgbAlpha-int] */
Color4 a = Color4::fromSrgbAlpha(0xff336680);
Color4 b = 0xff336680_srgbaf;
/* [Color4-fromSrgbAlpha-int] */
static_cast<void>(a);
static_cast<void>(b);
}

{
/* [Color4-unpack] */
Color4ub a{0xff, 0x33, 0x66, 0x99};
auto bFromSrgb = Color4::fromSrgbAlpha(a);  // {1.0f, 0.03311f, 0.1329f, 0.6f}
auto bFromLinear = Math::unpack<Color4>(a); // {1.0f, 0.2f, 0.4f, 0.6f}
/* [Color4-unpack] */
static_cast<void>(bFromLinear);
static_cast<void>(bFromSrgb);
}

{
/* [Color4-toSrgbAlpha] */
Color4 color;
Vector4ub srgbAlpha = color.toSrgbAlpha<UnsignedByte>();
/* [Color4-toSrgbAlpha] */
static_cast<void>(srgbAlpha);
}

{
/* [Color4-pack] */
Color4 a{1.0f, 0.2f, 0.4f, 0.6f};
auto bSrgb = a.toSrgbAlpha<UnsignedByte>(); // {0xff, 0x7c, 0xaa, 0x99}
auto bLinear = Math::pack<Color4ub>(a);     // {0xff, 0x33, 0x66, 0x99}
/* [Color4-pack] */
static_cast<void>(bLinear);
static_cast<void>(bSrgb);
}

{
/* [Color3-debug] */
Debug{Debug::Flag::Color} << 0xdcdcdc_rgb << 0xa5c9ea_rgb << 0x3bd267_rgb
    << 0xc7cf2f_rgb << 0xcd3431_rgb << 0x2f83cc_rgb << 0x747474_rgb;
/* [Color3-debug] */
}

{
/* [Color4-debug] */
Debug{Debug::Flag::Color}
    << 0x3bd26700_rgba << 0x3bd26733_rgba << 0x3bd26766_rgba
    << 0x3bd26799_rgba << 0x3bd267cc_rgba << 0x3bd267ff_rgba;
/* [Color4-debug] */
}

{
/* [_rgb] */
using namespace Math::Literals;
Color3ub a = 0x33b27f_rgb;      // {0x33, 0xb2, 0x7f}
/* [_rgb] */
static_cast<void>(a);
}

{
/* [_srgb] */
using namespace Math::Literals;
Vector3ub a = 0x33b27f_srgb;    // {0x33, 0xb2, 0x7f}
/* [_srgb] */
static_cast<void>(a);
}

{
/* [_rgba] */
using namespace Math::Literals;
Color4ub a = 0x33b27fcc_rgba;   // {0x33, 0xb2, 0x7f, 0xcc}
/* [_rgba] */
static_cast<void>(a);
}

{
/* [_srgba] */
using namespace Math::Literals;
Vector4ub a = 0x33b27fcc_srgba; // {0x33, 0xb2, 0x7f, 0xcc}
/* [_srgba] */
static_cast<void>(a);
}

{
/* [_rgbf] */
using namespace Math::Literals;
Color3 a = 0x33b27f_rgbf;       // {0.2f, 0.698039f, 0.498039f}
/* [_rgbf] */
static_cast<void>(a);
}

{
/* [_srgbf] */
using namespace Math::Literals;
Color3 a = 0x33b27f_srgbf;      // {0.0331048f, 0.445201f, 0.212231f}
/* [_srgbf] */
static_cast<void>(a);
}

{
/* [_rgbaf] */
using namespace Math::Literals;
Color4 a = 0x33b27fcc_rgbaf;    // {0.2f, 0.698039f, 0.498039f, 0.8f}
/* [_rgbaf] */
static_cast<void>(a);
}

{
/* [_srgbaf] */
using namespace Math::Literals;
Color4 a = 0x33b27fcc_srgbaf;   // {0.0331048f, 0.445201f, 0.212231f, 0.8f}
/* [_srgbaf] */
static_cast<void>(a);
}

{
/* [CubicHermite-fromBezier] */
CubicBezier2D segment;
auto startPoint = CubicHermite2D::fromBezier(
    {Vector2{}, Vector2{}, Vector2{}, segment[3]}, segment);
auto endPoint = CubicHermite2D::fromBezier(segment,
    {segment[0], Vector2{}, Vector2{}, Vector2{}});
/* [CubicHermite-fromBezier] */
static_cast<void>(startPoint);
static_cast<void>(endPoint);
}

{
/* [Dual-conversion] */
Math::Dual<Float> floatingPoint{1.3f, 2.7f};
Math::Dual<Byte> integral{floatingPoint}; // {1, 2}
/* [Dual-conversion] */
}

[](const Vector3& point){
Frustum frustum;
/* [Frustum-range] */
for(Vector4 plane: frustum)
    if(Math::Distance::pointPlaneScaled(point, plane) < 0.0f) return false;
return true;
/* [Frustum-range] */
}({});

{
/* [div] */
Int quotient, remainder;
std::tie(quotient, remainder) = Math::div(57, 6); // {9, 3}
/* [div] */
}

{
/* [div-equivalent] */
Int quotient = 57/6;
Int remainder = 57%6;
/* [div-equivalent] */
static_cast<void>(quotient);
static_cast<void>(remainder);
}

{
Float value{}, min{}, max{};
/* [clamp] */
Math::min(Math::max(value, min), max)
/* [clamp] */
;
}

{
Float a{};
/* [lerpInverted-map] */
Deg b = Math::lerp(5.0_degf, 15.0_degf,
    Math::lerpInverted(-1.0f, 1.0f, a));
Deg bClamped = Math::lerp(5.0_degf, 15.0_degf, Math::clamp(
    Math::lerpInverted(-1.0f, 1.0f, a), 0.0f, 1.0f));
/* [lerpInverted-map] */
static_cast<void>(b);
static_cast<void>(bClamped);
}

{
/* [Half-usage] */
using namespace Math::Literals;

Half a = 3.14159_h;
Debug{} << a;                   // Prints 3.14159
Debug{} << Float(a);            // Prints 3.14159
Debug{} << UnsignedShort(a);    // Prints 25675
/* [Half-usage] */
}

{
/* [Half-usage-vector] */
Vector3h a{3.14159_h, -1.4142_h, 1.618_h};
Vector3 b{a};                   // converts to 32-bit floats
Debug{} << a;                   // prints {3.14159, -1.4142, 1.618}
Debug{} << Vector3us{a};        // prints {16968, 48552, 15993}
/* [Half-usage-vector] */
}

{
Rad angle{};
typedef Float T;
/* [Intersection-tanAngleSqPlusOne] */
T tanAngleSqPlusOne = Math::pow<2>(Math::tan(angle*T(0.5))) + T(1);
/* [Intersection-tanAngleSqPlusOne] */
static_cast<void>(tanAngleSqPlusOne);
}

{
Rad angle{};
typedef Float T;
/* [Intersection-sinAngle-tanAngle] */
T sinAngle = Math::sin(angle*T(0.5));
T tanAngle = Math::tan(angle*T(0.5));
/* [Intersection-sinAngle-tanAngle] */
static_cast<void>(sinAngle);
static_cast<void>(tanAngle);
}

{
Rad angle{};
typedef Float T;
/* [Intersection-sinAngle-tanAngleSqPlusOne] */
T sinAngle = Math::sin(angle*T(0.5));
T tanAngleSqPlusOne = Math::pow<2>(Math::tan(angle*T(0.5))) + T(1);
/* [Intersection-sinAngle-tanAngleSqPlusOne] */
static_cast<void>(sinAngle);
static_cast<void>(tanAngleSqPlusOne);
}

{
/* [Matrix-conversion] */
Matrix2x2 floatingPoint{Vector2{1.3f, 2.7f}, Vector2{-15.0f, 7.0f}};
Math::Matrix2x2<Byte> integral{floatingPoint}; // {{1, 2}, {-15, 7}}
/* [Matrix-conversion] */
}

{
/* [Quaternion-fromEuler] */
Rad x, y, z;
Quaternion a =
    Quaternion::rotation(z, Vector3::zAxis())*
    Quaternion::rotation(y, Vector3::yAxis())*
    Quaternion::rotation(x, Vector3::xAxis());
/* [Quaternion-fromEuler] */
static_cast<void>(a);
}

{
/* [unpack-template-explicit] */
// Literal type is (signed) char, but we assumed unsigned char, a != 1.0f
Float a = Math::unpack<Float>('\xFF');

// b = 1.0f
Float b = Math::unpack<Float, UnsignedByte>('\xFF');
/* [unpack-template-explicit] */
static_cast<void>(a);
static_cast<void>(b);
}

{
/* [unpack] */
Float a = Math::unpack<Float, UnsignedShort>(8191);     // 0.124987f
Float b = Math::unpack<Float, UnsignedShort, 14>(8191); // 0.499969f
Float c = Math::unpack<Float, 14>(8191u);               // 0.499969f
Float d = Math::unpack<Float, 14>(8191);                // 1.0f
/* [unpack] */
static_cast<void>(a);
static_cast<void>(b);
static_cast<void>(c);
static_cast<void>(d);
}

{
/* [pack] */
auto a = Math::pack<UnsignedShort>(0.5f);     // 32767
auto b = Math::pack<UnsignedShort, 14>(0.5f); // 8191
/* [pack] */
static_cast<void>(a);
static_cast<void>(b);
}

{
Range1D range, a, b;
constexpr UnsignedInt dimensions = 1;
typedef Float T;
/* [Range-generic] */
Math::Vector<dimensions, T> min = range.min(); // works for 1D, 2D and 3D

T c = Math::max<dimensions, T>(a.size(), b.size()).product(); // vector max()
/* [Range-generic] */
static_cast<void>(min);
static_cast<void>(c);
}

{
/* [Range-construct-minmax2D] */
Vector2 texcoords[50];
Range2D bounds = Math::minmax(texcoords);
/* [Range-construct-minmax2D] */
static_cast<void>(bounds);
}

{
/* [Range-construct-minmax3D] */
Vector3 a, b, c;
Range3D bounds = Math::minmax({a, b, c});
/* [Range-construct-minmax3D] */
static_cast<void>(bounds);
}

{
/* [Range-fromCenter-integer] */
Vector2i center, filterRadius;
auto filterArea = Range2Di::fromSize(center, Vector2i{1}).padded(filterRadius);
/* [Range-fromCenter-integer] */
static_cast<void>(filterArea);
}

{
/* [Range-conversion] */
Range2D floatingPoint{{1.3f, 2.7f}, {-15.0f, 7.0f}};
Range2Di integral{floatingPoint}; // {{1, 2}, {-15, 7}}
/* [Range-conversion] */
}

{
/* [RectangularMatrix-conversion] */
Math::RectangularMatrix<4, 1, Float> floatingPoint{1.3f, 2.7f, -15.0f, 7.0f};
Math::RectangularMatrix<4, 1, Byte> integral{floatingPoint}; // {1, 2, -15, 7}
/* [RectangularMatrix-conversion] */
}

{
/* [RectangularMatrix-access] */
Matrix4x3 m;
Float a = m[2][1];
/* [RectangularMatrix-access] */
static_cast<void>(a);
}

{
/* [StrictWeakOrdering] */
std::set<Vector2, Math::StrictWeakOrdering> mySet;
std::map<Vector4, Int, Math::StrictWeakOrdering> myMap;
/* [StrictWeakOrdering] */
static_cast<void>(myMap);
static_cast<void>(mySet);
}

{
/* [gather] */
Vector4i original(-1, 2, 3, 4);

auto vec = Math::gather<'w', '1', '0', 'x', 'y', 'z'>(original);
        // vec == { 4, 1, 0, -1, 2, 3 }
/* [gather] */
static_cast<void>(vec);
}

{
/* [scatter] */
Vector4 vec{1.5f, 3.0f, 0.1f, 1.1f};
Vector2 coords{5.0f, -2.0f};
vec = Math::scatter<'z', 'w'>(vec, coords); // { 1.5, 3.0, 5.0, -2.0 }

/* Combine the two for more advanced swizzles */
Vector4 vec2;
vec2 = Math::scatter<'w', 'x', 'y'>(vec2, Math::gather<'x', 'w', 'y'>(vec));
/* [scatter] */
}

{
Float a{}, b{};
/* [TypeTraits-equalsZero] */
Math::TypeTraits<Float>::equals(a, b);
Math::TypeTraits<Float>::equalsZero(a - b,
    Math::max(Math::abs(a), Math::abs(b)));
/* [TypeTraits-equalsZero] */
}

{
/* [Vector-conversion] */
Vector4 floatingPoint{1.3f, 2.7f, -15.0f, 7.0f};
Vector4i integral{floatingPoint}; // {1, 2, -15, 7}
/* [Vector-conversion] */
}

{
/* [Vector-length-integer] */
Vector2i a{25, -1};
Float length = Vector2{a}.length();         // ~25.099
/* [Vector-length-integer] */
static_cast<void>(length);

/* [Vector-length-manhattan] */
Int manhattanLength = Math::abs(a).sum();   // 26
/* [Vector-length-manhattan] */
static_cast<void>(manhattanLength);
}

{
Vector2 vec;
Float length{};
/* [Vector-resized] */
vec*(vec.lengthInverted()*length) // the parentheses are important
/* [Vector-resized] */
;
}

{
/* [Vector2-xAxis] */
Matrix3::translation(Vector2::xAxis(5.0f));
        // same as Matrix3::translation({5.0f, 0.0f});
/* [Vector2-xAxis] */
}

{
/* [Vector2-xScale] */
Matrix3::scaling(Vector2::xScale(-2.0f));
        // same as Matrix3::scaling({-2.0f, 1.0f});
/* [Vector2-xScale] */
}

{
/* [Vector3-xAxis] */
Matrix4::translation(Vector3::xAxis(5.0f));
        // same as Matrix4::translation({5.0f, 0.0f, 0.0f});
Matrix4::rotation(30.0_degf, Vector3::xAxis());
        // same as Matrix4::rotation(30.0_degf, {1.0f, 0.0f, 0.0f});
/* [Vector3-xAxis] */
}

{
/* [Vector3-xScale] */
Matrix4::scaling(Vector3::xScale(-2.0f));
        // same as Matrix4::scaling({-2.0f, 1.0f, 1.0f});
/* [Vector3-xScale] */
}

}
