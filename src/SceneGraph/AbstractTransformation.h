#ifndef Magnum_SceneGraph_AbstractTransformation_h
#define Magnum_SceneGraph_AbstractTransformation_h
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
 * @brief Class Magnum::SceneGraph::AbstractTransformation, alias Magnum::SceneGraph::AbstractBasicTransformation2D, Magnum::SceneGraph::AbstractBasicTransformation3D, typedef Magnum::SceneGraph::AbstractTransformation2D, Magnum::SceneGraph::AbstractTransformation3D, enum Magnum::SceneGraph::TransformationType
 */

#include "SceneGraph/SceneGraph.h"
#include "SceneGraph/magnumSceneGraphVisibility.h"

namespace Magnum { namespace SceneGraph {

/**
@brief Base for transformations

Provides transformation implementation for @ref Object instances.

@see @ref scenegraph, @ref AbstractBasicTransformation2D,
    @ref AbstractBasicTransformation3D, @ref AbstractTransformation2D,
    @ref AbstractTransformation3D
*/
template<UnsignedInt dimensions, class T> class MAGNUM_SCENEGRAPH_EXPORT AbstractTransformation {
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
        ~AbstractTransformation() = default;

    #ifdef DOXYGEN_GENERATING_OUTPUT
    protected:
    #else
    private:
    #endif
        /** @brief Polymorphic implementation for resetTransformation() */
        virtual void doResetTransformation() = 0;
};

/** @brief Transformation type */
enum class TransformationType: UnsignedByte {
    /** Global transformation, applied after all other transformations. */
    Global = 0x00,

    /** Local transformation, applied before all other transformations. */
    Local = 0x01
};

#ifndef CORRADE_GCC46_COMPATIBILITY
/**
@brief Base transformation for two-dimensional scenes

Convenience alternative to <tt>%AbstractTransformation<2, T></tt>. See
AbstractTransformation for more information.
@note Not available on GCC < 4.7. Use <tt>%AbstractTransformation<2, T></tt>
    instead.
@see @ref AbstractTransformation2D, @ref AbstractBasicTransformation3D
*/
template<class T> using AbstractBasicTransformation2D = AbstractTransformation<2, T>;
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

Convenience alternative to <tt>%AbstractTransformation<3, T></tt>. See
AbstractTransformation for more information.
@note Not available on GCC < 4.7. Use <tt>%AbstractTransformation<3, T></tt>
    instead.
@see @ref AbstractTransformation3D, @ref AbstractBasicTransformation2D
*/
template<class T> using AbstractBasicTransformation3D = AbstractTransformation<3, T>;
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

namespace Implementation {
    /* See DualQuaternionTransformation.h for example implementation */
    template<class T> struct Transformation;
}

}}

#endif
