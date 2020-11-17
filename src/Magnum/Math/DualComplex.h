#ifndef Magnum_Math_DualComplex_h
#define Magnum_Math_DualComplex_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2020 Jonathan Hale <squareys@googlemail.com>

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
 * @brief Class @ref Magnum::Math::DualComplex
 */

#include "Magnum/Math/Complex.h"
#include "Magnum/Math/Dual.h"
#include "Magnum/Math/Matrix3.h"

namespace Magnum { namespace Math {

namespace Implementation {
    template<class, class> struct DualComplexConverter;
}

/**
@brief Dual complex number
@tparam T   Underlying data type

Represents 2D rotation and translation. Usually denoted as the following in
equations, with @f$ q_0 @f$ being the @ref real() part and @f$ q_\epsilon @f$
the @ref dual() part: @f[
    \hat q = q_0 + \epsilon q_\epsilon
@f]

See @ref Dual and @ref Complex for further notation description and
@ref transformations for brief introduction.
@see @ref Magnum::DualComplex, @ref Magnum::DualComplexd, @ref Dual,
    @ref Complex, @ref Matrix3
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
         *      \hat c = (\cos(\theta) + i \sin(\theta)) + \epsilon (0 + i0)
         * @f]
         *
         * For creating a dual complex number from a rotation @ref Complex, use
         * the implicit conversion provided by
         * @ref DualComplex(const Complex<T>&, const Complex<T>&).
         * @see @ref Complex::rotation(), @ref Matrix3::rotation(),
         *      @ref DualQuaternion::rotation()
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
         * @see @ref translation() const, @ref Matrix3::translation(const Vector2<T>&),
         *      @ref DualQuaternion::translation(), @ref Vector2::xAxis(),
         *      @ref Vector2::yAxis()
         */
        static DualComplex<T> translation(const Vector2<T>& vector) {
            return {{}, {vector.x(), vector.y()}};
        }

        /**
         * @brief Create dual complex number from rotation matrix
         *
         * Expects that the matrix represents rigid transformation.
         * @see @ref toMatrix(), @ref Complex::fromMatrix(),
         *      @ref Matrix3::isRigidTransformation()
         */
        static DualComplex<T> fromMatrix(const Matrix3<T>& matrix) {
            CORRADE_ASSERT(matrix.isRigidTransformation(),
                "Math::DualComplex::fromMatrix(): the matrix doesn't represent rigid transformation:" << Corrade::Utility::Debug::newline << matrix, {});
            return {Implementation::complexFromMatrix(matrix.rotationScaling()), Complex<T>(matrix.translation())};
        }

        /**
         * @brief Create dual complext from rotation complex and translation vector
         * @m_since_latest
         *
         * @f[
         *      \hat c = r + \epsilon (v_x + iv_y)
         * @f]
         *
         * @see @ref translation(), @ref rotation()
         *      @ref Matrix3::from(const Matrix2x2<T>&, const Vector2<T>&),
         *      @ref Matrix4::from(const Matrix3x3<T>&, const Vector3<T>&),
         *      @ref DualQuaternion::from(const Quaternion<T>&, const Vector3<T>&)
         */
        static DualComplex<T> from(const Complex<T>& rotation, const Vector2<T>& translation) {
            return {rotation, Complex<T>{translation}};
        }

        /**
         * @brief Default constructor
         *
         * Equivalent to @ref DualComplex(IdentityInitT).
         */
        constexpr /*implicit*/ DualComplex() noexcept: Dual<Complex<T>>({}, {T(0), T(0)}) {}

        /**
         * @brief Identity constructor
         *
         * Creates unit dual complex number. @f[
         *      \hat c = (0 + i1) + \epsilon (0 + i0)
         * @f]
         */
        constexpr explicit DualComplex(IdentityInitT) noexcept: Dual<Complex<T>>({}, {T(0), T(0)}) {}

        /** @brief Construct zero-initialized dual complex number */
        constexpr explicit DualComplex(ZeroInitT) noexcept: Dual<Complex<T>>{Complex<T>{ZeroInit}, Complex<T>{ZeroInit}} {}

        /** @brief Construct without initializing the contents */
        explicit DualComplex(Magnum::NoInitT) noexcept: Dual<Complex<T>>{Magnum::NoInit} {}

        /**
         * @brief Construct dual complex number from real and dual part
         *
         * @f[
         *      \hat c = c_0 + \epsilon c_\epsilon
         * @f]
         *
         * This constructor can be also used to implicitly convert a rotation
         * complex number to a rotation dual complex number.
         */
        constexpr /*implicit*/ DualComplex(const Complex<T>& real, const Complex<T>& dual = Complex<T>(T(0), T(0))) noexcept: Dual<Complex<T>>(real, dual) {}

        /* No constructor from a pair of Dual values because that would be
           ambiguous with the above */

        /**
         * @brief Construct dual complex number from vector
         *
         * To be used in transformations later. @f[
         *      \hat c = (0 + i1) + \epsilon(v_x + iv_y)
         * @f]
         */
        constexpr explicit DualComplex(const Vector2<T>& vector) noexcept: Dual<Complex<T>>({}, Complex<T>(vector)) {}

        /**
         * @brief Construct dual complex number from another of different type
         *
         * Performs only default casting on the values, no rounding or anything
         * else.
         */
        template<class U> constexpr explicit DualComplex(const DualComplex<U>& other) noexcept: Dual<Complex<T>>{other} {}

        /** @brief Construct dual complex number from external representation */
        template<class U, class V = decltype(Implementation::DualComplexConverter<T, U>::from(std::declval<U>()))> constexpr explicit DualComplex(const U& other): DualComplex{Implementation::DualComplexConverter<T, U>::from(other)} {}

        /** @brief Copy constructor */
        constexpr /*implicit*/ DualComplex(const Dual<Complex<T>>& other) noexcept: Dual<Complex<T>>(other) {}

        /** @brief Convert dual complex number to external representation */
        template<class U, class V = decltype(Implementation::DualComplexConverter<T, U>::to(std::declval<DualComplex<T>>()))> constexpr explicit operator U() const {
            return Implementation::DualComplexConverter<T, U>::to(*this);
        }

        /**
         * @brief Raw data
         * @return One-dimensional array of four elements
         *
         * @see @ref real(), @ref dual()
         */
        T* data() { return Dual<Complex<T>>::data()->data(); }
        constexpr const T* data() const { return Dual<Complex<T>>::data()->data(); } /**< @overload */

        /**
         * @brief Whether the dual complex number is normalized
         *
         * Dual complex number is normalized if its real part has unit length: @f[
         *      |c_0|^2 = |c_0| = 1
         * @f]
         * @see @ref Complex::dot(), @ref normalized()
         * @todoc Improve the equation as in Complex::isNormalized()
         */
        bool isNormalized() const {
            return Implementation::isNormalizedSquared(lengthSquared());
        }

        /**
         * @brief Rotation part of dual complex number
         *
         * @see @ref Complex::angle()
         */
        constexpr Complex<T> rotation() const {
            return Dual<Complex<T>>::real();
        }

        /**
         * @brief Translation part of dual complex number
         *
         * @f[
         *      \boldsymbol a = (c_\epsilon c_0^*)
         * @f]
         * @see @ref translation(const Vector2<T>&)
         */
        Vector2<T> translation() const {
            return Vector2<T>(Dual<Complex<T>>::dual());
        }

        /**
         * @brief Convert dual complex number to transformation matrix
         *
         * @see @ref fromMatrix(), @ref Complex::toMatrix()
         */
        Matrix3<T> toMatrix() const {
            return Matrix3<T>::from(Dual<Complex<T>>::real().toMatrix(), translation());
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
            return {Dual<Complex<T>>::real()*other.real(), Dual<Complex<T>>::real()*other.dual() + Dual<Complex<T>>::dual()};
        }

        /**
         * @brief Complex-conjugated dual complex number
         *
         * @f[
         *      \hat c^* = c^*_0 + c^*_\epsilon
         * @f]
         * @see @ref dualConjugated(), @ref conjugated(),
         *      @ref Complex::conjugated()
         */
        DualComplex<T> complexConjugated() const {
            return {Dual<Complex<T>>::real().conjugated(), Dual<Complex<T>>::dual().conjugated()};
        }

        /**
         * @brief Dual-conjugated dual complex number
         *
         * @f[
         *      \overline{\hat c} = c_0 - \epsilon c_\epsilon
         * @f]
         * @see @ref complexConjugated(), @ref conjugated(),
         *      @ref Dual::conjugated()
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
         * @see @ref complexConjugated(), @ref dualConjugated(),
         *      @ref Complex::conjugated(), @ref Dual::conjugated()
         */
        DualComplex<T> conjugated() const {
            return {Dual<Complex<T>>::real().conjugated(), {-Dual<Complex<T>>::dual().real(), Dual<Complex<T>>::dual().imaginary()}};
        }

        /**
         * @brief Complex number length squared
         *
         * Should be used instead of @ref length() for comparing complex number
         * length with other values, because it doesn't compute the square root. @f[
         *      |\hat c|^2 = c_0 \cdot c_0 = |c_0|^2
         * @f]
         * @todo Can this be done similarly to dual quaternins?
         */
        T lengthSquared() const {
            return Dual<Complex<T>>::real().dot();
        }

        /**
         * @brief Dual quaternion length
         *
         * See @ref lengthSquared() which is faster for comparing length with
         * other values. @f[
         *      |\hat c| = \sqrt{c_0 \cdot c_0} = |c_0|
         * @f]
         * @todo can this be done similarly to dual quaternions?
         */
        T length() const {
            return Dual<Complex<T>>::real().length();
        }

        /**
         * @brief Normalized dual complex number (of unit length)
         *
         * @f[
         *      c' = \frac{c_0}{|c_0|}
         * @f]
         * @see @ref isNormalized()
         * @todo can this be done similarly to dual quaternions?
         */
        DualComplex<T> normalized() const {
            return {Dual<Complex<T>>::real()/length(), Dual<Complex<T>>::dual()};
        }

        /**
         * @brief Inverted dual complex number
         *
         * See @ref invertedNormalized() which is faster for normalized dual
         * complex numbers. @f[
         *      \hat c^{-1} = c_0^{-1} - \epsilon c_\epsilon
         * @f]
         * @todo can this be done similarly to dual quaternions?
         */
        DualComplex<T> inverted() const {
            return DualComplex<T>(Dual<Complex<T>>::real().inverted(), {{}, {}})*DualComplex<T>({}, -Dual<Complex<T>>::dual());
        }

        /**
         * @brief Inverted normalized dual complex number
         *
         * Expects that the complex number is normalized. @f[
         *      \hat c^{-1} = c_0^{-1} - \epsilon c_\epsilon = c_0^* - \epsilon c_\epsilon
         * @f]
         * @see @ref isNormalized(), @ref inverted()
         * @todo can this be done similarly to dual quaternions?
         */
        DualComplex<T> invertedNormalized() const {
            return DualComplex<T>(Dual<Complex<T>>::real().invertedNormalized(), {{}, {}})*DualComplex<T>({}, -Dual<Complex<T>>::dual());
        }

        /**
         * @brief Rotate a vector with a dual complex number
         * @m_since{2020,06}
         *
         * Calls @ref Complex::transformVector() on the @ref real() part,
         * see its documentation for more information.
         */
        Vector2<T> transformVector(const Vector2<T>& vector) const {
            return Dual<Complex<T>>::real().transformVector(vector);
        }

        /**
         * @brief Rotate and translate point with dual complex number
         *
         * @f[
         *      v' = \hat c v = \hat c ((0 + i) + \epsilon(v_x + iv_y))
         * @f]
         * @see @ref DualComplex(const Vector2<T>&), @ref dual(),
         *      @ref Matrix3::transformPoint(),
         *      @ref DualQuaternion::transformPoint()
         */
        Vector2<T> transformPoint(const Vector2<T>& vector) const {
            return Vector2<T>(((*this)*DualComplex<T>(vector)).dual());
        }

        MAGNUM_DUAL_SUBCLASS_IMPLEMENTATION(DualComplex, Vector2, T)
        /* Not using MAGNUM_DUAL_SUBCLASS_MULTIPLICATION_IMPLEMENTATION(), as
           we have special multiplication/division implementation */
};

MAGNUM_DUAL_OPERATOR_IMPLEMENTATION(DualComplex, Vector2, T)

#ifndef CORRADE_NO_DEBUG
/** @debugoperator{DualComplex} */
template<class T> Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug& debug, const DualComplex<T>& value) {
    return debug << "DualComplex({" << Corrade::Utility::Debug::nospace
          << value.real().real() << Corrade::Utility::Debug::nospace << ","
          << value.real().imaginary() << Corrade::Utility::Debug::nospace << "}, {"
          << Corrade::Utility::Debug::nospace
          << value.dual().real() << Corrade::Utility::Debug::nospace << ","
          << value.dual().imaginary() << Corrade::Utility::Debug::nospace << "})";
}

/* Explicit instantiation for commonly used types */
#ifndef DOXYGEN_GENERATING_OUTPUT
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const DualComplex<Float>&);
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const DualComplex<Double>&);
#endif
#endif

namespace Implementation {
    template<class T> struct StrictWeakOrdering<DualComplex<T>>: StrictWeakOrdering<Dual<Complex<T>>> {};
}

}}

#endif
