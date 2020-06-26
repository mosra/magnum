#ifndef Magnum_SceneGraph_DualQuaternionTransformation_h
#define Magnum_SceneGraph_DualQuaternionTransformation_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class @ref Magnum::SceneGraph::BasicDualQuaternionTransformation, typedef @ref Magnum::SceneGraph::DualQuaternionTransformation
 */

#include "Magnum/Math/DualQuaternion.h"
#include "Magnum/SceneGraph/AbstractTranslationRotation3D.h"
#include "Magnum/SceneGraph/Object.h"

namespace Magnum { namespace SceneGraph {

/**
@brief Three-dimensional transformation implemented using dual quaternions

This class allows only rigid transformation (i.e. only rotation and
translation). Uses @ref Math::DualQuaternion as underlying transformation type.
@see @ref scenegraph, @ref DualQuaternionTransformation,
    @ref BasicDualComplexTransformation
*/
template<class T> class BasicDualQuaternionTransformation: public AbstractBasicTranslationRotation3D<T> {
    public:
        /** @brief Underlying transformation type */
        typedef Math::DualQuaternion<T> DataType;

        /** @brief Object transformation */
        Math::DualQuaternion<T> transformation() const { return _transformation; }

        /**
         * @brief Set transformation
         * @return Reference to self (for method chaining)
         *
         * Expects that the dual quaternion is normalized.
         * @see @ref Math::DualQuaternion::isNormalized()
         */
        Object<BasicDualQuaternionTransformation<T>>& setTransformation(const Math::DualQuaternion<T>& transformation) {
            CORRADE_ASSERT(transformation.isNormalized(),
                "SceneGraph::DualQuaternionTransformation::setTransformation(): the dual quaternion is not normalized",
                static_cast<Object<BasicDualQuaternionTransformation<T>>&>(*this));
            return setTransformationInternal(transformation);
        }

        /** @copydoc AbstractTranslationRotationScaling3D::resetTransformation() */
        Object<BasicDualQuaternionTransformation<T>>& resetTransformation() {
            return setTransformationInternal({});
        }

        /**
         * @brief Normalize rotation part
         * @return Reference to self (for method chaining)
         *
         * Normalizes the rotation part to prevent rounding errors when rotating
         * the object subsequently.
         * @see @ref Math::DualQuaternion::normalized()
         */
        Object<BasicDualQuaternionTransformation<T>>& normalizeRotation() {
            return setTransformationInternal(_transformation.normalized());
        }

        /**
         * @brief Transform the object
         * @return Reference to self (for method chaining)
         *
         * Expects that the dual quaternion is normalized.
         * @see @ref transformLocal(), @ref Math::DualQuaternion::isNormalized()
         */
        Object<BasicDualQuaternionTransformation<T>>& transform(const Math::DualQuaternion<T>& transformation) {
            CORRADE_ASSERT(transformation.isNormalized(),
                "SceneGraph::DualQuaternionTransformation::transform(): the dual quaternion is not normalized",
                static_cast<Object<BasicDualQuaternionTransformation<T>>&>(*this));
            return transformInternal(transformation);
        }

        /**
         * @brief Transform the object as a local transformation
         *
         * Similar to the above, except that the transformation is applied
         * before all others.
         */
        Object<BasicDualQuaternionTransformation<T>>& transformLocal(const Math::DualQuaternion<T>& transformation) {
            CORRADE_ASSERT(transformation.isNormalized(),
                "SceneGraph::DualQuaternionTransformation::transformLocal(): the dual quaternion is not normalized",
                static_cast<Object<BasicDualQuaternionTransformation<T>>&>(*this));
            return transformLocalInternal(transformation);
        }

        /**
         * @brief Translate the object
         * @return Reference to self (for method chaining)
         *
         * Same as calling @ref transform() with @ref Math::DualQuaternion::translation().
         * @see @ref translateLocal(), @ref Math::Vector3::xAxis(),
         *      @ref Math::Vector3::yAxis(), @ref Math::Vector3::zAxis()
         */
        Object<BasicDualQuaternionTransformation<T>>& translate(const Math::Vector3<T>& vector) {
            return transformInternal(Math::DualQuaternion<T>::translation(vector));
        }

        /**
         * @brief Translate the object as a local transformation
         *
         * Similar to the above, except that the transformation is applied
         * before all others.
         */
        Object<BasicDualQuaternionTransformation<T>>& translateLocal(const Math::Vector3<T>& vector) {
            return transformLocalInternal(Math::DualQuaternion<T>::translation(vector));
        }

        /**
         * @brief Rotate the object using a quaternion
         * @param quaternion    Normalized quaternion
         * @return Reference to self (for method chaining)
         * @m_since{2020,06}
         *
         * Same as calling @ref transform() with @p quaternion. Expects that
         * the quaternion is normalized.
         * @see @ref rotate(Math::Rad<T>, const Math::Vector3<T>&),
         *      @ref rotateLocal(const Math::Quaternion<T>&), @ref rotateX(),
         *      @ref rotateY(), @ref rotateZ()
         */
        Object<BasicDualQuaternionTransformation<T>>& rotate(const Math::Quaternion<T>& quaternion) {
            return transformInternal(quaternion);
        }

        /**
         * @brief Rotate the object using a quaternion as a local transformation
         * @m_since{2020,06}
         *
         * Similar to the above, except that the transformation is applied
         * before all others.
         */
        Object<BasicDualQuaternionTransformation<T>>& rotateLocal(const Math::Quaternion<T>& quaternion) {
            return transformLocalInternal(quaternion);
        }

        /**
         * @brief Rotate the object
         * @param angle             Angle (counterclockwise)
         * @param normalizedAxis    Normalized rotation axis
         * @return Reference to self (for method chaining)
         *
         * Same as calling @ref transform() with @ref Math::DualQuaternion::rotation().
         * @see @ref rotate(const Math::Quaternion<T>&), @ref rotateLocal(),
         *      @ref Math::Vector3::xAxis(), @ref Math::Vector3::yAxis(),
         *      @ref Math::Vector3::zAxis(), @ref normalizeRotation()
         */
        Object<BasicDualQuaternionTransformation<T>>& rotate(Math::Rad<T> angle, const Math::Vector3<T>& normalizedAxis) {
            return transformInternal(Math::DualQuaternion<T>::rotation(angle, normalizedAxis));
        }

        /**
         * @brief Rotate the object as a local transformation
         *
         * Similar to the above, except that the transformation is applied
         * before all others.
         */
        Object<BasicDualQuaternionTransformation<T>>& rotateLocal(Math::Rad<T> angle, const Math::Vector3<T>& normalizedAxis) {
            return transformLocalInternal(Math::DualQuaternion<T>::rotation(angle, normalizedAxis));
        }

        /* Overloads to remove WTF-factor from method chaining order */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        Object<BasicDualQuaternionTransformation<T>>& rotateX(Math::Rad<T> angle) {
            return rotate(angle, Math::Vector3<T>::xAxis());
        }
        Object<BasicDualQuaternionTransformation<T>>& rotateXLocal(Math::Rad<T> angle) {
            return rotateLocal(angle, Math::Vector3<T>::xAxis());
        }
        Object<BasicDualQuaternionTransformation<T>>& rotateY(Math::Rad<T> angle) {
            return rotate(angle, Math::Vector3<T>::yAxis());
        }
        Object<BasicDualQuaternionTransformation<T>>& rotateYLocal(Math::Rad<T> angle) {
            return rotateLocal(angle, Math::Vector3<T>::yAxis());
        }
        Object<BasicDualQuaternionTransformation<T>>& rotateZ(Math::Rad<T> angle) {
            return rotate(angle, Math::Vector3<T>::zAxis());
        }
        Object<BasicDualQuaternionTransformation<T>>& rotateZLocal(Math::Rad<T> angle) {
            return rotateLocal(angle, Math::Vector3<T>::zAxis());
        }
        #endif

    protected:
        /* Allow construction only from Object */
        explicit BasicDualQuaternionTransformation() = default;

    private:
        void doResetTransformation() override final { resetTransformation(); }

        void doTranslate(const Math::Vector3<T>& vector) override final {
            translate(vector);
        }
        void doTranslateLocal(const Math::Vector3<T>& vector) override final {
            translateLocal(vector);
        }

        void doRotate(const Math::Quaternion<T>& quaternion) override final {
            rotate(quaternion);
        }
        void doRotateLocal(const Math::Quaternion<T>& quaternion) override final {
            rotateLocal(quaternion);
        }

        void doRotate(Math::Rad<T> angle, const Math::Vector3<T>& normalizedAxis) override final {
            rotate(angle, normalizedAxis);
        }
        void doRotateLocal(Math::Rad<T> angle, const Math::Vector3<T>& normalizedAxis) override final {
            rotateLocal(angle, normalizedAxis);
        }

        /* No assertions fired, for internal use */
        Object<BasicDualQuaternionTransformation<T>>& setTransformationInternal(const Math::DualQuaternion<T>& transformation) {
            /* Setting transformation is forbidden for the scene */
            /** @todo Assert for this? */
            /** @todo Do this in some common code so we don't need to include Object? */
            if(!static_cast<Object<BasicDualQuaternionTransformation<T>>*>(this)->isScene()) {
                _transformation = transformation;
                static_cast<Object<BasicDualQuaternionTransformation<T>>*>(this)->setDirty();
            }

            return static_cast<Object<BasicDualQuaternionTransformation<T>>&>(*this);
        }

        /* No assertions fired, for internal use */
        Object<BasicDualQuaternionTransformation<T>>& transformInternal(const Math::DualQuaternion<T>& transformation) {
            return setTransformationInternal(transformation*_transformation);
        }
        Object<BasicDualQuaternionTransformation<T>>& transformLocalInternal(const Math::DualQuaternion<T>& transformation) {
            return setTransformationInternal(_transformation*transformation);
        }

        Math::DualQuaternion<T> _transformation;
};

/**
@brief Three-dimensional transformation for float scenes implemented using dual quaternions

@see @ref DualComplexTransformation
*/
typedef BasicDualQuaternionTransformation<Float> DualQuaternionTransformation;

namespace Implementation {

template<class T> struct Transformation<BasicDualQuaternionTransformation<T>> {
    static Math::DualQuaternion<T> fromMatrix(const Math::Matrix4<T>& matrix) {
        CORRADE_ASSERT(matrix.isRigidTransformation(),
            "SceneGraph::DualQuaternionTransformation: the matrix doesn't represent rigid transformation", {});
        return Math::DualQuaternion<T>::fromMatrix(matrix);
    }

    constexpr static Math::Matrix4<T> toMatrix(const Math::DualQuaternion<T>& transformation) {
        return transformation.toMatrix();
    }

    static Math::DualQuaternion<T> compose(const Math::DualQuaternion<T>& parent, const Math::DualQuaternion<T>& child) {
        return parent*child;
    }

    static Math::DualQuaternion<T> inverted(const Math::DualQuaternion<T>& transformation) {
        return transformation.invertedNormalized();
    }
};

}

#if defined(CORRADE_TARGET_WINDOWS) && !(defined(CORRADE_TARGET_MINGW) && !defined(CORRADE_TARGET_CLANG))
extern template class MAGNUM_SCENEGRAPH_EXPORT Object<BasicDualQuaternionTransformation<Float>>;
#endif

}}

#endif
