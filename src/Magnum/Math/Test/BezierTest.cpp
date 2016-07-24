/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016
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

#include <Corrade/TestSuite/Tester.h>
#include "Magnum/Math/Bezier.h"

namespace Magnum { namespace Math { namespace Test {

            struct BezierTest: Corrade::TestSuite::Tester {
                explicit BezierTest();

                void testQuadratic();

                void testCubic();

            };

            BezierTest::BezierTest() {
                addTests({&BezierTest::testQuadratic});
                addTests({&BezierTest::testCubic});
            }

            template <class T>
            inline T sqr(T t){ return t*t; }

            template <class T>
            inline T cube(T t){ return t*t*t; }


            void BezierTest::testQuadratic() {
                typedef Math::Vector<2, Float> Vector2;
                Vector2 p0(0.0f, 0.0f), p1(10.0f, 15.0f), p2(20.0f, 4.0f);
                std::array<Vector2, 3> points = {p0, p1, p2};
                QuadraticBezier2D<Float> bezier(points);
                for(Float t = 0.0; t <= 1.0f; t += 0.01f) {
                    Vector2 expected = sqr(1-t)*p0 + 2*(1-t)*t*p1 + sqr(t) *p2;
                    CORRADE_COMPARE(bezier.lerp(t), expected);
                }
            }

            void BezierTest::testCubic() {
                typedef Math::Vector<2, Float> Vector2;
                Vector2 p0(0.0f, 0.0f), p1(10.0f, 15.0f), p2(20.0f, 4.0f), p3(5.0f, -20.0f);
                std::array<Vector2, 4> points = {p0, p1, p2, p3};
                CubicBezier2D<Float> bezier(points);
                for(Float t = 0.0; t <= 1.0f; t += 0.01f) {
                    Vector2 expected = cube(1-t)*p0
                                       + 3*sqr(1-t)*t*p1
                                       + 3*(1-t)*sqr(t) *p2
                                       + cube(t)*p3;
                    CORRADE_COMPARE(bezier.lerp(t), expected);
                }
            }


        }}}

CORRADE_TEST_MAIN(Magnum::Math::Test::BezierTest)
