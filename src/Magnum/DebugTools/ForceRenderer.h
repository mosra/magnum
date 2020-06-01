#ifndef Magnum_DebugTools_ForceRenderer_h
#define Magnum_DebugTools_ForceRenderer_h
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
 * @brief Class @ref Magnum::DebugTools::ForceRenderer, @ref Magnum::DebugTools::ForceRendererOptions, typedef @ref Magnum::DebugTools::ForceRenderer2D, @ref Magnum::DebugTools::ForceRenderer3D
 */
#endif

#include "Magnum/Resource.h"
#include "Magnum/DebugTools/DebugTools.h"
#include "Magnum/DebugTools/visibility.h"
#include "Magnum/GL/GL.h"
#include "Magnum/Math/Color.h"
#include "Magnum/SceneGraph/Drawable.h"
#include "Magnum/Shaders/Shaders.h"

#ifdef MAGNUM_TARGET_GL
namespace Magnum { namespace DebugTools {

/**
@brief Force renderer options

See @ref ForceRenderer documentation for more information.

@note This class is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL "TARGET_GL" and `WITH_SCENEGRAPH` enabled (done by
    default). See @ref building-features for more information.
*/
class ForceRendererOptions {
    public:
        constexpr ForceRendererOptions(): _color(1.0f), _size(1.0f) {}

        /** @brief Color of rendered arrow */
        constexpr Color4 color() const { return _color; }

        /**
         * @brief Set color of rendered arrow
         * @return Reference to self (for method chaining)
         *
         * Default is @cpp 0xffffffff_rgbaf @ce.
         */
        ForceRendererOptions& setColor(const Color4& color) {
            _color = color;
            return *this;
        }

        /** @brief Size of rendered arrow */
        constexpr Float size() const { return _size; }

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief @copybrief size()
         * @m_deprecated_since{2019,10} Use @ref size() instead.
         */
        constexpr CORRADE_DEPRECATED("use size() instead") Float scale() const { return _size; }
        #endif

        /**
         * @brief Set scale of rendered arrow
         * @return Reference to self (for method chaining)
         *
         * Default is @cpp 1.0f @ce.
         */
        ForceRendererOptions& setSize(Float size) {
            _size = size;
            return *this;
        }

    private:
        Color4 _color;
        Float _size;
};

/**
@brief Force renderer

Visualizes force pushing on object by an arrow of the same direction and size.
See @ref debug-tools-renderers for more information.

@section DebugTools-ForceRenderer-usage Basic usage

Example code --- note that the @p force is saved as a reference to the original
vector (so when you update it, the renderer updates itself as well), and thus
it must be available for the whole lifetime of the renderer. The renderer is
automatically added to object's features so you don't need to keep a reference
to it.

@snippet MagnumDebugTools-gl.cpp ForceRenderer

@note This class is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL "TARGET_GL" and `WITH_SCENEGRAPH` enabled (done by
    default). See @ref building-features for more information.

@see @ref ForceRenderer2D, @ref ForceRenderer3D, @ref ForceRendererOptions
*/
template<UnsignedInt dimensions> class ForceRenderer: public SceneGraph::Drawable<dimensions, Float> {
    public:
        /**
         * @brief Constructor
         * @param manager       Resource manager instance
         * @param object        Object for which to create debug renderer
         * @param forcePosition Where to render the force, relative to object
         * @param force         Reference to the force vector
         * @param options       Options resource key. See
         *      @ref DebugTools-ForceRenderer-usage "class documentation" for
         *      more information.
         * @param drawables     Drawable group
         */
        explicit ForceRenderer(ResourceManager& manager, SceneGraph::AbstractObject<dimensions, Float>& object, const VectorTypeFor<dimensions, Float>& forcePosition, const VectorTypeFor<dimensions, Float>& force, ResourceKey options = ResourceKey(), SceneGraph::DrawableGroup<dimensions, Float>* drawables = nullptr);

        /**
         * You have to pass a reference to an external force vector --- the
         * renderer doesn't store a copy.
         */
        explicit ForceRenderer(ResourceManager&, SceneGraph::AbstractObject<dimensions, Float>&, const VectorTypeFor<dimensions, Float>&, VectorTypeFor<dimensions, Float>&&, ResourceKey = ResourceKey(), SceneGraph::DrawableGroup<dimensions, Float>* = nullptr) = delete;

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Constructor
         * @m_deprecated_since{2019,10} Implicit @ref ResourceManager singleton
         *      is deprecated, use @ref ForceRenderer(ResourceManager&, SceneGraph::AbstractObject<dimensions, Float>&, const VectorTypeFor<dimensions, Float>&, const VectorTypeFor<dimensions, Float>&, ResourceKey, SceneGraph::DrawableGroup<dimensions, Float>*)
         *      instead.
         */
        explicit CORRADE_DEPRECATED("implicit ResourceManager singleton is deprecated, use a constructor with explicit ResourceManager reference instead") ForceRenderer(SceneGraph::AbstractObject<dimensions, Float>& object, const VectorTypeFor<dimensions, Float>& forcePosition, const VectorTypeFor<dimensions, Float>& force, ResourceKey options = ResourceKey(), SceneGraph::DrawableGroup<dimensions, Float>* drawables = nullptr);

        #ifndef DOXYGEN_GENERATOR_OUTPUT
        explicit CORRADE_DEPRECATED("implicit ResourceManager singleton is deprecated, use a constructor with explicit DebugTools::ResourceManager reference instead") ForceRenderer(SceneGraph::AbstractObject<dimensions, Float>&, const VectorTypeFor<dimensions, Float>&, VectorTypeFor<dimensions, Float>&&, ResourceKey = ResourceKey(), SceneGraph::DrawableGroup<dimensions, Float>* = nullptr) = delete;
        #endif
        #endif

        ~ForceRenderer();

    private:
        void draw(const MatrixTypeFor<dimensions, Float>& transformationMatrix, SceneGraph::Camera<dimensions, Float>& camera) override;

        const VectorTypeFor<dimensions, Float> _forcePosition;
        const VectorTypeFor<dimensions, Float>& _force;

        Resource<ForceRendererOptions> _options;
        Resource<GL::AbstractShaderProgram, Shaders::Flat<dimensions>> _shader;
        Resource<GL::Mesh> _mesh;
};

/** @brief Two-dimensional force renderer */
typedef ForceRenderer<2> ForceRenderer2D;

/** @brief Three-dimensional force renderer */
typedef ForceRenderer<3> ForceRenderer3D;

}}
#else
#error this header is available only in the OpenGL build
#endif

#endif
