#ifndef Magnum_Math_Dual_h
#define Magnum_Math_Dual_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015
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
 * @brief Class @ref Magnum::Math::Dual
 */

#include <cmath>
#include <Corrade/Utility/Debug.h>

#include "Magnum/Math/Tags.h"
#include "Magnum/Math/TypeTraits.h"

namespace Magnum { namespace Math {

/**
@brief Dual number
@tparam T   Underlying data type
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
        constexpr /*implicit*/ Dual(): _real(), _dual() {}

        /** @brief Construct without initializing the contents */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        explicit Dual(NoInitT);
        #else
        template<class U = T, class = typename std::enable_if<std::is_pod<U>{}>::type> Dual(NoInitT) {}
        template<class U = T, class V = T, class = typename std::enable_if<std::is_constructible<U, NoInitT>{}>::type> Dual(NoInitT): _real{NoInit}, _dual{NoInit} {}
        #endif

        /**
         * @brief Construct dual number from real and dual part
         *
         * @f[
         *      \hat a = a_0 + \epsilon a_\epsilon
         * @f]
         */
        constexpr /*implicit*/ Dual(const T& real, const T& dual = T()): _real(real), _dual(dual) {}

        /** @brief Equality comparison */
        bool operator==(const Dual<T>& other) const {
            return TypeTraits<T>::equals(_real, other._real) &&
                   TypeTraits<T>::equals(_dual, other._dual);
        }

        /** @brief Non-equality comparison */
        bool operator!=(const Dual<T>& other) const {
            return !operator==(other);
        }

        /** @brief Real part */
        constexpr T real() const { return _real; }

        /** @brief Dual part */
        constexpr T dual() const { return _dual; }

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
         */
        template<class U> Dual<T> operator*(const Dual<U>& other) const {
            return {_real*other._real, _real*other._dual + _dual*other._real};
        }

        /**
         * @brief Divide by dual number
         *
         * @f[
         *      \frac{\hat a}{\hat b} = \frac{a_0}{b_0} + \epsilon \frac{a_\epsilon b_0 - a_0 b_\epsilon}{b_0^2}
         * @f]
         */
        template<class U> Dual<T> operator/(const Dual<U>& other) const {
            return {_real/other._real, (_dual*other._real - _real*other._dual)/(other._real*other._real)};
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

#ifndef DOXYGEN_GENERATING_OUTPUT
#define MAGNUM_DUAL_SUBCLASS_IMPLEMENTATION(Type, Underlying)               \
    Type<T> operator-() const {                                             \
        return Dual<Underlying<T>>::operator-();                            \
    }                                                                       \
    Type<T>& operator+=(const Dual<Underlying<T>>& other) {                 \
        Dual<Underlying<T>>::operator+=(other);                             \
        return *this;                                                       \
    }                                                                       \
    Type<T> operator+(const Dual<Underlying<T>>& other) const {             \
        return Dual<Underlying<T>>::operator+(other);                       \
    }                                                                       \
    Type<T>& operator-=(const Dual<Underlying<T>>& other) {                 \
        Dual<Underlying<T>>::operator-=(other);                             \
        return *this;                                                       \
    }                                                                       \
    Type<T> operator-(const Dual<Underlying<T>>& other) const {             \
        return Dual<Underlying<T>>::operator-(other);                       \
    }                                                                       \
    template<class U> Type<T> operator*(const Dual<U>& other) const {       \
        return Dual<Underlying<T>>::operator*(other);                       \
    }                                                                       \
    template<class U> Type<T> operator/(const Dual<U>& other) const {       \
        return Dual<Underlying<T>>::operator/(other);                       \
    }
#endif

/** @debugoperator{Magnum::Math::Dual} */
template<class T> Corrade::Utility::Debug operator<<(Corrade::Utility::Debug debug, const Dual<T>& value) {
    debug << "Dual(";
    debug.setFlag(Corrade::Utility::Debug::SpaceAfterEachValue, false);
    debug << value.real() << ", " << value.dual() << ")";
    debug.setFlag(Corrade::Utility::Debug::SpaceAfterEachValue, true);
    return debug;
}

/** @relates Dual
@brief Square root of dual number

@f[
    \sqrt{\hat a} = \sqrt{a_0} + \epsilon \frac{a_\epsilon}{2 \sqrt{a_0}}
@f]
@see @ref Math::sqrt(const T&)
*/
template<class T> Dual<T> sqrt(const Dual<T>& dual) {
    T sqrt0 = std::sqrt(dual.real());
    return {sqrt0, dual.dual()/(2*sqrt0)};
}

}}

#endif
