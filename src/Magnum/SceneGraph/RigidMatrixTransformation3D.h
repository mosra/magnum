#ifndef Magnum_SceneGraph_RigidMatrixTransformation3D_h
#define Magnum_SceneGraph_RigidMatrixTransformation3D_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014
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
@see @ref RigidMatrixTransformation3D, @ref scenegraph, @ref BasicRigidMatrixTransformation2D
*/
template<class T> class BasicRigidMatrixTransformation3D: public AbstractBasicTranslationRotation3D<T> {
    public:
        /** @brief Underlying transformation type */
        typedef Math::Matrix4<T> DataType;

        /** @brief %Object transformation */
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
         * @brief Multiply transformation
         * @param transformation    Transformation
         * @param type              Transformation type
         * @return Reference to self (for method chaining)
         *
         * Expects that the matrix represents rigid transformation.
         * @see @ref Math::Matrix4::isRigidTransformation()
         */
        Object<BasicRigidMatrixTransformation3D<T>>& transform(const Math::Matrix4<T>& transformation, TransformationType type = TransformationType::Global) {
            CORRADE_ASSERT(transformation.isRigidTransformation(),
                "SceneGraph::RigidMatrixTransformation3D::transform(): the matrix doesn't represent rigid transformation",
                static_cast<Object<BasicRigidMatrixTransformation3D<T>>&>(*this));
            return transformInternal(transformation, type);
        }

        /**
         * @copydoc AbstractTranslationRotationScaling3D::translate()
         * Same as calling @ref transform() with @ref Math::Matrix4::translation().
         */
        Object<BasicRigidMatrixTransformation3D<T>>& translate(const Math::Vector3<T>& vector, TransformationType type = TransformationType::Global) {
            return transformInternal(Math::Matrix4<T>::translation(vector), type);
        }

        /**
         * @brief Rotate object
         * @param angle             Angle (counterclockwise)
         * @param normalizedAxis    Normalized rotation axis
         * @param type              Transformation type
         * @return Reference to self (for method chaining)
         *
         * Same as calling @ref transform() with @ref Math::Matrix4::rotation().
         * @see @ref rotateX(), @ref rotateY(), @ref rotateZ(),
         *      @ref normalizeRotation(), @ref Math::Vector3::xAxis(),
         *      @ref Math::Vector3::yAxis(), @ref Math::Vector3::zAxis()
         */
        Object<BasicRigidMatrixTransformation3D<T>>& rotate(Math::Rad<T> angle, const Math::Vector3<T>& normalizedAxis, TransformationType type = TransformationType::Global) {
            return transformInternal(Math::Matrix4<T>::rotation(angle, normalizedAxis), type);
        }

        /**
         * @brief Rotate object around X axis
         * @param angle             Angle (counterclockwise)
         * @param type              Transformation type
         * @return Reference to self (for method chaining)
         *
         * Same as calling @ref transform() with
         * @ref Math::Matrix4::rotationX().
         * @see @ref normalizeRotation()
         */
        Object<BasicRigidMatrixTransformation3D<T>>& rotateX(Math::Rad<T> angle, TransformationType type = TransformationType::Global) {
            return transformInternal(Math::Matrix4<T>::rotationX(angle), type);
        }

        /**
         * @brief Rotate object around Y axis
         * @param angle             Angle (counterclockwise)
         * @param type              Transformation type
         * @return Reference to self (for method chaining)
         *
         * Same as calling @ref transform() with
         * @ref Math::Matrix4::rotationY().
         * @see @ref normalizeRotation()
         */
        Object<BasicRigidMatrixTransformation3D<T>>& rotateY(Math::Rad<T> angle, TransformationType type = TransformationType::Global) {
            return transformInternal(Math::Matrix4<T>::rotationY(angle), type);
        }

        /**
         * @brief Rotate object around Z axis
         * @param angle             Angle (counterclockwise)
         * @param type              Transformation type
         * @return Reference to self (for method chaining)
         *
         * Same as calling @ref transform() with
         * @ref Math::Matrix4::rotationZ().
         * @see @ref normalizeRotation()
         */
        Object<BasicRigidMatrixTransformation3D<T>>& rotateZ(Math::Rad<T> angle, TransformationType type = TransformationType::Global) {
            return transformInternal(Math::Matrix4<T>::rotationZ(angle), type);
        }

        /**
         * @brief Reflect object
         * @param normal    Normal of the plane through which to reflect
         *      (normalized)
         * @param type      Transformation type
         * @return Reference to self (for method chaining)
         *
         * Same as calling @ref transform() with
         * @ref Math::Matrix4::reflection().
         */
        Object<BasicRigidMatrixTransformation3D<T>>& reflect(const Math::Vector3<T>& normal, TransformationType type = TransformationType::Global) {
            return transformInternal(Math::Matrix4<T>::reflection(normal), type);
        }

    protected:
        /* Allow construction only from Object */
        explicit BasicRigidMatrixTransformation3D() = default;

    private:
        void doResetTransformation() override final { resetTransformation(); }

        void doTranslate(const Math::Vector3<T>& vector, TransformationType type) override final {
            translate(vector, type);
        }

        void doRotate(Math::Rad<T> angle, const Math::Vector3<T>& normalizedAxis, TransformationType type) override final {
            rotate(angle, normalizedAxis, type);
        }

        void doRotateX(Math::Rad<T> angle, TransformationType type) override final {
            rotateX(angle, type);
        }

        void doRotateY(Math::Rad<T> angle, TransformationType type) override final {
            rotateY(angle, type);
        }

        void doRotateZ(Math::Rad<T> angle, TransformationType type) override final {
            rotateZ(angle, type);
        }

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
        Object<BasicRigidMatrixTransformation3D<T>>& transformInternal(const Math::Matrix4<T>& transformation, TransformationType type) {
            return setTransformationInternal(type == TransformationType::Global ?
                transformation*_transformation : _transformation*transformation);
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

#ifdef CORRADE_TARGET_WINDOWS
extern template class MAGNUM_SCENEGRAPH_EXPORT Object<BasicRigidMatrixTransformation3D<Float>>;
#endif

}}

#endif
