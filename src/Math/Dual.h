#ifndef Magnum_Math_Dual_h
#define Magnum_Math_Dual_h
/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

/** @file
 * @brief Class Magnum::Math::Dual
 */

#include <Utility/Debug.h>

#include "Math/MathTypeTraits.h"

namespace Magnum { namespace Math {

/** @brief %Dual number */
template<class T> class Dual {
    template<class U> friend class Dual;

    public:
        /**
         * @brief Default constructor
         *
         * Both parts are default-constructed.
         */
        inline constexpr /*implicit*/ Dual(): _real(), _dual() {}

        /**
         * @brief Construct dual number from real and dual part
         *
         * @f[
         *      \hat a = a_0 + \epsilon a_\epsilon
         * @f]
         */
        inline constexpr /*implicit*/ Dual(const T& real, const T& dual = T()): _real(real), _dual(dual) {}

        /** @brief Equality comparison */
        inline bool operator==(const Dual<T>& other) const {
            return MathTypeTraits<T>::equals(_real, other._real) &&
                   MathTypeTraits<T>::equals(_dual, other._dual);
        }

        /** @brief Non-equality comparison */
        inline bool operator!=(const Dual<T>& other) const {
            return !operator==(other);
        }

        /** @brief Real part */
        inline constexpr T real() const { return _real; }

        /** @brief %Dual part */
        inline constexpr T dual() const { return _dual; }

        /**
         * @brief Add and assign dual number
         *
         * The computation is done in-place. @f[
         *      \hat a + \hat b = a_0 + b_0 + \epsilon (a_\epsilon + b_\epsilon)
         * @f]
         */
        inline Dual<T>& operator+=(const Dual<T>& other) {
            _real += other._real;
            _dual += other._dual;
            return *this;
        }

        /**
         * @brief Add dual number
         *
         * @see operator+=()
         */
        inline Dual<T> operator+(const Dual<T>& other) const {
            return Dual<T>(*this)+=other;
        }

        /**
         * @brief Negated dual number
         *
         * @f[
         *      -\hat a = -a_0 - \epsilon a_\epsilon
         * @f]
         */
        inline Dual<T> operator-() const {
            return {-_real, -_dual};
        }

        /**
         * @brief Subtract and assign dual number
         *
         * The computation is done in-place. @f[
         *      \hat a - \hat b = a_0 - b_0 + \epsilon (a_\epsilon - b_\epsilon)
         * @f]
         */
        inline Dual<T>& operator-=(const Dual<T>& other) {
            _real -= other._real;
            _dual -= other._dual;
            return *this;
        }

        /**
         * @brief Subtract dual number
         *
         * @see operator-=()
         */
        inline Dual<T> operator-(const Dual<T>& other) const {
            return Dual<T>(*this)-=other;
        }

        /**
         * @brief Multiply by dual number
         *
         * @f[
         *      \hat a \hat b = a_0 b_0 + \epsilon (a_0 b_\epsilon + a_\epsilon b_0)
         * @f]
         */
        inline Dual<T> operator*(const Dual<T>& other) const {
            return {_real*other._real, _real*other._dual + _dual*other._real};
        }

        /**
         * @brief Divide by dual number
         *
         * @f[
         *      \frac{\hat a}{\hat b} = \frac{a_0}{b_0} + \epsilon \frac{a_\epsilon b_0 - a_0 b_\epsilon}{b_0^2}
         * @f]
         */
        template<class U> inline Dual<T> operator/(const Dual<U>& other) const {
            return {_real/other._real, (_dual*other._real - _real*other._dual)/(other._real*other._real)};
        }

        /**
         * @brief Conjugated dual number
         *
         * @f[
         *      \overline{\hat a} = a_0 - \epsilon a_\epsilon
         * @f]
         */
        inline Dual<T> conjugated() const {
            return {_real, -_dual};
        }

    private:
        T _real, _dual;
};

/** @debugoperator{Magnum::Math::Dual} */
template<class T> Corrade::Utility::Debug operator<<(Corrade::Utility::Debug debug, const Dual<T>& value) {
    debug << "Dual(";
    debug.setFlag(Corrade::Utility::Debug::SpaceAfterEachValue, false);
    debug << value.real() << ", " << value.dual() << ")";
    debug.setFlag(Corrade::Utility::Debug::SpaceAfterEachValue, true);
    return debug;
}

}}

#endif
