#ifndef Magnum_DebugTools_ObjectRenderer_h
#define Magnum_DebugTools_ObjectRenderer_h
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
 * @brief Class Magnum::DebugTools::ObjectRenderer, Magnum::DebugTools::ObjectRendererOptions, typedef Magnum::DebugTools::ObjectRenderer2D, Magnum::DebugTools::ObjectRenderer3D
 */

#include "Resource.h"
#include "SceneGraph/Drawable.h"
#include "Shaders/Shaders.h"

#include "magnumDebugToolsVisibility.h"

namespace Magnum { namespace DebugTools {

/**
@brief Object renderer options

See ObjectRenderer documentation for more information.
*/
class ObjectRendererOptions {
    public:
        inline constexpr ObjectRendererOptions(): _size(1.0f) {}

        /** @brief Size of the rendered axes */
        inline constexpr GLfloat size() const { return _size; }

        /**
         * @brief Set size of the rendered axes
         * @return Pointer to self (for method chaining)
         *
         * Default is `1.0f`.
         */
        inline ObjectRendererOptions* setSize(GLfloat size) {
            _size = size;
            return this;
        }

    private:
        GLfloat _size;
};

/**
@brief Object renderer

Visualizes object position, rotation and scale using colored axes. See
@ref debug-tools-renderers for more information.

@section ObjectRenderer-usage Basic usage

Example code:
@code
// Create some options
DebugTools::ResourceManager::instance()->set("my",
    (new DebugTools::ObjectRendererOptions())->setSize(0.3f));

// Create debug renderer for given object, use "my" options for it
Object3D* object;
new DebugTools::ObjectRenderer2D(object, "my", debugDrawables);
@endcode

@see ObjectRenderer2D, ObjectRenderer3D
*/
template<std::uint8_t dimensions> class MAGNUM_DEBUGTOOLS_EXPORT ObjectRenderer: public SceneGraph::Drawable<dimensions> {
    public:
        /**
         * @brief Constructor
         * @param object    Object for which to create debug renderer
         * @param options   Options resource key. See
         *      @ref ObjectRenderer-usage "class documentation" for more
         *      information.
         * @param drawables Drawable group
         *
         * The renderer is automatically added to object's features.
         */
        explicit ObjectRenderer(SceneGraph::AbstractObject<dimensions>* object, ResourceKey options = ResourceKey(), SceneGraph::DrawableGroup<dimensions>* drawables = nullptr);

    protected:
        /** @todoc Remove GLfloat when Doxygen properly treats this as override */
        void draw(const typename DimensionTraits<dimensions, GLfloat>::MatrixType& transformationMatrix, SceneGraph::AbstractCamera<dimensions, GLfloat>* camera) override;

    private:
        Resource<ObjectRendererOptions> options;
        Resource<AbstractShaderProgram, Shaders::VertexColorShader<dimensions>> shader;
        Resource<Mesh> mesh;
        Resource<Buffer> vertexBuffer, indexBuffer;
};

/** @brief Two-dimensional object renderer */
typedef ObjectRenderer<2> ObjectRenderer2D;

/** @brief Three-dimensional object renderer */
typedef ObjectRenderer<3> ObjectRenderer3D;

}}

#endif
