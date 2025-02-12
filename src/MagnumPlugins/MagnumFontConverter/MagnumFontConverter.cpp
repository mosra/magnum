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

#include "MagnumFontConverter.h"

#include <algorithm> /* std::sort() */
#include <sstream>
#include <unordered_map>
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Containers/Triple.h>
#include <Corrade/Utility/Configuration.h>
#include <Corrade/Utility/Path.h>

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
    if(cache.size().z() != 1) {
        Error{} << "Text::MagnumFontConverter::exportFontToData(): exporting array glyph caches is not supported";
        return {};
    }
    if(cache.features() & GlyphCacheFeature::ImageProcessing && !(cache.features() >= GlyphCacheFeature::ProcessedImageDownload)) {
        Error{} << "Text::MagnumFontConverter::exportFontToData(): glyph cache has image processing but doesn't support image download";
        return {};
    }

    Containers::Optional<UnsignedInt> fontId = cache.findFont(font);
    #ifdef MAGNUM_BUILD_DEPRECATED
    /* Make it work with the old-style glyph cache filling that adds exactly
       one font into the cache and doesn't associate any pointer with it */
    if(!fontId && cache.fontCount() == 1 && cache.fontPointer(0) == nullptr)
        fontId = 0;
    #endif
    if(!fontId) {
        Error{} << "Text::MagnumFontConverter::exportFontToData(): font not found among" << cache.fontCount() << "fonts in passed glyph cache";
        return {};
    }

    Utility::Configuration configuration;

    configuration.setValue("version", 1);
    configuration.setValue("image", Utility::Path::filename(filename) + ".tga");
    configuration.setValue("originalImageSize", cache.size().xy());
    configuration.setValue("padding", cache.padding());
    configuration.setValue("fontSize", font.size());
    configuration.setValue("ascent", font.ascent());
    configuration.setValue("descent", font.descent());
    configuration.setValue("lineHeight", font.lineHeight());

    /* Get the glyphs and sort them for predictable output */
    std::vector<std::pair<UnsignedInt, std::pair<Vector2i, Range2Di>>> sortedGlyphs;
    const Containers::StridedArrayView1D<const Vector2i> offsets = cache.glyphOffsets();
    const Containers::StridedArrayView1D<const Range2Di> rectangles = cache.glyphRectangles();
    for(UnsignedInt fontGlyphId = 0; fontGlyphId != cache.fontGlyphCount(*fontId); ++fontGlyphId)
        if(const UnsignedInt glyphId = cache.glyphId(*fontId, fontGlyphId))
            sortedGlyphs.emplace_back(fontGlyphId, std::make_pair(offsets[glyphId], rectangles[glyphId]));
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
        /** @todo this branch is messy, clean up; also there's now a
            distinction between a cache-global invalid glyph and font-local,
            what to do there? */
        Containers::Triple<Vector2i, Int, Range2Di> glyph =
            oldGlyphId ? cache.glyph(*fontId, oldGlyphId) : cache.glyph(0);
        Utility::ConfigurationGroup* group = configuration.addGroup("glyph");
        group->setValue("advance", font.glyphAdvance(oldGlyphId));
        group->setValue("position", glyph.first() + cache.padding());
        group->setValue("rectangle", glyph.third().padded(-cache.padding()));
    }

    std::ostringstream confOut;
    configuration.save(confOut);
    std::string confStr = confOut.str();
    Containers::Array<char> confData{confStr.size()};
    std::copy(confStr.begin(), confStr.end(), confData.begin());

    /* Save cache image. Either the source image or the processed one if the
       cache has image processing. */
    Containers::Optional<Containers::Array<char>> tgaData;
    if(cache.features() & GlyphCacheFeature::ImageProcessing) {
        const Image3D image3 = cache.processedImage();
        tgaData = Trade::TgaImageConverter().convertToData(ImageView2D{image3.format(), image3.size().xy(), image3.data()});
    } else {
        const ImageView3D image3 = cache.image();
        tgaData = Trade::TgaImageConverter().convertToData(ImageView2D{image3.format(), image3.size().xy(), image3.data()});
    }
    if(!tgaData) {
        Error{} << "Text::MagnumFontConverter::exportFontToData(): cannot create a TGA image";
        return {};
    }

    std::vector<std::pair<std::string, Containers::Array<char>>> out;
    out.emplace_back(filename + ".conf", Utility::move(confData));
    out.emplace_back(filename + ".tga", *Utility::move(tgaData));
    return out;
}

}}

CORRADE_PLUGIN_REGISTER(MagnumFontConverter, Magnum::Text::MagnumFontConverter,
    MAGNUM_TEXT_ABSTRACTFONTCONVERTER_PLUGIN_INTERFACE)
