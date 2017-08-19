#ifndef Magnum_Math_Complex_h
#define Magnum_Math_Complex_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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
 * @brief Class @ref Magnum::Math::Complex
 */

#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/Debug.h>

#include "Magnum/Math/Matrix.h"
#include "Magnum/Math/Vector2.h"

namespace Magnum { namespace Math {

namespace Implementation {
    /* No assertions fired, for internal use. Not private member because used
       from outside the class. */
    template<class T> constexpr static Complex<T> complexFromMatrix(const Matrix2x2<T>& matrix) {
        return {matrix[0][0], matrix[0][1]};
    }

    template<class, class> struct ComplexConverter;
}

/** @relatesalso Complex
@brief Dot product of two complex numbers

@f[
    c_0 \cdot c_1 = a_0 a_1 + b_0 b_1
@f]
@see @ref Complex::dot() const
*/
template<class T> inline T dot(const Complex<T>& a, const Complex<T>& b) {
    return a.real()*b.real() + a.imaginary()*b.imaginary();
}

/** @relatesalso Complex
@brief Angle between normalized complex numbers

Expects that both complex numbers are normalized. @f[
    \theta = acos \left( \frac{Re(c_0 \cdot c_1))}{|c_0| |c_1|} \right) = acos (a_0 a_1 + b_0 b_1)
@f]
@see @ref Complex::isNormalized(),
    @ref angle(const Quaternion<T>&, const Quaternion<T>&),
    @ref angle(const Vector<size, T>&, const Vector<size, T>&)
*/
template<class T> inline Rad<T> angle(const Complex<T>& normalizedA, const Complex<T>& normalizedB) {
    CORRADE_ASSERT(normalizedA.isNormalized() && normalizedB.isNormalized(),
                   "Math::angle(): complex numbers must be normalized", {});
    return Rad<T>(std::acos(normalizedA.real()*normalizedB.real() + normalizedA.imaginary()*normalizedB.imaginary()));
}

/**
@brief Complex number
@tparam T   Data type

Represents 2D rotation. See @ref transformations for brief introduction.
@see @ref Magnum::Complex, @ref Magnum::Complexd, @ref Matrix3
*/
template<class T> class Complex {
    template<class> friend class Complex;

    public:
        typedef T Type; /**< @brief Underlying data type */

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @copybrief Math::dot(const Complex<T>&, const Complex<T>&)
         * @deprecated Use @ref Math::dot(const Complex<T>&, const Complex<T>&)
         *      instead.
         */
        CORRADE_DEPRECATED("use Math::dot() instead") static T dot(const Complex<T>& a, const Complex<T>& b) {
            return Math::dot(a, b);
        }

        /**
         * @copybrief Math::angle(const Complex<T>&, const Complex<T>&)
         * @deprecated Use @ref Math::angle(const Complex<T>&, const Complex<T>&)
         *      instead.
         */
        CORRADE_DEPRECATED("use Math::angle() instead") static Rad<T> angle(const Complex<T>& normalizedA, const Complex<T>& normalizedB) {
            return Math::angle(normalizedA, normalizedB);
        }
        #endif

        /**
         * @brief Rotation complex number
         * @param angle             Rotation angle (counterclockwise)
         *
         * @f[
         *      c = cos \theta + i sin \theta
         * @f]
         * @see @ref angle(), @ref Matrix3::rotation(),
         *      @ref Quaternion::rotation()
         */
        static Complex<T> rotation(Rad<T> angle) {
            return {std::cos(T(angle)), std::sin(T(angle))};
        }

        /**
         * @brief Create complex number from rotation matrix
         *
         * Expects that the matrix is orthogonal (i.e. pure rotation).
         * @see @ref toMatrix(), @ref DualComplex::fromMatrix(),
         *      @ref Matrix::isOrthogonal()
         */
        static Complex<T> fromMatrix(const Matrix2x2<T>& matrix) {
            CORRADE_ASSERT(matrix.isOrthogonal(),
                "Math::Complex::fromMatrix(): the matrix is not orthogonal", {});
            return Implementation::complexFromMatrix(matrix);
        }

        /**
         * @brief Default constructor
         *
         * Constructs unit complex number. @f[
         *      c = 1 + i0
         * @f]
         */
        constexpr /*implicit*/ Complex(IdentityInitT = IdentityInit) noexcept: _real(T(1)), _imaginary(T(0)) {}

        /** @brief Construct zero-initialized complex number */
        constexpr explicit Complex(ZeroInitT) noexcept: _real{}, _imaginary{} {}

        /** @brief Construct without initializing the contents */
        explicit Complex(NoInitT) noexcept {}

        /**
         * @brief Construct complex number from real and imaginary part
         *
         * @f[
         *      c = a + ib
         * @f]
         */
        constexpr /*implicit*/ Complex(T real, T imaginary) noexcept: _real(real), _imaginary(imaginary) {}

        /**
         * @brief Construct complex number from vector
         *
         * To be used in transformations later. @f[
         *      c = v_x + iv_y
         * @f]
         * @see @ref operator Vector2<T>(), @ref transformVector()
         */
        constexpr explicit Complex(const Vector2<T>& vector) noexcept: _real(vector.x()), _imaginary(vector.y()) {}

        /**
         * @brief Construct complex number from another of different type
         *
         * Performs only default casting on the values, no rounding or anything
         * else.
         */
        template<class U> constexpr explicit Complex(const Complex<U>& other) noexcept: _real{T(other._real)}, _imaginary{T(other._imaginary)} {}

        /** @brief Construct complex number from external representation */
        template<class U, class V = decltype(Implementation::ComplexConverter<T, U>::from(std::declval<U>()))> constexpr explicit Complex(const U& other): Complex{Implementation::ComplexConverter<T, U>::from(other)} {}

        /** @brief Copy constructor */
        constexpr /*implicit*/ Complex(const Complex<T>&) noexcept = default;

        /** @brief Convert complex number to external representation */
        template<class U, class V = decltype(Implementation::ComplexConverter<T, U>::to(std::declval<Complex<T>>()))> constexpr explicit operator U() const {
            return Implementation::ComplexConverter<T, U>::to(*this);
        }

        /** @brief Equality comparison */
        bool operator==(const Complex<T>& other) const {
            return TypeTraits<T>::equals(_real, other._real) &&
                   TypeTraits<T>::equals(_imaginary, other._imaginary);
        }

        /** @brief Non-equality comparison */
        bool operator!=(const Complex<T>& other) const {
            return !operator==(other);
        }

        /**
         * @brief Whether the complex number is normalized
         *
         * Complex number is normalized if it has unit length: @f[
         *      |c \cdot c - 1| < 2 \epsilon + \epsilon^2 \cong 2 \epsilon
         * @f]
         * @see @ref dot(), @ref normalized()
         */
        bool isNormalized() const {
            return Implementation::isNormalizedSquared(dot());
        }

        /** @brief Real part */
        constexpr T real() const { return _real; }

        /** @brief Imaginary part */
        constexpr T imaginary() const { return _imaginary; }

        /**
         * @brief Convert complex number to vector
         *
         * @f[
         *      \boldsymbol v = \begin{pmatrix} a \\ b \end{pmatrix}
         * @f]
         */
        constexpr explicit operator Vector2<T>() const {
            return {_real, _imaginary};
        }

        /**
         * @brief Rotation angle of complex number
         *
         * @f[
         *      \theta = atan2(b, a)
         * @f]
         * @see @ref rotation()
         */
        Rad<T> angle() const {
            return Rad<T>(std::atan2(_imaginary, _real));
        }

        /**
         * @brief Convert complex number to rotation matrix
         *
         * @f[
         *      M = \begin{pmatrix}
         *               a & -b \\
         *               b &  a
         *          \end{pmatrix}
         * @f]
         * @see @ref fromMatrix(), @ref DualComplex::toMatrix(),
         *      @ref Matrix3::from(const Matrix2x2<T>&, const Vector2<T>&)
         */
        Matrix2x2<T> toMatrix() const {
            return {Vector<2, T>(_real, _imaginary),
                    Vector<2, T>(-_imaginary, _real)};
        }

        /**
         * @brief Add complex number and assign
         *
         * The computation is done in-place. @f[
         *      c_0 + c_1 = (a_0 + a_1) + i(b_0 + b_1)
         * @f]
         */
        Complex<T>& operator+=(const Complex<T>& other) {
            _real += other._real;
            _imaginary += other._imaginary;
            return *this;
        }

        /**
         * @brief Add complex number
         *
         * @see @ref operator+=(const Complex<T>&)
         */
        Complex<T> operator+(const Complex<T>& other) const {
            return Complex<T>(*this) += other;
        }

        /**
         * @brief Negated complex number
         *
         * @f[
         *      -c = -a -ib
         * @f]
         */
        Complex<T> operator-() const {
            return {-_real, -_imaginary};
        }

        /**
         * @brief Subtract complex number and assign
         *
         * The computation is done in-place. @f[
         *      c_0 - c_1 = (a_0 - a_1) + i(b_0 - b_1)
         * @f]
         */
        Complex<T>& operator-=(const Complex<T>& other) {
            _real -= other._real;
            _imaginary -= other._imaginary;
            return *this;
        }

        /**
         * @brief Subtract complex number
         *
         * @see @ref operator-=(const Complex<T>&)
         */
        Complex<T> operator-(const Complex<T>& other) const {
            return Complex<T>(*this) -= other;
        }

        /**
         * @brief Multiply with scalar and assign
         *
         * The computation is done in-place. @f[
         *      c \cdot t = ta + itb
         * @f]
         */
        Complex<T>& operator*=(T scalar) {
            _real *= scalar;
            _imaginary *= scalar;
            return *this;
        }

        /**
         * @brief Multiply with scalar
         *
         * @see @ref operator*=(T)
         */
        Complex<T> operator*(T scalar) const {
            return Complex<T>(*this) *= scalar;
        }

        /**
         * @brief Divide with scalar and assign
         *
         * The computation is done in-place. @f[
         *      \frac c t = \frac a t + i \frac b t
         * @f]
         */
        Complex<T>& operator/=(T scalar) {
            _real /= scalar;
            _imaginary /= scalar;
            return *this;
        }

        /**
         * @brief Divide with scalar
         *
         * @see @ref operator/=(T)
         */
        Complex<T> operator/(T scalar) const {
            return Complex<T>(*this) /= scalar;
        }

        /**
         * @brief Multiply with complex number
         *
         * @f[
         *      c_0 c_1 = (a_0 + ib_0)(a_1 + ib_1) = (a_0 a_1 - b_0 b_1) + i(a_1 b_0 + a_0 b_1)
         * @f]
         */
        Complex<T> operator*(const Complex<T>& other) const {
            return {_real*other._real - _imaginary*other._imaginary,
                    _imaginary*other._real + _real*other._imaginary};
        }

        /**
         * @brief Dot product of the complex number
         *
         * Should be used instead of @ref length() for comparing complex number
         * length with other values, because it doesn't compute the square
         * root. @f[
         *      c \cdot c = a^2 + b^2
         * @f]
         * @see @ref dot(const Complex&, const Complex&), @ref isNormalized()
         */
        T dot() const { return Math::dot(*this, *this); }

        /**
         * @brief Complex number length
         *
         * See also @ref dot() const which is faster for comparing length with
         * other values. @f[
         *      |c| = \sqrt{c \cdot c}
         * @f]
         * @see @ref isNormalized()
         */
        T length() const {
            /** @todo Remove when newlib has this fixed */
            #ifndef CORRADE_TARGET_ANDROID
            return std::hypot(_real, _imaginary);
            #else
            return std::sqrt(dot());
            #endif
        }

        /**
         * @brief Normalized complex number (of unit length)
         *
         * @see @ref isNormalized()
         */
        Complex<T> normalized() const {
            return (*this)/length();
        }

        /**
         * @brief Conjugated complex number
         *
         * @f[
         *      c^* = a - ib
         * @f]
         */
        Complex<T> conjugated() const {
            return {_real, -_imaginary};
        }

        /**
         * @brief Inverted complex number
         *
         * See @ref invertedNormalized() which is faster for normalized
         * complex numbers. @f[
         *      c^{-1} = \frac{c^*}{|c|^2} = \frac{c^*}{c \cdot c}
         * @f]
         */
        Complex<T> inverted() const {
            return conjugated()/dot();
        }

        /**
         * @brief Inverted normalized complex number
         *
         * Equivalent to @ref conjugated(). Expects that the complex number is
         * normalized. @f[
         *      c^{-1} = \frac{c^*}{c \cdot c} = c^*
         * @f]
         * @see @ref isNormalized(), @ref inverted()
         */
        Complex<T> invertedNormalized() const {
            CORRADE_ASSERT(isNormalized(),
                           "Math::Complex::invertedNormalized(): complex number must be normalized", {});
            return conjugated();
        }

        /**
         * @brief Rotate vector with complex number
         *
         * @f[
         *      v' = c v = c (v_x + iv_y)
         * @f]
         * @see @ref Complex(const Vector2<T>&), @ref operator Vector2<T>(),
         *      @ref Matrix3::transformVector()
         */
        Vector2<T> transformVector(const Vector2<T>& vector) const {
            return Vector2<T>((*this)*Complex<T>(vector));
        }

    private:
        T _real, _imaginary;
};

/** @relates Complex
@brief Multiply scalar with complex

Same as @ref Complex::operator*(T) const.
*/
template<class T> inline Complex<T> operator*(T scalar, const Complex<T>& complex) {
    return complex*scalar;
}

/** @relates Complex
@brief Divide complex with number and invert

@f[
    \frac t c = \frac t a + i \frac t b
@f]
@see @ref Complex::operator/()
*/
template<class T> inline Complex<T> operator/(T scalar, const Complex<T>& complex) {
    return {scalar/complex.real(), scalar/complex.imaginary()};
}

/** @debugoperator{Magnum::Math::Complex} */
template<class T> Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug& debug, const Complex<T>& value) {
    return debug << "Complex(" << Corrade::Utility::Debug::nospace
        << value.real() << Corrade::Utility::Debug::nospace << ","
        << value.imaginary() << Corrade::Utility::Debug::nospace << ")";
}

/* Explicit instantiation for commonly used types */
#ifndef DOXYGEN_GENERATING_OUTPUT
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const Complex<Float>&);
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const Complex<Double>&);
#endif

}}

#endif
