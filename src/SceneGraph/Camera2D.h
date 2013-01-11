#ifndef Magnum_SceneGraph_Camera2D_h
#define Magnum_SceneGraph_Camera2D_h
/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
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
For other specializations you have to use Camera2D.hpp implementation file to
avoid linker errors. See @ref compilation-speedup-hpp for more information.

 - @ref Camera2D "Camera2D<GLfloat>"

@see @ref scenegraph, Camera3D, Drawable, DrawableGroup
*/
#ifndef DOXYGEN_GENERATING_OUTPUT
template<class T>
#else
template<class T = GLfloat>
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
        inline explicit Camera2D(AbstractObject<2, T>* object): AbstractCamera<2, T>(object) {}

        /**
         * @brief Set projection
         * @param size      Size of the view
         * @return Pointer to self (for method chaining)
         *
         * The area of given size will be scaled down to range @f$ [-1; 1] @f$
         * on all directions.
         */
        Camera2D<T>* setProjection(const Math::Vector2<T>& size);

        /* Overloads to remove WTF-factor from method chaining order */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        inline Camera2D<T>* setAspectRatioPolicy(AspectRatioPolicy policy) {
            AbstractCamera<2, T>::setAspectRatioPolicy(policy);
            return this;
        }
        #endif
};

}}

#endif
