#ifndef Magnum_Math_Dual_h
#define Magnum_Math_Dual_h
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
 * @brief Class @ref Magnum::Math::Dual
 */

#ifndef CORRADE_NO_DEBUG
#include <Corrade/Utility/Debug.h>
#endif
#include <Corrade/Utility/StlMath.h>
#include <Corrade/Utility/TypeTraits.h>

#include "Magnum/Math/Angle.h"
#include "Magnum/Math/Tags.h"
#include "Magnum/Math/TypeTraits.h"

namespace Magnum { namespace Math {

namespace Implementation {
    CORRADE_HAS_TYPE(IsDual, decltype(std::declval<const T>().dual()));
}

/**
@brief Dual number
@tparam T   Underlying data type

Usually denoted as the following in equations, with @f$ a_0 @f$ being the
@ref real() part and @f$ a_\epsilon @f$ the @ref dual() part: @f[
    \hat a = a_0 + \epsilon a_\epsilon
@f]
*/
template<class T> class Dual {
    template<class> friend class Dual;

    public:
        typedef T Type;                         /**< @brief Underlying data type */

        /**
         * @brief Default constructor
         *
         * Both parts are default-constructed.
         */
        constexpr /*implicit*/ Dual() noexcept: _real{}, _dual{} {}

        /** @brief Construct zero-initialized dual number */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        constexpr explicit Dual(ZeroInitT) noexcept;
        #else
        /* MSVC 2015 can't handle {} instead of ::value */
        template<class U = T, class = typename std::enable_if<std::is_standard_layout<U>::value && std::is_trivial<U>::value>::type> constexpr explicit Dual(ZeroInitT) noexcept: _real{}, _dual{} {}
        template<class U = T, class V = T, class = typename std::enable_if<std::is_constructible<U, ZeroInitT>::value>::type> constexpr explicit Dual(ZeroInitT) noexcept: _real{ZeroInit}, _dual{ZeroInit} {}
        #endif

        /** @brief Construct without initializing the contents */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        explicit Dual(NoInitT) noexcept;
        #else
        /* MSVC 2015 can't handle {} instead of ::value */
        template<class U = T, class = typename std::enable_if<std::is_standard_layout<U>::value && std::is_trivial<U>::value>::type> explicit Dual(Magnum::NoInitT) noexcept {}
        template<class U = T, class V = T, class = typename std::enable_if<std::is_constructible<U, Magnum::NoInitT>::value>::type> explicit Dual(Magnum::NoInitT) noexcept: _real{Magnum::NoInit}, _dual{Magnum::NoInit} {}
        #endif

        /**
         * @brief Construct dual number from real and dual part
         *
         * @f[
         *      \hat a = a_0 + \epsilon a_\epsilon
         * @f]
         */
        #if !defined(CORRADE_MSVC2017_COMPATIBILITY) || defined(CORRADE_MSVC2015_COMPATIBILITY)
        constexpr /*implicit*/ Dual(const T& real, const T& dual = T()) noexcept: _real(real), _dual(dual) {}
        #else
        /* The default parameter makes MSVC2017 confused -- "expression does
           not evaluate to a constant". MSVC2015 works. */
        constexpr /*implicit*/ Dual(const T& real, const T& dual) noexcept: _real(real), _dual(dual) {}
        constexpr /*implicit*/ Dual(const T& real) noexcept: _real(real), _dual() {}
        #endif

        /**
         * @brief Construct dual number from another of different type
         *
         * Performs only default casting on the values, no rounding or anything
         * else. Example usage:
         *
         * @snippet MagnumMath.cpp Dual-conversion
         */
        template<class U> constexpr explicit Dual(const Dual<U>& other) noexcept: _real{T(other._real)}, _dual{T(other._dual)} {}

        /** @brief Copy constructor */
        constexpr /*implicit*/ Dual(const Dual<T>&) noexcept = default;

        /**
         * @brief Raw data
         * @return One-dimensional array of two elements
         *
         * @see @ref real(), @ref dual()
         */
        T* data() { return &_real; }
        constexpr const T* data() const { return &_real; } /**< @overload */

        /** @brief Equality comparison */
        bool operator==(const Dual<T>& other) const {
            return TypeTraits<T>::equals(_real, other._real) &&
                   TypeTraits<T>::equals(_dual, other._dual);
        }

        /** @brief Non-equality comparison */
        bool operator!=(const Dual<T>& other) const {
            return !operator==(other);
        }

        /**
         * @brief Real part (@f$ a_0 @f$)
         *
         * @see @ref data()
         */
        T& real() { return _real; }
        /* Returning const so it's possible to call constexpr functions on the
           result. WTF, C++?! */
        constexpr const T real() const { return _real; } /**< @overload */

        /**
         * @brief Dual part (@f$ a_\epsilon @f$)
         *
         * @see @ref data()
         */
        T& dual() { return _dual; }
        /* Returning const so it's possible to call constexpr functions on the
           result. WTF, C++?! */
        constexpr const T dual() const { return _dual; } /**< @overload */

        /**
         * @brief Add and assign dual number
         *
         * The computation is done in-place. @f[
         *      \hat a + \hat b = a_0 + b_0 + \epsilon (a_\epsilon + b_\epsilon)
         * @f]
         */
        Dual<T>& operator+=(const Dual<T>& other) {
            _real += other._real;
            _dual += other._dual;
            return *this;
        }

        /**
         * @brief Add dual number
         *
         * @see @ref operator+=()
         */
        Dual<T> operator+(const Dual<T>& other) const {
            return Dual<T>(*this)+=other;
        }

        /**
         * @brief Negated dual number
         *
         * @f[
         *      -\hat a = -a_0 - \epsilon a_\epsilon
         * @f]
         */
        Dual<T> operator-() const {
            return {-_real, -_dual};
        }

        /**
         * @brief Subtract and assign dual number
         *
         * The computation is done in-place. @f[
         *      \hat a - \hat b = a_0 - b_0 + \epsilon (a_\epsilon - b_\epsilon)
         * @f]
         */
        Dual<T>& operator-=(const Dual<T>& other) {
            _real -= other._real;
            _dual -= other._dual;
            return *this;
        }

        /**
         * @brief Subtract dual number
         *
         * @see @ref operator-=()
         */
        Dual<T> operator-(const Dual<T>& other) const {
            return Dual<T>(*this)-=other;
        }

        /**
         * @brief Multiply by dual number
         *
         * @f[
         *      \hat a \hat b = a_0 b_0 + \epsilon (a_0 b_\epsilon + a_\epsilon b_0)
         * @f]
         * @see @ref operator*(const U&) const,
         *      @ref operator*(const T&, const Dual<U>&)
         */
        template<class U> auto operator*(const Dual<U>& other) const -> Dual<decltype(std::declval<T>()*std::declval<U>())> {
            return {_real*other._real, _real*other._dual + _dual*other._real};
        }

        /**
         * @brief Multiply by real number
         *
         * Equivalent to the above assuming that @f$ b_\epsilon = 0 @f$.
         * @f[
         *      \hat a \hat b = a_0 b_0 + \epsilon (a_0 b_\epsilon + a_\epsilon b_0) = a_0 b_0 + \epsilon a_\epsilon b_0
         * @f]
         * @see @ref operator*(const Dual<U>&) const,
         *      @ref operator*(const T&, const Dual<U>&)
         */
        template<class U, class V = typename std::enable_if<!Implementation::IsDual<U>::value, void>::type> Dual<decltype(std::declval<T>()*std::declval<U>())> operator*(const U& other) const {
            return {_real*other, _dual*other};
        }

        /**
         * @brief Divide by dual number
         *
         * @f[
         *      \frac{\hat a}{\hat b} = \frac{a_0}{b_0} + \epsilon \frac{a_\epsilon b_0 - a_0 b_\epsilon}{b_0^2}
         * @f]
         * @see @ref operator/(const U&) const
         */
        template<class U> auto operator/(const Dual<U>& other) const -> Dual<decltype(std::declval<T>()/std::declval<U>())> {
            return {_real/other._real, (_dual*other._real - _real*other._dual)/(other._real*other._real)};
        }

        /**
         * @brief Divide by real number
         *
         * Equivalent to the above assuming that @f$ b_\epsilon = 0 @f$.
         * @f[
         *      \frac{\hat a}{\hat b} = \frac{a_0}{b_0} + \epsilon \frac{a_\epsilon b_0 - a_0 b_\epsilon}{b_0^2} = \frac{a_0}{b_0} + \epsilon \frac{a_\epsilon}{b_0}
         * @f]
         * @see @ref operator/(const Dual<U>&) const
         */
        template<class U, class V = typename std::enable_if<!Implementation::IsDual<U>::value, Dual<decltype(std::declval<T>()/std::declval<U>())>>::type> V operator/(const U& other) const {
            return {_real/other, _dual/other};
        }

        /**
         * @brief Conjugated dual number
         *
         * @f[
         *      \overline{\hat a} = a_0 - \epsilon a_\epsilon
         * @f]
         */
        Dual<T> conjugated() const {
            return {_real, -_dual};
        }

    private:
        T _real, _dual;
};

/** @relates Dual
@brief Multiply real number by dual number

Equivalent to @ref Dual::operator*(const Dual<U>&) const assuming that
@f$ a_\epsilon = 0 @f$.
@f[
     \hat a \hat b = a_0 b_0 + \epsilon (a_0 b_\epsilon + a_\epsilon b_0) = a_0 b_0 + \epsilon a_0 b_\epsilon
@f]
*/
template<class T, class U, class V = typename std::enable_if<!Implementation::IsDual<T>::value, Dual<decltype(std::declval<T>()*std::declval<U>())>>::type> inline V operator*(const T& a, const Dual<U>& b) {
    return {a*b.real(), a*b.dual()};
}

#ifndef DOXYGEN_GENERATING_OUTPUT
#define MAGNUM_DUAL_SUBCLASS_IMPLEMENTATION(Type, Underlying, Multiplicable) \
    Type<T> operator-() const {                                             \
        return Math::Dual<Underlying<T>>::operator-();                      \
    }                                                                       \
    Type<T>& operator+=(const Math::Dual<Underlying<T>>& other) {           \
        Math::Dual<Underlying<T>>::operator+=(other);                       \
        return *this;                                                       \
    }                                                                       \
    Type<T> operator+(const Math::Dual<Underlying<T>>& other) const {       \
        return Math::Dual<Underlying<T>>::operator+(other);                 \
    }                                                                       \
    Type<T>& operator-=(const Math::Dual<Underlying<T>>& other) {           \
        Math::Dual<Underlying<T>>::operator-=(other);                       \
        return *this;                                                       \
    }                                                                       \
    Type<T> operator-(const Math::Dual<Underlying<T>>& other) const {       \
        return Math::Dual<Underlying<T>>::operator-(other);                 \
    }                                                                       \
    Type<T> operator*(const Math::Dual<Multiplicable>& other) const {       \
        return Math::Dual<Underlying<T>>::operator*(other);                 \
    }                                                                       \
    Type<T> operator*(const Multiplicable& other) const {                   \
        return Math::Dual<Underlying<T>>::operator*(other);                 \
    }                                                                       \
    Type<T> operator/(const Math::Dual<Multiplicable>& other) const {       \
        return Math::Dual<Underlying<T>>::operator/(other);                 \
    }                                                                       \
    Type<T> operator/(const Multiplicable& other) const {                   \
        return Math::Dual<Underlying<T>>::operator/(other);                 \
    }

/* DualComplex needs its own special implementation of multiplication/division */
#define MAGNUM_DUAL_SUBCLASS_MULTIPLICATION_IMPLEMENTATION(Type, Underlying) \
    template<class U> Type<T> operator*(const Math::Dual<U>& other) const { \
        return Math::Dual<Underlying<T>>::operator*(other);                 \
    }                                                                       \
    template<class U> Type<T> operator/(const Math::Dual<U>& other) const { \
        return Math::Dual<Underlying<T>>::operator/(other);                 \
    }                                                                       \
    Type<T> operator*(const Math::Dual<Underlying<T>>& other) const {       \
        return Math::Dual<Underlying<T>>::operator*(other);                 \
    }                                                                       \
    Type<T> operator/(const Math::Dual<Underlying<T>>& other) const {       \
        return Math::Dual<Underlying<T>>::operator/(other);                 \
    }

#define MAGNUM_DUAL_OPERATOR_IMPLEMENTATION(Type, Underlying, Multiplicable) \
    template<class T> inline Type<T> operator*(const Math::Dual<Multiplicable>& a, const Type<T>& b) { \
        return a*static_cast<const Math::Dual<Underlying<T>>&>(b);          \
    }                                                                       \
    template<class T> inline Type<T> operator*(const Multiplicable& a, const Type<T>& b) { \
        return a*static_cast<const Math::Dual<Underlying<T>>&>(b);          \
    }                                                                       \
    template<class T> inline Type<T> operator/(const Math::Dual<Multiplicable>& a, const Type<T>& b) { \
        return a/static_cast<const Math::Dual<Underlying<T>>&>(b);          \
    }
#endif

#ifndef CORRADE_NO_DEBUG
/** @debugoperator{Dual} */
template<class T> Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug& debug, const Dual<T>& value) {
    return debug << "Dual(" << Corrade::Utility::Debug::nospace
        << value.real() << Corrade::Utility::Debug::nospace << ","
        << value.dual() << Corrade::Utility::Debug::nospace << ")";
}
#endif

/** @relatesalso Dual
@brief Square root of dual number

@f[
    \sqrt{\hat a} = \sqrt{a_0} + \epsilon \frac{a_\epsilon}{2 \sqrt{a_0}}
@f]
@see @ref sqrt(T)
*/
template<class T> Dual<T> sqrt(const Dual<T>& dual) {
    T sqrt0 = std::sqrt(dual.real());
    return {sqrt0, dual.dual()/(2*sqrt0)};
}

/** @relatesalso Dual
@brief Sine and cosine of dual angle

@f[
\begin{array}{rcl}
    sin(\hat a) & = & sin(a_0) + \epsilon a_\epsilon cos(a_0) \\
    cos(\hat a) & = & cos(a_0) - \epsilon a_\epsilon sin(a_0)
\end{array}
@f]
@see @ref sincos(Rad<T>)
*/
/* The function accepts Unit instead of Rad to make it working with operator
   products (e.g. 2*35.0_degf, which is of type Unit) */
template<class T> std::pair<Dual<T>, Dual<T>> sincos(const Dual<Rad<T>>& angle)
{
    /* Not using Math::sincos(), because I don't want to include Functions.h */
    const T sin = std::sin(T(angle.real()));
    const T cos = std::cos(T(angle.real()));
    return {{sin, T(angle.dual())*cos}, {cos, -T(angle.dual())*sin}};
}
#ifndef DOXYGEN_GENERATING_OUTPUT
template<class T> std::pair<Dual<T>, Dual<T>> sincos(const Dual<Deg<T>>& angle) { return sincos(Dual<Rad<T>>(angle)); }
template<class T> std::pair<Dual<T>, Dual<T>> sincos(const Dual<Unit<Rad, T>>& angle) { return sincos(Dual<Rad<T>>(angle)); }
template<class T> std::pair<Dual<T>, Dual<T>> sincos(const Dual<Unit<Deg, T>>& angle) { return sincos(Dual<Rad<T>>(angle)); }
#endif

namespace Implementation {

template<class T> struct StrictWeakOrdering<Dual<T>> {
    bool operator()(const Dual<T>& a, const Dual<T>& b) const {
        StrictWeakOrdering<T> o;
        if(o(a.real(), b.real()))
            return true;
        if(o(b.real(), a.real()))
            return false;

        return o(a.dual(), b.dual());
    }
};

}

}}

#endif
