#ifndef Magnum_SceneGraph_MatrixTransformation3D_h
#define Magnum_SceneGraph_MatrixTransformation3D_h
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
 * @brief Class Magnum::SceneGraph::MatrixTransformation3D
 */

#include "Math/Matrix4.h"
#include "AbstractTranslationRotationScaling3D.h"
#include "Object.h"

namespace Magnum { namespace SceneGraph {

/**
@brief Three-dimensional transformation implemented using matrices

Uses Math::Matrix4 as underlying type.
@see @ref scenegraph, RigidMatrixTransformation3D, MatrixTransformation2D
*/
#ifndef DOXYGEN_GENERATING_OUTPUT
template<class T>
#else
template<class T = Float>
#endif
class MatrixTransformation3D: public AbstractTranslationRotationScaling3D<T> {
    public:
        /** @brief Transformation matrix type */
        typedef typename DimensionTraits<3, T>::MatrixType DataType;

        #ifndef DOXYGEN_GENERATING_OUTPUT
        inline constexpr static Math::Matrix4<T> fromMatrix(const Math::Matrix4<T>& matrix) {
            return matrix;
        }

        inline constexpr static Math::Matrix4<T> toMatrix(const Math::Matrix4<T>& transformation) {
            return transformation;
        }

        inline static Math::Matrix4<T> compose(const Math::Matrix4<T>& parent, const Math::Matrix4<T>& child) {
            return parent*child;
        }

        inline static Math::Matrix4<T> inverted(const Math::Matrix4<T>& transformation) {
            return transformation.inverted();
        }

        inline Math::Matrix4<T> transformation() const {
            return _transformation;
        }
        #endif

        /**
         * @brief Set transformation
         * @return Pointer to self (for method chaining)
         */
        MatrixTransformation3D<T>* setTransformation(const Math::Matrix4<T>& transformation) {
            /* Setting transformation is forbidden for the scene */
            /** @todo Assert for this? */
            /** @todo Do this in some common code so we don't need to include Object? */
            if(!static_cast<Object<MatrixTransformation3D<T>>*>(this)->isScene()) {
                _transformation = transformation;
                static_cast<Object<MatrixTransformation3D<T>>*>(this)->setDirty();
            }

            return this;
        }

        inline MatrixTransformation3D<T>* resetTransformation() override {
            setTransformation({});
            return this;
        }

        /**
         * @brief Multiply transformation
         * @param transformation    Transformation
         * @param type              Transformation type
         * @return Pointer to self (for method chaining)
         */
        inline MatrixTransformation3D<T>* transform(const Math::Matrix4<T>& transformation, TransformationType type = TransformationType::Global) {
            setTransformation(type == TransformationType::Global ?
                transformation*_transformation : _transformation*transformation);
            return this;
        }

        /**
         * @copydoc AbstractTranslationRotationScaling3D::translate()
         * Same as calling transform() with Matrix4::translation().
         */
        inline MatrixTransformation3D<T>* translate(const Math::Vector3<T>& vector, TransformationType type = TransformationType::Global) override {
            transform(Math::Matrix4<T>::translation(vector), type);
            return this;
        }

        /**
         * @copydoc AbstractTranslationRotationScaling3D::rotate()
         * Same as calling transform() with Matrix4::rotation().
         */
        inline MatrixTransformation3D<T>* rotate(Math::Rad<T> angle, const Math::Vector3<T>& normalizedAxis, TransformationType type = TransformationType::Global) override {
            transform(Math::Matrix4<T>::rotation(angle, normalizedAxis), type);
            return this;
        }

        /**
         * @brief Rotate object around X axis
         * @param angle             Angle (counterclockwise)
         * @param type              Transformation type
         * @return Pointer to self (for method chaining)
         *
         * Same as calling transform() with Matrix4::rotationX().
         */
        inline MatrixTransformation3D<T>* rotateX(Math::Rad<T> angle, TransformationType type = TransformationType::Global) override {
            transform(Math::Matrix4<T>::rotationX(angle), type);
            return this;
        }

        /**
         * @brief Rotate object around Y axis
         * @param angle             Angle (counterclockwise)
         * @param type              Transformation type
         * @return Pointer to self (for method chaining)
         *
         * Same as calling transform() with Matrix4::rotationY().
         */
        inline MatrixTransformation3D<T>* rotateY(Math::Rad<T> angle, TransformationType type = TransformationType::Global) override {
            transform(Math::Matrix4<T>::rotationY(angle), type);
            return this;
        }

        /**
         * @brief Rotate object around Z axis
         * @param angle             Angle (counterclockwise)
         * @param type              Transformation type
         * @return Pointer to self (for method chaining)
         *
         * Same as calling transform() with Matrix4::rotationZ().
         */
        inline MatrixTransformation3D<T>* rotateZ(Math::Rad<T> angle, TransformationType type = TransformationType::Global) override {
            transform(Math::Matrix4<T>::rotationZ(angle), type);
            return this;
        }

        /**
         * @copydoc AbstractTranslationRotationScaling3D::scale()
         * Same as calling transform() with Matrix4::scaling().
         */
        inline MatrixTransformation3D<T>* scale(const Math::Vector3<T>& vector, TransformationType type = TransformationType::Global) override {
            transform(Math::Matrix4<T>::scaling(vector), type);
            return this;
        }

        /**
         * @brief Reflect object
         * @param normal    Normal of the plane through which to reflect
         *      (normalized)
         * @param type      Transformation type
         * @return Pointer to self (for method chaining)
         *
         * Same as calling transform() with Matrix4::reflection().
         */
        inline MatrixTransformation3D<T>* reflect(const Math::Vector3<T>& normal, TransformationType type = TransformationType::Global) {
            transform(Math::Matrix4<T>::reflection(normal), type);
            return this;
        }

    protected:
        /* Allow construction only from Object */
        inline explicit MatrixTransformation3D() = default;

    private:
        Math::Matrix4<T> _transformation;
};

}}

#endif
