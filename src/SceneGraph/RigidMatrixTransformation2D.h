#ifndef Magnum_SceneGraph_RigidMatrixTransformation2D_h
#define Magnum_SceneGraph_RigidMatrixTransformation2D_h
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
 * @brief Class Magnum::SceneGraph::BasicRigidMatrixTransformation2D, typedef Magnum::SceneGraph::RigidMatrixTransformation2D
 */

#include "Math/Matrix3.h"
#include "Math/Algorithms/GramSchmidt.h"
#include "AbstractTranslationRotation2D.h"
#include "Object.h"

namespace Magnum { namespace SceneGraph {

/**
@brief Two-dimensional rigid transformation implemented using matrices

Unlike BasicMatrixTransformation2D this class allows only rotation, reflection
and translation (no scaling or setting arbitrary transformations). This allows
to use Matrix3::invertedRigid() for faster computation of inverse
transformations.
@see @ref RigidMatrixTransformation2D, @ref scenegraph, @ref BasicRigidMatrixTransformation3D
*/
template<class T> class BasicRigidMatrixTransformation2D: public AbstractBasicTranslationRotation2D<T> {
    public:
        /** @brief Underlying transformation type */
        typedef Math::Matrix3<T> DataType;

        /** @brief Object transformation */
        Math::Matrix3<T> transformation() const { return _transformation; }

        /**
         * @brief Set transformation
         * @return Reference to self (for method chaining)
         *
         * Expects that the matrix represents rigid transformation.
         * @see Matrix3::isRigidTransformation()
         */
        Object<BasicRigidMatrixTransformation2D<T>>& setTransformation(const Math::Matrix3<T>& transformation) {
            CORRADE_ASSERT(transformation.isRigidTransformation(),
                "SceneGraph::RigidMatrixTransformation2D::setTransformation(): the matrix doesn't represent rigid transformation",
                static_cast<Object<BasicRigidMatrixTransformation2D<T>>&>(*this));
            setTransformationInternal(transformation);
            return static_cast<Object<BasicRigidMatrixTransformation2D<T>>&>(*this);
        }

        /** @copydoc AbstractTranslationRotationScaling2D::resetTransformation() */
        Object<BasicRigidMatrixTransformation2D<T>>& resetTransformation() {
            setTransformationInternal({});
            return static_cast<Object<BasicRigidMatrixTransformation2D<T>>&>(*this);
        }

        /**
         * @brief Normalize rotation part
         * @return Reference to self (for method chaining)
         *
         * Normalizes the rotation part using Math::Algorithms::gramSchmidt()
         * to prevent rounding errors when rotating the object subsequently.
         */
        Object<BasicRigidMatrixTransformation2D<T>>& normalizeRotation() {
            setTransformationInternal(Math::Matrix3<T>::from(
                Math::Algorithms::gramSchmidtOrthonormalize(_transformation.rotationScaling()),
                _transformation.translation()));
            return static_cast<Object<BasicRigidMatrixTransformation2D<T>>&>(*this);
        }

        /**
         * @brief Transform object
         * @param transformation    Transformation
         * @param type              Transformation type
         * @return Reference to self (for method chaining)
         *
         * Expects that the matrix represents rigid transformation.
         * @see Matrix3::isRigidTransformation()
         */
        Object<BasicRigidMatrixTransformation2D<T>>& transform(const Math::Matrix3<T>& transformation, TransformationType type = TransformationType::Global) {
            CORRADE_ASSERT(transformation.isRigidTransformation(),
                "SceneGraph::RigidMatrixTransformation2D::transform(): the matrix doesn't represent rigid transformation",
                static_cast<Object<BasicRigidMatrixTransformation2D<T>>&>(*this));
            transformInternal(transformation, type);
            return static_cast<Object<BasicRigidMatrixTransformation2D<T>>&>(*this);
        }

        /**
         * @copydoc AbstractTranslationRotationScaling2D::translate()
         * Same as calling transform() with Matrix3::translation().
         */
        Object<BasicRigidMatrixTransformation2D<T>>& translate(const Math::Vector2<T>& vector, TransformationType type = TransformationType::Global) {
            transformInternal(Math::Matrix3<T>::translation(vector), type);
            return static_cast<Object<BasicRigidMatrixTransformation2D<T>>&>(*this);
        }

        /**
         * @brief Rotate object
         * @param angle     Angle (counterclockwise)
         * @param type      Transformation type
         * @return Reference to self (for method chaining)
         *
         * Same as calling transform() with Matrix3::rotation().
         * @see normalizeRotation()
         */
        Object<BasicRigidMatrixTransformation2D<T>>& rotate(Math::Rad<T> angle, TransformationType type = TransformationType::Global) {
            transformInternal(Math::Matrix3<T>::rotation(angle), type);
            return static_cast<Object<BasicRigidMatrixTransformation2D<T>>&>(*this);
        }

        /**
         * @brief Reflect object
         * @param normal    Normal of the line through which to reflect
         *      (normalized)
         * @param type      Transformation type
         * @return Reference to self (for method chaining)
         *
         * Same as calling transform() with Matrix3::reflection().
         */
        Object<BasicRigidMatrixTransformation2D<T>>& reflect(const Math::Vector2<T>& normal, TransformationType type = TransformationType::Global) {
            transformInternal(Math::Matrix3<T>::reflection(normal), type);
            return static_cast<Object<BasicRigidMatrixTransformation2D<T>>&>(*this);
        }

        /**
         * @brief Move object in stacking order
         * @param under     Sibling object under which to move or `nullptr`,
         *      if you want to move it above all.
         * @return Reference to self (for method chaining)
         */
        Object<BasicRigidMatrixTransformation2D<T>>& move(Object<BasicRigidMatrixTransformation2D<T>>* under) {
            static_cast<Object<BasicRigidMatrixTransformation2D>*>(this)->Containers::template LinkedList<Object<BasicRigidMatrixTransformation2D<T>>>::move(this, under);
            return static_cast<Object<BasicRigidMatrixTransformation2D<T>>&>(*this);
        }

    protected:
        /* Allow construction only from Object */
        explicit BasicRigidMatrixTransformation2D() = default;

    private:
        void doResetTransformation() override final { resetTransformation(); }

        void doTranslate(const Math::Vector2<T>& vector, TransformationType type) override final {
            translate(vector, type);
        }

        void doRotate(Math::Rad<T> angle, TransformationType type) override final {
            doRotate(angle, type);
        }

        /* No assertions fired, for internal use */
        void setTransformationInternal(const Math::Matrix3<T>& transformation) {
            /* Setting transformation is forbidden for the scene */
            /** @todo Assert for this? */
            /** @todo Do this in some common code so we don't need to include Object? */
            if(!static_cast<Object<BasicRigidMatrixTransformation2D<T>>*>(this)->isScene()) {
                _transformation = transformation;
                static_cast<Object<BasicRigidMatrixTransformation2D<T>>*>(this)->setDirty();
            }
        }

        /* No assertions fired, for internal use */
        void transformInternal(const Math::Matrix3<T>& transformation, TransformationType type) {
            setTransformation(type == TransformationType::Global ?
                transformation*_transformation : _transformation*transformation);
        }

        Math::Matrix3<T> _transformation;
};

/**
@brief Two-dimensional rigid transformation for float scenes implemented using matrices

@see @ref RigidMatrixTransformation3D
*/
typedef BasicRigidMatrixTransformation2D<Float> RigidMatrixTransformation2D;

namespace Implementation {

template<class T> struct Transformation<BasicRigidMatrixTransformation2D<T>> {
    static Math::Matrix3<T> fromMatrix(const Math::Matrix3<T>& matrix) {
        CORRADE_ASSERT(matrix.isRigidTransformation(),
            "SceneGraph::RigidMatrixTransformation2D: the matrix doesn't represent rigid transformation", {});
        return matrix;
    }

    constexpr static Math::Matrix3<T> toMatrix(const Math::Matrix3<T>& transformation) {
        return transformation;
    }

    static Math::Matrix3<T> compose(const Math::Matrix3<T>& parent, const Math::Matrix3<T>& child) {
        return parent*child;
    }

    static Math::Matrix3<T> inverted(const Math::Matrix3<T>& transformation) {
        return transformation.invertedRigid();
    }
};

}

}}

#endif
