#ifndef Magnum_SceneGraph_TranslationRotationScalingTransformation3D_h
#define Magnum_SceneGraph_TranslationRotationScalingTransformation3D_h
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
 * @brief Class @ref Magnum::SceneGraph::BasicTranslationRotationScalingTransformation3D, typedef @ref Magnum::SceneGraph::TranslationRotationScalingTransformation3D
 */

#include "Magnum/Math/Matrix4.h"
#include "Magnum/Math/Quaternion.h"
#include "Magnum/SceneGraph/AbstractTranslationRotationScaling3D.h"
#include "Magnum/SceneGraph/Object.h"

namespace Magnum { namespace SceneGraph {

/**
@brief Three-dimensional transformation implemented using translation, rotation and scaling

Similar to @ref BasicMatrixTransformation3D, but stores translation, rotation
and scaling separately. This makes it more suitable for e.g. animation, where
there are usually separate animation tracks for translation, rotation and
scaling. This separation also imposes some constraints --- for given object,
scaling is always applied first, rotation second and translation last. In
particular, unlike with matrix-based transformation implementation, it's not
possible to rotate a translated object, for example --- one has to apply the
rotation first and then translate using a rotated vector.
@see @ref scenegraph, @ref TranslationRotationScalingTransformation3D,
    @ref BasicTranslationRotationScalingTransformation2D
*/
template<class T> class BasicTranslationRotationScalingTransformation3D: public AbstractBasicTranslationRotationScaling3D<T> {
    public:
        /** @brief Underlying transformation type */
        typedef Math::Matrix4<T> DataType;

        /** @brief Object transformation */
        Math::Matrix4<T> transformation() const;

        /**
         * @brief Set transformation
         * @return Reference to self (for method chaining)
         *
         * Expects that the transformation doesn't contain shear or reflection.
         */
        Object<BasicTranslationRotationScalingTransformation3D<T>>& setTransformation(const Math::Matrix4<T>& transformation);

        /** @brief Object translation */
        Math::Vector3<T> translation() const { return _translation; }

        /**
         * @brief Set translation
         * @return Reference to self (for method chaining)
         *
         * Translation is always applied last, after rotation and scaling.
         */
        Object<BasicTranslationRotationScalingTransformation3D<T>>& setTranslation(const Math::Vector3<T>& translation);

        /** @brief Object rotation */
        Math::Quaternion<T> rotation() const { return _rotation; }

        /**
         * @brief Set rotation
         * @return Reference to self (for method chaining)
         *
         * Rotation is always applied after scaling and before translation.
         * Expects that the quaternion is normalized.
         */
        Object<BasicTranslationRotationScalingTransformation3D<T>>& setRotation(const Math::Quaternion<T>& rotation);

        /** @brief Object scaling */
        Math::Vector3<T> scaling() const { return _scaling; }

        /**
         * @brief Set scaling
         * @return Reference to self (for method chaining)
         *
         * Scaling is always applied first, before rotation and translation.
         */
        Object<BasicTranslationRotationScalingTransformation3D<T>>& setScaling(const Math::Vector3<T>& scaling);

        /** @copydoc AbstractTranslationRotationScaling3D::resetTransformation() */
        Object<BasicTranslationRotationScalingTransformation3D<T>>& resetTransformation() {
            return setTransformation({});
        }

        /**
         * @brief Translate the object
         * @return Reference to self (for method chaining)
         *
         * Note that translation is always applied last, after rotation and
         * scaling.
         * @see @ref translateLocal(), @ref Math::Vector3::xAxis(),
         *      @ref Math::Vector3::yAxis(), @ref Math::Vector3::zAxis()
         */
        Object<BasicTranslationRotationScalingTransformation3D<T>>& translate(const Math::Vector3<T>& vector) {
            return setTranslation(vector + _translation);
        }

        /**
         * @brief Translate the object as a local transformation
         *
         * Equivalent to the above, as translation is commutative. Note that
         * translation is always applied last, after rotation and scaling.
         */
        Object<BasicTranslationRotationScalingTransformation3D<T>>& translateLocal(const Math::Vector3<T>& vector) {
            return setTranslation(_translation + vector);
        }

        /**
         * @brief Rotate the object using a quaternion
         * @param quaternion    Normalized quaternion
         * @return Reference to self (for method chaining)
         *
         * Note that rotation is always applied after scaling and before
         * translation. Expects that the quaternion is normalized.
         * @see @ref rotate(Math::Rad<T>, const Math::Vector3<T>&),
         *      @ref rotateLocal(const Math::Quaternion<T>&), @ref rotateX(),
         *      @ref rotateY(), @ref rotateZ()
         */
        Object<BasicTranslationRotationScalingTransformation3D<T>>& rotate(const Math::Quaternion<T>& quaternion) {
            return setRotation(quaternion*_rotation);
        }

        /**
         * @brief Rotate the object using a quaternion as a local transformation
         *
         * Similar to the above, except that the rotation is applied before all
         * other rotations. Note that rotation is always applied after scaling
         * and before translation.
         */
        Object<BasicTranslationRotationScalingTransformation3D<T>>& rotateLocal(const Math::Quaternion<T>& quaternion) {
            return setRotation(_rotation*quaternion);
        }

        /**
         * @brief Rotate the object
         * @param angle             Angle (counterclockwise)
         * @param normalizedAxis    Normalized rotation axis
         * @return Reference to self (for method chaining)
         *
         * Same as calling @ref rotate(const Math::Quaternion<T>&)
         * with @ref Quaternion::rotation(). Note that rotation is always
         * applied after scaling and before translation.
         * @see @ref rotateLocal(), @ref rotateX(), @ref rotateY(),
         *      @ref rotateZ(), @ref Math::Vector3::xAxis(),
         *      @ref Math::Vector3::yAxis(), @ref Math::Vector3::zAxis()
         */
        Object<BasicTranslationRotationScalingTransformation3D<T>>& rotate(Math::Rad<T> angle, const Math::Vector3<T>& normalizedAxis) {
            return rotate(Math::Quaternion<T>::rotation(angle, normalizedAxis));
        }

        /**
         * @brief Rotate the object as a local transformation
         *
         * Similar to the above, except that the rotation is applied before all
         * other rotations. Note that rotation is always applied after scaling
         * and before translation. Same as calling
         * @ref rotateLocal(const Math::Quaternion<T>&) with
         * @ref Math::Quaternion::rotation().
         */
        Object<BasicTranslationRotationScalingTransformation3D<T>>& rotateLocal(Math::Rad<T> angle, const Math::Vector3<T>& normalizedAxis) {
            return rotateLocal(Math::Quaternion<T>::rotation(angle, normalizedAxis));
        }

        /**
         * @brief Rotate the object around X axis
         * @param angle             Angle (counterclockwise)
         * @return Reference to self (for method chaining)
         *
         * Same as calling @ref rotate() with @ref Math::Vector3::xAxis() as an
         * axis.
         * @see @ref rotateXLocal()
         */
        Object<BasicTranslationRotationScalingTransformation3D<T>>& rotateX(Math::Rad<T> angle) {
            return rotate(angle, Math::Vector3<T>::xAxis());
        }

        /**
         * @brief Rotate the object around X axis as a local transformation
         *
         * Similar to the above, except that the rotation is applied before all
         * other rotations. Note that rotation is always applied after scaling
         * and before translation. Same as calling @ref rotateLocal() with
         * @ref Math::Vector3::xAxis() as an axis.
         */
        Object<BasicTranslationRotationScalingTransformation3D<T>>& rotateXLocal(Math::Rad<T> angle) {
            return rotateLocal(angle, Math::Vector3<T>::xAxis());
        }

        /**
         * @brief Rotate the object around Y axis
         * @param angle             Angle (counterclockwise)
         * @return Reference to self (for method chaining)
         *
         * Same as calling @ref rotate() with @ref Math::Vector3::yAxis() as an
         * axis.
         * @see @ref rotateYLocal()
         */
        Object<BasicTranslationRotationScalingTransformation3D<T>>& rotateY(Math::Rad<T> angle) {
            return rotate(angle, Math::Vector3<T>::yAxis());
        }

        /**
         * @brief Rotate the object around Y axis as a local transformation
         *
         * Similar to the above, except that the rotation is applied before all
         * other rotations. Note that rotation is always applied after scaling
         * and before translation. Same as calling @ref rotateLocal() with
         * @ref Math::Vector3::yAxis() as an axis.
         */
        Object<BasicTranslationRotationScalingTransformation3D<T>>& rotateYLocal(Math::Rad<T> angle) {
            return rotateLocal(angle, Math::Vector3<T>::yAxis());
        }

        /**
         * @brief Rotate the object around Z axis
         * @param angle             Angle (counterclockwise)
         * @return Reference to self (for method chaining)
         *
         * Same as calling @ref rotate() with @ref Math::Vector3::yAxis() as an
         * axis.
         * @see @ref rotateZLocal()
         */
        Object<BasicTranslationRotationScalingTransformation3D<T>>& rotateZ(Math::Rad<T> angle) {
            return rotate(angle, Math::Vector3<T>::zAxis());
        }

        /**
         * @brief Rotate the object around Z axis as a local transformation
         *
         * Similar to the above, except that the rotation is applied before all
         * other rotations. Note that rotation is always applied after scaling
         * and before translation. Same as calling @ref rotateLocal() with
         * @ref Math::Vector3::zAxis() as an axis.
         */
        Object<BasicTranslationRotationScalingTransformation3D<T>>& rotateZLocal(Math::Rad<T> angle) {
            return rotateLocal(angle, Math::Vector3<T>::zAxis());
        }

        /**
         * @brief Scale the object
         * @return Reference to self (for method chaining)
         *
         * Note that scaling is always applied first, before rotation and
         * translation.
         * @see @ref scaleLocal(), @ref Math::Vector3::xScale(),
         *      @ref Math::Vector3::yScale(), @ref Math::Vector3::zScale()
         */
        Object<BasicTranslationRotationScalingTransformation3D<T>>& scale(const Math::Vector3<T>& vector) {
            return setScaling(vector*_scaling);
        }

        /**
         * @brief Scale the object as a local transformation
         *
         * Equivalent to the above, as scaling is commutative. Note that
         * scaling is always first, before rotation and translation.
         */
        Object<BasicTranslationRotationScalingTransformation3D<T>>& scaleLocal(const Math::Vector3<T>& vector) {
            return setScaling(_scaling*vector);
        }

    protected:
        /* Allow construction only from Object */
        explicit BasicTranslationRotationScalingTransformation3D() = default;

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

        Math::Vector3<T> _translation;
        Math::Quaternion<T> _rotation;
        /* Can't {} on GCC 4.8 because it complains about the constructor being
           explicit, _scaling(T(1)) fails on the most vexing parse (what the
           ... eh???) */
        Math::Vector3<T> _scaling = Math::Vector3<T>(T(1));
};

/**
@brief Three-dimensional transformation for float scenes implemented using translation, rotation and scaling

@see @ref TranslationRotationScalingTransformation2D
*/
typedef BasicTranslationRotationScalingTransformation3D<Float> TranslationRotationScalingTransformation3D;

template<class T> Math::Matrix4<T> BasicTranslationRotationScalingTransformation3D<T>::transformation() const {
    return Math::Matrix4<T>::from(_rotation.toMatrix(), _translation)*
        Math::Matrix4<T>::scaling(_scaling);
}

template<class T> Object<BasicTranslationRotationScalingTransformation3D<T>>& BasicTranslationRotationScalingTransformation3D<T>::setTransformation(const Math::Matrix4<T>& transformation) {
    /* Setting transformation is forbidden for the scene */
    /** @todo Assert for this? */
    /** @todo Do this in some common code so we don't need to include Object? */
    if(!static_cast<Object<BasicTranslationRotationScalingTransformation3D<T>>*>(this)->isScene()) {
        _translation = transformation.translation();
        _rotation = Math::Quaternion<T>::fromMatrix(transformation.rotationShear());
        _scaling = transformation.scaling();
        static_cast<Object<BasicTranslationRotationScalingTransformation3D<T>>*>(this)->setDirty();
    }

    return static_cast<Object<BasicTranslationRotationScalingTransformation3D<T>>&>(*this);
}

template<class T> Object<BasicTranslationRotationScalingTransformation3D<T>>& BasicTranslationRotationScalingTransformation3D<T>::setTranslation(const Math::Vector3<T>& translation) {
    /* Setting transformation is forbidden for the scene */
    /** @todo Assert for this? */
    /** @todo Do this in some common code so we don't need to include Object? */
    if(!static_cast<Object<BasicTranslationRotationScalingTransformation3D<T>>*>(this)->isScene()) {
        _translation = translation;
        static_cast<Object<BasicTranslationRotationScalingTransformation3D<T>>*>(this)->setDirty();
    }

    return static_cast<Object<BasicTranslationRotationScalingTransformation3D<T>>&>(*this);
}

template<class T> Object<BasicTranslationRotationScalingTransformation3D<T>>& BasicTranslationRotationScalingTransformation3D<T>::setRotation(const Math::Quaternion<T>& rotation) {
    /* Setting transformation is forbidden for the scene */
    /** @todo Assert for this? */
    /** @todo Do this in some common code so we don't need to include Object? */
    if(!static_cast<Object<BasicTranslationRotationScalingTransformation3D<T>>*>(this)->isScene()) {
        _rotation = rotation;
        static_cast<Object<BasicTranslationRotationScalingTransformation3D<T>>*>(this)->setDirty();
    }

    return static_cast<Object<BasicTranslationRotationScalingTransformation3D<T>>&>(*this);
}

template<class T> Object<BasicTranslationRotationScalingTransformation3D<T>>& BasicTranslationRotationScalingTransformation3D<T>::setScaling(const Math::Vector3<T>& scaling) {
    /* Setting transformation is forbidden for the scene */
    /** @todo Assert for this? */
    /** @todo Do this in some common code so we don't need to include Object? */
    if(!static_cast<Object<BasicTranslationRotationScalingTransformation3D<T>>*>(this)->isScene()) {
        _scaling = scaling;
        static_cast<Object<BasicTranslationRotationScalingTransformation3D<T>>*>(this)->setDirty();
    }

    return static_cast<Object<BasicTranslationRotationScalingTransformation3D<T>>&>(*this);
}

namespace Implementation {

template<class T> struct Transformation<BasicTranslationRotationScalingTransformation3D<T>> {
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
extern template class MAGNUM_SCENEGRAPH_EXPORT Object<BasicTranslationRotationScalingTransformation3D<Float>>;
#endif

}}

#endif
