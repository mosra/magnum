#ifndef Magnum_SceneGraph_EuclideanMatrixTransformation3D_h
#define Magnum_SceneGraph_EuclideanMatrixTransformation3D_h
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
 * @brief Class Magnum::SceneGraph::EuclideanMatrixTransformation3D
 */

#include "Math/Matrix4.h"
#include "Math/Algorithms/GramSchmidt.h"
#include "AbstractTranslationRotation3D.h"
#include "Object.h"

namespace Magnum { namespace SceneGraph {

/**
@brief Three-dimensional euclidean transformation implemented using matrices

Unlike MatrixTransformation3D this class allows only rotation, reflection and
translation (no scaling or setting arbitrary transformations). This allows to
use Matrix4::invertedEuclidean() for faster computation of inverse
transformations.
@see @ref scenegraph, EuclideanMatrixTransformation2D
*/
#ifndef DOXYGEN_GENERATING_OUTPUT
template<class T>
#else
template<class T = GLfloat>
#endif
class EuclideanMatrixTransformation3D: public AbstractTranslationRotation3D<T> {
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
            return transformation.invertedEuclidean();
        }

        inline Math::Matrix4<T> transformation() const {
            return _transformation;
        }
        #endif

        /**
         * @brief Reset transformation to default
         * @return Pointer to self (for method chaining)
         */
        inline EuclideanMatrixTransformation3D<T>* resetTransformation() {
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
        EuclideanMatrixTransformation3D<T>* normalizeRotation() {
            setTransformation(Math::Matrix4<T>::from(
                Math::Algorithms::gramSchmidtOrthonormalize(_transformation.rotationScaling()),
                _transformation.translation()));
            return this;
        }

        /**
         * @brief Translate object
         * @param vector            Translation vector
         * @param type              Transformation type
         * @return Pointer to self (for method chaining)
         *
         * @see Vector3::xAxis(), Vector3::yAxis(), Vector3::zAxis(),
         *      Matrix4::translation()
         */
        inline EuclideanMatrixTransformation3D<T>* translate(const Math::Vector3<T>& vector, TransformationType type = TransformationType::Global) override {
            transform(Math::Matrix4<T>::translation(vector), type);
            return this;
        }

        /**
         * @brief Rotate object
         * @param angle             Angle (counterclockwise)
         * @param normalizedAxis    Normalized rotation axis
         * @param type              Transformation type
         * @return Pointer to self (for method chaining)
         *
         * @see rotateX(), rotateY(), rotateZ(), Vector3::xAxis(),
         *      Vector3::yAxis(), Vector3::zAxis(), normalizeRotation(),
         *      Matrix4::rotation()
         */
        inline EuclideanMatrixTransformation3D<T>* rotate(Math::Rad<T> angle, const Math::Vector3<T>& normalizedAxis, TransformationType type = TransformationType::Global) override {
            transform(Math::Matrix4<T>::rotation(angle, normalizedAxis), type);
            return this;
        }

        /**
         * @brief Rotate object around X axis
         * @param angle             Angle (counterclockwise)
         * @param type              Transformation type
         * @return Pointer to self (for method chaining)
         *
         * @see normalizeRotation(), Matrix4::rotationX()
         */
        inline EuclideanMatrixTransformation3D<T>* rotateX(Math::Rad<T> angle, TransformationType type = TransformationType::Global) override {
            transform(Math::Matrix4<T>::rotationX(angle), type);
            return this;
        }

        /**
         * @brief Rotate object around Y axis
         * @param angle             Angle (counterclockwise)
         * @param type              Transformation type
         * @return Pointer to self (for method chaining)
         *
         * @see normalizeRotation(), Matrix4::rotationY()
         */
        inline EuclideanMatrixTransformation3D<T>* rotateY(Math::Rad<T> angle, TransformationType type = TransformationType::Global) override {
            transform(Math::Matrix4<T>::rotationY(angle), type);
            return this;
        }

        /**
         * @brief Rotate object around Z axis
         * @param angle             Angle (counterclockwise)
         * @param type              Transformation type
         * @return Pointer to self (for method chaining)
         *
         * @see normalizeRotation(), Matrix4::rotationZ()
         */
        inline EuclideanMatrixTransformation3D<T>* rotateZ(Math::Rad<T> angle, TransformationType type = TransformationType::Global) override {
            transform(Math::Matrix4<T>::rotationZ(angle), type);
            return this;
        }

        /**
         * @brief Reflect object
         * @param normal    Normal of the plane through which to reflect
         *      (normalized)
         * @param type      Transformation type
         * @return Pointer to self (for method chaining)
         *
         * @see Matrix4::reflection()
         */
        inline EuclideanMatrixTransformation3D<T>* reflect(const Math::Vector3<T>& normal, TransformationType type = TransformationType::Global) {
            transform(Math::Matrix4<T>::reflection(normal), type);
            return this;
        }

    protected:
        /* Allow construction only from Object */
        inline explicit EuclideanMatrixTransformation3D() = default;

    private:
        inline void setTransformation(const Math::Matrix4<T>& transformation) {
            /* Setting transformation is forbidden for the scene */
            /** @todo Assert for this? */
            /** @todo Do this in some common code so we don't need to include Object? */
            if(!static_cast<Object<EuclideanMatrixTransformation3D<T>>*>(this)->isScene()) {
                _transformation = transformation;
                static_cast<Object<EuclideanMatrixTransformation3D<T>>*>(this)->setDirty();
            }
        }

        inline void transform(const Math::Matrix4<T>& transformation, TransformationType type) {
            setTransformation(type == TransformationType::Global ?
                transformation*_transformation : _transformation*transformation);
        }

        Math::Matrix4<T> _transformation;
};

}}

#endif
