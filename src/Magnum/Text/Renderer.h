#ifndef Magnum_Text_Renderer_h
#define Magnum_Text_Renderer_h
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

/** @file Text/Renderer.h
 * @brief Class @ref Magnum::Text::AbstractRenderer, @ref Magnum::Text::Renderer, typedef @ref Magnum::Text::Renderer2D, @ref Magnum::Text::Renderer3D
 */

#include <string>
#include <tuple>
#include <vector>

#include "Magnum/Math/Range.h"
#include "Magnum/Buffer.h"
#include "Magnum/DimensionTraits.h"
#include "Magnum/Mesh.h"
#include "Magnum/Text/Text.h"
#include "Magnum/Text/Alignment.h"
#include "Magnum/Text/visibility.h"

#ifdef CORRADE_TARGET_EMSCRIPTEN
#include <Corrade/Containers/Array.h>
#endif

namespace Magnum { namespace Text {

/**
@brief Base for text renderers

Not meant to be used directly, see @ref Renderer for more information.
@see @ref Renderer2D, @ref Renderer3D
*/
class MAGNUM_TEXT_EXPORT AbstractRenderer {
    public:
        /**
         * @brief Render text
         * @param font          Font
         * @param cache         Glyph cache
         * @param size          Font size
         * @param text          Text to render
         * @param alignment     Text alignment
         *
         * Returns tuple with vertex positions, texture coordinates, indices
         * and rectangle spanning the rendered text.
         */
        static std::tuple<std::vector<Vector2>, std::vector<Vector2>, std::vector<UnsignedInt>, Range2D> render(AbstractFont& font, const GlyphCache& cache, Float size, const std::string& text, Alignment alignment = Alignment::LineLeft);

        /**
         * @brief Capacity for rendered glyphs
         *
         * @see @ref reserve()
         */
        UnsignedInt capacity() const { return _capacity; }

        /** @brief Rectangle spanning the rendered text */
        Range2D rectangle() const { return _rectangle; }

        /** @brief Vertex buffer */
        Buffer& vertexBuffer() { return _vertexBuffer; }

        /** @brief Index buffer */
        Buffer& indexBuffer() { return _indexBuffer; }

        /** @brief Mesh */
        Mesh& mesh() { return _mesh; }

        /**
         * @brief Reserve capacity for rendered glyphs
         *
         * Reallocates memory in buffers to hold @p glyphCount glyphs and
         * prefills index buffer. Consider using appropriate @p vertexBufferUsage
         * if the text will be changed frequently. Index buffer is changed
         * only by calling this function, thus @p indexBufferUsage generally
         * doesn't need to be so dynamic if the capacity won't be changed much.
         *
         * Initially zero capacity is reserved.
         * @see @ref capacity()
         */
        void reserve(UnsignedInt glyphCount, BufferUsage vertexBufferUsage, BufferUsage indexBufferUsage);

        /**
         * @brief Render text
         *
         * Renders the text to vertex buffer, reusing index buffer already
         * filled with @ref reserve(). Rectangle spanning the rendered text is
         * available through @ref rectangle().
         *
         * Initially no text is rendered.
         * @attention The capacity must be large enough to contain all glyphs,
         *      see @ref reserve() for more information.
         */
        void render(const std::string& text);

    #ifndef DOXYGEN_GENERATING_OUTPUT
    protected:
    #else
    private:
    #endif
        explicit MAGNUM_TEXT_LOCAL AbstractRenderer(AbstractFont& font, const GlyphCache& cache, Float size, Alignment alignment);

        ~AbstractRenderer();

        Mesh _mesh;
        Buffer _vertexBuffer, _indexBuffer;
        #ifdef CORRADE_TARGET_EMSCRIPTEN
        Containers::Array<UnsignedByte> _vertexBufferData, _indexBufferData;
        #endif

    private:
        AbstractFont& font;
        const GlyphCache& cache;
        Float size;
        Alignment _alignment;
        UnsignedInt _capacity;
        Range2D _rectangle;

        #if defined(MAGNUM_TARGET_GLES2) && !defined(CORRADE_TARGET_EMSCRIPTEN)
        typedef void*(*BufferMapImplementation)(Buffer&, GLsizeiptr);
        static MAGNUM_TEXT_LOCAL void* bufferMapImplementationFull(Buffer& buffer, GLsizeiptr length);
        static MAGNUM_TEXT_LOCAL void* bufferMapImplementationRange(Buffer& buffer, GLsizeiptr length);
        static BufferMapImplementation bufferMapImplementation;
        #else
        #ifndef CORRADE_TARGET_EMSCRIPTEN
        static
        #else
        MAGNUM_TEXT_LOCAL
        #endif
        void* bufferMapImplementation(Buffer& buffer, GLsizeiptr length);
        #endif

        #if defined(MAGNUM_TARGET_GLES2) && !defined(CORRADE_TARGET_EMSCRIPTEN)
        typedef void(*BufferUnmapImplementation)(Buffer&);
        static MAGNUM_TEXT_LOCAL void bufferUnmapImplementationDefault(Buffer& buffer);
        static MAGNUM_TEXT_LOCAL BufferUnmapImplementation bufferUnmapImplementation;
        #else
        #ifndef CORRADE_TARGET_EMSCRIPTEN
        static
        #else
        MAGNUM_TEXT_LOCAL
        #endif
        void bufferUnmapImplementation(Buffer& buffer);
        #endif
};

/**
@brief Text renderer

Lays out the text into mesh using given font. Use of ligatures, kerning etc.
depends on features supported by particular font and its layouter.

## Usage

Immutable text (e.g. menu items, credits) can be simply rendered using static
methods, returning result either as data arrays or as fully configured mesh.
The text can be then drawn as usual by configuring the shader and drawing the
mesh:
@code
// Font instance, received from plugin manager
std::unique_ptr<Text::AbstractFont> font;

// Configured glyph cache
Text::GlyphCache cache;

Shaders::Vector2D shader;
Buffer vertexBuffer, indexBuffer;
Mesh mesh;

// Render the text, centered
std::tie(mesh, std::ignore) = Text::Renderer2D::render(*font, cache, 0.15f,
    "Hello World!", vertexBuffer, indexBuffer, BufferUsage::StaticDraw, Text::Alignment::LineCenter);

// Draw the text on the screen
shader.setTransformationProjectionMatrix(projection)
    .setColor(Color3(1.0f))
    .setVectorTexture(glyphCache->texture());
mesh.draw(shader);
@endcode
See @ref render(AbstractFont&, const GlyphCache&, Float, const std::string&, Alignment) and
@ref render(AbstractFont&, const GlyphCache&, Float, const std::string&, Buffer&, Buffer&, BufferUsage, Alignment)
for more information.

While this method is sufficient for one-shot rendering of static texts, for
mutable texts (e.g. FPS counters, chat messages) there is another approach
that doesn't recreate everything on each text change:
@code
std::unique_ptr<Text::AbstractFont> font;
Text::GlyphCache cache;
Shaders::Vector2D shader;

// Initialize renderer and reserve memory for enough glyphs
Text::Renderer2D renderer(*font, cache, 0.15f);
renderer.reserve(32, BufferUsage::DynamicDraw, BufferUsage::StaticDraw, Text::Alignment::LineCenter);

// Update the text occasionally
renderer.render("Hello World Countdown: 10");

// Draw the text on the screen
shader.setTransformationProjectionMatrix(projection)
    .setColor(Color3(1.0f))
    .setVectorTexture(glyphCache->texture());
renderer.mesh().draw(shader);
@endcode

## Required OpenGL functionality

Mutable text rendering requires @extension{ARB,map_buffer_range} on desktop
OpenGL (also part of OpenGL ES 3.0). If neither @extension{EXT,map_buffer_range}
nor @extension{CHROMIUM,map_sub} is not available in ES 2.0, at least
@extension{OES,mapbuffer} must be supported for asynchronous buffer updates.
There is no similar extension in WebGL, thus plain (and slow) buffer updates
are used there.

@see @ref Renderer2D, @ref Renderer3D, @ref AbstractFont,
    @ref Shaders::AbstractVector
*/
template<UnsignedInt dimensions> class MAGNUM_TEXT_EXPORT Renderer: public AbstractRenderer {
    public:
        /**
         * @brief Render text
         * @param font          Font
         * @param cache         Glyph cache
         * @param size          Font size
         * @param text          Text to render
         * @param vertexBuffer  Buffer where to store vertices
         * @param indexBuffer   Buffer where to store indices
         * @param usage         Usage of vertex and index buffer
         * @param alignment     Text alignment
         *
         * Returns mesh prepared for use with @ref Shaders::AbstractVector
         * subclasses and rectangle spanning the rendered text.
         */
        static std::tuple<Mesh, Range2D> render(AbstractFont& font, const GlyphCache& cache, Float size, const std::string& text, Buffer& vertexBuffer, Buffer& indexBuffer, BufferUsage usage, Alignment alignment = Alignment::LineLeft);

        /**
         * @brief Constructor
         * @param font          Font
         * @param cache         Glyph cache
         * @param size          Font size
         * @param alignment     Text alignment
         */
        explicit Renderer(AbstractFont& font, const GlyphCache& cache, Float size, Alignment alignment = Alignment::LineLeft);
        Renderer(AbstractFont&, GlyphCache&&, Float, Alignment alignment = Alignment::LineLeft) = delete; /**< @overload */

        using AbstractRenderer::render;
};

/** @brief Two-dimensional text renderer */
typedef Renderer<2> Renderer2D;

/** @brief Three-dimensional text renderer */
typedef Renderer<3> Renderer3D;

}}

#endif
