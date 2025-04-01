#ifndef Magnum_Text_Renderer_h
#define Magnum_Text_Renderer_h
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
 * @brief Class @ref Magnum::Text::RendererCore, @ref Magnum::Text::Renderer, @ref Magnum::Text::AbstractRenderer, typedef @ref Magnum::Text::Renderer2D, @ref Magnum::Text::Renderer3D, function @ref Magnum::Text::renderLineGlyphPositionsInto(), @ref Magnum::Text::renderGlyphQuadsInto(), @ref Magnum::Text::glyphQuadBounds(), @ref Magnum::Text::alignRenderedLine(), @ref Magnum::Text::alignRenderedBlock(), @ref Magnum::Text::renderGlyphQuadIndicesInto(), @ref Magnum::Text::glyphRangeForBytes()
 */

#include <initializer_list>
#include <Corrade/Containers/Pointer.h>

#include "Magnum/Magnum.h"
#include "Magnum/Text/Text.h"
#include "Magnum/Text/visibility.h"

#if defined(MAGNUM_TARGET_GL) && defined(MAGNUM_BUILD_DEPRECATED)
#include <Corrade/Utility/StlForwardTuple.h>
#include <Corrade/Utility/StlForwardVector.h>
#include <Corrade/Utility/StlForwardString.h>

#include "Magnum/Math/Range.h"
#include "Magnum/GL/GL.h"
#include "Magnum/Text/Alignment.h"
#endif

namespace Magnum { namespace Text {

/**
@brief Text renderer core flag
@m_since_latest

@see @ref RendererFlags, @ref Renderer
*/
enum class RendererCoreFlag: UnsignedByte {
    /**
     * Populate glyph cluster info in @ref RendererCore::glyphClusters() for
     * text selection and editing purposes.
     */
    GlyphClusters = 1 << 0,

    /* Additions to this enum have to be propagated to RendererFlag,
       RendererGLFlag and the masks in RendererCore::flags() and
       Renderer::flags() */
};

/**
 * @debugoperatorenum{RendererCoreFlag}
 * @m_since_latest
 */
MAGNUM_TEXT_EXPORT Debug& operator<<(Debug& output, RendererCoreFlag value);

/**
@brief Text renderer core flags
@m_since_latest

@see @ref Renderer
*/
typedef Containers::EnumSet<RendererCoreFlag> RendererCoreFlags;

CORRADE_ENUMSET_OPERATORS(RendererCoreFlags)

/**
 * @debugoperatorenum{RendererCoreFlags}
 * @m_since_latest
 */
MAGNUM_TEXT_EXPORT Debug& operator<<(Debug& output, RendererCoreFlags value);

/**
@brief Text renderer core
@m_since_latest

Implements essential logic for rendering text formed from multiple runs, lines
and fonts, providing access to glyph positions, glyph IDs and glyph cluster
information that can be subsequently used to show the text on the screen and
perform cursor or selection placement.

See the higher-level @ref Renderer subclass for full usage documentation ---
the input interface is mostly the same between the two, with just the output
being something else. The rest of this documentation thus only highlights the
differences between the two. For lowest-level functionality see the
@ref AbstractShaper class, which exposes text shaping capabilities implemented
directly by particular font plugins.

@section Text-RendererCore-usage Usage

A @ref RendererCore instance is created and populated the same way as a
@ref Renderer or @ref RendererGL, with @ref add() and @ref render() behaving
exactly the same:

@snippet Text.cpp RendererCore-usage

Once rendered, the @ref glyphPositions() and @ref glyphIds() views provide
access to rendered glyph data, and @ref runScales() with @ref runEnds()
describe which scale is applied to particular glyph ranges in order to place
the glyphs at given positions apprpriately scaled. You can then use the
low-level @ref renderGlyphQuadsInto() utility to create textured quads, or feed
the data to some entirely different system that renders the text without
needing textured quads at all.

@snippet Text.cpp RendererCore-usage-quads

@section Text-RendererCore-clusters Mapping between input text and shaped glyphs

For implementing text selection or editing, if
@ref RendererCoreFlag::GlyphClusters is enabled on @ref RendererCore
construction, the renderer exposes also the glyph cluster information for each
run via @ref glyphClusters(). See the @ref Text-Renderer-clusters "relevant Renderer documentation"
for a detailed explanation of how the data get used.

@section Text-RendererCore-allocators Providing custom glyph and run data allocators

For more control over memory allocations or for very customized use, it's
possible to hook up custom allocators for glyph and run data. For example, if
you always use the @ref RendererCore to render only up to a fixed amount of
glyphs, you can direct it to statically sized arrays:

@snippet Text.cpp RendererCore-allocators-static

A behavior worth mentioning is that on @ref clear() or @ref reset() the
allocators get called with the count being @cpp 0 @ce, which can be used to
"redirect" the allocation to some entirely different memory. That allows you to
use a single renderer instance to independently render and replace several
different text blocks, instead of having a dedicated renderer instance for each
or having to copy the rendered data out every time:

@snippet Text.cpp RendererCore-allocators-redirect

Expected allocator behavior is fully documented in the @ref RendererCore()
constructor, note especially the special casing for glyph advances. The
@ref Text-Renderer-allocators "Renderer subclass then provides two additional allocator hooks for index and vertex data."
*/
class MAGNUM_TEXT_EXPORT RendererCore {
    public:
        /**
         * @brief Construct
         * @param glyphCache    Glyph cache to use for glyph ID mapping
         * @param flags         Opt-in feature flags
         *
         * By default, the renderer allocates the memory for glyph and run data
         * internally. Use the overload below to supply external allocators.
         * @todoc the damn thing can't link to functions taking functions
         */
        explicit RendererCore(const AbstractGlyphCache& glyphCache, RendererCoreFlags flags = {}): RendererCore{glyphCache, nullptr, nullptr, nullptr, nullptr, flags} {}

        /**
         * @brief Construct with external allocators
         * @param glyphCache        Glyph cache to use for glyph ID mapping
         * @param glyphAllocator    Glyph allocator function or
         *      @cpp nullptr @ce
         * @param glyphAllocatorState  State pointer to pass to
         *      @p glyphAllocator
         * @param runAllocator      Run allocator function or @cpp nullptr @ce
         * @param runAllocatorState  State pointer to pass to @p runAllocator
         * @param flags             Opt-in feature flags
         *
         * The @p glyphAllocator gets called with desired @p glyphCount every
         * time @ref glyphCount() reaches @ref glyphCapacity(). Size of
         * passed-in @p glyphPositions, @p glyphIds and @p glyphClusters views
         * matches @ref glyphCount(). The @p glyphAdvances view is a temporary
         * storage with contents that don't need to be preserved on
         * reallocation and is thus passed in empty. If the renderer wasn't
         * constructed with @ref RendererCoreFlag::GlyphClusters, the
         * @p glyphClusters is @cpp nullptr @ce to indicate it's not meant to
         * be allocated. The allocator is expected to replace all passed views
         * with new views that are larger by *at least* @p glyphCount, pointing
         * to a reallocated memory with contents from the original view
         * preserved. Initially @ref glyphCount() is @cpp 0 @ce and the views
         * are all passed in empty, every subsequent time the views match a
         * prefix of views previously returned by the allocator. To save
         * memory, the renderer guarantees that @p glyphIds and
         * @p glyphClusters are only filled once @p glyphAdvances were merged
         * into @p glyphPositions. In other words, the @p glyphAdvances can
         * alias a suffix of @p glyphIds and @p glyphClusters.
         *
         * The @p runAllocator gets called with desired @p runCount every time
         * @ref runCount() reaches @ref runCapacity(). Size of passed-in
         * @p runScales and @p runEnds views matches @ref runCount(). The
         * allocator is expected to replace the views with new views that are
         * larger by *at least* @p runCount, pointing to a reallocated memory
         * with contents from the original views preserved. Initially
         * @ref runCount() is @cpp 0 @ce and the views are passed in empty,
         * every subsequent time the views match a prefix of views previously
         * returned by the allocator.
         *
         * The renderer always requests only exactly the desired size and the
         * growth strategy is up to the allocators themselves --- the returned
         * views can be larger than requested and aren't all required to all
         * have the same size. The minimum of size increases across all views
         * is then treated as the new @ref glyphCapacity() / @ref runCapacity().
         *
         * As a special case, when @ref clear() or @ref reset() is called, the
         * allocators are called with empty views and @p glyphCount /
         * @p runCount being @cpp 0 @ce. This is to allow the allocators to
         * perform any needed reset as well.
         *
         * If @p glyphAllocator or @p runAllocator is @cpp nullptr @ce,
         * @p glyphAllocatorState or @p runAllocatorState is ignored and
         * default builtin allocator get used for either. Passing
         * @cpp nullptr @ce for both is equivalent to calling the
         * @ref RendererCore(const AbstractGlyphCache&, RendererCoreFlags)
         * constructor.
         */
        explicit RendererCore(const AbstractGlyphCache& glyphCache, void(*glyphAllocator)(void* state, UnsignedInt glyphCount, Containers::StridedArrayView1D<Vector2>& glyphPositions, Containers::StridedArrayView1D<UnsignedInt>& glyphIds, Containers::StridedArrayView1D<UnsignedInt>* glyphClusters, Containers::StridedArrayView1D<Vector2>& glyphAdvances), void* glyphAllocatorState, void(*runAllocator)(void* state, UnsignedInt runCount, Containers::StridedArrayView1D<Float>& runScales, Containers::StridedArrayView1D<UnsignedInt>& runEnds), void* runAllocatorState, RendererCoreFlags flags = {});

        /**
         * @brief Construct without creating the internal state
         * @m_since_latest
         *
         * The constructed instance is equivalent to moved-from state, i.e. no
         * APIs can be safely called on the object. Useful in cases where you
         * will overwrite the instance later anyway. Move another object over
         * it to make it useful.
         *
         * Note that this is a low-level and a potentially dangerous API, see
         * the documentation of @ref NoCreate for alternatives.
         */
        explicit RendererCore(NoCreateT) noexcept;

        /** @brief Copying is not allowed */
        RendererCore(RendererCore&) = delete;

        /**
         * @brief Move constructor
         *
         * Performs a destructive move, i.e. the original object isn't usable
         * afterwards anymore.
         */
        RendererCore(RendererCore&&) noexcept;

        ~RendererCore();

        /** @brief Copying is not allowed */
        RendererCore& operator=(RendererCore&) = delete;

        /** @brief Move assignment */
        RendererCore& operator=(RendererCore&&) noexcept;

        /** @brief Glyph cache associated with the renderer */
        const AbstractGlyphCache& glyphCache() const;

        /** @brief Flags */
        RendererCoreFlags flags() const;

        /**
         * @brief Total count of rendered glyphs
         *
         * Does *not* include glyphs from the current in-progress rendering, if
         * any, as their contents are not finalized yet. Use
         * @ref renderingGlyphCount() to query the count including the
         * in-progress glyphs.
         * @see @ref isRendering(), @ref runCount()
         */
        UnsignedInt glyphCount() const;

        /**
         * @brief Glyph capacity
         *
         * @see @ref glyphCount(), @ref runCapacity(), @ref reserve()
         */
        UnsignedInt glyphCapacity() const;

        /**
         * @brief Total count of rendered runs
         *
         * Does *not* include runs from the current in-progress rendering, if
         * any, as their contents are not finalized yet. Use
         * @ref renderingRunCount() to query the count including the
         * in-progress runs.
         * @see @ref isRendering(), @ref glyphCount()
         */
        UnsignedInt runCount() const;

        /**
         * @brief Run capacity
         *
         * @see @ref runCount(), @ref glyphCapacity() @ref reserve()
         */
        UnsignedInt runCapacity() const;

        /**
         * @brief Whether text rendering is currently in progress
         *
         * Returns @cpp true @ce if there are any @ref add() calls not yet
         * followed by a @ref render(), @cpp false @ce otherwise. If rendering
         * is in progress, @ref setCursor(), @ref setAlignment() and
         * @ref setLayoutDirection() cannot be called. The @ref glyphCount(),
         * @ref runCount() and all data accessors don't include the
         * yet-to-be-finalized contents.
         */
        bool isRendering() const;

        /**
         * @brief Total count of glyphs including current in-progress rendering
         *
         * Can be used for example to query which glyphs correspond to the last
         * @ref add() call. If @ref isRendering() is @cpp false @ce, the
         * returned value is the same as @ref glyphCount().
         */
        UnsignedInt renderingGlyphCount() const;

        /**
         * @brief Total count of runs including current in-progress rendering
         *
         * Can be used for example to query which runs correspond to the last
         * @ref add() call. If @ref isRendering() is @cpp false @ce, the
         * returned value is the same as @ref runCount().
         */
        UnsignedInt renderingRunCount() const;

        /**
         * @brief Cursor position
         *
         * Note that this does *not* return the current position at which an
         * in-progress rendering is happening --- the way the glyphs get placed
         * before they're aligned to their final position is internal to the
         * implementation and querying such in-progress state would be of
         * little use.
         */
        Vector2 cursor() const;

        /**
         * @brief Set cursor position for the next rendered text
         * @return Reference to self (for method chaining)
         *
         * The next rendered text is placed according to specified @p cursor,
         * @ref alignment() and @ref lineAdvance(). Expects that rendering is
         * currently not in progress, meaning that the cursor can be only
         * specified before rendering a particular piece of text. Initial value
         * is @cpp {0.0f, 0.0f} @ce.
         * @see @ref isRendering()
         */
        RendererCore& setCursor(const Vector2& cursor);

        /** @brief Text alignment */
        Alignment alignment() const;

        /**
         * @brief Set alignment for the next rendered text
         * @return Reference to self (for method chaining)
         *
         * The next rendered text is placed according to specified
         * @ref cursor(), @p alignment and @ref lineAdvance(). Expects that
         * rendering is currently not in progress, meaning that the alignment
         * can be only specified before rendering a particular piece of text.
         * Initial value is @ref Alignment::MiddleCenter.
         * @see @ref isRendering()
         */
        RendererCore& setAlignment(Alignment alignment);

        /** @brief Line advance */
        Float lineAdvance() const;

        /**
         * @brief Set line advance for the next rendered text
         * @return Reference to self (for method chaining)
         *
         * The next rendered text is placed according to specified
         * @ref cursor(), @ref alignment and @p advance. The advance value is
         * used according to @ref layoutDirection() and in a coordinate system
         * matching @ref AbstractFont::ascent() and
         * @relativeref{AbstractFont,descent()}, so for example causes the next
         * line to be shifted in a negative Y direction for
         * @ref LayoutDirection::HorizontalTopToBottom. Expects that rendering
         * is currently not in progress, meaning that the line advance can be
         * only specified before rendering a particular piece of text. If set
         * to @cpp 0.0f @ce, the line advance is picked metrics of the first
         * font a corresponding size passed to @ref add().
         * @see @ref isRendering(), @ref AbstractFont::lineHeight()
         */
        RendererCore& setLineAdvance(Float advance);

        /** @brief Layout direction */
        LayoutDirection layoutDirection() const;

        /**
         * @brief Set layout direction
         * @return Reference to self (for method chaining)
         *
         * Expects that rendering is currently not in progress. Currently
         * expected to always be @ref LayoutDirection::HorizontalTopToBottom.
         * Initial value is @ref LayoutDirection::HorizontalTopToBottom.
         * @see @ref isRendering()
         */
        RendererCore& setLayoutDirection(LayoutDirection direction);

        /**
         * @brief Glyph positions
         *
         * The returned view has a size of @ref glyphCount(). Note that the
         * contents are not guaranteed to be meaningful if custom glyph
         * allocator is used, as the user code is free to perform subsequent
         * operations on those.
         */
        Containers::StridedArrayView1D<const Vector2> glyphPositions() const;

        /**
         * @brief Glyph IDs
         *
         * The returned view has a size of @ref glyphCount(). Note that the
         * contents are not guaranteed to be meaningful if custom glyph
         * allocator is used, as the user code is free to perform subsequent
         * operations on those.
         */
        Containers::StridedArrayView1D<const UnsignedInt> glyphIds() const;

        /**
         * @brief Glyph cluster IDs
         *
         * Expects that the renderer was constructed with
         * @ref RendererCoreFlag::GlyphClusters. The returned view has a size
         * of @ref glyphCount(). Note that the contents are not guaranteed to
         * be meaningful if custom glyph allocator is used, as the user code is
         * free to perform subsequent operations on those.
         */
        Containers::StridedArrayView1D<const UnsignedInt> glyphClusters() const;

        /**
         * @brief Text run scales
         *
         * The returned view has a size of @ref runCount(). Note that the
         * contents are not guaranteed to be meaningful if custom run allocator
         * is used, as the user code is free to perform subsequent operations
         * on those.
         */
        Containers::StridedArrayView1D<const Float> runScales() const;

        /**
         * @brief Text run end glyph offsets
         *
         * The returned view has a size of @ref runCount(), the last value is
         * equal to @ref glyphCount(), and the values index the
         * @ref glyphPositions(), @ref glyphIds() and @ref glyphClusters()
         * views. The first text run glyphs start at offset @cpp 0 @ce and end
         * at @cpp runEnds()[0] @ce, the second text run glyphs start at offset
         * @cpp runEnds()[0] @ce and end at @cpp runEnds()[1] @ce, etc. See
         * also the @ref glyphsForRuns() function which provides a convenient
         * way to get a range of glyphs corresponding to a range of runs
         * without having to deal with edge cases.
         *
         * Note that the contents of the returned view are not guaranteed to be
         * meaningful if custom run allocator is used, as the user code is free
         * to perform subsequent operations on those.
         */
        Containers::StridedArrayView1D<const UnsignedInt> runEnds() const;

        /**
         * @brief Range of glyphs corresponding to a range of runs
         *
         * With @p runRange being for example the second value returned by
         * @ref render(), returns a begin and end glyph offset for given run
         * range, which can then be used to index the @ref glyphPositions(),
         * @ref glyphIds() and @ref glyphClusters() views; when multipled by
         * @cpp 6 @ce to index the @ref Renderer::indices() view and when
         * multiplied by @cpp 4 @ce to index the @ref Renderer::vertexPositions()
         * and @relativeref{Renderer,vertexTextureCoordinates()} /
         * @relativeref{Renderer,vertexTextureArrayCoordinates()} views.
         * Expects that both the min and max @p runRange value are less than or
         * equal to @ref renderingRunCount().
         *
         * Note that the returned value is not guaranteed to be meaningful if
         * custom run allocator is used, as the user code is free to perform
         * subsequent operations on the run data.
         * @see @ref runEnds()
         */
        Range1Dui glyphsForRuns(const Range1Dui& runRange) const;

        /**
         * @brief Reserve capacity for given glyph and run count
         * @return Reference to self (for method chaining)
         *
         * @see @ref glyphCapacity(), @ref glyphCount(),
         *      @ref runCapacity(), @ref runCount()
         */
        RendererCore& reserve(UnsignedInt glyphCapacity, UnsignedInt runCapacity);

        /**
         * @brief Clear rendered glyphs and runs
         * @return Reference to self (for method chaining)
         *
         * The @ref glyphCount() and @ref runCount() becomes @cpp 0 @ce after
         * this call and any in-progress rendering is discarded, making
         * @ref isRendering() return @cpp false @ce. If custom glyph or run
         * allocators are used, they get called with empty views and zero
         * sizes.
         *
         * Depending on allocator used, @ref glyphCapacity() and
         * @ref runCapacity() may stay non-zero. The @ref cursor(),
         * @ref alignment(), @ref lineAdvance() and @ref layoutDirection() are
         * left untouched, use @ref reset() to reset those to their default
         * values as well.
         */
        RendererCore& clear();

        /**
         * @brief Reset internal renderer state
         * @return Reference to self (for method chaining)
         *
         * Calls @ref clear(), and additionally @ref cursor(),
         * @ref alignment(), @ref lineAdvance() and @ref layoutDirection() are
         * reset to their default values. Apart from @ref glyphCapacity() and
         * @ref runCapacity(), which may stay non-zero depending on allocator
         * used, the instance is equivalent to a default-constructed state.
         */
        RendererCore& reset();

        /**
         * @brief Add to the currently rendered text
         * @param shaper    Shaper instance to render with
         * @param size      Font size
         * @param text      Text in UTF-8
         * @param begin     Beginning byte in the input text
         * @param end       (One byte after) the end byte in the input text
         * @param features  Typographic features to apply for the whole text or
         *      its subranges
         * @return Reference to self (for method chaining)
         *
         * Splits @p text into individual lines and shapes each with given
         * @p shaper. Places and aligns the text according to @ref cursor(),
         * @ref alignment() and @ref layoutDirection(), continuing from the
         * state left after the previous @ref add(), if any.
         *
         * After calling this function, @ref isRendering() returns
         * @cpp true @ce, @ref renderingGlyphCount() and @ref renderingRunCount()
         * are updated with the count of newly added glyphs and runs, and
         * @ref setCursor(), @ref setAlignment() or @ref setLayoutDirection()
         * cannot be called anymore. Call @ref add() more times and wrap up
         * with @ref render() to perform the final alignment and other steps
         * necessary to finalize the rendering. If you only need to render the
         * whole text at once, you can use
         * @ref render(AbstractShaper&, Float, Containers::StringView, Containers::ArrayView<const FeatureRange>)
         * instead.
         *
         * The function assumes that the @p shaper has either appropriate
         * script, language and shape direction set, or has them left at
         * defaults in order let them be autodetected. In order to allow the
         * implementation to perform shaping aware of surrounding context, such
         * as picking correct glyphs for beginning, middle or end of a word or a
         * paragraph, the individual @ref add() calls should ideally be made
         * with the same @p text view and the slices defined by @p begin and
         * @p end. Use the @ref add(AbstractShaper&, Float, Containers::StringView, Containers::ArrayView<const FeatureRange>)
         * overload to pass a string as a whole.
         *
         * The function uses @ref AbstractShaper::shape(),
         * @ref renderLineGlyphPositionsInto(), @ref alignRenderedLine() and
         * @ref glyphQuadBounds() internally, see their documentation for more
         * information.
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        RendererCore& add(AbstractShaper& shaper, Float size, Containers::StringView text, UnsignedInt begin, UnsignedInt end, Containers::ArrayView<const FeatureRange> features = {});
        #else
        /* To not have to include ArrayView */
        RendererCore& add(AbstractShaper& shaper, Float size, Containers::StringView text, UnsignedInt begin, UnsignedInt end);
        RendererCore& add(AbstractShaper& shaper, Float size, Containers::StringView text, UnsignedInt begin, UnsignedInt end, Containers::ArrayView<const FeatureRange> features);
        #endif

        /** @overload */
        RendererCore& add(AbstractShaper& shaper, Float size, Containers::StringView text, UnsignedInt begin, UnsignedInt end, std::initializer_list<FeatureRange> features);

        /**
         * @brief Add a whole string to the currently rendered text
         *
         * Equivalent to @ref add(AbstractShaper&, Float, Containers::StringView, UnsignedInt, UnsignedInt, Containers::ArrayView<const FeatureRange>)
         * with @p begin set to @cpp 0 @ce and @p end to size of @p text.
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        RendererCore& add(AbstractShaper& shaper, Float size, Containers::StringView text, Containers::ArrayView<const FeatureRange> features = {});
        #else
        /* To not have to include ArrayView */
        RendererCore& add(AbstractShaper& shaper, Float size, Containers::StringView text);
        RendererCore& add(AbstractShaper& shaper, Float size, Containers::StringView text, Containers::ArrayView<const FeatureRange> features);
        #endif

        /** @overload */
        RendererCore& add(AbstractShaper& shaper, Float size, Containers::StringView text, std::initializer_list<FeatureRange> features);

        /**
         * @brief Wrap up rendering of all text added so far
         *
         * Performs a final alignment of the text block added by preceding
         * @ref add() calls and wraps up the rendering. After calling this
         * function, @ref isRendering() returns @cpp false @ce,
         * @ref glyphCount() and @ref runCount() are updated with the count of
         * all rendered glyphs and runs, and @ref setCursor(),
         * @ref setAlignment() or @ref setLayoutDirection() can be called again
         * for the next text to be rendered.
         *
         * The function returns a bounding box and a range of runs of the
         * currently rendered text, the run range can then be used to index
         * the @ref runScales() and @ref runEnds() views. Note that it's
         * possible for the render to produce an empty range, such as when an
         * empty text was passed or when it was just newlines. You can use
         * @ref glyphsForRuns() to convert the returned run range to a begin
         * and end glyph offset, which can be then used to index the
         * @ref glyphPositions(), @ref glyphIds() and @ref glyphClusters()
         * views; when multipled by @cpp 6 @ce to index the
         * @ref Renderer::indices() view and when multiplied by @cpp 4 @ce to
         * index the @ref Renderer::vertexPositions() and
         * @relativeref{Renderer,vertexTextureCoordinates()} /
         * @relativeref{Renderer,vertexTextureArrayCoordinates()} views.
         *
         * The rendered glyph range is not touched or used by the renderer in
         * any way afterwards. If the renderer was created with custom
         * allocators, the caller can thus perform further operations on the
         * allocated data.
         *
         * Use @ref clear() or @ref reset() to discard all text rendered so
         * far. The function uses @ref alignRenderedBlock() internally, see its
         * documentation for more information.
         */
        Containers::Pair<Range2D, Range1Dui> render();

        /**
         * @brief Render a whole text at once
         *
         * A convenience shortcut for rendering a single piece of text that's
         * equivalent to calling @ref add(AbstractShaper&, Float, Containers::StringView, Containers::ArrayView<const FeatureRange>)
         * followed by @ref render(). See their documentation for more
         * information.
         *
         * After calling this function, @ref isRendering() returns
         * @cpp false @ce. If this function is called while rendering is in
         * progress, the glyphs rendered so far are included in the result as
         * well.
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        Containers::Pair<Range2D, Range1Dui> render(AbstractShaper& shaper, Float size, Containers::StringView text, Containers::ArrayView<const FeatureRange> features = {});
        #else
        /* To not have to include ArrayView */
        Containers::Pair<Range2D, Range1Dui> render(AbstractShaper& shaper, Float size, Containers::StringView text);
        Containers::Pair<Range2D, Range1Dui> render(AbstractShaper& shaper, Float size, Containers::StringView text, Containers::ArrayView<const FeatureRange> features);
        #endif

        /** @overload */
        Containers::Pair<Range2D, Range1Dui> render(AbstractShaper& shaper, Float size, Containers::StringView text, std::initializer_list<FeatureRange> features);

    #ifdef DOXYGEN_GENERATING_OUTPUT
    private:
    #else
    protected:
    #endif
        struct State;
        struct AllocatorState;
        Containers::Pointer<State> _state;

        /* Delegated to by Renderer constructors */
        explicit MAGNUM_TEXT_LOCAL RendererCore(Containers::Pointer<State>&& state);

        /* Called by reset() and Renderer::reset() */
        MAGNUM_TEXT_LOCAL void resetInternal();

    private:
        /* While the allocators get just size to grow by, these functions get
           the total count */
        MAGNUM_TEXT_LOCAL void allocateGlyphs(
            #ifndef CORRADE_NO_ASSERT
            const char* messagePrefix,
            #endif
            UnsignedInt totalGlyphCount);
        MAGNUM_TEXT_LOCAL void allocateRuns(
            #ifndef CORRADE_NO_ASSERT
            const char* messagePrefix,
            #endif
            UnsignedInt totalRunCount);
        MAGNUM_TEXT_LOCAL void alignAndFinishLine();
};

/**
@brief Text renderer flag
@m_since_latest

A superset of @ref RendererCoreFlag.
@see @ref RendererFlags, @ref Renderer
*/
enum class RendererFlag: UnsignedByte {
    /**
     * Populate glyph cluster info in @ref Renderer::glyphPositions() and
     * @ref Renderer::glyphClusters() for text selection and editing purposes.
     *
     * Compared to @ref RendererCore and @ref RendererCoreFlag::GlyphClusters,
     * the @ref Renderer by default queries glyph positions to a temporary
     * location that's later overwritten by quad vertex positions to save
     * memory so this flag includes both clusters and positions.
     */
    GlyphPositionsClusters = Int(RendererCoreFlag::GlyphClusters)

    /* Additions to this enum have to be propagated to RendererGLFlag and the
       mask in Renderer::flags() */
};

/**
 * @debugoperatorenum{RendererFlag}
 * @m_since_latest
 */
MAGNUM_TEXT_EXPORT Debug& operator<<(Debug& output, RendererFlag value);

/**
@brief Text renderer flags
@m_since_latest

A superset of @ref RendererCoreFlags.
@see @ref Renderer
*/
typedef Containers::EnumSet<RendererFlag> RendererFlags;

CORRADE_ENUMSET_OPERATORS(RendererFlags)

/**
 * @debugoperatorenum{RendererFlags}
 * @m_since_latest
 */
MAGNUM_TEXT_EXPORT Debug& operator<<(Debug& output, RendererFlags value);

/**
@brief Text renderer
@m_since_latest

Implements logic for rendering text formed from multiple runs, lines and fonts,
resulting in a textured quad mesh, optionally with glyph cluster information
that can be used to perform cursor and selection placement. You'll likely use
the renderer through the @ref RendererGL subclass, which directly populates a
@ref GL::Mesh instance with the rendered data.

The @ref RendererCore base implements just glyph positioning and layout, to be
used in scenarios where forming textured quads is deferred to later or not
needed at all. For lowest-level functionality see the @ref AbstractShaper
class, which exposes text shaping capabilities implemented directly by
particular font plugins.

@section Text-Renderer-usage Usage

Assuming you'll want to subsequently render the text with OpenGL, construct the
renderer using the @ref RendererGL subclass and an OpenGL glyph cache
implementation, such as a @ref GlyphCacheGL.

@snippet Text-gl.cpp Renderer-usage-construct

Before rendering with a particular @ref AbstractFont, the glyph cache has to be
filled with desired glyphs from it, if not done already, as shown below. The
@ref AbstractFont class documentation shows additional ways how to fill the
glyph cache.

@snippet Text.cpp Renderer-usage-fill

To render a text, pass an @ref AbstractShaper instance created from the font
together with desired font size and actual text to
@ref render(AbstractShaper&, Float, Containers::StringView, Containers::ArrayView<const FeatureRange>) "render()":

@snippet Text.cpp Renderer-usage-render

Once rendered, the @ref RendererGL::mesh() contains the rendered glyphs as
textured quads. The mesh can be drawn using @ref Shaders::VectorGL together
with binding @ref GlyphCacheGL::texture() for drawing. Usually you'll also want
to set up @ref GL::Renderer::Feature::Blending "alpha blending" so overlapping
glyph quads don't cut into each other. Assuming the drawing is performed in a
@ref Platform::Sdl2Application "Platform::*Application" subclass, the code
below sets up the drawing that the font pixel size matches the window pixels by
specifying @ref Platform::Sdl2Application::windowSize()  "Platform::*Application::windowSize()"
as the projection size, and it appears in the center of the window. With
@ref GlyphCacheArrayGL or @ref DistanceFieldGlyphCacheGL the drawing setup is
slightly different, see their documentation for examples.

@snippet Text-gl.cpp Renderer-usage-draw

If you use just the base @ref Renderer, the rendered index and vertex data are
exposed through @ref indices(), @ref vertexPositions() and
@ref vertexTextureCoordinates() / @ref vertexTextureArrayCoordinates(), which
you can pass to a custom renderer, for example. Likewise, the glyph cache can
be a custom @ref AbstractGlyphCache subclass that uploads the rasterized glyph
data to a texture in a custom renderer. For more control over data layout in
custom use cases see the @ref Text-Renderer-allocators section down below.

@subsection Text-Renderer-usage-layout-options Cursor, alignment and other layouting options

Internally, the renderer splits the passed text to individual lines and shapes
each separately, placing them together at a concrete cursor position with
specific alignment and line advance. The initial cursor position is at
@cpp {0.0f, 0.0f} @ce with a @ref Text::Alignment::MiddleCenter and line
advance matching @ref AbstractFont::lineHeight(). This can be overriden using
@ref setCursor(), @ref setAlignment() and @ref setLineAdvance() before
rendering a particular piece of text. The following snippet renders the same
text as above but wrapped to two lines, aligned to bottom right and positioned
ten pixels from the bottom right corner of the window:

@snippet Text.cpp Renderer-usage-layout-options

The renderer supports only horizontal text layout right now. The
@ref setLayoutDirection() API is currently just a placeholder for when vertical
layout is supported in the future as well.

@subsection Text-Renderer-usage-shape-properties Font script, language and shaping direction

The @ref AbstractShaper instance contains internal font-specific state used for
actual text shaping. If you create an instance and reuse it multiple times
instead of creating it on the fly, it allows the implementation to
@ref Text-AbstractShaper-instances "reuse allocated resources". It's also
possible to call @ref AbstractShaper::setScript(),
@relativeref{AbstractShaper,setLanguage()} and
@relativeref{AbstractShaper,setDirection()} on the instance if at least some
properties of the input text are known, which can make the shaping process
faster, or help in cases the properties can't be unambiguously detected from
the input:

@snippet Text.cpp Renderer-usage-shape-properties

The @ref Text-AbstractShaper-usage-properties "AbstractShaper documentation"
has additional info about how the shaping properties are handled and how to
check what's supported by a particular font plugin.

@subsection Text-Renderer-usage-shape-features Font features

The last argument to @ref render(AbstractShaper&, Float, Containers::StringView, Containers::ArrayView<const FeatureRange>) "render()"
allows enabling and disabling typographic features. For example, assuming the
font would support small capitals (and the particular @ref AbstractFont plugin
would recognize and use the feature), we could render the "world" part with
small caps, resulting in "Hello, ᴡᴏʀʟᴅ!":

@snippet Text.cpp Renderer-usage-shape-features

The behavior is equivalent to font features passed to @ref AbstractShaper::shape(),
see the @ref Text-AbstractShaper-usage-features "AbstractShaper documentation"
for further details.

@subsection Text-Renderer-usage-blocks Rendering multiple text blocks

Each call to @ref render(AbstractShaper&, Float, Containers::StringView, Containers::ArrayView<const FeatureRange>) "render()"
doesn't replace the previously rendered but *appends* to it, starting again
from the position specified with @ref setCursor(). Ultimately that means you
can render multiple blocks of text into the same mesh. The following snippet
places the two parts of the text to bottom left and bottom right window
corners:

@snippet Text.cpp Renderer-usage-blocks

Then you can either draw everything at once, with the same shader setup as
listed above, or draw particular parts with different settings. For that, the
@ref render(AbstractShaper&, Float, Containers::StringView, Containers::ArrayView<const FeatureRange>) "render()"
function returns two values --- a bounding box that can be used for various
placement and alignment purposes, and a range of text *runs* the rendered text
spans. A run is a range of glyphs together with an associated scale, and the
run offsets can be converted to glyph offsets using @ref glyphsForRuns().
Glyph offsets then directly correspond to vertex and index offsets, as each
glyph is a quad consisting of four vertices and six indices. The following code
draws each of the two pieces with a different color by making temporary
@ref GL::MeshView instances spanning just the corresponding glyph range:

@snippet Text-gl.cpp Renderer-usage-blocks-draw

Finally, @ref clear() discards all text rendered so far, meaning that the next
@ref render(AbstractShaper&, Float, Containers::StringView, Containers::ArrayView<const FeatureRange>) "render()"
will start from an empty state. The @ref reset() function additionally resets
also all other state like cursor position and alignment to default values.

@subsection Text-Renderer-usage-runs Rendering multiple text runs together

Besides rendering the whole text at once with a single font, it's possible to
render different parts of the text with different fonts, sizes or even just
differently configured shaper instances. This is done by using @ref add(),
which, compared to @ref render(AbstractShaper&, Float, Containers::StringView, Containers::ArrayView<const FeatureRange>) "render()", continues shaping where
the previous @ref add() ended instead of going back to the cursor specified
with @ref setCursor(). When done, a call to the parameter-less @ref render()
performs final alignment and wraps up the rendering. In the following snippet,
a bold font is used to render the "world" part:

@snippet Text.cpp Renderer-usage-runs

Fonts can be switched anywhere, not just at word boundaries. However, to make
use of the full shaping capabilities of a certain font implementation, it's
recommended to supply the text with additional context so the shaper can
perform kerning, glyph substitution and other operations even across the
individual pieces. This is done by passing the whole text every time and
specifying the range to shape by a begin and end *byte* offset into it,
allowing the shaper to peek outside of the actually shaped piece of text:

@snippet Text.cpp Renderer-usage-runs-begin-end

@section Text-Renderer-usage-font-size Font size

So far, for simplicity, the snippets above passed @ref AbstractFont::size() to
@ref render() or @ref add(), making the text rendered at exactly the size the
font glyphs were rasterized into the cache. The size at which the glyphs are
rasterized into the cache and the size at which a text is drawn on the screen
don't have to match, however.

When rendering the text, there are two common approaches --- either setting up
the size to match a global user interface scale, or having the text size
proportional to the window size. The first approach is what's shown in the
above snippets, with a projection that matches @ref Platform::Sdl2Application::windowSize() "Platform::*Application::windowSize()",
and results in e.g. a 12 pt font matching a 12 pt font in other applications.
With the regular @ref GlyphCacheGL / @ref GlyphCacheArrayGL, an even crisper
result may be achieved by doubly supersampling the rasterized font compared to
the size it's drawn at, which is shown in the following snippet. Additional
considerations for proper DPI awareness are described further below.

@snippet Text.cpp Renderer-dpi-supersampling

The second approach, with text size being relative to the window size, is for
cases where the text is meant to match surrounding art, such as in a game menu.
In this case the projection size is usually something arbitrary that doesn't
match window pixels, and the text point size then has to be relative to that.
For this use case a @ref DistanceFieldGlyphCacheGL is the better match, as it
can provide text at different sizes without the scaling causing blurriness or
aliased edges. See its documentation for details about picking the right font
size and other parameters for best results.

@subsection Text-Renderer-usage-font-size-dpi DPI awareness

To achieve crisp rendering and/or text size matching other applications on
HiDPI displays, additional steps need to be taken. There are two separate
concepts for DPI-aware rendering:

-   Interface size --- size at which the interface elements are positioned on
    the screen. Often, for simplicity, the interface is using some "virtual
    units", so a 12 pt font is still a 12 pt font independently of how the
    interface is scaled compared to actual display properties (for example by
    setting a global 150% scale in the desktop environment, or by zooming a
    browser window). The size passed to @ref render(AbstractShaper&, Float, Containers::StringView, Containers::ArrayView<const FeatureRange>) "render()"
    or @ref add() should match these virtual units.
-   Framebuffer size --- how many pixels is actually there. If a 192 DPI
    display has a 200% interface scale, a 12 pt font would be 32 pixels. But if
    it only has a 150% scale, all interface elements will be smaller, and a 12
    pt font would be only 24 pixels. The size used by the @ref AbstractFont and
    @ref GlyphCacheGL should be chosen with respect to the actual physical
    pixels.

When using for example @ref Platform::Sdl2Application or other `*Application`
implementations, you usually have three values at your disposal ---
@ref Platform::Sdl2Application::windowSize() "windowSize()",
@ref Platform::Sdl2Application::framebufferSize() "framebufferSize()" and
@ref Platform::Sdl2Application::dpiScaling() "dpiScaling()". Their relation is
documented thoroughly in @ref Platform-Sdl2Application-dpi, for this particular
case a scaled interface size, used instead of window size for the projection,
would be calculated like this:

@snippet Text.cpp Renderer-dpi-interface-size

And a multiplier for the @ref AbstractFont and @ref GlyphCacheGL font size like
this. The @ref render(AbstractShaper&, Float, Containers::StringView, Containers::ArrayView<const FeatureRange>) "render()"
or @ref add() keeps using the size without this multiplier.

@snippet Text.cpp Renderer-dpi-size-multiplier

@section Text-Renderer-performance Rendering and mesh drawing performance

To avoid repeated reallocations when rendering a larger chunk of text, when the
text is combined of many small runs or when rendering many separate text blocks
together, call @ref reserve() with expected glyph and run count.

For the mesh the @ref Renderer by default uses @ref MeshIndexType::UnsignedByte,
and changes it to a larger type each time the @ref glyphCapacity() exceeds the
range representable in given type. Call @ref setIndexType() if you want to use
a larger type right from the start even if the glyph capacity doesn't need it.
In case of @ref RendererGL, @ref MeshIndexType::UnsignedShort is used by
default instead, as 8-bit indices are discouraged on contemporary GPUs.

@todo update this once @ref AbstractShaper has @ref reserve() as well

@section Text-Renderer-clusters Mapping between input text and shaped glyphs

For implementing text selection or editing, if
@ref RendererFlag::GlyphPositionsClusters is enabled on @ref Renderer
construction, the renderer exposes also glyph position and cluster information
for each run via @ref glyphPositions() and @ref glyphClusters(). The clusters
are used the same way @ref Text-AbstractShaper-clusters "as described in the AbstractShaper documentation",
but additionally with mapping a particular text run to a concrete range of
glyphs for which to query the runs:

@snippet Text.cpp Renderer-clusters

When using @ref RendererGL, the flag is
@ref RendererGLFlag::GlyphPositionsClusters instead, for @ref RendererCore it's
just @ref RendererCoreFlag::GlyphClusters as glyph positions are available
always.

@section Text-Renderer-allocators Providing custom index and vertex data allocators

If you're using @ref Renderer directly and not the @ref RendererGL subclass,
it's possible to hook up custom allocators for index and vertex data. Let's say
that you want to have per-vertex colors in addition to positions and texture
coordinates. The text renderer produces position and texture coordinate data
inside @ref render() and colors get filled in after, by querying the vertex
range corresponding to the produced glyph run produced using
@ref glyphsForRuns():

@snippet Text.cpp Renderer-allocators-vertex

In case of indices you can for example use a single statically-allocated memory
across all renderers, to avoid each allocating its own copy:

@snippet Text.cpp Renderer-allocators-index

Expected allocator behavior is fully documented in the @ref Renderer()
constructor, note especially the differences when array glyph caches are used.
The @ref Text-RendererCore-allocators section in the @ref RendererCore
documentation shows more use cases with examples for the remaining two
allocators.
*/
class MAGNUM_TEXT_EXPORT Renderer: public RendererCore {
    public:
        /**
         * @brief Construct
         * @param glyphCache    Glyph cache to use
         * @param flags         Opt-in feature flags
         *
         * By default, the renderer allocates the memory for glyph, run, index
         * and vertex data internally. Use the overload below to supply
         * external allocators.
         * @todoc the damn thing can't link to functions taking functions
         */
        explicit Renderer(const AbstractGlyphCache& glyphCache, RendererFlags flags = {}): Renderer{glyphCache, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, flags} {}

        /**
         * @brief Construct with external allocators
         * @param glyphCache        Glyph cache to use for glyph ID mapping
         * @param glyphAllocator    Glyph allocator function or @cpp nullptr @ce
         * @param glyphAllocatorState  State pointer to pass to @p glyphAllocator
         * @param runAllocator      Run allocator function or @cpp nullptr @ce
         * @param runAllocatorState  State pointer to pass to @p runAllocator
         * @param indexAllocator   Index allocator function or @cpp nullptr @ce
         * @param indexAllocatorState  State pointer to pass to @p indexAllocator
         * @param vertexAllocator   Vertex allocator function or @cpp nullptr @ce
         * @param vertexAllocatorState  State pointer to pass to @p vertexAllocator
         * @param flags             Opt-in feature flags
         *
         * The @p glyphAllocator gets called with desired @p glyphCount every
         * time @ref glyphCount() reaches @ref glyphCapacity(). Size of
         * passed-in @p glyphPositions, @p glyphIds and @p glyphClusters views
         * matches @ref glyphCount(). The @p glyphAdvances view is a temporary
         * storage with contents that don't need to be preserved on
         * reallocation and is thus passed in empty. If the renderer wasn't
         * constructed with @ref RendererFlag::GlyphPositionsClusters, the
         * @p glyphClusters is @cpp nullptr @ce to indicate it's not meant to
         * be allocated. The allocator is expected to replace all passed views
         * with new views that are larger by *at least* @p glyphCount, pointing
         * to a reallocated memory with contents from the original view
         * preserved. Initially @ref glyphCount() is @cpp 0 @ce and the views
         * are all passed in empty, every subsequent time the views match a
         * prefix of views previously returned by the allocator. To save
         * memory, the renderer guarantees that @p glyphIds and
         * @p glyphClusters are only filled once @p glyphAdvances were merged
         * into @p glyphPositions. In other words, the @p glyphAdvances can
         * alias a suffix of @p glyphIds and @p glyphClusters.
         *
         * The @p runAllocator gets called with desired @p runCount every time
         * @ref runCount() reaches @ref runCapacity(). Size of passed-in
         * @p runScales and @p runEnds views matches @ref runCount(). The
         * allocator is expected to replace the views with new views that are
         * larger by *at least* @p runCount, pointing to a reallocated memory
         * with contents from the original views preserved. Initially
         * @ref runCount() is @cpp 0 @ce and the views are passed in empty,
         * every subsequent time the views match a prefix of views previously
         * returned by the allocator.
         *
         * The @p indexAllocator gets called with desired @p size every time
         * @ref glyphCapacity() increases. Size of passed-in @p indices array
         * either matches @ref glyphCapacity() times @cpp 6 @ce times size of
         * @ref indexType() if the index type stays the same, or is empty if
         * the index type changes (and the whole index array is going to
         * get rebuilt with a different type, thus no contents need to be
         * preserved). The allocator is expected to replace the passed view
         * with a new view that's larger by *at least* @p size, pointing to a
         * reallocated memory with contents from the original view preserved.
         * Initially @ref glyphCapacity() is @cpp 0 @ce and the view is passed
         * in empty, every subsequent time the view matches a prefix of the
         * view previously returned by the allocator.
         *
         * The @p vertexAllocator gets called with @p vertexCount every time
         * @ref glyphCount() reaches @ref glyphCapacity(). Size of passed-in
         * @p vertexPositions and @p vertexTextureCoordinates views matches
         * @ref glyphCount() times @cpp 4 @ce. The allocator is expected to
         * replace the views with new views that are larger by *at least*
         * @p vertexCount, pointing to a reallocated memory with contents from
         * the original views preserved. Initially @ref glyphCount() is
         * @cpp 0 @ce and the views are passed in empty, every subsequent time
         * the views match a prefix of views previously returned by the
         * allocator. If the @p glyphCache is an array, the allocator is
         * expected to (re)allocate @p vertexTextureCoordinates for a
         * @relativeref{Magnum,Vector3} type even though the view points to
         * just the first two components of each texture coordinates.
         *
         * The renderer always requests only exactly the desired size and the
         * growth strategy is up to the allocators themselves --- the returned
         * glyph and run views can be larger than requested and aren't all
         * required to all have the same size. The minimum of size increases
         * across all views is then treated as the new @ref glyphCapacity(),
         * @ref glyphIndexCapacity(), @ref glyphVertexCapacity() and
         * @ref runCapacity().
         *
         * As a special case, when @ref clear() or @ref reset() is called, the
         * allocators are called with empty views and @p glyphCount /
         * @p runCount / @p size / @p vertexCount being @cpp 0 @ce. This is to
         * allow the allocators to perform any needed reset as well.
         *
         * If @p glyphAllocator, @p runAllocator, @p indexAllocator or
         * @p vertexAllocator is @cpp nullptr @ce, @p glyphAllocatorState,
         * @p runAllocatorState, @p indexAllocatorState or
         * @p vertexAllocatorState is ignored and default builtin allocator get
         * used for either. Passing @cpp nullptr @ce for all is equivalent to
         * calling the @ref Renderer(const AbstractGlyphCache&, RendererFlags)
         * constructor.
         */
        explicit Renderer(const AbstractGlyphCache& glyphCache, void(*glyphAllocator)(void* state, UnsignedInt glyphCount, Containers::StridedArrayView1D<Vector2>& glyphPositions, Containers::StridedArrayView1D<UnsignedInt>& glyphIds, Containers::StridedArrayView1D<UnsignedInt>* glyphClusters, Containers::StridedArrayView1D<Vector2>& glyphAdvances), void* glyphAllocatorState, void(*runAllocator)(void* state, UnsignedInt runCount, Containers::StridedArrayView1D<Float>& runScales, Containers::StridedArrayView1D<UnsignedInt>& runEnds), void* runAllocatorState, void(*indexAllocator)(void* state, UnsignedInt size, Containers::ArrayView<char>& indices), void* indexAllocatorState, void(*vertexAllocator)(void* state, UnsignedInt vertexCount, Containers::StridedArrayView1D<Vector2>& vertexPositions, Containers::StridedArrayView1D<Vector2>& vertexTextureCoordinates), void* vertexAllocatorState, RendererFlags flags = {});

        /**
         * @brief Construct without creating the internal state
         * @m_since_latest
         *
         * The constructed instance is equivalent to moved-from state, i.e. no
         * APIs can be safely called on the object. Useful in cases where you
         * will overwrite the instance later anyway. Move another object over
         * it to make it useful.
         *
         * Note that this is a low-level and a potentially dangerous API, see
         * the documentation of @ref NoCreate for alternatives.
         */
        explicit Renderer(NoCreateT) noexcept: RendererCore{NoCreate} {}

        /** @brief Copying is not allowed */
        Renderer(Renderer&) = delete;

        /**
         * @brief Move constructor
         *
         * Performs a destructive move, i.e. the original object isn't usable
         * afterwards anymore.
         */
        Renderer(Renderer&&) noexcept;

        ~Renderer();

        /** @brief Copying is not allowed */
        Renderer& operator=(Renderer&) = delete;

        /** @brief Move assignment */
        Renderer& operator=(Renderer&&) noexcept;

        /** @brief Flags */
        RendererFlags flags() const;

        /**
         * @brief Glyph index capacity
         *
         * Describes how many glyphs can be rendered into the index buffer. The
         * actual index count is six times the capacity.
         * @see @ref glyphCapacity(), @ref glyphVertexCapacity(),
         *      @ref glyphCount(), @ref runCapacity(), @ref reserve()
         */
        UnsignedInt glyphIndexCapacity() const;

        /**
         * @brief Glyph vertex capacity
         *
         * Describes how many glyphs can be rendered into the vertex buffer.
         * The actual vertex count is four times the capacity.
         * @see @ref glyphCapacity(), @ref glyphIndexCapacity(),
         *      @ref glyphCount(), @ref runCapacity(), @ref reserve()
         */
        UnsignedInt glyphVertexCapacity() const;

        /**
         * @brief Index type
         *
         * The smallest type that can describe vertices for all
         * @ref glyphCapacity() glyphs and isn't smaller than what was set in
         * @ref setIndexType(). Initially set to
         * @ref MeshIndexType::UnsignedByte, a lerger type is automatically
         * switched to once the capacity exceeds @cpp 64 @ce and @cpp 16384 @ce
         * glyphs.
         */
        MeshIndexType indexType() const;

        /**
         * @brief Set index type
         * @return Reference to self (for method chaining)
         *
         * Sets the smallest possible index type to be used. Initially
         * @ref MeshIndexType::UnsignedByte, a larger type is automatically
         * switched to once @ref glyphCapacity() exceeds @cpp 64 @ce and
         * @cpp 16384 @ce glyphs. Set to a larger type if you want it to be
         * used even if the glyph capacity is smaller. Setting it back to a
         * smaller type afterwards uses the type only if the glyph capacity
         * allows it.
         */
        Renderer& setIndexType(MeshIndexType atLeast);

        /**
         * @brief Glyph positions
         *
         * Expects that the renderer was constructed with
         * @ref RendererFlag::GlyphPositionsClusters. The returned view has a
         * size of @ref glyphCount(). Note that the contents are not guaranteed
         * to be meaningful if custom glyph allocator is used, as the user code
         * is free to perform subsequent operations on those.
         */
        Containers::StridedArrayView1D<const Vector2> glyphPositions() const;

        /**
         * @brief Glyph IDs are not accessible
         *
         * Unlike with @ref RendererCore, to save memory, glyph IDs are
         * retrieved only to a temporary location to produce glyph quads and
         * are subsequently overwritten by vertex data.
         */
        Containers::StridedArrayView1D<const UnsignedInt> glyphIds() const = delete;

        /**
         * @brief Glyph cluster IDs
         *
         * Expects that the renderer was constructed with
         * @ref RendererFlag::GlyphPositionsClusters. The returned view has a
         * size of @ref glyphCount(). Note that the contents are not guaranteed
         * to be meaningful if custom glyph allocator is used, as the user code
         * is free to perform subsequent operations on those.
         */
        Containers::StridedArrayView1D<const UnsignedInt> glyphClusters() const;

        /**
         * @brief Type-erased glyph quad indices
         *
         * The returned view is contiguous with a size of @ref glyphCount()
         * times @cpp 6 @ce, the second dimension having a size of
         * @ref indexType(). The values index the  @ref vertexPositions() and
         * @ref vertexTextureCoordinates() / @ref vertexTextureArrayCoordinates()
         * arrays. Note that the contents are not guaranteed to be meaningful
         * if custom index allocator is used, as the user code is free to
         * perform subsequent operations on those.
         *
         * Use the templated overload below to get the indices in a concrete
         * type.
         */
        Containers::StridedArrayView2D<const char> indices() const;

        /**
         * @brief Glyph quad indices
         *
         * Expects that @p T is either @relativeref{Magnum,UnsignedByte},
         * @relativeref{Magnum,UnsignedShort} or
         * @relativeref{Magnum,UnsignedInt} and matches @ref indexType(). The
         * returned view has a size of @ref glyphCount() times @cpp 6 @ce. Note
         * that the contents are not guaranteed to be meaningful if custom
         * index allocator is used, as the user code is free to perform
         * subsequent operations on those.
         *
         * Use the non-templated overload above to get a type-erased view on
         * the indices.
         */
        template<class T> Containers::ArrayView<const T> indices() const;

        /**
         * @brief Vertex positions
         *
         * The returned view has a size of @ref glyphCount() times @cpp 4 @ce.
         * Note that the contents are not guaranteed to be meaningful if custom
         * vertex allocator is used, as the user code is free to perform
         * subsequent operations on those.
         */
        Containers::StridedArrayView1D<const Vector2> vertexPositions() const;

        /**
         * @brief Vertex texture coordinates
         *
         * Expects that the renderer was constructed with a non-array
         * @ref AbstractGlyphCache, i.e. with a depth equal to @cpp 1 @ce.
         * The returned view has a size of @ref glyphCount() times @cpp 4 @ce.
         * Note that the contents are not guaranteed to be meaningful if custom
         * vertex allocator is used, as the user code is free to perform
         * subsequent operations on those.
         */
        Containers::StridedArrayView1D<const Vector2> vertexTextureCoordinates() const;

        /**
         * @brief Vertex texture array coordinates
         *
         * Expects that the renderer was constructed with an array
         * @ref AbstractGlyphCache, i.e. with a depth larger than @cpp 1 @ce.
         * The returned view has a size of @ref glyphCount() times @cpp 4 @ce.
         * Note that the contents are not guaranteed to be meaningful if custom
         * vertex allocator is used, as the user code is free to perform
         * subsequent operations on those.
         */
        Containers::StridedArrayView1D<const Vector3> vertexTextureArrayCoordinates() const;

        /**
         * @brief Reserve capacity for given glyph count
         * @return Reference to self (for method chaining)
         *
         * Calls @ref RendererCore::reserve() and additionally reserves
         * capacity also for the corresponding index and vertex memory. Note
         * that while reserved index and vertex capacity is derived from
         * @p glyphCapacity and @ref indexType(), their actually allocated
         * capacity doesn't need to match @ref glyphCapacity() and is exposed
         * through @ref glyphIndexCapacity() and @ref glyphVertexCapacity().
         * @see @ref glyphCount(), @ref runCapacity(), @ref runCount()
         */
        Renderer& reserve(UnsignedInt glyphCapacity, UnsignedInt runCapacity);

        /**
         * @brief Clear rendered glyphs, runs and vertices
         * @return Reference to self (for method chaining)
         *
         * Calls @ref RendererCore::clear(). The @ref glyphCount() and
         * @ref runCount() becomes @cpp 0 @ce after this call and any
         * in-progress rendering is discarded, making @ref isRendering() return
         * @cpp false @ce. If custom glyph, run or vertex allocators are used,
         * they get called with empty views and zero sizes. Custom index
         * allocator isn't called however, as the index buffer only needs
         * updating when its capacity isn't large enough.
         *
         * Depending on allocator used, @ref glyphCapacity(),
         * @ref glyphVertexCapacity() and @ref runCapacity() may stay non-zero.
         * The @ref cursor(), @ref alignment(), @ref lineAdvance() and
         * @ref layoutDirection() are left untouched, use @ref reset() to reset
         * those to their default values as well.
         */
        Renderer& clear();

        /**
         * @brief Reset internal renderer state
         * @return Reference to self (for method chaining)
         *
         * Calls @ref clear(), and additionally @ref cursor(),
         * @ref alignment(), @ref lineAdvance() and @ref layoutDirection() are
         * reset to their default values. Apart from @ref glyphCapacity(),
         * @ref glyphVertexCapacity() and @ref runCapacity() which may stay
         * non-zero depending on allocator used, and @ref glyphIndexCapacity()
         * plus @ref indexType() which are left untouched, the instance is
         * equivalent to a default-constructed state.
         */
        Renderer& reset();

        /**
         * @brief Wrap up rendering of all text added so far
         *
         * Calls @ref RendererCore::render() and populates also index and
         * vertex data, subsequently available through @ref indices(),
         * @ref vertexPositions() and @ref vertexTextureCoordinates() /
         * @ref vertexTextureArrayCoordinates().
         *
         * The function uses @ref renderGlyphQuadsInto() and
         * @ref renderGlyphQuadIndicesInto() internally, see their
         * documentation for more information.
         */
        Containers::Pair<Range2D, Range1Dui> render();

        /* Overloads to remove a WTF factor from method chaining order, and to
           ensure our render() is called instead of RenderCore::render() */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        Renderer& setCursor(const Vector2& cursor) {
            return static_cast<Renderer&>(RendererCore::setCursor(cursor));
        }
        Renderer& setAlignment(Alignment alignment) {
            return static_cast<Renderer&>(RendererCore::setAlignment(alignment));
        }
        Renderer& setLineAdvance(Float advance) {
            return static_cast<Renderer&>(RendererCore::setLineAdvance(advance));
        }
        Renderer& setLayoutDirection(LayoutDirection direction) {
            return static_cast<Renderer&>(RendererCore::setLayoutDirection(direction));
        }

        Renderer& add(AbstractShaper& shaper, Float size, Containers::StringView text, UnsignedInt begin, UnsignedInt end, Containers::ArrayView<const FeatureRange> features);
        Renderer& add(AbstractShaper& shaper, Float size, Containers::StringView text, UnsignedInt begin, UnsignedInt end);
        Renderer& add(AbstractShaper& shaper, Float size, Containers::StringView text, UnsignedInt begin, UnsignedInt end, std::initializer_list<FeatureRange> features);
        Renderer& add(AbstractShaper& shaper, Float size, Containers::StringView text, Containers::ArrayView<const FeatureRange> features);
        Renderer& add(AbstractShaper& shaper, Float size, Containers::StringView text);
        Renderer& add(AbstractShaper& shaper, Float size, Containers::StringView text, std::initializer_list<FeatureRange> features);

        Containers::Pair<Range2D, Range1Dui> render(AbstractShaper& shaper, Float size, Containers::StringView text, Containers::ArrayView<const FeatureRange> features);
        Containers::Pair<Range2D, Range1Dui> render(AbstractShaper& shaper, Float size, Containers::StringView text);
        Containers::Pair<Range2D, Range1Dui> render(AbstractShaper& shaper, Float size, Containers::StringView text, std::initializer_list<FeatureRange> features);
        #endif

    #ifdef DOXYGEN_GENERATING_OUTPUT
    private:
    #else
    protected:
    #endif
        struct State;

        /* Delegated to by RendererGL constructors */
        explicit MAGNUM_TEXT_LOCAL Renderer(Containers::Pointer<State>&& state);

    private:
        /* While the allocators get just size to grow by, these functions get
           the total count */
        MAGNUM_TEXT_LOCAL void allocateIndices(
            #ifndef CORRADE_NO_ASSERT
            const char* messagePrefix,
            #endif
            UnsignedInt totalGlyphCount);
        MAGNUM_TEXT_LOCAL void allocateVertices(
            #ifndef CORRADE_NO_ASSERT
            const char* messagePrefix,
            #endif
            UnsignedInt totalGlyphCount);
};

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
@see @ref glyphQuadBounds()
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
@see @ref AbstractGlyphCache::glyphIdsInto(), @ref glyphQuadBounds()
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
@brief Calculate glyph quad bounds from cache-global glyph IDs
@param[in] cache                Glyph cache to query for glyph rectangles
@param[in] scale                Size to render the glyphs at divided by size of
    the input font
@param[in] glyphPositions       Glyph positions coming from an earlier call to
    @ref renderLineGlyphPositionsInto()
@param[in] glyphIds             Cache-global glyph IDs
@return Rectangle spanning the glyph quads
@m_since_latest

Returns the same rectangle as @ref renderGlyphQuadsInto(const AbstractGlyphCache&, Float, const Containers::StridedArrayView1D<const Vector2>&, const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView1D<Vector2>&, const Containers::StridedArrayView1D<Vector3>&)
but without actually generating the glyph quads. Use the returned value for
@ref alignRenderedLine() with a `*GlyphBounds` @ref Alignment value.

Note that, unlike with @ref renderGlyphQuadsInto(const AbstractFont&, Float, const AbstractGlyphCache&, const Containers::StridedArrayView1D<const Vector2>&, const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView1D<Vector2>&, const Containers::StridedArrayView1D<Vector3>&),
this function doesn't have an overload taking font-local glyph IDs, as it
doesn't have any scratch space to store them. Use
@ref AbstractGlyphCache::glyphIdsInto() instead to convert them to cache-global
and then call this function with the result.
*/
MAGNUM_TEXT_EXPORT Range2D glyphQuadBounds(const AbstractGlyphCache& cache, Float scale, const Containers::StridedArrayView1D<const Vector2>& glyphPositions, const Containers::StridedArrayView1D<const UnsignedInt>& glyphIds);

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

If @p alignment is `*GlyphBounds`, this function should get glyph @p positions
for the whole line coming from @ref renderLineGlyphPositionsInto() and a
@p lineRectangle being generated with @ref glyphQuadBounds() from those
positions. Alternatively, it should get vertex @p positions for a whole line
coming from @ref renderGlyphQuadsInto() and @p lineRectangle being all
rectangles returned by that function combined together with
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
given range. Assumes that @p clusters are either monotonically non-decreasing
or non-increasing.

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

/* As the deprecated AbstractRenderer relies on GL functionality, it'd be
   ideally put into RendererGL.h, but that'd be a breaking change as existing
   code expects it in Renderer.h. OTOH, the implementation is in RendererGL.cpp
   to avoid including GL headers and nasty STL things in Renderer.cpp as
   well. */
#if defined(MAGNUM_TARGET_GL) && defined(MAGNUM_BUILD_DEPRECATED)
/**
@brief OpenGL text renderer
@m_deprecated_since_latest Use @ref Renderer or @ref RendererGL instead, which
    expose a superset of the functionality through a more efficient interface.
    It doesn't distinguish between 2D and 3D anymore as the original
    implementation had two-component vertex positions in 3D as well. If you
    really need a third coordinate for transforming individual glyphs, use a
    @ref Text-Renderer-allocators "custom Renderer vertex allocator".

Lays out the text into mesh using given font. Use of ligatures, kerning etc.
depends on features supported by particular font and its layouter.

@section Text-BasicRenderer-usage Usage

Immutable text (e.g. menu items, credits) can be simply rendered using static
methods, returning result either as data arrays or as fully configured mesh.
The text can be then drawn as usual by configuring the shader and drawing the
mesh:

@snippet Text-gl.cpp Renderer2D-usage1

See @ref render(AbstractFont&, const AbstractGlyphCache&, Float, const std::string&, Alignment)
and @ref render(AbstractFont&, const AbstractGlyphCache&, Float, const std::string&, GL::Buffer&, GL::Buffer&, GL::BufferUsage, Alignment)
for more information.

While this method is sufficient for one-shot rendering of static texts, for
mutable texts (e.g. FPS counters, chat messages) there is another approach
that doesn't recreate everything on each text change:

@snippet Text-gl.cpp Renderer2D-usage2

@note This class is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.

@see @ref Renderer2D, @ref Renderer3D, @ref AbstractFont,
    @ref Shaders::VectorGL, @ref Shaders::DistanceFieldVectorGL
*/
class CORRADE_DEPRECATED("use Renderer or RendererGL instead") MAGNUM_TEXT_EXPORT AbstractRenderer {
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
         * @brief Render text
         * @param font          Font
         * @param cache         Glyph cache
         * @param size          Font size
         * @param text          Text to render
         * @param vertexBuffer  Buffer where to store vertices
         * @param indexBuffer   Buffer where to store indices
         * @param usage         Ignored, provided just for backward
         *      compatibility
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
         *
         * Expects that @p font is present in @p cache and that @p cache isn't
         * an array.
         */
        explicit AbstractRenderer(AbstractFont& font, const AbstractGlyphCache& cache, Float size, Alignment alignment = Alignment::LineLeft);
        explicit AbstractRenderer(AbstractFont&, AbstractGlyphCache&&, Float, Alignment alignment = Alignment::LineLeft) = delete; /**< @overload */

        /** @brief Copying is not allowed */
        CORRADE_IGNORE_DEPRECATED_PUSH /* GCC 4.8 warns due to the argument */
        AbstractRenderer(AbstractRenderer&) = delete;
        CORRADE_IGNORE_DEPRECATED_POP

        /**
         * @brief Move constructor
         *
         * Performs a destructive move, i.e. the original object isn't usable
         * afterwards anymore.
         */
        CORRADE_IGNORE_DEPRECATED_PUSH /* GCC 4.8 warns due to the argument */
        AbstractRenderer(AbstractRenderer&&) noexcept;
        CORRADE_IGNORE_DEPRECATED_POP

        ~AbstractRenderer();

        /** @brief Copying is not allowed */
        CORRADE_IGNORE_DEPRECATED_PUSH /* GCC 4.8 warns due to the argument */
        AbstractRenderer& operator=(AbstractRenderer&) = delete;
        CORRADE_IGNORE_DEPRECATED_POP

        /** @brief Move assignment is not allowed */
        /* Because it contains reference members. Not going to fix this, just
           pinning down existing behavior. */
        CORRADE_IGNORE_DEPRECATED_PUSH /* GCC 4.8 warns due to the argument */
        AbstractRenderer& operator=(AbstractRenderer&&) = delete;
        CORRADE_IGNORE_DEPRECATED_POP

        /**
         * @brief Capacity for rendered glyphs
         *
         * @see @ref reserve()
         */
        UnsignedInt capacity() const;

        /**
         * @brief Font size in points
         * @m_since_latest
         */
        Float fontSize() const { return _fontSize; }

        /** @brief Rectangle spanning the rendered text */
        Range2D rectangle() const { return _rectangle; }

        /** @brief Vertex buffer */
        GL::Buffer& vertexBuffer();

        /** @brief Index buffer */
        GL::Buffer& indexBuffer();

        /** @brief Mesh */
        GL::Mesh& mesh();

        /**
         * @brief Reserve capacity for rendered glyphs
         *
         * Reallocates memory in buffers to hold @p glyphCount glyphs and
         * prefills index buffer. The @p vertexBufferUsage and
         * @p indexBufferUsage parameters are ignored and provided just for
         * backward compatibility.
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
         */
        void render(const std::string& text);

    private:
        /* Cannot be a base because RendererGL is defined in another header,
           and making a cyclic dependency between the two for *all* users is
           worse than one extra allocation for just some */
        Containers::Pointer<RendererGL> _renderer;
        AbstractFont& _font;
        Float _fontSize;
        Range2D _rectangle;
};

/**
@brief Two-dimensional text renderer
@m_deprecated_since_latest Use @ref Renderer or @ref RendererGL instead, which
    expose a superset of the functionality through a more efficient interface.
    It doesn't distinguish between 2D and 3D anymore as the original
    implementation had two-component vertex positions in 3D as well. If you
    really need a third coordinate for transforming individual glyphs, use a
    @ref Text-Renderer-allocators "custom Renderer vertex allocator".

@note This class is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.
*/
CORRADE_IGNORE_DEPRECATED_PUSH /* idiotic MSVC warns for deprecated APIs using deprecated APIs */
typedef CORRADE_DEPRECATED("use Renderer or RendererGL instead") AbstractRenderer Renderer2D;
CORRADE_IGNORE_DEPRECATED_POP

/**
@brief Three-dimensional text renderer
@m_deprecated_since_latest Use @ref Renderer or @ref RendererGL instead, which
    expose a superset of the functionality through a more efficient interface.
    It doesn't distinguish between 2D and 3D anymore as the original
    implementation had two-component vertex positions in 3D as well. If you
    really need a third coordinate for transforming individual glyphs, use a
    @ref Text-Renderer-allocators "custom Renderer vertex allocator".

@note This class is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.
*/
CORRADE_IGNORE_DEPRECATED_PUSH /* idiotic MSVC warns for deprecated APIs using deprecated APIs */
typedef CORRADE_DEPRECATED("use Renderer or RendererGL instead") AbstractRenderer Renderer3D;
CORRADE_IGNORE_DEPRECATED_POP
#endif

}}

#endif
