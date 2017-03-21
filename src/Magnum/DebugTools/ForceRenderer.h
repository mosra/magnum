#ifndef Magnum_DebugTools_ForceRenderer_h
#define Magnum_DebugTools_ForceRenderer_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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
 * @brief Class @ref Magnum::DebugTools::ForceRenderer, @ref Magnum::DebugTools::ForceRendererOptions, typedef @ref Magnum::DebugTools::ForceRenderer2D, @ref Magnum::DebugTools::ForceRenderer3D
 */

#include "Magnum/Resource.h"
#include "Magnum/Math/Color.h"
#include "Magnum/SceneGraph/Drawable.h"
#include "Magnum/Shaders/Shaders.h"
#include "Magnum/DebugTools/visibility.h"

namespace Magnum { namespace DebugTools {

/**
@brief Force renderer options

See @ref ForceRenderer documentation for more information.
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
         * Default is 100% opaque white.
         */
        ForceRendererOptions& setColor(const Color4& color) {
            _color = color;
            return *this;
        }

        /** @brief Scale of rendered arrow */
        constexpr Float scale() const { return _size; }

        /**
         * @brief Set scale of rendered arrow
         * @return Reference to self (for method chaining)
         *
         * Default is `1.0f`.
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

@anchor DebugTools-ForceRenderer-usage
## Basic usage

Example code:
@code
DebugTools::ResourceManager::instance().set("my", DebugTools::ForceRendererOptions()
    .setScale(5.0f)
    .setColor(Color3::fromHSV(120.0_degf, 1.0f, 0.7f));

// Create debug renderer for given object, use "my" options for it
Object3D* object;
Vector3 force;
new DebugTools::ForceRenderer2D(object, {0.3f, 1.5f, -0.7f}, &force, "my", debugDrawables);
@endcode

@see @ref ForceRenderer2D, @ref ForceRenderer3D, @ref ForceRendererOptions
*/
template<UnsignedInt dimensions> class MAGNUM_DEBUGTOOLS_EXPORT ForceRenderer: public SceneGraph::Drawable<dimensions, Float> {
    public:
        /**
         * @brief Constructor
         * @param object        Object for which to create debug renderer
         * @param forcePosition Where to render the force, relative to object
         * @param force         Force vector
         * @param options       Options resource key. See
         *      @ref DebugTools-ForceRenderer-usage "class documentation" for
         *      more information.
         * @param drawables     Drawable group
         *
         * The renderer is automatically added to object's features, @p force is
         * saved as reference to original vector and thus it must be available
         * for the whole lifetime of the renderer.
         */
        explicit ForceRenderer(SceneGraph::AbstractObject<dimensions, Float>& object, const VectorTypeFor<dimensions, Float>& forcePosition, const VectorTypeFor<dimensions, Float>& force, ResourceKey options = ResourceKey(), SceneGraph::DrawableGroup<dimensions, Float>* drawables = nullptr);

        /**
         * You have to pass reference to existing force instance, as the
         * renderer uses the current value when rendering.
         */
        ForceRenderer(SceneGraph::AbstractObject<dimensions, Float>&, const VectorTypeFor<dimensions, Float>&, VectorTypeFor<dimensions, Float>&&, ResourceKey = ResourceKey(), SceneGraph::DrawableGroup<dimensions, Float>* = nullptr) = delete;

        ~ForceRenderer();

    private:
        void draw(const MatrixTypeFor<dimensions, Float>& transformationMatrix, SceneGraph::Camera<dimensions, Float>& camera) override;

        const VectorTypeFor<dimensions, Float> _forcePosition;
        const VectorTypeFor<dimensions, Float>& _force;

        Resource<ForceRendererOptions> _options;
        Resource<AbstractShaderProgram, Shaders::Flat<dimensions>> _shader;
        Resource<Mesh> _mesh;
        Resource<Buffer> _vertexBuffer, _indexBuffer;
};

/** @brief Two-dimensional force renderer */
typedef ForceRenderer<2> ForceRenderer2D;

/** @brief Three-dimensional force renderer */
typedef ForceRenderer<3> ForceRenderer3D;

}}

#endif
