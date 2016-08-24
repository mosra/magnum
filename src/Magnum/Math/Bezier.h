#ifndef Magnum_Math_Bezier_h
#define Magnum_Math_Bezier_h

/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2016 Ashwin Ravichandran <ashwinravichandran24@gmail.com>

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

        /** @brief Default constructor */
        constexpr /*implicit*/ Bezier(ZeroInitT = ZeroInit): _points{} {}

        /** @brief Construct Bezier without initializing the contents */
        explicit Bezier(NoInitT) {}

        /** @brief Construct Bezier curve with the given array of control points */
        template<typename... U> constexpr Bezier(Vector<dimensions, T> first, U... next):_points{first, next...} {
            static_assert(sizeof...(U) + 1 == order + 1, "Bezier : Wrong number of arguments");
        }

        /**
         * @brief Divides a Bezier curve into two curves of same order having their own control points.
         *        De Casteljau's algorithm is used.
         * @param t The interpolation factor
         *
         * @return Array of two Bezier curves of the same order
         */
        std::array<Bezier<order, dimensions, T>, 2> subdivide(Float t) const {
            auto iPoints = calculateIntermediatePoints(t);
            Bezier<order, dimensions, T> left, right;
            for(std::size_t i = 0; i <= order; ++i) {
                left[i] = iPoints[0][i];
            }
            for(std::size_t i = 0, j = order; i <= order; --j, ++i) {
                right[i] = iPoints[i][j];
            }
            return {left, right};
        }

        /**
         * @brief Finds the point in the curve for a given interpolation factor
         *        De Casteljau's algorithm is used.
         * @param t The interpolation factor
         */
        Vector<dimensions, T> lerp(Float t) const {
            auto iPoints = calculateIntermediatePoints(t);
            return iPoints[0][order];
        }

        /**
         * @brief Control points of Bezier
         * @return One-dimensional array of `size` length.
         *
         * @see @ref operator[]()
         */
        Vector<dimensions, T>* points() { return _points; }
        constexpr const Vector<dimensions, T>* points() const { return _points; } /**< @overload */

        /**
         * @brief Value at given position
         *
         * @see @ref points()
         */
        Vector<dimensions, T>& operator[](std::size_t pos) { return _points[pos]; }
        constexpr Vector<dimensions, T> operator[](std::size_t pos) const { return _points[pos]; } /**< @overload */

    private:

        /**
         * @brief Calculates and returns all intermediate points generated when using De Casteljau's algorithm
         * @param t The interpolation factor
         *
         */
        std::array<Bezier<order, dimensions, T>, order + 1> calculateIntermediatePoints(Float t) const {
            std::array<Bezier<order, dimensions, T>, order + 1> iPoints;
            for(std::size_t i = 0; i <= order; ++i) {
                iPoints[i][0] = _points[i];
            }
            for(std::size_t r = 1; r <= order; ++r) {
                for(std::size_t i = 0; i <= order - r; ++i) {
                    iPoints[i][r] = (1 - t)*iPoints[i][r - 1] + t*iPoints[i + 1][r - 1];
                }
            }
            return iPoints;
        }

        Vector<dimensions, T> _points[order + 1];
};

template<UnsignedInt dimensions, class T> using QuadraticBezier = Bezier<2, dimensions, T>;
template<UnsignedInt dimensions, class T> using CubicBezier = Bezier<3, dimensions, T>;
template<class T> using QuadraticBezier2D = QuadraticBezier<2, T>;
template<class T> using QuadraticBezier3D = QuadraticBezier<3, T>;
template<class T> using CubicBezier2D = CubicBezier<2, T>;
template<class T> using CubicBezier3D = CubicBezier<3, T>;

}}
#endif

