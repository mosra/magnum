#ifndef Magnum_Text_AbstractShaper_h
#define Magnum_Text_AbstractShaper_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class @ref Magnum::Text::AbstractShaper, @ref Magnum::Text::FeatureRange, enum @ref Magnum::Text::Direction
 * @m_since_latest
 */

#include <initializer_list>
#include <Corrade/Containers/Reference.h>

#include "Magnum/Magnum.h"
#include "Magnum/Text/Text.h"
#include "Magnum/Text/visibility.h"

namespace Magnum { namespace Text {

/**
@brief Direction a text is shaped in
@m_since_latest

@see @ref AbstractShaper::setDirection(), @ref AbstractShaper::direction()
*/
enum class Direction: UnsignedByte {
    /**
     * Unspecified. When set in @ref AbstractShaper::setDirection(), makes the
     * shaping rely on direction autodetection implemented in a particular
     * @ref AbstractFont plugin (if any). When returned from
     * @ref AbstractShaper::direction() after a successful
     * @ref AbstractShaper::shape() call, it means a particular
     * @ref AbstractFont plugin doesn't implement any script-specific behavior.
     */
    Unspecified = 0,

    /**
     * Left to right. When returned from @ref AbstractShaper::direction(),
     * the @p advances filled by @ref AbstractShaper::glyphsInto() are
     * guaranteed to have their Y components @cpp 0.0f @ce.
     */
    LeftToRight = 1,

    /**
     * Right to left. When returned from @ref AbstractShaper::direction(),
     * the @p advances filled by @ref AbstractShaper::glyphsInto() are
     * guaranteed to have their Y components @cpp 0.0f @ce.
     */
    RightToLeft,

    /**
     * Top to bottom. When returned from @ref AbstractShaper::direction(),
     * the @p advances filled by @ref AbstractShaper::glyphsInto() are
     * guaranteed to have their X components @cpp 0.0f @ce.
     */
    TopToBottom,

    /**
     * Bottom to top. When returned from @ref AbstractShaper::direction(),
     * the @p advances filled by @ref AbstractShaper::glyphsInto() are
     * guaranteed to have their X components @cpp 0.0f @ce.
     */
    BottomToTop
};

/** @debugoperatorenum{Direction} */
MAGNUM_TEXT_EXPORT Debug& operator<<(Debug& debug, Direction value);

/**
@brief OpenType feature for a text range
@m_since_latest

@see @ref AbstractShaper::shape()
*/
class FeatureRange {
    public:
        /**
         * @brief Constructor
         * @param feature   Feature to control
         * @param begin     Beginning byte in the input text
         * @param end       (One byte after) the end byte in the input text
         * @param value     Feature value to set
         */
        constexpr /*implicit*/ FeatureRange(Feature feature, UnsignedInt begin, UnsignedInt end, UnsignedInt value = true): _feature{feature}, _value{value}, _begin{begin}, _end{end} {}

        /**
         * @brief Construct for the whole text
         *
         * Equivalent to calling @ref FeatureRange(Feature, UnsignedInt, UnsignedInt, UnsignedInt)
         * with @p begin set to @cpp 0 @ce and @p end to @cpp 0xffffffffu @ce.
         */
        constexpr /*implicit*/ FeatureRange(Feature feature, UnsignedInt value = true): _feature{feature}, _value{value}, _begin{0}, _end{~UnsignedInt{}} {}

        /** @brief Feature to control */
        constexpr Feature feature() const { return _feature; }

        /**
         * @brief Whether to enable the feature
         *
         * Returns @cpp false @ce if @ref value() is @cpp 0 @ce, @cpp true @ce
         * otherwise.
         */
        constexpr bool isEnabled() const { return _value; }

        /** @brief Feature value to set */
        constexpr UnsignedInt value() const { return _value; }

        /**
         * @brief Beginning byte in the input text
         *
         * If the feature is set for the whole text, this is @cpp 0 @ce.
         */
        constexpr UnsignedInt begin() const { return _begin; }

        /**
         * @brief (One byte after) the end byte in the input text
         *
         * If the feature is set for the whole text, this is
         * @cpp 0xffffffffu @ce.
         */
        constexpr UnsignedInt end() const { return _end; }

    private:
        friend AbstractShaper;

        Feature _feature;
        UnsignedInt _value;
        UnsignedInt _begin;
        UnsignedInt _end;
};

/**
@brief Base for text shapers
@m_since_latest

Returned from @ref AbstractFont::createShaper(), provides an interface for
* *shaping* text with the @ref AbstractFont it originated from. Meant to be
(privately) subclassed by @ref AbstractFont plugin implementations.

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
with @ref glyphCount() and then getting the glyph data with @ref glyphsInto().
Glyph IDs can be then queried in (or inserted into) an @ref AbstractGlyphCache,
and the rendered glyphs positioned at @p offsets with the cursor moving by
@p advances is what makes up the final shaped text.

@snippet MagnumText.cpp AbstractShaper-shape

For best results, it's recommended to call (a subset of) @ref setScript(),
@ref setLanguage() and @ref setDirection() if at least some properties of the
input text are known, as shown above. Without these, the font plugin may
attempt to autodetect the properties, which might not always give a correct
result. If a particular font plugin doesn't implement given script, language or
direction or if it doesn't have any special handling for it, given function
will return @cpp false @ce. The @ref script() const, @ref language() const and
@ref direction() const can be used to inspect results of autodetection after
@ref shape() has been called. The set of supported scripts, languages and
directions and exact behavior for unsupported values is plugin-specific --- it
may for example choose a fallback instead, or it may ignore the setting
altogeter. See documentation of particular @ref AbstractFont subclasses for
more information.

@subsection Text-AbstractShaper-usage-features Enabling and disabling typographic features

In the above snippet, the whole text is shaped using typographic features that
are default in the font. For example, assuming the font would support small
capitals (and the particular @ref AbstractFont plugin would recognize and use
the feature), we could render the "world" part with small caps, resulting in
"Hello, ᴡᴏʀʟᴅ!".

@snippet MagnumText.cpp AbstractShaper-shape-features

Similarly, features can be enabled for the whole text by omitting the begin and
end parameters, or for example a feature that a particular @ref AbstractFont
plugin uses by default can be disabled by passing an explicit @cpp false @ce
argument. The range, if present, is always given in *bytes* of the UTF-8 input.
Capabilities of typographic features are rather broad, see the @ref Feature
enum and documentation linked from it for exhaustive information.

@subsection Text-AbstractShaper-usage-multiple Combining different shapers

Sometimes it's desirable to render different parts of the text with different
fonts, not just different features of the same font. A variation of the above
example could be rendering the "world" part with a bold font:

@snippet MagnumText.cpp AbstractShaper-shape-multiple

The resulting `glyphs` array is usable the same way as in the above case, with
a difference that the glyph IDs have to be looked up in an
@ref AbstractGlyphCache with a font ID corresponding to the range they're in.
Also note that the whole text is passed every time and a begin & end is
specified for it instead of passing just the slice alone. While possibly not
having any visible effect in this particular case, in general it allows the
shaper to make additional decisions based on surrounding context, for example
picking glyphs that are better connected to their neighbors in handwriting
fonts.

@subsection Text-AbstractShaper-usage-instances Managing multiple instances

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

@section Text-AbstractShaper-subclassing Subclassing

The @ref AbstractFont plugin is meant to create a local @ref AbstractShaper
subclass. It implements at least @ref doShape() and @ref doGlyphsInto(), and
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
         * @brief Set text direction
         *
         * The direction is used for all following @ref shape() calls. If not
         * called at all or if explicitly set to @ref Direction::Unspecified,
         * the @ref AbstractFont plugin may attempt to guess the direction from
         * the input text. The actual direction used for shaping (if any) is
         * queryable with @ref direction() const after @ref shape() has been
         * called.
         *
         * Returns @cpp true @ce if the plugin supports setting a language and
         * the language is supported, @cpp false @ce otherwise, in which case
         * the shaping falls back to a generic behavior. See documentation of a
         * particular font plugin for more information.
         * @see @ref setScript(), @ref setLanguage()
         */
        bool setDirection(Direction direction);

        /**
         * @brief Shape a text
         * @param text      Text in UTF-8
         * @param features  OpenType features to apply for the whole text or
         *      its subranges
         *
         * Expects that @p text is non-empty, that both @p begin and all
         * @ref FeatureRange::begin() are contained within @p text, and that
         * @p end and all @ref FeatureRange::end() are either contained within
         * @p text or have a value of @cpp 0xffffffffu @ce. On success returns
         * the number of shaped glyphs (which is also subsequently available
         * through @ref glyphCount() const) and updates the @ref script() const,
         * @ref language() const and @ref direction() const values.
         *
         * On failure, such as when the input is not valid UTF-8 or when
         * specified combination or script, language and direction is
         * unsupported, prints a message to @relativeref{Magnum,Error} and
         * returns @cpp 0 @ce.
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
         * @param features  OpenType features to apply for the whole text or
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
         * @see @ref Text-AbstractShaper-usage-multiple
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
         * If the last @ref shape() call failed, it hasn't been called yet or
         * if the @ref AbstractFont doesn't implement any script-specific
         * behavior, returns @ref Script::Unspecified.
         * @see @ref setScript(), @ref language(), @ref direction()
         */
        Script script() const;

        /**
         * @brief Language used for the last @ref shape() call
         *
         * If the last @ref shape() call failed, it hasn't been called yet or
         * if the @ref AbstractFont doesn't implement any language-specific
         * behavior, returns an empty string.
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
         * @brief Language used for the last @ref shape() call
         *
         * If the last @ref shape() call failed, it hasn't been called yet or
         * if the @ref AbstractFont doesn't implement any direction-specific
         * behavior, returns @ref Direction::Unspecified.
         * @see @ref setDirection(), @ref script(), @ref language()
         */
        Direction direction() const;

        /**
         * @brief Retrieve glyph information
         * @param[out] ids          Where to put glyph IDs
         * @param[out] offsets      Where to put glyph offsets
         * @param[out] advances     Where to put glyph advances
         *
         * The @p ids, @p offsets and @p advances views are all expected to
         * have a size of @ref glyphCount(). After calling this function, the
         * @p ids are commonly looked up in or inserted into an
         * @ref AbstractGlyphCache, @p offsets specify where to put the glyph
         * relative to current cursor (which is then further offset for the
         * particular glyph rectangle returned from the glyph cache) and
         * @p advances specify in which direction to move the cursor for the
         * next glyph. For @ref direction() being @ref Direction::LeftToRight
         * or @relativeref{Direction,RightToLeft} Y components of @p advances
         * are @cpp 0.0f @ce, for @relativeref{Direction,TopToBottom} or
         * @relativeref{Direction,BottomToTop} X components of @p advances are
         * @cpp 0.0f @ce.
         * @see @ref direction()
         */
        void glyphsInto(const Containers::StridedArrayView1D<UnsignedInt>& ids, const Containers::StridedArrayView1D<Vector2>& offsets, const Containers::StridedArrayView1D<Vector2>& advances) const;

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
        virtual bool doSetDirection(Direction direction);

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
         * Default implementation returns @ref Direction::Unspecified.
         */
        virtual Direction doDirection() const;

        /**
         * @brief Implemenation for @ref glyphsInto()
         *
         * The @p ids, @p offsets and @p advances are guaranteed to have a size
         * of @ref glyphCount(). Called only if @ref glyphCount() is not
         * @cpp 0 @ce.
         */
        virtual void doGlyphsInto(const Containers::StridedArrayView1D<UnsignedInt>& ids, const Containers::StridedArrayView1D<Vector2>& offsets, const Containers::StridedArrayView1D<Vector2>& advances) const = 0;

        Containers::Reference<AbstractFont> _font;
        UnsignedInt _glyphCount;
};

}}

#endif
