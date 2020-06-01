#ifndef Magnum_SceneGraph_Camera_h
#define Magnum_SceneGraph_Camera_h
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
 * @brief Class @ref Magnum::SceneGraph::Camera, enum @ref Magnum::SceneGraph::AspectRatioPolicy, alias @ref Magnum::SceneGraph::BasicCamera2D, @ref Magnum::SceneGraph::BasicCamera3D, typedef @ref Magnum::SceneGraph::Camera2D, @ref Magnum::SceneGraph::Camera3D
 */

#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Matrix4.h"
#include "Magnum/SceneGraph/AbstractFeature.h"
#include "Magnum/SceneGraph/visibility.h"

#ifdef CORRADE_TARGET_WINDOWS /* I so HATE windef.h */
#undef near
#undef far
#endif

namespace Magnum { namespace SceneGraph {

/**
@brief Camera aspect ratio policy

@see @ref Camera::setAspectRatioPolicy()
*/
enum class AspectRatioPolicy: UnsignedByte {
    NotPreserved,   /**< Don't preserve aspect ratio (default) */
    Extend,         /**< Extend on larger side of view */
    Clip            /**< Clip on smaller side of view */
};

namespace Implementation {
    template<UnsignedInt dimensions, class T> MatrixTypeFor<dimensions, T> aspectRatioFix(AspectRatioPolicy aspectRatioPolicy, const Math::Vector2<T>& projectionScale, const Vector2i& viewport);
}

/**
@brief Camera

See @ref Drawable documentation for more information. The camera by default
displays OpenGL unit cube `[(-1, -1, -1); (1, 1, 1)]` and doesn't do any aspect
ratio correction.

Common setup example for 2D scenes:

@snippet MagnumSceneGraph.cpp Camera-2D

Common setup example for 3D scenes:

@snippet MagnumSceneGraph.cpp Camera-3D

@section SceneGraph-Camera-explicit-specializations Explicit template specializations

The following specializations are explicitly compiled into @ref SceneGraph
library. For other specializations (e.g. using @ref Magnum::Double "Double"
type) you have to use @ref Camera.hpp implementation file to avoid linker
errors. See also @ref compilation-speedup-hpp for more information.

-   @ref Camera2D
-   @ref Camera3D

@see @ref scenegraph, @ref BasicCamera2D, @ref BasicCamera3D, @ref Camera2D,
    @ref Camera3D, @ref Drawable, @ref DrawableGroup
*/
template<UnsignedInt dimensions, class T> class Camera: public AbstractFeature<dimensions, T> {
    public:
        /**
         * @brief Constructor
         * @param object        Object holding the camera
         *
         * Sets orthographic projection to the default OpenGL cube (range
         * @f$ [-1; 1] @f$ in all directions).
         * @see @ref setProjectionMatrix()
         */
        explicit Camera(AbstractObject<dimensions, T>& object);

        #ifndef DOXYGEN_GENERATING_OUTPUT
        /* This is here to avoid ambiguity with deleted copy constructor when
           passing `*this` from class subclassing both Camera and AbstractObject */
        template<class U, class = typename std::enable_if<std::is_base_of<AbstractObject<dimensions, T>, U>::value>::type> explicit Camera(U& object): Camera<dimensions, T>{static_cast<AbstractObject<dimensions, T>&>(object)} {}
        #endif

        ~Camera();

        /** @brief Aspect ratio policy */
        AspectRatioPolicy aspectRatioPolicy() const { return _aspectRatioPolicy; }

        /**
         * @brief Set aspect ratio policy
         * @return Reference to self (for method chaining)
         *
         * @attention Aspect ratio correction might not work properly with some
         *      specific cases of projection matrices. Projection matrices
         *      generated with @ref Matrix3::projection(),
         *      @ref Matrix4::orthographicProjection() or
         *      @ref Matrix4::perspectiveProjection() are known to be working.
         */
        Camera<dimensions, T>& setAspectRatioPolicy(AspectRatioPolicy policy);

        /**
         * @brief Camera matrix
         *
         * Camera matrix describes world position relative to the camera and is
         * applied after object transformation matrix and before projection
         * matrix.
         */
        MatrixTypeFor<dimensions, T> cameraMatrix() {
            AbstractFeature<dimensions, T>::object().setClean();
            return _cameraMatrix;
        }

        /**
         * @brief Projection matrix
         *
         * Projection matrix handles e.g. perspective distortion and is applied
         * as last, after @ref cameraMatrix() and object transformation matrix.
         * @see @ref projectionSize()
         */
        MatrixTypeFor<dimensions, T> projectionMatrix() const { return _projectionMatrix; }

        /**
         * @brief Set projection matrix
         * @return Reference to self (for method chaining)
         *
         * @see @ref Matrix3::projection(), @ref Matrix4::orthographicProjection(),
         *      @ref Matrix4::perspectiveProjection()
         */
        Camera<dimensions, T>& setProjectionMatrix(const MatrixTypeFor<dimensions, T>& matrix);

        /**
         * @brief Size of (near) XY plane in current projection
         *
         * Returns size of near XY plane computed from projection matrix.
         *
         * Conversion from integer window-space coordinates with origin in top
         * left corner and Y down (e.g. from
         * @ref Platform::Sdl2Application::MouseEvent "Platform::*Application::MouseEvent")
         * to floating-point coordinates on near XY plane with origin at camera
         * position and Y up can be done using the following snippet:
         *
         * @snippet MagnumSceneGraph-gl.cpp Camera-projectionSize
         *
         * This is position relative to camera transformation, getting absolute
         * transformation in 2D scene can be done for example using
         * @ref SceneGraph::Object::absoluteTransformation():
         *
         * @snippet MagnumSceneGraph-gl.cpp Camera-projectionSize-absolute
         *
         * @see @ref projectionMatrix()
         */
        Math::Vector2<T> projectionSize() const {
            return {T(2.0)/_projectionMatrix[0].x(), T(2.0)/_projectionMatrix[1].y()};
        }

        /** @brief Viewport size */
        Vector2i viewport() const { return _viewport; }

        /**
         * @brief Set viewport size
         *
         * Stores viewport size internally and recalculates projection matrix
         * according to aspect ratio policy.
         * @see @ref setAspectRatioPolicy()
         */
        void setViewport(const Vector2i& size);

        /**
         * @brief Drawable transformations
         *
         * Returns calculated camera-relative transformations for given group
         * of drawables. Useful in combination with @ref draw(const std::vector<std::pair<std::reference_wrapper<Drawable<dimensions, T>>, MatrixTypeFor<dimensions, T>>>&)
         * to implement custom draw order or object culling. See
         * @ref SceneGraph-Drawable-draw-order for more information.
         */
        std::vector<std::pair<std::reference_wrapper<Drawable<dimensions, T>>, MatrixTypeFor<dimensions, T>>> drawableTransformations(DrawableGroup<dimensions, T>& group);

        /**
         * @brief Draw
         *
         * Draws given group of drawables.
         * @see @ref draw(const std::vector<std::pair<std::reference_wrapper<Drawable<dimensions, T>>, MatrixTypeFor<dimensions, T>>>&)
         */
        void draw(DrawableGroup<dimensions, T>& group);

        /**
         * @brief Draw given drawables with transformations
         *
         * Useful in combination with @ref drawableTransformations() for
         * implementing custom draw order or object culling. See
         * @ref SceneGraph-Drawable-draw-order for more information.
         */
        void draw(const std::vector<std::pair<std::reference_wrapper<Drawable<dimensions, T>>, MatrixTypeFor<dimensions, T>>>& drawableTransformations);

    private:
        /** Recalculates camera matrix */
        void cleanInverted(const MatrixTypeFor<dimensions, T>& invertedAbsoluteTransformationMatrix) override {
            _cameraMatrix = invertedAbsoluteTransformationMatrix;
        }

        void fixAspectRatio();

        MatrixTypeFor<dimensions, T> _rawProjectionMatrix;
        AspectRatioPolicy _aspectRatioPolicy;

        MatrixTypeFor<dimensions, T> _projectionMatrix;
        MatrixTypeFor<dimensions, T> _cameraMatrix;

        Vector2i _viewport;
};

/**
@brief Camera for two-dimensional scenes

Convenience alternative to @cpp Camera<2, T> @ce. See @ref Camera for more
information.
@see @ref Camera2D, @ref BasicCamera3D
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<class T> using BasicCamera2D = Camera<2, T>;
#endif

/**
@brief Camera for two-dimensional float scenes

@see @ref Camera3D
*/
typedef BasicCamera2D<Float> Camera2D;

/**
@brief Camera for three-dimensional scenes

Convenience alternative to @cpp Camera<3, T> @ce. See @ref Camera for more
information.
@see @ref Camera3D, @ref BasicCamera2D
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<class T> using BasicCamera3D = Camera<3, T>;
#endif

/**
@brief Camera for three-dimensional float scenes

@see @ref Camera2D
*/
typedef BasicCamera3D<Float> Camera3D;

#if defined(CORRADE_TARGET_WINDOWS) && !(defined(CORRADE_TARGET_MINGW) && !defined(CORRADE_TARGET_CLANG))
extern template class MAGNUM_SCENEGRAPH_EXPORT Camera<2, Float>;
extern template class MAGNUM_SCENEGRAPH_EXPORT Camera<3, Float>;
#endif

}}

#endif
