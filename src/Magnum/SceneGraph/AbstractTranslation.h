#ifndef Magnum_SceneGraph_AbstractTranslation_h
#define Magnum_SceneGraph_AbstractTranslation_h
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
 * @brief Class @ref Magnum::SceneGraph::AbstractTranslation, alias @ref Magnum::SceneGraph::AbstractBasicTranslation2D, @ref Magnum::SceneGraph::AbstractBasicTranslation3D, typedef @ref Magnum::SceneGraph::AbstractTranslation2D, @ref Magnum::SceneGraph::AbstractBasicTranslation3D
 */

#include "Magnum/DimensionTraits.h"
#include "Magnum/Math/Vector3.h"
#include "Magnum/SceneGraph/AbstractTransformation.h"

namespace Magnum { namespace SceneGraph {

/**
@brief Base transformation for two-dimensional scenes supporting translation

See @ref scenegraph-features-transformation for more information.

By default the translation is stored with the same underlying type as resulting
transformation matrix, but it's possible to store translation in e.g. integral
coordinates while having floating-point transformation matrix.

@see @ref scenegraph, @ref AbstractBasicTranslation2D,
    @ref AbstractBasicTranslation3D, @ref AbstractTranslation2D,
    @ref AbstractTranslation3D, @ref TranslationTransformation
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
         * @brief Translate the object
         * @return Reference to self (for method chaining)
         *
         * @see @ref translateLocal(), @ref Math::Vector2::xAxis(),
         *      @ref Math::Vector2::yAxis(), @ref Math::Vector3::xAxis(),
         *      @ref Math::Vector3::yAxis(), @ref Math::Vector3::zAxis()
         */
        AbstractTranslation<dimensions, T, TranslationType>& translate(const VectorTypeFor<dimensions, TranslationType>& vector) {
            doTranslate(vector);
            return *this;
        }

        /**
         * @brief Translate the object as a local transformation
         *
         * Similar to the above, except that the transformation is applied
         * before all others.
         */
        AbstractTranslation<dimensions, T, TranslationType>& translateLocal(const VectorTypeFor<dimensions, TranslationType>& vector) {
            doTranslateLocal(vector);
            return *this;
        }

        /* Overloads to remove WTF-factor from method chaining order */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        AbstractTranslation<dimensions, T, TranslationType>& resetTransformation() {
            AbstractTransformation<dimensions, T>::resetTransformation();
            return *this;
        }
        #endif

    protected:
        ~AbstractTranslation() = default;

    #ifdef DOXYGEN_GENERATING_OUTPUT
    protected:
    #else
    private:
    #endif
        /** @brief Polymorphic implementation for @ref translate() */
        virtual void doTranslate(const VectorTypeFor<dimensions, TranslationType>& vector) = 0;

        /** @brief Polymorphic implementation for @ref translateLocal() */
        virtual void doTranslateLocal(const VectorTypeFor<dimensions, TranslationType>& vector) = 0;
};

/**
@brief Base transformation for two-dimensional scenes supporting translation

Convenience alternative to @cpp AbstractTranslation<2, T, TranslationType> @ce.
See @ref AbstractTranslation for more information.
@see @ref AbstractTranslation2D, @ref AbstractBasicTranslation3D
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
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
typedef AbstractBasicTranslation2D<Float> AbstractTranslation2D;

/**
@brief Base transformation for three-dimensional scenes supporting translation

Convenience alternative to @cpp AbstractTranslation<3, T, TranslationType> @ce.
See @ref AbstractTranslation for more information.
@see @ref AbstractTranslation3D, @ref AbstractBasicTranslation2D
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
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
typedef AbstractBasicTranslation3D<Float> AbstractTranslation3D;

}}

#endif
