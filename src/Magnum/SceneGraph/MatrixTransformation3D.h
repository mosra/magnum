#ifndef Magnum_SceneGraph_MatrixTransformation3D_h
#define Magnum_SceneGraph_MatrixTransformation3D_h
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
 * @brief Class @ref Magnum::SceneGraph::BasicMatrixTransformation3D, typedef @ref Magnum::SceneGraph::MatrixTransformation3D
 */

#include "Magnum/Math/Matrix4.h"
#include "Magnum/SceneGraph/AbstractTranslationRotationScaling3D.h"
#include "Magnum/SceneGraph/Object.h"

namespace Magnum { namespace SceneGraph {

/**
@brief Three-dimensional transformation implemented using matrices

Uses @ref Math::Matrix4 as underlying transformation type.
@see @ref scenegraph, @ref MatrixTransformation3D,
    @ref BasicRigidMatrixTransformation3D, @ref BasicMatrixTransformation2D
*/
template<class T> class BasicMatrixTransformation3D: public AbstractBasicTranslationRotationScaling3D<T> {
    public:
        /** @brief Underlying transformation type */
        typedef Math::Matrix4<T> DataType;

        /** @brief Object transformation */
        Math::Matrix4<T> transformation() const { return _transformation; }

        /**
         * @brief Set transformation
         * @return Reference to self (for method chaining)
         */
        Object<BasicMatrixTransformation3D<T>>& setTransformation(const Math::Matrix4<T>& transformation) {
            /* Setting transformation is forbidden for the scene */
            /** @todo Assert for this? */
            /** @todo Do this in some common code so we don't need to include Object? */
            if(!static_cast<Object<BasicMatrixTransformation3D<T>>*>(this)->isScene()) {
                _transformation = transformation;
                static_cast<Object<BasicMatrixTransformation3D<T>>*>(this)->setDirty();
            }

            return static_cast<Object<BasicMatrixTransformation3D<T>>&>(*this);
        }

        /** @copydoc AbstractTranslationRotationScaling3D::resetTransformation() */
        Object<BasicMatrixTransformation3D<T>>& resetTransformation() {
            return setTransformation({});
        }

        /**
         * @brief Transform object
         * @return Reference to self (for method chaining)
         *
         * @see @ref transformLocal()
         */
        Object<BasicMatrixTransformation3D<T>>& transform(const Math::Matrix4<T>& transformation) {
            return setTransformation(transformation*_transformation);
        }

        /**
         * @brief Transform object as a local transformation
         *
         * Similar to the above, except that the transformation is applied
         * before all others.
         */
        Object<BasicMatrixTransformation3D<T>>& transformLocal(const Math::Matrix4<T>& transformation) {
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
        CORRADE_DEPRECATED("use transform() or transformLocal() instead") Object<BasicMatrixTransformation3D<T>>& transform(const Math::Matrix4<T>& transformation, TransformationType type) {
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
         * Same as calling @ref transform() with @ref Math::Matrix4::translation().
         * @see @ref translateLocal(), @ref Math::Vector3::xAxis(),
         *      @ref Math::Vector3::yAxis(), @ref Math::Vector3::zAxis()
         */
        Object<BasicMatrixTransformation3D<T>>& translate(const Math::Vector3<T>& vector) {
            return transform(Math::Matrix4<T>::translation(vector));
        }

        /**
         * @brief Translate object as a local transformation
         *
         * Similar to the above, except that the transformation is applied
         * before all others. Same as calling @ref transformLocal() with
         * @ref Math::Matrix4::translation().
         */
        Object<BasicMatrixTransformation3D<T>>& translateLocal(const Math::Vector3<T>& vector) {
            return transformLocal(Math::Matrix4<T>::translation(vector));
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
        CORRADE_DEPRECATED("use translate() or translateLocal() instead") Object<BasicMatrixTransformation3D<T>>& translate(const Math::Vector3<T>& vector, TransformationType type) {
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
         * @param angle             Angle (counterclockwise)
         * @param normalizedAxis    Normalized rotation axis
         * @return Reference to self (for method chaining)
         *
         * Same as calling @ref transform() with @ref Math::Matrix4::rotation().
         * @see @ref rotateLocal(), @ref rotateX(), @ref rotateY(),
         *      @ref rotateZ(), @ref Math::Vector3::xAxis(),
         *      @ref Math::Vector3::yAxis(), @ref Math::Vector3::zAxis()
         */
        Object<BasicMatrixTransformation3D<T>>& rotate(Math::Rad<T> angle, const Math::Vector3<T>& normalizedAxis) {
            return transform(Math::Matrix4<T>::rotation(angle, normalizedAxis));
        }

        /**
         * @brief Rotate object as a local transformation
         *
         * Similar to the above, except that the transformation is applied
         * before all others. Same as calling @ref transformLocal() with
         * @ref Math::Matrix4::rotation().
         */
        Object<BasicMatrixTransformation3D<T>>& rotateLocal(Math::Rad<T> angle, const Math::Vector3<T>& normalizedAxis) {
            return transformLocal(Math::Matrix4<T>::rotation(angle, normalizedAxis));
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
        CORRADE_DEPRECATED("use rotate() or rotateLocal() instead") Object<BasicMatrixTransformation3D<T>>& rotate(Math::Rad<T> angle, const Math::Vector3<T>& normalizedAxis, TransformationType type) {
            return type == TransformationType::Global ? rotate(angle, normalizedAxis) : rotateLocal(angle, normalizedAxis);
        }
        #ifdef __GNUC__
        #pragma GCC diagnostic pop
        #elif defined(_MSC_VER)
        #pragma warning(pop)
        #endif
        #endif

        /**
         * @brief Rotate object around X axis
         * @param angle             Angle (counterclockwise)
         * @return Reference to self (for method chaining)
         *
         * Same as calling @ref transform() with @ref Math::Matrix4::rotationX().
         * @see @ref rotateXLocal()
         */
        Object<BasicMatrixTransformation3D<T>>& rotateX(Math::Rad<T> angle) {
            return transform(Math::Matrix4<T>::rotationX(angle));
        }

        /**
         * @brief Rotate object around X axis as a local transformation
         *
         * Similar to the above, except that the transformation is applied
         * before all others. Same as calling @ref transformLocal() with
         * @ref Math::Matrix4::rotationX().
         */
        Object<BasicMatrixTransformation3D<T>>& rotateXLocal(Math::Rad<T> angle) {
            return transformLocal(Math::Matrix4<T>::rotationX(angle));
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
         * @copybrief rotateX()
         * @deprecated Use @ref rotateX() or @ref rotateXLocal() instead.
         */
        CORRADE_DEPRECATED("use rotateX() or rotateXLocal() instead") Object<BasicMatrixTransformation3D<T>>& rotateX(Math::Rad<T> angle, TransformationType type) {
            return type == TransformationType::Global ? rotateX(angle) : rotateXLocal(angle);
        }
        #ifdef __GNUC__
        #pragma GCC diagnostic pop
        #elif defined(_MSC_VER)
        #pragma warning(pop)
        #endif
        #endif

        /**
         * @brief Rotate object around Y axis
         * @param angle             Angle (counterclockwise)
         * @return Reference to self (for method chaining)
         *
         * Same as calling @ref transform() with @ref Math::Matrix4::rotationY().
         * @see @ref rotateYLocal()
         */
        Object<BasicMatrixTransformation3D<T>>& rotateY(Math::Rad<T> angle) {
            return transform(Math::Matrix4<T>::rotationY(angle));
        }

        /**
         * @brief Rotate object around Y axis as a local transformation
         *
         * Similar to the above, except that the transformation is applied
         * before all others. Same as calling @ref transformLocal() with
         * @ref Math::Matrix4::rotationY().
         */
        Object<BasicMatrixTransformation3D<T>>& rotateYLocal(Math::Rad<T> angle) {
            return transformLocal(Math::Matrix4<T>::rotationY(angle));
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
         * @copybrief rotateY()
         * @deprecated Use @ref rotateY() or @ref rotateYLocal() instead.
         */
        CORRADE_DEPRECATED("use rotateY() or rotateYLocal() instead") Object<BasicMatrixTransformation3D<T>>& rotateY(Math::Rad<T> angle, TransformationType type) {
            return type == TransformationType::Global ? rotateY(angle) : rotateYLocal(angle);
        }
        #ifdef __GNUC__
        #pragma GCC diagnostic pop
        #elif defined(_MSC_VER)
        #pragma warning(pop)
        #endif
        #endif

        /**
         * @brief Rotate object around Z axis
         * @param angle             Angle (counterclockwise)
         * @return Reference to self (for method chaining)
         *
         * Same as calling @ref transform() with @ref Math::Matrix4::rotationZ().
         * @see @ref rotateZLocal()
         */
        Object<BasicMatrixTransformation3D<T>>& rotateZ(Math::Rad<T> angle) {
            return transform(Math::Matrix4<T>::rotationZ(angle));
        }

        /**
         * @brief Rotate object around Z axis as a local transformation
         *
         * Similar to the above, except that the transformation is applied
         * before all others. Same as calling @ref transformLocal() with
         * @ref Math::Matrix4::rotationZ().
         */
        Object<BasicMatrixTransformation3D<T>>& rotateZLocal(Math::Rad<T> angle) {
            return transformLocal(Math::Matrix4<T>::rotationZ(angle));
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
         * @copybrief rotateZ()
         * @deprecated Use @ref rotateZ() or @ref rotateZLocal() instead.
         */
        CORRADE_DEPRECATED("use rotateZ() or rotateZLocal() instead") Object<BasicMatrixTransformation3D<T>>& rotateZ(Math::Rad<T> angle, TransformationType type) {
            return type == TransformationType::Global ? rotateZ(angle) : rotateZLocal(angle);
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
         * Same as calling @ref transform() with @ref Math::Matrix4::scaling().
         * @see @ref scaleLocal(), @ref Math::Vector3::xScale(),
         *      @ref Math::Vector3::yScale(), @ref Math::Vector3::zScale()
         */
        Object<BasicMatrixTransformation3D<T>>& scale(const Math::Vector3<T>& vector) {
            return transform(Math::Matrix4<T>::scaling(vector));
        }

        /**
         * @brief Scale object as a local transformation
         *
         * Similar to the above, except that the transformation is applied
         * before all others. Same as calling @ref transformLocal() with
         * @ref Math::Matrix4::scaling().
         */
        Object<BasicMatrixTransformation3D<T>>& scaleLocal(const Math::Vector3<T>& vector) {
            return transformLocal(Math::Matrix4<T>::scaling(vector));
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
        CORRADE_DEPRECATED("use scale() or scaleLocal() instead") Object<BasicMatrixTransformation3D<T>>& scale(const Math::Vector3<T>& vector, TransformationType type) {
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
         * @param normal    Normal of the plane through which to reflect
         *      (normalized)
         * @return Reference to self (for method chaining)
         *
         * Same as calling @ref transform() with @ref Math::Matrix4::reflection().
         * @see @ref reflectLocal()
         */
        Object<BasicMatrixTransformation3D<T>>& reflect(const Math::Vector3<T>& normal) {
            return transform(Math::Matrix4<T>::reflection(normal));
        }

        /**
         * @brief Reflect object as a local transformation
         *
         * Similar to the above, except that the transformation is applied
         * before all others. Same as calling @ref transformLocal() with
         * @ref Math::Matrix4::reflection().
         */
        Object<BasicMatrixTransformation3D<T>>& reflectLocal(const Math::Vector3<T>& normal) {
            return transformLocal(Math::Matrix4<T>::reflection(normal));
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
        CORRADE_DEPRECATED("use reflect() or reflectLocal() instead") Object<BasicMatrixTransformation3D<T>>& reflect(const Math::Vector3<T>& normal, TransformationType type) {
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
        explicit BasicMatrixTransformation3D() = default;

    private:
        void doResetTransformation() override final { resetTransformation(); }

        void doTranslate(const Math::Vector3<T>& vector) override final { translate(vector); }
        void doTranslateLocal(const Math::Vector3<T>& vector) override final { translateLocal(vector); }

        void doRotate(Math::Rad<T> angle, const Math::Vector3<T>& normalizedAxis) override final {
            rotate(angle, normalizedAxis);
        }
        void doRotateLocal(Math::Rad<T> angle, const Math::Vector3<T>& normalizedAxis) override final {
            rotateLocal(angle, normalizedAxis);
        }

        void doRotateX(Math::Rad<T> angle) override final { rotateX(angle); }
        void doRotateXLocal(Math::Rad<T> angle) override final { rotateXLocal(angle); }

        void doRotateY(Math::Rad<T> angle) override final { rotateY(angle); }
        void doRotateYLocal(Math::Rad<T> angle) override final { rotateYLocal(angle); }

        void doRotateZ(Math::Rad<T> angle) override final { rotateZ(angle); }
        void doRotateZLocal(Math::Rad<T> angle) override final { rotateZLocal(angle); }

        void doScale(const Math::Vector3<T>& vector) override final { scale(vector); }
        void doScaleLocal(const Math::Vector3<T>& vector) override final { scaleLocal(vector); }

        Math::Matrix4<T> _transformation;
};

/**
@brief Three-dimensional transformation for float scenes implemented using matrices

@see @ref MatrixTransformation2D
*/
typedef BasicMatrixTransformation3D<Float> MatrixTransformation3D;

namespace Implementation {

template<class T> struct Transformation<BasicMatrixTransformation3D<T>> {
    constexpr static Math::Matrix4<T> fromMatrix(const Math::Matrix4<T>& matrix) {
        return matrix;
    }

    constexpr static Math::Matrix4<T> toMatrix(const Math::Matrix4<T>& transformation) {
        return transformation;
    }

    static Math::Matrix4<T> compose(const Math::Matrix4<T>& parent, const Math::Matrix4<T>& child) {
        return parent*child;
    }

    static Math::Matrix4<T> inverted(const Math::Matrix4<T>& transformation) {
        return transformation.inverted();
    }
};

}

#if defined(CORRADE_TARGET_WINDOWS) && !defined(__MINGW32__)
extern template class MAGNUM_SCENEGRAPH_EXPORT Object<BasicMatrixTransformation3D<Float>>;
#endif

}}

#endif
