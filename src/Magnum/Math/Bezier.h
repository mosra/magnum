#ifndef Magnum_Math_Bezier_h
#define Magnum_Math_Bezier_h

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

/** @file
 * @brief Class @ref Magnum::Math::Bezier
 */

#include <array>
#include "Vector.h"

namespace Magnum { namespace Math {
        /**
        @brief Bezier
        @tparam order       Order of Bezier curve
        @tparam dimensions  Dimensions of the control points
        @tparam T           Underlying data type

        See <a href="https://en.wikipedia.org/wiki/B%C3%A9zier_curve">Bezier Curve</a>.
        */
        template<UnsignedInt order, UnsignedInt dimensions, class T> class Bezier {

        public:

            /** @brief Construct Bezier curve with the given array of control points */
            explicit Bezier(const std::array<Vector<dimensions, T>, order+1> &points) {
                _points = points;
            }

            /**
             * @brief Divides a Bezier curve into two curves of same order having their own control points.
             *        De Casteljau's algorithm is used.
             * @param t The interpolation factor
             *
             * @return Array of two Bezier curves of the same order
             */
            std::array<Bezier<order, dimensions, T >, 2> subdivide(Float t) const {
                auto i_points = calculateIntermediatePoints(t);
                std::array<Vector<dimensions, T>, order + 1> left, right;
                for(UnsignedInt i=0; i<=order; ++i){
                    left[i] = i_points[0][i];
                }
                for(UnsignedInt i = 0, j = order; j>=0; --j, ++i){
                    right[i] = i_points[i][j];
                }
                return {Bezier<order, dimensions, T>(left), Bezier<order, dimensions, T>(right)};
            }

            /**
             * @brief Finds the point in the curve for a given interpolation factor
             *        De Casteljau's algorithm is used.
             * @param t The interpolation factor
             */
            Vector<dimensions, T> lerp (Float t) const {
                auto i_points= calculateIntermediatePoints(t);
                return i_points[0][order];
            };

        private:

            /**
             * @brief Calculates and returns all intermediate points generated when using De Casteljau's algorithm
             * @param t The interpolation factor
             *
             */
            std::array<std::array<Vector<dimensions,T>, order + 1>, order + 1> calculateIntermediatePoints(Float t) const {
                const auto n = order + 1;
                std::array<std::array<Vector<dimensions,T>, n>, n> i_points;
                for (UnsignedInt i = 0; i < n; ++i) {
                    i_points[i][0] = _points[i];
                }
                for (UnsignedInt r = 1; r < n; ++r) {
                    for (UnsignedInt i = 0; i < n - r; ++i) {
                        i_points[i][r] = (1 - t) * i_points[i][r - 1] + t * i_points[i + 1][r - 1];
                    }
                }
                return i_points;
            };

            std::array<Vector<dimensions, T >, order + 1> _points;
        };

        template<UnsignedInt dimensions, class T> using QuadraticBezier = Bezier<2, dimensions, T>;
        template<UnsignedInt dimensions, class T> using CubicBezier = Bezier <3, dimensions, T>;
        template<class T> using QuadraticBezier2D = QuadraticBezier<2, T>;
        template<class T> using QuadraticBezier3D = QuadraticBezier<3, T>;
        template<class T> using CubicBezier2D = CubicBezier<2, T>;
        template<class T> using CubicBezier3D = CubicBezier<3, T>;

    }

}
#endif //Magnum_Math_Bezier_h

