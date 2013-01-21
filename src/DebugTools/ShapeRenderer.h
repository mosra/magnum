#ifndef Magnum_DebugTools_ShapeRenderer_h
#define Magnum_DebugTools_ShapeRenderer_h
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

#include "Color.h"
#include "Resource.h"
#include "SceneGraph/Drawable.h"
#include "Physics/Physics.h"

#include "magnumDebugToolsVisibility.h"

namespace Magnum { namespace DebugTools {

/** @todoc Remove `ifndef` when Doxygen is sane again */
#ifndef DOXYGEN_GENERATING_OUTPUT
template<std::uint8_t> class ShapeRenderer;
#endif

#ifndef DOXYGEN_GENERATING_OUTPUT
namespace Implementation {
    template<std::uint8_t> class AbstractShapeRenderer;

    template<std::uint8_t dimensions> void createDebugMesh(ShapeRenderer<dimensions>* renderer, Physics::AbstractShape<dimensions>* shape);
}
#endif

/**
@brief Shape renderer options

See ShapeRenderer documentation for more information.
*/
class ShapeRendererOptions {
    public:
        /** @brief Color of rendered shape */
        inline constexpr Color3<> color() const { return _color; }

        /**
         * @brief Set color of rendered shape
         * @return Pointer to self (for method chaining)
         *
         * Default is black.
         */
        inline ShapeRendererOptions* setColor(const Color3<>& color) {
            _color = color;
            return this;
        }

    private:
        Color3<> _color;
};

/**
@brief Shape renderer

Visualizes collision shape. See @ref debug-tools-renderers for more information.

@section ShapeRenderer-usage Basic usage

Example code:
@code
// Create some options
DebugTools::ResourceManager::instance()->set("red",
    (new DebugTools::ShapeRendererOptions())->setColor({1.0f, 0.0f, 0.0f}));

// Create debug renderer for given shape, use "red" options for it
Physics::ObjectShape2D* shape;
new DebugTools::ShapeRenderer2D(shape, "red", debugDrawables);
@endcode

@see ShapeRenderer2D, ShapeRenderer3D
*/
template<std::uint8_t dimensions> class MAGNUM_DEBUGTOOLS_EXPORT ShapeRenderer: public SceneGraph::Drawable<dimensions> {
    #ifndef DOXYGEN_GENERATING_OUTPUT
    friend void Implementation::createDebugMesh<>(ShapeRenderer<dimensions>*, Physics::AbstractShape<dimensions>*);
    #endif

    public:
        /**
         * @brief Constructor
         * @param shape     Object for which to create debug renderer
         * @param options   Options resource key. See
         *      @ref ShapeRenderer-usage "class documentation" for more
         *      information.
         * @param drawables Drawable group
         *
         * The renderer is automatically added to shape's object features,
         * @p shape must be available for the whole lifetime of the renderer.
         *
         * @attention Passed object must have assigned shape.
         */
        explicit ShapeRenderer(Physics::ObjectShape<dimensions>* shape, ResourceKey options = ResourceKey(), SceneGraph::DrawableGroup<dimensions>* drawables = nullptr);

        ~ShapeRenderer();

    protected:
        /** @todoc Remove GLfloat when Doxygen properly treats this as override */
        void draw(const typename DimensionTraits<dimensions, GLfloat>::MatrixType& transformationMatrix, SceneGraph::AbstractCamera<dimensions, GLfloat>* camera) override;

    private:
        Resource<ShapeRendererOptions> options;
        std::vector<Implementation::AbstractShapeRenderer<dimensions>*> renderers;
};

/** @brief Two-dimensional shape renderer */
typedef ShapeRenderer<2> ShapeRenderer2D;

/** @brief Three-dimensional shape renderer */
typedef ShapeRenderer<3> ShapeRenderer3D;

}}

#endif
