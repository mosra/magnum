#ifndef Magnum_Math_Bezier_h
#define Magnum_Math_Bezier_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>
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

#include "Magnum/Math/Vector.h"

namespace Magnum { namespace Math {

namespace Implementation {
    template<UnsignedInt, UnsignedInt, class, class> struct BezierConverter;
}

/**
@brief Bézier curve
@tparam order       Order of Bézier curve
@tparam dimensions  Dimensions of control points
@tparam T           Underlying data type

Represents a M-order N-dimensional
[Bézier Curve](https://en.wikipedia.org/wiki/B%C3%A9zier_curve) segment.

Cubic Bézier curves are fully interchangeable with cubic Hermite splines, use
@ref fromCubicHermite() and @ref CubicHermite::fromBezier() for the conversion.

@see @ref QuadraticBezier, @ref CubicBezier, @ref QuadraticBezier2D,
    @ref QuadraticBezier3D, @ref CubicBezier2D, @ref CubicBezier3D
*/
template<UnsignedInt order, UnsignedInt dimensions, class T> class Bezier {
    static_assert(order != 0, "Bezier cannot have zero order");

    template<UnsignedInt, UnsignedInt, class> friend class Bezier;

    public:
        typedef T Type;             /**< @brief Underlying data type */

        enum: UnsignedInt {
            Order = order,          /**< Order of Bézier curve */
            Dimensions = dimensions /**< Dimensions of control points */
        };

        /**
         * @brief Create cubic Hermite spline point from adjacent Bézier curve segments
         *
         * Given two cubic Hermite spline points defined by points
         * @f$ \boldsymbol{p}_i @f$, in-tangents @f$ \boldsymbol{m}_i @f$ and
         * out-tangents @f$ \boldsymbol{n}_i @f$, the corresponding cubic
         * Bezier curve segment with points @f$ \boldsymbol{c}_0 @f$,
         * @f$ \boldsymbol{c}_1 @f$, @f$ \boldsymbol{c}_2 @f$ and
         * @f$ \boldsymbol{c}_3 @f$ is defined as: @f[
         *      \begin{array}{rcl}
         *          \boldsymbol{c}_0 & = & \boldsymbol{p}_a \\
         *          \boldsymbol{c}_1 & = & \frac{1}{3} \boldsymbol{n}_a - \boldsymbol{p}_a \\
         *          \boldsymbol{c}_2 & = & \boldsymbol{p}_b - \frac{1}{3} \boldsymbol{m}_b \\
         *          \boldsymbol{c}_3 & = & \boldsymbol{p}_b
         *      \end{array}
         * @f]
         *
         * Enabled only on @ref CubicBezier for @ref CubicHermite with vector
         * underlying types. See @ref CubicHermite::fromBezier() for the
         * inverse operation.
         */
        template<class VectorType> static
        #ifndef DOXYGEN_GENERATING_OUTPUT
        typename std::enable_if<std::is_base_of<Vector<dimensions, T>, VectorType>::value && order == 3, Bezier<order, dimensions, T>>::type
        #else
        Bezier<order, dimensions, T>
        #endif
        fromCubicHermite(const CubicHermite<VectorType>& a, const CubicHermite<VectorType>& b) {
            return {a.point(), a.outTangent()/T(3) - a.point(), b.point() - b.inTangent()/T(3), b.point()};
        }

        /**
         * @brief Default constructor
         *
         * Equivalent to @ref Bezier(ZeroInitT).
         */
        constexpr /*implicit*/ Bezier() noexcept: Bezier<order, dimensions, T>{typename Implementation::GenerateSequence<order + 1>::Type{}, ZeroInit} {}

        /**
         * @brief Construct a zero curve
         *
         * All control points are zero vectors.
         */
        constexpr explicit Bezier(ZeroInitT) noexcept: Bezier<order, dimensions, T>{typename Implementation::GenerateSequence<order + 1>::Type{}, ZeroInit} {}

        /** @brief Construct Bézier without initializing the contents */
        explicit Bezier(Magnum::NoInitT) noexcept: Bezier<order, dimensions, T>{typename Implementation::GenerateSequence<order + 1>::Type{}, Magnum::NoInit} {}

        /** @brief Construct Bézier curve with given array of control points */
        template<typename... U> constexpr /*implicit*/ Bezier(const Vector<dimensions, T>& first, U... next) noexcept: _data{first, next...} {
            static_assert(sizeof...(U) + 1 == order + 1, "Wrong number of arguments");
        }

        /**
         * @brief Construct Bézier curve from another of different type
         *
         * Performs only default casting on the values, no rounding or
         * anything else.
         */
        template<class U> constexpr explicit Bezier(const Bezier<order, dimensions, U>& other) noexcept: Bezier{typename Implementation::GenerateSequence<order + 1>::Type(), other} {}

        /** @brief Construct Bézier curve from external representation */
        template<class U, class V = decltype(Implementation::BezierConverter<order, dimensions, T, U>::from(std::declval<U>()))> constexpr explicit Bezier(const U& other) noexcept: Bezier<order, dimensions, T>{Implementation::BezierConverter<order, dimensions, T, U>::from(other)} {}

        /** @brief Convert Bézier curve to external representation */
        template<class U, class V = decltype(Implementation::BezierConverter<order, dimensions, T, U>::to(std::declval<Bezier<order, dimensions, T>>()))> constexpr explicit operator U() const {
            return Implementation::BezierConverter<order, dimensions, T, U>::to(*this);
        }

        /**
         * @brief Raw data
         * @return One-dimensional array of @cpp order + 1 @ce elements
         *
         * @see @ref operator[]()
         */
        Vector<dimensions, T>* data() { return _data; }
        constexpr const Vector<dimensions, T>* data() const { return _data; } /**< @overload */

        /** @brief Equality comparison */
        bool operator==(const Bezier<order, dimensions, T>& other) const {
            for(std::size_t i = 0; i != order + 1; ++i)
                if(_data[i] != other._data[i]) return false;
            return true;
        }

        /** @brief Non-equality comparison */
        bool operator!=(const Bezier<order, dimensions, T>& other) const {
            return !operator==(other);
        }

        /**
         * @brief Control point access
         *
         * @p i should not be larger than @ref Order.
         */
        Vector<dimensions, T>& operator[](std::size_t i) { return _data[i]; }
        /* returns const& so [][] operations are also constexpr */
        constexpr const Vector<dimensions, T>& operator[](std::size_t i) const { return _data[i]; } /**< @overload */

        /**
         * @brief Interpolate the curve at given position
         *
         * Returns point on the curve for given interpolation factor. Uses
         * the [De Casteljau's algorithm](https://en.wikipedia.org/wiki/De_Casteljau%27s_algorithm).
         * @see @ref subdivide()
         */
        Vector<dimensions, T> value(Float t) const {
            Bezier<order, dimensions, T> iPoints[order + 1];
            calculateIntermediatePoints(iPoints, t);
            return iPoints[0][order];
        }

        /**
         * @brief Subdivide the curve at given position
         *
         * Returns two Bézier curves following the original curve, split at
         * given interpolation factor. Uses the [De Casteljau's algorithm](https://en.wikipedia.org/wiki/De_Casteljau%27s_algorithm).
         * @see @ref value()
         */
        std::pair<Bezier<order, dimensions, T>, Bezier<order, dimensions, T>> subdivide(Float t) const {
            Bezier<order, dimensions, T> iPoints[order + 1];
            calculateIntermediatePoints(iPoints, t);
            Bezier<order, dimensions, T> left, right;
            for(std::size_t i = 0; i <= order; ++i)
                left[i] = iPoints[0][i];
            for(std::size_t i = 0, j = order; i <= order; --j, ++i)
                right[i] = iPoints[i][j];
            return {left, right};
        }

    private:
        /* Implementation for Bezier<order, dimensions, T>::Bezier(const Bezier<order, dimensions, U>&) */
        template<class U, std::size_t ...sequence> constexpr explicit Bezier(Implementation::Sequence<sequence...>, const Bezier<order, dimensions, U>& other) noexcept: _data{Vector<dimensions, T>(other._data[sequence])...} {}

        /* Implementation for Bezier<order, dimensions, T>::Bezier(ZeroInitT) and Bezier<order, dimensions, T>::Bezier(NoInitT) */
        /* MSVC 2015 can't handle {} here */
        template<class U, std::size_t ...sequence> constexpr explicit Bezier(Implementation::Sequence<sequence...>, U): _data{Vector<dimensions, T>((static_cast<void>(sequence), U{typename U::Init{}}))...} {}

        /* Calculates and returns all intermediate points generated when using De Casteljau's algorithm */
        void calculateIntermediatePoints(Bezier<order, dimensions, T>(&iPoints)[order + 1], Float t) const {
            for(std::size_t i = 0; i <= order; ++i) {
                iPoints[i][0] = _data[i];
            }
            for(std::size_t r = 1; r <= order; ++r) {
                for(std::size_t i = 0; i <= order - r; ++i) {
                    iPoints[i][r] = (1 - t)*iPoints[i][r - 1] + t*iPoints[i + 1][r - 1];
                }
            }
        }

        Vector<dimensions, T> _data[order + 1];
};

/**
@brief Quadratic Bézier curve

Convenience alternative to @cpp Bezier<2, dimensions, T> @ce. See @ref Bezier
for more information.
@see @ref QuadraticBezier2D, @ref QuadraticBezier3D
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<UnsignedInt dimensions, class T> using QuadraticBezier = Bezier<2, dimensions, T>;
#endif

/**
@brief Two-dimensional quadratic Bézier curve

Convenience alternative to @cpp QuadraticBezier<2, T> @ce. See
@ref QuadraticBezier and @ref Bezier for more information.
@see @ref QuadraticBezier3D, @ref Magnum::QuadraticBezier2D,
    @ref Magnum::QuadraticBezier2Dd
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<class T> using QuadraticBezier2D = QuadraticBezier<2, T>;
#endif

/**
@brief Three-dimensional quadratic Bézier curve

Convenience alternative to @cpp QuadraticBezier<3, T> @ce. See
@ref QuadraticBezier and @ref Bezier for more information.
@see @ref QuadraticBezier2D, @ref Magnum::QuadraticBezier3D,
    @ref Magnum::QuadraticBezier3Dd
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<class T> using QuadraticBezier3D = QuadraticBezier<3, T>;
#endif

/**
@brief Cubic Bézier curve

Convenience alternative to @cpp Bezier<3, dimensions, T> @ce. See @ref Bezier
for more information.
@see @ref CubicBezier2D, @ref CubicBezier3D
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<UnsignedInt dimensions, class T> using CubicBezier = Bezier<3, dimensions, T>;
#endif

/**
@brief Two-dimensional cubic Bézier curve

Convenience alternative to @cpp CubicBezier<2, T> @ce. See @ref CubicBezier and
@ref Bezier for more information.
@see @ref CubicBezier3D, @ref Magnum::CubicBezier2D,
    @ref Magnum::CubicBezier2Dd
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<class T> using CubicBezier2D = CubicBezier<2, T>;
#endif

/**
@brief Three-dimensional cubic Bézier curve

Convenience alternative to @cpp CubicBezier<3, T> @ce. See @ref CubicBezier and
@ref Bezier for more information.
@see @ref CubicBezier2D, @ref Magnum::CubicBezier3D,
    @ref Magnum::CubicBezier3Dd
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<class T> using CubicBezier3D = CubicBezier<3, T>;
#endif

#ifndef CORRADE_NO_DEBUG
/** @debugoperator{Bezier} */
template<UnsignedInt order, UnsignedInt dimensions, class T> Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug& debug, const Bezier<order, dimensions, T>& value) {
    debug << "Bezier(" << Corrade::Utility::Debug::nospace;
    for(UnsignedInt o = 0; o != order + 1; ++o) {
        debug << (o ? ", {" : "{") << Corrade::Utility::Debug::nospace << value[o][0] << Corrade::Utility::Debug::nospace;
        for(UnsignedInt i = 1; i != dimensions; ++i)
            debug << "," << value[o][i] << Corrade::Utility::Debug::nospace;
        debug << "}" << Corrade::Utility::Debug::nospace;
    }
    return debug << ")";
}

/* Explicit instantiation for commonly used types */
#ifndef DOXYGEN_GENERATING_OUTPUT
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const Bezier<2, 2, Float>&);
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const Bezier<2, 3, Float>&);
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const Bezier<3, 2, Float>&);
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const Bezier<3, 3, Float>&);
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const Bezier<2, 2, Double>&);
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const Bezier<2, 3, Double>&);
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const Bezier<3, 2, Double>&);
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const Bezier<3, 3, Double>&);
#endif
#endif

namespace Implementation {

template<UnsignedInt order, UnsignedInt dimensions, class T> struct StrictWeakOrdering<Bezier<order, dimensions, T>> {
    bool operator()(const Bezier<order, dimensions, T>& a, const Bezier<order, dimensions, T>& b) const {
        StrictWeakOrdering<Vector<dimensions, T>> o;
        for(std::size_t i = 0; i < order + 1; ++i) {
            if(o(a[i], b[i]))
                return true;
            if(o(b[i], a[i]))
                return false;
        }

        return false; /* a and b are equivalent */
    }
};

}

}}

#endif
