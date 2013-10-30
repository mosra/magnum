#ifndef Magnum_SceneGraph_DualComplexTransformation_h
#define Magnum_SceneGraph_DualComplexTransformation_h
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
 * @brief Class Magnum::SceneGraph::BasicDualComplexTransformation, typedef Magnum::SceneGraph::DualComplexTransformation
 */

#include "Math/DualComplex.h"
#include "AbstractTranslationRotation2D.h"
#include "Object.h"

namespace Magnum { namespace SceneGraph {

/**
@brief Two-dimensional transformation implemented using dual complex numbers

This class allows only rigid transformation (i.e. only rotation and
translation).
@see @ref DualComplexTransformation, @ref scenegraph, Math::DualComplex, @ref BasicDualQuaternionTransformation
*/
template<class T> class BasicDualComplexTransformation: public AbstractBasicTranslationRotation2D<T> {
    public:
        /** @brief Transformation type */
        typedef Math::DualComplex<T> DataType;

        /** @brief Object transformation */
        Math::DualComplex<T> transformation() const { return _transformation; }

        /**
         * @brief Set transformation
         * @return Reference to self (for method chaining)
         *
         * Expects that the dual complex number is normalized.
         * @see DualComplex::isNormalized()
         */
        Object<BasicDualComplexTransformation<T>>& setTransformation(const Math::DualComplex<T>& transformation) {
            CORRADE_ASSERT(transformation.isNormalized(),
                "SceneGraph::DualComplexTransformation::setTransformation(): the dual complex number is not normalized",
                static_cast<Object<BasicDualComplexTransformation<T>>&>(*this));
            return setTransformationInternal(transformation);
        }

        /** @copydoc AbstractTranslationRotationScaling2D::resetTransformation() */
        Object<BasicDualComplexTransformation<T>>& resetTransformation() {
            return setTransformationInternal({});
        }

        /**
         * @brief Normalize rotation part
         * @return Reference to self (for method chaining)
         *
         * Normalizes the rotation part to prevent rounding errors when rotating
         * the object subsequently.
         * @see DualComplex::normalized()
         */
        Object<BasicDualComplexTransformation<T>>& normalizeRotation() {
            return setTransformationInternal(_transformation.normalized());
        }

        /**
         * @brief Transform object
         * @param transformation    Transformation
         * @param type              Transformation type
         * @return Reference to self (for method chaining)
         *
         * Expects that the dual complex number is normalized.
         * @see DualComplex::isNormalized()
         */
        Object<BasicDualComplexTransformation<T>>& transform(const Math::DualComplex<T>& transformation, TransformationType type = TransformationType::Global) {
            CORRADE_ASSERT(transformation.isNormalized(),
                "SceneGraph::DualComplexTransformation::transform(): the dual complex number is not normalized",
                static_cast<Object<BasicDualComplexTransformation<T>>&>(*this));
            return transformInternal(transformation, type);
        }

        /**
         * @copydoc AbstractTranslationRotationScaling2D::translate()
         * Same as calling transform() with DualComplex::translation().
         */
        Object<BasicDualComplexTransformation<T>>& translate(const Math::Vector2<T>& vector, TransformationType type = TransformationType::Global) {
            return transformInternal(Math::DualComplex<T>::translation(vector), type);
        }

        /**
         * @brief Rotate object
         * @param angle     Angle (counterclockwise)
         * @param type      Transformation type
         * @return Reference to self (for method chaining)
         *
         * Same as calling transform() with DualComplex::rotation().
         * @see normalizeRotation()
         */
        Object<BasicDualComplexTransformation<T>>& rotate(Math::Rad<T> angle, TransformationType type = TransformationType::Global) {
            return transformInternal(Math::DualComplex<T>::rotation(angle), type);
        }

        /**
         * @brief Move object in stacking order
         * @param under     Sibling object under which to move or `nullptr`,
         *      if you want to move it above all.
         * @return Reference to self (for method chaining)
         */
        Object<BasicDualComplexTransformation<T>>& move(Object<BasicDualComplexTransformation<T>>* under) {
            static_cast<Object<BasicDualComplexTransformation>*>(this)->Containers::template LinkedList<Object<BasicDualComplexTransformation<T>>>::move(this, under);
            return static_cast<Object<BasicDualComplexTransformation<T>>&>(*this);
        }

    protected:
        /* Allow construction only from Object */
        explicit BasicDualComplexTransformation();

    private:
        void doResetTransformation() override final { resetTransformation(); }

        void doTranslate(const Math::Vector2<T>& vector, TransformationType type) override final {
            translate(vector, type);
        }

        void doRotate(Math::Rad<T> angle, TransformationType type) override final {
            doRotate(angle, type);
        }

        /* No assertions fired, for internal use */
        Object<BasicDualComplexTransformation<T>>& setTransformationInternal(const Math::DualComplex<T>& transformation) {
            /* Setting transformation is forbidden for the scene */
            /** @todo Assert for this? */
            /** @todo Do this in some common code so we don't need to include Object? */
            if(!static_cast<Object<BasicDualComplexTransformation<T>>*>(this)->isScene()) {
                _transformation = transformation;
                static_cast<Object<BasicDualComplexTransformation<T>>*>(this)->setDirty();
            }

            return static_cast<Object<BasicDualComplexTransformation<T>>&>(*this);
        }

        /* No assertions fired, for internal use */
        Object<BasicDualComplexTransformation<T>>& transformInternal(const Math::DualComplex<T>& transformation, TransformationType type) {
            return setTransformationInternal(type == TransformationType::Global ?
                transformation*_transformation : _transformation*transformation);
        }

        Math::DualComplex<T> _transformation;
};

template<class T> inline BasicDualComplexTransformation<T>::BasicDualComplexTransformation() = default;

/**
@brief Two-dimensional transformation for float scenes implemented using dual complex numbers

@see @ref DualQuaternionTransformation
*/
typedef BasicDualComplexTransformation<Float> DualComplexTransformation;

namespace Implementation {

template<class T> struct Transformation<BasicDualComplexTransformation<T>> {
    static Math::DualComplex<T> fromMatrix(const Math::Matrix3<T>& matrix) {
        return Math::DualComplex<T>::fromMatrix(matrix);
    }

    constexpr static Math::Matrix3<T> toMatrix(const Math::DualComplex<T>& transformation) {
        return transformation.toMatrix();
    }

    static Math::DualComplex<T> compose(const Math::DualComplex<T>& parent, const Math::DualComplex<T>& child) {
        return parent*child;
    }

    static Math::DualComplex<T> inverted(const Math::DualComplex<T>& transformation) {
        return transformation.invertedNormalized();
    }
};

}

}}

#endif
