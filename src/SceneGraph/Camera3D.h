#ifndef Magnum_SceneGraph_Camera3D_h
#define Magnum_SceneGraph_Camera3D_h
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
 * @brief Class Magnum::SceneGraph::Camera3D
 */

#include "AbstractCamera.h"

#ifdef WIN32 /* I so HATE windows.h */
#undef near
#undef far
#endif

namespace Magnum { namespace SceneGraph {

/**
@brief Camera for three-dimensional scenes

See Drawable documentation for more information.

@section Camera3D-explicit-specializations Explicit template specializations

The following specialization are explicitly compiled into SceneGraph library.
For other specializations you have to use Camera3D.hpp implementation file to
avoid linker errors. See @ref compilation-speedup-hpp for more information.

 - @ref Camera3D "Camera3D<GLfloat>"

@see Camera2D, Drawable, DrawableGroup
*/
#ifndef DOXYGEN_GENERATING_OUTPUT
template<class T>
#else
template<class T = GLfloat>
#endif
class MAGNUM_SCENEGRAPH_EXPORT Camera3D: public AbstractCamera<3, T> {
    public:
        /**
         * @brief Constructor
         * @param object    %Object holding this feature
         *
         * Sets orthographic projection to the default OpenGL cube (range @f$ [-1; 1] @f$ in all directions).
         * @see setOrthographic(), setPerspective()
         */
        inline Camera3D(AbstractObject<3, T>* object): AbstractCamera<3, T>(object), _near(0.0f), _far(0.0f) {}

        /**
         * @brief Set orthographic projection
         * @param size      Size of the view
         * @param near      Near clipping plane
         * @param far       Far clipping plane
         * @return Pointer to self (for method chaining)
         *
         * The volume of given size will be scaled down to range @f$ [-1; 1] @f$
         * on all directions.
         */
        Camera3D<T>* setOrthographic(const Math::Vector2<T>& size, T near, T far);

        /**
         * @brief Set perspective projection
         * @param fov       Field of view angle
         * @param near      Near clipping plane
         * @param far       Far clipping plane
         * @return Pointer to self (for method chaining)
         *
         * @todo Aspect ratio
         */
        Camera3D<T>* setPerspective(T fov, T near, T far);

        /** @brief Near clipping plane */
        inline T near() const { return _near; }

        /** @brief Far clipping plane */
        inline T far() const { return _far; }

        /* Overloads to remove WTF-factor from method chaining order */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        inline Camera3D<T>* setAspectRatioPolicy(AspectRatioPolicy policy) {
            AbstractCamera<3, T>::setAspectRatioPolicy(policy);
            return this;
        }
        #endif

    private:
        T _near, _far;
};

}}

#endif
