#ifndef Magnum_SceneGraph_MatrixTransformation2D_h
#define Magnum_SceneGraph_MatrixTransformation2D_h
/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
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
@see @ref scenegraph, MatrixTransformation3D
*/
#ifndef DOXYGEN_GENERATING_OUTPUT
template<class T>
#else
template<class T = GLfloat>
#endif
class MatrixTransformation2D: public AbstractTranslationRotationScaling2D<T> {
    public:
        /** @brief Transformation matrix type */
        typedef typename DimensionTraits<2, T>::MatrixType DataType;

        #ifndef DOXYGEN_GENERATING_OUTPUT
        inline constexpr static Math::Matrix3<T> fromMatrix(const Math::Matrix3<T>& matrix) {
            return matrix;
        }

        inline constexpr static Math::Matrix3<T> toMatrix(const Math::Matrix3<T>& transformation) {
            return transformation;
        }

        inline static Math::Matrix3<T> compose(const Math::Matrix3<T>& parent, const Math::Matrix3<T>& child) {
            return parent*child;
        }

        inline static Math::Matrix3<T> inverted(const Math::Matrix3<T>& transformation) {
            return transformation.inverted();
        }

        inline Math::Matrix3<T> transformation() const {
            return _transformation;
        }
        #endif

        /**
         * @brief Set transformation
         * @return Pointer to self (for method chaining)
         */
        MatrixTransformation2D<T>* setTransformation(const Math::Matrix3<T>& transformation) {
            /* Setting transformation is forbidden for the scene */
            /** @todo Assert for this? */
            /** @todo Do this in some common code so we don't need to include Object? */
            if(!static_cast<Object<MatrixTransformation2D<T>>*>(this)->isScene()) {
                _transformation = transformation;
                static_cast<Object<MatrixTransformation2D<T>>*>(this)->setDirty();
            }

            return this;
        }

        /**
         * @brief Transform object
         * @param transformation    Transformation
         * @param type              Transformation type
         * @return Pointer to self (for method chaining)
         */
        inline MatrixTransformation2D<T>* transform(const Math::Matrix3<T>& transformation, TransformationType type = TransformationType::Global) {
            setTransformation(type == TransformationType::Global ?
                transformation*_transformation : _transformation*transformation);
            return this;
        }

        /**
         * @copydoc AbstractTranslationRotationScaling2D::translate()
         * Same as calling transform() with Matrix3::translation().
         */
        inline MatrixTransformation2D<T>* translate(const Math::Vector2<T>& vector, TransformationType type = TransformationType::Global) override {
            transform(Math::Matrix3<T>::translation(vector), type);
            return this;
        }

        /**
         * @copydoc AbstractTranslationRotationScaling2D::rotate()
         * Same as calling transform() with Matrix3::rotation().
         */
        inline MatrixTransformation2D<T>* rotate(T angle, TransformationType type = TransformationType::Global) override {
            transform(Math::Matrix3<T>::rotation(angle), type);
            return this;
        }

        /**
         * @copydoc AbstractTranslationRotationScaling2D::scale()
         * Same as calling transform() with Matrix3::scaling().
         */
        inline MatrixTransformation2D<T>* scale(const Math::Vector2<T>& vector, TransformationType type = TransformationType::Global) override {
            transform(Math::Matrix3<T>::scaling(vector), type);
            return this;
        }

        /**
         * @brief Move object in stacking order
         * @param under     Sibling object under which to move or `nullptr`,
         *      if you want to move it above all.
         * @return Pointer to self (for method chaining)
         */
        inline MatrixTransformation2D<T>* move(Object<MatrixTransformation2D<T>>* under) {
            static_cast<Object<MatrixTransformation2D>*>(this)->Corrade::Containers::LinkedList<Object<MatrixTransformation2D<T>>>::move(this, under);
            return this;
        }

    protected:
        /* Allow construction only from Object */
        inline MatrixTransformation2D() {}

    private:
        Math::Matrix3<T> _transformation;
};

}}

#endif
