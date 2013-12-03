#ifndef Magnum_SceneGraph_TranslationTransformation_h
#define Magnum_SceneGraph_TranslationTransformation_h
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
 * @brief Class @ref Magnum::SceneGraph::TranslationTransformation, alias @ref Magnum::SceneGraph::BasicTranslationTransformation2D, @ref Magnum::SceneGraph::BasicTranslationTransformation3D, typedef @ref Magnum::SceneGraph::TranslationTransformation2D, @ref Magnum::SceneGraph::BasicTranslationTransformation3D
 */

#include "Math/Matrix3.h"
#include "Math/Matrix4.h"
#include "SceneGraph/AbstractTranslation.h"
#include "SceneGraph/Object.h"

namespace Magnum { namespace SceneGraph {

/**
@brief Translation-only transformation

Uses @ref Math::Vector2 or @ref Math::Vector3 as underlying type. By default
the translation is stored with the same underlying type as resulting
transformation matrix, but it's possible to store translation in e.g. integral
coordinates while having floating-point transformation matrix.

Note that translation is commutative, so all @ref TransformationType parameters
have no effect and are included only for compatibility with other
transformation implementations.

@see @ref BasicTranslationTransformation2D, @ref BasicTranslationTransformation3D,
    @ref TranslationTransformation2D, @ref TranslationTransformation3D,
    @ref scenegraph
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
template<UnsignedInt dimensions, class T, class TranslationType = T>
#else
template<UnsignedInt dimensions, class T, class TranslationType>
#endif
class TranslationTransformation: public AbstractTranslation<dimensions, T, TranslationType> {
    public:
        /** @brief Underlying transformation type */
        typedef typename DimensionTraits<dimensions, TranslationType>::VectorType DataType;

        /** @brief Object transformation */
        typename DimensionTraits<dimensions, TranslationType>::VectorType transformation() const { return _transformation; }

        /**
         * @brief Set transformation
         * @return Reference to self (for method chaining)
         */
        Object<TranslationTransformation<dimensions, T, TranslationType>>& setTransformation(const typename DimensionTraits<dimensions, TranslationType>::VectorType& transformation) {
            /* Setting transformation is forbidden for the scene */
            /** @todo Assert for this? */
            /** @todo Do this in some common code so we don't need to include Object? */
            if(!static_cast<Object<TranslationTransformation<dimensions, T, TranslationType>>*>(this)->isScene()) {
                _transformation = transformation;
                static_cast<Object<TranslationTransformation<dimensions, T, TranslationType>>*>(this)->setDirty();
            }

            return static_cast<Object<TranslationTransformation<dimensions, T, TranslationType>>&>(*this);
        }

        /** @copydoc AbstractTranslation::resetTransformation() */
        Object<TranslationTransformation<dimensions, T, TranslationType>>& resetTransformation() {
            return setTransformation({});
        }

        /**
         * @brief Add transformation
         * @param transformation    Transformation
         * @return Reference to self (for method chaining)
         *
         * Equivalent to @ref translate(), provided only for compatibility with
         * other implementations.
         */
        Object<TranslationTransformation<dimensions, T, TranslationType>>& transform(const typename DimensionTraits<dimensions, TranslationType>::VectorType& transformation, TransformationType = TransformationType::Global) {
            return translate(transformation);
        }

        /**
         * @brief Translate object
         * @param vector    Translation vector
         * @return Reference to self (for method chaining)
         *
         * @see @ref Vector2::xAxis(), @ref Vector2::yAxis(), @ref Vector3::xAxis(),
         *      @ref Vector3::yAxis(), @ref Vector3::zAxis()
         */
        Object<TranslationTransformation<dimensions, T, TranslationType>>& translate(const typename DimensionTraits<dimensions, TranslationType>::VectorType& vector, TransformationType = TransformationType::Global) {
            _transformation += vector;
            return static_cast<Object<TranslationTransformation<dimensions, T, TranslationType>>&>(*this);
        }

    protected:
        /* Allow construction only from Object */
        explicit TranslationTransformation();

    private:
        void doResetTransformation() override final { resetTransformation(); }

        void doTranslate(const typename DimensionTraits<dimensions, TranslationType>::VectorType& vector, TransformationType) override final {
            translate(vector);
        }

        typename DimensionTraits<dimensions, TranslationType>::VectorType _transformation;
};

template<UnsignedInt dimensions, class T, class TranslationType> inline TranslationTransformation<dimensions, T, TranslationType>::TranslationTransformation() = default;

#ifndef CORRADE_GCC46_COMPATIBILITY
/**
@brief Base transformation for two-dimensional scenes supporting translation

Convenience alternative to <tt>%TranslationTransformation<2, T, TranslationType></tt>.
See @ref TranslationTransformation for more information.
@note Not available on GCC < 4.7. Use <tt>%TranslationTransformation<2, T, TranslationType></tt>
    instead.
@see @ref TranslationTransformation2D, @ref BasicTranslationTransformation3D
*/
#ifndef CORRADE_MSVC2013_COMPATIBILITY /* Apparently cannot have multiply defined aliases */
#ifdef DOXYGEN_GENERATING_OUTPUT
template<class T, class TranslationType = T>
#else
template<class T, class TranslationType>
#endif
using BasicTranslationTransformation2D = TranslationTransformation<2, T, TranslationType>;
#endif
#endif

/**
@brief Base transformation for two-dimensional float scenes supporting translation

@see @ref TranslationTransformation3D
*/
#ifndef CORRADE_GCC46_COMPATIBILITY
typedef BasicTranslationTransformation2D<Float> TranslationTransformation2D;
#else
typedef TranslationTransformation<2, Float> TranslationTransformation2D;
#endif

#ifndef CORRADE_GCC46_COMPATIBILITY
/**
@brief Base transformation for three-dimensional scenes supporting translation

Convenience alternative to <tt>%TranslationTransformation<3, T, TranslationType></tt>.
See @ref TranslationTransformation for more information.
@note Not available on GCC < 4.7. Use <tt>%TranslationTransformation<3, T, TranslationType></tt>
    instead.
@see @ref TranslationTransformation3D, @ref BasicTranslationTransformation2D
*/
#ifndef CORRADE_MSVC2013_COMPATIBILITY /* Apparently cannot have multiply defined aliases */
#ifdef DOXYGEN_GENERATING_OUTPUT
template<class T, class TranslationType = T>
#else
template<class T, class TranslationType>
#endif
using BasicTranslationTransformation3D = TranslationTransformation<3, T, TranslationType>;
#endif
#endif

/**
@brief Base transformation for three-dimensional float scenes supporting translation

@see @ref TranslationTransformation2D
*/
#ifndef CORRADE_GCC46_COMPATIBILITY
typedef BasicTranslationTransformation3D<Float> TranslationTransformation3D;
#else
typedef TranslationTransformation<3, Float> TranslationTransformation3D;
#endif

namespace Implementation {

template<UnsignedInt dimensions, class T, class TranslationType> struct Transformation<TranslationTransformation<dimensions, T, TranslationType>> {
    static typename DimensionTraits<dimensions, TranslationType>::VectorType fromMatrix(const typename DimensionTraits<dimensions, T>::MatrixType& matrix) {
        CORRADE_ASSERT((matrix.rotationScaling() == Math::Matrix<dimensions, T>()),
           "SceneGraph::TranslationTransformation: the matrix doesn't represent pure translation", {});
        return typename DimensionTraits<dimensions, TranslationType>::VectorType(matrix.translation());
    }

    constexpr static typename DimensionTraits<dimensions, T>::MatrixType toMatrix(const typename DimensionTraits<dimensions, TranslationType>::VectorType& transformation) {
        return DimensionTraits<dimensions, T>::MatrixType::translation(typename DimensionTraits<dimensions, T>::VectorType(transformation));
    }

    static typename DimensionTraits<dimensions, TranslationType>::VectorType compose(const typename DimensionTraits<dimensions, TranslationType>::VectorType& parent, const typename DimensionTraits<dimensions, TranslationType>::VectorType& child) {
        return parent+child;
    }

    static typename DimensionTraits<dimensions, TranslationType>::VectorType inverted(const typename DimensionTraits<dimensions, TranslationType>::VectorType& transformation) {
        return -transformation;
    }
};

}

}}

#endif
