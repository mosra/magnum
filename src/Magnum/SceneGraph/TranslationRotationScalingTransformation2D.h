#ifndef Magnum_SceneGraph_TranslationRotationScalingTransformation2D_h
#define Magnum_SceneGraph_TranslationRotationScalingTransformation2D_h
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
 * @brief Class @ref Magnum::SceneGraph::BasicTranslationRotationScalingTransformation2D, typedef @ref Magnum::SceneGraph::TranslationRotationScalingTransformation2D
 */

#include "Magnum/Math/Complex.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/SceneGraph/AbstractTranslationRotationScaling2D.h"
#include "Magnum/SceneGraph/Object.h"

namespace Magnum { namespace SceneGraph {

/**
@brief Two-dimensional transformation implemented using translation, rotation and scaling

Similar to @ref BasicMatrixTransformation2D, but stores translation, rotation
and scaling separately. This makes it more suitable for e.g. animation, where
there are usually separate animation tracks for translation, rotation and
scaling. This separation also imposes some constraints --- for given object,
scaling is always applied first, rotation second and translation last. In
particular, unlike with matrix-based transformation implementation, it's not
possible to rotate a translated object, for example --- one has to apply the
rotation first and then translate using a rotated vector.
@see @ref scenegraph, @ref TranslationRotationScalingTransformation2D,
    @ref BasicTranslationRotationScalingTransformation3D
*/
template<class T> class BasicTranslationRotationScalingTransformation2D: public AbstractBasicTranslationRotationScaling2D<T> {
    public:
        /** @brief Underlying transformation type */
        typedef Math::Matrix3<T> DataType;

        /** @brief Object transformation */
        Math::Matrix3<T> transformation() const;

        /**
         * @brief Set transformation
         * @return Reference to self (for method chaining)
         *
         * Expects that the transformation doesn't contain shear or reflection.
         */
        Object<BasicTranslationRotationScalingTransformation2D<T>>& setTransformation(const Math::Matrix3<T>& transformation);

        /** @brief Object translation */
        Math::Vector2<T> translation() const { return _translation; }

        /**
         * @brief Set translation
         * @return Reference to self (for method chaining)
         *
         * Translation is always applied last, after rotation and scaling.
         */
        Object<BasicTranslationRotationScalingTransformation2D<T>>& setTranslation(const Math::Vector2<T>& translation);

        /** @brief Object rotation */
        Math::Complex<T> rotation() const { return _rotation; }

        /**
         * @brief Set rotation
         * @return Reference to self (for method chaining)
         *
         * Rotation is always applied after scaling and before translation.
         * Expects that the quaternion is normalized.
         */
        Object<BasicTranslationRotationScalingTransformation2D<T>>& setRotation(const Math::Complex<T>& rotation);

        /** @brief Object scaling */
        Math::Vector2<T> scaling() const { return _scaling; }

        /**
         * @brief Set scaling
         * @return Reference to self (for method chaining)
         *
         * Scaling is always applied first, before rotation and translation.
         */
        Object<BasicTranslationRotationScalingTransformation2D<T>>& setScaling(const Math::Vector2<T>& scaling);

        /** @copydoc AbstractTranslationRotationScaling2D::resetTransformation() */
        Object<BasicTranslationRotationScalingTransformation2D<T>>& resetTransformation() {
            return setTransformation({});
        }

        /**
         * @brief Translate the object
         * @return Reference to self (for method chaining)
         *
         * Note that translation is always applied last, after rotation and
         * scaling.
         * @see @ref translateLocal(), @ref Math::Vector2::xAxis(),
         *      @ref Math::Vector2::yAxis()
         */
        Object<BasicTranslationRotationScalingTransformation2D<T>>& translate(const Math::Vector2<T>& vector) {
            return setTranslation(vector + _translation);
        }

        /**
         * @brief Translate the object as a local transformation
         *
         * Equivalent to the above, as translation is commutative. Note that
         * translation is always applied last, after rotation and scaling.
         */
        Object<BasicTranslationRotationScalingTransformation2D<T>>& translateLocal(const Math::Vector2<T>& vector) {
            return setTranslation(_translation + vector);
        }

        /**
         * @brief Rotate the object using a complex number
         * @param complex       Normalized complex number
         * @return Reference to self (for method chaining)
         *
         * Note that rotation is always applied after scaling and before
         * translation. Expects that the complex number is normalized.
         * @see @ref rotate(Math::Rad<T>),
         *      @ref rotateLocal(const Math::Complex<T>&)
         */
        Object<BasicTranslationRotationScalingTransformation2D<T>>& rotate(const Math::Complex<T>& complex) {
            return setRotation(complex*_rotation);
        }

        /**
         * @brief Rotate the object using a complex number as a local transformation
         *
         * Equivalent to the above, as 2D rotation is commutative. Note that
         * rotation is always applied after scaling and before translation.
         */
        Object<BasicTranslationRotationScalingTransformation2D<T>>& rotateLocal(const Math::Complex<T>& complex) {
            return setRotation(_rotation*complex);
        }

        /**
         * @brief Rotate the object
         * @param angle             Angle (counterclockwise)
         * @return Reference to self (for method chaining)
         *
         * Same as calling @ref rotate(const Math::Complex<T>&) with
         * @ref Math::Complex::rotation(). Note that rotation is always applied after
         * scaling and before translation.
         * @see @ref rotateLocal()
         */
        Object<BasicTranslationRotationScalingTransformation2D<T>>& rotate(Math::Rad<T> angle) {
            return rotate(Math::Complex<T>::rotation(angle));
        }

        /**
         * @brief Rotate the object as a local transformation
         *
         * Similar to the above, except that the rotation is applied before all
         * other rotations. Note that rotation is always applied after scaling
         * and before translation. Same as calling
         * @ref rotateLocal(const Math::Complex<T>&) with
         * @ref Math::Complex::rotation().
         */
        Object<BasicTranslationRotationScalingTransformation2D<T>>& rotateLocal(Math::Rad<T> angle) {
            return rotateLocal(Math::Complex<T>::rotation(angle));
        }

        /**
         * @brief Scale the object
         * @return Reference to self (for method chaining)
         *
         * Note that scaling is always applied first, before rotation and
         * translation.
         * @see @ref scaleLocal(), @ref Math::Vector2::xScale(),
         *      @ref Math::Vector2::yScale()
         */
        Object<BasicTranslationRotationScalingTransformation2D<T>>& scale(const Math::Vector2<T>& vector) {
            return setScaling(vector*_scaling);
        }

        /**
         * @brief Scale the object as a local transformation
         *
         * Equivalent to the above, as scaling is commutative. Note that
         * scaling is always first, before rotation and translation.
         */
        Object<BasicTranslationRotationScalingTransformation2D<T>>& scaleLocal(const Math::Vector2<T>& vector) {
            return setScaling(_scaling*vector);
        }

    protected:
        /* Allow construction only from Object */
        explicit BasicTranslationRotationScalingTransformation2D() = default;

    private:
        void doResetTransformation() override final { resetTransformation(); }

        void doTranslate(const Math::Vector2<T>& vector) override final { translate(vector); }
        void doTranslateLocal(const Math::Vector2<T>& vector) override final { translateLocal(vector); }

        void doRotate(const Math::Complex<T>& complex) override final {
            rotate(complex);
        }
        void doRotateLocal(const Math::Complex<T>& complex) override final {
            rotateLocal(complex);
        }

        void doRotate(Math::Rad<T> angle) override final {
            rotate(angle);
        }
        void doRotateLocal(Math::Rad<T> angle) override final {
            rotateLocal(angle);
        }

        void doScale(const Math::Vector2<T>& vector) override final { scale(vector); }
        void doScaleLocal(const Math::Vector2<T>& vector) override final { scaleLocal(vector); }

        Math::Vector2<T> _translation;
        Math::Complex<T> _rotation;
        /* Can't {} on GCC 4.8 because it complains about the constructor being
           explicit, _scaling(T(1)) fails on the most vexing parse (what the
           ... eh???) */
        Math::Vector2<T> _scaling = Math::Vector2<T>(T(1));
};

/**
@brief Two-dimensional transformation for float scenes implemented using translation, rotation and scaling

@see @ref TranslationRotationScalingTransformation3D
*/
typedef BasicTranslationRotationScalingTransformation2D<Float> TranslationRotationScalingTransformation2D;

template<class T> Math::Matrix3<T> BasicTranslationRotationScalingTransformation2D<T>::transformation() const {
    return Math::Matrix3<T>::from(_rotation.toMatrix(), _translation)*
        Math::Matrix3<T>::scaling(_scaling);
}

template<class T> Object<BasicTranslationRotationScalingTransformation2D<T>>& BasicTranslationRotationScalingTransformation2D<T>::setTransformation(const Math::Matrix3<T>& transformation) {
    /* Setting transformation is forbidden for the scene */
    /** @todo Assert for this? */
    /** @todo Do this in some common code so we don't need to include Object? */
    if(!static_cast<Object<BasicTranslationRotationScalingTransformation2D<T>>*>(this)->isScene()) {
        _translation = transformation.translation();
        _rotation = Math::Complex<T>::fromMatrix(transformation.rotationShear());
        _scaling = transformation.scaling();
        static_cast<Object<BasicTranslationRotationScalingTransformation2D<T>>*>(this)->setDirty();
    }

    return static_cast<Object<BasicTranslationRotationScalingTransformation2D<T>>&>(*this);
}

template<class T> Object<BasicTranslationRotationScalingTransformation2D<T>>& BasicTranslationRotationScalingTransformation2D<T>::setTranslation(const Math::Vector2<T>& translation) {
    /* Setting transformation is forbidden for the scene */
    /** @todo Assert for this? */
    /** @todo Do this in some common code so we don't need to include Object? */
    if(!static_cast<Object<BasicTranslationRotationScalingTransformation2D<T>>*>(this)->isScene()) {
        _translation = translation;
        static_cast<Object<BasicTranslationRotationScalingTransformation2D<T>>*>(this)->setDirty();
    }

    return static_cast<Object<BasicTranslationRotationScalingTransformation2D<T>>&>(*this);
}

template<class T> Object<BasicTranslationRotationScalingTransformation2D<T>>& BasicTranslationRotationScalingTransformation2D<T>::setRotation(const Math::Complex<T>& rotation) {
    /* Setting transformation is forbidden for the scene */
    /** @todo Assert for this? */
    /** @todo Do this in some common code so we don't need to include Object? */
    if(!static_cast<Object<BasicTranslationRotationScalingTransformation2D<T>>*>(this)->isScene()) {
        _rotation = rotation;
        static_cast<Object<BasicTranslationRotationScalingTransformation2D<T>>*>(this)->setDirty();
    }

    return static_cast<Object<BasicTranslationRotationScalingTransformation2D<T>>&>(*this);
}

template<class T> Object<BasicTranslationRotationScalingTransformation2D<T>>& BasicTranslationRotationScalingTransformation2D<T>::setScaling(const Math::Vector2<T>& scaling) {
    /* Setting transformation is forbidden for the scene */
    /** @todo Assert for this? */
    /** @todo Do this in some common code so we don't need to include Object? */
    if(!static_cast<Object<BasicTranslationRotationScalingTransformation2D<T>>*>(this)->isScene()) {
        _scaling = scaling;
        static_cast<Object<BasicTranslationRotationScalingTransformation2D<T>>*>(this)->setDirty();
    }

    return static_cast<Object<BasicTranslationRotationScalingTransformation2D<T>>&>(*this);
}

namespace Implementation {

template<class T> struct Transformation<BasicTranslationRotationScalingTransformation2D<T>> {
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

#if defined(CORRADE_TARGET_WINDOWS) && !(defined(CORRADE_TARGET_MINGW) && !defined(CORRADE_TARGET_CLANG))
extern template class MAGNUM_SCENEGRAPH_EXPORT Object<BasicTranslationRotationScalingTransformation2D<Float>>;
#endif

}}

#endif
