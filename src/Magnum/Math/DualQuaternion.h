#ifndef Magnum_Math_DualQuaternion_h
#define Magnum_Math_DualQuaternion_h
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
 * @brief Class @ref Magnum::Math::DualQuaternion, function @ref Magnum::Math::sclerp()
 */

#include <Corrade/Utility/StlMath.h>

#include "Magnum/Math/Dual.h"
#include "Magnum/Math/Functions.h"
#include "Magnum/Math/Matrix4.h"
#include "Magnum/Math/Quaternion.h"

namespace Magnum { namespace Math {

namespace Implementation {
    template<class, class> struct DualQuaternionConverter;
}

/** @relatesalso DualQuaternion
@brief Screw linear interpolation of two dual quaternions
@param normalizedA  First dual quaternion
@param normalizedB  Second dual quaternion
@param t            Interpolation phase (from range @f$ [0; 1] @f$)

Expects that both dual quaternions are normalized. If the real parts are the
same or one is a negation of the other, returns the @ref DualQuaternion::rotation()
(real) part combined with interpolated @ref DualQuaternion::translation(): @f[
    \begin{array}{rcl}
        d & = & q_{A_0} \cdot q_{B_0} \\[5pt]
        {\hat q}_{ScLERP} & = & 2 \left[(1 - t)(q_{A_\epsilon} q_{A_0}^*)_V + t (q_{B_\epsilon} q_{B_0}^*)_V \right] q_A, ~ {\color{m-primary} \text{if} ~ d \ge 1}
    \end{array}
@f]

@m_class{m-noindent}

otherwise, the interpolation is performed as: @f[
    \begin{array}{rcl}
        l + \epsilon m & = & \hat q_A^* \hat q_B \\[5pt]
        \frac{\hat a} 2 & = & \arccos \left( l_S \right) - \epsilon m_S \frac 1 {|\boldsymbol{l}_V|} \\[5pt]
        \hat {\boldsymbol n} & = & \boldsymbol n_0 + \epsilon \boldsymbol n_\epsilon,
        ~~~~~~~~ \boldsymbol n_0 = \boldsymbol{l}_V \frac 1 {|\boldsymbol{l}_V|},
        ~~~~~~~~ \boldsymbol n_\epsilon = \left(\boldsymbol{m}_V - {\boldsymbol n}_0 \frac {a_\epsilon} 2 l_S \right)\frac 1 {|\boldsymbol{l}_V|} \\[5pt]
        {\hat q}_{ScLERP} & = & \hat q_A (\hat q_A^* \hat q_B)^t =
            \hat q_A \left[ \hat {\boldsymbol n} \sin \left( t \frac {\hat a} 2 \right), \cos \left( t \frac {\hat a} 2 \right) \right]
    \end{array}
@f]

Note that this function does not check for shortest path interpolation, see
@ref sclerpShortestPath() for an alternative.
@see @ref DualQuaternion::isNormalized(), @ref DualQuaternion::quaternionConjugated(),
    @ref lerp(const Quaternion<T>&, const Quaternion<T>&, T),
    @ref slerp(const Quaternion<T>&, const Quaternion<T>&, T)
*/
template<class T> inline DualQuaternion<T> sclerp(const DualQuaternion<T>& normalizedA, const DualQuaternion<T>& normalizedB, const T t) {
    CORRADE_ASSERT(normalizedA.isNormalized() && normalizedB.isNormalized(),
        "Math::sclerp(): dual quaternions" << normalizedA << "and" << normalizedB << "are not normalized", {});
    const T cosHalfAngle = dot(normalizedA.real(), normalizedB.real());

    /* Avoid division by zero: interpolate just the translation part */
    /** @todo could this be optimized somehow? */
    if(std::abs(cosHalfAngle) >= T(1) - TypeTraits<T>::epsilon())
        return DualQuaternion<T>::translation(Implementation::lerp(normalizedA.translation(), normalizedB.translation(), t))*DualQuaternion<T>{normalizedA.real()};

    /* l + εm = q_A^**q_B */
    const DualQuaternion<T> diff = normalizedA.quaternionConjugated()*normalizedB;
    const Quaternion<T>& l = diff.real();
    const Quaternion<T>& m = diff.dual();

    /* a/2 = acos(l_S) - εm_S/|l_V| */
    const T invr = l.vector().lengthInverted();
    const Dual<T> aHalf{std::acos(l.scalar()), -m.scalar()*invr};

    /* direction = n_0 = l_V/|l_V|
       moment = n_ε = (m_V - n_0*(a_ε/2)*l_S)/|l_V| */
    const Vector3<T> direction = l.vector()*invr;
    const Vector3<T> moment = (m.vector() - direction*(aHalf.dual()*l.scalar()))*invr;
    const Dual<Vector3<T>> n{direction, moment};

    /* q_ScLERP = q_A*(cos(t*a/2) + n*sin(t*a/2)) */
    const std::pair<Dual<T>, Dual<T>> sincos = Math::sincos(t*Dual<Rad<T>>(aHalf));
    return normalizedA*DualQuaternion<T>{n*sincos.first, sincos.second};
}

/** @relatesalso DualQuaternion
@brief Screw linear shortest-path interpolation of two dual quaternions
@param normalizedA  First dual quaternion
@param normalizedB  Second dual quaternion
@param t            Interpolation phase (from range @f$ [0; 1] @f$)

Unlike @ref sclerp(const DualQuaternion<T>&, const DualQuaternion<T>&, T) this
function interpolates on the shortest path. Expects that both dual quaternions
are normalized. If the real parts are the same or one is a negation of the
other, returns the @ref DualQuaternion::rotation() (real) part combined with
interpolated @ref DualQuaternion::translation(): @f[
    \begin{array}{rcl}
        d & = & q_{A_0} \cdot q_{B_0} \\[5pt]
        {\hat q}_{ScLERP} & = & 2 \left((1 - t)(q_{A_\epsilon} q_{A_0}^*)_V + t (q_{B_\epsilon} q_{B_0}^*)_V \right) (q_{A_0} + \epsilon [\boldsymbol{0}, 0]), ~ {\color{m-primary} \text{if} ~ d \ge 1}
    \end{array}
@f]

@m_class{m-noindent}

otherwise, the interpolation is performed as: @f[
    \begin{array}{rcl}
        l + \epsilon m & = & \begin{cases}
            \phantom{-}\hat q_A^* \hat q_B, & d \ge 0 \\
            -\hat q_A^* \hat q_B, & d < 0 \\
        \end{cases} \\[15pt]
        \frac{\hat a} 2 & = & \arccos \left( l_S \right) - \epsilon m_S \frac 1 {|\boldsymbol{l}_V|} \\[5pt]
        \hat {\boldsymbol n} & = & \boldsymbol n_0 + \epsilon \boldsymbol n_\epsilon,
        ~~~~~~~~ \boldsymbol n_0 = \boldsymbol{l}_V \frac 1 {|\boldsymbol{l}_V|},
        ~~~~~~~~ \boldsymbol n_\epsilon = \left(\boldsymbol{m}_V - {\boldsymbol n}_0 \frac {a_\epsilon} 2 l_S \right)\frac 1 {|\boldsymbol{l}_V|} \\[5pt]
        {\hat q}_{ScLERP} & = & \hat q_A (\hat q_A^* \hat q_B)^t =
            \hat q_A \left[ \hat {\boldsymbol n} \sin \left( t \frac {\hat a} 2 \right), \cos \left( t \frac {\hat a} 2 \right) \right]
    \end{array}
@f]
@see @ref DualQuaternion::isNormalized(), @ref lerpShortestPath(),
    @ref slerpShortestPath()
*/
template<class T> inline DualQuaternion<T> sclerpShortestPath(const DualQuaternion<T>& normalizedA, const DualQuaternion<T>& normalizedB, const T t) {
    CORRADE_ASSERT(normalizedA.isNormalized() && normalizedB.isNormalized(),
        "Math::sclerp(): dual quaternions" << normalizedA << "and" << normalizedB << "are not normalized", {});
    const T cosHalfAngle = dot(normalizedA.real(), normalizedB.real());

    /* Avoid division by zero: interpolate just the translation part */
    /** @todo could this be optimized somehow? */
    if(std::abs(cosHalfAngle) >= T(1) - TypeTraits<T>::epsilon())
        return DualQuaternion<T>::translation(Implementation::lerp(normalizedA.translation(), normalizedB.translation(), t))*DualQuaternion<T>{normalizedA.real()};

    /* l + εm = q_A^**q_B, multiplying with -1 ensures shortest path when dot < 0 */
    const DualQuaternion<T> diff = normalizedA.quaternionConjugated()*(cosHalfAngle < T(0) ? -normalizedB : normalizedB);
    const Quaternion<T>& l = diff.real();
    const Quaternion<T>& m = diff.dual();

    /* a/2 = acos(l_S) - εm_S/|l_V| */
    const T invr = l.vector().lengthInverted();
    const Dual<T> aHalf{std::acos(l.scalar()), -m.scalar()*invr};

    /* direction = n_0 = l_V/|l_V|
       moment = n_ε = (m_V - n_0*(a_ε/2)*l_S)/|l_V| */
    const Vector3<T> direction = l.vector()*invr;
    const Vector3<T> moment = (m.vector() - direction*(aHalf.dual()*l.scalar()))*invr;
    const Dual<Vector3<T>> n{direction, moment};

    /* q_ScLERP = q_A*(cos(t*a/2) + n*sin(t*a/2)) */
    const std::pair<Dual<T>, Dual<T>> sincos = Math::sincos(t*Dual<Rad<T>>(aHalf));
    return normalizedA*DualQuaternion<T>{n*sincos.first, sincos.second};
}

/**
@brief Dual quaternion
@tparam T   Underlying data type

Represents 3D rotation and translation. Usually denoted as the following in
equations, with @f$ q_0 @f$ being the @ref real() part and @f$ q_\epsilon @f$
the @ref dual() part: @f[
    \hat q = q_0 + \epsilon q_\epsilon
@f]

See @ref Dual and @ref Quaternion for further notation description and
@ref transformations for a brief introduction.
@see @ref Magnum::DualQuaternion, @ref Magnum::DualQuaterniond, @ref Dual,
    @ref Quaternion, @ref Matrix4
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
         *      \hat q = [\boldsymbol a \cdot \sin(\frac{\theta}{2}), \cos(\frac{\theta}{2})] + \epsilon [\boldsymbol 0, 0]
         * @f]
         *
         * For creating a dual quaternion from a rotation @ref Quaternion, use
         * the implicit conversion provided by
         * @ref DualQuaternion(const Quaternion<T>&, const Quaternion<T>&).
         * @see @ref rotation() const, @ref Quaternion::rotation(),
         *      @ref Matrix4::rotation(), @ref DualComplex::rotation(),
         *      @ref Vector3::xAxis(), @ref Vector3::yAxis(),
         *      @ref Vector3::zAxis(), @ref Vector::isNormalized()
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
         * @see @ref translation() const,
         *      @ref Matrix4::translation(const Vector3<T>&),
         *      @ref DualComplex::translation(), @ref Vector3::xAxis(),
         *      @ref Vector3::yAxis(), @ref Vector3::zAxis()
         */
        static DualQuaternion<T> translation(const Vector3<T>& vector) {
            return {{}, {vector/T(2), T(0)}};
        }

        /**
         * @brief Create dual quaternion from transformation matrix
         *
         * Expects that the matrix represents rigid transformation.
         * @see @ref toMatrix(), @ref Quaternion::fromMatrix(),
         *      @ref Matrix4::isRigidTransformation()
         */
        static DualQuaternion<T> fromMatrix(const Matrix4<T>& matrix) {
            CORRADE_ASSERT(matrix.isRigidTransformation(),
                "Math::DualQuaternion::fromMatrix(): the matrix doesn't represent a rigid transformation:" << Corrade::Utility::Debug::newline << matrix, {});

            Quaternion<T> q = Implementation::quaternionFromMatrix(matrix.rotationScaling());
            return {q, Quaternion<T>(matrix.translation()/2)*q};
        }

        /**
         * @brief Create dual quaternion from rotation quaternion and translation vector
         * @m_since_latest
         *
         * @f[
         *      \hat q = r + \epsilon [\frac{\boldsymbol t}{2}, 0] r
         * @f]
         *
         * @see @ref translation(), @ref rotation(),
         *      @ref Matrix3::from(const Matrix2x2<T>&, const Vector2<T>&),
         *      @ref Matrix4::from(const Matrix3x3<T>&, const Vector3<T>&),
         *      @ref DualComplex::from(const Complex<T>&, const Vector2<T>&)
         */
        static DualQuaternion<T> from(const Quaternion<T>& rotation, const Vector3<T>& translation) {
            return {rotation, Quaternion<T>{translation/T(2)}*rotation};
        }

        /**
         * @brief Default constructor
         *
         * Equivalent to @ref DualQuaternion(IdentityInitT).
         */
        constexpr /*implicit*/ DualQuaternion() noexcept: Dual<Quaternion<T>>{{}, {{}, T(0)}} {}

        /**
         * @brief Identity constructor
         *
         * Creates unit dual quaternion. @f[
         *      \hat q = [\boldsymbol 0, 1] + \epsilon [\boldsymbol 0, 0]
         * @f]
         */
        constexpr explicit DualQuaternion(IdentityInitT) noexcept: Dual<Quaternion<T>>{{}, {{}, T(0)}} {}

        /** @brief Construct zero-initialized dual quaternion */
        constexpr explicit DualQuaternion(ZeroInitT) noexcept: Dual<Quaternion<T>>{Quaternion<T>{ZeroInit}, Quaternion<T>{ZeroInit}} {}

        /** @brief Construct without initializing the contents */
        explicit DualQuaternion(Magnum::NoInitT) noexcept: Dual<Quaternion<T>>{Magnum::NoInit} {}

        /**
         * @brief Construct dual quaternion from real and dual part
         *
         * @f[
         *      \hat q = q_0 + \epsilon q_\epsilon
         * @f]
         *
         * This constructor can be also used to implicitly convert a rotation
         * quaternion to a rotation dual quaternion.
         */
        constexpr /*implicit*/ DualQuaternion(const Quaternion<T>& real, const Quaternion<T>& dual = Quaternion<T>({}, T(0))) noexcept: Dual<Quaternion<T>>(real, dual) {}

        /**
         * @brief Construct dual quaternion from dual vector and scalar parts
         *
         * @f[
         *      \hat q = [\hat{\boldsymbol v}, \hat s] = [\boldsymbol v_0, s_0] + \epsilon [\boldsymbol v_\epsilon, s_\epsilon]
         * @f]
         */
        constexpr /*implicit*/ DualQuaternion(const Dual<Vector3<T>>& vector, const Dual<T>& scalar) noexcept: Dual<Quaternion<T>>{{vector.real(), scalar.real()}, {vector.dual(), scalar.dual()}} {}

        /**
         * @brief Construct dual quaternion from vector
         *
         * To be used in transformations later. @f[
         *      \hat q = [\boldsymbol 0, 1] + \epsilon [\boldsymbol v, 0]
         * @f]
         * @see @ref transformPointNormalized()
         */
        constexpr explicit DualQuaternion(const Vector3<T>& vector) noexcept: Dual<Quaternion<T>>({}, {vector, T(0)}) {}

        /**
         * @brief Construct dual quaternion from another of different type
         *
         * Performs only default casting on the values, no rounding or anything
         * else.
         */
        template<class U> constexpr explicit DualQuaternion(const DualQuaternion<U>& other) noexcept: Dual<Quaternion<T>>(other) {}

        /** @brief Construct dual quaternion from external representation */
        template<class U, class V = decltype(Implementation::DualQuaternionConverter<T, U>::from(std::declval<U>()))> constexpr explicit DualQuaternion(const U& other): DualQuaternion{Implementation::DualQuaternionConverter<T, U>::from(other)} {}

        /** @brief Copy constructor */
        constexpr /*implicit*/ DualQuaternion(const Dual<Quaternion<T>>& other) noexcept: Dual<Quaternion<T>>(other) {}

        /** @brief Convert dual quaternion to external representation */
        template<class U, class V = decltype(Implementation::DualQuaternionConverter<T, U>::to(std::declval<DualQuaternion<T>>()))> constexpr explicit operator U() const {
            return Implementation::DualQuaternionConverter<T, U>::to(*this);
        }

        /**
         * @brief Raw data
         * @return One-dimensional array of eight elements
         *
         * @see @ref real(), @ref dual()
         */
        T* data() { return Dual<Quaternion<T>>::data()->data(); }
        constexpr const T* data() const { return Dual<Quaternion<T>>::data()->data(); } /**< @overload */

        /**
         * @brief Whether the dual quaternion is normalized
         *
         * Dual quaternion is normalized if it has unit length: @f[
         *      |\hat q|^2 = |\hat q| = 1 + \epsilon 0
         * @f]
         * @see @ref lengthSquared(), @ref normalized()
         * @todoc Improve the equation as in Quaternion::isNormalized()
         */
        bool isNormalized() const {
            /* Comparing dual part to zero considering the magnitude of the
               translation -- the epsilon be much larger for large values. */
            Dual<T> a = lengthSquared();
            return Implementation::isNormalizedSquared(a.real()) &&
                   TypeTraits<T>::equalsZero(a.dual(), Math::max(Math::abs(Math::Dual<Quaternion<T>>::dual().vector()).max(), Math::abs(Math::Dual<Quaternion<T>>::dual().scalar())));
        }

        /**
         * @brief Rotation part of unit dual quaternion
         *
         * @see @ref Quaternion::angle(), @ref Quaternion::axis()
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
         * @see @ref translation(const Vector3<T>&)
         */
        Vector3<T> translation() const {
            return (Dual<Quaternion<T>>::dual()*Dual<Quaternion<T>>::real().conjugated()).vector()*T(2);
        }

        /**
         * @brief Convert dual quaternion to transformation matrix
         *
         * @see @ref fromMatrix(), @ref Quaternion::toMatrix()
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
         * @see @ref dualConjugated(), @ref conjugated(),
         *      @ref Quaternion::conjugated()
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
         * @see @ref quaternionConjugated(), @ref conjugated(),
         *      @ref Dual::conjugated()
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
         * @see @ref quaternionConjugated(), @ref dualConjugated(),
         *      @ref Quaternion::conjugated(), @ref Dual::conjugated()
         */
        DualQuaternion<T> conjugated() const {
            return {Dual<Quaternion<T>>::real().conjugated(), {Dual<Quaternion<T>>::dual().vector(), -Dual<Quaternion<T>>::dual().scalar()}};
        }

        /**
         * @brief Dual quaternion length squared
         *
         * Should be used instead of @ref length() for comparing dual
         * quaternion length with other values, because it doesn't compute the
         * square root. @f[
         *      |\hat q|^2 = \sqrt{\hat q^* \hat q}^2 = q_0 \cdot q_0 + \epsilon 2 (q_0 \cdot q_\epsilon)
         * @f]
         */
        Dual<T> lengthSquared() const {
            return {Dual<Quaternion<T>>::real().dot(), T(2)*dot(Dual<Quaternion<T>>::real(), Dual<Quaternion<T>>::dual())};
        }

        /**
         * @brief Dual quaternion length
         *
         * See @ref lengthSquared() which is faster for comparing length with other
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
         * @see @ref isNormalized()
         */
        DualQuaternion<T> normalized() const {
            return (*this)/length();
        }

        /**
         * @brief Inverted dual quaternion
         *
         * See @ref invertedNormalized() which is faster for normalized dual
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
         * Equivalent to @ref quaternionConjugated(). Expects that the
         * quaternion is normalized. @f[
         *      \hat q^{-1} = \frac{\hat q^*}{|\hat q|^2} = \hat q^*
         * @f]
         * @see @ref isNormalized(), @ref inverted()
         */
        DualQuaternion<T> invertedNormalized() const {
            CORRADE_ASSERT(isNormalized(),
                "Math::DualQuaternion::invertedNormalized():" << *this << "is not normalized", {});
            return quaternionConjugated();
        }

        /**
         * @brief Rotate a vector with a dual quaternion
         * @m_since{2020,06}
         *
         * Calls @ref Quaternion::transformVector() on the @ref real() part,
         * see its documentation for more information.
         */
        Vector3<T> transformVector(const Vector3<T>& vector) const {
            return Dual<Quaternion<T>>::real().transformVector(vector);
        }

        /**
         * @brief Rotate a vector with a normalized dual quaternion
         * @m_since{2020,06}
         *
         * Calls @ref Quaternion::transformVectorNormalized() on the
         * @ref real() part, see its documentation for more information.
         */
        Vector3<T> transformVectorNormalized(const Vector3<T>& vector) const {
            return Dual<Quaternion<T>>::real().transformVectorNormalized(vector);
        }

        /**
         * @brief Rotate and translate point with dual quaternion
         *
         * See @ref transformPointNormalized(), which is faster for normalized
         * dual quaternions. @f[
         *      v' = \hat q v \overline{\hat q^{-1}} = \hat q ([\boldsymbol 0, 1] + \epsilon [\boldsymbol v, 0]) \overline{\hat q^{-1}}
         * @f]
         * @see @ref DualQuaternion(const Vector3<T>&), @ref dual(),
         *      @ref Matrix4::transformPoint(),
         *      @ref DualComplex::transformPoint()
         */
        Vector3<T> transformPoint(const Vector3<T>& vector) const {
            return ((*this)*DualQuaternion<T>(vector)*inverted().dualConjugated()).dual().vector();
        }

        /**
         * @brief Rotate and translate point with normalized dual quaternion
         *
         * Faster alternative to @ref transformPoint(), expects that the dual
         * quaternion is normalized. @f[
         *      v' = \hat q v \overline{\hat q^{-1}} = \hat q v \overline{\hat q^*} = \hat q ([\boldsymbol 0, 1] + \epsilon [\boldsymbol v, 0]) \overline{\hat q^*}
         * @f]
         * @see @ref isNormalized(), @ref DualQuaternion(const Vector3<T>&),
         *      @ref dual(), @ref Matrix4::transformPoint(),
         *      @ref Quaternion::transformVectorNormalized(),
         *      @ref DualComplex::transformPoint()
         */
        Vector3<T> transformPointNormalized(const Vector3<T>& vector) const {
            CORRADE_ASSERT(isNormalized(),
                "Math::DualQuaternion::transformPointNormalized():" << *this << "is not normalized", {});
            return ((*this)*DualQuaternion<T>(vector)*conjugated()).dual().vector();
        }

        MAGNUM_DUAL_SUBCLASS_IMPLEMENTATION(DualQuaternion, Quaternion, T)
        MAGNUM_DUAL_SUBCLASS_MULTIPLICATION_IMPLEMENTATION(DualQuaternion, Quaternion)
};

MAGNUM_DUAL_OPERATOR_IMPLEMENTATION(DualQuaternion, Quaternion, T)

#ifndef CORRADE_NO_DEBUG
/** @debugoperator{DualQuaternion} */
template<class T> Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug& debug, const DualQuaternion<T>& value) {
    return debug << "DualQuaternion({{" << Corrade::Utility::Debug::nospace
        << value.real().vector().x() << Corrade::Utility::Debug::nospace << ","
        << value.real().vector().y() << Corrade::Utility::Debug::nospace << ","
        << value.real().vector().z() << Corrade::Utility::Debug::nospace << "},"
        << value.real().scalar() << Corrade::Utility::Debug::nospace << "}, {{"
        << Corrade::Utility::Debug::nospace
        << value.dual().vector().x() << Corrade::Utility::Debug::nospace << ","
        << value.dual().vector().y() << Corrade::Utility::Debug::nospace << ","
        << value.dual().vector().z() << Corrade::Utility::Debug::nospace << "},"
        << value.dual().scalar() << Corrade::Utility::Debug::nospace << "})";
}

/* Explicit instantiation for commonly used types */
#ifndef DOXYGEN_GENERATING_OUTPUT
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const DualQuaternion<Float>&);
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const DualQuaternion<Double>&);
#endif
#endif

namespace Implementation {
    template<class T> struct StrictWeakOrdering<DualQuaternion<T>>: StrictWeakOrdering<Dual<Quaternion<T>>> {};
}

}}

#endif
