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

#include "MagnumFontConverter.h"

#include <algorithm>
#include <sstream>
#include <Corrade/Containers/Array.h>
#include <Corrade/Utility/Configuration.h>
#include <Corrade/Utility/Directory.h>

#include "Magnum/Image.h"
#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/Math/ConfigurationValue.h"
#include "Magnum/Text/AbstractFont.h"
#include "Magnum/Text/AbstractGlyphCache.h"
#include "MagnumPlugins/TgaImageConverter/TgaImageConverter.h"

namespace Magnum { namespace Text {

MagnumFontConverter::MagnumFontConverter() = default;

MagnumFontConverter::MagnumFontConverter(PluginManager::AbstractManager& manager, const std::string& plugin): AbstractFontConverter{manager, plugin} {}

FontConverterFeatures MagnumFontConverter::doFeatures() const {
    return FontConverterFeature::ExportFont|FontConverterFeature::ConvertData|FontConverterFeature::MultiFile;
}

std::vector<std::pair<std::string, Containers::Array<char>>> MagnumFontConverter::doExportFontToData(AbstractFont& font, AbstractGlyphCache& cache, const std::string& filename, const std::u32string& characters) const {
    if(!(cache.features() & GlyphCacheFeature::ImageDownload)) {
        Error{} << "Text::MagnumFontConverter::exportFontToData(): passed glyph cache doesn't support image download";
        return {};
    }

    Utility::Configuration configuration;

    configuration.setValue("version", 1);
    configuration.setValue("image", Utility::Directory::filename(filename) + ".tga");
    configuration.setValue("originalImageSize", cache.textureSize());
    configuration.setValue("padding", cache.padding());
    configuration.setValue("fontSize", font.size());
    configuration.setValue("ascent", font.ascent());
    configuration.setValue("descent", font.descent());
    configuration.setValue("lineHeight", font.lineHeight());

    /* Get the glyphs and sort them for predictable output */
    std::vector<std::pair<UnsignedInt, std::pair<Vector2i, Range2Di>>> sortedGlyphs;
    for(const std::pair<const UnsignedInt, std::pair<Vector2i, Range2Di>>& glyph: cache)
        sortedGlyphs.emplace_back(glyph);
    std::sort(sortedGlyphs.begin(), sortedGlyphs.end(),
        [](const std::pair<UnsignedInt, std::pair<Vector2i, Range2Di>>& a,
           const std::pair<UnsignedInt, std::pair<Vector2i, Range2Di>>& b) {
            return a.first < b.first;
        });

    /* Compress glyph IDs so the glyphs are in a consecutive array, glyph 0
       should stay at position 0 */
    std::unordered_map<UnsignedInt, UnsignedInt> glyphIdMap;
    glyphIdMap.reserve(cache.glyphCount());
    glyphIdMap.emplace(0, 0);
    for(const std::pair<UnsignedInt, std::pair<Vector2i, Range2Di>>& glyph: sortedGlyphs)
        glyphIdMap.emplace(glyph.first, glyphIdMap.size());

    /** @todo Save only glyphs contained in @p characters */

    /* Inverse map from new glyph IDs to old ones */
    std::vector<UnsignedInt> inverseGlyphIdMap(glyphIdMap.size());
    for(const std::pair<const UnsignedInt, UnsignedInt>& map: glyphIdMap)
        inverseGlyphIdMap[map.second] = map.first;

    /* Character->glyph map, map glyph IDs to new ones */
    for(const char32_t c: characters) {
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
    for(UnsignedInt oldGlyphId: inverseGlyphIdMap) {
        std::pair<Vector2i, Range2Di> glyph = cache[oldGlyphId];
        Utility::ConfigurationGroup* group = configuration.addGroup("glyph");
        group->setValue("advance", font.glyphAdvance(oldGlyphId));
        group->setValue("position", glyph.first+cache.padding());
        group->setValue("rectangle", glyph.second.padded(-cache.padding()));
    }

    std::ostringstream confOut;
    configuration.save(confOut);
    std::string confStr = confOut.str();
    Containers::Array<char> confData{confStr.size()};
    std::copy(confStr.begin(), confStr.end(), confData.begin());

    /* Save cache image */
    auto tgaData = Trade::TgaImageConverter().exportToData(cache.image());

    std::vector<std::pair<std::string, Containers::Array<char>>> out;
    out.emplace_back(filename + ".conf", std::move(confData));
    out.emplace_back(filename + ".tga", std::move(tgaData));
    return out;
}

}}

CORRADE_PLUGIN_REGISTER(MagnumFontConverter, Magnum::Text::MagnumFontConverter,
    "cz.mosra.magnum.Text.AbstractFontConverter/0.2")
