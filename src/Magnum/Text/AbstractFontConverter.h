#ifndef Magnum_Text_AbstractFontConverter_h
#define Magnum_Text_AbstractFontConverter_h
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
 * @brief Class @ref Magnum::Text::AbstractFontConverter, enum @ref Magnum::Text::FontConverterFeature, enum set @ref Magnum::Text::FontConverterFeatures
 */

#include <vector>
#include <Corrade/Containers/EnumSet.h>
#include <Corrade/PluginManager/AbstractPlugin.h>

#include "Magnum/Magnum.h"
#include "Magnum/Text/Text.h"
#include "Magnum/Text/visibility.h"

#ifdef CORRADE_TARGET_LIBCXX
#include <string> /* Libc++ doesn't have std::u32string in the fwdecl */
#else
#include <Corrade/Utility/StlForwardString.h>
#endif

namespace Magnum { namespace Text {

/**
@brief Features supported by a font converter
@m_since{2020,06}

@see @ref FontConverterFeatures, @ref AbstractFontConverter::features()
*/
enum class FontConverterFeature: UnsignedByte {
    /**
     * Exporting font using @ref AbstractFontConverter::exportFontToFile(),
     * @ref AbstractFontConverter::exportFontToData() or
     * @ref AbstractFontConverter::exportFontToSingleData()
     * @see @ref FontConverterFeature::ConvertData
     */
    ExportFont = 1 << 0,

    /**
     * Export glyph cache using
     * @ref AbstractFontConverter::exportGlyphCacheToFile(),
     * @ref AbstractFontConverter::exportGlyphCacheToData() or
     * @ref AbstractFontConverter::exportGlyphCacheToSingleData()
     * @see @ref FontConverterFeature::ConvertData
     */
    ExportGlyphCache = 1 << 1,

    /**
     * Import glyph cache using
     * @ref AbstractFontConverter::importGlyphCacheFromFile(),
     * @ref AbstractFontConverter::importGlyphCacheFromData() or
     * @ref AbstractFontConverter::importGlyphCacheFromSingleData()
     * @see @ref FontConverterFeature::ConvertData
     */
    ImportGlyphCache = 1 << 2,

    /**
     * Convert from/to data using
     * @ref AbstractFontConverter::exportFontToData(),
     * @ref AbstractFontConverter::exportFontToSingleData(),
     * @ref AbstractFontConverter::exportGlyphCacheToData(),
     * @ref AbstractFontConverter::exportGlyphCacheToSingleData(),
     * @ref AbstractFontConverter::importGlyphCacheFromData() or
     * @ref AbstractFontConverter::importGlyphCacheFromSingleData()
     */
    ConvertData = 1 << 4,

    /**
     * The format is multi-file, thus
     * @ref AbstractFontConverter::exportFontToSingleData(),
     * @ref AbstractFontConverter::exportGlyphCacheToSingleData() and
     * @ref AbstractFontConverter::importGlyphCacheFromSingleData() convenience
     * functions cannot be used.
     */
    MultiFile = 1 << 5
};

/**
@brief Features supported by a font converter
@m_since{2020,06}

@see @ref AbstractFontConverter::features()
*/
typedef Containers::EnumSet<FontConverterFeature> FontConverterFeatures;

CORRADE_ENUMSET_OPERATORS(FontConverterFeatures)

/** @debugoperatorenum{FontConverterFeature} */
MAGNUM_TEXT_EXPORT Debug& operator<<(Debug& debug, FontConverterFeature value);

/** @debugoperatorenum{FontConverterFeatures} */
MAGNUM_TEXT_EXPORT Debug& operator<<(Debug& debug, FontConverterFeatures value);

/**
@brief Base for font converter plugins

Provides functionality for converting arbitrary font to different format. See
@ref plugins for more information and `*FontConverter` classes in @ref Text
namespace for available font converter plugins.

You can use the @ref magnum-fontconverter "magnum-fontconverter" utility to do
font conversion on command-line.

@m_class{m-note m-success}

@par
    There's also a @ref magnum-fontconverter "magnum-fontconverter" tool,
    exposing functionality of all font converter plugins on a command line.

@section Text-AbstractFontConverter-subclassing Subclassing

Plugin implements @ref doFeatures() and one or more of `exportTo*()` /
`importFrom*()` functions based on what features are supported. Characters
passed to font exporting functions are converted to list of unique UTF-32
characters.

You don't need to do most of the redundant sanity checks, these things are
checked by the implementation:

-   Functions `doExportFontTo*()` are called only if
    @ref FontConverterFeature::ExportFont is supported, functions
    `doExportGlyphCacheTo*()` are called only if
    @ref FontConverterFeature::ExportGlyphCache is supported.
-   Functions `doImportGlyphCacheFrom*()` are called only if
    @ref FontConverterFeature::ImportGlyphCache is supported.
-   Functions `doExport*To*Data()` and `doImport*From*Data()` are called only
    if @ref FontConverterFeature::ConvertData is supported.
-   Function `doImport*FromData()` is called only if there is at least one data
    array passed.

@attention @ref Corrade::Containers::Array instances returned from the plugin
    should *not* use anything else than the default deleter, otherwise this can
    cause dangling function pointer call on array destruction if the plugin
    gets unloaded before the array is destroyed.
*/
class MAGNUM_TEXT_EXPORT AbstractFontConverter: public PluginManager::AbstractPlugin {
    public:
        #ifdef MAGNUM_BUILD_DEPRECATED
        /** @brief @copybrief FontConverterFeature
         * @m_deprecated_since{2020,06} Use @ref FontConverterFeature instead.
         */
        typedef CORRADE_DEPRECATED("use FontConverterFeature instead") FontConverterFeature Feature;

        /** @brief @copybrief FontConverterFeatures
         * @m_deprecated_since{2020,06} Use @ref FontConverterFeatures instead.
         */
        typedef CORRADE_DEPRECATED("use FontConverterFeatures instead") FontConverterFeatures Features;
        #endif

        /**
         * @brief Plugin interface
         *
         * @snippet Magnum/Text/AbstractFontConverter.cpp interface
         */
        static std::string pluginInterface();

        #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
        /**
         * @brief Plugin search paths
         *
         * Looks into `magnum/fontconverters/` or `magnum-d/fontconverters/`
         * next to the dynamic @ref Trade library, next to the executable and
         * elsewhere according to the rules documented in
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
        explicit AbstractFontConverter();

        /** @brief Plugin manager constructor */
        explicit AbstractFontConverter(PluginManager::AbstractManager& manager, const std::string& plugin);

        /** @brief Features supported by this converter */
        FontConverterFeatures features() const { return doFeatures(); }

        /**
         * @brief Export font to raw data
         * @param font          Opened font
         * @param cache         Populated glyph cache
         * @param filename      Output filename
         * @param characters    Characters to export
         *
         * Available only if @ref FontConverterFeature::ConvertData and
         * @ref FontConverterFeature::ExportFont is supported. Returns pairs of
         * filename and data on success, empty vector otherwise. All data will
         * be sharing common basename derived from @p filename. If the plugin
         * doesn't have @ref FontConverterFeature::MultiFile, only one pair is
         * returned, thus using @ref exportFontToSingleData() might be more
         * convenient in that case.
         * @see @ref features(), @ref exportFontToFile(),
         *      @ref exportGlyphCacheToData()
         */
        std::vector<std::pair<std::string, Containers::Array<char>>> exportFontToData(AbstractFont& font, AbstractGlyphCache& cache, const std::string& filename, const std::string& characters) const;

        /**
         * @brief Export font to single raw data
         *
         * Available only if @ref FontConverterFeature::ConvertData and
         * @ref FontConverterFeature::ExportFont is supported and the plugin
         * doesn't have @ref FontConverterFeature::MultiFile. Returns data on
         * success, zero-sized array otherwise. See @ref exportFontToData() for
         * more information.
         * @see @ref features(), @ref exportFontToFile(),
         *      @ref exportGlyphCacheToSingleData()
         */
        Containers::Array<char> exportFontToSingleData(AbstractFont& font, AbstractGlyphCache& cache, const std::string& characters) const;

        /**
         * @brief Export font to file
         *
         * Available only if @ref FontConverterFeature::ExportFont is
         * supported. If the plugin has @ref FontConverterFeature::MultiFile,
         * the function will create more than one file in given path, all
         * sharing common basename derived from @p filename. Returns
         * @cpp true @ce on success, @cpp false @ce otherwise. See
         * @ref exportFontToData() for more information.
         * @see @ref features(), @ref exportFontToData(),
         *      @ref exportGlyphCacheToFile()
         */
        bool exportFontToFile(AbstractFont& font, AbstractGlyphCache& cache, const std::string& filename, const std::string& characters) const;

        /**
         * @brief Export glyph cache to raw data
         * @param cache         Populated glyph cache
         * @param filename      Output filename
         *
         * Available only if @ref FontConverterFeature::ConvertData and
         * @ref FontConverterFeature::ExportGlyphCache is supported. Returns
         * pairs of filename and data on success, empty vector otherwise. All
         * data will be sharing common basename derived from @p filename. If
         * the plugin doesn't have @ref FontConverterFeature::MultiFile, only
         * one pair is returned, thus using @ref exportGlyphCacheToSingleData()
         * might be more convenient in that case.
         *
         * All glyphs from given cache will be exported. If you want to export
         * smaller subset, fill the cache with less characters.
         * @see @ref features(), @ref exportGlyphCacheToFile(),
         *      @ref exportFontToData()
         */
        std::vector<std::pair<std::string, Containers::Array<char>>> exportGlyphCacheToData(AbstractGlyphCache& cache, const std::string& filename) const;

        /**
         * @brief Export glyph cache to single raw data
         *
         * Available only if @ref FontConverterFeature::ConvertData and
         * @ref FontConverterFeature::ExportGlyphCache is supported and the
         * plugin doesn't have @ref FontConverterFeature::MultiFile. Returns
         * data on success, zero-sized array otherwise. See
         * @ref exportGlyphCacheToData() for more information.
         * @see @ref features(), @ref exportGlyphCacheToFile(),
         *      @ref importGlyphCacheFromSingleData()
         */
        Containers::Array<char> exportGlyphCacheToSingleData(AbstractGlyphCache& cache) const;

        /**
         * @brief Export glyph cache to file
         *
         * Available only if @ref FontConverterFeature::ExportGlyphCache is
         * supported. If the plugin has @ref FontConverterFeature::MultiFile,
         * the function will create more than one file in given path, all
         * sharing common basename derived from @p filename. Returns
         * @cpp true @ce on success, @cpp false @ce otherwise.
         * @see @ref features(), @ref exportGlyphCacheToData(),
         *      @ref exportFontToFile()
         */
        bool exportGlyphCacheToFile(AbstractGlyphCache& cache, const std::string& filename) const;

        /**
         * @brief Import glyph cache from raw data
         * @param data      Pairs of filename and file data
         *
         * Available only if @ref FontConverterFeature::ConvertData and
         * @ref FontConverterFeature::ImportGlyphCache is supported. Returns
         * imported cache on success, @cpp nullptr @ce otherwise. If the plugin
         * doesn't have @ref FontConverterFeature::MultiFile, only one file is
         * needed, thus using @ref importGlyphCacheFromSingleData() might be
         * more convenient in that case.
         * @see @ref features(), @ref importGlyphCacheFromFile(),
         *      @ref exportGlyphCacheToData()
         */
        Containers::Pointer<AbstractGlyphCache> importGlyphCacheFromData(const std::vector<std::pair<std::string, Containers::ArrayView<const char>>>& data) const;

        /**
         * @brief Import glyph cache from single raw data
         *
         * Available only if @ref FontConverterFeature::ConvertData and
         * @ref FontConverterFeature::ImportGlyphCache is supported and the
         * plugin doesn't have @ref FontConverterFeature::MultiFile. Returns
         * imported cache on success, @cpp nullptr @ce otherwise. See
         * @ref importGlyphCacheFromData() for multi-file conversion.
         * @see @ref features(), @ref importGlyphCacheFromFile(),
         *      @ref exportFontToSingleData()
         */
        Containers::Pointer<AbstractGlyphCache> importGlyphCacheFromSingleData(Containers::ArrayView<const char> data) const;

        /**
         * @brief Import glyph cache from file
         *
         * Available only if @ref FontConverterFeature::ImportGlyphCache is
         * supported. If the plugin has @ref FontConverterFeature::MultiFile,
         * the function will use additional files in given path, all sharing
         * common basename derived from @p filename. Returns imported cache on
         * success, @cpp nullptr @ce otherwise.
         * @see @ref features(), @ref importGlyphCacheFromData(),
         *      @ref exportGlyphCacheToFile()
         */
        Containers::Pointer<AbstractGlyphCache> importGlyphCacheFromFile(const std::string& filename) const;

    private:
        /** @brief Implementation for @ref features() */
        virtual FontConverterFeatures doFeatures() const = 0;

        /**
         * @brief Implementation for @ref exportFontToData()
         *
         * If the plugin doesn't have @ref FontConverterFeature::MultiFile,
         * default implementation calls @ref doExportFontToSingleData().
         */
        virtual std::vector<std::pair<std::string, Containers::Array<char>>> doExportFontToData(AbstractFont& font, AbstractGlyphCache& cache, const std::string& filename, const std::u32string& characters) const;

        /** @brief Implementation for @ref exportFontToSingleData() */
        virtual Containers::Array<char> doExportFontToSingleData(AbstractFont& font, AbstractGlyphCache& cache, const std::u32string& characters) const;

        /**
         * @brief Implementation for @ref exportFontToFile()
         *
         * If @ref FontConverterFeature::ConvertData is supported, default
         * implementation calls @ref doExportFontToData() and saves the result
         * to given file(s).
         */
        virtual bool doExportFontToFile(AbstractFont& font, AbstractGlyphCache& cache, const std::string& filename, const std::u32string& characters) const;

        /**
         * @brief Implementation for @ref exportGlyphCacheToData()
         *
         * If the plugin doesn't have @ref FontConverterFeature::MultiFile,
         * default implementation calls @ref doExportGlyphCacheToSingleData().
         */
        virtual std::vector<std::pair<std::string, Containers::Array<char>>> doExportGlyphCacheToData(AbstractGlyphCache& cache, const std::string& filename) const;

        /** @brief Implementation for @ref exportGlyphCacheToSingleData() */
        virtual Containers::Array<char> doExportGlyphCacheToSingleData(AbstractGlyphCache& cache) const;

        /**
         * @brief Implementation for @ref exportGlyphCacheToFile()
         *
         * If @ref FontConverterFeature::ConvertData is supported, default
         * implementation calls @ref doExportGlyphCacheToData() and saves the
         * result to given file(s).
         */
        virtual bool doExportGlyphCacheToFile(AbstractGlyphCache& cache, const std::string& filename) const;

        /**
         * @brief Implementation for @ref importGlyphCacheFromData()
         *
         * If the plugin doesn't have @ref FontConverterFeature::MultiFile,
         * default implementation calls @ref doImportGlyphCacheFromSingleData().
         */
        virtual Containers::Pointer<AbstractGlyphCache> doImportGlyphCacheFromData(const std::vector<std::pair<std::string, Containers::ArrayView<const char>>>& data) const;

        /** @brief Implementation for @ref importGlyphCacheFromSingleData() */
        virtual Containers::Pointer<AbstractGlyphCache> doImportGlyphCacheFromSingleData(Containers::ArrayView<const char> data) const;

        /**
         * @brief Implementation for @ref importGlyphCacheFromFile()
         *
         * If @ref FontConverterFeature::ConvertData is supported and the
         * plugin doesn't have @ref FontConverterFeature::MultiFile, default
         * implementation opens the file and calls
         * @ref doImportGlyphCacheFromSingleData() with its contents.
         */
        virtual Containers::Pointer<AbstractGlyphCache> doImportGlyphCacheFromFile(const std::string& filename) const;
};

}}

#endif
