#ifndef Magnum_SceneGraph_Camera2D_h
#define Magnum_SceneGraph_Camera2D_h
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
 * @brief Class @ref Magnum::SceneGraph::BasicCamera2D, typedef @ref Magnum::SceneGraph::Camera2D
 */

#include "Magnum/SceneGraph/AbstractCamera.h"

namespace Magnum { namespace SceneGraph {

/**
@brief Camera for two-dimensional scenes

See Drawable documentation for introduction. The camera by default displays
OpenGL unit cube `[(-1, -1, -1); (1, 1, 1)]` and doesn't do any aspect ratio
correction. Common setup example:
@code
SceneGraph::Camera2D camera(&cameraObject);
camera.setProjection({4.0f/3.0f, 1.0f})
      .setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend);
@endcode

@anchor SceneGraph-Camera2D-explicit-specializations
## Explicit template specializations

The following specialization is explicitly compiled into @ref SceneGraph
library. For other specializations (e.g. using @ref Double type) you have to
use @ref Camera2D.hpp implementation file to avoid linker errors. See also
relevant section in @ref SceneGraph-AbstractCamera-explicit-specializations "AbstractCamera"
class documentation or @ref compilation-speedup-hpp for more information.

-   @ref Camera2D

@see @ref scenegraph, @ref Camera2D, @ref BasicCamera3D, @ref Drawable,
    @ref DrawableGroup
*/
template<class T> class BasicCamera2D: public AbstractBasicCamera2D<T> {
    public:
        /**
         * @brief Constructor
         * @param object    %Object holding this feature
         *
         * Sets orthographic projection to the default OpenGL cube (range @f$ [-1; 1] @f$ in all directions).
         * @see @ref setProjection()
         */
        explicit BasicCamera2D(AbstractBasicObject2D<T>& object);

        #ifndef DOXYGEN_GENERATING_OUTPUT
        /* This is here to avoid ambiguity with deleted copy constructor when
           passing `*this` from class subclassing both BasicCamera2D and
           AbstractObject */
        template<class U, class = typename std::enable_if<std::is_base_of<AbstractBasicObject2D<T>, U>::value>::type> BasicCamera2D(U& object): BasicCamera2D(static_cast<AbstractBasicObject2D<T>&>(object)) {}
        #endif

        /**
         * @brief Set projection
         * @param size      Size of the view
         * @return Reference to self (for method chaining)
         *
         * @see @ref Matrix3::projection()
         */
        BasicCamera2D<T>& setProjection(const Math::Vector2<T>& size);

        /* Overloads to remove WTF-factor from method chaining order */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        BasicCamera2D<T>& setAspectRatioPolicy(AspectRatioPolicy policy) {
            AbstractBasicCamera2D<T>::setAspectRatioPolicy(policy);
            return *this;
        }
        #endif
};

/**
@brief Camera for two-dimensional float scenes

@see @ref Camera3D
*/
typedef BasicCamera2D<Float> Camera2D;

#if defined(CORRADE_TARGET_WINDOWS) && !defined(__MINGW32__)
extern template class MAGNUM_SCENEGRAPH_EXPORT BasicCamera2D<Float>;
#endif

}}

#endif
