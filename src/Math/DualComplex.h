#ifndef Magnum_Math_DualComplex_h
#define Magnum_Math_DualComplex_h
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
 * @brief Class Magnum::Math::DualComplex
 */

#include "Math/Dual.h"
#include "Math/Complex.h"
#include "Math/Matrix3.h"

namespace Magnum { namespace Math {

/**
@brief %Dual complex number
@tparam T   Underlying data type

Represents 2D rotation and translation.
@see Magnum::DualComplex, Dual, Complex, Matrix3
@todo Can this be done similarly as in dual quaternions? It sort of works, but
    the math beneath is weird.
*/
template<class T> class DualComplex: public Dual<Complex<T>> {
    public:
        typedef T Type;                         /**< @brief Underlying data type */

        /**
         * @brief Rotation dual complex number
         * @param angle         Rotation angle (counterclockwise)
         *
         * @f[
         *      \hat c = (cos \theta + i sin \theta) + \epsilon (0 + i0)
         * @f]
         * @see rotationAngle(), Complex::rotation(), Matrix3::rotation(),
         *      DualQuaternion::rotation()
         */
        inline static DualComplex<T> rotation(Rad<T> angle) {
            return {Complex<T>::rotation(angle), {{}, {}}};
        }

        /**
         * @brief Translation dual complex number
         * @param vector    Translation vector
         *
         * @f[
         *      \hat c = (0 + i1) + \epsilon (v_x + iv_y)
         * @f]
         * @see translation() const, Matrix3::translation(const Vector2&),
         *      DualQuaternion::translation(), Vector2::xAxis(), Vector2::yAxis()
         */
        inline static DualComplex<T> translation(const Vector2<T>& vector) {
            return {{}, {vector.x(), vector.y()}};
        }

        /**
         * @brief Default constructor
         *
         * Creates unit dual complex number. @f[
         *      \hat c = (0 + i1) + \epsilon (0 + i0)
         * @f]
         * @todoc Remove workaround when Doxygen is predictable
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        inline constexpr /*implicit*/ DualComplex();
        #else
        inline constexpr /*implicit*/ DualComplex(): Dual<Complex<T>>({}, {T(0), T(0)}) {}
        #endif

        /**
         * @brief Construct dual complex number from real and dual part
         *
         * @f[
         *      \hat c = c_0 + \epsilon c_\epsilon
         * @f]
         */
        inline constexpr /*implicit*/ DualComplex(const Complex<T>& real, const Complex<T>& dual): Dual<Complex<T>>(real, dual) {}

        /**
         * @brief Construct dual complex number from vector
         *
         * To be used in transformations later. @f[
         *      \hat c = (0 + i1) + \epsilon(v_x + iv_y)
         * @f]
         * @todoc Remove workaround when Doxygen is predictable
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        inline constexpr explicit DualComplex(const Vector2<T>& vector);
        #else
        inline constexpr explicit DualComplex(const Vector2<T>& vector): Dual<Complex<T>>({}, Complex<T>(vector)) {}
        #endif

        /**
         * @brief Rotation angle of dual complex number
         *
         * @f[
         *      \theta = atan2(b_0, a_0)
         * @f]
         * @see rotation(), Complex::rotationAngle()
         */
        inline Rad<T> rotationAngle() const {
            return this->real().rotationAngle();
        }

        /**
         * @brief Translation part of dual complex number
         *
         * @f[
         *      \boldsymbol a = (c_\epsilon c_0^*)
         * @f]
         * @see translation(const Vector2&)
         */
        inline Vector2<T> translation() const {
            return Vector2<T>(this->dual());
        }

        /**
         * @brief Convert dual complex number to transformation matrix
         *
         * @see Complex::matrix()
         */
        inline Matrix3<T> matrix() const {
            return Matrix3<T>::from(this->real().matrix(), translation());
        }

        /**
         * @brief Multipy with dual complex number
         *
         * @f[
         *      \hat a \hat b = a_0 b_0 + \epsilon (a_0 b_\epsilon + a_\epsilon)
         * @f]
         * @todo can this be done similarly to dual quaternions?
         */
        inline DualComplex<T> operator*(const DualComplex<T>& other) const {
            return {this->real()*other.real(), this->real()*other.dual() + this->dual()};
        }

        /**
         * @brief Complex-conjugated dual complex number
         *
         * @f[
         *      \hat c^* = c^*_0 + c^*_\epsilon
         * @f]
         * @see dualConjugated(), conjugated(), Complex::conjugated()
         */
        inline DualComplex<T> complexConjugated() const {
            return {this->real().conjugated(), this->dual().conjugated()};
        }

        /**
         * @brief Dual-conjugated dual complex number
         *
         * @f[
         *      \overline{\hat c} = c_0 - \epsilon c_\epsilon
         * @f]
         * @see complexConjugated(), conjugated(), Dual::conjugated()
         */
        inline DualComplex<T> dualConjugated() const {
            return Dual<Complex<T>>::conjugated();
        }

        /**
         * @brief Conjugated dual complex number
         *
         * Both complex and dual conjugation. @f[
         *      \overline{\hat c^*} = c^*_0 - \epsilon c^*_\epsilon = c^*_0 + \epsilon(-a_\epsilon + ib_\epsilon)
         * @f]
         * @see complexConjugated(), dualConjugated(), Complex::conjugated(),
         *      Dual::conjugated()
         */
        inline DualComplex<T> conjugated() const {
            return {this->real().conjugated(), {-this->dual().real(), this->dual().imaginary()}};
        }

        /**
         * @brief %Complex number length squared
         *
         * Should be used instead of length() for comparing complex number
         * length with other values, because it doesn't compute the square root. @f[
         *      |\hat c|^2 = c_0 \cdot c_0 = |c_0|^2
         * @f]
         * @todo Can this be done similarly to dual quaternins?
         */
        inline T lengthSquared() const {
            return this->real().dot();
        }

        /**
         * @brief %Dual quaternion length
         *
         * See lengthSquared() which is faster for comparing length with other
         * values. @f[
         *      |\hat c| = \sqrt{c_0 \cdot c_0} = |c_0|
         * @f]
         * @todo can this be done similarly to dual quaternions?
         */
        inline T length() const {
            return this->real().length();
        }

        /**
         * @brief Normalized dual complex number (of unit length)
         *
         * @f[
         *      c' = \frac{c_0}{|c_0|}
         * @f]
         * @todo can this be done similarly to dual quaternions?
         */
        inline DualComplex<T> normalized() const {
            return {this->real()/length(), this->dual()};
        }

        /**
         * @brief Inverted dual complex number
         *
         * See invertedNormalized() which is faster for normalized dual complex
         * numbers. @f[
         *      \hat c^{-1} = c_0^{-1} - \epsilon c_\epsilon
         * @f]
         * @todo can this be done similarly to dual quaternions?
         */
        inline DualComplex<T> inverted() const {
            return DualComplex<T>(this->real().inverted(), {{}, {}})*DualComplex<T>({}, -this->dual());
        }

        /**
         * @brief Inverted normalized dual complex number
         *
         * Expects that the complex number is normalized. @f[
         *      \hat c^{-1} = c_0^{-1} - \epsilon c_\epsilon = c_0^* - \epsilon c_\epsilon
         * @f]
         * @see inverted()
         * @todo can this be done similarly to dual quaternions?
         */
        inline DualComplex<T> invertedNormalized() const {
            return DualComplex<T>(this->real().invertedNormalized(), {{}, {}})*DualComplex<T>({}, -this->dual());
        }

        /**
         * @brief Rotate and translate point with dual complex number
         *
         * See transformPointNormalized(), which is faster for normalized dual
         * complex number. @f[
         *      v' = \hat c v = \hat c ((0 + i) + \epsilon(v_x + iv_y))
         * @f]
         * @see DualComplex(const Vector2&), dual(), Matrix3::transformPoint(),
         *      Complex::transformVector(), DualQuaternion::transformPoint()
         */
        inline Vector2<T> transformPoint(const Vector2<T>& vector) const {
            return Vector2<T>(((*this)*DualComplex<T>(vector)).dual());
        }

        /* Verbatim copy of DUAL_SUBCLASS_IMPLEMENTATION(), as we need to hide
           Dual's operator*() and operator/() */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        inline DualComplex<T> operator-() const {
            return Dual<Complex<T>>::operator-();
        }
        inline DualComplex<T>& operator+=(const Dual<Complex<T>>& other) {
            Dual<Complex<T>>::operator+=(other);
            return *this;
        }
        inline DualComplex<T> operator+(const Dual<Complex<T>>& other) const {
            return Dual<Complex<T>>::operator+(other);
        }
        inline DualComplex<T>& operator-=(const Dual<Complex<T>>& other) {
            Dual<Complex<T>>::operator-=(other);
            return *this;
        }
        inline DualComplex<T> operator-(const Dual<Complex<T>>& other) const {
            return Dual<Complex<T>>::operator-(other);
        }
        #endif

    private:
        /* Used by Dual operators and dualConjugated() */
        inline constexpr DualComplex(const Dual<Complex<T>>& other): Dual<Complex<T>>(other) {}

        /* Just to be sure nobody uses this, as it wouldn't probably work with
           our operator*() */
        using Dual<Complex<T>>::operator*;
        using Dual<Complex<T>>::operator/;
};

/** @debugoperator{Magnum::Math::DualQuaternion} */
template<class T> Corrade::Utility::Debug operator<<(Corrade::Utility::Debug debug, const DualComplex<T>& value) {
    debug << "DualComplex({";
    debug.setFlag(Corrade::Utility::Debug::SpaceAfterEachValue, false);
    debug << value.real().real() << ", " << value.real().imaginary() << "}, {"
          << value.dual().real() << ", " << value.dual().imaginary() << "})";
    debug.setFlag(Corrade::Utility::Debug::SpaceAfterEachValue, true);
    return debug;
}

/* Explicit instantiation for commonly used types */
#ifndef DOXYGEN_GENERATING_OUTPUT
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const DualComplex<float>&);
#ifndef MAGNUM_TARGET_GLES
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const DualComplex<double>&);
#endif
#endif

}}

#endif
