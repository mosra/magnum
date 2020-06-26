#ifndef Magnum_SceneGraph_AbstractTranslationRotation3D_h
#define Magnum_SceneGraph_AbstractTranslationRotation3D_h
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
 * @brief Class @ref Magnum::SceneGraph::AbstractBasicTranslationRotation3D, typedef @ref Magnum::SceneGraph::AbstractTranslationRotation3D
 */

#include "Magnum/SceneGraph/AbstractTranslation.h"

namespace Magnum { namespace SceneGraph {

/**
@brief Base transformation for three-dimensional scenes supporting translation and rotation

See @ref scenegraph-features-transformation for more information.

@see @ref scenegraph, @ref AbstractTranslationRotation3D,
    @ref AbstractBasicTranslationRotation2D,
    @ref BasicRigidMatrixTransformation3D,
    @ref BasicDualQuaternionTransformation
*/
template<class T> class AbstractBasicTranslationRotation3D: public AbstractBasicTranslation3D<T> {
    public:
        explicit AbstractBasicTranslationRotation3D() = default;

        /**
         * @brief Rotate the object using a quaternion
         * @param quaternion    Normalized quaternion
         * @return Reference to self (for method chaining)
         * @m_since{2020,06}
         *
         * Expects that the quaternion is normalized.
         * @see @ref rotate(Math::Rad<T>, const Math::Vector3<T>&),
         *      @ref rotateLocal(const Math::Quaternion<T>&), @ref rotateX(),
         *      @ref rotateY(), @ref rotateZ()
         */
        AbstractBasicTranslationRotation3D<T>& rotate(const Math::Quaternion<T>& quaternion) {
            doRotate(quaternion);
            return *this;
        }

        /**
         * @brief Rotate the object using a quaternion as a local transformation
         * @m_since{2020,06}
         *
         * Similar to the above, except that the transformation is applied
         * before all others.
         */
        AbstractBasicTranslationRotation3D<T>& rotateLocal(const Math::Quaternion<T>& quaternion) {
            doRotateLocal(quaternion);
            return *this;
        }

        /**
         * @brief Rotate the object
         * @param angle             Angle (counterclockwise)
         * @param normalizedAxis    Normalized rotation axis
         * @return Reference to self (for method chaining)
         *
         * @see @ref rotate(const Math::Quaternion<T>&), @ref rotateLocal(),
         *      @ref rotateX(), @ref rotateY(), @ref rotateZ(),
         *      @ref Math::Vector3::xAxis(), @ref Math::Vector3::yAxis(),
         *      @ref Math::Vector3::zAxis()
         */
        AbstractBasicTranslationRotation3D<T>& rotate(Math::Rad<T> angle, const Math::Vector3<T>& normalizedAxis) {
            doRotate(angle, normalizedAxis);
            return *this;
        }

        /**
         * @brief Rotate the object as a local transformation
         *
         * Similar to the above, except that the transformation is applied
         * before all others.
         */
        AbstractBasicTranslationRotation3D<T>& rotateLocal(Math::Rad<T> angle, const Math::Vector3<T>& normalizedAxis) {
            doRotateLocal(angle, normalizedAxis);
            return *this;
        }

        /**
         * @brief Rotate the object around X axis
         * @param angle             Angle (counterclockwise)
         * @return Reference to self (for method chaining)
         *
         * In some implementations faster than calling
         * @cpp rotate(angle, Vector3::xAxis()) @ce, see subclasses for more
         * information.
         * @see @ref rotateXLocal()
         */
        AbstractBasicTranslationRotation3D<T>& rotateX(Math::Rad<T> angle) {
            doRotateX(angle);
            return *this;
        }

        /**
         * @brief Rotate the object around X axis as a local transformation
         *
         * Similar to the above, except that the transformation is applied
         * before all others. In some implementations faster than calling
         * @cpp rotateLocal(angle, Vector3::xAxis()) @ce, see subclasses for
         * more information.
         */
        AbstractBasicTranslationRotation3D<T>& rotateXLocal(Math::Rad<T> angle) {
            doRotateXLocal(angle);
            return *this;
        }

        /**
         * @brief Rotate the object around Y axis
         * @param angle             Angle (counterclockwise)
         * @return Reference to self (for method chaining)
         *
         * In some implementations faster than calling
         * @cpp rotate(angle, Vector3::yAxis()) @ce, see subclasses for more
         * information.
         * @see @ref rotateYLocal()
         */
        AbstractBasicTranslationRotation3D<T>& rotateY(Math::Rad<T> angle) {
            doRotateY(angle);
            return *this;
        }

        /**
         * @brief Rotate the object around Y axis as a local transformation
         *
         * Similar to the above, except that the transformation is applied
         * before all others. In some implementations faster than calling
         * @cpp rotateLocal(angle, Vector3::yAxis()) @ce, see subclasses for
         * more information.
         */
        AbstractBasicTranslationRotation3D<T>& rotateYLocal(Math::Rad<T> angle) {
            doRotateYLocal(angle);
            return *this;
        }

        /**
         * @brief Rotate the object around Z axis
         * @param angle             Angle (counterclockwise)
         * @return Reference to self (for method chaining)
         *
         * In some implementations faster than calling
         * @cpp rotate(angle, Vector3::zAxis()) @ce, see subclasses for more
         * information.
         * @see @ref rotateZLocal()
         */
        AbstractBasicTranslationRotation3D<T>& rotateZ(Math::Rad<T> angle) {
            doRotateZ(angle);
            return *this;
        }

        /**
         * @brief Rotate the object around Z axis as a local transformation
         *
         * Similar to the above, except that the transformation is applied
         * before all others. In some implementations faster than calling
         * @cpp rotateLocal(angle, Vector3::zAxis()) @ce, see subclasses for
         * more information.
         */
        AbstractBasicTranslationRotation3D<T>& rotateZLocal(Math::Rad<T> angle) {
            doRotateZLocal(angle);
            return *this;
        }

        /* Overloads to remove WTF-factor from method chaining order */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        AbstractBasicTranslationRotation3D<T>& resetTransformation() {
            AbstractBasicTranslation3D<T>::resetTransformation();
            return *this;
        }
        AbstractBasicTranslationRotation3D<T>& translate(const Math::Vector3<T>& vector) {
            AbstractBasicTranslation3D<T>::translate(vector);
            return *this;
        }
        AbstractBasicTranslationRotation3D<T>& translateLocal(const Math::Vector3<T>& vector) {
            AbstractBasicTranslation3D<T>::translateLocal(vector);
            return *this;
        }
        #endif

    protected:
        ~AbstractBasicTranslationRotation3D() = default;

    private:
        /**
         * @brief Polymorphic implementation for @ref rotate(const Math::Quaternion<T>&)
         * @m_since{2020,06}
         */
        virtual void doRotate(const Math::Quaternion<T>&) = 0;

        /**
         * @brief Polymorphic implementation for @ref rotateLocal(const Math::Quaternion<T>&)
         * @m_since{2020,06}
         */
        virtual void doRotateLocal(const Math::Quaternion<T>&) = 0;

        /** @brief Polymorphic implementation for @ref rotate(Math::Rad<T>, const Math::Vector3<T>&) */
        virtual void doRotate(Math::Rad<T> angle, const Math::Vector3<T>& normalizedAxis) = 0;

        /** @brief Polymorphic implementation for @ref rotateLocal(Math::Rad<T>, const Math::Vector3<T>&) */
        virtual void doRotateLocal(Math::Rad<T> angle, const Math::Vector3<T>& normalizedAxis) = 0;

        /**
         * @brief Polymorphic implementation for @ref rotateX()
         *
         * Default implementation calls @ref rotate() with
         * @ref Math::Vector3::xAxis().
         */
        virtual void doRotateX(Math::Rad<T> angle) {
            rotate(angle, Math::Vector3<T>::xAxis());
        }

        /**
         * @brief Polymorphic implementation for @ref rotateXLocal()
         *
         * Default implementation calls @ref rotateLocal() with
         * @ref Math::Vector3::xAxis().
         */
        virtual void doRotateXLocal(Math::Rad<T> angle) {
            rotateLocal(angle, Math::Vector3<T>::xAxis());
        }

        /**
         * @brief Polymorphic implementation for @ref rotateY()
         *
         * Default implementation calls @ref rotate() with
         * @ref Math::Vector3::yAxis().
         */
        virtual void doRotateY(Math::Rad<T> angle) {
            rotate(angle, Math::Vector3<T>::yAxis());
        }

        /**
         * @brief Polymorphic implementation for @ref rotateYLocal()
         *
         * Default implementation calls @ref rotateLocal() with
         * @ref Math::Vector3::yAxis().
         */
        virtual void doRotateYLocal(Math::Rad<T> angle) {
            rotateLocal(angle, Math::Vector3<T>::yAxis());
        }

        /**
         * @brief Polymorphic implementation for @ref rotateZ()
         *
         * Default implementation calls @ref rotate() with
         * @ref Math::Vector3::zAxis().
         */
        virtual void doRotateZ(Math::Rad<T> angle) {
            rotate(angle, Math::Vector3<T>::zAxis());
        }

        /**
         * @brief Polymorphic implementation for @ref rotateZLocal()
         *
         * Default implementation calls @ref rotateLocal() with
         * @ref Math::Vector3::zAxis().
         */
        virtual void doRotateZLocal(Math::Rad<T> angle) {
            rotateLocal(angle, Math::Vector3<T>::zAxis());
        }
};

/**
@brief Base transformation for three-dimensional float scenes supporting translation and rotation

@see @ref AbstractTranslationRotation2D
*/
typedef AbstractBasicTranslationRotation3D<Float> AbstractTranslationRotation3D;

}}

#endif
