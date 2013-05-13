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
 * @brief Class Magnum::SceneGraph::AbstractCamera, enum Magnum::SceneGraph::AspectRatioPolicy, alias Magnum::SceneGraph::AbstractCamera2D, Magnum::SceneGraph::AbstractCamera3D
 */

#include "Math/Matrix3.h"
#include "Math/Matrix4.h"
#include "AbstractFeature.h"

#include "magnumSceneGraphVisibility.h"

namespace Magnum { namespace SceneGraph {

/**
@brief Camera aspect ratio policy

@see AbstractCamera::setAspectRatioPolicy()
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

 - @ref AbstractCamera "AbstractCamera<2, Float>"
 - @ref AbstractCamera "AbstractCamera<3, Float>"

@see @ref scenegraph, Drawable, DrawableGroup, AbstractCamera2D, AbstractCamera3D
*/
#ifndef DOXYGEN_GENERATING_OUTPUT
template<UnsignedInt dimensions, class T>
#else
template<UnsignedInt dimensions, class T = Float>
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
template<class T = Float>
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
template<class T = Float>
#else
template<class T>
#endif
using AbstractCamera3D = AbstractCamera<3, T>;
#endif

}}

#endif
