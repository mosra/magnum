#ifndef Magnum_SceneGraph_MatrixTransformation2D_h
#define Magnum_SceneGraph_MatrixTransformation2D_h
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
         * @brief Transform the object
         * @return Reference to self (for method chaining)
         *
         * @see @ref transformLocal()
         */
        Object<BasicMatrixTransformation2D<T>>& transform(const Math::Matrix3<T>& transformation) {
            return setTransformation(transformation*_transformation);
        }

        /**
         * @brief Transform the object as a local transformation
         *
         * Similar to the above, except that the transformation is applied
         * before all others.
         */
        Object<BasicMatrixTransformation2D<T>>& transformLocal(const Math::Matrix3<T>& transformation) {
            return setTransformation(_transformation*transformation);
        }

        /** @copydoc AbstractTranslationRotationScaling2D::resetTransformation() */
        Object<BasicMatrixTransformation2D<T>>& resetTransformation() {
            return setTransformation({});
        }

        /**
         * Translate the object
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
         * @brief Translate the object as a local transformation
         *
         * Similar to the above, except that the transformation is applied
         * before all others. Same as calling @ref transformLocal() with
         * @ref Math::Matrix3::translation().
         */
        Object<BasicMatrixTransformation2D<T>>& translateLocal(const Math::Vector2<T>& vector) {
            return transformLocal(Math::Matrix3<T>::translation(vector));
        }

        /**
         * @brief Rotate the object using a complex number
         * @param complex       Normalized complex number
         * @return Reference to self (for method chaining)
         * @m_since{2020,06}
         *
         * Expects that the complex number is normalized.
         * @see @ref rotate(Math::Rad<T>),
         *      @ref rotateLocal(const Math::Complex<T>&)
         */
        Object<BasicMatrixTransformation2D<T>>& rotate(const Math::Complex<T>& complex);

        /**
         * @brief Rotate the object using a complex number as a local transformation
         * @m_since{2020,06}
         *
         * Similar to the above, except that the transformation is applied
         * before all others.
         */
        Object<BasicMatrixTransformation2D<T>>& rotateLocal(const Math::Complex<T>& complex);

        /**
         * @brief Rotate the object
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
         * @brief Rotate the object as a local transformation
         *
         * Similar to the above, except that the transformation is applied
         * before all others. Same as calling @ref transformLocal() with
         * @ref Math::Matrix3::rotation().
         */
        Object<BasicMatrixTransformation2D<T>>& rotateLocal(Math::Rad<T> angle) {
            return transformLocal(Math::Matrix3<T>::rotation(angle));
        }

        /**
         * @brief Scale the object
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
         * @brief Scale the object as a local transformation
         *
         * Similar to the above, except that the transformation is applied
         * before all others. Same as calling @ref transformLocal() with
         * @ref Math::Matrix3::scaling().
         */
        Object<BasicMatrixTransformation2D<T>>& scaleLocal(const Math::Vector2<T>& vector) {
            return transformLocal(Math::Matrix3<T>::scaling(vector));
        }

        /**
         * @brief Reflect the object
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
         * @brief Reflect the object as a local transformation
         *
         * Similar to the above, except that the transformation is applied
         * before all others. Same as calling @ref transformLocal() with
         * @ref Math::Matrix3::reflection().
         */
        Object<BasicMatrixTransformation2D<T>>& reflectLocal(const Math::Vector2<T>& normal) {
            return transformLocal(Math::Matrix3<T>::reflection(normal));
        }

    protected:
        /* Allow construction only from Object */
        explicit BasicMatrixTransformation2D() = default;

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

#if defined(CORRADE_TARGET_WINDOWS) && !(defined(CORRADE_TARGET_MINGW) && !defined(CORRADE_TARGET_CLANG))
extern template class MAGNUM_SCENEGRAPH_EXPORT BasicMatrixTransformation2D<Float>;
extern template class MAGNUM_SCENEGRAPH_EXPORT Object<BasicMatrixTransformation2D<Float>>;
#endif

}}

#endif
