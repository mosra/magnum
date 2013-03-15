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
 * @brief Class Magnum::SceneGraph::RigidMatrixTransformation2D
 */

#include "Math/Matrix3.h"
#include "Math/Algorithms/GramSchmidt.h"
#include "AbstractTranslationRotation2D.h"
#include "Object.h"

namespace Magnum { namespace SceneGraph {

/**
@brief Two-dimensional rigid transformation implemented using matrices

Unlike MatrixTransformation2D this class allows only rotation, reflection and
translation (no scaling or setting arbitrary transformations). This allows to
use Matrix3::invertedRigid() for faster computation of inverse transformations.
@see @ref scenegraph, RigidMatrixTransformation3D
*/
#ifndef DOXYGEN_GENERATING_OUTPUT
template<class T>
#else
template<class T = Float>
#endif
class RigidMatrixTransformation2D: public AbstractTranslationRotation2D<T> {
    public:
        /** @brief Transformation matrix type */
        typedef typename DimensionTraits<2, T>::MatrixType DataType;

        #ifndef DOXYGEN_GENERATING_OUTPUT
        inline static Math::Matrix3<T> fromMatrix(const Math::Matrix3<T>& matrix) {
            CORRADE_ASSERT(matrix.isRigidTransformation(),
                "SceneGraph::RigidMatrixTransformation2D::fromMatrix(): the matrix doesn't represent rigid transformation", {});
            return matrix;
        }

        inline constexpr static Math::Matrix3<T> toMatrix(const Math::Matrix3<T>& transformation) {
            return transformation;
        }

        inline static Math::Matrix3<T> compose(const Math::Matrix3<T>& parent, const Math::Matrix3<T>& child) {
            return parent*child;
        }

        inline static Math::Matrix3<T> inverted(const Math::Matrix3<T>& transformation) {
            return transformation.invertedRigid();
        }

        inline Math::Matrix3<T> transformation() const {
            return _transformation;
        }
        #endif

        /**
         * @brief Normalize rotation part
         * @return Pointer to self (for method chaining)
         *
         * Normalizes the rotation part using Math::Algorithms::gramSchmidt()
         * to prevent rounding errors when rotating the object subsequently.
         */
        RigidMatrixTransformation2D<T>* normalizeRotation() {
            setTransformation(Math::Matrix3<T>::from(
                Math::Algorithms::gramSchmidtOrthonormalize(_transformation.rotationScaling()),
                _transformation.translation()));
            return this;
        }

        inline RigidMatrixTransformation2D<T>* resetTransformation() override {
            setTransformation({});
            return this;
        }

        /** @copydoc AbstractTranslationRotation2D::translate() */
        inline RigidMatrixTransformation2D<T>* translate(const Math::Vector2<T>& vector, TransformationType type = TransformationType::Global) override {
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
        inline RigidMatrixTransformation2D<T>* rotate(Math::Rad<T> angle, TransformationType type = TransformationType::Global) override {
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
        inline RigidMatrixTransformation2D<T>* reflect(const Math::Vector2<T>& normal, TransformationType type = TransformationType::Global) {
            transform(Math::Matrix3<T>::reflection(normal), type);
            return this;
        }

        /**
         * @brief Move object in stacking order
         * @param under     Sibling object under which to move or `nullptr`,
         *      if you want to move it above all.
         * @return Pointer to self (for method chaining)
         */
        inline RigidMatrixTransformation2D<T>* move(Object<RigidMatrixTransformation2D<T>>* under) {
            static_cast<Object<RigidMatrixTransformation2D>*>(this)->Corrade::Containers::template LinkedList<Object<RigidMatrixTransformation2D<T>>>::move(this, under);
            return this;
        }

    protected:
        /* Allow construction only from Object */
        inline explicit RigidMatrixTransformation2D() = default;

    private:
        inline void setTransformation(const Math::Matrix3<T>& transformation) {
            /* Setting transformation is forbidden for the scene */
            /** @todo Assert for this? */
            /** @todo Do this in some common code so we don't need to include Object? */
            if(!static_cast<Object<RigidMatrixTransformation2D<T>>*>(this)->isScene()) {
                _transformation = transformation;
                static_cast<Object<RigidMatrixTransformation2D<T>>*>(this)->setDirty();
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
