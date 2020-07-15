#ifndef Magnum_Math_CubicHermiteSpline_h
#define Magnum_Math_CubicHermiteSpline_h
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

/** @file
 * @brief Class @ref Magnum::Math::CubicHermite, alias @ref Magnum::Math::CubicHermite2D, @ref Magnum::Math::CubicHermite3D, function @ref Magnum::Math::select(), @ref Magnum::Math::lerp(), @ref Magnum::Math::splerp()
 */

#include "Magnum/Math/Complex.h"
#include "Magnum/Math/Quaternion.h"

namespace Magnum { namespace Math {

/**
@brief Cubic Hermite spline point

Represents a point on a [cubic Hermite spline](https://en.wikipedia.org/wiki/Cubic_Hermite_spline).

Unlike @ref Bezier, which describes a curve segment, this structure describes
a spline point @f$ \boldsymbol{p} @f$, with in-tangent @f$ \boldsymbol{m} @f$
and out-tangent @f$ \boldsymbol{n} @f$. This form is more suitable for
animation keyframe representation. The structure assumes the in/out tangents
to be in their final form, i.e. already normalized by length of their adjacent
segments.

Cubic Hermite splines are fully interchangeable with cubic Bézier curves, use
@ref fromBezier() and @ref Bezier::fromCubicHermite() for the conversion.

@see @ref CubicHermite2D, @ref CubicHermite3D,
    @ref Magnum::CubicHermite2D, @ref Magnum::CubicHermite2Dd,
    @ref Magnum::CubicHermite3D, @ref Magnum::CubicHermite3Dd,
    @ref CubicBezier
@experimental
*/
template<class T> class CubicHermite {
    public:
        typedef T Type;             /**< @brief Underlying data type */

        /**
         * @brief Create cubic Hermite spline point from adjacent Bézier curve segments
         *
         * Given two adjacent cubic Bézier curve segments defined by points
         * @f$ \boldsymbol{a}_i @f$ and @f$ \boldsymbol{b}_i @f$,
         * @f$ i \in \{ 0, 1, 2, 3 \} @f$, the corresponding cubic Hermite
         * spline point @f$ \boldsymbol{p} @f$, in-tangent @f$ \boldsymbol{m} @f$
         * and out-tangent @f$ \boldsymbol{n} @f$ is defined as: @f[
         *      \begin{array}{rcl}
         *          \boldsymbol{m} & = & 3 (\boldsymbol{a}_3 - \boldsymbol{a}_2)
         *                           =   3 (\boldsymbol{b}_0 - \boldsymbol{a}_2) \\
         *          \boldsymbol{p} & = & \boldsymbol{a}_3 = \boldsymbol{b}_0 \\
         *          \boldsymbol{n} & = & 3 (\boldsymbol{b}_1 - \boldsymbol{a}_3)
         *                           =   3 (\boldsymbol{b}_1 - \boldsymbol{b}_0)
         *      \end{array}
         * @f]
         *
         * Expects that the two segments are adjacent (i.e., the endpoint of
         * first segment is the start point of the second). If you need to
         * create a cubic Hermite spline point that's at the beginning or at
         * the end of a curve, simply pass a dummy Bézier segment that
         * satisfies this constraint as the other parameter:
         *
         * @snippet MagnumMath.cpp CubicHermite-fromBezier
         *
         * Enabled only on vector underlying types. See
         * @ref Bezier::fromCubicHermite() for the inverse operation.
         */
        template<UnsignedInt dimensions, class U> static
        #ifdef DOXYGEN_GENERATING_OUTPUT
        CubicHermite<T>
        #else
        typename std::enable_if<std::is_base_of<Vector<dimensions, U>, T>::value, CubicHermite<T>>::type
        #endif
        fromBezier(const CubicBezier<dimensions, U>& a, const CubicBezier<dimensions, U>& b) {
            return CORRADE_CONSTEXPR_ASSERT(a[3] == b[0],
                "Math::CubicHermite::fromBezier(): segments are not adjacent"),
                CubicHermite<T>{3*(a[3] - a[2]), a[3], 3*(b[1] - a[3])};
        }

        /**
         * @brief Default constructor
         *
         * Equivalent to @ref CubicHermite(ZeroInitT) for vector types and to
         * @ref CubicHermite(IdentityInitT) for complex and quaternion types.
         */
        constexpr /*implicit*/ CubicHermite() noexcept: CubicHermite{typename std::conditional<std::is_constructible<T, IdentityInitT>::value, IdentityInitT, ZeroInitT>::type{typename std::conditional<std::is_constructible<T, IdentityInitT>::value, IdentityInitT, ZeroInitT>::type::Init{}}} {}

        /**
         * @brief Default constructor
         *
         * Construct cubic Hermite spline point with all control points being
         * zero.
         */
        constexpr explicit CubicHermite(ZeroInitT) noexcept: CubicHermite{ZeroInit, typename std::conditional<std::is_constructible<T, ZeroInitT>::value, ZeroInitT*, void*>::type{}} {}

        /**
         * @brief Identity constructor
         *
         * The @ref point() is constructed as identity in order to have
         * interpolation working correctly; @ref inTangent() and
         * @ref outTangent() is constructed as zero. Enabled only for complex
         * and quaternion types.
         */
        template<class U = T, class = typename std::enable_if<std::is_constructible<U, IdentityInitT>::value>::type> constexpr explicit CubicHermite(IdentityInitT) noexcept: _inTangent{ZeroInit}, _point{IdentityInit}, _outTangent{ZeroInit} {}

        /** @brief Construct cubic Hermite spline point without initializing its contents */
        explicit CubicHermite(Magnum::NoInitT) noexcept: CubicHermite{Magnum::NoInit, typename std::conditional<std::is_constructible<T, Magnum::NoInitT>::value, Magnum::NoInitT*, void*>::type{}} {}

        /**
         * @brief Construct cubic Hermite spline point with given control points
         * @param inTangent     In-tangent @f$ \boldsymbol{m} @f$
         * @param point         Point @f$ \boldsymbol{p} @f$
         * @param outTangent    Out-tangent @f$ \boldsymbol{n} @f$
         */
        constexpr /*implicit*/ CubicHermite(const T& inTangent, const T& point, const T& outTangent) noexcept: _inTangent{inTangent}, _point{point}, _outTangent{outTangent} {}

        /**
         * @brief Construct subic Hermite spline point from another of different type
         *
         * Performs only default casting on the values, no rounding or
         * anything else.
         */
        template<class U> constexpr explicit CubicHermite(const CubicHermite<U>& other) noexcept: _inTangent{T(other._inTangent)}, _point{T(other._point)}, _outTangent{T(other._outTangent)} {}

        /**
         * @brief Raw data
         * @return One-dimensional array of three elements
         *
         * @see @ref inTangent(), @ref point(), @ref outTangent()
         */
        T* data() { return &_inTangent; }
        constexpr const T* data() const { return &_inTangent; } /**< @overload */

        /** @brief Equality comparison */
        bool operator==(const CubicHermite<T>& other) const;

        /** @brief Non-equality comparison */
        bool operator!=(const CubicHermite<T>& other) const {
            return !operator==(other);
        }

        /** @brief In-tangent @f$ \boldsymbol{m} @f$ */
        T& inTangent() { return _inTangent; }
        /* returns const& so [] operations are also constexpr */
        constexpr const T& inTangent() const { return _inTangent; } /**< @overload */

        /** @brief Point @f$ \boldsymbol{p} @f$ */
        T& point() { return _point; }
        /* returns const& so [] operations are also constexpr */
        constexpr const T& point() const { return _point; } /**< @overload */

        /** @brief Out-tangent @f$ \boldsymbol{n} @f$ */
        T& outTangent() { return _outTangent; }
        /* returns const& so [] operations are also constexpr */
        constexpr const T& outTangent() const { return _outTangent; } /**< @overload */

    private:
        template<class> friend class CubicHermite;

        /* Called from CubicHermite(ZeroInit), either using the ZeroInit
           constructor (if available) or passing zero directly (for scalar
           types) */
        constexpr explicit CubicHermite(ZeroInitT, ZeroInitT*) noexcept: _inTangent{ZeroInit}, _point{ZeroInit}, _outTangent{ZeroInit} {}
        constexpr explicit CubicHermite(ZeroInitT, void*) noexcept: _inTangent{T(0)}, _point{T(0)}, _outTangent{T(0)} {}

        /* Called from CubicHermite(NoInit), either using the NoInit
           constructor (if available) or not doing anything */
        explicit CubicHermite(Magnum::NoInitT, Magnum::NoInitT*) noexcept: _inTangent{Magnum::NoInit}, _point{Magnum::NoInit}, _outTangent{Magnum::NoInit} {}
        explicit CubicHermite(Magnum::NoInitT, void*) noexcept {}

        T _inTangent;
        T _point;
        T _outTangent;
};

/**
@brief Scalar cubic Hermite spline point

Convenience alternative to @cpp CubicHermite<T> @ce. See @ref CubicHermite for
more information.
@see @ref CubicHermite2D, @ref CubicHermite3D, @ref CubicHermiteComplex,
    @ref CubicHermiteQuaternion, @ref Magnum::CubicHermite1D,
    @ref Magnum::CubicHermite1Dd
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<class T> using CubicHermite1D = CubicHermite<T>;
#endif

/**
@brief Two-dimensional cubic Hermite spline point

Convenience alternative to @cpp CubicHermite<Vector2<T>> @ce. See
@ref CubicHermite for more information.
@see @ref CubicHermite1D, @ref CubicHermite3D, @ref CubicHermiteComplex,
    @ref CubicHermiteQuaternion, @ref Magnum::CubicHermite2D,
    @ref Magnum::CubicHermite2Dd
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<class T> using CubicHermite2D = CubicHermite<Vector2<T>>;
#endif

/**
@brief Three-dimensional cubic Hermite spline point

Convenience alternative to @cpp CubicHermite<Vector2<T>> @ce. See
@ref CubicHermite for more information.
@see @ref CubicHermite1D, @ref CubicHermite2D, @ref CubicHermiteComplex,
    @ref CubicHermiteQuaternion, @ref Magnum::CubicHermite3D,
    @ref Magnum::CubicHermite3Dd
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<class T> using CubicHermite3D = CubicHermite<Vector3<T>>;
#endif

/**
@brief Cubic Hermite spline complex number

Convenience alternative to @cpp CubicHermite<Complex<T>> @ce. See
@ref CubicHermite for more information.
@see @ref CubicHermite1D, @ref CubicHermite2D, @ref CubicHermite3D,
    @ref CubicHermiteQuaternion, @ref Magnum::CubicHermiteComplex,
    @ref Magnum::CubicHermiteComplexd
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<class T> using CubicHermiteComplex = CubicHermite<Complex<T>>;
#endif

/**
@brief Cubic Hermite spline quaternion

Convenience alternative to @cpp CubicHermite<Quaternion<T>> @ce. See
@ref CubicHermite for more information.
@see @ref CubicHermite1D, @ref CubicHermite2D, @ref CubicHermite3D,
    @ref CubicHermiteComplex, @ref Magnum::CubicHermiteQuaternion,
    @ref Magnum::CubicHermiteQuaterniond
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<class T> using CubicHermiteQuaternion = CubicHermite<Quaternion<T>>;
#endif

#ifndef CORRADE_NO_DEBUG
/** @debugoperator{CubicHermite} */
template<class T> Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug& debug, const CubicHermite<T>& value) {
    return debug << "CubicHermite(" << Corrade::Utility::Debug::nospace
        << value.inTangent() << Corrade::Utility::Debug::nospace << ","
        << value.point() << Corrade::Utility::Debug::nospace << ","
        << value.outTangent() << Corrade::Utility::Debug::nospace << ")";
}

/* Explicit instantiation for commonly used types */
#ifndef DOXYGEN_GENERATING_OUTPUT
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const CubicHermite<Float>&);
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const CubicHermite<Double>&);
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const CubicHermite<Vector2<Float>>&);
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const CubicHermite<Vector3<Float>>&);
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const CubicHermite<Vector4<Float>>&);
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const CubicHermite<Vector2<Double>>&);
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const CubicHermite<Vector3<Double>>&);
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const CubicHermite<Vector4<Double>>&);
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const CubicHermite<Complex<Float>>&);
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const CubicHermite<Complex<Double>>&);
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const CubicHermite<Quaternion<Float>>&);
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const CubicHermite<Quaternion<Double>>&);
#endif
#endif

/** @relatesalso CubicHermite
@brief Constant interpolation of two cubic Hermite spline points
@param a     First spline point
@param b     Second spline point
@param t     Interpolation phase (from range @f$ [0; 1] @f$)

Given segment points @f$ \boldsymbol{p}_i @f$, in-tangents @f$ \boldsymbol{m}_i @f$
and out-tangents @f$ \boldsymbol{n}_i @f$, the interpolated value @f$ \boldsymbol{p} @f$
at phase @f$ t @f$ is: @f[
    \boldsymbol{p}(t) = \begin{cases}
        \boldsymbol{p}_a, & t < 1 \\
        \boldsymbol{p}_b, & t \ge 1
    \end{cases}
@f]

Equivalent to calling @ref select(const T&, const T&, U) on
@ref CubicHermite::point() extracted from both @p a and @p b.
@see @ref lerp(const CubicHermite<T>&, const CubicHermite<T>&, U),
    @ref splerp(const CubicHermite<T>&, const CubicHermite<T>&, U)
*/
template<class T, class U> T select(const CubicHermite<T>& a, const CubicHermite<T>& b, U t) {
    /* Not using select() from Functions.h to avoid the header dependency */
    return t < U(1) ? a.point() : b.point();
}

/** @relatesalso CubicHermite
@brief Linear interpolation of two cubic Hermite points
@param a     First spline point
@param b     Second spline point
@param t     Interpolation phase (from range @f$ [0; 1] @f$)

Given segment points @f$ \boldsymbol{p}_i @f$, in-tangents @f$ \boldsymbol{m}_i @f$
and out-tangents @f$ \boldsymbol{n}_i @f$, the interpolated value @f$ \boldsymbol{p} @f$
at phase @f$ t @f$ is: @f[
    \boldsymbol{p}(t) = (1 - t) \boldsymbol{p}_a + t \boldsymbol{p}_b
@f]

Equivalent to calling @ref lerp(const T&, const T&, U) on
@ref CubicHermite::point() extracted from both @p a and @p b.
@see @ref lerp(const CubicHermiteComplex<T>&, const CubicHermiteComplex<T>&, T),
    @ref lerp(const CubicHermiteQuaternion<T>&, const CubicHermiteQuaternion<T>&, T),
    @ref lerpShortestPath(const CubicHermiteQuaternion<T>&, const CubicHermiteQuaternion<T>&, T),
    @ref slerp(const CubicHermiteComplex<T>&, const CubicHermiteComplex<T>&, T),
    @ref slerp(const CubicHermiteQuaternion<T>&, const CubicHermiteQuaternion<T>&, T),
    @ref slerpShortestPath(const CubicHermiteQuaternion<T>&, const CubicHermiteQuaternion<T>&, T),
    @ref select(const CubicHermite<T>&, const CubicHermite<T>&, U),
    @ref splerp(const CubicHermite<T>&, const CubicHermite<T>&, U),
    @ref splerp(const CubicHermiteComplex<T>&, const CubicHermiteComplex<T>&, T),
    @ref splerp(const CubicHermiteQuaternion<T>&, const CubicHermiteQuaternion<T>&, T)
*/
template<class T, class U> T lerp(const CubicHermite<T>& a, const CubicHermite<T>& b, U t) {
    /* To avoid dependency on Functions.h */
    return Implementation::lerp(a.point(), b.point(), t);
}

/** @relatesalso CubicHermite
@brief Linear interpolation of two cubic Hermite complex numbers

Equivalent to calling @ref lerp(const Complex<T>&, const Complex<T>&, T) on
@ref CubicHermite::point() extracted from @p a and @p b. Compared to
@ref lerp(const CubicHermite<T>&, const CubicHermite<T>&, U) this adds a
normalization step after. Expects that @ref CubicHermite::point() is a
normalized complex number in both @p a and @p b.
@see @ref Complex::isNormalized(),
    @ref lerp(const CubicHermiteQuaternion<T>&, const CubicHermiteQuaternion<T>&, T),
    @ref select(const CubicHermite<T>&, const CubicHermite<T>&, U),
    @ref slerp(const CubicHermiteComplex<T>&, const CubicHermiteComplex<T>&, T),
    @ref splerp(const CubicHermiteComplex<T>&, const CubicHermiteComplex<T>&, T)
*/
template<class T> Complex<T> lerp(const CubicHermiteComplex<T>& a, const CubicHermiteComplex<T>& b, T t) {
    return lerp(a.point(), b.point(), t);
}

/** @relatesalso CubicHermite
@brief Linear interpolation of two cubic Hermite quaternions

Equivalent to calling @ref lerp(const Quaternion<T>&, const Quaternion<T>&, T)
on @ref CubicHermite::point() extracted from @p a and @p b. Compared to
@ref lerp(const CubicHermite<T>&, const CubicHermite<T>&, U) this adds a
normalization step after. Expects that @ref CubicHermite::point() is a
normalized quaternion in both @p a and @p b.
@see @ref Quaternion::isNormalized(),
    @ref lerpShortestPath(const CubicHermiteQuaternion<T>&, const CubicHermiteQuaternion<T>&, T),
    @ref lerp(const CubicHermiteComplex<T>&, const CubicHermiteComplex<T>&, T),
    @ref select(const CubicHermite<T>&, const CubicHermite<T>&, U),
    @ref slerp(const CubicHermiteQuaternion<T>&, const CubicHermiteQuaternion<T>&, T),
    @ref splerp(const CubicHermiteQuaternion<T>&, const CubicHermiteQuaternion<T>&, T)
*/
template<class T> Quaternion<T> lerp(const CubicHermiteQuaternion<T>& a, const CubicHermiteQuaternion<T>& b, T t) {
    return lerp(a.point(), b.point(), t);
}

/** @relatesalso CubicHermite
@brief Linear shortest-path interpolation of two cubic Hermite quaternions

Equivalent to calling @ref lerpShortestPath(const Quaternion<T>&, const Quaternion<T>&, T)
on @ref CubicHermite::point() extracted from @p a and @p b. Expects that
@ref CubicHermite::point() is a normalized quaternion in both @p a and @p b.

Note that rotations interpolated with this function may go along a completely
different path compared to @ref splerp(const CubicHermiteQuaternion<T>&, const CubicHermiteQuaternion<T>&, T).
Use @ref lerp(const CubicHermiteQuaternion<T>&, const CubicHermiteQuaternion<T>&, T)
for behavior that is consistent with spline interpolation.
@see @ref Quaternion::isNormalized(),
    @ref slerpShortestPath(const CubicHermiteQuaternion<T>&, const CubicHermiteQuaternion<T>&, T)
*/
template<class T> Quaternion<T> lerpShortestPath(const CubicHermiteQuaternion<T>& a, const CubicHermiteQuaternion<T>& b, T t) {
    return lerpShortestPath(a.point(), b.point(), t);
}

/** @relatesalso CubicHermite
@brief Spherical linear interpolation of two cubic Hermite complex numbers

Equivalent to calling @ref slerp(const Complex<T>&, const Complex<T>&, T) on
@ref CubicHermite::point() extracted from @p a and @p b. Expects that
@ref CubicHermite::point() is a normalized complex number in both @p a and @p b.
@see @ref Complex::isNormalized(),
    @ref slerp(const CubicHermiteQuaternion<T>&, const CubicHermiteQuaternion<T>&, T),
    @ref select(const CubicHermite<T>&, const CubicHermite<T>&, U),
    @ref lerp(const CubicHermiteComplex<T>&, const CubicHermiteComplex<T>&, T),
    @ref splerp(const CubicHermiteComplex<T>&, const CubicHermiteComplex<T>&, T)
 */
template<class T> inline Complex<T> slerp(const CubicHermiteComplex<T>& a, const CubicHermiteComplex<T>& b, T t) {
    return slerp(a.point(), b.point(), t);
}

/** @relatesalso CubicHermite
@brief Spherical linear interpolation of two cubic Hermite quaternions

Equivalent to calling @ref slerp(const Quaternion<T>&, const Quaternion<T>&, T)
on @ref CubicHermite::point() extracted from @p a and @p b. Expects that
@ref CubicHermite::point() is a normalized quaternion in both @p a and @p b.
@see @ref Quaternion::isNormalized(),
    @ref slerpShortestPath(const CubicHermiteQuaternion<T>&, const CubicHermiteQuaternion<T>&, T),
    @ref slerp(const CubicHermiteComplex<T>&, const CubicHermiteComplex<T>&, T),
    @ref select(const CubicHermite<T>&, const CubicHermite<T>&, U),
    @ref lerp(const CubicHermiteQuaternion<T>&, const CubicHermiteQuaternion<T>&, T),
    @ref splerp(const CubicHermiteQuaternion<T>&, const CubicHermiteQuaternion<T>&, T)
 */
template<class T> inline Quaternion<T> slerp(const CubicHermiteQuaternion<T>& a, const CubicHermiteQuaternion<T>& b, T t) {
    return slerp(a.point(), b.point(), t);
}

/** @relatesalso CubicHermite
@brief Spherical linear shortest-path interpolation of two cubic Hermite quaternions

Equivalent to calling @ref slerpShortestPath(const Quaternion<T>&, const Quaternion<T>&, T)
on @ref CubicHermite::point() extracted from @p a and @p b. Expects that
@ref CubicHermite::point() is a normalized quaternion in both @p a and @p b.

Note that rotations interpolated with this function may go along a completely
different path compared to @ref splerp(const CubicHermiteQuaternion<T>&, const CubicHermiteQuaternion<T>&, T).
Use @ref slerp(const CubicHermiteQuaternion<T>&, const CubicHermiteQuaternion<T>&, T)
for spherical linear interpolation with behavior that is consistent with spline
interpolation.
@see @ref Quaternion::isNormalized(),
    @ref lerpShortestPath(const CubicHermiteQuaternion<T>&, const CubicHermiteQuaternion<T>&, T)
*/
template<class T> Quaternion<T> slerpShortestPath(const CubicHermiteQuaternion<T>& a, const CubicHermiteQuaternion<T>& b, T t) {
    return slerpShortestPath(a.point(), b.point(), t);
}

/** @relatesalso CubicHermite
@brief Spline interpolation of two cubic Hermite points
@param a     First spline point
@param b     Second spline point
@param t     Interpolation phase

Given segment points @f$ \boldsymbol{p}_i @f$, in-tangents @f$ \boldsymbol{m}_i @f$
and out-tangents @f$ \boldsymbol{n}_i @f$, the interpolated value @f$ \boldsymbol{p} @f$
at phase @f$ t @f$ is: @f[
    \boldsymbol{p}(t) = (2 t^3 - 3 t^2 + 1) \boldsymbol{p}_a + (t^3 - 2 t^2 + t) \boldsymbol{n}_a + (-2 t^3 + 3 t^2) \boldsymbol{p}_b + (t^3 - t^2)\boldsymbol{m}_b
@f]

@see @ref splerp(const CubicHermiteComplex<T>&, const CubicHermiteComplex<T>&, T),
    @ref splerp(const CubicHermiteQuaternion<T>&, const CubicHermiteQuaternion<T>&, T),
    @ref select(const CubicHermite<T>&, const CubicHermite<T>&, U),
    @ref lerp(const CubicHermite<T>&, const CubicHermite<T>&, U)
*/
template<class T, class U> T splerp(const CubicHermite<T>& a, const CubicHermite<T>& b, U t) {
    return (U(2)*t*t*t - U(3)*t*t + U(1))*a.point() +
        (t*t*t - U(2)*t*t + t)*a.outTangent() +
        (U(-2)*t*t*t + U(3)*t*t)*b.point() +
        (t*t*t - t*t)*b.inTangent();
}

/** @relatesalso CubicHermite
@brief Spline interpolation of two cubic Hermite complex numbers

Unlike @ref splerp(const CubicHermite<T>&, const CubicHermite<T>&, U) this adds
a normalization step after. Given segment points @f$ \boldsymbol{p}_i @f$,
in-tangents @f$ \boldsymbol{m}_i @f$ and out-tangents @f$ \boldsymbol{n}_i @f$,
the interpolated value @f$ \boldsymbol{p} @f$ at phase @f$ t @f$ is: @f[
    \begin{array}{rcl}
        \boldsymbol{p'}(t) & = & (2 t^3 - 3 t^2 + 1) \boldsymbol{p}_a + (t^3 - 2 t^2 + t) \boldsymbol{n}_a + (-2 t^3 + 3 t^2) \boldsymbol{p}_b + (t^3 - t^2)\boldsymbol{m}_b \\
        \boldsymbol{p}(t) & = & \cfrac{\boldsymbol{p'}(t)}{|\boldsymbol{p'}(t)|}
    \end{array}
@f]

Expects that @ref CubicHermite::point() is a normalized complex number in both
@p a and @p b.
@see @ref Complex::isNormalized(),
    @ref splerp(const CubicHermiteQuaternion<T>&, const CubicHermiteQuaternion<T>&, T),
    @ref select(const CubicHermite<T>&, const CubicHermite<T>&, U),
    @ref lerp(const CubicHermiteComplex<T>&, const CubicHermiteComplex<T>&, T),
    @ref slerp(const CubicHermiteComplex<T>&, const CubicHermiteComplex<T>&, T)
*/
template<class T> Complex<T> splerp(const CubicHermiteComplex<T>& a, const CubicHermiteComplex<T>& b, T t) {
    CORRADE_ASSERT(a.point().isNormalized() && b.point().isNormalized(),
        "Math::splerp(): complex spline points" << a.point() << "and" << b.point() << "are not normalized", {});
    return ((T(2)*t*t*t - T(3)*t*t + T(1))*a.point() +
        (t*t*t - T(2)*t*t + t)*a.outTangent() +
        (T(-2)*t*t*t + T(3)*t*t)*b.point() +
        (t*t*t - t*t)*b.inTangent()).normalized();
}

/** @relatesalso CubicHermite
@brief Spline interpolation of two cubic Hermite quaternions

Unlike @ref splerp(const CubicHermite<T>&, const CubicHermite<T>&, U) this adds
a normalization step after. Given segment points @f$ \boldsymbol{p}_i @f$,
in-tangents @f$ \boldsymbol{m}_i @f$ and out-tangents @f$ \boldsymbol{n}_i @f$,
the interpolated value @f$ \boldsymbol{p} @f$ at phase @f$ t @f$ is: @f[
    \begin{array}{rcl}
        \boldsymbol{p'}(t) & = & (2 t^3 - 3 t^2 + 1) \boldsymbol{p}_a + (t^3 - 2 t^2 + t) \boldsymbol{n}_a + (-2 t^3 + 3 t^2) \boldsymbol{p}_b + (t^3 - t^2)\boldsymbol{m}_b \\
        \boldsymbol{p}(t) & = & \cfrac{\boldsymbol{p'}(t)}{|\boldsymbol{p'}(t)|}
    \end{array}
@f]

Expects that @ref CubicHermite::point() is a normalized quaternion in both @p a
and @p b.
@see @ref Quaternion::isNormalized(),
    @ref splerp(const CubicHermiteComplex<T>&, const CubicHermiteComplex<T>&, T),
    @ref select(const CubicHermite<T>&, const CubicHermite<T>&, U),
    @ref lerp(const CubicHermiteQuaternion<T>&, const CubicHermiteQuaternion<T>&, T),
    @ref slerp(const CubicHermiteQuaternion<T>&, const CubicHermiteQuaternion<T>&, T)
*/
template<class T> Quaternion<T> splerp(const CubicHermiteQuaternion<T>& a, const CubicHermiteQuaternion<T>& b, T t) {
    CORRADE_ASSERT(a.point().isNormalized() && b.point().isNormalized(),
        "Math::splerp(): quaternion spline points" << a.point() << "and" << b.point() << "are not normalized", {});
    return ((T(2)*t*t*t - T(3)*t*t + T(1))*a.point() +
        (t*t*t - T(2)*t*t + t)*a.outTangent() +
        (T(-2)*t*t*t + T(3)*t*t)*b.point() +
        (t*t*t - t*t)*b.inTangent()).normalized();
}

template<class T> inline bool CubicHermite<T>::operator==(const CubicHermite<T>& other) const {
    /* For non-scalar types default implementation of TypeTraits would be used,
       which is just operator== */
    return TypeTraits<T>::equals(_inTangent, other._inTangent) &&
        TypeTraits<T>::equals(_point, other._point) &&
        TypeTraits<T>::equals(_outTangent, other._outTangent);
}

namespace Implementation {

template<class T> struct StrictWeakOrdering<CubicHermite<T>> {
    bool operator()(const CubicHermite<T>& a, const CubicHermite<T>& b) const {
        StrictWeakOrdering<T> o;
        if(o(a.inTangent(), b.inTangent()))
            return true;
        if(o(b.inTangent(), a.inTangent()))
            return false;
        if(o(a.point(), b.point()))
            return true;
        if(o(b.point(), a.point()))
            return false;
        return o(a.outTangent(), b.outTangent());
    }
};

}

}}

#endif
