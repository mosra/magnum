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

#include "AbstractFontConverter.h"

#include <algorithm> /* std::sort(), std::unique() */
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/EnumSet.hpp>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/String.h>
#include <Corrade/Containers/StringStl.h> /** @todo remove once AbstractFontConverter is <string>-free */
#include <Corrade/PluginManager/Manager.hpp>
#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/DebugStl.h> /** @todo remove once AbstractFontConverter is <string>-free */
#include <Corrade/Utility/Path.h>
#include <Corrade/Utility/Unicode.h>

#include "Magnum/Text/AbstractGlyphCache.h"

#ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
#include "Magnum/Text/configure.h"
#endif

namespace Corrade { namespace PluginManager {

template class MAGNUM_TEXT_EXPORT Manager<Magnum::Text::AbstractFontConverter>;

}}

namespace Magnum { namespace Text {

using namespace Containers::Literals;

namespace {

std::u32string uniqueUnicode(const std::string& characters)
{
    /* Convert UTF-8 to UTF-32 */
    Containers::Optional<Containers::Array<char32_t>> utf32 = Utility::Unicode::utf32(characters);
    /** @todo result validity check */
    std::u32string result{utf32->begin(), utf32->end()};

    /* Remove duplicate glyphs */
    std::sort(result.begin(), result.end());
    result.erase(std::unique(result.begin(), result.end()), result.end());

    return result;
}

}

Containers::StringView AbstractFontConverter::pluginInterface() {
    return MAGNUM_TEXT_ABSTRACTFONTCONVERTER_PLUGIN_INTERFACE ""_s;
}

#ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
Containers::Array<Containers::String> AbstractFontConverter::pluginSearchPaths() {
    const Containers::Optional<Containers::String> libraryLocation = Utility::Path::libraryLocation(&pluginInterface);
    return PluginManager::implicitPluginSearchPaths(
        #ifndef MAGNUM_BUILD_STATIC
        libraryLocation ? *libraryLocation : Containers::String{},
        #else
        {},
        #endif
        #ifdef CORRADE_IS_DEBUG_BUILD
        MAGNUM_PLUGINS_FONTCONVERTER_DEBUG_DIR,
        #else
        MAGNUM_PLUGINS_FONTCONVERTER_DIR,
        #endif
        #ifdef CORRADE_IS_DEBUG_BUILD
        "magnum-d/"
        #else
        "magnum/"
        #endif
        "fontconverters");
}
#endif

AbstractFontConverter::AbstractFontConverter() = default;

AbstractFontConverter::AbstractFontConverter(PluginManager::AbstractManager& manager, const Containers::StringView& plugin): PluginManager::AbstractPlugin{manager, plugin} {}

std::vector<std::pair<std::string, Containers::Array<char>>> AbstractFontConverter::exportFontToData(AbstractFont& font, AbstractGlyphCache& cache, const std::string& filename, const std::string& characters) const {
    CORRADE_ASSERT(features() >= (FontConverterFeature::ExportFont|FontConverterFeature::ConvertData),
        "Text::AbstractFontConverter::exportFontToData(): feature not supported", {});

    std::vector<std::pair<std::string, Containers::Array<char>>> out = doExportFontToData(font, cache, filename, uniqueUnicode(characters));
    #ifndef CORRADE_NO_ASSERT
    for(const auto& i: out)
        CORRADE_ASSERT(!i.second.deleter(),
            "Text::AbstractFontConverter::exportFontToData(): implementation is not allowed to use a custom Array deleter", {});
    #endif

    return out;
}

std::vector<std::pair<std::string, Containers::Array<char>>> AbstractFontConverter::doExportFontToData(AbstractFont& font, AbstractGlyphCache& cache, const std::string& filename, const std::u32string& characters) const {
    CORRADE_ASSERT(!(features() & FontConverterFeature::MultiFile),
        "Text::AbstractFontConverter::exportFontToData(): feature advertised but not implemented", {});

    std::vector<std::pair<std::string, Containers::Array<char>>> out;
    Containers::Array<char> result = doExportFontToSingleData(font, cache, characters);
    if(result) out.emplace_back(filename, Utility::move(result));
    return out;
}

Containers::Array<char> AbstractFontConverter::exportFontToSingleData(AbstractFont& font, AbstractGlyphCache& cache, const std::string& characters) const {
    CORRADE_ASSERT(features() >= (FontConverterFeature::ExportFont|FontConverterFeature::ConvertData),
        "Text::AbstractFontConverter::exportFontToSingleData(): feature not supported", nullptr);
    CORRADE_ASSERT(!(features() & FontConverterFeature::MultiFile),
        "Text::AbstractFontConverter::exportFontToSingleData(): the format is not single-file", nullptr);

    Containers::Array<char> out = doExportFontToSingleData(font, cache, uniqueUnicode(characters));
    CORRADE_ASSERT(!out.deleter(),
        "Text::AbstractFontConverter::exportFontToSingleData(): implementation is not allowed to use a custom Array deleter", {});
    return out;
}

Containers::Array<char> AbstractFontConverter::doExportFontToSingleData(AbstractFont&, AbstractGlyphCache&, const std::u32string&) const {
    CORRADE_ASSERT_UNREACHABLE("Text::AbstractFontConverter::exportFontToSingleData(): feature advertised but not implemented", {});
}

bool AbstractFontConverter::exportFontToFile(AbstractFont& font, AbstractGlyphCache& cache, const std::string& filename, const std::string& characters) const {
    CORRADE_ASSERT(features() & FontConverterFeature::ExportFont,
        "Text::AbstractFontConverter::exportFontToFile(): feature not supported", false);

    return doExportFontToFile(font, cache, filename, uniqueUnicode(characters));
}

bool AbstractFontConverter::doExportFontToFile(AbstractFont& font, AbstractGlyphCache& cache, const std::string& filename, const std::u32string& characters) const {
    CORRADE_ASSERT(features() & FontConverterFeature::ConvertData,
        "Text::AbstractFontConverter::exportFontToFile(): feature advertised but not implemented", {});

    /* Export all data */
    const auto data = doExportFontToData(font, cache, filename, characters);
    if(data.empty()) return false;

    for(const auto& d: data) if(!Utility::Path::write(d.first, d.second)) {
        Error() << "Text::AbstractFontConverter::exportFontToFile(): cannot write to file" << d.first;
        return false;
    }

    return true;
}

std::vector<std::pair<std::string, Containers::Array<char>>> AbstractFontConverter::exportGlyphCacheToData(AbstractGlyphCache& cache, const std::string& filename) const {
    CORRADE_ASSERT(features() >= (FontConverterFeature::ExportGlyphCache|FontConverterFeature::ConvertData),
        "Text::AbstractFontConverter::exportGlyphCacheToData(): feature not supported", {});

    std::vector<std::pair<std::string, Containers::Array<char>>> out = doExportGlyphCacheToData(cache, filename);
    #ifndef CORRADE_NO_ASSERT
    for(const auto& i: out)
        CORRADE_ASSERT(!i.second.deleter(),
            "Text::AbstractFontConverter::exportGlyphCacheToData(): implementation is not allowed to use a custom Array deleter", {});
    #endif

    return out;
}

std::vector<std::pair<std::string, Containers::Array<char>>> AbstractFontConverter::doExportGlyphCacheToData(AbstractGlyphCache& cache, const std::string& filename) const {
    CORRADE_ASSERT(!(features() & FontConverterFeature::MultiFile),
        "Text::AbstractFontConverter::exportGlyphCacheToData(): feature advertised but not implemented", {});

    std::vector<std::pair<std::string, Containers::Array<char>>> out;
    Containers::Array<char> result = doExportGlyphCacheToSingleData(cache);
    if(result) out.emplace_back(filename, Utility::move(result));
    return out;
}

Containers::Array<char> AbstractFontConverter::exportGlyphCacheToSingleData(AbstractGlyphCache& cache) const {
    CORRADE_ASSERT(features() >= (FontConverterFeature::ExportGlyphCache|FontConverterFeature::ConvertData),
        "Text::AbstractFontConverter::exportGlyphCacheToSingleData(): feature not supported", nullptr);
    CORRADE_ASSERT(!(features() & FontConverterFeature::MultiFile),
        "Text::AbstractFontConverter::exportGlyphCacheToSingleData(): the format is not single-file", nullptr);

    Containers::Array<char> out = doExportGlyphCacheToSingleData(cache);
    CORRADE_ASSERT(!out.deleter(),
        "Text::AbstractFontConverter::exportGlyphCacheToSingleData(): implementation is not allowed to use a custom Array deleter", {});
    return out;
}

Containers::Array<char> AbstractFontConverter::doExportGlyphCacheToSingleData(AbstractGlyphCache&) const {
    CORRADE_ASSERT_UNREACHABLE("Text::AbstractFontConverter::exportGlyphCacheToSingleData(): feature advertised but not implemented", {});
}

bool AbstractFontConverter::exportGlyphCacheToFile(AbstractGlyphCache& cache, const std::string& filename) const {
    CORRADE_ASSERT(features() & FontConverterFeature::ExportGlyphCache,
        "Text::AbstractFontConverter::exportGlyphCacheToFile(): feature not supported", false);

    return doExportGlyphCacheToFile(cache, filename);
}

bool AbstractFontConverter::doExportGlyphCacheToFile(AbstractGlyphCache& cache, const std::string& filename) const {
    CORRADE_ASSERT(features() & FontConverterFeature::ConvertData,
        "Text::AbstractFontConverter::exportGlyphCacheToFile(): feature advertised but not implemented", {});

    /* Export all data */
    const auto data = doExportGlyphCacheToData(cache, filename);
    if(data.empty()) return false;

    for(const auto& d: data) if(!Utility::Path::write(d.first, d.second)) {
        Error() << "Text::AbstractFontConverter::exportGlyphCacheToFile(): cannot write to file" << d.first;
        return false;
    }

    return true;
}

Containers::Pointer<AbstractGlyphCache> AbstractFontConverter::importGlyphCacheFromData(const std::vector<std::pair<std::string, Containers::ArrayView<const char>>>& data) const {
    CORRADE_ASSERT(features() >= (FontConverterFeature::ImportGlyphCache|FontConverterFeature::ConvertData),
        "Text::AbstractFontConverter::importGlyphCacheFromData(): feature not supported", nullptr);
    CORRADE_ASSERT(!data.empty(),
        "Text::AbstractFontConverter::importGlyphCacheFromData(): no data passed", nullptr);

    return doImportGlyphCacheFromData(data);
}

Containers::Pointer<AbstractGlyphCache> AbstractFontConverter::doImportGlyphCacheFromData(const std::vector<std::pair<std::string, Containers::ArrayView<const char>>>& data) const {
    CORRADE_ASSERT(!(features() & FontConverterFeature::MultiFile),
        "Text::AbstractFontConverter::importGlyphCacheFromData(): feature advertised but not implemented", nullptr);
    CORRADE_ASSERT(data.size() == 1,
        "Text::AbstractFontConverter::importGlyphCacheFromData(): expected just one file for single-file format", nullptr);

    return doImportGlyphCacheFromSingleData(data[0].second);
}

Containers::Pointer<AbstractGlyphCache> AbstractFontConverter::importGlyphCacheFromSingleData(Containers::ArrayView<const char> data) const {
    CORRADE_ASSERT(features() >= (FontConverterFeature::ImportGlyphCache|FontConverterFeature::ConvertData),
        "Text::AbstractFontConverter::importGlyphCacheFromSingleData(): feature not supported", nullptr);
    CORRADE_ASSERT(!(features() & FontConverterFeature::MultiFile),
        "Text::AbstractFontConverter::importGlyphCacheFromSingleData(): the format is not single-file", nullptr);

    return doImportGlyphCacheFromSingleData(data);
}

Containers::Pointer<AbstractGlyphCache> AbstractFontConverter::doImportGlyphCacheFromSingleData(Containers::ArrayView<const char>) const {
    CORRADE_ASSERT_UNREACHABLE("Text::AbstractFontConverter::importGlyphCacheFromSingleData(): feature advertised but not implemented", {});
}

Containers::Pointer<AbstractGlyphCache> AbstractFontConverter::importGlyphCacheFromFile(const std::string& filename) const {
    CORRADE_ASSERT(features() & FontConverterFeature::ImportGlyphCache,
        "Text::AbstractFontConverter::importGlyphCacheFromFile(): feature not supported", nullptr);

    return doImportGlyphCacheFromFile(filename);
}

Containers::Pointer<AbstractGlyphCache> AbstractFontConverter::doImportGlyphCacheFromFile(const std::string& filename) const {
    CORRADE_ASSERT(features() & FontConverterFeature::ConvertData && !(features() & FontConverterFeature::MultiFile),
        "Text::AbstractFontConverter::importGlyphCacheFromFile(): feature advertised but not implemented", {});

    /* Open file */
    const Containers::Optional<Containers::Array<char>> data = Utility::Path::read(filename);
    if(!data) {
        Error() << "Text::AbstractFontConverter::importGlyphCacheFromFile(): cannot open file" << filename;
        return nullptr;
    }

    return doImportGlyphCacheFromSingleData(*data);
}

Debug& operator<<(Debug& debug, const FontConverterFeature value) {
    const bool packed = debug.immediateFlags() >= Debug::Flag::Packed;

    if(!packed)
        debug << "Text::FontConverterFeature" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(v) case FontConverterFeature::v: return debug << (packed ? "" : "::") << Debug::nospace << #v;
        _c(ExportFont)
        _c(ExportGlyphCache)
        _c(ImportGlyphCache)
        _c(ConvertData)
        _c(MultiFile)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << (packed ? "" : "(") << Debug::nospace << Debug::hex << Containers::enumCastUnderlyingType(value) << Debug::nospace << (packed ? "" : ")");
}

Debug& operator<<(Debug& debug, const FontConverterFeatures value) {
    return Containers::enumSetDebugOutput(debug, value, debug.immediateFlags() >= Debug::Flag::Packed ? "{}" : "Text::FontConverterFeatures{}", {
        FontConverterFeature::ExportFont,
        FontConverterFeature::ExportGlyphCache,
        FontConverterFeature::ImportGlyphCache,
        FontConverterFeature::ConvertData,
        FontConverterFeature::MultiFile});
}

}}
