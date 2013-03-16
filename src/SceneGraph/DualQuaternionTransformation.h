#ifndef Magnum_SceneGraph_DualQuaternionTransformation_h
#define Magnum_SceneGraph_DualQuaternionTransformation_h
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
 * @brief Class Magnum::SceneGraph::DualQuaternionTransformation
 */

#include "Math/DualQuaternion.h"
#include "AbstractTranslationRotation3D.h"
#include "Object.h"

namespace Magnum { namespace SceneGraph {

/**
@brief Three-dimensional transformation implemented using dual quaternions

This class allows only rigid transformation (i.e. only rotation and
translation).
@see @ref scenegraph, Math::DualQuaternion, DualComplexTransformation
*/
#ifndef DOXYGEN_GENERATING_OUTPUT
template<class T>
#else
template<class T = Float>
#endif
class DualQuaternionTransformation: public AbstractTranslationRotation3D<T> {
    public:
        /** @brief Underlying transformation type */
        typedef Math::DualQuaternion<T> DataType;

        #ifndef DOXYGEN_GENERATING_OUTPUT
        inline static Math::DualQuaternion<T> fromMatrix(const Math::Matrix4<T>& matrix) {
            CORRADE_ASSERT(matrix.isRigidTransformation(),
                "SceneGraph::DualQuaternionTransformation::fromMatrix(): the matrix doesn't represent rigid transformation", {});
            return Math::DualQuaternion<T>::fromMatrix(matrix);
        }

        inline constexpr static Math::Matrix4<T> toMatrix(const Math::DualQuaternion<T>& transformation) {
            return transformation.toMatrix();
        }

        inline static Math::DualQuaternion<T> compose(const Math::DualQuaternion<T>& parent, const Math::DualQuaternion<T>& child) {
            return parent*child;
        }

        inline static Math::DualQuaternion<T> inverted(const Math::DualQuaternion<T>& transformation) {
            return transformation.invertedNormalized();
        }

        inline Math::DualQuaternion<T> transformation() const {
            return _transformation;
        }
        #endif

        /**
         * @brief Normalize rotation part
         * @return Pointer to self (for method chaining)
         *
         * Normalizes the rotation part to prevent rounding errors when rotating
         * the object subsequently.
         * @see DualQuaternion::normalized()
         */
        DualQuaternionTransformation<T>* normalizeRotation() {
            setTransformation(_transformation.normalized());
            return this;
        }

        /**
         * @brief Set transformation
         * @return Pointer to self (for method chaining)
         *
         * Expects that the dual quaternion is normalized.
         * @see DualQuaternion::isNormalized()
         */
        DualQuaternionTransformation<T>* setTransformation(const Math::DualQuaternion<T>& transformation) {
            CORRADE_ASSERT(transformation.isNormalized(),
                "SceneGraph::DualQuaternionTransformation::setTransformation(): the dual quaternion is not normalized", this);
            setTransformationInternal(transformation);
            return this;
        }

        inline DualQuaternionTransformation<T>* resetTransformation() override {
            setTransformation({});
            return this;
        }

        /**
         * @brief Multiply transformation
         * @param transformation    Transformation
         * @param type              Transformation type
         * @return Pointer to self (for method chaining)
         *
         * Expects that the dual quaternion is normalized.
         * @see DualQuaternion::isNormalized()
         */
        inline DualQuaternionTransformation<T>* transform(const Math::DualQuaternion<T>& transformation, TransformationType type = TransformationType::Global) {
            CORRADE_ASSERT(transformation.isNormalized(),
                "SceneGraph::DualQuaternionTransformation::transform(): the dual quaternion is not normalized", this);
            transformInternal(transformation, type);
            return this;
        }

        /**
         * @copydoc AbstractTranslationRotationScaling3D::translate()
         * Same as calling transform() with DualQuaternion::translation().
         */
        inline DualQuaternionTransformation<T>* translate(const Math::Vector3<T>& vector, TransformationType type = TransformationType::Global) override {
            transformInternal(Math::DualQuaternion<T>::translation(vector), type);
            return this;
        }

        /**
         * @brief Rotate object
         * @param angle             Angle (counterclockwise)
         * @param normalizedAxis    Normalized rotation axis
         * @param type              Transformation type
         * @return Pointer to self (for method chaining)
         *
         * Same as calling transform() with DualQuaternion::rotation().
         * @see Vector3::xAxis(), Vector3::yAxis(), Vector3::zAxis(),
         *      normalizeRotation()
         */
        inline DualQuaternionTransformation<T>* rotate(Math::Rad<T> angle, const Math::Vector3<T>& normalizedAxis, TransformationType type = TransformationType::Global) override {
            transformInternal(Math::DualQuaternion<T>::rotation(angle, normalizedAxis), type);
            return this;
        }

    protected:
        /* Allow construction only from Object */
        inline explicit DualQuaternionTransformation() = default;

    private:
        /* No assertions fired, for internal use */
        inline void setTransformationInternal(const Math::DualQuaternion<T>& transformation) {
            /* Setting transformation is forbidden for the scene */
            /** @todo Assert for this? */
            /** @todo Do this in some common code so we don't need to include Object? */
            if(!static_cast<Object<DualQuaternionTransformation<T>>*>(this)->isScene()) {
                _transformation = transformation;
                static_cast<Object<DualQuaternionTransformation<T>>*>(this)->setDirty();
            }
        }

        /* No assertions fired, for internal use */
        inline void transformInternal(const Math::DualQuaternion<T>& transformation, TransformationType type) {
            setTransformation(type == TransformationType::Global ?
                transformation*_transformation : _transformation*transformation);
        }

        Math::DualQuaternion<T> _transformation;
};

}}

#endif
