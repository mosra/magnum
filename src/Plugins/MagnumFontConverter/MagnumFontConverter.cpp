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

#include "MagnumFontConverter.h"

#include <sstream>
#include <Containers/Array.h>
#include <Utility/Directory.h>

#include "ColorFormat.h"
#include "Image.h"
#include "Text/GlyphCache.h"
#include "Text/AbstractFont.h"
#include "TgaImageConverter/TgaImageConverter.h"

namespace Magnum { namespace Text {

MagnumFontConverter::MagnumFontConverter() = default;

MagnumFontConverter::MagnumFontConverter(PluginManager::AbstractManager* manager, std::string plugin): AbstractFontConverter(manager, std::move(plugin)) {}

auto MagnumFontConverter::doFeatures() const -> Features {
    return Feature::ExportFont|Feature::ConvertData|Feature::MultiFile;
}

#ifndef __MINGW32__
std::vector<std::pair<std::string, Containers::Array<unsigned char>>> MagnumFontConverter::doExportFontToData(AbstractFont& font, GlyphCache& cache, const std::string& filename, const std::u32string& characters) const
#else
std::vector<std::pair<std::string, Containers::Array<unsigned char>>> MagnumFontConverter::doExportFontToData(AbstractFont& font, GlyphCache& cache, const std::string& filename, const std::vector<char32_t>& characters) const
#endif
{
    Utility::Configuration configuration;

    configuration.setValue("version", 1);
    configuration.setValue("image", Utility::Directory::filename(filename) + ".tga");
    configuration.setValue("originalImageSize", cache.textureSize());
    configuration.setValue("padding", cache.padding());
    configuration.setValue("fontSize", font.size());
    configuration.setValue("lineHeight", font.lineHeight());

    /* Compress glyph IDs so the glyphs are in consecutive array, glyph 0
       should stay at position 0 */
    std::unordered_map<UnsignedInt, UnsignedInt> glyphIdMap;
    #ifndef CORRADE_GCC44_COMPATIBILITY
    glyphIdMap.reserve(cache.glyphCount());
    #endif
    #ifndef CORRADE_GCC46_COMPATIBILITY
    glyphIdMap.emplace(0, 0);
    #else
    glyphIdMap.insert({0, 0});
    #endif
    for(auto it = cache.begin(); it != cache.end(); ++it)
        #ifndef CORRADE_GCC46_COMPATIBILITY
        glyphIdMap.emplace(it->first, glyphIdMap.size());
        #else
        glyphIdMap.insert({it->first, glyphIdMap.size()});
        #endif

    /** @todo Save only glyphs contained in @p characters */

    /* Inverse map from new glyph IDs to old ones */
    std::vector<UnsignedInt> inverseGlyphIdMap(glyphIdMap.size());
    for(auto it = glyphIdMap.begin(); it != glyphIdMap.end(); ++it)
        inverseGlyphIdMap[it->second] = it->first;

    /* Character->glyph map, map glyph IDs to new ones */
    for(auto it = characters.begin(); it != characters.end(); ++it) {
        const char32_t c = *it;

        Utility::ConfigurationGroup* group = configuration.addGroup("char");
        const UnsignedInt glyphId = font.glyphId(c);
        group->setValue("unicode", c);

        /* Map old glyph ID to new, if not found, map to glyph 0 */
        auto found = glyphIdMap.find(glyphId);
        group->setValue("glyph", found == glyphIdMap.end() ? 0 : glyphIdMap.at(glyphId));
    }

    /* Save glyph properties in order which preserves their IDs, remove padding
       from the values so they aren't added twice when using the font later */
    /** @todo Some better way to handle this padding stuff */
    for(auto it = inverseGlyphIdMap.begin(); it != inverseGlyphIdMap.end(); ++it) {
        const UnsignedInt oldGlyphId = *it;

        std::pair<Vector2i, Range2Di> glyph = cache[oldGlyphId];
        Utility::ConfigurationGroup* group = configuration.addGroup("glyph");
        group->setValue("advance", font.glyphAdvance(oldGlyphId));
        group->setValue("position", glyph.first+cache.padding());
        group->setValue("rectangle", glyph.second.padded(-cache.padding()));
    }

    std::ostringstream confOut;
    configuration.save(confOut);
    std::string confStr = confOut.str();
    Containers::Array<unsigned char> confData{confStr.size()};
    std::copy(confStr.begin(), confStr.end(), confData.begin());

    /* Save cache image */
    Image2D image(ColorFormat::Red, ColorType::UnsignedByte);
    cache.texture().image(0, image);
    auto tgaData = Trade::TgaImageConverter().exportToData(image);

    std::vector<std::pair<std::string, Containers::Array<unsigned char>>> out;
    out.emplace_back(filename + ".conf", std::move(confData));
    out.emplace_back(filename + ".tga", std::move(tgaData));
    return std::move(out);
}

}}
