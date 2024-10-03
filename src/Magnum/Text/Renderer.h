#ifndef Magnum_Text_Renderer_h
#define Magnum_Text_Renderer_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024
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
 * @brief Class @ref Magnum::Text::AbstractRenderer, @ref Magnum::Text::Renderer, typedef @ref Magnum::Text::Renderer2D, @ref Magnum::Text::Renderer3D, function @ref Magnum::Text::renderLineGlyphPositionsInto(), @ref Magnum::Text::renderGlyphQuadsInto(), @ref Magnum::Text::alignRenderedLine(), @ref Magnum::Text::alignRenderedBlock(), @ref Magnum::Text::renderGlyphQuadIndicesInto()
 */

#include "Magnum/Magnum.h"
#include "Magnum/Text/Text.h"
#include "Magnum/Text/visibility.h"

#ifdef MAGNUM_TARGET_GL
#include <string>
#include <tuple>
#include <vector>

#include "Magnum/DimensionTraits.h"
#include "Magnum/Math/Range.h"
#include "Magnum/GL/Buffer.h"
#include "Magnum/GL/Mesh.h"
#include "Magnum/Text/Alignment.h"

#ifdef CORRADE_TARGET_EMSCRIPTEN
#include <Corrade/Containers/Array.h>
#endif
#endif

namespace Magnum { namespace Text {

/**
@brief Render glyph positions for a (part of a) single line
@param[in] font             Font to query metrics from
@param[in] size             Size to render the glyphs at
@param[in] direction        Layout direction. Currently expected to always be
    @ref LayoutDirection::HorizontalTopToBottom.
@param[in] glyphOffsets     Glyph offsets coming from @ref AbstractShaper
    instance(s) associated with @p font
@param[in] glyphAdvances    Glyph advances coming from @ref AbstractShaper
    instance(s) associated with @p font
@param[in,out] cursor       Initial cursor position. Is updated to a final
    cursor position after all glyphs are rendered.
@param[out] glyphPositions  Where to put output absolute glyph positions
@return Rectangle spanning the rendered cursor range in one direction and font
    descent to ascent in the other
@m_since_latest

The output of this function are just glyph positions alone, which is useful for
example when the actual glyph quad expansion is done by a shader or when the
glyphs get subsequently rasterized some other way than applying a glyph texture
to a sequence of quads. Use @ref renderGlyphQuadsInto() on the resulting
@p glyphPositions array to form actual glyph quads together with texture
coordinates.

The @p glyphOffsets, @p glyphAdvances and @p glyphPositions views are all
expected to have the same size. It's possible to use the same view for
@p glyphOffsets and @p glyphPositions, which will turn the input relative glyph
offsets into absolute positions.

Calls to this function don't strictly need to match calls to
@ref AbstractShaper::shape(). For example if multiple text runs on a single
line differ just by script, language or direction but not by a font or
rendering size, they can be shaped into consecutive portions of a larger
@p glyphOffsets and @p glyphAdvances array and this function can be then called
just once for all runs together. If the font or rendering size changes between
text runs however, you have to call this function for each such run separately
and each time use the updated @p cursor value as an input for the next
@ref renderLineGlyphPositionsInto() call.

@m_class{m-note m-warning}

@par
    This function only works on a single line of text. When rendering a
    multi-line text, you have to split it by lines and then shape, render and
    align each individually, and adjust @p cursor for each new line as
    appropriate.

Once the whole line is rendered, @ref Math::join() the rectangles returned from
all calls to this function and pass them together with positions for the whole
line to @ref alignRenderedLine(). Finally, to align a multi-line block, join
rectangles returned from all @ref alignRenderedLine() calls and pass them
together with positions for the whole text to @ref alignRenderedBlock().
*/
MAGNUM_TEXT_EXPORT Range2D renderLineGlyphPositionsInto(const AbstractFont& font, Float size, LayoutDirection direction, const Containers::StridedArrayView1D<const Vector2>& glyphOffsets, const Containers::StridedArrayView1D<const Vector2>& glyphAdvances, Vector2& cursor, const Containers::StridedArrayView1D<Vector2>& glyphPositions);

/**
@brief Render glyph quads for a (part of a) single line from font-specific glyph IDs
@param[in] font                 Font the glyphs are coming from
@param[in] size                 Size to render the glyphs at
@param[in] cache                Glyph cache to query for glyph rectangles
@param[in] glyphPositions       Glyph positions coming from an earlier call to
    @ref renderLineGlyphPositionsInto()
@param[in] fontGlyphIds         Glyph IDs coming from @ref AbstractShaper
    instance(s) associated with @p font
@param[out] vertexPositions     Where to put output vertex positions
@param[out] vertexTextureCoordinates  Where to put output texture coordinates
@return Rectangle spanning the rendered glyph quads
@m_since_latest

Produces a sequence of quad corner positions and texture coordinates in order
as shown below. The @p glyphPositions and @p glyphIds views are expected to
have the same size, the @p vertexPositions and @p vertexTextureCoordinates
views are then expected to be four times larger than @p glyphPositions and
@p glyphIds, in order to ultimately contain four corner vertices for each
glyph. To optimize memory use, it's possible to alias @p glyphPositions with
@cpp vertexPositions.every(4) @ce and @p glyphIds with
@cpp vertexTextureCoordinates.every(4) @ce. The @p vertexTextureCoordinates are
temporarily used to store resolved cache-global glyph IDs, the rendering is
then performed in a way that first reads the position and ID for each glyph and
only then fills in the vertex data.

@verbatim
2---3
|   |
|   |
|   |
0---1
@endverbatim

If the text doesn't need to be aligned based on the actual glyph bounds (i.e.,
the desired @ref Alignment isn't `*GlyphBounds`), it's  possible to call this
function even on a multi-line text run provided that @ref alignRenderedLine()
was called on the @p glyphPositions before to align lines relatively to each
other. Otherwise this function should be called on each line individually and
then the @p vertexPositions passed further to @ref alignRenderedLine().

Expects that @p font is contained in @p cache. Glyph IDs not found in the cache
are replaced with the cache-global invalid glyph. If the @p cache is only 2D,
you can use the @ref renderGlyphQuadsInto(const AbstractFont&, Float, const AbstractGlyphCache&, const Containers::StridedArrayView1D<const Vector2>&, const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView1D<Vector2>&, const Containers::StridedArrayView1D<Vector2>&)
overload to get just 2D texture coordinates out. Use the
@ref renderGlyphQuadsInto(const AbstractGlyphCache&, Float, const Containers::StridedArrayView1D<const Vector2>&, const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView1D<Vector2>&, const Containers::StridedArrayView1D<Vector2>&)
overload if you already have cache-global glyph IDs. Use
@ref renderGlyphQuadIndicesInto() to populate the corresponding index array.
*/
MAGNUM_TEXT_EXPORT Range2D renderGlyphQuadsInto(const AbstractFont& font, Float size, const AbstractGlyphCache& cache, const Containers::StridedArrayView1D<const Vector2>& glyphPositions, const Containers::StridedArrayView1D<const UnsignedInt>& fontGlyphIds, const Containers::StridedArrayView1D<Vector2>& vertexPositions, const Containers::StridedArrayView1D<Vector3>& vertexTextureCoordinates);

/**
@brief Render glyph quads for a (part of a) single line from font-specific glyph IDs and a 2D glyph cache
@m_since_latest

Compared to @ref renderGlyphQuadsInto(const AbstractFont&, Float, const AbstractGlyphCache&, const Containers::StridedArrayView1D<const Vector2>&, const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView1D<Vector2>&, const Containers::StridedArrayView1D<Vector3>&)
outputs just 2D texture coordinates. Expects that @ref AbstractGlyphCache::size()
depth is @cpp 1 @ce.
*/
MAGNUM_TEXT_EXPORT Range2D renderGlyphQuadsInto(const AbstractFont& font, Float size, const AbstractGlyphCache& cache, const Containers::StridedArrayView1D<const Vector2>& glyphPositions, const Containers::StridedArrayView1D<const UnsignedInt>& fontGlyphIds, const Containers::StridedArrayView1D<Vector2>& vertexPositions, const Containers::StridedArrayView1D<Vector2>& vertexTextureCoordinates);

/**
@brief Render glyph quads for a (part of a) single line from cache-global glyph IDs
@param[in] cache                Glyph cache to query for glyph rectangles
@param[in] scale                Size to render the glyphs at divided by size of
    the input font
@param[in] glyphPositions       Glyph positions coming from an earlier call to
    @ref renderLineGlyphPositionsInto()
@param[in] glyphIds             Cache-global glyph IDs
@param[out] vertexPositions     Where to put output vertex positions
@param[out] vertexTextureCoordinates  Where to put output texture coordinates
@return Rectangle spanning the rendered glyph quads
@m_since_latest

Compared to @ref renderGlyphQuadsInto(const AbstractFont&, Float, const AbstractGlyphCache&, const Containers::StridedArrayView1D<const Vector2>&, const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView1D<Vector2>&, const Containers::StridedArrayView1D<Vector3>&)
this function operates takes cache-global glyph IDs as an input, i.e. no
mapping from font-specific glyph IDs to cache-global IDs happens in this case.
As with the above overload, to optimize memory use, it's possible to alias
@p glyphPositions and @p glyphIds with @cpp vertexPositions.every(4) @ce and
@cpp vertexTextureCoordinates.every(4) @ce.
*/
MAGNUM_TEXT_EXPORT Range2D renderGlyphQuadsInto(const AbstractGlyphCache& cache, Float scale, const Containers::StridedArrayView1D<const Vector2>& glyphPositions, const Containers::StridedArrayView1D<const UnsignedInt>& glyphIds, const Containers::StridedArrayView1D<Vector2>& vertexPositions, const Containers::StridedArrayView1D<Vector3>& vertexTextureCoordinates);

/**
@brief Render glyph quads for a (part of a) single line from cache-global glyph IDs and a 2D glyph cache
@m_since_latest

Compared to @ref renderGlyphQuadsInto(const AbstractGlyphCache&, Float, const Containers::StridedArrayView1D<const Vector2>&, const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView1D<Vector2>&, const Containers::StridedArrayView1D<Vector3>&)
outputs just 2D texture coordinates. Expects that @ref AbstractGlyphCache::size()
depth is @cpp 1 @ce.
*/
MAGNUM_TEXT_EXPORT Range2D renderGlyphQuadsInto(const AbstractGlyphCache& cache, Float scale, const Containers::StridedArrayView1D<const Vector2>& glyphPositions, const Containers::StridedArrayView1D<const UnsignedInt>& glyphIds, const Containers::StridedArrayView1D<Vector2>& vertexPositions, const Containers::StridedArrayView1D<Vector2>& vertexTextureCoordinates);

/**
@brief Align a rendered line
@param[in] lineRectangle    Rectangle spanning the whole line
@param[in] direction        Layout direction. Currently expected to always be
    @ref LayoutDirection::HorizontalTopToBottom.
@param[in] alignment        Desired alignment. Only the part in direction of
    the line is used.
@param[in,out] positions    Positions of glyphs or glyph quad vertices on the
    whole line to be aligned
@return The @p lineRectangle, translated in the direction of the line based on
    the alignment.
@m_since_latest

Expects that @p alignment isn't `*Start` or `*End`, those values have to be
resolved to `*Left` or `*Right` based on desired or detected
@ref ShapeDirection using @ref alignmentForDirection() before being passed to
this function.

If @p alignment isn't `*GlyphBounds`, this function should get glyph
@p positions for the whole line coming from @ref renderLineGlyphPositionsInto()
and @p lineRectangle being all rectangles returned by that function combined
together with @ref Math::join().

If @p alignment is `*GlyphBounds`, this function should get vertex @p positions
for a whole line coming from @ref renderGlyphQuadsInto() and @p lineRectangle
being all rectangles returned by that function combined together with
@ref Math::join().

The @p positions are translated in one axis based on the @p inputRectangle and
the part of @p alignment matching line direction in @p direction. Values of the
@p positions themselves aren't considered when calculating the alignment. To
align a multi-line block, join rectangles returned from all calls to this
function and pass them together with positions for the whole block to
@ref alignRenderedBlock().
*/
MAGNUM_TEXT_EXPORT Range2D alignRenderedLine(const Range2D& lineRectangle, LayoutDirection direction, Alignment alignment, const Containers::StridedArrayView1D<Vector2>& positions);

/**
@brief Align a rendered block
@param[in] blockRectangle   Rectangle spanning all lines in the block
@param[in] direction        Layout direction. Currently expected to always be
    @ref LayoutDirection::HorizontalTopToBottom.
@param[in] alignment        Desired alignment. Only the part in direction of
    the line is used.
@param[in,out] positions    Positions of glyphs or glyph quad vertices on the
    whole line to be aligned
@return The @p blockRectangle, translated in the direction of the layout
    advance based on the alignment.
@m_since_latest

Expects that @p alignment isn't `*Start` or `*End`, those values have to be
resolved to `*Left` or `*Right` based on desired or detected
@ref ShapeDirection using @ref alignmentForDirection() before being passed to
this function.

This function should get glyph or vertex @p positions for all lines as aligned
by calls to @ref alignRenderedLine(), and @p blockRectangle being all line
rectangles returned by that function combined together with @ref Math::join().

The @p positions are translated in one axis based on the @p inputRectangle and
the part of @p alignment matching layout advance in @p direction. Values of the
@p positions themselves aren't considered when calculating the translation.
*/
MAGNUM_TEXT_EXPORT Range2D alignRenderedBlock(const Range2D& blockRectangle, LayoutDirection direction, Alignment alignment, const Containers::StridedArrayView1D<Vector2>& positions);

/**
@brief Render 32-bit glyph quad indices
@param[in]  glyphOffset     Offset of the first glyph to generate indices for
@param[out] indices         Where to put the generated indices
@m_since_latest

Produces a sequence of quad indices in order as shown below, with the index
values being shifted by @cpp glyphOffset*4 @ce. Expects that the @p indices
view size is divisible by @cpp 6 @ce and the value range fits into the output
type.

@verbatim
2---3 2 3---5
|   | |\ \  |
|   | | \ \ |
|   | |  \ \|
0---1 0---1 4
@endverbatim
*/
MAGNUM_TEXT_EXPORT void renderGlyphQuadIndicesInto(UnsignedInt glyphOffset, const Containers::StridedArrayView1D<UnsignedInt>& indices);

/**
@brief Render 16-bit glyph quad indices
@m_since_latest

See @ref renderGlyphQuadIndicesInto(UnsignedInt, const Containers::StridedArrayView1D<UnsignedInt>&)
for more information.
*/
MAGNUM_TEXT_EXPORT void renderGlyphQuadIndicesInto(UnsignedInt glyphOffset, const Containers::StridedArrayView1D<UnsignedShort>& indices);

/**
@brief Render 8-bit glyph quad indices
@m_since_latest

See @ref renderGlyphQuadIndicesInto(UnsignedInt, const Containers::StridedArrayView1D<UnsignedInt>&)
for more information.
*/
MAGNUM_TEXT_EXPORT void renderGlyphQuadIndicesInto(UnsignedInt glyphOffset, const Containers::StridedArrayView1D<UnsignedByte>& indices);

/**
@brief Find a glyph range corresponding to given byte range in the input text
@m_since_latest

Assuming @p clusters is a view containing cluster IDs returned from
@ref AbstractShaper::glyphClustersInto() and @p begin and @p end are byte
positions in the text passed to @ref AbstractShaper::shape() for which the
cluster IDs were retrieved, returns a range in the glyph array that contains
given range. Assumes that @p clusters are either monotonically non-dereasing or
non-increasing.

If @p clusters are empty or @p end is less or equal to all @p clusters, returns
@cpp {0, 0} @ce. If @p begin is greater than all @p clusters are, both return
values are set to @p clusters size. In both cases the empty returned range
means there are no glyphs corresponding to given byte. Otherwise, if the input
range is non-empty, the returned range is always at least one glyph. The
returned range always points to cluster boundaries, even if the input is inside
a multi-byte character or ligature or inside a multi-glyph cluster.

If @p begin is greater than @p end, the first value of the output is also
greater than the second. Otherwise, the first value of the output is always
less than or equal to the second.

At the moment, the lookup is done with an @f$ \mathcal{O}(n) @f$ complexity,
with @f$ n @f$ being size of the @p clusters view.

Mapping in the other direction, from glyphs to input bytes, is simply
@cpp clusters[i] @ce. See @ref AbstractShaper::glyphClustersInto() for more
information about how the cluster IDs may look like depending on the input and
shaper features used.
*/
MAGNUM_TEXT_EXPORT Containers::Pair<UnsignedInt, UnsignedInt> glyphRangeForBytes(const Containers::StridedArrayView1D<const UnsignedInt>& clusters, UnsignedInt begin, UnsignedInt end);

#ifdef MAGNUM_TARGET_GL
/**
@brief Base for text renderers

Not meant to be used directly, see the @ref Renderer class for more
information.

@note This class is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.

@see @ref Renderer2D, @ref Renderer3D
*/
class MAGNUM_TEXT_EXPORT AbstractRenderer {
    public:
        /**
         * @brief Render text
         * @param font          Font
         * @param cache         Glyph cache
         * @param size          Font size in points
         * @param text          Text to render
         * @param alignment     Text alignment
         *
         * Returns a tuple with vertex positions, texture coordinates, indices
         * and rectangle spanning the rendered text. Expects that @p font is
         * present in @p cache and that @p cache isn't an array.
         */
        static std::tuple<std::vector<Vector2>, std::vector<Vector2>, std::vector<UnsignedInt>, Range2D> render(AbstractFont& font, const AbstractGlyphCache& cache, Float size, const std::string& text, Alignment alignment = Alignment::LineLeft);

        /**
         * @brief Capacity for rendered glyphs
         *
         * @see @ref reserve()
         */
        UnsignedInt capacity() const { return _capacity; }

        /**
         * @brief Font size in points
         * @m_since_latest
         */
        Float fontSize() const { return _fontSize; }

        /** @brief Rectangle spanning the rendered text */
        Range2D rectangle() const { return _rectangle; }

        /** @brief Vertex buffer */
        GL::Buffer& vertexBuffer() { return _vertexBuffer; }

        /** @brief Index buffer */
        GL::Buffer& indexBuffer() { return _indexBuffer; }

        /** @brief Mesh */
        GL::Mesh& mesh() { return _mesh; }

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
        void reserve(UnsignedInt glyphCount, GL::BufferUsage vertexBufferUsage, GL::BufferUsage indexBufferUsage);

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
        explicit MAGNUM_TEXT_LOCAL AbstractRenderer(AbstractFont& font, const AbstractGlyphCache& cache, Float size, Alignment alignment);

        ~AbstractRenderer();

        GL::Mesh _mesh;
        GL::Buffer _vertexBuffer, _indexBuffer;
        #ifdef CORRADE_TARGET_EMSCRIPTEN
        Containers::Array<UnsignedByte> _vertexBufferData, _indexBufferData;
        #endif

    private:
        AbstractFont& font;
        const AbstractGlyphCache& cache;
        Float _fontSize;
        Alignment _alignment;
        UnsignedInt _capacity;
        Range2D _rectangle;

        #if defined(MAGNUM_TARGET_GLES2) && !defined(CORRADE_TARGET_EMSCRIPTEN)
        typedef void*(*BufferMapImplementation)(GL::Buffer&, GLsizeiptr);
        static MAGNUM_TEXT_LOCAL void* bufferMapImplementationFull(GL::Buffer& buffer, GLsizeiptr length);
        static MAGNUM_TEXT_LOCAL void* bufferMapImplementationRange(GL::Buffer& buffer, GLsizeiptr length);
        static BufferMapImplementation bufferMapImplementation;
        #else
        #ifndef CORRADE_TARGET_EMSCRIPTEN
        static
        #else
        MAGNUM_TEXT_LOCAL
        #endif
        void* bufferMapImplementation(GL::Buffer& buffer, GLsizeiptr length);
        #endif

        #if defined(MAGNUM_TARGET_GLES2) && !defined(CORRADE_TARGET_EMSCRIPTEN)
        typedef void(*BufferUnmapImplementation)(GL::Buffer&);
        static MAGNUM_TEXT_LOCAL void bufferUnmapImplementationDefault(GL::Buffer& buffer);
        static MAGNUM_TEXT_LOCAL BufferUnmapImplementation bufferUnmapImplementation;
        #else
        #ifndef CORRADE_TARGET_EMSCRIPTEN
        static
        #else
        MAGNUM_TEXT_LOCAL
        #endif
        void bufferUnmapImplementation(GL::Buffer& buffer);
        #endif
};

/**
@brief Text renderer

Lays out the text into mesh using given font. Use of ligatures, kerning etc.
depends on features supported by particular font and its layouter.

@section Text-Renderer-usage Usage

Immutable text (e.g. menu items, credits) can be simply rendered using static
methods, returning result either as data arrays or as fully configured mesh.
The text can be then drawn as usual by configuring the shader and drawing the
mesh:

@snippet Text-gl.cpp Renderer-usage1

See @ref render(AbstractFont&, const AbstractGlyphCache&, Float, const std::string&, Alignment)
and @ref render(AbstractFont&, const AbstractGlyphCache&, Float, const std::string&, GL::Buffer&, GL::Buffer&, GL::BufferUsage, Alignment)
for more information.

While this method is sufficient for one-shot rendering of static texts, for
mutable texts (e.g. FPS counters, chat messages) there is another approach
that doesn't recreate everything on each text change:

@snippet Text-gl.cpp Renderer-usage2

@subsection Text-Renderer-usage-font-size Font size

As mentioned in @ref Text-AbstractFont-usage-font-size "AbstractFont class documentation",
the size at which the font is loaded is decoupled from the size at which a
concrete text is rendered. In particular, with a concrete projection matrix,
the size you pass to either @ref render() or to the @ref Renderer() constructor
will always result in the same size of the rendered text, independently of the
size the font was loaded in. Size of the loaded font is the size at which the
glyphs get prerendered into the glyph cache, affecting visual quality.

When rendering the text, there are two common approaches --- either setting up
the size to match a global user interface scale, or having the text size
proportional to the window size. The first approach results in e.g. a 12 pt
font matching a 12 pt font in other applications, and is what's shown in the
above snippets. The most straightforward way to achieve that is to set up the
projection matrix size to match actual window pixels, such as @ref Platform::Sdl2Application::windowSize() "Platform::*Application::windowSize()".
If using the regular @ref GlyphCache, for best visual quality it should be
created with the @ref AbstractFont loaded at the same size as the text to be
rendered, although often a double supersampling achieves a crisper result.
I.e., loading the font with 24 pt, but rendering with 12 pt. See below for
@ref Text-Renderer-usage-font-size-dpi "additional considerations for proper DPI awareness".

The second approach, with text size being relative to the window size, is for
cases where the text is meant to match surrounding art, such as in a game menu.
In this case the projection size is usually something arbitrary that doesn't
match window pixels, and the text point size then has to be relative to that.
For this use case a @ref DistanceFieldGlyphCache is the better match, as it can
provide text at different sizes with minimal quality loss. See its
documentation for details about picking the right font size and other
parameters for best results.

@subsection Text-Renderer-usage-font-size-dpi DPI awareness

To achieve crisp rendering and/or text size matching other applications on
HiDPI displays, additional steps need to be taken. There are two separate
concepts for DPI-aware rendering:

-   Interface size --- size at which the interface elements are positioned on
    the screen. Often, for simplicity, the interface is using some "virtual
    units", so a 12 pt font is still a 12 pt font independently of how the
    interface is scaled compared to actual display properties (for example by
    setting a global 150% scale in the desktop environment, or by zooming a
    browser window). The size used by the @ref Renderer should match these
    virtual units.
-   Framebuffer size --- how many pixels is actually there. If a 192 DPI
    display has a 200% interface scale, a 12 pt font would be 32 pixels. But if
    it only has a 150% scale, all interface elements will be smaller, and a 12
    pt font would be only 24 pixels. The size used by the @ref AbstractFont and
    @ref GlyphCache should be chosen with respect to the actual physical
    pixels.

When using for example @ref Platform::Sdl2Application or other `*Application`
implementations, you usually have three values at your disposal ---
@ref Platform::Sdl2Application::windowSize() "windowSize()",
@ref Platform::Sdl2Application::framebufferSize() "framebufferSize()" and
@ref Platform::Sdl2Application::dpiScaling() "dpiScaling()". Their relation is
documented thoroughly in @ref Platform-Sdl2Application-dpi, for this particular
case a scaled interface size, used instead of window size for the projection,
would be calculated like this:

@snippet Text-gl.cpp Renderer-dpi-interface-size

And a multiplier for the @ref AbstractFont and @ref GlyphCache font size like
this. The @ref Renderer keeps using the size without this multiplier.

@snippet Text-gl.cpp Renderer-dpi-size-multiplier

@section Text-Renderer-required-opengl-functionality Required OpenGL functionality

Mutable text rendering requires @gl_extension{ARB,map_buffer_range} on desktop
OpenGL (also part of OpenGL ES 3.0). If @gl_extension{EXT,map_buffer_range} is not
available in ES 2.0, at least @gl_extension{OES,mapbuffer} must be supported for
asynchronous buffer updates. There is no similar extension in WebGL, thus plain
(and slow) buffer updates are used there.

@note This class is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.

@see @ref Renderer2D, @ref Renderer3D, @ref AbstractFont,
    @ref Shaders::VectorGL, @ref Shaders::DistanceFieldVectorGL
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
         * Returns a mesh prepared for use with @ref Shaders::VectorGL or
         * @ref Shaders::DistanceFieldVectorGL and a rectangle spanning the
         * rendered text. Expects that @p font is present in @p cache and that
         * @p cache isn't an array.
         */
        static std::tuple<GL::Mesh, Range2D> render(AbstractFont& font, const AbstractGlyphCache& cache, Float size, const std::string& text, GL::Buffer& vertexBuffer, GL::Buffer& indexBuffer, GL::BufferUsage usage, Alignment alignment = Alignment::LineLeft);

        /**
         * @brief Constructor
         * @param font          Font
         * @param cache         Glyph cache
         * @param size          Font size
         * @param alignment     Text alignment
         */
        explicit Renderer(AbstractFont& font, const AbstractGlyphCache& cache, Float size, Alignment alignment = Alignment::LineLeft);
        Renderer(AbstractFont&, AbstractGlyphCache&&, Float, Alignment alignment = Alignment::LineLeft) = delete; /**< @overload */

        #ifndef DOXYGEN_GENERATING_OUTPUT
        using AbstractRenderer::render;
        #endif
};

/**
@brief Two-dimensional text renderer

@note This class is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.
*/
typedef Renderer<2> Renderer2D;

/**
@brief Three-dimensional text renderer

@note This class is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.
*/
typedef Renderer<3> Renderer3D;
#endif

}}

#endif
