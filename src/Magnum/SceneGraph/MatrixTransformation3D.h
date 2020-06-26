#ifndef Magnum_SceneGraph_MatrixTransformation3D_h
#define Magnum_SceneGraph_MatrixTransformation3D_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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
         * @brief Transform the object
         * @return Reference to self (for method chaining)
         *
         * @see @ref transformLocal()
         */
        Object<BasicMatrixTransformation3D<T>>& transform(const Math::Matrix4<T>& transformation) {
            return setTransformation(transformation*_transformation);
        }

        /**
         * @brief Transform the object as a local transformation
         *
         * Similar to the above, except that the transformation is applied
         * before all others.
         */
        Object<BasicMatrixTransformation3D<T>>& transformLocal(const Math::Matrix4<T>& transformation) {
            return setTransformation(_transformation*transformation);
        }

        /**
         * @brief Translate the object
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
         * @brief Translate the object as a local transformation
         *
         * Similar to the above, except that the transformation is applied
         * before all others. Same as calling @ref transformLocal() with
         * @ref Math::Matrix4::translation().
         */
        Object<BasicMatrixTransformation3D<T>>& translateLocal(const Math::Vector3<T>& vector) {
            return transformLocal(Math::Matrix4<T>::translation(vector));
        }

        /**
         * @brief Rotate the object using a quaternion
         * @param quaternion    Normalized quaternion
         * @return Reference to self (for method chaining)
         * @m_since{2020,06}
         *
         * Expects that the quaternion is normalized.
         * @see @ref rotate(Math::Rad<T>, const Math::Vector3<T>&),
         *      @ref rotateLocal(const Math::Quaternion<T>&), @ref rotateX(),
         *      @ref rotateY(), @ref rotateZ()
         */
        Object<BasicMatrixTransformation3D<T>>& rotate(const Math::Quaternion<T>& quaternion);

        /**
         * @brief Rotate the object using a quaternion as a local transformation
         * @m_since{2020,06}
         *
         * Similar to the above, except that the transformation is applied
         * before all others.
         */
        Object<BasicMatrixTransformation3D<T>>& rotateLocal(const Math::Quaternion<T>& quaternion);

        /**
         * @brief Rotate the object
         * @param angle             Angle (counterclockwise)
         * @param normalizedAxis    Normalized rotation axis
         * @return Reference to self (for method chaining)
         *
         * Same as calling @ref transform() with @ref Math::Matrix4::rotation().
         * @see @ref rotate(const Math::Quaternion<T>&), @ref rotateLocal(),
         *      @ref rotateX(), @ref rotateY(), @ref rotateZ(),
         *      @ref Math::Vector3::xAxis(), @ref Math::Vector3::yAxis(),
         *      @ref Math::Vector3::zAxis()
         */
        Object<BasicMatrixTransformation3D<T>>& rotate(Math::Rad<T> angle, const Math::Vector3<T>& normalizedAxis) {
            return transform(Math::Matrix4<T>::rotation(angle, normalizedAxis));
        }

        /**
         * @brief Rotate the object as a local transformation
         *
         * Similar to the above, except that the transformation is applied
         * before all others. Same as calling @ref transformLocal() with
         * @ref Math::Matrix4::rotation().
         */
        Object<BasicMatrixTransformation3D<T>>& rotateLocal(Math::Rad<T> angle, const Math::Vector3<T>& normalizedAxis) {
            return transformLocal(Math::Matrix4<T>::rotation(angle, normalizedAxis));
        }

        /**
         * @brief Rotate the object around X axis
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
         * @brief Rotate the object around X axis as a local transformation
         *
         * Similar to the above, except that the transformation is applied
         * before all others. Same as calling @ref transformLocal() with
         * @ref Math::Matrix4::rotationX().
         */
        Object<BasicMatrixTransformation3D<T>>& rotateXLocal(Math::Rad<T> angle) {
            return transformLocal(Math::Matrix4<T>::rotationX(angle));
        }

        /**
         * @brief Rotate the object around Y axis
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
         * @brief Rotate the object around Y axis as a local transformation
         *
         * Similar to the above, except that the transformation is applied
         * before all others. Same as calling @ref transformLocal() with
         * @ref Math::Matrix4::rotationY().
         */
        Object<BasicMatrixTransformation3D<T>>& rotateYLocal(Math::Rad<T> angle) {
            return transformLocal(Math::Matrix4<T>::rotationY(angle));
        }

        /**
         * @brief Rotate the object around Z axis
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
         * @brief Rotate the object around Z axis as a local transformation
         *
         * Similar to the above, except that the transformation is applied
         * before all others. Same as calling @ref transformLocal() with
         * @ref Math::Matrix4::rotationZ().
         */
        Object<BasicMatrixTransformation3D<T>>& rotateZLocal(Math::Rad<T> angle) {
            return transformLocal(Math::Matrix4<T>::rotationZ(angle));
        }

        /**
         * @brief Scale the object
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
         * @brief Scale the object as a local transformation
         *
         * Similar to the above, except that the transformation is applied
         * before all others. Same as calling @ref transformLocal() with
         * @ref Math::Matrix4::scaling().
         */
        Object<BasicMatrixTransformation3D<T>>& scaleLocal(const Math::Vector3<T>& vector) {
            return transformLocal(Math::Matrix4<T>::scaling(vector));
        }

        /**
         * @brief Reflect the object
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
         * @brief Reflect the object as a local transformation
         *
         * Similar to the above, except that the transformation is applied
         * before all others. Same as calling @ref transformLocal() with
         * @ref Math::Matrix4::reflection().
         */
        Object<BasicMatrixTransformation3D<T>>& reflectLocal(const Math::Vector3<T>& normal) {
            return transformLocal(Math::Matrix4<T>::reflection(normal));
        }

    protected:
        /* Allow construction only from Object */
        explicit BasicMatrixTransformation3D() = default;

    private:
        void doResetTransformation() override final { resetTransformation(); }

        void doTranslate(const Math::Vector3<T>& vector) override final { translate(vector); }
        void doTranslateLocal(const Math::Vector3<T>& vector) override final { translateLocal(vector); }

        void doRotate(const Math::Quaternion<T>& quaternion) override final {
            rotate(quaternion);
        }
        void doRotateLocal(const Math::Quaternion<T>& quaternion) override final {
            rotateLocal(quaternion);
        }

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

#if defined(CORRADE_TARGET_WINDOWS) && !(defined(CORRADE_TARGET_MINGW) && !defined(CORRADE_TARGET_CLANG))
extern template class MAGNUM_SCENEGRAPH_EXPORT BasicMatrixTransformation3D<Float>;
extern template class MAGNUM_SCENEGRAPH_EXPORT Object<BasicMatrixTransformation3D<Float>>;
#endif

}}

#endif
