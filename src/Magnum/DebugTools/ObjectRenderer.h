#ifndef Magnum_DebugTools_ObjectRenderer_h
#define Magnum_DebugTools_ObjectRenderer_h
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

#ifdef MAGNUM_TARGET_GL
/** @file
 * @brief Class @ref Magnum::DebugTools::ObjectRenderer, @ref Magnum::DebugTools::ObjectRendererOptions, typedef @ref Magnum::DebugTools::ObjectRenderer2D, @ref Magnum::DebugTools::ObjectRenderer3D
 */
#endif

#include "Magnum/Resource.h"
#include "Magnum/DebugTools/DebugTools.h"
#include "Magnum/DebugTools/visibility.h"
#include "Magnum/GL/GL.h"
#include "Magnum/SceneGraph/Drawable.h"
#include "Magnum/Shaders/Shaders.h"

#ifdef MAGNUM_TARGET_GL
namespace Magnum { namespace DebugTools {

/**
@brief Object renderer options

See @ref ObjectRenderer documentation for more information.

@note This class is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL "TARGET_GL" and `WITH_SCENEGRAPH` enabled (done by
    default). See @ref building-features for more information.
*/
class ObjectRendererOptions {
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
        ObjectRendererOptions& setSize(Float size) {
            _size = size;
            return *this;
        }

    private:
        Float _size;
};

/**
@brief Object renderer

Visualizes object position, rotation and scale using colored axes. See
@ref debug-tools-renderers for more information.

@section DebugTools-ObjectRenderer-usage Basic usage

Example code:

@snippet MagnumDebugTools-gl.cpp ObjectRenderer

@note This class is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL "TARGET_GL" and `WITH_SCENEGRAPH` enabled (done by
    default). See @ref building-features for more information.

@see @ref ObjectRenderer2D, @ref ObjectRenderer3D, @ref ObjectRendererOptions
*/
template<UnsignedInt dimensions> class ObjectRenderer: public SceneGraph::Drawable<dimensions, Float> {
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
        explicit ObjectRenderer(ResourceManager& manager, SceneGraph::AbstractObject<dimensions, Float>& object, ResourceKey options = ResourceKey(), SceneGraph::DrawableGroup<dimensions, Float>* drawables = nullptr);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Constructor
         * @m_deprecated_since{2019,10} Implicit @ref ResourceManager singleton
         *      is deprecated, use @ref ObjectRenderer(ResourceManager&, SceneGraph::AbstractObject<dimensions, Float>&, ResourceKey, SceneGraph::DrawableGroup<dimensions, Float>*)
         *      instead.
         */
        explicit CORRADE_DEPRECATED("implicit ResourceManager singleton is deprecated, use a constructor with explicit ResourceManager reference instead") ObjectRenderer(SceneGraph::AbstractObject<dimensions, Float>& object, ResourceKey options = ResourceKey(), SceneGraph::DrawableGroup<dimensions, Float>* drawables = nullptr);
        #endif

        ~ObjectRenderer();

    private:
        void draw(const MatrixTypeFor<dimensions, Float>& transformationMatrix, SceneGraph::Camera<dimensions, Float>& camera) override;

        Resource<ObjectRendererOptions> _options;
        Resource<GL::AbstractShaderProgram, Shaders::VertexColor<dimensions>> _shader;
        Resource<GL::Mesh> _mesh;
};

/** @brief Two-dimensional object renderer */
typedef ObjectRenderer<2> ObjectRenderer2D;

/** @brief Three-dimensional object renderer */
typedef ObjectRenderer<3> ObjectRenderer3D;

}}
#else
#error this header is available only in the OpenGL build
#endif

#endif
