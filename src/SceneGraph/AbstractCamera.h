#ifndef Magnum_SceneGraph_AbstractCamera_h
#define Magnum_SceneGraph_AbstractCamera_h
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
 * @brief Class Magnum::SceneGraph::AbstractBasicCamera, enum Magnum::SceneGraph::AspectRatioPolicy, typedef Magnum::SceneGraph::AbstractCamera2D, Magnum::SceneGraph::AbstractCamera3D
 */

#include "Math/Matrix3.h"
#include "Math/Matrix4.h"
#include "AbstractFeature.h"

#include "magnumSceneGraphVisibility.h"

namespace Magnum { namespace SceneGraph {

/**
@brief Camera aspect ratio policy

@see AbstractBasicCamera::setAspectRatioPolicy()
*/
enum class AspectRatioPolicy: UnsignedByte {
    NotPreserved,   /**< Don't preserve aspect ratio (default) */
    Extend,         /**< Extend on larger side of view */
    Clip            /**< Clip on smaller side of view */
};

namespace Implementation {
    template<UnsignedInt dimensions, class T> typename DimensionTraits<dimensions, T>::MatrixType aspectRatioFix(AspectRatioPolicy aspectRatioPolicy, const Math::Vector2<T>& projectionScale, const Vector2i& viewport);
}

/**
@brief Base for cameras

See Drawable documentation for more information. This class is not directly
instantiatable, use Camera2D or Camera3D subclasses instead.

@section AbstractCamera-explicit-specializations Explicit template specializations

The following specialization are explicitly compiled into SceneGraph library.
For other specializations (e.g. using Double type) you have to use
AbstractCamera.hpp implementation file to avoid linker errors. See also
relevant sections in
@ref Camera2D-explicit-specializations "Camera2D" and
@ref Camera3D-explicit-specializations "Camera3D" class documentation or
@ref compilation-speedup-hpp for more information.

 - @ref AbstractBasicCamera "AbstractBasicCamera<2, Float>"
 - @ref AbstractBasicCamera "AbstractBasicCamera<3, Float>"

@see AbstractCamera2D, AbstractCamera3D, @ref scenegraph, Drawable, DrawableGroup
*/
template<UnsignedInt dimensions, class T> class MAGNUM_SCENEGRAPH_EXPORT AbstractBasicCamera: public AbstractFeature<dimensions, T> {
    public:
        /**
         * @brief Constructor
         * @param object        Object holding the camera
         */
        explicit AbstractBasicCamera(AbstractObject<dimensions, T>* object);

        virtual ~AbstractBasicCamera() = 0;

        /** @brief Aspect ratio policy */
        AspectRatioPolicy aspectRatioPolicy() const { return _aspectRatioPolicy; }

        /**
         * @brief Set aspect ratio policy
         * @return Pointer to self (for method chaining)
         */
        AbstractBasicCamera<dimensions, T>* setAspectRatioPolicy(AspectRatioPolicy policy);

        /**
         * @brief Camera matrix
         *
         * Camera matrix describes world position relative to the camera and is
         * applied as first.
         */
        typename DimensionTraits<dimensions, T>::MatrixType cameraMatrix() {
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
        typename DimensionTraits<dimensions, T>::MatrixType projectionMatrix() const { return _projectionMatrix; }

        /**
         * @brief Size of (near) XY plane in current projection
         *
         * Returns size of near XY plane computed from projection matrix.
         * @see projectionMatrix()
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
         * @see setAspectRatioPolicy()
         */
        virtual void setViewport(const Vector2i& size);

        /**
         * @brief Draw
         *
         * Draws given group of drawables.
         */
        virtual void draw(BasicDrawableGroup<dimensions, T>& group);

    protected:
        /** Recalculates camera matrix */
        void cleanInverted(const typename DimensionTraits<dimensions, T>::MatrixType& invertedAbsoluteTransformationMatrix) override {
            _cameraMatrix = invertedAbsoluteTransformationMatrix;
        }

        #ifndef DOXYGEN_GENERATING_OUTPUT
        void fixAspectRatio() {
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

/**
@brief Base camera for two-dimensional float scenes

@see AbstractCamera3D
*/
typedef AbstractBasicCamera<2, Float> AbstractCamera2D;

/**
@brief Base camera for three-dimensional float scenes

@see AbstractBasicCamera2D
*/
typedef AbstractBasicCamera<3, Float> AbstractCamera3D;

}}

#endif
