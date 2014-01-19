#ifndef Magnum_SceneGraph_MatrixTransformation3D_h
#define Magnum_SceneGraph_MatrixTransformation3D_h
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
 * @brief Class Magnum::SceneGraph::BasicMatrixTransformation3D, typedef Magnum::SceneGraph::MatrixTransformation3D
 */

#include "Magnum/Math/Matrix4.h"
#include "Magnum/SceneGraph/AbstractTranslationRotationScaling3D.h"
#include "Magnum/SceneGraph/Object.h"

namespace Magnum { namespace SceneGraph {

/**
@brief Three-dimensional transformation implemented using matrices

Uses Math::Matrix4 as underlying type.
@see @ref MatrixTransformation3D, @ref scenegraph, @ref BasicRigidMatrixTransformation3D, @ref BasicMatrixTransformation2D
*/
template<class T> class BasicMatrixTransformation3D: public AbstractBasicTranslationRotationScaling3D<T> {
    public:
        /** @brief Underlying transformation type */
        typedef Math::Matrix4<T> DataType;

        /** @brief Object transformation */
        Math::Matrix4<T> transformation() const { return _transformation; }

        /**
         * @brief Set transformation
         * @return Reference to self (for method chaining)
         */
        Object<BasicMatrixTransformation3D<T>>& setTransformation(const Math::Matrix4<T>& transformation) {
            /* Setting transformation is forbidden for the scene */
            /** @todo Assert for this? */
            /** @todo Do this in some common code so we don't need to include Object? */
            if(!static_cast<Object<BasicMatrixTransformation3D<T>>*>(this)->isScene()) {
                _transformation = transformation;
                static_cast<Object<BasicMatrixTransformation3D<T>>*>(this)->setDirty();
            }

            return static_cast<Object<BasicMatrixTransformation3D<T>>&>(*this);
        }

        /** @copydoc AbstractTranslationRotationScaling3D::resetTransformation() */
        Object<BasicMatrixTransformation3D<T>>& resetTransformation() {
            return setTransformation({});
        }

        /**
         * @brief Multiply transformation
         * @param transformation    Transformation
         * @param type              Transformation type
         * @return Reference to self (for method chaining)
         */
        Object<BasicMatrixTransformation3D<T>>& transform(const Math::Matrix4<T>& transformation, TransformationType type = TransformationType::Global) {
            return setTransformation(type == TransformationType::Global ?
                transformation*_transformation : _transformation*transformation);
        }

        /**
         * @copydoc AbstractTranslationRotationScaling3D::translate()
         * Same as calling transform() with Matrix4::translation().
         */
        Object<BasicMatrixTransformation3D<T>>& translate(const Math::Vector3<T>& vector, TransformationType type = TransformationType::Global) {
            return transform(Math::Matrix4<T>::translation(vector), type);
        }

        /**
         * @copydoc AbstractTranslationRotationScaling3D::rotate()
         * Same as calling transform() with Matrix4::rotation().
         */
        Object<BasicMatrixTransformation3D<T>>& rotate(Math::Rad<T> angle, const Math::Vector3<T>& normalizedAxis, TransformationType type = TransformationType::Global) {
            return transform(Math::Matrix4<T>::rotation(angle, normalizedAxis), type);
        }

        /**
         * @brief Rotate object around X axis
         * @param angle             Angle (counterclockwise)
         * @param type              Transformation type
         * @return Reference to self (for method chaining)
         *
         * Same as calling transform() with Matrix4::rotationX().
         */
        Object<BasicMatrixTransformation3D<T>>& rotateX(Math::Rad<T> angle, TransformationType type = TransformationType::Global) {
            return transform(Math::Matrix4<T>::rotationX(angle), type);
        }

        /**
         * @brief Rotate object around Y axis
         * @param angle             Angle (counterclockwise)
         * @param type              Transformation type
         * @return Reference to self (for method chaining)
         *
         * Same as calling transform() with Matrix4::rotationY().
         */
        Object<BasicMatrixTransformation3D<T>>& rotateY(Math::Rad<T> angle, TransformationType type = TransformationType::Global) {
            return transform(Math::Matrix4<T>::rotationY(angle), type);
        }

        /**
         * @brief Rotate object around Z axis
         * @param angle             Angle (counterclockwise)
         * @param type              Transformation type
         * @return Reference to self (for method chaining)
         *
         * Same as calling transform() with Matrix4::rotationZ().
         */
        Object<BasicMatrixTransformation3D<T>>& rotateZ(Math::Rad<T> angle, TransformationType type = TransformationType::Global) {
            return transform(Math::Matrix4<T>::rotationZ(angle), type);
        }

        /**
         * @copydoc AbstractTranslationRotationScaling3D::scale()
         * Same as calling transform() with Matrix4::scaling().
         */
        Object<BasicMatrixTransformation3D<T>>& scale(const Math::Vector3<T>& vector, TransformationType type = TransformationType::Global) {
            return transform(Math::Matrix4<T>::scaling(vector), type);
        }

        /**
         * @brief Reflect object
         * @param normal    Normal of the plane through which to reflect
         *      (normalized)
         * @param type      Transformation type
         * @return Reference to self (for method chaining)
         *
         * Same as calling transform() with Matrix4::reflection().
         */
        Object<BasicMatrixTransformation3D<T>>& reflect(const Math::Vector3<T>& normal, TransformationType type = TransformationType::Global) {
            return transform(Math::Matrix4<T>::reflection(normal), type);
        }

    protected:
        /* Allow construction only from Object */
        explicit BasicMatrixTransformation3D() = default;

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

        void doScale(const Math::Vector3<T>& vector, TransformationType type) override final {
            scale(vector, type);
        }

        Math::Matrix4<T> _transformation;
};

/**
@brief Three-dimensional transformation for float scenes implemented using matrices

@see @ref MatrixTransformation2D
*/
typedef BasicMatrixTransformation3D<Float> MatrixTransformation3D;

namespace Implementation {

template<class T> struct Transformation<BasicMatrixTransformation3D<T>> {
    constexpr static Math::Matrix4<T> fromMatrix(const Math::Matrix4<T>& matrix) {
        return matrix;
    }

    constexpr static Math::Matrix4<T> toMatrix(const Math::Matrix4<T>& transformation) {
        return transformation;
    }

    static Math::Matrix4<T> compose(const Math::Matrix4<T>& parent, const Math::Matrix4<T>& child) {
        return parent*child;
    }

    static Math::Matrix4<T> inverted(const Math::Matrix4<T>& transformation) {
        return transformation.inverted();
    }
};

}

#ifdef CORRADE_TARGET_WINDOWS
extern template class MAGNUM_SCENEGRAPH_EXPORT Object<BasicMatrixTransformation3D<Float>>;
#endif

}}

#endif
