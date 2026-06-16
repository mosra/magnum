#ifndef Magnum_DebugTools_ForceRenderer_h
#define Magnum_DebugTools_ForceRenderer_h
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
 * @brief Class @ref Magnum::DebugTools::ForceRenderer, @ref Magnum::DebugTools::ForceRendererOptions, typedef @ref Magnum::DebugTools::ForceRenderer2D, @ref Magnum::DebugTools::ForceRenderer3D
 * @m_deprecated_since_latest Use @ref Magnum/Primitives/Arrow.h and
 *      @ref Primitives::arrow2D() or @relativeref{Primitives,arrow3D()} in a
 *      custom drawable instead
 */
#endif

#include "Magnum/configure.h"

#if defined(MAGNUM_BUILD_DEPRECATED) && defined(MAGNUM_TARGET_GL)
#include "Magnum/Resource.h"
#include "Magnum/DebugTools/DebugTools.h"
#include "Magnum/DebugTools/visibility.h"
#include "Magnum/GL/GL.h"
#include "Magnum/Math/Color.h"
#include "Magnum/SceneGraph/Drawable.h"
#include "Magnum/Shaders/Shaders.h"

#ifndef _MAGNUM_NO_DEPRECATED_FORCERENDERER
CORRADE_DEPRECATED_FILE("use Magnum/Primitives/Arrow.h and Primitives::arrow2D() or arrow3D() in a custom drawable instead")
#endif

namespace Magnum { namespace DebugTools {

/**
@brief Force renderer options
@m_deprecated_since_latest Use @ref Primitives::arrow2D() or
    @relativeref{Primitives,arrow3D()} in a custom drawable instead

See @ref ForceRenderer documentation for more information.

@note This class is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL "TARGET_GL" and `MAGNUM_WITH_SCENEGRAPH` enabled
    (done by default). See @ref building-features for more information.
*/
class CORRADE_DEPRECATED("use Primitives::arrow2D() or arrow3D() in a custom drawable instead") ForceRendererOptions {
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
        CORRADE_IGNORE_DEPRECATED_PUSH /* GCC 4.8 warns due to the return type */
        ForceRendererOptions& setColor(const Color4& color) {
            _color = color;
            return *this;
        }
        CORRADE_IGNORE_DEPRECATED_POP

        /** @brief Size of rendered arrow */
        constexpr Float size() const { return _size; }

        /**
         * @brief Set scale of rendered arrow
         * @return Reference to self (for method chaining)
         *
         * Default is @cpp 1.0f @ce.
         */
        CORRADE_IGNORE_DEPRECATED_PUSH /* GCC 4.8 warns due to the return type */
        ForceRendererOptions& setSize(Float size) {
            _size = size;
            return *this;
        }
        CORRADE_IGNORE_DEPRECATED_POP

    private:
        Color4 _color;
        Float _size;
};

/**
@brief Force renderer
@m_deprecated_since_latest Use @ref Primitives::arrow2D() or
    @relativeref{Primitives,arrow3D()} in a custom drawable instead

Visualizes force pushing on object by an arrow of the same direction and size.

@section DebugTools-ForceRenderer-usage Basic usage

Example code --- note that the @p force is saved as a reference to the original
vector (so when you update it, the renderer updates itself as well), and thus
it must be available for the whole lifetime of the renderer. The renderer is
automatically added to object's features so you don't need to keep a reference
to it.

@snippet DebugTools-gl.cpp ForceRenderer

@note This class is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL "TARGET_GL" and `MAGNUM_WITH_SCENEGRAPH` enabled
    (done by default). See @ref building-features for more information.

@see @ref ForceRenderer2D, @ref ForceRenderer3D, @ref ForceRendererOptions
*/
template<UnsignedInt dimensions> class CORRADE_DEPRECATED("use Primitives::arrow2D() or arrow3D() in a custom drawable instead") ForceRenderer: public SceneGraph::Drawable<dimensions, Float> {
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
        CORRADE_IGNORE_DEPRECATED_PUSH /* GCC 4.8 warns due to the argument */
        explicit ForceRenderer(ResourceManager& manager, SceneGraph::AbstractObject<dimensions, Float>& object, const VectorTypeFor<dimensions, Float>& forcePosition, const VectorTypeFor<dimensions, Float>& force, ResourceKey options = ResourceKey(), SceneGraph::DrawableGroup<dimensions, Float>* drawables = nullptr);
        CORRADE_IGNORE_DEPRECATED_POP

        /**
         * You have to pass a reference to an external force vector --- the
         * renderer doesn't store a copy.
         */
        CORRADE_IGNORE_DEPRECATED_PUSH /* GCC 4.8 warns due to the argument */
        explicit ForceRenderer(ResourceManager&, SceneGraph::AbstractObject<dimensions, Float>&, const VectorTypeFor<dimensions, Float>&, VectorTypeFor<dimensions, Float>&&, ResourceKey = ResourceKey(), SceneGraph::DrawableGroup<dimensions, Float>* = nullptr) = delete;
        CORRADE_IGNORE_DEPRECATED_POP

        ~ForceRenderer();

    private:
        void draw(const MatrixTypeFor<dimensions, Float>& transformationMatrix, SceneGraph::Camera<dimensions, Float>& camera) override;

        const VectorTypeFor<dimensions, Float> _forcePosition;
        const VectorTypeFor<dimensions, Float>& _force;

        CORRADE_IGNORE_DEPRECATED_PUSH /* GCC 4.8 warns due to the type */
        Resource<ForceRendererOptions> _options;
        CORRADE_IGNORE_DEPRECATED_POP
        Resource<GL::AbstractShaderProgram, Shaders::FlatGL<dimensions>> _shader;
        Resource<GL::Mesh> _mesh;
};

/**
@brief Two-dimensional force renderer
@m_deprecated_since_latest Use @ref Primitives::arrow2D() or
    @relativeref{Primitives,arrow3D()} in a custom drawable instead

@note This typedef is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.
*/
CORRADE_IGNORE_DEPRECATED_PUSH /* MSVC warns here */
typedef CORRADE_DEPRECATED("use Primitives::arrow2D() in a custom drawable instead") ForceRenderer<2> ForceRenderer2D;
CORRADE_IGNORE_DEPRECATED_POP

/**
@brief Three-dimensional force renderer
@m_deprecated_since_latest Use @ref Primitives::arrow2D() or
    @relativeref{Primitives,arrow3D()} in a custom drawable instead

@note This typedef is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.
*/
CORRADE_IGNORE_DEPRECATED_PUSH /* MSVC warns here */
typedef CORRADE_DEPRECATED("use Primitives::arrow3D() in a custom drawable instead") ForceRenderer<3> ForceRenderer3D;
CORRADE_IGNORE_DEPRECATED_POP

}}
#elif !defined(MAGNUM_BUILD_DEPRECATED)
#error use Magnum/Primitives/Arrow.h and Primitives::arrow2D() or arrow3D() in a custom drawable instead
#elif !defined(MAGNUM_TARGET_GL)
#error this header is available only in the OpenGL build
#endif

#endif
