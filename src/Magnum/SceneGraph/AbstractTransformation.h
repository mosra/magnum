#ifndef Magnum_SceneGraph_AbstractTransformation_h
#define Magnum_SceneGraph_AbstractTransformation_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015
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
 * @brief Class @ref Magnum::SceneGraph::AbstractTransformation, alias @ref Magnum::SceneGraph::AbstractBasicTransformation2D, @ref Magnum::SceneGraph::AbstractBasicTransformation3D, typedef @ref Magnum::SceneGraph::AbstractTransformation2D, @ref Magnum::SceneGraph::AbstractTransformation3D, enum @ref Magnum::SceneGraph::TransformationType
 */

#include <Corrade/Utility/Macros.h>

#include "Magnum/SceneGraph/SceneGraph.h"
#include "Magnum/SceneGraph/visibility.h"

namespace Magnum { namespace SceneGraph {

/**
@brief Base for transformations

Provides transformation implementation for @ref Object instances. See
@ref scenegraph-features-transformation for more information.

@anchor SceneGraph-AbstractTransformation-explicit-specializations
## Explicit template specializations

The following specializations are explicitly compiled into @ref SceneGraph
library. For other specializations (e.g. using @ref Magnum::Double "Double"
type) you have to use @ref Object.hpp implementation file to avoid linker
errors. See @ref compilation-speedup-hpp for more information.

-   @ref AbstractTransformation2D
-   @ref AbstractTransformation3D

@see @ref scenegraph, @ref AbstractBasicTransformation2D,
    @ref AbstractBasicTransformation3D, @ref AbstractTransformation2D,
    @ref AbstractTransformation3D
*/
template<UnsignedInt dimensions, class T> class AbstractTransformation {
    public:
        /** @brief Underlying floating-point type */
        typedef T Type;

        /** @brief Dimension count */
        static const UnsignedInt Dimensions = dimensions;

        explicit AbstractTransformation();

        /**
         * @brief Reset object transformation
         * @return Reference to self (for method chaining)
         */
        AbstractTransformation<dimensions, T>& resetTransformation() {
            doResetTransformation();
            return *this;
        }

    protected:
        ~AbstractTransformation();

    #ifdef DOXYGEN_GENERATING_OUTPUT
    protected:
    #else
    private:
    #endif
        /** @brief Polymorphic implementation for @ref resetTransformation() */
        virtual void doResetTransformation() = 0;
};

template<UnsignedInt dimensions, class T> inline AbstractTransformation<dimensions, T>::~AbstractTransformation() = default;

#ifdef MAGNUM_BUILD_DEPRECATED
/**
@brief Transformation type
@deprecated Use `*Transformation*::*()` and `*Transformation::*Local*()`
    overloads instead.
*/
enum class CORRADE_DEPRECATED_ENUM("use *() and *Local() overloads instead") TransformationType: UnsignedByte {
    /** Global transformation, applied after all other transformations. */
    Global = 0x00,

    /** Local transformation, applied before all other transformations. */
    Local = 0x01
};
#endif

#ifndef CORRADE_GCC46_COMPATIBILITY
/**
@brief Base transformation for two-dimensional scenes

Convenience alternative to `AbstractTransformation<2, T>`. See
@ref AbstractTransformation for more information.
@note Not available on GCC < 4.7. Use <tt>%AbstractTransformation<2, T></tt>
    instead.
@see @ref AbstractTransformation2D, @ref AbstractBasicTransformation3D
*/
#ifndef CORRADE_MSVC2013_COMPATIBILITY /* Apparently cannot have multiply defined aliases */
template<class T> using AbstractBasicTransformation2D = AbstractTransformation<2, T>;
#endif
#endif

/**
@brief Base transformation for two-dimensional float scenes

@see @ref AbstractTransformation3D
*/
#ifndef CORRADE_GCC46_COMPATIBILITY
typedef AbstractBasicTransformation2D<Float> AbstractTransformation2D;
#else
typedef AbstractTransformation<2, Float> AbstractTransformation2D;
#endif

#ifndef CORRADE_GCC46_COMPATIBILITY
/**
@brief Base transformation for three-dimensional scenes

Convenience alternative to `AbstractTransformation<3, T>`. See
@ref AbstractTransformation for more information.
@note Not available on GCC < 4.7. Use <tt>%AbstractTransformation<3, T></tt>
    instead.
@see @ref AbstractTransformation3D, @ref AbstractBasicTransformation2D
*/
#ifndef CORRADE_MSVC2013_COMPATIBILITY /* Apparently cannot have multiply defined aliases */
template<class T> using AbstractBasicTransformation3D = AbstractTransformation<3, T>;
#endif
#endif

/**
@brief Base transformation for three-dimensional float scenes

@see @ref AbstractTransformation2D
*/
#ifndef CORRADE_GCC46_COMPATIBILITY
typedef AbstractBasicTransformation3D<Float> AbstractTransformation3D;
#else
typedef AbstractTransformation<3, Float> AbstractTransformation3D;
#endif

#if defined(CORRADE_TARGET_WINDOWS) && !defined(__MINGW32__)
extern template class MAGNUM_SCENEGRAPH_EXPORT AbstractTransformation<2, Float>;
extern template class MAGNUM_SCENEGRAPH_EXPORT AbstractTransformation<3, Float>;
#endif

}}

#endif
