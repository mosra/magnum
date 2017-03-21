#ifndef Magnum_Math_Bezier_h
#define Magnum_Math_Bezier_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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

namespace Implementation {
    template<UnsignedInt, UnsignedInt, class, class> struct BezierConverter;
}

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
    static_assert(order != 0, "Bezier cannot have zero order");

    template<UnsignedInt, UnsignedInt, class> friend class Bezier;

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
        constexpr /*implicit*/ Bezier(ZeroInitT = ZeroInit) noexcept
            /** @todoc remove workaround when doxygen is sane */
            #ifndef DOXYGEN_GENERATING_OUTPUT
            : Bezier<order, dimensions, T>{typename Implementation::GenerateSequence<order + 1>::Type{}, ZeroInit}
            #endif
            {}

        /** @brief Construct Bézier without initializing the contents */
        explicit Bezier(NoInitT) noexcept
            /** @todoc remove workaround when doxygen is sane */
            #ifndef DOXYGEN_GENERATING_OUTPUT
            : Bezier<order, dimensions, T>{typename Implementation::GenerateSequence<order + 1>::Type{}, NoInit}
            #endif
            {}

        /** @brief Construct Bézier curve with given array of control points */
        template<typename... U> constexpr Bezier(const Vector<dimensions, T>& first, U... next) noexcept: _data{first, next...} {
            static_assert(sizeof...(U) + 1 == order + 1, "Wrong number of arguments");
        }

        /**
         * @brief Construct Bézier curve from another of different type
         *
         * Performs only default casting on the values, no rounding or
         * anything else.
         */
        template<class U> constexpr explicit Bezier(const Bezier<order, dimensions, U>& other) noexcept: Bezier{typename Implementation::GenerateSequence<order + 1>::Type(), other} {}

        /** @brief Construct Bézier from external representation */
        template<class U, class V = decltype(Implementation::BezierConverter<order, dimensions, T, U>::from(std::declval<U>()))> constexpr explicit Bezier(const U& other) noexcept: Bezier<order, dimensions, T>{Implementation::BezierConverter<order, dimensions, T, U>::from(other)} {}

        /** @brief Convert Bézier to external representation */
        template<class U, class V = decltype(Implementation::BezierConverter<order, dimensions, T, U>::to(std::declval<Bezier<order, dimensions, T>>()))> constexpr explicit operator U() const {
            return Implementation::BezierConverter<order, dimensions, T, U>::to(*this);
        }

        /** @brief Equality comparison */
        bool operator==(const Bezier<order, dimensions, T>& other) const {
            for(std::size_t i = 0; i != order + 1; ++i)
                if((*this)[i] != other[i]) return false;
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
            return calculateIntermediatePoints(t)[0][order];
        }

        /**
         * @brief Subdivide the curve at given position
         *
         * Returns two Bézier curves following the original curve, split at
         * given interpolation factor. Uses the [De Casteljau's algorithm](https://en.wikipedia.org/wiki/De_Casteljau%27s_algorithm).
         * @see @ref value()
         */
        std::pair<Bezier<order, dimensions, T>, Bezier<order, dimensions, T>> subdivide(Float t) const {
            const auto iPoints = calculateIntermediatePoints(t);
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
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<UnsignedInt dimensions, class T> using QuadraticBezier = Bezier<2, dimensions, T>;
#endif

/**
@brief Two-dimensional quadratic Bézier curve

Convenience alternative to `QuadraticBezier<2, T>`. See @ref QuadraticBezier
and @ref Bezier for more information.
@see @ref QuadraticBezier3D, @ref Magnum::QuadraticBezier2D,
    @ref Magnum::QuadraticBezier2Dd
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<class T> using QuadraticBezier2D = QuadraticBezier<2, T>;
#endif

/**
@brief Three-dimensional quadratic Bézier curve

Convenience alternative to `QuadraticBezier<3, T>`. See @ref QuadraticBezier
and @ref Bezier for more information.
@see @ref QuadraticBezier2D, @ref Magnum::QuadraticBezier3D,
    @ref Magnum::QuadraticBezier3Dd
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<class T> using QuadraticBezier3D = QuadraticBezier<3, T>;
#endif

/**
@brief Cubic Bézier curve

Convenience alternative to `Bezier<3, dimensions, T>`. See @ref Bezier for more
information.
@see @ref CubicBezier2D, @ref CubicBezier3D
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<UnsignedInt dimensions, class T> using CubicBezier = Bezier<3, dimensions, T>;
#endif

/**
@brief Two-dimensional cubic Bézier curve

Convenience alternative to `CubicBezier<2, T>`. See @ref CubicBezier
and @ref Bezier for more information.
@see @ref CubicBezier3D, @ref Magnum::CubicBezier2D,
    @ref Magnum::CubicBezier2Dd
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<class T> using CubicBezier2D = CubicBezier<2, T>;
#endif

/**
@brief Three-dimensional cubic Bézier curve

Convenience alternative to `CubicBezier<3, T>`. See @ref CubicBezier
and @ref Bezier for more information.
@see @ref CubicBezier2D, @ref Magnum::CubicBezier3D,
    @ref Magnum::CubicBezier3Dd
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<class T> using CubicBezier3D = CubicBezier<3, T>;
#endif

/** @debugoperator{Magnum::Math::Bezier} */
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

}}

namespace Corrade { namespace Utility {

/** @configurationvalue{Magnum::Math::Bezier} */
template<Magnum::UnsignedInt order, Magnum::UnsignedInt dimensions, class T> struct ConfigurationValue<Magnum::Math::Bezier<order, dimensions, T>> {
    ConfigurationValue() = delete;

    /** @brief Writes elements separated with spaces */
    static std::string toString(const Magnum::Math::Bezier<order, dimensions, T>& value, ConfigurationValueFlags flags) {
        std::string output;

        for(std::size_t o = 0; o != order + 1; ++o) {
            for(std::size_t i = 0; i != dimensions; ++i) {
                if(!output.empty()) output += ' ';
                output += ConfigurationValue<T>::toString(value[o][i], flags);
            }
        }

        return output;
    }

    /** @brief Reads elements separated with whitespace */
    static Magnum::Math::Bezier<order, dimensions, T> fromString(const std::string& stringValue, ConfigurationValueFlags flags) {
        Magnum::Math::Bezier<order, dimensions, T> result;

        std::size_t oldpos = 0, pos = std::string::npos, i = 0;
        do {
            pos = stringValue.find(' ', oldpos);
            std::string part = stringValue.substr(oldpos, pos-oldpos);

            if(!part.empty()) {
                result[i/dimensions][i%dimensions] = ConfigurationValue<T>::fromString(part, flags);
                ++i;
            }

            oldpos = pos+1;
        } while(pos != std::string::npos);

        return result;
    }
};

/* Explicit instantiation for commonly used types */
#if !defined(DOXYGEN_GENERATING_OUTPUT) && !defined(__MINGW32__)
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Bezier<2, 2, Magnum::Float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Bezier<2, 3, Magnum::Float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Bezier<3, 2, Magnum::Float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Bezier<3, 3, Magnum::Float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Bezier<2, 2, Magnum::Double>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Bezier<2, 3, Magnum::Double>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Bezier<3, 2, Magnum::Double>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Bezier<3, 3, Magnum::Double>>;
#endif

}}

#endif

