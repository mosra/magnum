#ifndef Magnum_SceneGraph_Camera3D_h
#define Magnum_SceneGraph_Camera3D_h
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
 * @brief Class Magnum::SceneGraph::Camera3D
 */

#include "AbstractCamera.h"

#ifdef _WIN32 /* I so HATE windows.h */
#undef near
#undef far
#endif

namespace Magnum { namespace SceneGraph {

/**
@brief Camera for three-dimensional scenes

See Drawable documentation for introduction. The camera by default displays
OpenGL unit cube `[(-1, -1, -1); (1, 1, 1)]` with orthographic projection and
doesn't do any aspect ratio correction. Common setup example:
@code
SceneGraph::Camera3D<>* camera = new SceneGraph::Camera3D<>(&cameraObject);
camera->setPerspective({}, 0.001f, 100.0f)
      ->setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend);
@endcode

@section Camera3D-explicit-specializations Explicit template specializations

The following specialization are explicitly compiled into SceneGraph library.
For other specializations (e.g. using Double type) you have to use
Camera3D.hpp implementation file to avoid linker errors. See
@ref compilation-speedup-hpp for more information.

 - @ref Camera3D "Camera3D<Float>"

@see @ref scenegraph, Camera2D, Drawable, DrawableGroup
*/
#ifndef DOXYGEN_GENERATING_OUTPUT
template<class T>
#else
template<class T = Float>
#endif
class MAGNUM_SCENEGRAPH_EXPORT Camera3D: public AbstractCamera<3, T> {
    public:
        /**
         * @brief Constructor
         * @param object    %Object holding this feature
         */
        explicit Camera3D(AbstractObject<3, T>* object);

        /**
         * @brief Set orthographic projection
         * @param size          Size of the view
         * @param near          Near clipping plane
         * @param far           Far clipping plane
         * @return Pointer to self (for method chaining)
         *
         * @see setPerspective(), Matrix4::orthographicProjection()
         */
        Camera3D<T>* setOrthographic(const Math::Vector2<T>& size, T near, T far);

        /**
         * @brief Set perspective projection
         * @param size          Size of near clipping plane
         * @param near          Near clipping plane
         * @param far           Far clipping plane
         * @return Pointer to self (for method chaining)
         *
         * @see setOrthographic(), Matrix4::perspectiveProjection()
         */
        Camera3D<T>* setPerspective(const Math::Vector2<T>& size, T near, T far);

        /**
         * @brief Set perspective projection
         * @param fov           Field of view angle (horizontal)
         * @param aspectRatio   Aspect ratio
         * @param near          Near clipping plane
         * @param far           Far clipping plane
         * @return Pointer to self (for method chaining)
         *
         * @see setOrthographic(), Matrix4::perspectiveProjection()
         */
        Camera3D<T>* setPerspective(Math::Rad<T> fov, T aspectRatio, T near, T far);

        /** @brief Near clipping plane */
        T near() const { return _near; }

        /** @brief Far clipping plane */
        T far() const { return _far; }

        /* Overloads to remove WTF-factor from method chaining order */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        Camera3D<T>* setAspectRatioPolicy(AspectRatioPolicy policy) {
            AbstractCamera<3, T>::setAspectRatioPolicy(policy);
            return this;
        }
        #endif

    private:
        T _near, _far;
};

}}

#endif
