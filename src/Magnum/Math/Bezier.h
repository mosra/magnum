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
 * @brief Class @ref Magnum::Math::Bezier, alias @ref Magnum::Math::QuadraticBezier, @ref Magnum::Math::QuadraticBezier2D, @ref Magnum::Math::QuadraticBezier3D, @ref Magnum::Math::CubicBezier, @ref Magnum::Math::CubicBezier2D, @ref Magnum::Math::CubicBezier3D
 */

#include <array>

#include "Magnum/Math/Vector.h"

namespace Magnum { namespace Math {

/**
@brief Bézier curve
@tparam order       Order of Bézier curve
@tparam dimensions  Dimensions of control points
@tparam T           Underlying data type

Implementation of M-order N-dimensional
[Bézier Curve](https://en.wikipedia.org/wiki/B%C3%A9zier_curve).
@see @ref QuadraticBezier, @ref CubicBezier, @ref QuadraticBezier2D,
    @ref QuadraticBezier3D, @ref CubicBezier2D, @ref CubicBezier3D
*/
template<UnsignedInt order, UnsignedInt dimensions, class T> class Bezier {
    public:
        typedef T Type;             /**< @brief Underlying data type */

        enum: UnsignedInt {
            Order = order,          /**< Order of Bézier curve */
            Dimensions = dimensions /**< Dimensions of control points */
        };

        /**
         * @brief Default constructor
         *
         * Construct the curve with all control points being zero vectors.
         */
        constexpr /*implicit*/ Bezier(ZeroInitT = ZeroInit) noexcept: _data{} {}

        /** @brief Construct Bézier without initializing the contents */
        explicit Bezier(NoInitT) noexcept {}

        /** @brief Construct Bézier curve with given array of control points */
        template<typename... U> constexpr Bezier(const Vector<dimensions, T>& first, U... next) noexcept: _data{first, next...} {
            static_assert(sizeof...(U) + 1 == order + 1, "Wrong number of arguments");
        }

        /**
         * @brief Subdivide the curve
         * @param t The interpolation factor
         *
         * Divides the curve into two Bézier curves of same order having their
         * own control points. Uses the [De Casteljau's algorithm](https://en.wikipedia.org/wiki/De_Casteljau%27s_algorithm).
         */
        std::array<Bezier<order, dimensions, T>, 2> subdivide(Float t) const {
            const auto iPoints = calculateIntermediatePoints(t);
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
         * @brief Interpolate the curve
         * @param t The interpolation factor
         *
         * Finds the point in the curve for a given interpolation factor. Uses
         * the [De Casteljau's algorithm](https://en.wikipedia.org/wiki/De_Casteljau%27s_algorithm).
         */
        Vector<dimensions, T> lerp(Float t) const {
            const auto iPoints = calculateIntermediatePoints(t);
            return iPoints[0][order];
        }

        /**
         * @brief Control point access
         *
         * @p i should not be larger than @ref Order.
         */
        Vector<dimensions, T>& operator[](std::size_t i) { return _data[i]; }
        constexpr Vector<dimensions, T> operator[](std::size_t i) const { return _data[i]; } /**< @overload */

    private:
        /* Calculates and returns all intermediate points generated when using De Casteljau's algorithm */
        std::array<Bezier<order, dimensions, T>, order + 1> calculateIntermediatePoints(Float t) const {
            std::array<Bezier<order, dimensions, T>, order + 1> iPoints;
            for(std::size_t i = 0; i <= order; ++i) {
                iPoints[i][0] = _data[i];
            }
            for(std::size_t r = 1; r <= order; ++r) {
                for(std::size_t i = 0; i <= order - r; ++i) {
                    iPoints[i][r] = (1 - t)*iPoints[i][r - 1] + t*iPoints[i + 1][r - 1];
                }
            }
            return iPoints;
        }

        Vector<dimensions, T> _data[order + 1];
};

/**
@brief Quadratic Bézier curve

Convenience alternative to `Bezier<2, dimensions, T>`. See @ref Bezier for more
information.
@see @ref QuadraticBezier2D, @ref QuadraticBezier3D
*/
template<UnsignedInt dimensions, class T> using QuadraticBezier = Bezier<2, dimensions, T>;

/**
@brief Two-dimensional quadratic Bézier curve

Convenience alternative to `QuadraticBezier<2, T>`. See @ref QuadraticBezier
and @ref Bezier for more information.
@see @ref QuadraticBezier3D
*/
template<class T> using QuadraticBezier2D = QuadraticBezier<2, T>;

/**
@brief Three-dimensional quadratic Bézier curve

Convenience alternative to `QuadraticBezier<3, T>`. See @ref QuadraticBezier
and @ref Bezier for more information.
@see @ref QuadraticBezier2D
*/
template<class T> using QuadraticBezier3D = QuadraticBezier<3, T>;

/**
@brief Cubic Bézier curve

Convenience alternative to `Bezier<3, dimensions, T>`. See @ref Bezier for more
information.
@see @ref CubicBezier2D, @ref CubicBezier3D
*/
template<UnsignedInt dimensions, class T> using CubicBezier = Bezier<3, dimensions, T>;

/**
@brief Two-dimensional cubic Bézier curve

Convenience alternative to `CubicBezier<2, T>`. See @ref CubicBezier
and @ref Bezier for more information.
@see @ref CubicBezier3D
*/
template<class T> using CubicBezier2D = CubicBezier<2, T>;

/**
@brief Three-dimensional cubic Bézier curve

Convenience alternative to `CubicBezier<3, T>`. See @ref CubicBezier
and @ref Bezier for more information.
@see @ref CubicBezier2D
*/
template<class T> using CubicBezier3D = CubicBezier<3, T>;

}}

#endif

