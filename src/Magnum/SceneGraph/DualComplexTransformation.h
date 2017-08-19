#ifndef Magnum_SceneGraph_DualComplexTransformation_h
#define Magnum_SceneGraph_DualComplexTransformation_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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
 * @brief Class @ref Magnum::SceneGraph::BasicDualComplexTransformation, typedef @ref Magnum::SceneGraph::DualComplexTransformation
 */

#include "Magnum/Math/DualComplex.h"
#include "Magnum/SceneGraph/AbstractTranslationRotation2D.h"
#include "Magnum/SceneGraph/Object.h"

namespace Magnum { namespace SceneGraph {

/**
@brief Two-dimensional transformation implemented using dual complex numbers

This class allows only rigid transformation (i.e. only rotation and
translation). Uses @ref Math::DualComplex as underlying transformation type.
@see @ref scenegraph, @ref DualComplexTransformation,
    @ref BasicDualQuaternionTransformation
*/
template<class T> class BasicDualComplexTransformation: public AbstractBasicTranslationRotation2D<T> {
    public:
        /** @brief Underlying transformation type */
        typedef Math::DualComplex<T> DataType;

        /** @brief Object transformation */
        Math::DualComplex<T> transformation() const { return _transformation; }

        /**
         * @brief Set transformation
         * @return Reference to self (for method chaining)
         *
         * Expects that the dual complex number is normalized.
         * @see @ref Math::DualComplex::isNormalized()
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
         * @see @ref Math::DualComplex::normalized()
         */
        Object<BasicDualComplexTransformation<T>>& normalizeRotation() {
            return setTransformationInternal(_transformation.normalized());
        }

        /**
         * @brief Transform object
         * @return Reference to self (for method chaining)
         *
         * Expects that the dual complex number is normalized.
         * @see @ref transformLocal(), @ref Math::DualComplex::isNormalized()
         */
        Object<BasicDualComplexTransformation<T>>& transform(const Math::DualComplex<T>& transformation) {
            CORRADE_ASSERT(transformation.isNormalized(),
                "SceneGraph::DualComplexTransformation::transform(): the dual complex number is not normalized",
                static_cast<Object<BasicDualComplexTransformation<T>>&>(*this));
            return transformInternal(transformation);
        }

        /**
         * @brief Transform object as a local transformation
         *
         * Similar to the above, except that the transformation is applied
         * before all others.
         */
        Object<BasicDualComplexTransformation<T>>& transformLocal(const Math::DualComplex<T>& transformation) {
            CORRADE_ASSERT(transformation.isNormalized(),
                "SceneGraph::DualComplexTransformation::transformLocal(): the dual complex number is not normalized",
                static_cast<Object<BasicDualComplexTransformation<T>>&>(*this));
            return transformLocalInternal(transformation);
        }

        #ifdef MAGNUM_BUILD_DEPRECATED
        #ifdef __GNUC__
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wdeprecated-declarations"
        #elif defined(_MSC_VER)
        #pragma warning(push)
        #pragma warning(disable: 4996)
        #endif
        /**
         * @copybrief transform()
         * @deprecated Use @ref transform() or @ref transformLocal() instead.
         */
        CORRADE_DEPRECATED("use transform() or transformLocal() instead") Object<BasicDualComplexTransformation<T>>& transform(const Math::DualComplex<T>& transformation, TransformationType type) {
            return type == TransformationType::Global ? transform(transformation) : transformLocal(transformation);
        }
        #ifdef __GNUC__
        #pragma GCC diagnostic pop
        #elif defined(_MSC_VER)
        #pragma warning(pop)
        #endif
        #endif

        /**
         * @brief Translate object
         * @return Reference to self (for method chaining)
         *
         * Same as calling @ref transform() with @ref Math::DualComplex::translation().
         * @see @ref translateLocal(), @ref Math::Vector2::xAxis(),
         *      @ref Math::Vector2::yAxis()
         */
        Object<BasicDualComplexTransformation<T>>& translate(const Math::Vector2<T>& vector) {
            return transformInternal(Math::DualComplex<T>::translation(vector));
        }

        /**
         * @brief Translate object as a local transformation
         *
         * Similar to the above, except that the transformation is applied
         * before all others. Same as calling @ref transformLocal() with
         * @ref Math::DualComplex::translation().
         */
        Object<BasicDualComplexTransformation<T>>& translateLocal(const Math::Vector2<T>& vector) {
            return transformLocalInternal(Math::DualComplex<T>::translation(vector));
        }

        #ifdef MAGNUM_BUILD_DEPRECATED
        #ifdef __GNUC__
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wdeprecated-declarations"
        #elif defined(_MSC_VER)
        #pragma warning(push)
        #pragma warning(disable: 4996)
        #endif
        /**
         * @copybrief translate()
         * @deprecated Use @ref translate() or @ref translateLocal() instead.
         */
        CORRADE_DEPRECATED("use translate() or translateLocal() instead") Object<BasicDualComplexTransformation<T>>& translate(const Math::Vector2<T>& vector, TransformationType type) {
            return type == TransformationType::Global ? translate(vector) : translateLocal(vector);
        }
        #ifdef __GNUC__
        #pragma GCC diagnostic pop
        #elif defined(_MSC_VER)
        #pragma warning(pop)
        #endif
        #endif

        /**
         * @brief Rotate object
         * @param angle     Angle (counterclockwise)
         * @return Reference to self (for method chaining)
         *
         * Same as calling @ref transform() with @ref Math::DualComplex::rotation().
         * @see @ref rotateLocal(), @ref normalizeRotation()
         */
        Object<BasicDualComplexTransformation<T>>& rotate(Math::Rad<T> angle) {
            return transformInternal(Math::DualComplex<T>::rotation(angle));
        }

        /**
         * @brief Rotate object as a local transformation
         *
         * Similar to the above, except that the transformation is applied
         * before all others.
         */
        Object<BasicDualComplexTransformation<T>>& rotateLocal(Math::Rad<T> angle) {
            return transformLocalInternal(Math::DualComplex<T>::rotation(angle));
        }

        #ifdef MAGNUM_BUILD_DEPRECATED
        #ifdef __GNUC__
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wdeprecated-declarations"
        #elif defined(_MSC_VER)
        #pragma warning(push)
        #pragma warning(disable: 4996)
        #endif
        /**
         * @copybrief rotate()
         * @deprecated Use @ref rotate() or @ref rotateLocal() instead.
         */
        CORRADE_DEPRECATED("use rotate() or rotateLocal() instead") Object<BasicDualComplexTransformation<T>>& rotate(Math::Rad<T> angle, TransformationType type) {
            return type == TransformationType::Global ? rotate(angle) : rotateLocal(angle);
        }
        #ifdef __GNUC__
        #pragma GCC diagnostic pop
        #elif defined(_MSC_VER)
        #pragma warning(pop)
        #endif
        #endif

    protected:
        /* Allow construction only from Object */
        explicit BasicDualComplexTransformation() = default;

    private:
        void doResetTransformation() override final { resetTransformation(); }

        void doTranslate(const Math::Vector2<T>& vector) override final {
            translate(vector);
        }
        void doTranslateLocal(const Math::Vector2<T>& vector) override final {
            translateLocal(vector);
        }

        void doRotate(Math::Rad<T> angle) override final { rotate(angle); }
        void doRotateLocal(Math::Rad<T> angle) override final { rotateLocal(angle); }

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
        Object<BasicDualComplexTransformation<T>>& transformInternal(const Math::DualComplex<T>& transformation) {
            return setTransformationInternal(transformation*_transformation);
        }
        Object<BasicDualComplexTransformation<T>>& transformLocalInternal(const Math::DualComplex<T>& transformation) {
            return setTransformationInternal(_transformation*transformation);
        }

        Math::DualComplex<T> _transformation;
};

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

#if defined(CORRADE_TARGET_WINDOWS) && !defined(__MINGW32__)
extern template class MAGNUM_SCENEGRAPH_EXPORT Object<BasicDualComplexTransformation<Float>>;
#endif

}}

#endif
