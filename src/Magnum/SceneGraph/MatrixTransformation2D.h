#ifndef Magnum_SceneGraph_MatrixTransformation2D_h
#define Magnum_SceneGraph_MatrixTransformation2D_h
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
 * @brief Class @ref Magnum::SceneGraph::BasicMatrixTransformation2D, typedef @ref Magnum::SceneGraph::MatrixTransformation2D
 */

#include "Magnum/Math/Matrix3.h"
#include "Magnum/SceneGraph/AbstractTranslationRotationScaling2D.h"
#include "Magnum/SceneGraph/Object.h"

namespace Magnum { namespace SceneGraph {

/**
@brief Two-dimensional transformation implemented using matrices

Uses @ref Math::Matrix3 as underlying transformation type.
@see @ref scenegraph, @ref MatrixTransformation2D,
    @ref BasicRigidMatrixTransformation2D, @ref BasicMatrixTransformation3D
*/
template<class T> class BasicMatrixTransformation2D: public AbstractBasicTranslationRotationScaling2D<T> {
    public:
        /** @brief Underlying transformation type */
        typedef Math::Matrix3<T> DataType;

        /** @brief Object transformation */
        Math::Matrix3<T> transformation() const { return _transformation; }

        /**
         * @brief Set transformation
         * @return Reference to self (for method chaining)
         */
        Object<BasicMatrixTransformation2D<T>>& setTransformation(const Math::Matrix3<T>& transformation) {
            /* Setting transformation is forbidden for the scene */
            /** @todo Assert for this? */
            /** @todo Do this in some common code so we don't need to include Object? */
            if(!static_cast<Object<BasicMatrixTransformation2D<T>>*>(this)->isScene()) {
                _transformation = transformation;
                static_cast<Object<BasicMatrixTransformation2D<T>>*>(this)->setDirty();
            }

            return static_cast<Object<BasicMatrixTransformation2D<T>>&>(*this);
        }

        /**
         * @brief Transform object
         * @return Reference to self (for method chaining)
         *
         * @see @ref transformLocal()
         */
        Object<BasicMatrixTransformation2D<T>>& transform(const Math::Matrix3<T>& transformation) {
            return setTransformation(transformation*_transformation);
        }

        /**
         * @brief Transform object as a local transformation
         *
         * Similar to the above, except that the transformation is applied
         * before all others.
         */
        Object<BasicMatrixTransformation2D<T>>& transformLocal(const Math::Matrix3<T>& transformation) {
            return setTransformation(_transformation*transformation);
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
        CORRADE_DEPRECATED("use transform() or transformLocal() instead") Object<BasicMatrixTransformation2D<T>>& transform(const Math::Matrix3<T>& transformation, TransformationType type) {
            return type == TransformationType::Global ? transform(transformation) : transformLocal(transformation);
        }
        #ifdef __GNUC__
        #pragma GCC diagnostic pop
        #elif defined(_MSC_VER)
        #pragma warning(pop)
        #endif
        #endif

        /** @copydoc AbstractTranslationRotationScaling2D::resetTransformation() */
        Object<BasicMatrixTransformation2D<T>>& resetTransformation() {
            return setTransformation({});
        }

        /**
         * Translate object
         * @return Reference to self (for method chaining)
         *
         * Same as calling @ref transform() with @ref Math::Matrix3::translation().
         * @see @ref translateLocal(), @ref Math::Vector2::xAxis(),
         *      @ref Math::Vector2::yAxis()
         */
        Object<BasicMatrixTransformation2D<T>>& translate(const Math::Vector2<T>& vector) {
            return transform(Math::Matrix3<T>::translation(vector));
        }

        /**
         * @brief Translate object as a local transformation
         *
         * Similar to the above, except that the transformation is applied
         * before all others. Same as calling @ref transformLocal() with
         * @ref Math::Matrix3::translation().
         */
        Object<BasicMatrixTransformation2D<T>>& translateLocal(const Math::Vector2<T>& vector) {
            return transformLocal(Math::Matrix3<T>::translation(vector));
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
        CORRADE_DEPRECATED("use translate() or translateLocal() instead") Object<BasicMatrixTransformation2D<T>>& translate(const Math::Vector2<T>& vector, TransformationType type) {
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
         * Same as calling @ref transform() with @ref Math::Matrix3::rotation().
         * @see @ref rotateLocal()
         */
        Object<BasicMatrixTransformation2D<T>>& rotate(Math::Rad<T> angle) {
            return transform(Math::Matrix3<T>::rotation(angle));
        }

        /**
         * @brief Rotate object as a local transformation
         *
         * Similar to the above, except that the transformation is applied
         * before all others. Same as calling @ref transformLocal() with
         * @ref Math::Matrix3::rotation().
         */
        Object<BasicMatrixTransformation2D<T>>& rotateLocal(Math::Rad<T> angle) {
            return transformLocal(Math::Matrix3<T>::rotation(angle));
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
        CORRADE_DEPRECATED("use rotate() or rotateLocal() instead") Object<BasicMatrixTransformation2D<T>>& rotate(Math::Rad<T> angle, TransformationType type) {
            return type == TransformationType::Global ? rotate(angle) : rotateLocal(angle);
        }
        #ifdef __GNUC__
        #pragma GCC diagnostic pop
        #elif defined(_MSC_VER)
        #pragma warning(pop)
        #endif
        #endif

        /**
         * @brief Scale object
         * @return Reference to self (for method chaining)
         *
         * Same as calling @ref transform() with @ref Math::Matrix3::scaling().
         * @see @ref scaleLocal(), @ref Math::Vector2::xScale(),
         *      @ref Math::Vector2::yScale()
         */
        Object<BasicMatrixTransformation2D<T>>& scale(const Math::Vector2<T>& vector) {
            return transform(Math::Matrix3<T>::scaling(vector));
        }

        /**
         * @brief Scale object as a local transformation
         *
         * Similar to the above, except that the transformation is applied
         * before all others. Same as calling @ref transformLocal() with
         * @ref Math::Matrix3::scaling().
         */
        Object<BasicMatrixTransformation2D<T>>& scaleLocal(const Math::Vector2<T>& vector) {
            return transformLocal(Math::Matrix3<T>::scaling(vector));
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
         * @copybrief scale()
         * @deprecated Use @ref scale() or @ref scaleLocal() instead.
         */
        CORRADE_DEPRECATED("use scale() or scaleLocal() instead") Object<BasicMatrixTransformation2D<T>>& scale(const Math::Vector2<T>& vector, TransformationType type) {
            return type == TransformationType::Global ? scale(vector) : scaleLocal(vector);
        }
        #ifdef __GNUC__
        #pragma GCC diagnostic pop
        #elif defined(_MSC_VER)
        #pragma warning(pop)
        #endif
        #endif

        /**
         * @brief Reflect object
         * @param normal    Normal of the line through which to reflect
         *      (normalized)
         * @return Reference to self (for method chaining)
         *
         * Same as calling @ref transform() with @ref Math::Matrix3::reflection().
         * @see @ref reflectLocal(), @ref Math::Vector2::xAxis(),
         *      @ref Math::Vector2::yAxis()
         */
        Object<BasicMatrixTransformation2D<T>>& reflect(const Math::Vector2<T>& normal) {
            return transform(Math::Matrix3<T>::reflection(normal));
        }

        /**
         * @brief Reflect object as a local transformation
         *
         * Similar to the above, except that the transformation is applied
         * before all others. Same as calling @ref transformLocal() with
         * @ref Math::Matrix3::reflection().
         */
        Object<BasicMatrixTransformation2D<T>>& reflectLocal(const Math::Vector2<T>& normal) {
            return transformLocal(Math::Matrix3<T>::reflection(normal));
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
         * @copybrief reflect()
         * @deprecated Use @ref reflect() or @ref reflectLocal() instead.
         */
        CORRADE_DEPRECATED("use reflect() or reflectLocal() instead") Object<BasicMatrixTransformation2D<T>>& reflect(const Math::Vector2<T>& normal, TransformationType type) {
            return type == TransformationType::Global ? reflect(normal) : reflectLocal(normal);
        }
        #ifdef __GNUC__
        #pragma GCC diagnostic pop
        #elif defined(_MSC_VER)
        #pragma warning(pop)
        #endif
        #endif

    protected:
        /* Allow construction only from Object */
        explicit BasicMatrixTransformation2D() = default;

    private:
        void doResetTransformation() override final { resetTransformation(); }

        void doTranslate(const Math::Vector2<T>& vector) override final { translate(vector); }
        void doTranslateLocal(const Math::Vector2<T>& vector) override final { translateLocal(vector); }

        void doRotate(Math::Rad<T> angle) override final { rotate(angle); }
        void doRotateLocal(Math::Rad<T> angle) override final { rotateLocal(angle); }

        void doScale(const Math::Vector2<T>& vector) override final { scale(vector); }
        void doScaleLocal(const Math::Vector2<T>& vector) override final { scaleLocal(vector); }

        Math::Matrix3<T> _transformation;
};

/**
@brief Two-dimensional transformation for float scenes implemented using matrices

@see @ref MatrixTransformation3D
*/
typedef BasicMatrixTransformation2D<Float> MatrixTransformation2D;

namespace Implementation {

template<class T> struct Transformation<BasicMatrixTransformation2D<T>> {
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
};

}

#if defined(CORRADE_TARGET_WINDOWS) && !defined(__MINGW32__)
extern template class MAGNUM_SCENEGRAPH_EXPORT Object<BasicMatrixTransformation2D<Float>>;
#endif

}}

#endif
