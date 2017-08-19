#ifndef Magnum_SceneGraph_RigidMatrixTransformation3D_h
#define Magnum_SceneGraph_RigidMatrixTransformation3D_h
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
 * @brief Class @ref Magnum::SceneGraph::BasicRigidMatrixTransformation3D, typedef @ref Magnum::SceneGraph::RigidMatrixTransformation3D
 */

#include "Magnum/Math/Matrix4.h"
#include "Magnum/Math/Algorithms/GramSchmidt.h"
#include "Magnum/SceneGraph/AbstractTranslationRotation3D.h"
#include "Magnum/SceneGraph/Object.h"

namespace Magnum { namespace SceneGraph {

/**
@brief Three-dimensional rigid transformation implemented using matrices

Unlike @ref BasicMatrixTransformation3D this class allows only rotation,
reflection and translation (no scaling or setting arbitrary transformations).
This allows to use @ref Math::Matrix4::invertedRigid() for faster computation
of inverse transformations.
@see @ref scenegraph, @ref RigidMatrixTransformation3D,
    @ref BasicRigidMatrixTransformation2D
*/
template<class T> class BasicRigidMatrixTransformation3D: public AbstractBasicTranslationRotation3D<T> {
    public:
        /** @brief Underlying transformation type */
        typedef Math::Matrix4<T> DataType;

        /** @brief Object transformation */
        Math::Matrix4<T> transformation() const { return _transformation; }

        /**
         * @brief Set transformation
         * @return Reference to self (for method chaining)
         *
         * Expects that the matrix represents rigid transformation.
         * @see @ref Matrix4::isRigidTransformation()
         */
        Object<BasicRigidMatrixTransformation3D<T>>& setTransformation(const Math::Matrix4<T>& transformation) {
            CORRADE_ASSERT(transformation.isRigidTransformation(),
                "SceneGraph::RigidMatrixTransformation3D::setTransformation(): the matrix doesn't represent rigid transformation",
                static_cast<Object<BasicRigidMatrixTransformation3D<T>>&>(*this));
            return setTransformationInternal(transformation);
        }

        /** @copydoc AbstractTranslationRotationScaling3D::resetTransformation() */
        Object<BasicRigidMatrixTransformation3D<T>>& resetTransformation() {
            return setTransformationInternal({});
        }

        /**
         * @brief Normalize rotation part
         * @return Reference to self (for method chaining)
         *
         * Normalizes the rotation part using @ref Math::Algorithms::gramSchmidtOrthonormalize()
         * to prevent rounding errors when rotating the object subsequently.
         */
        Object<BasicRigidMatrixTransformation3D<T>>& normalizeRotation() {
            return setTransformationInternal(Math::Matrix4<T>::from(
                Math::Algorithms::gramSchmidtOrthonormalize(_transformation.rotationScaling()),
                _transformation.translation()));
        }

        /**
         * @brief Transform object
         * @return Reference to self (for method chaining)
         *
         * Expects that the matrix represents rigid transformation.
         * @see @ref transformLocal(), @ref Math::Matrix4::isRigidTransformation()
         */
        Object<BasicRigidMatrixTransformation3D<T>>& transform(const Math::Matrix4<T>& transformation) {
            CORRADE_ASSERT(transformation.isRigidTransformation(),
                "SceneGraph::RigidMatrixTransformation3D::transform(): the matrix doesn't represent rigid transformation",
                static_cast<Object<BasicRigidMatrixTransformation3D<T>>&>(*this));
            return transformInternal(transformation);
        }

        /**
         * @brief Transform object as a local transformation
         *
         * Similar to the above, except that the transformation is applied
         * before all others.
         */
        Object<BasicRigidMatrixTransformation3D<T>>& transformLocal(const Math::Matrix4<T>& transformation) {
            CORRADE_ASSERT(transformation.isRigidTransformation(),
                "SceneGraph::RigidMatrixTransformation3D::transform(): the matrix doesn't represent rigid transformation",
                static_cast<Object<BasicRigidMatrixTransformation3D<T>>&>(*this));
            return transformLocalInternal(transformation);
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
         * @copybrief transform()
         * @deprecated Use @ref transform() or @ref transformLocal() instead.
         */
        CORRADE_DEPRECATED("use transform() or transformLocal() instead") Object<BasicRigidMatrixTransformation3D<T>>& transform(const Math::Matrix4<T>& transformation, TransformationType type) {
            return type == TransformationType::Global ? transform(transformation) : transformLocal(transformation);
        }
        #ifdef __GNUC__
        #pragma GCC diagnostic pop
        #elif defined(_MSC_VER)
        #pragma warning(pop)
        #endif
        #endif

        /**
         * @brief Translate object
         * @return Reference to self (for method chaining)
         *
         * Same as calling @ref transform() with @ref Math::Matrix4::translation().
         * @see @ref translateLocal(), @ref Math::Vector3::xAxis(),
         *      @ref Math::Vector3::yAxis(), @ref Math::Vector3::zAxis()
         */
        Object<BasicRigidMatrixTransformation3D<T>>& translate(const Math::Vector3<T>& vector) {
            return transformInternal(Math::Matrix4<T>::translation(vector));
        }

        /**
         * @brief Translate object as a local transformation
         *
         * Similar to the above, except that the transformation is applied
         * before all others. Same as calling @ref transformLocal() with
         * @ref Math::Matrix4::translation().
         */
        Object<BasicRigidMatrixTransformation3D<T>>& translateLocal(const Math::Vector3<T>& vector) {
            return transformLocalInternal(Math::Matrix4<T>::translation(vector));
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
         * @copybrief translate()
         * @deprecated Use @ref translate() or @ref translateLocal() instead.
         */
        CORRADE_DEPRECATED("use translate() or translateLocal() instead") Object<BasicRigidMatrixTransformation3D<T>>& translate(const Math::Vector3<T>& vector, TransformationType type) {
            return type == TransformationType::Global ? translate(vector) : translateLocal(vector);
        }
        #ifdef __GNUC__
        #pragma GCC diagnostic pop
        #elif defined(_MSC_VER)
        #pragma warning(pop)
        #endif
        #endif

        /**
         * @brief Rotate object
         * @param angle             Angle (counterclockwise)
         * @param normalizedAxis    Normalized rotation axis
         * @return Reference to self (for method chaining)
         *
         * Same as calling @ref transform() with @ref Math::Matrix4::rotation().
         * @see @ref rotateLocal(), @ref rotateX(), @ref rotateY(),
         *      @ref rotateZ(), @ref normalizeRotation(),
         *      @ref Math::Vector3::xAxis(), @ref Math::Vector3::yAxis(),
         *      @ref Math::Vector3::zAxis()
         */
        Object<BasicRigidMatrixTransformation3D<T>>& rotate(Math::Rad<T> angle, const Math::Vector3<T>& normalizedAxis) {
            return transformInternal(Math::Matrix4<T>::rotation(angle, normalizedAxis));
        }

        /**
         * @brief Rotate object as a local transformation
         *
         * Similar to the above, except that the transformation is applied
         * before all others. Same as calling @ref transformLocal() with
         * @ref Math::Matrix4::rotation().
         */
        Object<BasicRigidMatrixTransformation3D<T>>& rotateLocal(Math::Rad<T> angle, const Math::Vector3<T>& normalizedAxis) {
            return transformLocalInternal(Math::Matrix4<T>::rotation(angle, normalizedAxis));
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
        CORRADE_DEPRECATED("use rotate() or rotateLocal() instead") Object<BasicRigidMatrixTransformation3D<T>>& rotate(Math::Rad<T> angle, const Math::Vector3<T>& normalizedAxis, TransformationType type) {
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
         * Same as calling @ref transform() with @ref Math::Matrix4::rotationX().
         * @see @ref rotateXLocal(), @ref normalizeRotation()
         */
        Object<BasicRigidMatrixTransformation3D<T>>& rotateX(Math::Rad<T> angle) {
            return transformInternal(Math::Matrix4<T>::rotationX(angle));
        }

        /**
         * @brief Rotate object around X axis as a local transformation
         *
         * Similar to the above, except that the transformation is applied
         * before all others. Same as calling @ref transformLocal() with
         * @ref Math::Matrix4::rotationX().
         */
        Object<BasicRigidMatrixTransformation3D<T>>& rotateXLocal(Math::Rad<T> angle) {
            return transformLocalInternal(Math::Matrix4<T>::rotationX(angle));
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
        CORRADE_DEPRECATED("use rotateX() or rotateXLocal() instead") Object<BasicRigidMatrixTransformation3D<T>>& rotateX(Math::Rad<T> angle, TransformationType type) {
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
         * Same as calling @ref transform() with @ref Math::Matrix4::rotationY().
         * @see @ref rotateYLocal(), @ref normalizeRotation()
         */
        Object<BasicRigidMatrixTransformation3D<T>>& rotateY(Math::Rad<T> angle) {
            return transformInternal(Math::Matrix4<T>::rotationY(angle));
        }

        /**
         * @brief Rotate object around Y axis as a local transformation
         *
         * Similar to the above, except that the transformation is applied
         * before all others. Same as calling @ref transformLocal() with
         * @ref Math::Matrix4::rotationY().
         */
        Object<BasicRigidMatrixTransformation3D<T>>& rotateYLocal(Math::Rad<T> angle) {
            return transformLocalInternal(Math::Matrix4<T>::rotationY(angle));
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
        CORRADE_DEPRECATED("use rotateY() or rotateYLocal() instead") Object<BasicRigidMatrixTransformation3D<T>>& rotateY(Math::Rad<T> angle, TransformationType type) {
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
         * Same as calling @ref transform() with @ref Math::Matrix4::rotationZ().
         * @see @ref rotateZLocal(), @ref normalizeRotation()
         */
        Object<BasicRigidMatrixTransformation3D<T>>& rotateZ(Math::Rad<T> angle) {
            return transformInternal(Math::Matrix4<T>::rotationZ(angle));
        }

        /**
         * @brief Rotate object around Z axis as a local transformation
         *
         * Similar to the above, except that the transformation is applied
         * before all others. Same as calling @ref transformLocal() with
         * @ref Math::Matrix4::rotationZ().
         */
        Object<BasicRigidMatrixTransformation3D<T>>& rotateZLocal(Math::Rad<T> angle) {
            return transformLocalInternal(Math::Matrix4<T>::rotationZ(angle));
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
        CORRADE_DEPRECATED("use rotateZ() or rotateZLocal() instead") Object<BasicRigidMatrixTransformation3D<T>>& rotateZ(Math::Rad<T> angle, TransformationType type) {
            return type == TransformationType::Global ? rotateZ(angle) : rotateZLocal(angle);
        }
        #ifdef __GNUC__
        #pragma GCC diagnostic pop
        #elif defined(_MSC_VER)
        #pragma warning(pop)
        #endif
        #endif

        /**
         * @brief Reflect object
         * @param normal    Normal of the plane through which to reflect
         *      (normalized)
         * @return Reference to self (for method chaining)
         *
         * Same as calling @ref transform() with @ref Math::Matrix4::reflection().
         * @see @ref reflectLocal()
         */
        Object<BasicRigidMatrixTransformation3D<T>>& reflect(const Math::Vector3<T>& normal) {
            return transformInternal(Math::Matrix4<T>::reflection(normal));
        }

        /**
         * @brief Reflect object as a local transformation
         *
         * Similar to the above, except that the transformation is applied
         * before all others. Same as calling @ref transformLocal() with
         * @ref Math::Matrix4::reflection().
         */
        Object<BasicRigidMatrixTransformation3D<T>>& reflectLocal(const Math::Vector3<T>& normal) {
            return transformLocalInternal(Math::Matrix4<T>::reflection(normal));
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
         * @copybrief reflect()
         * @deprecated Use @ref reflect() or @ref reflectLocal() instead.
         */
        CORRADE_DEPRECATED("use reflect() or reflectLocal() instead") Object<BasicRigidMatrixTransformation3D<T>>& reflect(const Math::Vector3<T>& normal, TransformationType type) {
            return type == TransformationType::Global ? reflect(normal) : reflectLocal(normal);
        }
        #ifdef __GNUC__
        #pragma GCC diagnostic pop
        #elif defined(_MSC_VER)
        #pragma warning(pop)
        #endif
        #endif

    protected:
        /* Allow construction only from Object */
        explicit BasicRigidMatrixTransformation3D() = default;

    private:
        void doResetTransformation() override final { resetTransformation(); }

        void doTranslate(const Math::Vector3<T>& vector) override final { translate(vector); }
        void doTranslateLocal(const Math::Vector3<T>& vector) override final { translateLocal(vector); }

        void doRotate(Math::Rad<T> angle, const Math::Vector3<T>& normalizedAxis) override final {
            rotate(angle, normalizedAxis);
        }
        void doRotateLocal(Math::Rad<T> angle, const Math::Vector3<T>& normalizedAxis) override final {
            rotateLocal(angle, normalizedAxis);
        }

        void doRotateX(Math::Rad<T> angle) override final { rotateX(angle); }
        void doRotateXLocal(Math::Rad<T> angle) override final { rotateXLocal(angle); }

        void doRotateY(Math::Rad<T> angle) override final { rotateY(angle); }
        void doRotateYLocal(Math::Rad<T> angle) override final { rotateYLocal(angle); }

        void doRotateZ(Math::Rad<T> angle) override final { rotateZ(angle); }
        void doRotateZLocal(Math::Rad<T> angle) override final { rotateZLocal(angle); }

        /* No assertions fired, for internal use */
        Object<BasicRigidMatrixTransformation3D<T>>& setTransformationInternal(const Math::Matrix4<T>& transformation) {
            /* Setting transformation is forbidden for the scene */
            /** @todo Assert for this? */
            /** @todo Do this in some common code so we don't need to include Object? */
            if(!static_cast<Object<BasicRigidMatrixTransformation3D<T>>*>(this)->isScene()) {
                _transformation = transformation;
                static_cast<Object<BasicRigidMatrixTransformation3D<T>>*>(this)->setDirty();
            }

            return static_cast<Object<BasicRigidMatrixTransformation3D<T>>&>(*this);
        }

        /* No assertions fired, for internal use */
        Object<BasicRigidMatrixTransformation3D<T>>& transformInternal(const Math::Matrix4<T>& transformation) {
            return setTransformationInternal(transformation*_transformation);
        }
        Object<BasicRigidMatrixTransformation3D<T>>& transformLocalInternal(const Math::Matrix4<T>& transformation) {
            return setTransformationInternal(_transformation*transformation);
        }

        Math::Matrix4<T> _transformation;
};

/**
@brief Three-dimensional rigid transformation for float scenes implemented using matrices

@see @ref RigidMatrixTransformation2D
*/
typedef BasicRigidMatrixTransformation3D<Float> RigidMatrixTransformation3D;

namespace Implementation {

template<class T> struct Transformation<BasicRigidMatrixTransformation3D<T>> {
    static Math::Matrix4<T> fromMatrix(const Math::Matrix4<T>& matrix) {
        CORRADE_ASSERT(matrix.isRigidTransformation(),
            "SceneGraph::RigidMatrixTransformation3D: the matrix doesn't represent rigid transformation", {});
        return matrix;
    }

    constexpr static Math::Matrix4<T> toMatrix(const Math::Matrix4<T>& transformation) {
        return transformation;
    }

    static Math::Matrix4<T> compose(const Math::Matrix4<T>& parent, const Math::Matrix4<T>& child) {
        return parent*child;
    }

    static Math::Matrix4<T> inverted(const Math::Matrix4<T>& transformation) {
        return transformation.invertedRigid();
    }
};

}

#if defined(CORRADE_TARGET_WINDOWS) && !defined(__MINGW32__)
extern template class MAGNUM_SCENEGRAPH_EXPORT Object<BasicRigidMatrixTransformation3D<Float>>;
#endif

}}

#endif
