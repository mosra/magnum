#ifndef Magnum_Math_DualComplex_h
#define Magnum_Math_DualComplex_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class Magnum::Math::DualComplex
 */

#include "Math/Dual.h"
#include "Math/Complex.h"
#include "Math/Matrix3.h"

namespace Magnum { namespace Math {

/**
@brief %Dual complex number
@tparam T   Underlying data type

Represents 2D rotation and translation. See @ref transformations for brief
introduction.
@see Magnum::DualComplex, Magnum::DualComplexd, Dual, Complex, Matrix3
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
         * @see angle(), Complex::rotation(), Matrix3::rotation(),
         *      DualQuaternion::rotation()
         */
        static DualComplex<T> rotation(Rad<T> angle) {
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
        static DualComplex<T> translation(const Vector2<T>& vector) {
            return {{}, {vector.x(), vector.y()}};
        }

        /**
         * @brief Create dual complex number from rotation matrix
         *
         * Expects that the matrix represents rigid transformation.
         * @see toMatrix(), Complex::fromMatrix(),
         *      Matrix3::isRigidTransformation()
         */
        static DualComplex<T> fromMatrix(const Matrix3<T>& matrix) {
            CORRADE_ASSERT(matrix.isRigidTransformation(),
                "Math::DualComplex::fromMatrix(): the matrix doesn't represent rigid transformation", {});
            return {Implementation::complexFromMatrix(matrix.rotationScaling()), Complex<T>(matrix.translation())};
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
        constexpr /*implicit*/ DualComplex();
        #else
        constexpr /*implicit*/ DualComplex(): Dual<Complex<T>>({}, {T(0), T(0)}) {}
        #endif

        /**
         * @brief Construct dual complex number from real and dual part
         *
         * @f[
         *      \hat c = c_0 + \epsilon c_\epsilon
         * @f]
         */
        constexpr /*implicit*/ DualComplex(const Complex<T>& real, const Complex<T>& dual = Complex<T>(T(0), T(0))): Dual<Complex<T>>(real, dual) {}

        /**
         * @brief Construct dual complex number from vector
         *
         * To be used in transformations later. @f[
         *      \hat c = (0 + i1) + \epsilon(v_x + iv_y)
         * @f]
         * @todoc Remove workaround when Doxygen is predictable
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        constexpr explicit DualComplex(const Vector2<T>& vector);
        #else
        constexpr explicit DualComplex(const Vector2<T>& vector): Dual<Complex<T>>({}, Complex<T>(vector)) {}
        #endif

        /**
         * @brief Whether the dual complex number is normalized
         *
         * Dual complex number is normalized if its real part has unit length: @f[
         *      |c_0|^2 = |c_0| = 1
         * @f]
         * @see Complex::dot(), normalized()
         */
        bool isNormalized() const {
            return Implementation::isNormalizedSquared(lengthSquared());
        }

        /**
         * @brief Rotation part of dual complex number
         *
         * @see Complex::angle()
         */
        constexpr Complex<T> rotation() const {
            return this->real();
        }

        /**
         * @brief Translation part of dual complex number
         *
         * @f[
         *      \boldsymbol a = (c_\epsilon c_0^*)
         * @f]
         * @see translation(const Vector2&)
         */
        Vector2<T> translation() const {
            return Vector2<T>(this->dual());
        }

        /**
         * @brief Convert dual complex number to transformation matrix
         *
         * @see fromMatrix(), Complex::toMatrix()
         */
        Matrix3<T> toMatrix() const {
            return Matrix3<T>::from(this->real().toMatrix(), translation());
        }

        /**
         * @brief Multipy with dual complex number
         *
         * @f[
         *      \hat a \hat b = a_0 b_0 + \epsilon (a_0 b_\epsilon + a_\epsilon)
         * @f]
         * @todo can this be done similarly to dual quaternions?
         */
        DualComplex<T> operator*(const DualComplex<T>& other) const {
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
        DualComplex<T> complexConjugated() const {
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
        DualComplex<T> dualConjugated() const {
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
        DualComplex<T> conjugated() const {
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
        T lengthSquared() const {
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
        T length() const {
            return this->real().length();
        }

        /**
         * @brief Normalized dual complex number (of unit length)
         *
         * @f[
         *      c' = \frac{c_0}{|c_0|}
         * @f]
         * @see isNormalized()
         * @todo can this be done similarly to dual quaternions?
         */
        DualComplex<T> normalized() const {
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
        DualComplex<T> inverted() const {
            return DualComplex<T>(this->real().inverted(), {{}, {}})*DualComplex<T>({}, -this->dual());
        }

        /**
         * @brief Inverted normalized dual complex number
         *
         * Expects that the complex number is normalized. @f[
         *      \hat c^{-1} = c_0^{-1} - \epsilon c_\epsilon = c_0^* - \epsilon c_\epsilon
         * @f]
         * @see isNormalized(), inverted()
         * @todo can this be done similarly to dual quaternions?
         */
        DualComplex<T> invertedNormalized() const {
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
        Vector2<T> transformPoint(const Vector2<T>& vector) const {
            return Vector2<T>(((*this)*DualComplex<T>(vector)).dual());
        }

        /* Verbatim copy of DUAL_SUBCLASS_IMPLEMENTATION(), as we need to hide
           Dual's operator*() and operator/() */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        DualComplex<T> operator-() const {
            return Dual<Complex<T>>::operator-();
        }
        DualComplex<T>& operator+=(const Dual<Complex<T>>& other) {
            Dual<Complex<T>>::operator+=(other);
            return *this;
        }
        DualComplex<T> operator+(const Dual<Complex<T>>& other) const {
            return Dual<Complex<T>>::operator+(other);
        }
        DualComplex<T>& operator-=(const Dual<Complex<T>>& other) {
            Dual<Complex<T>>::operator-=(other);
            return *this;
        }
        DualComplex<T> operator-(const Dual<Complex<T>>& other) const {
            return Dual<Complex<T>>::operator-(other);
        }
        #endif

    private:
        /* Used by Dual operators and dualConjugated() */
        constexpr DualComplex(const Dual<Complex<T>>& other): Dual<Complex<T>>(other) {}

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
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const DualComplex<Float>&);
#ifndef MAGNUM_TARGET_GLES
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const DualComplex<Double>&);
#endif
#endif

}}

#endif
