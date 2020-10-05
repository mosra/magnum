#ifndef Magnum_Text_AbstractFont_h
#define Magnum_Text_AbstractFont_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class @ref Magnum::Text::AbstractFont, @ref Magnum::Text::AbstractLayouter, enum @ref Magnum::Text::FontFeature, enum set @ref Magnum::Text::FontFeatures
 */

#include <string>
#include <vector>
#include <tuple>
#include <Corrade/PluginManager/AbstractPlugin.h>

#include "Magnum/Magnum.h"
#include "Magnum/Text/Text.h"
#include "Magnum/Text/visibility.h"

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

    #ifdef MAGNUM_BUILD_DEPRECATED
    /**
     * The format is multi-file, thus @ref AbstractFont::openSingleData()
     * convenience function cannot be used.
     * @m_deprecated_since{2019,10} Obsolete, use file callbacks
     *      instead.
     */
    MultiFile CORRADE_DEPRECATED_ENUM("obsolete, use file callbacks instead") = FileCallback,
    #endif

    /**
     * The font contains prepared glyph cache.
     *
     * @see @ref AbstractFont::fillGlyphCache(),
     *      @ref AbstractFont::createGlyphCache()
     */
    PreparedGlyphCache = 1 << 2
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

Provides interface for opening fonts, filling glyph cache and layouting the
glyphs.

@section Text-AbstractFont-usage Usage

Fonts are most commonly implemented as plugins. For example, loading a font
from the filesystem using the @ref StbTrueTypeFont plugin and prerendering all
needed glyphs can be done like this, completely with all error handling:

@snippet MagnumText.cpp AbstractFont-usage

See @ref plugins for more information about general plugin usage and `*Font`
classes in the @ref Text namespace for available font plugins. See
@ref GlyphCache for more information about glyph caches and @ref Renderer for
information about actual text rendering.

@subsection Text-AbstractFont-usage-callbacks Loading data from memory, using file callbacks

Besides loading data directly from the filesystem using @ref openFile() like
shown above, it's possible to use @ref openData() to import data from memory.
Note that the particular importer implementation must support
@ref FontFeature::OpenData for this method to work.

Some font formats consist of more than one file and in that case you may want
to intercept those references and load them in a custom way as well. For font
plugins that advertise support for this with @ref FontFeature::FileCallback
this is done by specifying a file loading callback using @ref setFileCallback().
The callback gets a filename, @ref InputFileCallbackPolicy and an user
pointer as parameters; returns a non-owning view on the loaded data or a
@ref Corrade::Containers::NullOpt "Containers::NullOpt" to indicate the file
loading failed. For example, loading a memory-mapped font could look like
below. Note that the file loading callback affects @ref openFile() as
well --- you don't have to load the top-level file manually and pass it to
@ref openData(), any font plugin supporting the callback feature handles that
correctly.

@snippet MagnumText.cpp AbstractFont-usage-callbacks

For importers that don't support @ref FontFeature::FileCallback directly, the
base @ref openFile() implementation will use the file callback to pass the
loaded data through to @ref openData(), in case the importer supports at least
@ref FontFeature::OpenData. If the importer supports neither
@ref FontFeature::FileCallback nor @ref FontFeature::OpenData,
@ref setFileCallback() doesn't allow the callbacks to be set.

The input file callback signature is the same for @ref Text::AbstractFont,
@ref ShaderTools::AbstractConverter and @ref Trade::AbstractImporter to allow
code reuse.

@section Text-AbstractFont-subclassing Subclassing

The plugin implements @ref doFeatures(), @ref doClose(), @ref doLayout(),
either @ref doCreateGlyphCache() or @ref doFillGlyphCache() and one or more of
`doOpen*()` functions. See also @ref AbstractLayouter for more information.

You don't need to do most of the redundant sanity checks, these things are
checked by the implementation:

-   Functions @ref doOpenData() and @ref doOpenFile() are called after the
    previous file was closed, function @ref doClose() is called only if there
    is any file opened.
-   Function @ref doOpenData() is called only if @ref FontFeature::OpenData is
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
         * @snippet Magnum/Text/AbstractFont.cpp interface
         */
        static std::string pluginInterface();

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
        static std::vector<std::string> pluginSearchPaths();
        #endif

        /** @brief Default constructor */
        explicit AbstractFont();

        /** @brief Plugin manager constructor */
        explicit AbstractFont(PluginManager::AbstractManager& manager, const std::string& plugin);

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
         * @snippet MagnumText.cpp AbstractFont-setFileCallback
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
         * @snippet MagnumText.cpp AbstractFont-setFileCallback-template
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
         * @brief Open font from raw data
         * @param data          File data
         * @param size          Font size
         *
         * Closes previous file, if it was opened, and tries to open given
         * file. Available only if @ref FontFeature::OpenData is supported.
         * Returns @cpp true @ce on success, @cpp false @ce otherwise.
         * @see @ref features(), @ref openFile()
         */
        bool openData(Containers::ArrayView<const char> data, Float size);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /** @brief @copybrief openData(Containers::ArrayView<const char>, Float)
         * @m_deprecated_since{2019,10} Use @ref openFile() with
         *      @ref setFileCallback() for opening multi-file fonts instead.
         */
        CORRADE_DEPRECATED("use openFile() with setFileCallback() for opening multi-file fonts instead") bool openData(const std::vector<std::pair<std::string, Containers::ArrayView<const char>>>& data, Float size);

        /** @brief @copybrief openData(Containers::ArrayView<const char>, Float)
         * @m_deprecated_since{2019,10} Use @ref openData(Containers::ArrayView<const char>, Float)
         *      instead.
         */
        CORRADE_DEPRECATED("use openData(Containers::ArrayView<const char>, Float) instead") bool openSingleData(Containers::ArrayView<const char> data, Float size);
        #endif

        /**
         * @brief Open font from file
         * @param filename      Font file
         * @param size          Font size
         *
         * Closes previous file, if it was opened, and tries to open given
         * file. Returns @cpp true @ce on success, @cpp false @ce otherwise.
         */
        bool openFile(const std::string& filename, Float size);

        /** @brief Close font */
        void close();

        /**
         * @brief Font size
         *
         * Returns scale in which @ref lineHeight(), @ref ascent(),
         * @ref descent() and @ref glyphAdvance() is returned. Expects that a
         * font is opened.
         */
        Float size() const;

        /**
         * @brief Font ascent
         *
         * Distance from baseline to top, scaled to font size. Positive value.
         * Expects that a font is opened.
         * @see @ref size(), @ref descent(), @ref lineHeight()
         */
        Float ascent() const;

        /**
         * @brief Font descent
         *
         * Distance from baseline to bottom, scalled to font size. Negative
         * value. Expects that a font is opened.
         * @see @ref size(), @ref ascent(), @ref lineHeight()
         */
        Float descent() const;

        /**
         * @brief Line height
         *
         * Returns line height scaled to font size. Expects that a font is
         * opened.
         * @see @ref size(), @ref ascent(), @ref descent()
         */
        Float lineHeight() const;

        /**
         * @brief Glyph ID for given character
         *
         * Expects that a font is opened.
         * @note This function is meant to be used only for font observations
         *      and conversions. In performance-critical code the @ref layout()
         *      function should be used instead.
         */
        UnsignedInt glyphId(char32_t character);

        /**
         * @brief Glyph advance
         * @param glyph     Glyph ID
         *
         * Returns glyph advance scaled to font size. Expects that a font is
         * opened.
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
         * @ref FontFeature::PreparedGlyphCache do not support partial glyph
         * cache filling, use @ref createGlyphCache() instead. Expects that a
         * font is opened.
         */
        void fillGlyphCache(AbstractGlyphCache& cache, const std::string& characters);

        /**
         * @brief Create glyph cache
         *
         * Configures and fills glyph cache with the contents of whole font.
         * Available only if @ref FontFeature::PreparedGlyphCache is supported.
         * Other fonts support only partial glyph cache filling, see
         * @ref fillGlyphCache(). Expects that a font is opened.
         */
        Containers::Pointer<AbstractGlyphCache> createGlyphCache();

        /**
         * @brief Layout the text using font's own layouter
         * @param cache     Glyph cache
         * @param size      Font size
         * @param text      Text to layout
         *
         * Note that the layouters support rendering of single-line text only.
         * See @ref Renderer class for more advanced text layouting. Expects
         * that a font is opened.
         * @see @ref fillGlyphCache(), @ref createGlyphCache()
         */
        Containers::Pointer<AbstractLayouter> layout(const AbstractGlyphCache& cache, Float size, const std::string& text);

    protected:
        /**
         * @brief Font metrics
         *
         * @see @ref doOpenFile(), @ref doOpenData()
         */
        struct Metrics {
            #ifndef DOXYGEN_GENERATING_OUTPUT
            /* Otherwise GCC 4.8 loudly complains about missing initializers */
            constexpr /*implicit*/ Metrics() noexcept: size{}, ascent{}, descent{}, lineHeight{} {}
            constexpr /*implicit*/ Metrics(Float size, Float ascent, Float descent, Float lineHeight) noexcept: size{size}, ascent{ascent}, descent{descent}, lineHeight{lineHeight} {}
            #endif

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

    protected:
        /**
         * @brief Implementation for @ref openFile()
         *
         * Return metrics of opened font on successful opening, zeros
         * otherwise. If @ref FontFeature::OpenData is supported, default
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
        virtual Metrics doOpenFile(const std::string& filename, Float size);

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
         * Return metrics of opened font on successful opening, zeros
         * otherwise.
         */
        virtual Metrics doOpenData(Containers::ArrayView<const char> data, Float size);

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

        Containers::Optional<Containers::ArrayView<const char>>(*_fileCallback)(const std::string&, InputFileCallbackPolicy, void*){};
        void* _fileCallbackUserData{};

        /* Used by the templated version only */
        struct FileCallbackTemplate {
            void(*callback)();
            const void* userData;
        /* GCC 4.8 complains loudly about missing initializers otherwise */
        } _fileCallbackTemplate{nullptr, nullptr};

        Float _size{}, _ascent{}, _descent{}, _lineHeight{};
};

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
