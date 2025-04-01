#ifndef Magnum_Text_RendererGL_h
#define Magnum_Text_RendererGL_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
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
 * @brief Class @ref Magnum::Text::RendererGL, enum @ref Magnum::Text::RendererGLFlag, enum set @ref Magnum::Text::RendererGLFlags
 * @m_since_latest
 */

#include "Magnum/configure.h"

#ifdef MAGNUM_TARGET_GL
#include "Magnum/GL/GL.h"
#include "Magnum/Text/Renderer.h"

namespace Magnum { namespace Text {

/**
@brief OpenGL text renderer flag
@m_since_latest

A superset of @ref RendererFlag.

@note This enum is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.

@see @ref RendererGLFlags, @ref RendererGL
*/
/* Currently is the same as RendererFlag, but is made to a dedicated type to
   not cause a breaking change once GL-specific flags are introduced, such as
   buffer mapping */
enum class RendererGLFlag: UnsignedByte {
    /** @copydoc RendererFlag::GlyphPositionsClusters */
    GlyphPositionsClusters = UnsignedByte(RendererFlag::GlyphPositionsClusters)
};

/**
@debugoperatorenum{RendererGLFlag}
@m_since_latest

@note This function is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.
*/
MAGNUM_TEXT_EXPORT Debug& operator<<(Debug& output, RendererGLFlag value);

/**
@brief OpenGL text renderer flags
@m_since_latest

@note This enum set is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.

@see @ref RendererGL
*/
typedef Containers::EnumSet<RendererGLFlag> RendererGLFlags;

CORRADE_ENUMSET_OPERATORS(RendererGLFlags)

/**
@debugoperatorenum{RendererGLFlags}
@m_since_latest

@note This function is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.
*/
MAGNUM_TEXT_EXPORT Debug& operator<<(Debug& output, RendererGLFlags value);

/**
@brief OpenGL text renderer
@m_since_latest

Specialization of a @ref Renderer that uploads index and vertex data to a
@ref GL::Mesh. See the @ref Renderer class documentation for information about
setting up an instance of this class, filling it with data and drawing the text
with it.

@note This class is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.
*/
class MAGNUM_TEXT_EXPORT RendererGL: public Renderer {
    public:
        /**
         * @brief Construct
         * @param glyphCache    Glyph cache to use
         * @param flags         Opt-in feature flags
         *
         * Unlike with the @ref Renderer base, the OpenGL implementation needs
         * to have a complete control over memory layout and allocation and
         * thus it isn't possible to supply custom allocators. If you want the
         * control, use @ref Renderer with custom index and vertex allocators
         * and fill a @ref GL::Mesh instance with the data manually.
         */
        explicit RendererGL(const AbstractGlyphCache& glyphCache, RendererGLFlags flags = {});

        /**
         * @brief Construct without creating the internal state and the OpenGL objects
         *
         * The constructed instance is equivalent to moved-from state, i.e. no
         * APIs can be safely called on the object. Useful in cases where you
         * will overwrite the instance later anyway. Move another object over
         * it to make it useful.
         *
         * This function can be safely used for constructing (and later
         * destructing) objects even without any OpenGL context being active.
         * However note that this is a low-level and a potentially dangerous
         * API, see the documentation of @ref NoCreate for alternatives.
         */
        explicit RendererGL(NoCreateT) noexcept: Renderer{NoCreate} {}

        /** @brief Copying is not allowed */
        RendererGL(RendererGL&) = delete;

        /**
         * @brief Move constructor
         *
         * Performs a destructive move, i.e. the original object isn't usable
         * afterwards anymore.
         */
        RendererGL(RendererGL&&) noexcept;

        ~RendererGL();

        /** @brief Copying is not allowed */
        RendererGL& operator=(RendererGL&) = delete;

        /** @brief Move assignment */
        RendererGL& operator=(RendererGL&&) noexcept;

        /** @brief Flags */
        RendererGLFlags flags() const;

        /** @brief Mesh containing the rendered index and vertex data */
        GL::Mesh& mesh();
        const GL::Mesh& mesh() const; /**< @overload */

        /**
         * @brief Set index type
         * @return Reference to self (for method chaining)
         *
         * Calls @ref Renderer::setIndexType() and updates @ref mesh() with the
         * rendered index data, if different from before. Compared to
         * @ref Renderer, the default index type is
         * @ref MeshIndexType::UnsignedShort, not
         * @ref MeshIndexType::UnsignedByte, as use of 8-bit indices is
         * discouraged on contemporary GPUs.
         */
        RendererGL& setIndexType(MeshIndexType atLeast);

        /**
         * @brief Reserve capacity for given glyph count
         * @return Reference to self (for method chaining)
         *
         * Calls @ref Renderer::reserve() and updates @ref mesh() with the
         * rendered index data, if different from before.
         */
        RendererGL& reserve(UnsignedInt glyphCapacity, UnsignedInt runCapacity);

        /**
         * @brief Clear rendered glyphs, runs and vertices
         * @return Reference to self (for method chaining)
         *
         * Calls @ref Renderer::clear() and additionally also sets @ref mesh()
         * index count to @cpp 0 @ce.
         */
        RendererGL& clear();

        /**
         * @brief Reset internal renderer state
         * @return Reference to self (for method chaining)
         *
         * Calls @ref Renderer::reset(), and additionally also sets @ref mesh()
         * index count to @cpp 0 @ce.
         */
        RendererGL& reset();

        /**
         * @brief Wrap up rendering of all text added so far
         *
         * Calls @ref Renderer::render(), updates @ref mesh() with the newly
         * rendered vertex data and potentially updates also the index data, if
         * different from before.
         */
        Containers::Pair<Range2D, Range1Dui> render();

        /* Overloads to remove a WTF factor from method chaining order, and to
           ensure our render() is called instead of Render::render() */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        RendererGL& setCursor(const Vector2& cursor) {
            return static_cast<RendererGL&>(Renderer::setCursor(cursor));
        }
        RendererGL& setAlignment(Alignment alignment) {
            return static_cast<RendererGL&>(Renderer::setAlignment(alignment));
        }
        RendererGL& setLineAdvance(Float advance) {
            return static_cast<RendererGL&>(Renderer::setLineAdvance(advance));
        }
        RendererGL& setLayoutDirection(LayoutDirection direction) {
            return static_cast<RendererGL&>(Renderer::setLayoutDirection(direction));
        }

        RendererGL& add(AbstractShaper& shaper, Float size, Containers::StringView text, UnsignedInt begin, UnsignedInt end, Containers::ArrayView<const FeatureRange> features);
        RendererGL& add(AbstractShaper& shaper, Float size, Containers::StringView text, UnsignedInt begin, UnsignedInt end);
        RendererGL& add(AbstractShaper& shaper, Float size, Containers::StringView text, UnsignedInt begin, UnsignedInt end, std::initializer_list<FeatureRange> features);
        RendererGL& add(AbstractShaper& shaper, Float size, Containers::StringView text, Containers::ArrayView<const FeatureRange> features);
        RendererGL& add(AbstractShaper& shaper, Float size, Containers::StringView text);
        RendererGL& add(AbstractShaper& shaper, Float size, Containers::StringView text, std::initializer_list<FeatureRange> features);

        Containers::Pair<Range2D, Range1Dui> render(AbstractShaper& shaper, Float size, Containers::StringView text, Containers::ArrayView<const FeatureRange> features);
        Containers::Pair<Range2D, Range1Dui> render(AbstractShaper& shaper, Float size, Containers::StringView text);
        Containers::Pair<Range2D, Range1Dui> render(AbstractShaper& shaper, Float size, Containers::StringView text, std::initializer_list<FeatureRange> features);
        #endif

    private:
        struct State;

        /* Needs to extract the GL::Buffer instances from inside */
        #ifdef MAGNUM_BUILD_DEPRECATED
        CORRADE_IGNORE_DEPRECATED_PUSH
        friend AbstractRenderer;
        CORRADE_IGNORE_DEPRECATED_POP
        #endif
};

}}
#else
#error this header is available only in the OpenGL build
#endif

#endif
