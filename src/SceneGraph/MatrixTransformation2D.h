#ifndef Magnum_SceneGraph_MatrixTransformation2D_h
#define Magnum_SceneGraph_MatrixTransformation2D_h
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
 * @brief Class Magnum::SceneGraph::MatrixTransformation2D
 */

#include "Math/Matrix3.h"
#include "AbstractTranslationRotationScaling2D.h"
#include "Object.h"

namespace Magnum { namespace SceneGraph {

/**
@brief Two-dimensional transformation implemented using matrices

Uses Math::Matrix3 as underlying type.
@see @ref scenegraph, RigidMatrixTransformation2D, MatrixTransformation3D
*/
#ifndef DOXYGEN_GENERATING_OUTPUT
template<class T>
#else
template<class T = Float>
#endif
class MatrixTransformation2D: public AbstractTranslationRotationScaling2D<T> {
    public:
        /** @brief Underlying transformation type */
        typedef Math::Matrix3<T> DataType;

        #ifndef DOXYGEN_GENERATING_OUTPUT
        constexpr static Math::Matrix3<T> fromMatrix(const Math::Matrix3<T>& matrix) {
            return matrix;
        }

        constexpr static Math::Matrix3<T> toMatrix(const Math::Matrix3<T>& transformation) {
            return transformation;
        }

        static Math::Matrix3<T> compose(const Math::Matrix3<T>& parent, const Math::Matrix3<T>& child) {
            return parent*child;
        }

        static Math::Matrix3<T> inverted(const Math::Matrix3<T>& transformation) {
            return transformation.inverted();
        }

        Math::Matrix3<T> transformation() const {
            return _transformation;
        }
        #endif

        /**
         * @brief Set transformation
         * @return Pointer to self (for method chaining)
         */
        Object<MatrixTransformation2D<T>>* setTransformation(const Math::Matrix3<T>& transformation) {
            /* Setting transformation is forbidden for the scene */
            /** @todo Assert for this? */
            /** @todo Do this in some common code so we don't need to include Object? */
            if(!static_cast<Object<MatrixTransformation2D<T>>*>(this)->isScene()) {
                _transformation = transformation;
                static_cast<Object<MatrixTransformation2D<T>>*>(this)->setDirty();
            }

            return static_cast<Object<MatrixTransformation2D<T>>*>(this);
        }

        /**
         * @brief Transform object
         * @param transformation    Transformation
         * @param type              Transformation type
         * @return Pointer to self (for method chaining)
         */
        Object<MatrixTransformation2D<T>>* transform(const Math::Matrix3<T>& transformation, TransformationType type = TransformationType::Global) {
            setTransformation(type == TransformationType::Global ?
                transformation*_transformation : _transformation*transformation);
            return static_cast<Object<MatrixTransformation2D<T>>*>(this);
        }

        /** @copydoc AbstractTranslationRotationScaling2D::resetTransformation() */
        Object<MatrixTransformation2D<T>>* resetTransformation() {
            setTransformation({});
            return static_cast<Object<MatrixTransformation2D<T>>*>(this);
        }

        /**
         * @copydoc AbstractTranslationRotationScaling2D::translate()
         * Same as calling transform() with Matrix3::translation().
         */
        Object<MatrixTransformation2D<T>>* translate(const Math::Vector2<T>& vector, TransformationType type = TransformationType::Global) {
            transform(Math::Matrix3<T>::translation(vector), type);
            return static_cast<Object<MatrixTransformation2D<T>>*>(this);
        }

        /**
         * @copydoc AbstractTranslationRotationScaling2D::rotate()
         * Same as calling transform() with Matrix3::rotation().
         */
        Object<MatrixTransformation2D<T>>* rotate(Math::Rad<T> angle, TransformationType type = TransformationType::Global) {
            transform(Math::Matrix3<T>::rotation(angle), type);
            return static_cast<Object<MatrixTransformation2D<T>>*>(this);
        }

        /**
         * @copydoc AbstractTranslationRotationScaling2D::scale()
         * Same as calling transform() with Matrix3::scaling().
         */
        Object<MatrixTransformation2D<T>>* scale(const Math::Vector2<T>& vector, TransformationType type = TransformationType::Global) {
            transform(Math::Matrix3<T>::scaling(vector), type);
            return static_cast<Object<MatrixTransformation2D<T>>*>(this);
        }

        /**
         * @brief Reflect object
         * @param normal    Normal of the line through which to reflect
         *      (normalized)
         * @param type      Transformation type
         * @return Pointer to self (for method chaining)
         *
         * Same as calling transform() with Matrix3::reflection().
         */
        Object<MatrixTransformation2D<T>>* reflect(const Math::Vector2<T>& normal, TransformationType type = TransformationType::Global) {
            transform(Math::Matrix3<T>::reflection(normal), type);
            return static_cast<Object<MatrixTransformation2D<T>>*>(this);
        }

        /**
         * @brief Move object in stacking order
         * @param under     Sibling object under which to move or `nullptr`,
         *      if you want to move it above all.
         * @return Pointer to self (for method chaining)
         */
        Object<MatrixTransformation2D<T>>* move(Object<MatrixTransformation2D<T>>* under) {
            static_cast<Object<MatrixTransformation2D>*>(this)->Containers::template LinkedList<Object<MatrixTransformation2D<T>>>::move(this, under);
            return static_cast<Object<MatrixTransformation2D<T>>*>(this);
        }

    protected:
        /* Allow construction only from Object */
        explicit MatrixTransformation2D();

    private:
        void doResetTransformation() override final { resetTransformation(); }

        void doTranslate(const Math::Vector2<T>& vector, TransformationType type) override final {
            translate(vector, type);
        }

        void doRotate(Math::Rad<T> angle, TransformationType type) override final {
            doRotate(angle, type);
        }

        void doScale(const Math::Vector2<T>& vector, TransformationType type) override final {
            scale(vector, type);
        }

        Math::Matrix3<T> _transformation;
};

template<class T> inline MatrixTransformation2D<T>::MatrixTransformation2D() = default;

}}

#endif
