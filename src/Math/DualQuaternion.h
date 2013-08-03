#ifndef Magnum_Math_DualQuaternion_h
#define Magnum_Math_DualQuaternion_h
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
 * @brief Class Magnum::Math::DualQuaternion
 */

#include "Math/Dual.h"
#include "Math/Matrix4.h"
#include "Math/Quaternion.h"

namespace Magnum { namespace Math {

/**
@brief %Dual quaternion
@tparam T   Underlying data type

Represents 3D rotation and translation. See @ref transformations for brief
introduction.
@see Magnum::DualQuaternion, Magnum::DualQuaterniond, Dual, Quaternion, Matrix4
*/
template<class T> class DualQuaternion: public Dual<Quaternion<T>> {
    public:
        typedef T Type;                         /**< @brief Underlying data type */

        /**
         * @brief Rotation dual quaternion
         * @param angle             Rotation angle (counterclockwise)
         * @param normalizedAxis    Normalized rotation axis
         *
         * Expects that the rotation axis is normalized. @f[
         *      \hat q = [\boldsymbol a \cdot sin \frac \theta 2, cos \frac \theta 2] + \epsilon [\boldsymbol 0, 0]
         * @f]
         * @see rotation() const, Quaternion::rotation(), Matrix4::rotation(),
         *      DualComplex::rotation(), Vector3::xAxis(), Vector3::yAxis(),
         *      Vector3::zAxis(), Vector::isNormalized()
         */
        static DualQuaternion<T> rotation(Rad<T> angle, const Vector3<T>& normalizedAxis) {
            return {Quaternion<T>::rotation(angle, normalizedAxis), {{}, T(0)}};
        }

        /** @todo Rotation about axis with arbitrary origin, screw motion */

        /**
         * @brief Translation dual quaternion
         * @param vector    Translation vector
         *
         * @f[
         *      \hat q = [\boldsymbol 0, 1] + \epsilon [\frac{\boldsymbol v}{2}, 0]
         * @f]
         * @see translation() const, Matrix4::translation(const Vector3&),
         *      DualComplex::translation(), Vector3::xAxis(), Vector3::yAxis(),
         *      Vector3::zAxis()
         */
        static DualQuaternion<T> translation(const Vector3<T>& vector) {
            return {{}, {vector/T(2), T(0)}};
        }

        /**
         * @brief Create dual quaternion from transformation matrix
         *
         * Expects that the matrix represents rigid transformation.
         * @see toMatrix(), Quaternion::fromMatrix(),
         *      Matrix4::isRigidTransformation()
         */
        static DualQuaternion<T> fromMatrix(const Matrix4<T>& matrix) {
            CORRADE_ASSERT(matrix.isRigidTransformation(),
                "Math::DualQuaternion::fromMatrix(): the matrix doesn't represent rigid transformation", {});

            Quaternion<T> q = Implementation::quaternionFromMatrix(matrix.rotationScaling());
            return {q, Quaternion<T>(matrix.translation()/2)*q};
        }

        /**
         * @brief Default constructor
         *
         * Creates unit dual quaternion. @f[
         *      \hat q = [\boldsymbol 0, 1] + \epsilon [\boldsymbol 0, 0]
         * @f]
         * @todoc Remove workaround when Doxygen is predictable
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        constexpr /*implicit*/ DualQuaternion();
        #else
        constexpr /*implicit*/ DualQuaternion(): Dual<Quaternion<T>>({}, {{}, T(0)}) {}
        #endif

        /**
         * @brief Construct dual quaternion from real and dual part
         *
         * @f[
         *      \hat q = q_0 + \epsilon q_\epsilon
         * @f]
         */
        constexpr /*implicit*/ DualQuaternion(const Quaternion<T>& real, const Quaternion<T>& dual = Quaternion<T>({}, T(0))): Dual<Quaternion<T>>(real, dual) {}

        /**
         * @brief Construct dual quaternion from vector
         *
         * To be used in transformations later. @f[
         *      \hat q = [\boldsymbol 0, 1] + \epsilon [\boldsymbol v, 0]
         * @f]
         * @see transformPointNormalized()
         * @todoc Remove workaround when Doxygen is predictable
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        constexpr explicit DualQuaternion(const Vector3<T>& vector);
        #else
        constexpr explicit DualQuaternion(const Vector3<T>& vector): Dual<Quaternion<T>>({}, {vector, T(0)}) {}
        #endif

        /** @brief Copy constructor */
        constexpr DualQuaternion(const Dual<Quaternion<T>>& other): Dual<Quaternion<T>>(other) {}

        /**
         * @brief Whether the dual quaternion is normalized
         *
         * Dual quaternion is normalized if it has unit length: @f[
         *      |\hat q|^2 = |\hat q| = 1 + \epsilon 0
         * @f]
         * @see lengthSquared(), normalized()
         * @todoc Improve the equation as in Quaternion::isNormalized()
         */
        bool isNormalized() const {
            /* Comparing dual part classically, as comparing sqrt() of it would
               lead to overly strict precision */
            Dual<T> a = lengthSquared();
            return Implementation::isNormalizedSquared(a.real()) &&
                   TypeTraits<T>::equals(a.dual(), T(0));
        }

        /**
         * @brief Rotation part of unit dual quaternion
         *
         * @see Quaternion::angle(), Quaternion::axis()
         */
        constexpr Quaternion<T> rotation() const {
            return Dual<Quaternion<T>>::real();
        }

        /**
         * @brief Translation part of unit dual quaternion
         *
         * @f[
         *      \boldsymbol a = 2 (q_\epsilon q_0^*)_V
         * @f]
         * @see translation(const Vector3&)
         */
        Vector3<T> translation() const {
            return (Dual<Quaternion<T>>::dual()*Dual<Quaternion<T>>::real().conjugated()).vector()*T(2);
        }

        /**
         * @brief Convert dual quaternion to transformation matrix
         *
         * @see fromMatrix(), Quaternion::toMatrix()
         */
        Matrix4<T> toMatrix() const {
            return Matrix4<T>::from(Dual<Quaternion<T>>::real().toMatrix(), translation());
        }

        /**
         * @brief Quaternion-conjugated dual quaternion
         *
         * @f[
         *      \hat q^* = q_0^* + q_\epsilon^*
         * @f]
         * @see dualConjugated(), conjugated(), Quaternion::conjugated()
         */
        DualQuaternion<T> quaternionConjugated() const {
            return {Dual<Quaternion<T>>::real().conjugated(), Dual<Quaternion<T>>::dual().conjugated()};
        }

        /**
         * @brief Dual-conjugated dual quaternion
         *
         * @f[
         *      \overline{\hat q} = q_0 - \epsilon q_\epsilon
         * @f]
         * @see quaternionConjugated(), conjugated(), Dual::conjugated()
         */
        DualQuaternion<T> dualConjugated() const {
            return Dual<Quaternion<T>>::conjugated();
        }

        /**
         * @brief Conjugated dual quaternion
         *
         * Both quaternion and dual conjugation. @f[
         *      \overline{\hat q^*} = q_0^* - \epsilon q_\epsilon^* = q_0^* + \epsilon [\boldsymbol q_{V \epsilon}, -q_{S \epsilon}]
         * @f]
         * @see quaternionConjugated(), dualConjugated(), Quaternion::conjugated(),
         *      Dual::conjugated()
         */
        DualQuaternion<T> conjugated() const {
            return {Dual<Quaternion<T>>::real().conjugated(), {Dual<Quaternion<T>>::dual().vector(), -Dual<Quaternion<T>>::dual().scalar()}};
        }

        /**
         * @brief %Dual quaternion length squared
         *
         * Should be used instead of length() for comparing dual quaternion
         * length with other values, because it doesn't compute the square root. @f[
         *      |\hat q|^2 = \sqrt{\hat q^* \hat q}^2 = q_0 \cdot q_0 + \epsilon 2 (q_0 \cdot q_\epsilon)
         * @f]
         */
        Dual<T> lengthSquared() const {
            return {Dual<Quaternion<T>>::real().dot(), T(2)*Quaternion<T>::dot(Dual<Quaternion<T>>::real(), Dual<Quaternion<T>>::dual())};
        }

        /**
         * @brief %Dual quaternion length
         *
         * See lengthSquared() which is faster for comparing length with other
         * values. @f[
         *      |\hat q| = \sqrt{\hat q^* \hat q} = |q_0| + \epsilon \frac{q_0 \cdot q_\epsilon}{|q_0|}
         * @f]
         */
        Dual<T> length() const {
            return Math::sqrt(lengthSquared());
        }

        /**
         * @brief Normalized dual quaternion (of unit length)
         *
         * @see isNormalized()
         */
        DualQuaternion<T> normalized() const {
            return (*this)/length();
        }

        /**
         * @brief Inverted dual quaternion
         *
         * See invertedNormalized() which is faster for normalized dual
         * quaternions. @f[
         *      \hat q^{-1} = \frac{\hat q^*}{|\hat q|^2}
         * @f]
         */
        DualQuaternion<T> inverted() const {
            return quaternionConjugated()/lengthSquared();
        }

        /**
         * @brief Inverted normalized dual quaternion
         *
         * Equivalent to quaternionConjugated(). Expects that the quaternion is
         * normalized. @f[
         *      \hat q^{-1} = \frac{\hat q^*}{|\hat q|^2} = \hat q^*
         * @f]
         * @see isNormalized(), inverted()
         */
        DualQuaternion<T> invertedNormalized() const {
            CORRADE_ASSERT(isNormalized(),
                           "Math::DualQuaternion::invertedNormalized(): dual quaternion must be normalized", {});
            return quaternionConjugated();
        }

        /**
         * @brief Rotate and translate point with dual quaternion
         *
         * See transformPointNormalized(), which is faster for normalized dual
         * quaternions. @f[
         *      v' = \hat q v \overline{\hat q^{-1}} = \hat q ([\boldsymbol 0, 1] + \epsilon [\boldsymbol v, 0]) \overline{\hat q^{-1}}
         * @f]
         * @see DualQuaternion(const Vector3&), dual(), Matrix4::transformPoint(),
         *      Quaternion::transformVector(), DualComplex::transformPoint()
         */
        Vector3<T> transformPoint(const Vector3<T>& vector) const {
            return ((*this)*DualQuaternion<T>(vector)*inverted().dualConjugated()).dual().vector();
        }

        /**
         * @brief Rotate and translate point with normalized dual quaternion
         *
         * Faster alternative to transformPoint(), expects that the dual
         * quaternion is normalized. @f[
         *      v' = \hat q v \overline{\hat q^{-1}} = \hat q v \overline{\hat q^*} = \hat q ([\boldsymbol 0, 1] + \epsilon [\boldsymbol v, 0]) \overline{\hat q^*}
         * @f]
         * @see isNormalized(), DualQuaternion(const Vector3&), dual(),
         *      Matrix4::transformPoint(), Quaternion::transformVectorNormalized(),
         *      DualComplex::transformPointNormalized()
         */
        Vector3<T> transformPointNormalized(const Vector3<T>& vector) const {
            CORRADE_ASSERT(isNormalized(),
                           "Math::DualQuaternion::transformPointNormalized(): dual quaternion must be normalized",
                           Vector3<T>(std::numeric_limits<T>::quiet_NaN()));
            return ((*this)*DualQuaternion<T>(vector)*conjugated()).dual().vector();
        }

        MAGNUM_DUAL_SUBCLASS_IMPLEMENTATION(DualQuaternion, Quaternion)
};

/** @debugoperator{Magnum::Math::DualQuaternion} */
template<class T> Corrade::Utility::Debug operator<<(Corrade::Utility::Debug debug, const DualQuaternion<T>& value) {
    debug << "DualQuaternion({{";
    debug.setFlag(Corrade::Utility::Debug::SpaceAfterEachValue, false);
    debug << value.real().vector().x() << ", " << value.real().vector().y() << ", " << value.real().vector().z()
          << "}, " << value.real().scalar() << "}, {{"
          << value.dual().vector().x() << ", " << value.dual().vector().y() << ", " << value.dual().vector().z()
          << "}, " << value.dual().scalar() << "})";
    debug.setFlag(Corrade::Utility::Debug::SpaceAfterEachValue, true);
    return debug;
}

/* Explicit instantiation for commonly used types */
#ifndef DOXYGEN_GENERATING_OUTPUT
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const DualQuaternion<Float>&);
#ifndef MAGNUM_TARGET_GLES
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const DualQuaternion<Double>&);
#endif
#endif

}}

#endif
