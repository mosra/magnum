#ifndef Magnum_DebugTools_ObjectRenderer_h
#define Magnum_DebugTools_ObjectRenderer_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025, 2026
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

#if defined(MAGNUM_BUILD_DEPRECATED) && defined(MAGNUM_TARGET_GL)
/** @file
 * @brief Class @ref Magnum::DebugTools::ObjectRenderer, @ref Magnum::DebugTools::ObjectRendererOptions, typedef @ref Magnum::DebugTools::ObjectRenderer2D, @ref Magnum::DebugTools::ObjectRenderer3D
 * @m_deprecated_since_latest Use @ref Magnum/Primitives/Axis.h and
 *      @ref Primitives::axis2D() or @relativeref{Primitives,axis3D()} in a
 *      custom drawable instead
 */
#endif

#include "Magnum/configure.h"

#ifdef MAGNUM_TARGET_GL
#include "Magnum/Resource.h"
#include "Magnum/DebugTools/DebugTools.h"
#include "Magnum/DebugTools/visibility.h"
#include "Magnum/GL/GL.h"
#include "Magnum/SceneGraph/Drawable.h"
#include "Magnum/Shaders/Shaders.h"

#ifndef _MAGNUM_NO_DEPRECATED_OBJECTRENDERER
CORRADE_DEPRECATED_FILE("use Magnum/Primitives/Axis.h and Primitives::axis2D() or axis3D() in a custom drawable instead")
#endif

namespace Magnum { namespace DebugTools {

/**
@brief Object renderer options
@m_deprecated_since_latest Use @ref Primitives::axis2D() or
    @relativeref{Primitives,axis3D()} in a custom drawable instead

See @ref ObjectRenderer documentation for more information.

@note This class is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL and `MAGNUM_WITH_SCENEGRAPH` enabled (done by
    default). See @ref building-features for more information.
*/
class CORRADE_DEPRECATED("use Primitives::axis2D() or axis3D() in a custom drawable instead") ObjectRendererOptions {
    public:
        constexpr ObjectRendererOptions(): _size(1.0f) {}

        /** @brief Size of the rendered axes */
        constexpr Float size() const { return _size; }

        /**
         * @brief Set size of the rendered axes
         * @return Reference to self (for method chaining)
         *
         * Default is @cpp 1.0f @ce.
         */
        CORRADE_IGNORE_DEPRECATED_PUSH /* GCC 4.8 warns due to the return type */
        ObjectRendererOptions& setSize(Float size) {
            _size = size;
            return *this;
        }
        CORRADE_IGNORE_DEPRECATED_POP

    private:
        Float _size;
};

/**
@brief Object renderer
@m_deprecated_since_latest Use @ref Primitives::axis2D() or
    @relativeref{Primitives,axis3D()} in a custom drawable instead

Visualizes object position, rotation and scale using colored axes.

@section DebugTools-ObjectRenderer-usage Basic usage

Example code:

@snippet DebugTools-gl.cpp ObjectRenderer

@note This class is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL and `MAGNUM_WITH_SCENEGRAPH` enabled (done by
    default). See @ref building-features for more information.

@see @ref ObjectRenderer2D, @ref ObjectRenderer3D, @ref ObjectRendererOptions
*/
template<UnsignedInt dimensions> class CORRADE_DEPRECATED("use Primitives::axis2D() or axis3D() in a custom drawable instead") ObjectRenderer: public SceneGraph::Drawable<dimensions, Float> {
    public:
        /**
         * @brief Constructor
         * @param manager   Resource manager instance
         * @param object    Object for which to create debug renderer
         * @param options   Options resource key. See
         *      @ref DebugTools-ObjectRenderer-usage "class documentation" for
         *      more information.
         * @param drawables Drawable group
         *
         * The renderer is automatically added to object's features.
         */
        CORRADE_IGNORE_DEPRECATED_PUSH /* GCC 4.8 warns due to the argument */
        explicit ObjectRenderer(ResourceManager& manager, SceneGraph::AbstractObject<dimensions, Float>& object, ResourceKey options = ResourceKey(), SceneGraph::DrawableGroup<dimensions, Float>* drawables = nullptr);
        CORRADE_IGNORE_DEPRECATED_POP

        CORRADE_IGNORE_DEPRECATED_PUSH /* MSVC warns here (?!) */
        ~ObjectRenderer();
        CORRADE_IGNORE_DEPRECATED_POP

    private:
        void draw(const MatrixTypeFor<dimensions, Float>& transformationMatrix, SceneGraph::Camera<dimensions, Float>& camera) override;

        CORRADE_IGNORE_DEPRECATED_PUSH /* GCC 4.8 warns here */
        Resource<ObjectRendererOptions> _options;
        Resource<GL::AbstractShaderProgram, Shaders::VertexColorGL<dimensions>> _shader;
        Resource<GL::Mesh> _mesh;
        CORRADE_IGNORE_DEPRECATED_POP
};

/**
@brief Two-dimensional object renderer

@note This typedef is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.
*/
CORRADE_IGNORE_DEPRECATED_PUSH /* MSVC warns here */
typedef CORRADE_DEPRECATED("use Primitives::axis2D() in a custom drawable instead") ObjectRenderer<2> ObjectRenderer2D;
CORRADE_IGNORE_DEPRECATED_POP

/**
@brief Three-dimensional object renderer

@note This typedef is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.
*/
CORRADE_IGNORE_DEPRECATED_PUSH /* MSVC warns here */
typedef CORRADE_DEPRECATED("use Primitives::axis3D() in a custom drawable instead") ObjectRenderer<3> ObjectRenderer3D;
CORRADE_IGNORE_DEPRECATED_POP

}}
#elif !defined(MAGNUM_BUILD_DEPRECATED)
#error use Magnum/Primitives/Axis.h and Primitives::axis2D() or axis3D() in a custom drawable instead
#elif !defined(MAGNUM_TARGET_GL)
#error this header is available only in the OpenGL build
#endif

#endif
