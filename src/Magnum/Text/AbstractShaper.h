#ifndef Magnum_Text_AbstractShaper_h
#define Magnum_Text_AbstractShaper_h
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
 * @brief Class @ref Magnum::Text::AbstractShaper
 * @m_since_latest
 */

#include <initializer_list>
#include <Corrade/Containers/Reference.h>

#include "Magnum/Magnum.h"
#include "Magnum/Text/Text.h"
#include "Magnum/Text/visibility.h"

namespace Magnum { namespace Text {

/**
@brief Base for text shapers
@m_since_latest

Returned from @ref AbstractFont::createShaper(), provides a low-level interface
for *shaping* text with the @ref AbstractFont it originated from. Meant to be
(privately) subclassed by @ref AbstractFont plugin implementations. For common
text rendering you'll likely want to use the high-level @ref Renderer, which
then invokes @ref AbstractShaper internally.

* *Shaping* is a process of converting a sequence of Unicode codepoints to a
visual form, i.e. a list of glyphs of a particular font, their offsets and horizontal or vertical advances. Shaping is often not a 1:1 mapping from
codepoints to glyphs, but involves merging, subdividing and reordering as well.

@section Text-AbstractShaper-usage Usage

Call @ref AbstractFont::createShaper() to get a shaper instance. The plugin
will always return a valid instance so it's not needed to check for the pointer
beging @cpp nullptr @ce, however note that the originating @ref AbstractFont
instance has to stay in scope for at least as long as the @ref AbstractShaper
is alive.

A text is shaped by calling @ref shape(), retrieving the shaped glyph count
with @ref glyphCount() and then getting the glyph data with @ref glyphIdsInto()
and @ref glyphOffsetsAdvancesInto(). Glyph IDs can be then queried in (or
inserted into) an @ref AbstractGlyphCache, and the rendered glyphs positioned
at @p offsets with the cursor moving by @p advances is what makes up the final
shaped text.

@snippet Text.cpp AbstractShaper-shape

@subsection Text-AbstractShaper-usage-properties Specifying shaping properties

By default, and depending on the font plugin capabilities, the shaper
autodetects the script, language and direction of the shaped text. It's
possible to call (a subset of) @ref setScript(), @ref setLanguage() and
@ref setDirection() if at least some properties of the input text are known,
which can make the shaping process faster, or help in cases the properties
can't be unambiguously detected from the input:

@snippet Text.cpp AbstractShaper-shape-properties

If a particular font plugin doesn't implement given script, language or
direction or if it doesn't have any special handling for it, given function
will return @cpp false @ce. The @ref script() const, @ref language() const and
@ref direction() const can be used to inspect results of autodetection after
@ref shape() has been called. Setting @ref Script::Unspecified, an empty
language string and @ref ShapeDirection::Unspecified makes the implementation
go back to autodetection for the next shaping operation.

The set of supported scripts, languages and directions and exact behavior for
unsupported values is plugin-specific --- it may for example choose a fallback
instead, or it may ignore the setting altogeter. See documentation of
particular @ref AbstractFont subclasses for more information.

@subsection Text-AbstractShaper-usage-features Enabling and disabling typographic features

In the above snippet, the whole text is shaped using typographic features that
are default in the font. The last argument to @ref shape() takes a list of
@ref FeatureRange items to override those. For example, assuming the font would
support small capitals (and the particular @ref AbstractFont plugin would
recognize and use the feature), we could render the "world" part with small
caps, resulting in "Hello, ᴡᴏʀʟᴅ!".

@snippet Text.cpp AbstractShaper-shape-features

Similarly, features can be enabled for the whole text by omitting the begin and
end parameters, or for example a feature that a particular @ref AbstractFont
plugin uses by default can be disabled by passing an explicit @cpp false @ce
argument. The range, if present, is always given in *bytes* of the UTF-8 input.
Capabilities of typographic features are rather broad, see the @ref Feature
enum and documentation linked from it for exhaustive information.

@section Text-AbstractShaper-multiple Combining different shapers

If it's desirable to render different parts of the text with different fonts,
the output from multiple shapers can be combined togeter. The following code is
a variation of the above example, shaping the "world" part with a bold font,
although in a quite verbose way compared to
@ref Text-Renderer-usage-runs "the same achieved with the high-level Renderer":

@snippet Text.cpp AbstractShaper-shape-multiple

The resulting `glyphs` array is usable the same way as in the above case, with
a difference that the glyph IDs have to be looked up in an
@ref AbstractGlyphCache with a font ID corresponding to the range they're in.
Similarly as with the linked higher-level example, the whole text is passed
every time and a begin & end is specified for it instead of passing just the
slice alone, to allow the shaper to get additional context if needed.

@section Text-AbstractShaper-instances Managing multiple instances

As shown above, a particular @ref AbstractShaper instance is reusable, i.e.
it's possible to call @ref shape() (and potentially also @ref setScript(),
@ref setLanguage() and @ref setDirection()) several times to shape multiple
pieces of text with it. Doing so allows the @ref AbstractFont plugin
implementation to reuse allocated buffers and other state compared to a fresh
instance from @ref AbstractFont::createShaper() having to be initialized every
time.

The application may choose several strategies, for example have a single
@ref AbstractShaper instance and shape all texts with it, resetting its state
every time. Or for example have a few persistent @ref AbstractShaper instances
for dynamic text that changes every frame, or have dedicated preconfigured
per-font, per-script or per-language instances.

@section Text-AbstractShaper-clusters Mapping between input text and shaped glyphs

For implementing text selection or editing, mapping from screen position to
concrete glyphs can be done using the advances returned from
@ref glyphOffsetsAdvancesInto(). From there however, in the general case, the
text can consist of multi-byte UTF-8 characters, the shaper can perform
ligature substitutions, glyph decomposition or reordering, and thus there's
rarely a 1:1 mapping from the shaped glyphs back to the input text.

The mapping from glyph IDs to bytes of the text passed to @ref shape() can be
retrieved using @ref glyphClustersInto(). In the following example, a range
between glyphs @cpp 2 @ce and @cpp 5 @ce is mapped to the input text bytes, for
example to copy it as a selection to clipboard:

@snippet Text.cpp AbstractShaper-shape-clusters-to-bytes

In the other direction, picking a range of glyphs corresponding to a range of
input bytes, involves finding cluster IDs matching given byte positions, which
is doable with the @ref glyphRangeForBytes() utility:

@snippet Text.cpp AbstractShaper-shape-bytes-to-clusters

See the documentation of @ref glyphClustersInto() for concrete examples of how
retrieved cluster IDs may look like depending on what operations the shaper
performs.

@section Text-AbstractShaper-subclassing Subclassing

The @ref AbstractFont plugin is meant to create a local @ref AbstractShaper
subclass. It implements at least @ref doShape(), @ref doGlyphIdsInto(),
@ref doGlyphOffsetsAdvancesInto() and @ref doGlyphClustersInto(), and
potentially also (a subset of) @ref doSetScript(), @ref doScript(),
@ref doSetLanguage(),@ref doLanguage(), @ref doSetDirection() and
@ref doDirection(). The public API does most sanity checks on its own, see
documentation of particular `do*()` functions for more information about the
guarantees.
*/
class MAGNUM_TEXT_EXPORT AbstractShaper {
    public:
        /**
         * @brief Constructor
         * @param font  Font the shaper is originating from
         */
        explicit AbstractShaper(AbstractFont& font);

        /** @brief Copying is not allowed */
        AbstractShaper(AbstractShaper&) = delete;

        /** @brief Move constructor */
        AbstractShaper(AbstractShaper&&) noexcept;

        /** @brief Copying is not allowed */
        AbstractShaper& operator=(AbstractShaper&) = delete;

        /** @brief Move assignment */
        AbstractShaper& operator=(AbstractShaper&&) noexcept;

        virtual ~AbstractShaper();

        /** @brief Font the shaper is originating from */
        AbstractFont& font() { return _font; }
        const AbstractFont& font() const { return _font; } /**< @overload */

        /**
         * @brief Set text script
         *
         * The script is used for all following @ref shape() calls. If not
         * called at all or if explicitly set to @ref Script::Unspecified, the
         * @ref AbstractFont plugin may attempt to guess the script from the
         * input text. The actual script used for shaping (if any) is queryable
         * with @ref script() const after @ref shape() has been called.
         *
         * Returns @cpp true @ce if the plugin supports setting a
         * script and the script is supported, @cpp false @ce otherwise, in
         * which case the shaping falls back to a generic behavior. See
         * documentation of a particular plugin for more information.
         * @see @ref setLanguage(), @ref setDirection()
         */
        bool setScript(Script script);

        /**
         * @brief Set text language
         *
         * The language is expected to be a [BCP 47 language tag](https://en.wikipedia.org/wiki/IETF_language_tag),
         * either just the base tag such as @cpp "en" @ce or @cpp "cs" @ce
         * alone, or further differentiating with a region subtag like for
         * example @cpp "en-US" @ce vs @cpp "en-GB" @ce.
         *
         * The language is used for all following @ref shape() calls. If not
         * called at all or if explicitly set to an empty string, the
         * @ref AbstractFont plugin may attempt to guess the language from the
         * input text or the execution environment, such as current locale. The
         * actual language used for shaping (if any) is queryable with
         * @ref language() const after @ref shape() has been called.
         *
         * Returns @cpp true @ce if the plugin supports setting a language and
         * the language is supported, @cpp false @ce otherwise, in which case
         * the shaping falls back to a generic behavior. See documentation of a
         * particular plugin for more information.
         * @see @ref setScript(), @ref setDirection()
         */
        bool setLanguage(Containers::StringView language);

        /**
         * @brief Set direction the text is meant to be shaped in
         *
         * The direction is used for all following @ref shape() calls. If not
         * called at all or if explicitly set to
         * @ref ShapeDirection::Unspecified, the @ref AbstractFont plugin may
         * attempt to guess the direction from the input text. The actual
         * direction used for shaping (if any) is queryable with
         * @ref direction() const after @ref shape() has been called.
         *
         * Returns @cpp true @ce if the plugin supports setting a language and
         * the language is supported, @cpp false @ce otherwise, in which case
         * the shaping falls back to a generic behavior. See documentation of a
         * particular font plugin for more information.
         * @see @ref setScript(), @ref setLanguage()
         */
        bool setDirection(ShapeDirection direction);

        /**
         * @brief Shape a text
         * @param text      Text in UTF-8
         * @param features  Typographic features to apply for the whole text or
         *      its subranges
         *
         * Expects that both @p begin and all @ref FeatureRange::begin() are
         * contained within @p text, and that @p end and all
         * @ref FeatureRange::end() are either contained within @p text or have
         * a value of @cpp 0xffffffffu @ce. Returns the number of shaped glyphs
         * (which is also subsequently available through @ref glyphCount() const)
         * and updates the @ref script() const, @ref language() const and
         * @ref direction() const values.
         *
         * Whether @p features are used depends on a particular
         * @ref AbstractFont plugin implementation and the font file itself as
         * well --- for example, a plugin may enable @ref Feature::Kerning
         * by default but the font may not even have appropriate tables for it
         * included, in which case no kerning is performed. See documentation
         * of a particular font plugin for more information.
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        UnsignedInt shape(Containers::StringView text, Containers::ArrayView<const FeatureRange> features = {});
        #else
        /* To not have to include ArrayView */
        UnsignedInt shape(Containers::StringView text, Containers::ArrayView<const FeatureRange> features);
        UnsignedInt shape(Containers::StringView text);
        #endif

        /** @overload */
        UnsignedInt shape(Containers::StringView text, std::initializer_list<FeatureRange> features);

        /**
         * @brief Shape a slice of text
         * @param text      Text in UTF-8
         * @param begin     Beginning byte in the input text
         * @param end       (One byte after) the end byte in the input text
         * @param features  Typographic features to apply for the whole text or
         *      its subranges
         *
         * A variant of @ref shape(Containers::StringView, Containers::ArrayView<const FeatureRange>)
         * to be used when passing pieces of larger text with different
         * shapers, for example when the script, language or direction changes
         * in each piece or when the pieces are using a different font
         * entirely. Compared to passing just the actually shaped slice of
         * @p text this allows the implementation to perform shaping aware of
         * surrounding context, such as picking correct glyphs for beginning,
         * middle or end of a word or a paragraph.
         * @see @ref Text-AbstractShaper-multiple
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        UnsignedInt shape(Containers::StringView text, UnsignedInt begin, UnsignedInt end, Containers::ArrayView<const FeatureRange> features = {});
        #else
        /* To not have to include ArrayView */
        UnsignedInt shape(Containers::StringView text, UnsignedInt begin, UnsignedInt end, Containers::ArrayView<const FeatureRange> features);
        UnsignedInt shape(Containers::StringView text, UnsignedInt begin, UnsignedInt end);
        #endif

        /** @overload */
        UnsignedInt shape(Containers::StringView text, UnsignedInt begin, UnsignedInt end, std::initializer_list<FeatureRange> features);

        /**
         * @brief Count of glyphs produced by the last @ref shape() call
         *
         * If the last @ref shape() call failed or it hasn't been called yet,
         * returns @cpp 0 @ce.
         */
        UnsignedInt glyphCount() const { return _glyphCount; }

        /**
         * @brief Script used for the last @ref shape() call
         *
         * May return @ref Script::Unspecified if @ref shape() hasn't been
         * called yet or if the @ref AbstractFont doesn't implement any
         * script-specific behavior.
         * @see @ref setScript(), @ref language(), @ref direction()
         */
        Script script() const;

        /**
         * @brief Language used for the last @ref shape() call
         *
         * May return an empty string if @ref shape() hasn't been called yet or
         * if the @ref AbstractFont doesn't implement any language-specific
         * behavior.
         *
         * The returned view is generally neither
         * @relativeref{Corrade,Containers::StringViewFlag::Global} nor
         * @relativeref{Corrade,Containers::StringViewFlag::NullTerminated} and
         * is only guaranteed to stay valid until the next @ref setLanguage()
         * or @ref shape() call. Particular @ref AbstractFont implementations
         * may give better guarantees, see their documentation for more
         * information.
         * @see @ref setLanguage(), @ref script(), @ref direction()
         */
        Containers::StringView language() const;

        /**
         * @brief Shape direction used for the last @ref shape() call
         *
         * May return @ref ShapeDirection::Unspecified if @ref shape() hasn't
         * been called yet or if the @ref AbstractFont doesn't implement any
         * script-specific behavior.
         *
         * The direction affects properties of advances coming from
         * @ref glyphOffsetsAdvancesInto() and cluster IDs coming from
         * @ref glyphClustersInto(), see particular @ref ShapeDirection values
         * for more information.
         * @see @ref setDirection(), @ref script(), @ref language()
         */
        ShapeDirection direction() const;

        /**
         * @brief Retrieve glyph IDs
         * @param[out] ids          Where to put glyph IDs
         *
         * The @p ids view is expected to have a size of @ref glyphCount().
         * After calling this function, the @p ids are commonly looked up in or
         * inserted into an @ref AbstractGlyphCache. Offsets and advances
         * corresponding to the IDs can be retrieved with
         * @ref glyphOffsetsAdvancesInto().
         */
        void glyphIdsInto(const Containers::StridedArrayView1D<UnsignedInt>& ids) const;

        /**
         * @brief Retrieve glyph offsets and advances
         * @param[out] offsets      Where to put glyph offsets
         * @param[out] advances     Where to put glyph advances
         *
         * The @p offsets and @p advances views are expected to have a size of
         * @ref glyphCount(). The @p offsets specify where to put the glyph
         * relative to current cursor (which is then further offset for the
         * particular glyph rectangle returned from the glyph cache) and
         * @p advances specify in which direction to move the cursor for the
         * next glyph. For @ref direction() being
         * @ref ShapeDirection::LeftToRight or
         * @relativeref{ShapeDirection,RightToLeft} Y components of @p advances
         * are @cpp 0.0f @ce, for @relativeref{ShapeDirection,TopToBottom} or
         * @relativeref{ShapeDirection,BottomToTop} X components of @p advances
         * are @cpp 0.0f @ce. Glyph IDs corresponding to the offsets and
         * advances can be retrieved with @ref glyphIdsInto().
         * @see @ref direction()
         */
        void glyphOffsetsAdvancesInto(const Containers::StridedArrayView1D<Vector2>& offsets, const Containers::StridedArrayView1D<Vector2>& advances) const;

        /**
         * @brief Retrieve glyph cluster IDs
         * @param[out] clusters     Where to put glyph clusters
         *
         * The @p clusters view is expected to have a size of
         * @ref glyphCount(). The cluster IDs are used to map shaped glyphs
         * back to the text passed to @ref shape(). By default, the cluster ID
         * sequence is monotonically non-decreasing or non-increasing based on
         * @ref direction(), with the IDs being byte positions in the original
         * text corresponding to particular glyphs:
         *
         * -    For plain ASCII text and with the shaper not performing any
         *      ligature substitutions, glyph decomposition or reordering, the
         *      @ref glyphCount() will be equal to the shaped text byte count,
         *      with clusters being a sequence of @cpp {0, 1, 2, 3, …} @ce, or
         *      additionally shifted if the @p begin parameter passed to
         *      @ref shape() was non-zero.
         * -    For UTF-8 text and the shaper again not performing any ligature
         *      substitutions, glyph decomposition or reordering, the sequence
         *      will point to start bytes of multi-byte UTF-8 characters. For
         *      example @cpp {0, 1, 3, 4, 7, …} @ce, assuming a two-byte UTF-8
         *      character at byte @cpp 1 @ce and a three-byte character at byte
         *      @cpp 4 @ce. Similar output will be if the shaper performs a
         *      ligature substitution (such as `fi` at byte @cpp 1 @ce and
         *      `ffl` at byte @cpp 4 @ce both turned into a ligature in an
         *      otherwise ASCII input).
         * -    If the shaper performs glyph decomposition, one character in
         *      the input may end up being multiple glyphs. For example
         *      @cpp {0, 1, 1, 3, 4, …} @ce, assuming a two-byte UTF-8
         *      character `ě` at byte @cpp 1 @ce being decomposed into two
         *      glyphs, `e` and `ˇ`.
         * -    If the shaper performs glyph reordering, the cluster ID will
         *      become the whole range of bytes in which the reordering
         *      happened, to preserve monotonicity. For example
         *      @cpp {0, 1, 1, 1, 1, 4, …} @ce, assuming glyphs corresponding
         *      to bytes @cpp 1 @ce to @cpp 3 @ce were swapped during shaping.
         *
         * Certain shaper implementations may offer behavior where the
         * monotonicity is not preserved or the mapping is not to the original
         * input bytes. Such behavior is however never the default, always
         * opt-in. See documentation of particular font plugins for more
         * information.
         * @see @ref Feature::StandardLigatures,
         *      @ref Feature::GlyphCompositionDecomposition,
         *      @ref HarfBuzzFont
         */
        void glyphClustersInto(const Containers::StridedArrayView1D<UnsignedInt>& clusters) const;

    private:
        /**
         * @brief Implemenation for @ref setScript()
         *
         * Default implementation does nothing and returns @cpp false @ce.
         */
        virtual bool doSetScript(Script script);

        /**
         * @brief Implemenation for @ref setLanguage()
         *
         * Default implementation does nothing and returns @cpp false @ce.
         */
        virtual bool doSetLanguage(Containers::StringView language);

        /**
         * @brief Implemenation for @ref setDirection()
         *
         * Default implementation does nothing and returns @cpp false @ce.
         */
        virtual bool doSetDirection(ShapeDirection direction);

        /**
         * @brief Implemenation for @ref shape()
         *
         * The @p begin as well as all @ref FeatureRange::begin() values
         * are guaranteed to be within @p text, @p end as well as all
         * @ref FeatureRange::end() values are guaranteed to be either within
         * @p text or have a value of @cpp 0xffffffffu @ce.
         */
        virtual UnsignedInt doShape(Containers::StringView text, UnsignedInt begin, UnsignedInt end, Containers::ArrayView<const FeatureRange> features) = 0;

        /**
         * @brief Implemenation for @ref script()
         *
         * Default implementation returns @ref Script::Unspecified.
         */
        virtual Script doScript() const;

        /**
         * @brief Implemenation for @ref language()
         *
         * Default implementation returns an empty string.
         */
        virtual Containers::StringView doLanguage() const;

        /**
         * @brief Implemenation for @ref direction()
         *
         * Default implementation returns @ref ShapeDirection::Unspecified.
         */
        virtual ShapeDirection doDirection() const;

        /**
         * @brief Implemenation for @ref glyphIdsInto()
         *
         * The @p ids are guaranteed to have a size of @ref glyphCount().
         * Called only if @ref glyphCount() is not @cpp 0 @ce.
         */
        virtual void doGlyphIdsInto(const Containers::StridedArrayView1D<UnsignedInt>& ids) const = 0;

        /**
         * @brief Implemenation for @ref glyphOffsetsAdvancesInto()
         *
         * The @p offsets and @p advances are guaranteed to have a size of
         * @ref glyphCount(). Called only if @ref glyphCount() is not
         * @cpp 0 @ce.
         */
        virtual void doGlyphOffsetsAdvancesInto(const Containers::StridedArrayView1D<Vector2>& offsets, const Containers::StridedArrayView1D<Vector2>& advances) const = 0;

        /**
         * @brief Implemenation for @ref glyphClustersInto()
         *
         * The @p clusters are guaranteed to have a size of @ref glyphCount().
         * Called only if @ref glyphCount() is not @cpp 0 @ce.
         */
        virtual void doGlyphClustersInto(const Containers::StridedArrayView1D<UnsignedInt>& clusters) const = 0;

        Containers::Reference<AbstractFont> _font;
        UnsignedInt _glyphCount;
};

}}

#endif
