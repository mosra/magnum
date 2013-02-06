#ifndef Magnum_SceneGraph_AbstractCamera_h
#define Magnum_SceneGraph_AbstractCamera_h
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
 * @brief Class Magnum::SceneGraph::AbstractCamera, enum Magnum::SceneGraph::AspectRatioPolicy, alias Magnum::SceneGraph::AbstractCamera2D, Magnum::SceneGraph::AbstractCamera3D
 */

#include "Math/Matrix3.h"
#include "Math/Matrix4.h"
#include "AbstractFeature.h"

#include "magnumSceneGraphVisibility.h"

namespace Magnum { namespace SceneGraph {

/** @relates AbstractCamera
@brief Camera aspect ratio policy

@see AbstractCamera::setAspectRatioPolicy()
*/
enum class AspectRatioPolicy: std::uint8_t {
    NotPreserved,   /**< Don't preserve aspect ratio (default) */
    Extend,         /**< Extend on larger side of view */
    Clip            /**< Clip on smaller side of view */
};

#ifndef DOXYGEN_GENERATING_OUTPUT
namespace Implementation {
    template<std::uint8_t dimensions, class T> typename DimensionTraits<dimensions, T>::MatrixType aspectRatioFix(AspectRatioPolicy aspectRatioPolicy, const Math::Vector2<T>& projectionScale, const Vector2i& viewport);
}
#endif

/**
@brief Base for cameras

See Drawable documentation for more information. This class is not directly
instantiatable, use Camera2D or Camera3D subclasses instead.

@section AbstractCamera-explicit-specializations Explicit template specializations

The following specialization are explicitly compiled into SceneGraph library.
For other specializations (e.g. using `double` type) you have to use
AbstractCamera.hpp implementation file to avoid linker errors. See also
relevant sections in
@ref Camera2D-explicit-specializations "Camera2D" and
@ref Camera3D-explicit-specializations "Camera3D" class documentation or
@ref compilation-speedup-hpp for more information.

 - @ref AbstractCamera "AbstractCamera<2, GLfloat>"
 - @ref AbstractCamera "AbstractCamera<3, GLfloat>"

@see @ref scenegraph, Drawable, DrawableGroup, AbstractCamera2D, AbstractCamera3D
*/
#ifndef DOXYGEN_GENERATING_OUTPUT
template<std::uint8_t dimensions, class T>
#else
template<std::uint8_t dimensions, class T = GLfloat>
#endif
class MAGNUM_SCENEGRAPH_EXPORT AbstractCamera: public AbstractFeature<dimensions, T> {
    public:
        /**
         * @brief Constructor
         * @param object        Object holding the camera
         */
        explicit AbstractCamera(AbstractObject<dimensions, T>* object);

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
        inline Vector2i viewport() const { return _viewport; }

        /**
         * @brief Set viewport size
         *
         * Stores viewport size internally and recalculates projection matrix
         * according to aspect ratio policy.
         * @see setAspectRatioPolicy()
         */
        virtual void setViewport(const Vector2i& size);

        /**
         * @brief Draw
         *
         * Draws given group of drawables.
         */
        virtual void draw(DrawableGroup<dimensions, T>& group);

    protected:
        /** Recalculates camera matrix */
        inline void cleanInverted(const typename DimensionTraits<dimensions, T>::MatrixType& invertedAbsoluteTransformationMatrix) override {
            _cameraMatrix = invertedAbsoluteTransformationMatrix;
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

        Vector2i _viewport;
};

#ifndef CORRADE_GCC46_COMPATIBILITY
/**
@brief Base for two-dimensional cameras

Convenience alternative to <tt>%AbstractCamera<2, T></tt>. See AbstractCamera
for more information.
@note Not available on GCC < 4.7. Use <tt>%AbstractCamera<2, T></tt> instead.
@see AbstractCamera3D
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
template<class T = GLfloat>
#else
template<class T>
#endif
using AbstractCamera2D = AbstractCamera<2, T>;

/**
@brief Base for three-dimensional cameras

Convenience alternative to <tt>%AbstractCamera<3, T></tt>. See AbstractCamera
for more information.
@note Not available on GCC < 4.7. Use <tt>%AbstractCamera<3, T></tt> instead.
@see AbstractCamera2D
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
template<class T = GLfloat>
#else
template<class T>
#endif
using AbstractCamera3D = AbstractCamera<3, T>;
#endif

}}

#endif
