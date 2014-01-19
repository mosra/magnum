#ifndef Magnum_SceneGraph_AbstractTranslation_h
#define Magnum_SceneGraph_AbstractTranslation_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014
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
 * @brief Class @ref Magnum::SceneGraph::AbstractTranslation, alias @ref Magnum::SceneGraph::AbstractBasicTranslation2D, @ref Magnum::SceneGraph::AbstractBasicTranslation3D, typedef @ref Magnum::SceneGraph::AbstractTranslation2D, @ref Magnum::SceneGraph::AbstractBasicTranslation3D
 */

#include "Magnum/DimensionTraits.h"
#include "Magnum/Math/Vector3.h"
#include "Magnum/SceneGraph/AbstractTransformation.h"

namespace Magnum { namespace SceneGraph {

/**
@brief Base transformation for two-dimensional scenes supporting translation

By default the translation is stored with the same underlying type as resulting
transformation matrix, but it's possible to store translation in e.g. integral
coordinates while having floating-point transformation matrix.

@see @ref AbstractBasicTranslation2D, @ref AbstractBasicTranslation3D,
    @ref AbstractTranslation2D, @ref AbstractTranslation3D, @ref scenegraph
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
template<UnsignedInt dimensions, class T, class TranslationType = T>
#else
template<UnsignedInt dimensions, class T, class TranslationType>
#endif
class AbstractTranslation: public AbstractTransformation<dimensions, T> {
    public:
        explicit AbstractTranslation() = default;

        /**
         * @brief Translate object
         * @param vector    Translation vector
         * @param type      Transformation type
         * @return Reference to self (for method chaining)
         *
         * @see @ref Vector2::xAxis(), @ref Vector2::yAxis(), @ref Vector3::xAxis(),
         *      @ref Vector3::yAxis(), @ref Vector3::zAxis()
         */
        AbstractTranslation<dimensions, T, TranslationType>& translate(const typename DimensionTraits<dimensions, TranslationType>::VectorType& vector, TransformationType type = TransformationType::Global) {
            doTranslate(vector, type);
            return *this;
        }

    protected:
        ~AbstractTranslation() = default;

    #ifdef DOXYGEN_GENERATING_OUTPUT
    protected:
    #else
    private:
    #endif
        /** @brief Polymorphic implementation for translate() */
        virtual void doTranslate(const typename DimensionTraits<dimensions, TranslationType>::VectorType& vector, TransformationType type) = 0;
};

#ifndef CORRADE_GCC46_COMPATIBILITY
/**
@brief Base transformation for two-dimensional scenes supporting translation

Convenience alternative to <tt>%AbstractTranslation<2, T, TranslationType></tt>.
See @ref AbstractTranslation for more information.
@note Not available on GCC < 4.7. Use <tt>%AbstractTranslation<2, T, TranslationType></tt>
    instead.
@see @ref AbstractTranslation2D, @ref AbstractBasicTranslation3D
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
template<class T, class TranslationType = T>
#else
template<class T, class TranslationType>
#endif
using AbstractBasicTranslation2D = AbstractTranslation<2, T, TranslationType>;
#endif

/**
@brief Base transformation for two-dimensional float scenes supporting translation

@see @ref AbstractTranslation3D
*/
#ifndef CORRADE_GCC46_COMPATIBILITY
typedef AbstractBasicTranslation2D<Float> AbstractTranslation2D;
#else
typedef AbstractTranslation<2, Float> AbstractTranslation2D;
#endif

#ifndef CORRADE_GCC46_COMPATIBILITY
/**
@brief Base transformation for three-dimensional scenes supporting translation

Convenience alternative to <tt>%AbstractTranslation<3, T, TranslationType></tt>.
See @ref AbstractTranslation for more information.
@note Not available on GCC < 4.7. Use <tt>%AbstractTranslation<3, T, TranslationType></tt>
    instead.
@see @ref AbstractTranslation3D, @ref AbstractBasicTranslation2D
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
template<class T, class TranslationType = T>
#else
template<class T, class TranslationType>
#endif
using AbstractBasicTranslation3D = AbstractTranslation<3, T, TranslationType>;
#endif

/**
@brief Base transformation for three-dimensional float scenes supporting translation

@see @ref AbstractTranslation2D
*/
#ifndef CORRADE_GCC46_COMPATIBILITY
typedef AbstractBasicTranslation3D<Float> AbstractTranslation3D;
#else
typedef AbstractTranslation<3, Float> AbstractTranslation3D;
#endif

}}

#endif
