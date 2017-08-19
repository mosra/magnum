#ifndef Magnum_SceneGraph_AbstractTranslationRotation3D_h
#define Magnum_SceneGraph_AbstractTranslationRotation3D_h
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
         * @brief Rotate object
         * @param angle             Angle (counterclockwise)
         * @param normalizedAxis    Normalized rotation axis
         * @return Reference to self (for method chaining)
         *
         * @see @ref rotateLocal(), @ref rotateX(), @ref rotateY(),
         *      @ref rotateZ(), @ref Math::Vector3::xAxis(),
         *      @ref Math::Vector3::yAxis(), @ref Math::Vector3::zAxis()
         */
        AbstractBasicTranslationRotation3D<T>& rotate(Math::Rad<T> angle, const Math::Vector3<T>& normalizedAxis) {
            doRotate(angle, normalizedAxis);
            return *this;
        }

        /**
         * @brief Rotate object as a local transformation
         *
         * Similar to the above, except that the transformation is applied
         * before all others.
         */
        AbstractBasicTranslationRotation3D<T>& rotateLocal(Math::Rad<T> angle, const Math::Vector3<T>& normalizedAxis) {
            doRotateLocal(angle, normalizedAxis);
            return *this;
        }

        #ifdef MAGNUM_BUILD_DEPRECATED
        #ifdef __GNUC__
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wdeprecated-declarations"
        #elif defined(_MSC_VER)
        #pragma warning(push)
        #pragma warning(disable: 4996)
        #endif
        /**
         * @copybrief rotate()
         * @deprecated Use @ref rotate() or @ref rotateLocal() instead.
         */
        CORRADE_DEPRECATED("use rotate() or rotateLocal() instead") AbstractBasicTranslationRotation3D<T>& rotate(Math::Rad<T> angle, const Math::Vector3<T>& normalizedAxis, TransformationType type) {
            return type == TransformationType::Global ? rotate(angle, normalizedAxis) : rotateLocal(angle, normalizedAxis);
        }
        #ifdef __GNUC__
        #pragma GCC diagnostic pop
        #elif defined(_MSC_VER)
        #pragma warning(pop)
        #endif
        #endif

        /**
         * @brief Rotate object around X axis
         * @param angle             Angle (counterclockwise)
         * @return Reference to self (for method chaining)
         *
         * In some implementations faster than calling
         * `rotate(angle, Vector3::xAxis())`, see subclasses for more
         * information.
         * @see @ref rotateXLocal()
         */
        AbstractBasicTranslationRotation3D<T>& rotateX(Math::Rad<T> angle) {
            doRotateX(angle);
            return *this;
        }

        /**
         * @brief Rotate object around X axis as a local transformation
         *
         * Similar to the above, except that the transformation is applied
         * before all others. In some implementations faster than calling
         * `rotateLocal(angle, Vector3::xAxis())`, see subclasses for more
         * information.
         */
        AbstractBasicTranslationRotation3D<T>& rotateXLocal(Math::Rad<T> angle) {
            doRotateXLocal(angle);
            return *this;
        }

        #ifdef MAGNUM_BUILD_DEPRECATED
        #ifdef __GNUC__
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wdeprecated-declarations"
        #elif defined(_MSC_VER)
        #pragma warning(push)
        #pragma warning(disable: 4996)
        #endif
        /**
         * @copybrief rotateX()
         * @deprecated Use @ref rotateX() or @ref rotateXLocal() instead.
         */
        CORRADE_DEPRECATED("use rotateX() or rotateXLocal() instead") AbstractBasicTranslationRotation3D<T>& rotateX(Math::Rad<T> angle, TransformationType type) {
            return type == TransformationType::Global ? rotateX(angle) : rotateXLocal(angle);
        }
        #ifdef __GNUC__
        #pragma GCC diagnostic pop
        #elif defined(_MSC_VER)
        #pragma warning(pop)
        #endif
        #endif

        /**
         * @brief Rotate object around Y axis
         * @param angle             Angle (counterclockwise)
         * @return Reference to self (for method chaining)
         *
         * In some implementations faster than calling
         * `rotate(angle, Vector3::yAxis())`, see subclasses for more
         * information.
         * @see @ref rotateYLocal()
         */
        AbstractBasicTranslationRotation3D<T>& rotateY(Math::Rad<T> angle) {
            doRotateY(angle);
            return *this;
        }

        /**
         * @brief Rotate object around Y axis as a local transformation
         *
         * Similar to the above, except that the transformation is applied
         * before all others. In some implementations faster than calling
         * `rotateLocal(angle, Vector3::yAxis())`, see subclasses for more
         * information.
         */
        AbstractBasicTranslationRotation3D<T>& rotateYLocal(Math::Rad<T> angle) {
            doRotateY(angle);
            return *this;
        }

        #ifdef MAGNUM_BUILD_DEPRECATED
        #ifdef __GNUC__
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wdeprecated-declarations"
        #elif defined(_MSC_VER)
        #pragma warning(push)
        #pragma warning(disable: 4996)
        #endif
        /**
         * @copybrief rotateY()
         * @deprecated Use @ref rotateY() or @ref rotateYLocal() instead.
         */
        CORRADE_DEPRECATED("use rotateY() or rotateYLocal() instead") AbstractBasicTranslationRotation3D<T>& rotateY(Math::Rad<T> angle, TransformationType type) {
            return type == TransformationType::Global ? rotateY(angle) : rotateYLocal(angle);
        }
        #ifdef __GNUC__
        #pragma GCC diagnostic pop
        #elif defined(_MSC_VER)
        #pragma warning(pop)
        #endif
        #endif

        /**
         * @brief Rotate object around Z axis
         * @param angle             Angle (counterclockwise)
         * @return Reference to self (for method chaining)
         *
         * In some implementations faster than calling
         * `rotate(angle, Vector3::zAxis())`, see subclasses for more
         * information.
         * @see @ref rotateZLocal()
         */
        AbstractBasicTranslationRotation3D<T>& rotateZ(Math::Rad<T> angle) {
            doRotateZ(angle);
            return *this;
        }

        /**
         * @brief Rotate object around Z axis as a local transformation
         *
         * Similar to the above, except that the transformation is applied
         * before all others. In some implementations faster than calling
         * `rotateLocal(angle, Vector3::zAxis())`, see subclasses for more
         * information.
         */
        AbstractBasicTranslationRotation3D<T>& rotateZLocal(Math::Rad<T> angle) {
            doRotateZLocal(angle);
            return *this;
        }

        #ifdef MAGNUM_BUILD_DEPRECATED
        #ifdef __GNUC__
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wdeprecated-declarations"
        #elif defined(_MSC_VER)
        #pragma warning(push)
        #pragma warning(disable: 4996)
        #endif
        /**
         * @copybrief rotateZ()
         * @deprecated Use @ref rotateZ() or @ref rotateZLocal() instead.
         */
        CORRADE_DEPRECATED("use rotateZ() or rotateZLocal() instead") AbstractBasicTranslationRotation3D<T>& rotateZ(Math::Rad<T> angle, TransformationType type) {
            return type == TransformationType::Global ? rotateZ(angle) : rotateZLocal(angle);
        }
        #ifdef __GNUC__
        #pragma GCC diagnostic pop
        #elif defined(_MSC_VER)
        #pragma warning(pop)
        #endif
        #endif

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
        #ifdef MAGNUM_BUILD_DEPRECATED
        #ifdef __GNUC__
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wdeprecated-declarations"
        #elif defined(_MSC_VER)
        #pragma warning(push)
        #pragma warning(disable: 4996)
        #endif
        CORRADE_DEPRECATED("use translate() or translateLocal() instead") AbstractBasicTranslationRotation3D<T>& translate(const Math::Vector3<T>& vector, TransformationType type) {
            AbstractBasicTranslation3D<T>::translate(vector, type);
            return *this;
        }
        #ifdef __GNUC__
        #pragma GCC diagnostic pop
        #elif defined(_MSC_VER)
        #pragma warning(pop)
        #endif
        #endif
        #endif

    protected:
        ~AbstractBasicTranslationRotation3D() = default;

    #ifdef DOXYGEN_GENERATING_OUTPUT
    protected:
    #else
    private:
    #endif
        /** @brief Polymorphic implementation for @ref rotate() */
        virtual void doRotate(Math::Rad<T> angle, const Math::Vector3<T>& normalizedAxis) = 0;

        /** @brief Polymorphic implementation for @ref rotateLocal() */
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
