#ifndef Magnum_Text_AbstractFontConverter_h
#define Magnum_Text_AbstractFontConverter_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class Magnum::Text::AbstractFontConverter
 */

#include <PluginManager/AbstractPlugin.h>

#include "Magnum.h"
#include "Text/Text.h"
#include "Text/magnumTextVisibility.h"

namespace Magnum { namespace Text {

/**
@brief Base for font converter plugins

Provides functionality for converting arbitrary font to different format.

@section AbstractFontConverter-subclassing Subclassing

Plugin implements doFeatures() and one or more of `exportTo*()` / `importFrom*()`
functions based on what features are supported. Characters passed to font
exporting functions are converted to list of unique UTF-32 characters.

You don't need to do most of the redundant sanity checks, these things are
checked by the implementation:

-   Functions `doExportFontTo*()` are called only if @ref Feature "Feature::ExportFont"
    is supported, functions `doExportGlyphCacheTo*()` are called only if
    @ref Feature "Feature::ExportGlyphCache" is supported.
-   Functions `doImportGlyphCacheFrom*()` are called only if
    @ref Feature "Feature::ImportGlyphCache" is supported.
-   Functions `doExport*To*Data()` and `doImport*From*Data()` are called only
    if @ref Feature "Feature::ConvertData" is supported.
-   Function `doImport*FromData()` is called only if there is at least one data
    array passed.
*/
class MAGNUM_TEXT_EXPORT AbstractFontConverter: public PluginManager::AbstractPlugin {
    CORRADE_PLUGIN_INTERFACE("cz.mosra.magnum.Text.AbstractFontConverter/0.1")

    public:
        /**
         * @brief Features supported by this converter
         *
         * @see Features, features()
         */
        enum class Feature: UnsignedByte {
            /**
             * Exporting font using exportToFile() or exportToData()
             * @see @ref Feature "Feature::ConvertData"
             */
            ExportFont = 1 << 0,

            /**
             * Export glyph cache using exportToFile() or exportToData()
             * @see @ref Feature "Feature::ConvertData"
             */
            ExportGlyphCache = 1 << 1,

            /**
             * Import glyph cache using importFromFile() or importFromData()
             * @see @ref Feature "Feature::ConvertData"
             */
            ImportGlyphCache = 1 << 2,

            /** Convert from/to data using exportToData() or importFromData() */
            ConvertData = 1 << 4,

            /**
             * The format is multi-file, thus exportToSingleData() and
             * importFromSingleData() convenience functions cannot be used.
             */
            MultiFile = 1 << 5
        };

        /**
         * @brief Features supported by this converter
         *
         * @see features()
         */
        typedef Containers::EnumSet<Feature, UnsignedByte> Features;

        /** @brief Default constructor */
        explicit AbstractFontConverter();

        /** @brief Plugin manager constructor */
        explicit AbstractFontConverter(PluginManager::AbstractManager* manager, std::string plugin);

        /** @brief Features supported by this converter */
        Features features() const { return doFeatures(); }

        /**
         * @brief Export font to raw data
         * @param font          Opened font
         * @param cache         Populated glyph cache
         * @param filename      Output filename
         * @param characters    Characters to export
         *
         * Available only if @ref Feature "Feature::ConvertData" and
         * @ref Feature "Feature::ExportFont" is supported. Returns pairs of
         * filename and data on success, empty vector otherwise. All data will
         * be sharing common basename derived from @p filename. If the plugin
         * doesn't have @ref Feature "Feature::MultiFile", only one pair is
         * returned, thus using exportFontToSingleData() might be more convenient
         * in that case.
         * @see features(), exportFontToFile(), exportGlyphCacheToData()
         */
        std::vector<std::pair<std::string, Containers::Array<unsigned char>>> exportFontToData(AbstractFont& font, GlyphCache& cache, const std::string& filename, const std::string& characters) const;

        /**
         * @brief Export font to single raw data
         *
         * Available only if @ref Feature "Feature::ConvertData" and
         * @ref Feature "Feature::ExportFont" is supported and the plugin
         * doesn't have @ref Feature "Feature::MultiFile". Returns data on
         * success, zero-sized array otherwise. See exportFontToData() for
         * more information.
         * @see features(), exportFontToFile(), importFromSingleData()
         */
        Containers::Array<unsigned char> exportFontToSingleData(AbstractFont& font, GlyphCache& cache, const std::string& characters) const;

        /**
         * @brief Export font to file
         *
         * Available only if @ref Feature "Feature::ExportFont" is supported.
         * If the plugin has @ref Feature "Feature::MultiFile", the function
         * will create more than one file in given path, all sharing common
         * basename derived from @p filename. Returns `true` on success,
         * `false` otherwise. See exportFontToData() for more information.
         * @see features(), exportFontToData(), exportGlyphCacheToFile()
         */
        bool exportFontToFile(AbstractFont& font, GlyphCache& cache, const std::string& filename, const std::string& characters) const;

        /**
         * @brief Export glyph cache to raw data
         * @param cache         Populated glyph cache
         * @param filename      Output filename
         *
         * Available only if @ref Feature "Feature::ConvertData" and
         * @ref Feature "Feature::ExportGlyphCache" is supported. Returns pairs
         * of filename and data on success, empty vector otherwise. All data
         * will be sharing common basename derived from @p filename. If the
         * plugin doesn't have @ref Feature "Feature::MultiFile", only one pair
         * is returned, thus using exportGlyphCacheToSingleData() might be more
         * convenient in that case.
         *
         * All glyphs from given cache will be exported. If you want to export
         * smaller subset, fill the cache with less characters.
         * @see features(), exportGlyphCacheToFile(), exportFontToData()
         */
        std::vector<std::pair<std::string, Containers::Array<unsigned char>>> exportGlyphCacheToData(GlyphCache& cache, const std::string& filename) const;

        /**
         * @brief Export glyph cache to single raw data
         *
         * Available only if @ref Feature "Feature::ConvertData" and
         * @ref Feature "Feature::ExportGlyphCache" is supported and the plugin
         * doesn't have @ref Feature "Feature::MultiFile". Returns data on
         * success, zero-sized array otherwise. See exportGlyphCacheToData()
         * for more information.
         * @see features(), exportGlyphCacheToFile(), importGlyphCacheFromSingleData()
         */
        Containers::Array<unsigned char> exportGlyphCacheToSingleData(GlyphCache& cache) const;

        /**
         * @brief Export glyph cache to file
         *
         * Available only if @ref Feature "Feature::ExportGlyphCache" is
         * supported. If the plugin has @ref Feature "Feature::MultiFile", the
         * function will create more than one file in given path, all sharing
         * common basename derived from @p filename. Returns `true` on success,
         * `false` otherwise.
         * @see features(), exportGlyphCacheToData(), exportFontToFile()
         */
        bool exportGlyphCacheToFile(GlyphCache& cache, const std::string& filename) const;

        /**
         * @brief Import glyph cache from raw data
         * @param data      Pairs of filename and file data
         *
         * Available only if @ref Feature "Feature::ConvertData" and
         * @ref Feature "Feature::ImportGlyphCache" is supported. Returns
         * imported cache on success, `nullptr` otherwise. If the plugin
         * doesn't have @ref Feature "Feature::MultiFile", only one file is
         * needed, thus using convertToSingleData() might be more convenient in
         * that case.
         * @see features(), importFromFile(), exportToData()
         */
        GlyphCache* importGlyphCacheFromData(const std::vector<std::pair<std::string, Containers::ArrayReference<const unsigned char>>>& data) const;

        /**
         * @brief Import glyph cache from single raw data
         *
         * Available only if @ref Feature "Feature::ConvertData" and
         * @ref Feature "Feature::ImportGlyphCache" is supported and the plugin
         * doesn't have @ref Feature "Feature::MultiFile". Returns imported
         * cache on success, `nullptr` otherwise. See importFromData() for
         * multi-file conversion.
         * @see features(), importFromFile(), exportToSingleData()
         */
        GlyphCache* importGlyphCacheFromSingleData(Containers::ArrayReference<const unsigned char> data) const;

        /**
         * @brief Import glyph cache from file
         *
         * Available only if @ref Feature "Feature::ImportGlyphCache" is
         * supported. If the plugin has @ref Feature "Feature::MultiFile", the
         * function will use additional files in given path, all sharing common
         * basename derived from @p filename. Returns imported cache on
         * success, `nullptr` otherwise.
         * @see features(), importFromData(), exportToFile()
         */
        GlyphCache* importGlyphCacheFromFile(const std::string& filename) const;

    #ifndef DOXYGEN_GENERATING_OUTPUT
    private:
    #else
    protected:
    #endif
        /** @brief Implementation for features() */
        virtual Features doFeatures() const = 0;

        /**
         * @brief Implementation for exportFontToData()
         *
         * If the plugin doesn't have @ref Feature "Feature::MultiFile",
         * default implementation calls doExportFontToSingleData().
         */
        virtual std::vector<std::pair<std::string, Containers::Array<unsigned char>>> doExportFontToData(AbstractFont& font, GlyphCache& cache, const std::string& filename, const std::u32string& characters) const;

        /** @brief Implementation for exportFontToSingleData() */
        virtual Containers::Array<unsigned char> doExportFontToSingleData(AbstractFont& font, GlyphCache& cache, const std::u32string& characters) const;

        /**
         * @brief Implementation for exportFontToFile()
         *
         * If @ref Feature "Feature::ConvertData" is supported, default
         * implementation calls doExportFontToData() and saves the result to
         * given file(s).
         */
        virtual bool doExportFontToFile(AbstractFont& font, GlyphCache& cache, const std::string& filename, const std::u32string& characters) const;

        /**
         * @brief Implementation for exportGlyphCacheToData()
         *
         * If the plugin doesn't have @ref Feature "Feature::MultiFile",
         * default implementation calls doExportGlyphCacheToSingleData().
         */
        virtual std::vector<std::pair<std::string, Containers::Array<unsigned char>>> doExportGlyphCacheToData(GlyphCache& cache, const std::string& filename) const;

        /** @brief Implementation for exportGlyphCacheToSingleData() */
        virtual Containers::Array<unsigned char> doExportGlyphCacheToSingleData(GlyphCache& cache) const;

        /**
         * @brief Implementation for exportGlyphCacheToFile()
         *
         * If @ref Feature "Feature::ConvertData" is supported, default
         * implementation calls doExportGlyphCacheToData() and saves the result
         * to given file(s).
         */
        virtual bool doExportGlyphCacheToFile(GlyphCache& cache, const std::string& filename) const;

        /**
         * @brief Implementation for importGlyphCacheFromData()
         *
         * If the plugin doesn't have @ref Feature "Feature::MultiFile",
         * default implementation calls doImportGlyphCacheFromSingleData().
         */
        virtual GlyphCache* doImportGlyphCacheFromData(const std::vector<std::pair<std::string, Containers::ArrayReference<const unsigned char>>>& data) const;

        /** @brief Implementation for importGlyphCacheFromSingleData() */
        virtual GlyphCache* doImportGlyphCacheFromSingleData(Containers::ArrayReference<const unsigned char> data) const;

        /**
         * @brief Implementation for importGlyphCacheFromFile()
         *
         * If @ref Feature "Feature::ConvertData" is supported and the plugin
         * doesn't have @ref Feature "Feature::MultiFile", default
         * implementation opens the file and calls doImportGlyphCacheFromSingleData()
         * with its contents.
         */
        virtual GlyphCache* doImportGlyphCacheFromFile(const std::string& filename) const;

    private:
        MAGNUM_TEXT_LOCAL static std::u32string uniqueUnicode(const std::string& characters);
};

CORRADE_ENUMSET_OPERATORS(AbstractFontConverter::Features)

}}

#endif
