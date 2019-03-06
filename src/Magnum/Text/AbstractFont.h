#ifndef Magnum_Text_AbstractFont_h
#define Magnum_Text_AbstractFont_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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
 * @brief Class @ref Magnum::Text::AbstractFont, @ref Magnum::Text::AbstractLayouter
 */

#include <string>
#include <vector>
#include <tuple>
#include <Corrade/PluginManager/AbstractPlugin.h>

#include "Magnum/Magnum.h"
#include "Magnum/GL/Texture.h"
#include "Magnum/Text/Text.h"
#include "Magnum/Text/visibility.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Containers/PointerStl.h>
#endif

namespace Magnum { namespace Text {

/**
@brief Base for font plugins

Provides interface for opening fonts, filling glyph cache and layouting the
glyphs. See @ref plugins for more information and `*Font` classes in @ref Text
namespace for available font plugins.

@section Text-AbstractFont-usage Usage

First step is to open the font using @ref openData(), @ref openSingleData() or
@ref openFile(). Next step is to prerender all the glyphs which will be used in
text rendering later, see @ref GlyphCache for more information. See
@ref Renderer for information about text rendering.

@section Text-AbstractFont-subclassing Subclassing

Plugin implements @ref doFeatures(), @ref doClose(), @ref doLayout(), either
@ref doCreateGlyphCache() or @ref doFillGlyphCache() and one or more of
`doOpen*()` functions. See also @ref AbstractLayouter for more information.

You don't need to do most of the redundant sanity checks, these things are
checked by the implementation:

-   Functions @ref doOpenData(), @ref doOpenSingleData() and @ref doOpenFile()
    are called after the previous file was closed, function @ref doClose() is
    called only if there is any file opened.
-   Functions @ref doOpenData() and @ref doOpenSingleData() are called only if
    @ref Feature::OpenData is supported.
-   All `do*()` implementations working on opened file are called only if
    there is any file opened.
*/
class MAGNUM_TEXT_EXPORT AbstractFont: public PluginManager::AbstractPlugin {
    public:
        /**
         * @brief Features supported by this importer
         *
         * @see @ref Features, @ref features()
         */
        enum class Feature: UnsignedByte {
            /** Opening fonts from raw data using @ref openData() */
            OpenData = 1 << 0,

            /**
             * The format is multi-file, thus @ref openSingleData() convenience
             * function cannot be used.
             */
            MultiFile = 1 << 1,

            /**
             * The font contains prepared glyph cache.
             *
             * @see @ref fillGlyphCache(), @ref createGlyphCache()
             */
            PreparedGlyphCache = 1 << 2
        };

        /** @brief Set of features supported by this importer */
        typedef Containers::EnumSet<Feature> Features;

        /**
         * @brief Plugin interface
         *
         * @code{.cpp}
         * "cz.mosra.magnum.Text.AbstractFont/0.3"
         * @endcode
         */
        static std::string pluginInterface();

        #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
        /**
         * @brief Plugin search paths
         *
         * First looks in `magnum/fonts/` or `magnum-d/fonts/` next to the
         * executable and as a fallback in `magnum/fonts/` or `magnum-d/fonts/`
         * in the runtime install location (`lib[64]/` on Unix-like systems,
         * `bin/` on Windows). The system-wide plugin search directory is
         * configurable using the `MAGNUM_PLUGINS_DIR` CMake variables, see
         * @ref building for more information.
         *
         * Not defined on platforms without
         *      @ref CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT "dynamic plugin support".
         */
        static std::vector<std::string> pluginSearchPaths();
        #endif

        /** @brief Default constructor */
        explicit AbstractFont();

        /** @brief Plugin manager constructor */
        explicit AbstractFont(PluginManager::AbstractManager& manager, const std::string& plugin);

        /** @brief Features supported by this font */
        Features features() const { return doFeatures(); }

        /** @brief Whether any file is opened */
        bool isOpened() const { return doIsOpened(); }

        /**
         * @brief Open font from raw data
         * @param data          Pairs of filename and file data
         * @param size          Font size
         *
         * Closes previous file, if it was opened, and tries to open given
         * file. Available only if @ref Feature::OpenData is supported. Returns
         * @cpp true @ce on success, @cpp false @ce otherwise.
         */
        bool openData(const std::vector<std::pair<std::string, Containers::ArrayView<const char>>>& data, Float size);

        /**
         * @brief Open font from single data
         * @param data          File data
         * @param size          Font size
         *
         * Closes previous file, if it was opened, and tries to open given
         * file. Available only if @ref Feature::OpenData is supported and the
         * plugin doesn't have @ref Feature::MultiFile. Returns @cpp true @ce
         * on success, @cpp false @ce otherwise.
         */
        bool openSingleData(Containers::ArrayView<const char> data, Float size);

        /**
         * @brief Open font from file
         * @param filename      Font file
         * @param size          Font size
         *
         * Closes previous file, if it was opened, and tries to open given
         * file. If the plugin has @ref Feature::MultiFile, the function will
         * use additional files in given path, all sharing common basename
         * derived from @p filename. Returns @cpp true @ce on success,
         * @cpp false @ce otherwise.
         */
        bool openFile(const std::string& filename, Float size);

        /** @brief Close font */
        void close();

        /**
         * @brief Font size
         *
         * Returns scale in which @ref lineHeight(), @ref ascent(),
         * @ref descent() and @ref glyphAdvance() is returned.
         */
        Float size() const { return _size; }

        /**
         * @brief Font ascent
         *
         * Distance from baseline to top, scaled to font size. Positive value.
         * @see @ref size(), @ref descent(), @ref lineHeight()
         */
        Float ascent() const { return _ascent; }

        /**
         * @brief Font descent
         *
         * Distance from baseline to bottom, scalled to font size. Negative
         * value.
         * @see @ref size(), @ref ascent(), @ref lineHeight()
         */
        Float descent() const { return _descent; }

        /**
         * @brief Line height
         *
         * Returns line height scaled to font size.
         * @see @ref size(), @ref ascent(), @ref descent()
         */
        Float lineHeight() const { return _lineHeight; }

        /**
         * @brief Glyph ID for given character
         *
         * @note This function is meant to be used only for font observations
         *      and conversions. In performance-critical code the @ref layout()
         *      function should be used instead.
         */
        UnsignedInt glyphId(char32_t character);

        /**
         * @brief Glyph advance
         * @param glyph     Glyph ID
         *
         * Returns glyph advance scaled to font size.
         * @note This function is meant to be used only for font observations
         *      and conversions. In performance-critical code the @ref layout()
         *      function should be used instead.
         * @see @ref glyphId(), @ref size()
         */
        Vector2 glyphAdvance(UnsignedInt glyph);

        /**
         * @brief Fill glyph cache with given character set
         * @param cache         Glyph cache instance
         * @param characters    UTF-8 characters to render
         *
         * Fills the cache with given characters. Fonts having
         * @ref Feature::PreparedGlyphCache do not support partial glyph cache
         * filling, use @ref createGlyphCache() instead.
         */
        void fillGlyphCache(AbstractGlyphCache& cache, const std::string& characters);

        /**
         * @brief Create glyph cache
         *
         * Configures and fills glyph cache with the contents of whole font.
         * Available only if @ref Feature::PreparedGlyphCache is supported.
         * Other fonts support only partial glyph cache filling, see
         * @ref fillGlyphCache().
         */
        Containers::Pointer<AbstractGlyphCache> createGlyphCache();

        /**
         * @brief Layout the text using font's own layouter
         * @param cache     Glyph cache
         * @param size      Font size
         * @param text      Text to layout
         *
         * Note that the layouters support rendering of single-line text only.
         * See @ref Renderer class for more advanced text layouting.
         * @see @ref fillGlyphCache(), @ref createGlyphCache()
         */
        Containers::Pointer<AbstractLayouter> layout(const AbstractGlyphCache& cache, Float size, const std::string& text);

    protected:
        /**
         * @brief Font metrics
         *
         * @see @ref doOpenFile(), @ref doOpenData(), @ref doOpenSingleData()
         */
        struct Metrics {
            /**
             * Font size
             * @see @ref size()
             */
            Float size;

            /**
             * Font ascent
             * @see @ref ascent()
             */
            Float ascent;

            /**
             * Font descent
             * @see @ref descent()
             */
            Float descent;

            /**
             * Line height
             * @see @ref lineHeight()
             */
            Float lineHeight;
        };

    private:
        /** @brief Implementation for @ref features() */
        virtual Features doFeatures() const = 0;

        /** @brief Implementation for @ref isOpened() */
        virtual bool doIsOpened() const = 0;

        /**
         * @brief Implementation for @ref openData()
         *
         * Return metrics of opened font on successful opening, zeros
         * otherwise. If the plugin doesn't have @ref Feature::MultiFile,
         * default implementation calls @ref doOpenSingleData().
         */
        virtual Metrics doOpenData(const std::vector<std::pair<std::string, Containers::ArrayView<const char>>>& data, Float size);

        /**
         * @brief Implementation for @ref openSingleData()
         *
         * Return metrics of opened font on successful opening, zeros
         * otherwise.
         */
        virtual Metrics doOpenSingleData(Containers::ArrayView<const char> data, Float size);

        /**
         * @brief Implementation for @ref openFile()
         *
         * Return metrics of opened font on successful opening, zeros
         * otherwise. If @ref Feature::OpenData is supported and the plugin
         * doesn't have @ref Feature::MultiFile, default implementation opens
         * the file and calls @ref doOpenSingleData() with its contents.
         */
        virtual Metrics doOpenFile(const std::string& filename, Float size);

        /** @brief Implementation for @ref close() */
        virtual void doClose() = 0;

        /** @brief Implementation for @ref glyphId() */
        virtual UnsignedInt doGlyphId(char32_t character) = 0;

        /** @brief Implementation for @ref glyphAdvance() */
        virtual Vector2 doGlyphAdvance(UnsignedInt glyph) = 0;

        /**
         * @brief Implementation for @ref fillGlyphCache()
         *
         * The string is converted from UTF-8 to UTF-32, unique characters are
         * *not* removed.
         */
        virtual void doFillGlyphCache(AbstractGlyphCache& cache, const std::u32string& characters);

        /** @brief Implementation for @ref createGlyphCache() */
        virtual Containers::Pointer<AbstractGlyphCache> doCreateGlyphCache();

        /** @brief Implementation for @ref layout() */
        virtual Containers::Pointer<AbstractLayouter> doLayout(const AbstractGlyphCache& cache, Float size, const std::string& text) = 0;

    #ifdef DOXYGEN_GENERATING_OUTPUT
    private:
    #endif
        Float _size{}, _ascent{}, _descent{}, _lineHeight{};
};

CORRADE_ENUMSET_OPERATORS(AbstractFont::Features)

/**
@brief Base for text layouters

Returned by @ref AbstractFont::layout().

@section Text-AbstractLayouter-subclassing Subclassing

Plugin creates private subclass (no need to expose it to end users) and
implements @ref doRenderGlyph(). Bounds checking on @p i is done automatically
in the wrapping @ref renderGlyph() function.
*/
class MAGNUM_TEXT_EXPORT AbstractLayouter {
    public:
        /** @brief Copying is not allowed */
        AbstractLayouter(const AbstractLayouter&) = delete;

        /** @brief Moving is not allowed */
        AbstractLayouter(AbstractLayouter&&) = delete;

        virtual ~AbstractLayouter();

        /** @brief Copying is not allowed */
        AbstractLayouter& operator=(const AbstractLayouter&) = delete;

        /** @brief Moving is not allowed */
        AbstractLayouter& operator=(const AbstractLayouter&&) = delete;

        /** @brief Count of glyphs in laid out text */
        UnsignedInt glyphCount() const { return _glyphCount; }

        /**
         * @brief Render glyph
         * @param i                 Glyph index
         * @param cursorPosition    Cursor position
         * @param rectangle         Bounding rectangle
         *
         * The function returns pair of quad position and texture coordinates,
         * advances @p cursorPosition to next character and updates @p rectangle
         * with extended bounds.
         */
        std::pair<Range2D, Range2D> renderGlyph(UnsignedInt i, Vector2& cursorPosition, Range2D& rectangle);

    protected:
        /**
         * @brief Constructor
         * @param glyphCount    Count of glyphs in laid out text
         */
        explicit AbstractLayouter(UnsignedInt glyphCount);

    #ifdef DOXYGEN_GENERATING_OUTPUT
    protected:
    #else
    private:
    #endif
        /**
         * @brief Implementation for @ref renderGlyph()
         * @param i                 Glyph index
         *
         * Return quad position (relative to current cursor position), texture
         * coordinates and advance to next glyph.
         */
        virtual std::tuple<Range2D, Range2D, Vector2> doRenderGlyph(UnsignedInt i) = 0;

    #ifdef DOXYGEN_GENERATING_OUTPUT
    private:
    #endif
        UnsignedInt _glyphCount;
};

}}

#endif
