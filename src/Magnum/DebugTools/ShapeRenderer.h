#ifndef Magnum_DebugTools_ShapeRenderer_h
#define Magnum_DebugTools_ShapeRenderer_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
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

#ifdef MAGNUM_TARGET_GL
/** @file
@brief Class @ref Magnum::DebugTools::ShapeRenderer, @ref Magnum::DebugTools::ShapeRendererOptions, typedef @ref Magnum::DebugTools::ShapeRenderer2D, @ref Magnum::DebugTools::ShapeRenderer3D

@deprecated The @ref Magnum::Shapes library is a failed design experiment and
    is scheduled for removal in a future release. Related geometry algorithms
    were moved to @ref Magnum::Math::Distance and @ref Magnum::Math::Intersection;
    if you need a full-fledged physics library, please have look at
    [Bullet](https://bulletphysics.org), which has Magnum integration in
    @ref Magnum::BulletIntegration, or at [Box2D](https://box2d.org/), which
    has a @ref examples-box2d "Magnum example" as well.
*/
#endif

#include "Magnum/Resource.h"
#include "Magnum/Math/Color.h"
#include "Magnum/SceneGraph/Drawable.h"
#include "Magnum/Shapes/Shapes.h"
#include "Magnum/Shapes/shapeImplementation.h"
#include "Magnum/DebugTools/DebugTools.h"
#include "Magnum/DebugTools/visibility.h"

#ifndef MAGNUM_BUILD_DEPRECATED
#error the Shapes library is scheduled for removal, see the docs for alternatives
#endif

/* I still have a test for this class and it shouldn't pollute the log there */
#ifndef _MAGNUM_DO_NOT_WARN_DEPRECATED_SHAPES
CORRADE_DEPRECATED_FILE("the Shapes library is scheduled for removal, see the docs for alternatives")
#endif

#ifdef MAGNUM_TARGET_GL
namespace Magnum { namespace DebugTools {

CORRADE_IGNORE_DEPRECATED_PUSH
template<UnsignedInt> class ShapeRenderer;

namespace Implementation {
    template<UnsignedInt> class AbstractShapeRenderer;

    template<UnsignedInt dimensions> void createDebugMesh(ShapeRenderer<dimensions>& renderer, const Shapes::Implementation::AbstractShape<dimensions>& shape);
}

/**
@brief Shape renderer options

@deprecated The @ref Shapes library is a failed design experiment and is
    scheduled for removal in a future release. Related geometry algorithms were
    moved to @ref Math::Distance and @ref Math::Intersection; if you need a
    full-fledged physics library, please have look at [Bullet](https://bulletphysics.org),
    which has Magnum integration in @ref BulletIntegration, or at
    [Box2D](https://box2d.org/), which has a @ref examples-box2d "Magnum example"
    as well.

See @ref ShapeRenderer documentation for more information.

@note This class is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.
*/
class CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") ShapeRendererOptions {
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
         * Default is @cpp 0xffffffff_rgbaf @ce.
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
         * Default is @cpp 0.25f @ce.
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

@deprecated The @ref Shapes library is a failed design experiment and is
    scheduled for removal in a future release. Related geometry algorithms were
    moved to @ref Math::Distance and @ref Math::Intersection; if you need a
    full-fledged physics library, please have look at [Bullet](https://bulletphysics.org),
    which has Magnum integration in @ref BulletIntegration, or at
    [Box2D](https://box2d.org/), which has a @ref examples-box2d "Magnum example"
    as well.

Visualizes collision shapes using wireframe primitives. See
@ref debug-tools-renderers for more information.

@section DebugTools-ShapeRenderer-usage Basic usage

Example code:

@code{.cpp}
// Create some options
DebugTools::ResourceManager::instance().set("red",
    DebugTools::ShapeRendererOptions().setColor({1.0f, 0.0f, 0.0f}));

// Create debug renderer for given shape, use "red" options for it
Shapes::AbstractShape2D* shape;
new DebugTools::ShapeRenderer2D(shape, "red", debugDrawables);
@endcode

@note This class is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.

@see @ref ShapeRenderer2D, @ref ShapeRenderer3D, @ref ShapeRendererOptions

@todo Different drawing style for inverted shapes? (marking the "inside" somehow)
*/
template<UnsignedInt dimensions> class CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") MAGNUM_DEBUGTOOLS_EXPORT ShapeRenderer: public SceneGraph::Drawable<dimensions, Float> {
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

/**
@brief Two-dimensional shape renderer

@deprecated The @ref Shapes library is a failed design experiment and is
    scheduled for removal in a future release. Related geometry algorithms were
    moved to @ref Math::Distance and @ref Math::Intersection; if you need a
    full-fledged physics library, please have look at [Bullet](https://bulletphysics.org),
    which has Magnum integration in @ref BulletIntegration, or at
    [Box2D](https://box2d.org/), which has a @ref examples-box2d "Magnum example"
    as well.
*/
typedef CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") ShapeRenderer<2> ShapeRenderer2D;

/**
@brief Three-dimensional shape renderer

@deprecated The @ref Shapes library is a failed design experiment and is
    scheduled for removal in a future release. Related geometry algorithms were
    moved to @ref Math::Distance and @ref Math::Intersection; if you need a
    full-fledged physics library, please have look at [Bullet](https://bulletphysics.org),
    which has Magnum integration in @ref BulletIntegration, or at
    [Box2D](https://box2d.org/), which has a @ref examples-box2d "Magnum example"
    as well.
*/
typedef CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") ShapeRenderer<3> ShapeRenderer3D;
CORRADE_IGNORE_DEPRECATED_POP

}}
#else
#error this header is available only in the OpenGL build
#endif

#endif
