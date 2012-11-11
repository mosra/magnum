#ifndef Magnum_SceneGraph_Camera_h
#define Magnum_SceneGraph_Camera_h
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
 * @brief Class Magnum::SceneGraph::AbstractCamera, Magnum::SceneGraph::Camera2D, Magnum::SceneGraph::Camera3D, enum AspectRatioPolicy, alias Magnum::SceneGraph::AbstractCamera2D, Magnum::SceneGraph::AbstractCamera3D
 */

#include "Math/Matrix3.h"
#include "Math/Matrix4.h"
#include "AbstractFeature.h"

#include "magnumSceneGraphVisibility.h"

#ifdef WIN32 /* I so HATE windows.h */
#undef near
#undef far
#endif

namespace Magnum { namespace SceneGraph {

template<std::uint8_t, class> class Drawable;
template<std::uint8_t, class, class> class FeatureGroup;
#ifndef MAGNUM_GCC46_COMPATIBILITY
template<std::uint8_t dimensions, class T = GLfloat> using DrawableGroup = FeatureGroup<dimensions, Drawable<dimensions, T>, T>;
#else
template<std::uint8_t, class> class DrawableGroup;
#endif

/** @relates AbstractCamera
@brief Camera aspect ratio policy

@see AbstractCamera::setAspectRatioPolicy()
*/
enum class AspectRatioPolicy {
    NotPreserved,   /**< Don't preserve aspect ratio (default) */
    Extend,         /**< Extend on larger side of view */
    Clip            /**< Clip on smaller side of view */
};

#ifndef DOXYGEN_GENERATING_OUTPUT
namespace Implementation {
    template<std::uint8_t dimensions, class T> typename DimensionTraits<dimensions, T>::MatrixType aspectRatioFix(AspectRatioPolicy aspectRatioPolicy, const Math::Vector2<T>& projectionScale, const Math::Vector2<GLsizei>& viewport);
}
#endif

/**
@brief Base for cameras

See Drawable documentation for more information.

@section AbstractCamera-explicit-specializations Explicit template specializations

The following specialization are explicitly compiled into SceneGraph library.
For other specializations you have to use Camera.hpp implementation file to
avoid linker errors. See @ref compilation-speedup-hpp for more information.

 - @ref AbstractCamera "AbstractCamera<2>"
 - @ref AbstractCamera "AbstractCamera<3>"

@see Camera2D, Camera3D, Drawable, DrawableGroup, AbstractCamera2D,
    AbstractCamera3D
*/
template<std::uint8_t dimensions, class T = GLfloat> class SCENEGRAPH_EXPORT AbstractCamera: public AbstractFeature<dimensions, T> {
    public:
        /**
         * @brief Constructor
         * @param object        Object holding the camera
         */
        inline AbstractCamera(AbstractObject<dimensions, T>* object): AbstractFeature<dimensions, T>(object), _aspectRatioPolicy(AspectRatioPolicy::NotPreserved) {
            AbstractFeature<dimensions, T>::setCachedTransformations(AbstractFeature<dimensions, T>::CachedTransformation::InvertedAbsolute);
        }

        virtual ~AbstractCamera() = 0;

        /** @brief Aspect ratio policy */
        inline AspectRatioPolicy aspectRatioPolicy() const { return _aspectRatioPolicy; }

        /**
         * @brief Set aspect ratio policy
         * @return Pointer to self (for method chaining)
         */
        AbstractCamera<dimensions, T>* setAspectRatioPolicy(AspectRatioPolicy policy);

        /**
         * @brief Camera matrix
         *
         * Camera matrix describes world position relative to the camera and is
         * applied as first.
         */
        inline typename DimensionTraits<dimensions, T>::MatrixType cameraMatrix() {
            AbstractFeature<dimensions, T>::object()->setClean();
            return _cameraMatrix;
        }

        /**
         * @brief Projection matrix
         *
         * Projection matrix handles e.g. perspective distortion and is applied
         * as last.
         * @see projectionSize()
         */
        inline typename DimensionTraits<dimensions, T>::MatrixType projectionMatrix() const { return _projectionMatrix; }

        /**
         * @brief Size of (near) XY plane in current projection
         *
         * Returns size of near XY plane computed from projection matrix.
         * @see projectionMatrix()
         */
        inline Math::Vector2<T> projectionSize() const {
            return {T(2.0)/_projectionMatrix[0].x(), T(2.0)/_projectionMatrix[1].y()};
        }

        /** @brief Viewport size */
        inline Math::Vector2<GLsizei> viewport() const { return _viewport; }

        /**
         * @brief Set viewport size
         *
         * Stores viewport size internally and recalculates projection matrix
         * according to aspect ratio policy.
         * @see setAspectRatioPolicy()
         */
        virtual void setViewport(const Math::Vector2<GLsizei>& size);

        /**
         * @brief Draw
         *
         * Draws given group of drawables.
         */
        virtual void draw(DrawableGroup<dimensions, T>& group);

    protected:
        /** Recalculates camera matrix */
        inline void cleanInverted(const typename DimensionTraits<dimensions, T>::MatrixType& invertedAbsoluteTransformation) override {
            _cameraMatrix = invertedAbsoluteTransformation;
        }

        #ifndef DOXYGEN_GENERATING_OUTPUT
        inline void fixAspectRatio() {
            _projectionMatrix = Implementation::aspectRatioFix<dimensions, T>(_aspectRatioPolicy, {rawProjectionMatrix[0].x(), rawProjectionMatrix[1].y()}, _viewport)*rawProjectionMatrix;
        }

        typename DimensionTraits<dimensions, T>::MatrixType rawProjectionMatrix;
        AspectRatioPolicy _aspectRatioPolicy;
        #endif

    private:
        typename DimensionTraits<dimensions, T>::MatrixType _projectionMatrix;
        typename DimensionTraits<dimensions, T>::MatrixType _cameraMatrix;

        Math::Vector2<GLsizei> _viewport;
};

template<std::uint8_t dimensions, class T> inline AbstractCamera<dimensions, T>::~AbstractCamera() {}

/**
@brief Base for two-dimensional cameras

Convenience alternative to <tt>%AbstractCamera<2, T></tt>. See AbstractCamera
for more information.
@note Not available on GCC < 4.7. Use <tt>%AbstractCamera<2, T></tt> instead.
@see AbstractCamera3D
@todoc Remove workaround when Doxygen supports alias template
*/
#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_GCC46_COMPATIBILITY
template<class T = GLfloat> using AbstractCamera2D = AbstractCamera<2, T>;
#endif
#else
typedef AbstractCamera<2, T = GLfloat> AbstractCamera2D;
#endif

/**
@brief Base for three-dimensional cameras

Convenience alternative to <tt>%AbstractCamera<3, T></tt>. See AbstractCamera
for more information.
@note Not available on GCC < 4.7. Use <tt>%AbstractCamera<3, T></tt> instead.
@see AbstractCamera2D
@todoc Remove workaround when Doxygen supports alias template
*/
#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_GCC46_COMPATIBILITY
template<class T = GLfloat> using AbstractCamera3D = AbstractCamera<3, T>;
#endif
#else
typedef AbstractCamera<3, T = GLfloat> AbstractCamera3D;
#endif

/**
@brief Camera for two-dimensional scenes

See Drawable documentation for more information.

@section Object-explicit-specializations Explicit template specializations

The following specialization are explicitly compiled into SceneGraph library.
For other specializations you have to use Camera.hpp implementation file to
avoid linker errors. See @ref compilation-speedup-hpp for more information.

 - @ref Camera2D "Camera2D<GLfloat>"

@see Camera3D, Drawable, DrawableGroup
*/
template<class T = GLfloat> class SCENEGRAPH_EXPORT Camera2D: public AbstractCamera<2, T> {
    public:
        /**
         * @brief Constructor
         * @param object    %Object holding this feature
         *
         * Sets orthographic projection to the default OpenGL cube (range @f$ [-1; 1] @f$ in all directions).
         * @see setOrthographic()
         */
        inline Camera2D(AbstractObject<2, T>* object): AbstractCamera<2, T>(object) {}

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

/**
@brief Camera for three-dimensional scenes

See Drawable documentation for more information.

@section Object-explicit-specializations Explicit template specializations

The following specialization are explicitly compiled into SceneGraph library.
For other specializations you have to use Camera.hpp implementation file to
avoid linker errors. See @ref compilation-speedup-hpp for more information.

 - @ref Camera3D "Camera3D<GLfloat>"

@see Camera2D, Drawable, DrawableGroup
*/
template<class T = GLfloat> class SCENEGRAPH_EXPORT Camera3D: public AbstractCamera<3, T> {
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

/* Make implementers' life easier */
#ifndef MAGNUM_GCC46_COMPATIBILITY
template<class T = GLfloat> using DrawableGroup2D = DrawableGroup<2, T>;
template<class T = GLfloat> using DrawableGroup3D = DrawableGroup<3, T>;
#endif

}}

#endif
