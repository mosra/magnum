#ifndef Magnum_SceneGraph_EuclideanMatrixTransformation2D_h
#define Magnum_SceneGraph_EuclideanMatrixTransformation2D_h
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
 * @brief Class Magnum::SceneGraph::EuclideanMatrixTransformation2D
 */

#include "Math/Matrix3.h"
#include "Math/Algorithms/GramSchmidt.h"
#include "AbstractTranslationRotation2D.h"
#include "Object.h"

namespace Magnum { namespace SceneGraph {

/**
@brief Two-dimensional euclidean transformation implemented using matrices

Unlike MatrixTransformation2D this class allows only rotation, reflection and
translation (no scaling or setting arbitrary transformations). This allows to
use Matrix3::invertedEuclidean() for faster computation of inverse
transformations.
@see @ref scenegraph, EuclideanMatrixTransformation3D
*/
#ifndef DOXYGEN_GENERATING_OUTPUT
template<class T>
#else
template<class T = GLfloat>
#endif
class EuclideanMatrixTransformation2D: public AbstractTranslationRotation2D<T> {
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
            return transformation.invertedEuclidean();
        }

        inline Math::Matrix3<T> transformation() const {
            return _transformation;
        }
        #endif

        /**
         * @brief Reset transformation to default
         * @return Pointer to self (for method chaining)
         */
        inline EuclideanMatrixTransformation2D<T>* resetTransformation() {
            setTransformation({});
            return this;
        }

        /**
         * @brief Normalize rotation part
         * @return Pointer to self (for method chaining)
         *
         * Normalizes the rotation part using Math::Algorithms::gramSchmidt()
         * to prevent rounding errors when rotating the object subsequently.
         */
        EuclideanMatrixTransformation2D<T>* normalizeRotation() {
            setTransformation(Math::Matrix3<T>::from(
                Math::Algorithms::gramSchmidtOrthonormalize(_transformation.rotationScaling()),
                _transformation.translation()));
            return this;
        }

        /** @copydoc AbstractTranslationRotation2D::translate() */
        inline EuclideanMatrixTransformation2D<T>* translate(const Math::Vector2<T>& vector, TransformationType type = TransformationType::Global) override {
            transform(Math::Matrix3<T>::translation(vector), type);
            return this;
        }

        /**
         * @brief Rotate object
         * @param angle     Angle (counterclockwise)
         * @param type      Transformation type
         * @return Pointer to self (for method chaining)
         *
         * @see normalizeRotation(), Matrix3::rotation()
         */
        inline EuclideanMatrixTransformation2D<T>* rotate(Math::Rad<T> angle, TransformationType type = TransformationType::Global) override {
            transform(Math::Matrix3<T>::rotation(angle), type);
            return this;
        }

        /**
         * @brief Reflect object
         * @param normal    Normal of the line through which to reflect
         *      (normalized)
         * @param type      Transformation type
         * @return Pointer to self (for method chaining)
         *
         * @see Matrix3::reflection()
         */
        inline EuclideanMatrixTransformation2D<T>* reflect(const Math::Vector2<T>& normal, TransformationType type = TransformationType::Global) {
            transform(Math::Matrix3<T>::reflection(normal), type);
            return this;
        }

        /**
         * @brief Move object in stacking order
         * @param under     Sibling object under which to move or `nullptr`,
         *      if you want to move it above all.
         * @return Pointer to self (for method chaining)
         */
        inline EuclideanMatrixTransformation2D<T>* move(Object<EuclideanMatrixTransformation2D<T>>* under) {
            static_cast<Object<EuclideanMatrixTransformation2D>*>(this)->Corrade::Containers::template LinkedList<Object<EuclideanMatrixTransformation2D<T>>>::move(this, under);
            return this;
        }

    protected:
        /* Allow construction only from Object */
        inline explicit EuclideanMatrixTransformation2D() = default;

    private:
        inline void setTransformation(const Math::Matrix3<T>& transformation) {
            /* Setting transformation is forbidden for the scene */
            /** @todo Assert for this? */
            /** @todo Do this in some common code so we don't need to include Object? */
            if(!static_cast<Object<EuclideanMatrixTransformation2D<T>>*>(this)->isScene()) {
                _transformation = transformation;
                static_cast<Object<EuclideanMatrixTransformation2D<T>>*>(this)->setDirty();
            }
        }

        inline void transform(const Math::Matrix3<T>& transformation, TransformationType type) {
            setTransformation(type == TransformationType::Global ?
                transformation*_transformation : _transformation*transformation);
        }

        Math::Matrix3<T> _transformation;
};

}}

#endif
