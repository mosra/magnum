#ifndef Magnum_SceneGraph_Camera2D_h
#define Magnum_SceneGraph_Camera2D_h
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
 * @brief Class Magnum::SceneGraph::Camera2D
 */

#include "AbstractCamera.h"

namespace Magnum { namespace SceneGraph {

/**
@brief Camera for two-dimensional scenes

See Drawable documentation for introduction. The camera by default displays
OpenGL unit cube `[(-1, -1, -1); (1, 1, 1)]` and doesn't do any aspect ratio
correction. Common setup example:
@code
SceneGraph::Camera2D<>* camera = new SceneGraph::Camera2D<>(&cameraObject);
camera->setProjection({4.0f/3.0f, 1.0f})
      ->setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend);
@endcode

@section Camera2D-explicit-specializations Explicit template specializations

The following specialization are explicitly compiled into SceneGraph library.
For other specializations (e.g. using Double type) you have to use
Camera2D.hpp implementation file to avoid linker errors. See
@ref compilation-speedup-hpp for more information.

 - @ref Camera2D "Camera2D<Float>"

@see @ref scenegraph, Camera3D, Drawable, DrawableGroup
*/
#ifndef DOXYGEN_GENERATING_OUTPUT
template<class T>
#else
template<class T = Float>
#endif
class MAGNUM_SCENEGRAPH_EXPORT Camera2D: public AbstractCamera<2, T> {
    public:
        /**
         * @brief Constructor
         * @param object    %Object holding this feature
         *
         * Sets orthographic projection to the default OpenGL cube (range @f$ [-1; 1] @f$ in all directions).
         * @see setProjection()
         */
        explicit Camera2D(AbstractObject<2, T>* object);

        /**
         * @brief Set projection
         * @param size      Size of the view
         * @return Pointer to self (for method chaining)
         *
         * @see Matrix3::projection()
         */
        Camera2D<T>* setProjection(const Math::Vector2<T>& size);

        /* Overloads to remove WTF-factor from method chaining order */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        Camera2D<T>* setAspectRatioPolicy(AspectRatioPolicy policy) {
            AbstractCamera<2, T>::setAspectRatioPolicy(policy);
            return this;
        }
        #endif
};

}}

#endif
