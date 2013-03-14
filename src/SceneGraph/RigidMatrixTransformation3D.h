#ifndef Magnum_SceneGraph_RigidMatrixTransformation3D_h
#define Magnum_SceneGraph_RigidMatrixTransformation3D_h
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
 * @brief Class Magnum::SceneGraph::RigidMatrixTransformation3D
 */

#include "Math/Matrix4.h"
#include "Math/Algorithms/GramSchmidt.h"
#include "AbstractTranslationRotation3D.h"
#include "Object.h"

namespace Magnum { namespace SceneGraph {

/**
@brief Three-dimensional rigid transformation implemented using matrices

Unlike MatrixTransformation3D this class allows only rotation, reflection and
translation (no scaling or setting arbitrary transformations). This allows to
use Matrix4::invertedRigid() for faster computation of inverse transformations.
@see @ref scenegraph, RigidMatrixTransformation2D
*/
#ifndef DOXYGEN_GENERATING_OUTPUT
template<class T>
#else
template<class T = Float>
#endif
class RigidMatrixTransformation3D: public AbstractTranslationRotation3D<T> {
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
            return transformation.invertedRigid();
        }

        inline Math::Matrix4<T> transformation() const {
            return _transformation;
        }
        #endif

        /**
         * @brief Reset transformation to default
         * @return Pointer to self (for method chaining)
         */
        inline RigidMatrixTransformation3D<T>* resetTransformation() {
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
        RigidMatrixTransformation3D<T>* normalizeRotation() {
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
        inline RigidMatrixTransformation3D<T>* translate(const Math::Vector3<T>& vector, TransformationType type = TransformationType::Global) override {
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
        inline RigidMatrixTransformation3D<T>* rotate(Math::Rad<T> angle, const Math::Vector3<T>& normalizedAxis, TransformationType type = TransformationType::Global) override {
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
        inline RigidMatrixTransformation3D<T>* rotateX(Math::Rad<T> angle, TransformationType type = TransformationType::Global) override {
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
        inline RigidMatrixTransformation3D<T>* rotateY(Math::Rad<T> angle, TransformationType type = TransformationType::Global) override {
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
        inline RigidMatrixTransformation3D<T>* rotateZ(Math::Rad<T> angle, TransformationType type = TransformationType::Global) override {
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
        inline RigidMatrixTransformation3D<T>* reflect(const Math::Vector3<T>& normal, TransformationType type = TransformationType::Global) {
            transform(Math::Matrix4<T>::reflection(normal), type);
            return this;
        }

    protected:
        /* Allow construction only from Object */
        inline explicit RigidMatrixTransformation3D() = default;

    private:
        inline void setTransformation(const Math::Matrix4<T>& transformation) {
            /* Setting transformation is forbidden for the scene */
            /** @todo Assert for this? */
            /** @todo Do this in some common code so we don't need to include Object? */
            if(!static_cast<Object<RigidMatrixTransformation3D<T>>*>(this)->isScene()) {
                _transformation = transformation;
                static_cast<Object<RigidMatrixTransformation3D<T>>*>(this)->setDirty();
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
