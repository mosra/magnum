#ifndef Magnum_Text_AbstractFont_h
#define Magnum_Text_AbstractFont_h
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
 * @brief Class @ref Magnum::Text::AbstractFont, enum @ref Magnum::Text::FontFeature, enum set @ref Magnum::Text::FontFeatures
 */

#include <initializer_list>
#include <Corrade/PluginManager/AbstractPlugin.h>
#include <Corrade/Utility/StlForwardString.h> /** @todo remove once file callbacks are std::string-free */

#include "Magnum/Magnum.h"
#include "Magnum/Text/Text.h"
#include "Magnum/Text/visibility.h"

#ifdef MAGNUM_BUILD_DEPRECATED
/* Used by deprecated AbstractLayouter */
#include <Corrade/Containers/Array.h>
/* For APIs that used to take or return a std::string */
#include <Corrade/Containers/StringStl.h>
/* renderGlyph() used to return a std::pair */
#include <Corrade/Containers/PairStl.h>
#endif

namespace Magnum { namespace Text {

/**
@brief Features supported by a font implementation
@m_since{2020,06}

@see @ref FontFeatures, @ref AbstractFont::features()
*/
enum class FontFeature: UnsignedByte {
    /** Opening fonts from raw data using @ref AbstractFont::openData() */
    OpenData = 1 << 0,

    /**
     * Specifying callbacks for loading additional files referenced from the
     * main file using @ref AbstractFont::setFileCallback(). If the font
     * doesn't expose this feature, the format is either single-file or loading
     * via callbacks is not supported.
     *
     * See @ref Text-AbstractFont-usage-callbacks and particular font plugin
     * documentation for more information.
     * @m_since{2019,10}
     */
    FileCallback = 1 << 1,

    /**
     * The font contains a prepared glyph cache.
     * @see @ref AbstractFont::fillGlyphCache(),
     *      @ref AbstractFont::createGlyphCache()
     */
    PreparedGlyphCache = 1 << 2,

    /* Glyph names are not exposed as a feature because even though the
       implementation may support these, a particular font file may not, and
       it'd give a false impression. */
};

/**
@brief Set of features supported by a font implementation
@m_since{2020,06}

@see @ref AbstractFont::features()
*/
typedef Containers::EnumSet<FontFeature> FontFeatures;

CORRADE_ENUMSET_OPERATORS(FontFeatures)

/** @debugoperatorenum{FontFeature} */
MAGNUM_TEXT_EXPORT Debug& operator<<(Debug& debug, FontFeature value);

/** @debugoperatorenum{FontFeatures} */
MAGNUM_TEXT_EXPORT Debug& operator<<(Debug& debug, FontFeatures value);

/**
@brief Base for font plugins

Provides interface for opening font files, filling a glyph cache with
rasterized glyphs and shaping a Unicode text into a sequence of glyph IDs and
their positions.

@section Text-AbstractFont-usage Usage

Fonts are most commonly implemented as plugins, which means the concrete
font implementation is loaded and instantiated through a @relativeref{Corrade,PluginManager::Manager}. A font is opened using either
@ref openFile() or @ref openData() together with specifying the size at which
glyphs will be rasterized. Then it stays open until the font is destroyed,
@ref close() or another font is opened.

In the following example a font is loaded from the filesystem using the
@ref StbTrueTypeFont plugin, prerendering desired glyphs into a
@ref GlyphCacheGL, completely with all error handling ---  depending on the
font file, font plugin implementation, font size and cache size, it may happen
that the characters won't all fit, which should be checked by the application:

@snippet Text-gl.cpp AbstractFont-usage

See @ref plugins for more information about general plugin usage and the list
of @m_class{m-doc} <a href="#derived-classes">derived classes</a> for available
font plugins. See @ref AbstractGlyphCache for more information about glyph
caches, @ref Renderer for high-level text rendering, and @ref AbstractShaper
for low-level access to the font text shaping functionality.

@section Text-AbstractFont-font-size Font size

Font libraries specify font size in *points*, where 1 pt = ~1.333 px at 96 DPI,
so in the above snippet a 12 pt font corresponds to 16 px on a 96 DPI display.
The font size corresponds to the height of the [EM quad](https://en.wikipedia.org/wiki/Em_(typography))
which is defined as the distance between ascent and descent.

Upon opening the font, the size in points is exposed in @ref size(). Derived
properties are specified *in pixels* in @ref lineHeight(), @ref ascent() and
@ref descent().

The font size used when opening the font affects how large the glyphs will be
when rasterized into the glyph cache. Actual text rendering with @ref Renderer
then uses its own font size, and the actual size that's visible on the screen
is then additionally depending on the actual projection used when drawing the
rendered mesh.

This decoupling of font sizes is useful for example in case of
@ref DistanceFieldGlyphCacheGL, where a single prerendered glyph size can be
used to render arbitrarily large font sizes without becoming blurry or jaggy.
When not using a distance field glyph cache, it's usually desirable to have the
font size and the actual rendered size match. See
@ref Text-Renderer-usage-font-size "the Renderer documentation" for further
information about picking font sizes.

@section Text-AbstractFont-glyph-cache Glyph cache filling options

Besides filling the cache with glyphs corresponding to individual characters in
a UTF-8 string, it's possible to directly specify glyph IDs with
@ref fillGlyphCache(AbstractGlyphCache&, const Containers::StridedArrayView1D<const UnsignedInt>&).
That's useful for example with ligatures like *fl* or *ff* that can get used by
a particular font implementation instead of individual letters, and which thus
don't have a clear mapping to a Unicode codepoint. You can use some font
introspection utility to retrieve either directly the glyph IDs, or get just
glyph names and match them with IDs using @ref glyphForName() at runtime. The
following snippet prerenders a few of the named ligatures present in the
[Source Sans](https://github.com/adobe-fonts/source-sans) font that this
website uses. Again explicitly checking that the operation succeeds, this time
with a @ref CORRADE_INTERNAL_ASSERT_OUTPUT() macro:

@snippet Text.cpp AbstractFont-glyph-cache-by-id

Subsequent calls to @ref fillGlyphCache() add to already existing glyphs,
allowing for incremental filling based on which glyphs are needed by actual
text. Such as by passing the output of @ref AbstractShaper::glyphIdsInto()
through @ref AbstractGlyphCache::glyphIdsInto() and adding all glyphs which it
didn't find. Note that, however, the glyph cache packing is the most efficient
when all glyphs are added at once.

Finally, it's possible to use this overload to populate the glyph cache with
all glyphs in the font:

@snippet Text.cpp AbstractFont-glyph-cache-all

@section Text-AbstractFont-usage-callbacks Loading data from memory, using file callbacks

Besides loading data directly from the filesystem using @ref openFile() like
shown above, it's possible to use @ref openData() to import data from memory.
Note that the particular importer implementation must support
@ref FontFeature::OpenData for this method to work.

@snippet Text.cpp AbstractFont-usage-data

Some font formats consist of more than one file and in that case you may want
to intercept those references and load them in a custom way as well. For font
plugins that advertise support for this with @ref FontFeature::FileCallback
this is done by specifying a file loading callback using @ref setFileCallback().
The callback gets a filename, @ref InputFileCallbackPolicy and an user
pointer as parameters; returns a non-owning view on the loaded data or a
@relativeref{Corrade,Containers::NullOpt} to indicate the file loading failed.
For example, loading a memory-mapped font could look like below. Note that the
file loading callback affects @ref openFile() as well --- you don't have to
load the top-level file manually and pass it to @ref openData(), any font
plugin supporting the callback feature handles that correctly.

@snippet Text.cpp AbstractFont-usage-callbacks

For importers that don't support @ref FontFeature::FileCallback directly, the
base @ref openFile() implementation will use the file callback to pass the
loaded data through to @ref openData(), in case the importer supports at least
@ref FontFeature::OpenData. If the importer supports neither
@ref FontFeature::FileCallback nor @ref FontFeature::OpenData,
@ref setFileCallback() doesn't allow the callbacks to be set.

The input file callback signature is the same for @ref Text::AbstractFont,
@ref ShaderTools::AbstractConverter and @ref Trade::AbstractImporter to allow
code reuse.

@section Text-AbstractFont-data-dependency Data dependency

The @ref AbstractShaper instances returned from @ref createShaper() have a code
and data dependency on the dynamic plugin module --- since their implementation
is in the plugin module itself, the plugin can't be unloaded until the returned
instance is destroyed.

@section Text-AbstractFont-subclassing Subclassing

The plugin needs to implement the @ref doFeatures(), @ref doClose(),
@ref doCreateShaper() functions, either @ref doCreateGlyphCache() or
@ref doFillGlyphCache() and one or more of `doOpen*()` functions. See also
@ref AbstractShaper for more information.

In order to support @ref FontFeature::FileCallback, the font needs to properly
use the callbacks to both load the top-level file in @ref doOpenFile() and also
load any external files when needed. The @ref doOpenFile() can delegate back
into the base implementation, but it should remember at least the base file
path to pass correct paths to subsequent file callbacks. The
@ref doSetFileCallback() can be overridden in case it's desired to respond to
file loading callback setup, but doesn't have to be.

You don't need to do most of the redundant sanity checks, these things are
checked by the implementation:

-   The @ref doOpenData() and @ref doOpenFile() functions are called after the
    previous file was closed, @ref doClose() is called only if there is any
    file opened.
-   The @ref doOpenData() is called only if @ref FontFeature::OpenData is
    supported.
-   The @ref doSetFileCallback() function is called only if
    @ref FontFeature::FileCallback is supported and there is no file opened.
-   All `do*()` implementations working on opened file are called only if
    there is any file opened.
*/
class MAGNUM_TEXT_EXPORT AbstractFont: public PluginManager::AbstractPlugin {
    public:
        #ifdef MAGNUM_BUILD_DEPRECATED
        /** @brief @copybrief FontFeature
         * @m_deprecated_since{2020,06} Use @ref FontFeature instead.
         */
        typedef CORRADE_DEPRECATED("use FontFeature instead") FontFeature Feature;

        /** @brief @copybrief FontFeatures
         * @m_deprecated_since{2020,06} Use @ref FontFeatures instead.
         */
        typedef CORRADE_DEPRECATED("use FontFeatures instead") FontFeatures Features;
        #endif

        /**
         * @brief Plugin interface
         *
         * @snippet Magnum/Text/AbstractFont.h interface
         *
         * @see @ref MAGNUM_TEXT_ABSTRACTFONT_PLUGIN_INTERFACE
         */
        static Containers::StringView pluginInterface();

        #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
        /**
         * @brief Plugin search paths
         *
         * Looks into `magnum/fonts/` or `magnum-d/fonts/` next to the dynamic
         * @ref Trade library, next to the executable and elsewhere according
         * to the rules documented in
         * @ref Corrade::PluginManager::implicitPluginSearchPaths(). The search
         * directory can be also hardcoded using the `MAGNUM_PLUGINS_DIR` CMake
         * variables, see @ref building for more information.
         *
         * Not defined on platforms without
         *      @ref CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT "dynamic plugin support".
         */
        static Containers::Array<Containers::String> pluginSearchPaths();
        #endif

        /** @brief Default constructor */
        explicit AbstractFont();

        /** @brief Plugin manager constructor */
        /* The plugin name is passed as a const& to make it possible for people
           to implement plugins without even having to include the StringView
           header. */
        explicit AbstractFont(PluginManager::AbstractManager& manager, const Containers::StringView& plugin);

        /** @brief Features supported by this font */
        FontFeatures features() const { return doFeatures(); }

        /**
         * @brief File opening callback function
         * @m_since{2019,10}
         *
         * @see @ref Text-AbstractFont-usage-callbacks
         */
        auto fileCallback() const -> Containers::Optional<Containers::ArrayView<const char>>(*)(const std::string&, InputFileCallbackPolicy, void*) { return _fileCallback; }

        /**
         * @brief File opening callback user data
         * @m_since{2019,10}
         *
         * @see @ref Text-AbstractFont-usage-callbacks
         */
        void* fileCallbackUserData() const { return _fileCallbackUserData; }

        /**
         * @brief Set file opening callback
         * @m_since{2019,10}
         *
         * In case the font plugin supports @ref FontFeature::FileCallback,
         * files opened through @ref openFile() will be loaded through the
         * provided callback. Besides that, all external files referenced by
         * the top-level file will be loaded through the callback function as
         * well, usually on demand. The callback function gets a filename,
         * @ref InputFileCallbackPolicy and the @p userData pointer as input
         * and returns a non-owning view on the loaded data as output or a
         * @ref Corrade::Containers::NullOpt if loading failed --- because
         * empty files might also be valid in some circumstances, @cpp nullptr @ce
         * can't be used to indicate a failure.
         *
         * In case the font plugin doesn't support @ref FontFeature::FileCallback
         * but supports at least @ref FontFeature::OpenData, a file opened
         * through @ref openFile() will be internally loaded through the
         * provided callback and then passed to @ref openData(). First the file
         * is loaded with @ref InputFileCallbackPolicy::LoadTemporary passed to
         * the callback, then the returned memory view is passed to
         * @ref openData() (sidestepping the potential @ref openFile()
         * implementation of that particular font plugin) and after that the
         * callback is called again with @ref InputFileCallbackPolicy::Close
         * because the semantics of @ref openData() don't require the data to
         * be alive after. In case you need a different behavior, use
         * @ref openData() directly.
         *
         * In case @p callback is @cpp nullptr @ce, the current callback (if
         * any) is reset. This function expects that the font plugin supports
         * either @ref FontFeature::FileCallback or @ref FontFeature::OpenData.
         * If a font plugin supports neither, callbacks can't be used.
         *
         * It's expected that this function is called *before* a file is
         * opened. It's also expected that the loaded data are kept in scope
         * for as long as the font plugin needs them, based on the value of
         * @ref InputFileCallbackPolicy. Documentation of particular importers
         * provides more information about the expected callback behavior.
         *
         * Following is an example of setting up a file loading callback for
         * fetching compiled-in resources from @ref Corrade::Utility::Resource.
         * See the overload below for a more convenient type-safe way to pass
         * the user data pointer.
         *
         * @snippet Text.cpp AbstractFont-setFileCallback
         *
         * @see @ref Text-AbstractFont-usage-callbacks
         */
        void setFileCallback(Containers::Optional<Containers::ArrayView<const char>>(*callback)(const std::string&, InputFileCallbackPolicy, void*), void* userData = nullptr);

        /**
         * @brief Set file opening callback
         * @m_since{2019,10}
         *
         * Equivalent to calling the above with a lambda wrapper that casts
         * @cpp void* @ce back to @cpp T* @ce and dereferences it in order to
         * pass it to @p callback. Example usage --- this reuses an existing
         * @ref Corrade::Utility::Resource instance to avoid a potentially slow
         * resource group lookup every time:
         *
         * @snippet Text.cpp AbstractFont-setFileCallback-template
         *
         * @see @ref Text-AbstractFont-usage-callbacks
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        template<class T> void setFileCallback(Containers::Optional<Containers::ArrayView<const char>>(*callback)(const std::string&, InputFileCallbackPolicy, T&), T& userData);
        #else
        /* Otherwise the user would be forced to use the + operator to convert
           a lambda to a function pointer and (besides being weird and
           annoying) it's also not portable because it doesn't work on MSVC
           2015 and older versions of MSVC 2017. */
        template<class Callback, class T> void setFileCallback(Callback callback, T& userData);
        #endif

        /** @brief Whether any file is opened */
        bool isOpened() const { return doIsOpened(); }

        /**
         * @brief Open raw data
         * @param data          File data
         * @param size          Font size in points
         *
         * Closes previous file, if it was opened, and tries to open given
         * raw data. Available only if @ref FontFeature::OpenData is supported.
         * On failure prints a message to @relativeref{Magnum,Error} and
         * returns @cpp false @ce.
         * @see @ref features(), @ref openFile()
         */
        bool openData(Containers::ArrayView<const void> data, Float size);

        /**
         * @brief Open a file
         * @param filename      Font file
         * @param size          Size to open the font in, in points
         *
         * Closes previous file, if it was opened, and tries to open given
         * file. On failure prints a message to @relativeref{Magnum,Error} and
         * returns @cpp false @ce. If file loading callbacks are set via
         * @ref setFileCallback() and @ref FontFeature::OpenData is supported,
         * this function uses the callback to load the file and passes the
         * memory view to @ref openData() instead. See @ref setFileCallback()
         * for more information.
         */
        bool openFile(Containers::StringView filename, Float size);

        /**
         * @brief Close currently opened file
         *
         * On certain implementations an explicit call to this function when
         * the file is no longer needed but the font instance is going to be
         * reused further may result in freed memory. This call is also done
         * automatically when the font instance gets destructed or when another
         * file is opened. If no file is opened, does nothing. After this
         * function is called, @ref isOpened() is guaranteed to return
         * @cpp false @ce.
         */
        void close();

        /**
         * @brief Font size in points
         *
         * Font size is defined as the distance between @ref ascent() and
         * @ref descent(), thus the value of @cpp (ascent - descent)*0.75f @ce
         * (i.e., converted from pixels) is equal to @ref size().
         * @see @ref lineHeight(), @ref glyphAdvance()
         */
        Float size() const;

        /**
         * @brief Font ascent in pixels
         *
         * Distance from the baseline to the top, a *positive* value. Font size
         * is defined as the distance between @ref ascent() and @ref descent(),
         * thus the value of @cpp (ascent - descent)*0.75f @ce (i.e., converted
         * to points) is equal to @ref size(). Expects that a font is opened.
         * @see @ref lineHeight(), @ref glyphAdvance()
         */
        Float ascent() const;

        /**
         * @brief Font descent in pixels
         *
         * Distance from the baseline to the bottom, a *negative* value. Font
         * size is defined as the distance between @ref ascent() and
         * @ref descent(), thus the value of @cpp (ascent - descent)*0.75f @ce
         * (i.e., converted to points) is equal to @ref size(). Expects that a
         * font is opened.
         * @see @ref lineHeight(), @ref glyphAdvance()
         */
        Float descent() const;

        /**
         * @brief Line height in pixels
         *
         * Distance between baselines in consecutive text lines that
         * corresponds to @ref ascent() and @ref descent(), a *positive* value.
         * Expects that a font is opened.
         * @see @ref size(), @ref glyphAdvance()
         */
        Float lineHeight() const;

        /**
         * @brief Total count of glyphs in the font
         * @m_since_latest
         *
         * Expects that a font is opened.
         * @note This function is meant to be used only for font observations
         *      and conversions. In performance-critical code the
         *      @ref fillGlyphCache() and @ref layout() functions should be
         *      used instead.
         */
        UnsignedInt glyphCount() const;

        /**
         * @brief Glyph ID for given character
         *
         * A convenience wrapper around @ref glyphIdsInto() for querying a
         * glyph ID for a single character. Expects that a font is opened.
         */
        UnsignedInt glyphId(char32_t character);

        /**
         * @brief Glyph IDs for given characters
         * @param[in]  characters   Input characters
         * @param[out] glyphs       Output glyph IDs
         * @m_since_latest
         *
         * Expects that a font is opened and that the @p characters and
         * @p glyphs views have the same size. The glyph IDs are all guaranteed
         * to be less than @ref glyphCount().
         */
        void glyphIdsInto(const Containers::StridedArrayView1D<const char32_t>& characters, const Containers::StridedArrayView1D<UnsignedInt>& glyphs);

        /**
         * @brief Glyph name
         * @m_since_latest
         *
         * Returns a name of the glyph in the font file, if present and
         * supported by the implementation, or an empty string. Expects that a
         * font is opened and @p glyph is less than @ref glyphCount().
         * @see @ref glyphForName()
         */
        Containers::String glyphName(UnsignedInt glyph);

        /**
         * @brief Glyph for given name
         * @m_since_latest
         *
         * If the implementation supports querying glyphs by name and the name
         * exists, returns a corresponding glyph ID, otherwise returns
         * @cpp 0 @ce for an invalid glyph. Note that certain named glyphs can
         * also map to glyph @cpp 0 @ce, in particular @cpp ".notdef" @ce in
         * TTF and OTF fonts, the way to distinguish them is to ask for name of
         * the zero glyph and compare. The returned index is guaranteed to be
         * less than @ref glyphCount().
         */
        UnsignedInt glyphForName(Containers::StringView name);

        /**
         * @brief Glyph size in pixels
         * @param glyph     Glyph ID
         * @m_since_latest
         *
         * Size of the glyph image in pixels when rasterized. Some
         * implementations may return fractional values, in which case
         * @ref Math::ceil() should be used to get the actual integer pixel
         * size. Expects that a font is opened and @p glyph is less than
         * @ref glyphCount().
         * @note This function is meant to be used only for font observations
         *      and conversions. In performance-critical code the
         *      @ref fillGlyphCache() and @ref layout() functions should be
         *      used instead.
         * @see @ref glyphId(), @ref size()
         */
        Vector2 glyphSize(UnsignedInt glyph);

        /**
         * @brief Glyph advance in pixels
         * @param glyph     Glyph ID
         *
         * Distance the cursor for the next glyph that follows @p glyph.
         * Doesn't consider kerning or any other advanced shaping features.
         * Expects that a font is opened and @p glyph is less than
         * @ref glyphCount().
         * @note This function is meant to be used only for font observations
         *      and conversions. In performance-critical code the @ref layout()
         *      function should be used instead.
         * @see @ref glyphId(), @ref size()
         */
        Vector2 glyphAdvance(UnsignedInt glyph);

        /**
         * @brief Fill glyph cache with given glyph IDs
         * @param cache         Glyph cache instance
         * @param glyphs        Glyph IDs to render
         * @m_since_latest
         *
         * Fills the cache with given glyph IDs. Fonts having
         * @ref FontFeature::PreparedGlyphCache do not support partial glyph
         * cache filling, use @ref createGlyphCache() instead. Expects that a
         * font is opened and @p glyphs are all unique and less than
         * @ref glyphCount().
         *
         * On success returns @cpp true @ce. On failure, for example if the
         * @p cache doesn't have expected format or the @p glyphs can't
         * fit, prints a message to @relativeref{Magnum,Error} and returns
         * @cpp false @ce.
         */
        bool fillGlyphCache(AbstractGlyphCache& cache, const Containers::StridedArrayView1D<const UnsignedInt>& glyphs);
        /** @overload */
        bool fillGlyphCache(AbstractGlyphCache& cache, std::initializer_list<UnsignedInt> glyphs);

        /**
         * @brief Fill glyph cache with given character set
         * @param cache         Glyph cache instance
         * @param characters    UTF-8 characters to render
         *
         * Converts @p characters to a list of Unicode codepoints, gets glyph
         * IDs for them using @ref glyphIdsInto(), removes duplicates, adds the
         * glyph @cpp 0 @ce if the font is not in @p cache already, and
         * delegates to @ref fillGlyphCache(AbstractGlyphCache&, const Containers::StridedArrayView1D<const UnsignedInt>&).
         */
        bool fillGlyphCache(AbstractGlyphCache& cache, Containers::StringView characters);

        /**
         * @brief Create glyph cache
         *
         * Configures and fills glyph cache with the contents of whole font.
         * Available only if @ref FontFeature::PreparedGlyphCache is supported.
         * Other fonts support only partial glyph cache filling, see
         * @ref fillGlyphCache(). Expects that a font is opened.
         *
         * On success returns an instance of a newly created glyph cache. On
         * failure, for example if a file containing glyph cache data cannot be
         * open, prints a message to @relativeref{Magnum,Error} and returns
         * @cpp nullptr @ce.
         */
        Containers::Pointer<AbstractGlyphCache> createGlyphCache();

        /**
         * @brief Create an instance of this font shaper implementation
         * @m_since_latest
         *
         * The returned class can be used to shape text using this font. See
         * its documentation for more information. Note that the font has to
         * stay in scope for as long as any @ref AbstractShaper instances
         * originating from the font exist. Expects that a font is opened. The
         * returned instance is never @cpp nullptr @ce.
         */
        Containers::Pointer<AbstractShaper> createShaper();

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Layout the text using font's own layouter
         * @param cache     Glyph cache
         * @param size      Size to layout the text in, in pooints
         * @param text      Text to layout
         *
         * @m_deprecated_since_latest Use @ref createShaper() and the
         *      @ref AbstractShaper class instead.
         *
         * Note that the layouters support rendering of single-line text only.
         * See @ref AbstractRenderer class for more advanced text layouting.
         * Expects that a font is opened.
         * @see @ref fillGlyphCache(), @ref createGlyphCache()
         */
        CORRADE_DEPRECATED("use createShaper() instead") Containers::Pointer<AbstractLayouter> layout(const AbstractGlyphCache& cache, Float size, Containers::StringView text);
        #endif

    protected:
        /**
         * @brief Font properties
         *
         * Returned from @ref doOpenFile(), @ref doOpenData().
         */
        struct Properties {
            #if defined(CORRADE_TARGET_GCC) && !defined(CORRADE_TARGET_CLANG) && __GNUC__ < 5
            /* Otherwise GCC 4.8 loudly complains about missing initializers */
            constexpr /*implicit*/ Properties() noexcept: size{}, ascent{}, descent{}, lineHeight{}, glyphCount{} {}
            constexpr /*implicit*/ Properties(Float size, Float ascent, Float descent, Float lineHeight, UnsignedInt glyphCount) noexcept: size{size}, ascent{ascent}, descent{descent}, lineHeight{lineHeight}, glyphCount{glyphCount} {}
            #endif

            /**
             * Font size in points
             * @see @ref size()
             */
            Float size;

            /**
             * Font ascent in pixels
             * @see @ref ascent()
             */
            Float ascent;

            /**
             * Font descent in pixels
             * @see @ref descent()
             */
            Float descent;

            /**
             * Line height in pixels
             * @see @ref lineHeight()
             */
            Float lineHeight;

            /**
             * Total count of glyphs in the font
             * @see @ref glyphCount()
             * @m_since_latest
             */
            UnsignedInt glyphCount;
        };

    protected:
        /**
         * @brief Implementation for @ref openFile()
         *
         * If @ref doIsOpened() returns @cpp true @ce after calling this
         * function, it's assumed that opening was successful and the
         * @ref Properties are expected to contain valid values. If
         * @ref doIsOpened() returns @cpp false @ce, the returned values are
         * ignored. If @ref FontFeature::OpenData is supported, default
         * implementation opens the file and calls @ref doOpenData() with its
         * contents. It is allowed to call this function from your
         * @ref doOpenFile() implementation --- in particular, this
         * implementation will also correctly handle callbacks set through
         * @ref setFileCallback().
         *
         * This function is not called when file callbacks are set through
         * @ref setFileCallback() and @ref FontFeature::FileCallback is not
         * supported --- instead, file is loaded though the callback and data
         * passed through to @ref doOpenData().
         */
        virtual Properties doOpenFile(Containers::StringView filename, Float size);

    private:
        /** @brief Implementation for @ref features() */
        virtual FontFeatures doFeatures() const = 0;

        /**
         * @brief Implementation for @ref setFileCallback()
         *
         * Useful when the font plugin needs to modify some internal state on
         * callback setup. Default implementation does nothing and this
         * function doesn't need to be implemented --- the callback function
         * and user data pointer are available through @ref fileCallback() and
         * @ref fileCallbackUserData().
         */
        virtual void doSetFileCallback(Containers::Optional<Containers::ArrayView<const char>>(*callback)(const std::string&, InputFileCallbackPolicy, void*), void* userData);

        /** @brief Implementation for @ref isOpened() */
        virtual bool doIsOpened() const = 0;

        /**
         * @brief Implementation for @ref openData()
         *
         * If @ref doIsOpened() returns @cpp true @ce after calling this
         * function, it's assumed that opening was successful and the
         * @ref Properties are expected to contain valid values. If
         * @ref doIsOpened() returns @cpp false @ce, the returned values are
         * ignored.
         */
        virtual Properties doOpenData(Containers::ArrayView<const char> data, Float size);

        /** @brief Implementation for @ref close() */
        virtual void doClose() = 0;

        /**
         * @brief Implementation for @ref glyphIdsInto()
         * @m_since_latest
         *
         * The implementation is expected to return all @p glyphs smaller than
         * @ref glyphCount().
         */
        virtual void doGlyphIdsInto(const Containers::StridedArrayView1D<const char32_t>& characters, const Containers::StridedArrayView1D<UnsignedInt>& glyphs) = 0;

        /**
         * @brief Implementation for @ref glyphName()
         * @m_since_latest
         *
         * Default implementation returns an empty string.
         */
        virtual Containers::String doGlyphName(UnsignedInt glyph);

        /**
         * @brief Implementation for @ref glyphForName()
         * @m_since_latest
         *
         * The implementation is expected to return a value smaller than
         * @ref glyphCount(). Default implementation returns @cpp 0 @ce.
         */
        virtual UnsignedInt doGlyphForName(Containers::StringView name);

        /**
         * @brief Implementation for @ref glyphSize()
         * @m_since_latest
         */
        virtual Vector2 doGlyphSize(UnsignedInt glyph) = 0;

        /** @brief Implementation for @ref glyphAdvance() */
        virtual Vector2 doGlyphAdvance(UnsignedInt glyph) = 0;

        /**
         * @brief Implementation for @ref fillGlyphCache()
         *
         * The @p glyphs are guaranteed to be unique and all less than
         * @ref glyphCount().
         */
        virtual bool doFillGlyphCache(AbstractGlyphCache& cache, const Containers::StridedArrayView1D<const UnsignedInt>& glyphs);

        /** @brief Implementation for @ref createGlyphCache() */
        virtual Containers::Pointer<AbstractGlyphCache> doCreateGlyphCache();

        /**
         * @brief Implementation for @ref createShaper()
         * @m_since_latest
         *
         * This function is only called if the font is opened. The
         * implementation is not allowed to return @cpp nullptr @ce.
         */
        virtual Containers::Pointer<AbstractShaper> doCreateShaper() = 0;

        Containers::Optional<Containers::ArrayView<const char>>(*_fileCallback)(const std::string&, InputFileCallbackPolicy, void*){};
        void* _fileCallbackUserData{};

        /* Used by the templated version only */
        struct FileCallbackTemplate {
            void(*callback)();
            const void* userData;
        /* GCC 4.8 complains loudly about missing initializers otherwise */
        } _fileCallbackTemplate{nullptr, nullptr};

        Float _size{}, _ascent{}, _descent{}, _lineHeight{};
        UnsignedInt _glyphCount{};
};

#ifdef MAGNUM_BUILD_DEPRECATED
/**
@brief Base for text layouters

@m_deprecated_since_latest Use @ref AbstractShaper returned from
    @ref AbstractFont::createShaper() instead.
*/
class MAGNUM_TEXT_EXPORT
    /* Clang-cl 12, included in MSVC 2019, chokes on two __declspec attributes
       being specified here. Clang-cl 16, included in MSVC 2022, doesn't, but I
       couldn't find any relevant changelog entry in the in-between versions to
       know which version this was actually fixed in, except for these:
        https://releases.llvm.org/16.0.0/tools/clang/docs/ReleaseNotes.html#bug-fixes-to-attribute-support
       Also probably not limited to just clang-cl, but also regular Clang
       within MSVC, and a MinGW Clang as well, so taking the safe path and
       excluding all Clang versions below 16 on Windows. */
    #if !defined(CORRADE_TARGET_WINDOWS) || !defined(CORRADE_TARGET_CLANG) || __clang_major__ >= 16
    CORRADE_DEPRECATED("use AbstractShaper instead")
    #endif
AbstractLayouter {
    public:
        /** @brief Copying is not allowed */
        CORRADE_IGNORE_DEPRECATED_PUSH /* GCC <=7 warns due to the argument */
        AbstractLayouter(const AbstractLayouter&) = delete;
        CORRADE_IGNORE_DEPRECATED_POP

        /** @brief Moving is not allowed */
        CORRADE_IGNORE_DEPRECATED_PUSH /* GCC <=7 warns due to the argument */
        AbstractLayouter(AbstractLayouter&&) = delete;
        CORRADE_IGNORE_DEPRECATED_POP

        virtual ~AbstractLayouter();

        /** @brief Copying is not allowed */
        CORRADE_IGNORE_DEPRECATED_PUSH /* GCC <=7 warns due to the argument */
        AbstractLayouter& operator=(const AbstractLayouter&) = delete;
        CORRADE_IGNORE_DEPRECATED_POP

        /** @brief Moving is not allowed */
        CORRADE_IGNORE_DEPRECATED_PUSH /* GCC <=7 warns due to the argument */
        AbstractLayouter& operator=(const AbstractLayouter&&) = delete;
        CORRADE_IGNORE_DEPRECATED_POP

        /** @brief Count of glyphs in the laid out text */
        UnsignedInt glyphCount() const { return _glyphs.size(); }

        /**
         * @brief Render a glyph
         * @param[in]     i                 Glyph index
         * @param[in,out] cursorPosition    Cursor position
         * @param[in,out] rectangle         Bounding rectangle
         *
         * The function returns a pair of quad position and texture
         * coordinates, advances @p cursorPosition to next character and
         * updates @p rectangle with extended bounds. Expects that @p i is less
         * than @ref glyphCount().
         */
        Containers::Pair<Range2D, Range2D> renderGlyph(UnsignedInt i, Vector2& cursorPosition, Range2D& rectangle);

    #ifdef DOXYGEN_GENERATING_OUTPUT
    private:
    #else
    public:
    #endif
        /* Can't just friend AbstractFont as this is actually called from
           Pointer internals */
        explicit AbstractLayouter(Containers::Array<Containers::Triple<Range2D, Range2D, Vector2>>&& glyphs);

    private:
        Containers::Array<Containers::Triple<Range2D, Range2D, Vector2>> _glyphs;
};
#endif

/**
@brief Font plugin interface
@m_since_latest

Same string as returned by
@relativeref{Magnum::Text,AbstractFont::pluginInterface()}, meant to be used
inside @ref CORRADE_PLUGIN_REGISTER() to avoid having to update the interface
string by hand every time the version gets bumped:

@snippet Text.cpp MAGNUM_TEXT_ABSTRACTFONT_PLUGIN_INTERFACE

The interface string version gets increased on every ABI break to prevent
silent crashes and memory corruption. Plugins built against the previous
version will then fail to load, a subsequent rebuild will make them pick up the
updated interface string.
*/
/* Silly indentation to make the string appear in pluginInterface() docs */
#define MAGNUM_TEXT_ABSTRACTFONT_PLUGIN_INTERFACE /* [interface] */ \
"cz.mosra.magnum.Text.AbstractFont/0.3.7"
/* [interface] */

#ifndef DOXYGEN_GENERATING_OUTPUT
template<class Callback, class T> void AbstractFont::setFileCallback(Callback callback, T& userData) {
    /* Don't try to wrap a null function pointer. Need to cast first because
       MSVC (even 2017) can't apply ! to a lambda. Ugh. */
    const auto callbackPtr = static_cast<Containers::Optional<Containers::ArrayView<const char>>(*)(const std::string&, InputFileCallbackPolicy, T&)>(callback);
    if(!callbackPtr) return setFileCallback(nullptr);

    _fileCallbackTemplate = { reinterpret_cast<void(*)()>(callbackPtr), static_cast<const void*>(&userData) };
    setFileCallback([](const std::string& filename, const InputFileCallbackPolicy flags, void* const userData) {
        auto& s = *reinterpret_cast<FileCallbackTemplate*>(userData);
        return reinterpret_cast<Containers::Optional<Containers::ArrayView<const char>>(*)(const std::string&, InputFileCallbackPolicy, T&)>(s.callback)(filename, flags, *static_cast<T*>(const_cast<void*>(s.userData)));
    }, &_fileCallbackTemplate);
}
#endif

}}

#endif
