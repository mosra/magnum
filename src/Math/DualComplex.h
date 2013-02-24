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

namespace Magnum { namespace Math {

/**
@brief %Dual complex number
@tparam T   Underlying data type

Represents 2D rotation and translation.
@see Dual, Complex, Matrix3
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
            return Vector2<T>(this->dual()*this->real().conjugated());
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
         *      |\hat c|^2 = \sqrt{\hat c^* \hat c}^2 = c_0 \cdot c_0 + \epsilon 2 (c_0 \cdot c_\epsilon)
         * @f]
         */
        inline Dual<T> lengthSquared() const {
            return {this->real().dot(), T(2)*Complex<T>::dot(this->real(), this->dual())};
        }

        /**
         * @brief %Dual quaternion length
         *
         * See lengthSquared() which is faster for comparing length with other
         * values. @f[
         *      |\hat c| = \sqrt{\hat{c^*} \hat c} = |c_0| + \epsilon \frac{c_0 \cdot c_\epsilon}{|c_0|}
         * @f]
         */
        inline Dual<T> length() const {
            return Math::sqrt(lengthSquared());
        }

        /** @brief Normalized dual complex number (of unit length) */
        inline DualComplex<T> normalized() const {
            return (*this)/length();
        }

        /**
         * @brief Inverted dual complex number
         *
         * See invertedNormalized() which is faster for normalized dual complex
         * numbers. @f[
         *      \hat c^{-1} = \frac{\hat c^*}{|\hat c|^2}
         * @f]
         */
        inline DualComplex<T> inverted() const {
            return complexConjugated()/lengthSquared();
        }

        /**
         * @brief Inverted normalized dual complex number
         *
         * Equivalent to complexConjugated(). Expects that the complex number
         * is normalized. @f[
         *      \hat c^{-1} = \frac{\hat c^*}{|\hat c|^2} = \hat c^*
         * @f]
         * @see inverted()
         */
        inline DualComplex<T> invertedNormalized() const {
            CORRADE_ASSERT(lengthSquared() == Dual<T>(1),
                           "Math::DualComplex::invertedNormalized(): dual complex number must be normalized", {});
            return complexConjugated();
        }

        MAGNUM_DUAL_SUBCLASS_IMPLEMENTATION(DualComplex, Complex)

    private:
        /* Used by Dual operators and dualConjugated() */
        inline constexpr DualComplex(const Dual<Complex<T>>& other): Dual<Complex<T>>(other) {}
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
