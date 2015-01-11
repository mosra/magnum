#ifndef Magnum_SceneGraph_Camera3D_h
#define Magnum_SceneGraph_Camera3D_h
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
 * @brief Class @ref Magnum::SceneGraph::BasicCamera3D, typedef @ref Magnum::SceneGraph::Camera3D
 */

#include "Magnum/SceneGraph/AbstractCamera.h"

#ifdef CORRADE_TARGET_WINDOWS /* I so HATE windef.h */
#undef near
#undef far
#endif

namespace Magnum { namespace SceneGraph {

/**
@brief Camera for three-dimensional scenes

See @ref Drawable documentation for complete introduction. The camera by
default displays OpenGL unit cube `[(-1, -1, -1); (1, 1, 1)]` with orthographic
projection and doesn't do any aspect ratio correction. Common setup example:
@code
SceneGraph::Camera3D camera{&cameraObject};
camera.setPerspective(35.0_degf, 1.0f, 0.001f, 100.0f)
      .setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend);
@endcode

@anchor SceneGraph-Camera3D-explicit-specializations
## Explicit template specializations

The following specialization is explicitly compiled into @ref SceneGraph
library. For other specializations (e.g. using @ref Magnum::Double "Double"
type) you have to use @ref Camera3D.hpp implementation file to avoid linker
errors. See also relevant section in @ref SceneGraph-AbstractCamera-explicit-specializations "AbstractCamera"
class documentation or @ref compilation-speedup-hpp for more information.

-   @ref Camera3D

@see @ref scenegraph, @ref Camera3D, @ref BasicCamera2D, @ref Drawable,
    @ref DrawableGroup
*/
template<class T> class BasicCamera3D: public AbstractBasicCamera3D<T> {
    public:
        /**
         * @brief Constructor
         * @param object    Object holding this feature
         */
        explicit BasicCamera3D(AbstractBasicObject3D<T>& object);

        #ifndef DOXYGEN_GENERATING_OUTPUT
        /* This is here to avoid ambiguity with deleted copy constructor when
           passing `*this` from class subclassing both BasicCamera3D and
           AbstractObject */
        template<class U, class = typename std::enable_if<std::is_base_of<AbstractBasicObject3D<T>, U>::value>::type> BasicCamera3D(U& object): BasicCamera3D(static_cast<AbstractBasicObject3D<T>&>(object)) {}
        #endif

        /**
         * @brief Set orthographic projection
         * @param size          Size of the view
         * @param near          Near clipping plane
         * @param far           Far clipping plane
         * @return Reference to self (for method chaining)
         *
         * @see @ref setPerspective(), @ref Matrix4::orthographicProjection()
         */
        BasicCamera3D<T>& setOrthographic(const Math::Vector2<T>& size, T near, T far);

        /**
         * @brief Set perspective projection
         * @param size          Size of near clipping plane
         * @param near          Near clipping plane
         * @param far           Far clipping plane
         * @return Reference to self (for method chaining)
         *
         * @see @ref setOrthographic(), @ref Matrix4::perspectiveProjection()
         */
        BasicCamera3D<T>& setPerspective(const Math::Vector2<T>& size, T near, T far);

        /**
         * @brief Set perspective projection
         * @param fov           Field of view angle (horizontal)
         * @param aspectRatio   Aspect ratio
         * @param near          Near clipping plane
         * @param far           Far clipping plane
         * @return Reference to self (for method chaining)
         *
         * @see @ref setOrthographic(), @ref Matrix4::perspectiveProjection()
         */
        BasicCamera3D<T>& setPerspective(Math::Rad<T> fov, T aspectRatio, T near, T far);

        /** @brief Near clipping plane */
        T near() const { return _near; }

        /** @brief Far clipping plane */
        T far() const { return _far; }

        /* Overloads to remove WTF-factor from method chaining order */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        BasicCamera3D<T>& setAspectRatioPolicy(AspectRatioPolicy policy) {
            AbstractBasicCamera3D<T>::setAspectRatioPolicy(policy);
            return *this;
        }
        #endif

    private:
        T _near, _far;
};

/**
@brief Camera for three-dimensional float scenes

@see @ref Camera2D
*/
typedef BasicCamera3D<Float> Camera3D;

#if defined(CORRADE_TARGET_WINDOWS) && !defined(__MINGW32__)
extern template class MAGNUM_SCENEGRAPH_EXPORT BasicCamera3D<Float>;
#endif

}}

#endif
