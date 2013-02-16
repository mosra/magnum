#ifndef Magnum_Math_DualQuaternion_h
#define Magnum_Math_DualQuaternion_h
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
 * @brief Class Magnum::Math::DualQuaternion
 */

#include "Math/Dual.h"
#include "Math/Quaternion.h"

namespace Magnum { namespace Math {

/**
@brief %Dual quaternion

@see Dual, Quaternion
*/
template<class T> class DualQuaternion: public Dual<Quaternion<T>> {
    public:
        /**
         * @brief Rotation dual quaternion
         * @param angle             Rotation angle (counterclockwise, in radians)
         * @param normalizedAxis    Normalized rotation axis
         *
         * Expects that the rotation axis is normalized. @f[
         *      \hat q = [\boldsymbol a \cdot sin \frac \theta 2, cos \frac \theta 2] + \epsilon [\boldsymbol 0, 0]
         * @f]
         * @see rotationAngle(), rotationAxis(), Quaternion::rotation(),
         *      Matrix4::rotation(), Vector3::xAxis(), Vector3::yAxis(),
         *      Vector3::zAxis(), deg(), rad()
         */
        inline static DualQuaternion<T> rotation(T angle, const Vector3<T>& normalizedAxis) {
            return {Quaternion<T>::rotation(angle, normalizedAxis), {{}, T(0)}};
        }

        /**
         * @brief Translation dual quaternion
         * @param vector    Translation vector
         *
         * @f[
         *      \hat q = [\boldsymbol 0, 1] + \epsilon [\frac{\boldsymbol v}{2}, 0]
         * @f]
         * @see translation() const, Matrix3::translation(const Vector2&),
         *      Vector3::xAxis(), Vector3::yAxis(), Vector3::zAxis()
         */
        inline static DualQuaternion<T> translation(const Vector3<T>& vector) {
            return {{}, {vector/T(2), T(0)}};
        }

        /**
         * @brief Default constructor
         *
         * All components set to zero except real scalar part, which is `1`.
         * @todoc Remove workaround when Doxygen is predictable
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        inline constexpr /*implicit*/ DualQuaternion();
        #else
        inline constexpr /*implicit*/ DualQuaternion(): Dual<Quaternion<T>>({}, {{}, T(0)}) {}
        #endif

        /**
         * @brief Construct dual quaternion from real and dual part
         *
         * @f[
         *      \hat q = q_0 + \epsilon q_\epsilon
         * @f]
         */
        inline constexpr /*implicit*/ DualQuaternion(const Quaternion<T>& real, const Quaternion<T>& dual): Dual<Quaternion<T>>(real, dual) {}

        /**
         * @brief Rotation angle of unit dual quaternion
         *
         * Expects that the real part of the quaternion is normalized. @f[
         *      \theta = 2 \cdot acos q_{S 0}
         * @f]
         * @see rotationAxis(), rotation(), Quaternion::rotationAngle()
         */
        inline T rotationAngle() const {
            return this->real().rotationAngle();
        }

        /**
         * @brief Rotation axis of unit dual quaternion
         *
         * Expects that the quaternion is normalized. Returns either unit-length
         * vector for valid rotation quaternion or NaN vector for
         * default-constructed quaternion. @f[
         *      \boldsymbol a = \frac{\boldsymbol q_{V 0}}{\sqrt{1 - q_{S 0}^2}}
         * @f]
         * @see rotationAngle(), rotation(), Quaternion::rotationAxis()
         */
        inline Vector3<T> rotationAxis() const {
            return this->real().rotationAxis();
        }

        /**
         * @brief Translation part of unit dual quaternion
         *
         * @f[
         *      \boldsymbol a = 2 (q_\epsilon q_0^*)_V
         * @f]
         * @see translation(const Vector3&)
         */
        inline Vector3<T> translation() const {
            return (this->dual()*this->real().conjugated()).vector()*T(2);
        }

        /**
         * @brief Quaternion-conjugated dual quaternion
         *
         * @f[
         *      \hat q^* = q_0^* + q_\epsilon^*
         * @f]
         * @see dualConjugated(), conjugated(), Quaternion::conjugated()
         */
        inline DualQuaternion<T> quaternionConjugated() const {
            return {this->real().conjugated(), this->dual().conjugated()};
        }

        /**
         * @brief Dual-conjugated dual quaternion
         *
         * @f[
         *      \overline{\hat q} = q_0 - \epsilon q_\epsilon
         * @f]
         * @see quaternionConjugated(), conjugated(), Dual::conjugated()
         */
        inline DualQuaternion<T> dualConjugated() const {
            return Dual<Quaternion<T>>::conjugated();
        }

        /**
         * @brief Conjugated dual quaternion
         *
         * Both quaternion and dual conjugation. @f[
         *      \overline{\hat q^*} = q_0^* - \epsilon q_\epsilon^* = q_0^* + \epsilon [\boldsymbol q_{\epsilon V}, -q_S]
         * @f]
         * @see quaternionConjugated(), dualConjugated(), Quaternion::conjugated(),
         *      Dual::conjugated()
         */
        inline DualQuaternion<T> conjugated() const {
            return {this->real().conjugated(), {this->dual().vector(), -this->dual().scalar()}};
        }

        /**
         * @brief %Dual quaternion norm
         *
         * @f[
         *      ||\hat q|| = \sqrt{\hat q^* \hat q} = ||q_0|| + \epsilon \frac{q_0 \cdot q_\epsilon}{||q_0||}
         * @f]
         */
        inline Dual<T> norm() const {
            T norm = this->real().length();
            return {norm, Quaternion<T>::dot(this->real(), this->dual())/norm};
        }

        /**
         * @brief Inverted dual quaternion
         *
         * See invertedNormalized() which is faster for normalized
         * dual quaternions. @f[
         *      \hat q^{-1} = \frac{\hat q^*}{||\hat q||^2}
         * @f]
         */
        inline DualQuaternion<T> inverted() const {
            return quaternionConjugated()/Math::pow<2>(norm());
        }

        /**
         * @brief Inverted normalized dual quaternion
         *
         * Equivalent to quaternionConjugated(). Expects that the quaternion is
         * normalized. @f[
         *      \hat q^{-1} = \frac{\hat q^*}{||\hat q||^2} = \hat q^*
         * @f]
         */
        inline DualQuaternion<T> invertedNormalized() const {
            CORRADE_ASSERT(MathTypeTraits<T>::equals(norm(), T(1)),
                           "Math::DualQuaternion::invertedNormalized(): dual quaternion must be normalized", {});
            return quaternionConjugated();
        }

        MAGNUM_DUAL_SUBCLASS_IMPLEMENTATION(DualQuaternion, Quaternion)

    private:
        /* Used by Dual operators and dualConjugated() */
        DualQuaternion<T>(const Dual<Quaternion<T>>& other): Dual<Quaternion<T>>(other) {}
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
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const DualQuaternion<float>&);
#ifndef MAGNUM_TARGET_GLES
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const DualQuaternion<double>&);
#endif
#endif

}}

#endif
