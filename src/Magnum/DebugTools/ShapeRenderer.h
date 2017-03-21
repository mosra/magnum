#ifndef Magnum_DebugTools_ShapeRenderer_h
#define Magnum_DebugTools_ShapeRenderer_h
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
 * @brief Class @ref Magnum::DebugTools::ShapeRenderer, @ref Magnum::DebugTools::ShapeRendererOptions, typedef @ref Magnum::DebugTools::ShapeRenderer2D, @ref Magnum::DebugTools::ShapeRenderer3D
 */

#include "Magnum/Resource.h"
#include "Magnum/Math/Color.h"
#include "Magnum/SceneGraph/Drawable.h"
#include "Magnum/Shapes/Shapes.h"
#include "Magnum/Shapes/shapeImplementation.h"
#include "Magnum/DebugTools/visibility.h"

namespace Magnum { namespace DebugTools {

template<UnsignedInt> class ShapeRenderer;

namespace Implementation {
    template<UnsignedInt> class AbstractShapeRenderer;

    template<UnsignedInt dimensions> void createDebugMesh(ShapeRenderer<dimensions>& renderer, const Shapes::Implementation::AbstractShape<dimensions>& shape);
}

/**
@brief Shape renderer options

See @ref ShapeRenderer documentation for more information.
*/
class ShapeRendererOptions {
    public:
        /**
         * @brief Shape rendering mode
         *
         * @see @ref setRenderMode()
         */
        enum class RenderMode: UnsignedByte {
            Wireframe,      /**< Wireframe rendering */
            Solid           /**< Solid rendering */
        };

        constexpr ShapeRendererOptions(): _color(1.0f), _pointSize(0.25f), _renderMode(RenderMode::Wireframe) {}

        /** @brief Shape rendering mode */
        constexpr RenderMode renderMode() const { return _renderMode; }

        /**
         * @brief Set shape rendering mode
         * @return Reference to self (for method chaining)
         *
         * Default is @ref RenderMode::Wireframe.
         */
        ShapeRendererOptions& setRenderMode(RenderMode mode) {
            _renderMode = mode;
            return *this;
        }

        /** @brief Color of rendered shape */
        constexpr Color4 color() const { return _color; }

        /**
         * @brief Set color of rendered shape
         * @return Reference to self (for method chaining)
         *
         * Default is 100% opaque white.
         */
        ShapeRendererOptions& setColor(const Color4& color) {
            _color = color;
            return *this;
        }

        /** @brief Point size */
        constexpr Float pointSize() const { return _pointSize; }

        /**
         * @brief Set point size
         * @return Reference to self (for method chaining)
         *
         * Size of rendered crosshairs, representing @ref Shapes::Point shapes.
         * Default is `0.25f`.
         */
        ShapeRendererOptions& setPointSize(Float size) {
            _pointSize = size;
            return *this;
        }

    private:
        Color4 _color;
        Float _pointSize;
        RenderMode _renderMode;
};

/**
@brief Shape renderer

Visualizes collision shapes using wireframe primitives. See
@ref debug-tools-renderers for more information.

@anchor DebugTools-ShapeRenderer-usage
## Basic usage

Example code:
@code
// Create some options
DebugTools::ResourceManager::instance().set("red",
    DebugTools::ShapeRendererOptions().setColor({1.0f, 0.0f, 0.0f}));

// Create debug renderer for given shape, use "red" options for it
Shapes::AbstractShape2D* shape;
new DebugTools::ShapeRenderer2D(shape, "red", debugDrawables);
@endcode

@see @ref ShapeRenderer2D, @ref ShapeRenderer3D, @ref ShapeRendererOptions

@todo Different drawing style for inverted shapes? (marking the "inside" somehow)
*/
template<UnsignedInt dimensions> class MAGNUM_DEBUGTOOLS_EXPORT ShapeRenderer: public SceneGraph::Drawable<dimensions, Float> {
    #ifndef DOXYGEN_GENERATING_OUTPUT
    friend void Implementation::createDebugMesh<>(ShapeRenderer<dimensions>&, const Shapes::Implementation::AbstractShape<dimensions>&);
    #endif

    public:
        /**
         * @brief Constructor
         * @param shape     Shape for which to create debug renderer
         * @param options   Options resource key. See
         *      @ref DebugTools-ShapeRenderer-usage "class documentation" for
         *      more information.
         * @param drawables Drawable group
         *
         * The renderer is automatically added to shape's object features,
         * @p shape must be available for the whole lifetime of the renderer
         * and if it is group, it must not change its internal structure.
         */
        explicit ShapeRenderer(Shapes::AbstractShape<dimensions>& shape, ResourceKey options = ResourceKey(), SceneGraph::DrawableGroup<dimensions, Float>* drawables = nullptr);

        ~ShapeRenderer();

    private:
        void draw(const MatrixTypeFor<dimensions, Float>& transformationMatrix, SceneGraph::Camera<dimensions, Float>& camera) override;

        Resource<ShapeRendererOptions> _options;
        std::vector<Implementation::AbstractShapeRenderer<dimensions>*> _renderers;
};

/** @brief Two-dimensional shape renderer */
typedef ShapeRenderer<2> ShapeRenderer2D;

/** @brief Three-dimensional shape renderer */
typedef ShapeRenderer<3> ShapeRenderer3D;

}}

#endif
